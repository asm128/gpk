#if defined(__linux__) && !defined(_GNU_SOURCE)
#	define _GNU_SOURCE // pipe2 and F_DUPFD_CLOEXEC.
#endif
#include "llc_process_internal.h"

#ifdef LLC_LINUX
#include <errno.h>    // EINTR, EPIPE and errno.
#include <fcntl.h>    // Atomic close-on-exec pipes and descriptor relocation.
#include <pthread.h>  // Input worker, cancellation and per-thread signal mask.
#include <signal.h>   // Preserve and consume SIGPIPE without changing global policy.
#include <spawn.h>    // Native argv-based process creation and file actions.
#include <sys/wait.h> // Child status and reaping.
#include <unistd.h>   // Pipe I/O and standard descriptors.

namespace llc::process_detail {
sttc llc::err_t processDescriptorClose(int & descriptor) {
	if(0 > descriptor)
		rtrn 0;
	int cancellation = 0;
	if(0 != pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cancellation))
		rtrn OS_ERROR;
	cnst int closing = descriptor;
	descriptor = -1;
	// Linux releases the descriptor even on EINTR; retrying could close a reused fd.
	cnst int result = close(closing);
	cnst int restored = pthread_setcancelstate(cancellation, 0);
	rtrn result || restored ? OS_ERROR : 0;
}

stct auto_process_descriptor : auto_handler<int*, nullptr> {
	explicit auto_process_descriptor(int & descriptor) nxpt { Handle = &descriptor; }
	~auto_process_descriptor() { if_fail_e(Close()); }
	err_t Close() {
		if(!Handle) rtrn 0;
		int * closing = Handle;
		Handle = 0;
		rtrn processDescriptorClose(*closing);
	}
};

stct auto_process_attributes : auto_handler<pthread_attr_t*, nullptr> {
	err_t & Result;
	auto_process_attributes(pthread_attr_t & attributes, err_t & result) nxpt : Result(result) { Handle = &attributes; }
	~auto_process_attributes() { if_fail_e(Close()); }
	err_t Close() {
		if(!Handle) rtrn 0;
		pthread_attr_t * closing = Handle;
		Handle = 0;
		if(0 == pthread_attr_destroy(closing)) rtrn 0;
		Result = OS_ERROR;
		rtrn OS_ERROR;
	}
};

stct auto_process_actions : auto_handler<posix_spawn_file_actions_t*, nullptr> {
	explicit auto_process_actions(posix_spawn_file_actions_t & actions) nxpt { Handle = &actions; }
	~auto_process_actions() { if_fail_e(Close()); }
	err_t Close() {
		if(!Handle) rtrn 0;
		posix_spawn_file_actions_t * closing = Handle;
		Handle = 0;
		rtrn posix_spawn_file_actions_destroy(closing) ? OS_ERROR : 0;
	}
};

sttc llc::err_t processPipesClose(SProcessLinux & process) {
	llc::err_t result = 0;
	if(failed(processDescriptorClose(process.StdinRead))) result = OS_ERROR;
	if(failed(processDescriptorClose(process.StdinWrite))) result = OS_ERROR;
	if(failed(processDescriptorClose(process.StdoutRead))) result = OS_ERROR;
	if(failed(processDescriptorClose(process.StdoutWrite))) result = OS_ERROR;
	rtrn result;
}

sttc void * processReap(void * parameter) {
	cnst pid_t child = (pid_t)(intptr_t)parameter;
	while(0 > waitpid(child, 0, 0) && EINTR == errno) {}
	rtrn 0;
}

llc::err_t processClose(SProcessLinux & process) {
	llc::err_t result = processPipesClose(process);
	if(0 >= process.PID)
		rtrn result;
	pid_t waited = -1;
	do { waited = waitpid(process.PID, 0, WNOHANG); } while(0 > waited && EINTR == errno);
	if(0 == waited) {
		cnst err_t error = [&]() -> err_t {
			pthread_attr_t attributes;
			if_true_ve(OS_ERROR, pthread_attr_init(&attributes));
			auto_process_attributes scope{attributes, result};
			if_true_ve(OS_ERROR, pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED));
			pthread_t waiter;
			if_true_ve(OS_ERROR, pthread_create(&waiter, &attributes, processReap, (void*)(intptr_t)process.PID));
			rtrn 0;
		}();
		if(error) {
			// Resource exhaustion must not leave a zombie; fall back to a blocking reap.
			processReap((void*)(intptr_t)process.PID);
			result = OS_ERROR;
		}
	}
	else if(0 > waited && ECHILD != errno)
		rtrn OS_ERROR;
	process.PID = -1;
	rtrn result;
}

sttc llc::err_t processPipe(int & readEnd, int & writeEnd) {
	int descriptors[2] = {-1, -1};
	if(0 != pipe2(descriptors, O_CLOEXEC))
		rtrn OS_ERROR;
	auto_process_descriptor writeScope{descriptors[1]}, readScope{descriptors[0]};
	// Keep spawn's source descriptors distinct from stdin/stdout/stderr, even if closed.
	for(int & descriptor : descriptors) {
		if(descriptor > STDERR_FILENO)
			continue;
		cnst int moved = fcntl(descriptor, F_DUPFD_CLOEXEC, STDERR_FILENO + 1);
		cnst llc::err_t closed = processDescriptorClose(descriptor);
		descriptor = moved;
		if_true_ve(OS_ERROR, 0 > moved || failed(closed));
	}
	readEnd = descriptors[0];
	writeEnd = descriptors[1];
	readScope.Handle = writeScope.Handle = 0;
	rtrn 0;
}

llc::err_t processRedirect(SProcessLinux & process) {
	if(0 < process.PID)
		rtrn OS_BUSY;
	if_fail_fe(processPipesClose(process));
	SProcessNativeScope<SProcessLinux> rollback{process};
	if_fail_ve(OS_ERROR, processPipe(process.StdinRead, process.StdinWrite));
	if_fail_ve(OS_ERROR, processPipe(process.StdoutRead, process.StdoutWrite));
	rollback.Inactive = true;
	rtrn 0;
}

// Compact one writable command line into argv storage; no per-argument strings or shell.
// Match MS CRT f3_t quotes/backslashes, including argv[0]'s literal backslashes.
sttc llc::err_t processArguments(as0_t & storage, apod<char*> & arguments) {
	u2_t read = 0, write = 0;
	while(read + 1 < storage.size()) {
		while(' ' == storage[read] || '\t' == storage[read]) ++read;
		if(!storage[read]) break;
		cnst bool programName = 0 == arguments.size();
		if_fail_fe(arguments.push_back(storage.begin() + write));
		bool quoted = false;
		while(storage[read] && (quoted || (' ' != storage[read] && '\t' != storage[read]))) {
			u2_t slashes = 0;
			if(!programName)
				while('\\' == storage[read]) { ++slashes; ++read; }
			cnst bool quote = '"' == storage[read];
			for(u2_t i = 0, count = quote ? slashes / 2 : slashes; i < count; ++i)
				storage[write++] = '\\';
			if(quote) {
				++read;
				if(slashes & 1U)
					storage[write++] = '"';
				else if(!programName && quoted && '"' == storage[read]) {
					storage[write++] = '"';
					++read;
				}
				else
					quoted = !quoted;
			}
			else if(storage[read] && (quoted || (' ' != storage[read] && '\t' != storage[read])))
				storage[write++] = storage[read++];
		}
		// Advance before replacing the separator when read and write coincide.
		while(' ' == storage[read] || '\t' == storage[read]) ++read;
		storage[write++] = 0;
	}
	if_true_fef(!arguments.size() || !arguments[0][0], "%s", "Command line must include a nonempty program name.");
	if_fail_fe(arguments.push_back(0));
	rtrn 0;
}

llc::err_t processCreate(SProcessLinux & process, vcst_t application, vcst_t commandLine, vcs0_t environment, u2_t creationFlags) {
	if(0 < process.PID)
		rtrn OS_BUSY;
	if_true_fef(creationFlags, "%s", "Windows process creation flags are unsupported on Linux.");
	if_true_fef(application.size() && memchr(application.begin(), 0, application.size()), "%s", "Application contains an embedded zero.");
	if_true_fef(commandLine.size() && memchr(commandLine.begin(), 0, commandLine.size()), "%s", "Command line contains an embedded zero.");
	as0_t argumentStorage, environmentStorage = environment;
	apod<char*> argv, envp;
	if_fail_fe(argumentStorage.append(commandLine.begin(), commandLine.size()));
	if_fail_fe(argumentStorage.push_back(0));
	if_fail_fe(processArguments(argumentStorage, argv));
	string executable = application;
	for(u2_t offset = 0; offset < environmentStorage.size() && environmentStorage[offset]; ) {
		char * end = (char*)memchr(environmentStorage.begin() + offset, 0, environmentStorage.size() - offset);
		if_null_fef(end, "%s", "Unterminated process environment entry.");
		if_fail_fe(envp.push_back(environmentStorage.begin() + offset));
		offset = (u2_t)(end - environmentStorage.begin()) + 1;
	}
	if_fail_fe(envp.push_back(0));
	cnst bool redirected = 0 <= process.StdinRead && 0 <= process.StdinWrite && 0 <= process.StdoutRead && 0 <= process.StdoutWrite;
	if_true_fef(!redirected && (0 <= process.StdinRead || 0 <= process.StdinWrite || 0 <= process.StdoutRead || 0 <= process.StdoutWrite), "%s", "Incomplete process pipe setup.");
	posix_spawn_file_actions_t actions;
	if(0 != posix_spawn_file_actions_init(&actions))
		rtrn OS_ERROR;
	auto_process_actions scope{actions};
	if(redirected) {
		if_true_ve(OS_ERROR, posix_spawn_file_actions_adddup2(&actions, process.StdinRead, STDIN_FILENO));
		if_true_ve(OS_ERROR, posix_spawn_file_actions_adddup2(&actions, process.StdoutWrite, STDOUT_FILENO));
		if_true_ve(OS_ERROR, posix_spawn_file_actions_adddup2(&actions, process.StdoutWrite, STDERR_FILENO));
		if_true_ve(OS_ERROR, posix_spawn_file_actions_addclose(&actions, process.StdinRead));
		if_true_ve(OS_ERROR, posix_spawn_file_actions_addclose(&actions, process.StdinWrite));
		if_true_ve(OS_ERROR, posix_spawn_file_actions_addclose(&actions, process.StdoutRead));
		if_true_ve(OS_ERROR, posix_spawn_file_actions_addclose(&actions, process.StdoutWrite));
	}
	cnst int error = application.size()
		? posix_spawn(&process.PID, executable.begin(), &actions, 0, argv.begin(), environment.size() ? envp.begin() : environ)
		: posix_spawnp(&process.PID, argv[0], &actions, 0, argv.begin(), environment.size() ? envp.begin() : environ);
	if(error) {
		process.PID = -1;
		rtrn OS_ERROR;
	}
	cnst err_t destroyed = scope.Close();
	cnst llc::err_t closedInput = processDescriptorClose(process.StdinRead);
	cnst llc::err_t closedOutput = processDescriptorClose(process.StdoutWrite);
	rtrn destroyed || failed(closedInput) || failed(closedOutput) ? OS_ERROR : 0;
}

stct SProcessWriteScope {
	auto_process_descriptor Pipe;
	sigset_t Blocked, Previous;
	bool BrokenPipe = false, AlreadyPending = false, Inactive = false;
	SProcessWriteScope(int & descriptor, cnst sigset_t & blocked, cnst sigset_t & previous) nxpt
		: Pipe(descriptor), Blocked(blocked), Previous(previous) {}
	~SProcessWriteScope() { if_fail_e(Close()); }
	err_t Close() {
		if(Inactive) rtrn 0;
		Inactive = true;
		if(BrokenPipe && !AlreadyPending) {
			cnst timespec noWait = {};
			while(0 > sigtimedwait(&Blocked, 0, &noWait) && EINTR == errno) {}
		}
		cnst int restored = pthread_sigmask(SIG_SETMASK, &Previous, 0);
		cnst err_t closed = Pipe.Close();
		rtrn restored ? OS_ERROR : closed;
	}
};

llc::err_t processWrite(SProcessLinux & process, vcu0_t input) {
	if(0 > process.StdinWrite)
		rtrn input.size() ? OS_ERROR : 0;
	// Block SIGPIPE only in this thread; never change the application's signal disposition.
	sigset_t blocked, previous, pending;
	sigemptyset(&blocked);
	sigaddset(&blocked, SIGPIPE);
	if(0 != pthread_sigmask(SIG_BLOCK, &blocked, &previous))
		rtrn OS_ERROR;
	SProcessWriteScope scope{process.StdinWrite, blocked, previous};
	if_true_ve(OS_ERROR, sigpending(&pending));
	scope.AlreadyPending = 1 == sigismember(&pending, SIGPIPE);
	for(u2_t offset = 0; offset < input.size(); ) {
		cnst ssize_t written = write(process.StdinWrite, input.begin() + offset, input.size() - offset);
		if(0 > written && EINTR == errno)
			continue;
		if(0 >= written) {
			scope.BrokenPipe = 0 > written && EPIPE == errno;
			rtrn OS_ERROR;
		}
		offset += (u2_t)written;
	}
	rtrn scope.Close();
}

llc::err_t processRead(SProcessLinux & process, au0_t & output) {
	if(0 > process.StdoutRead)
		rtrn OS_ERROR;
	auto_process_descriptor scope{process.StdoutRead};
	u0_t buffer[4096] = {};
	for(;;) {
		cnst ssize_t count = read(process.StdoutRead, buffer, sizeof(buffer));
		if(0 > count && EINTR == errno)
			continue;
		if_true_ve(OS_ERROR, 0 > count);
		if(0 == count) break;
		if_fail_ve(OS_ERROR, output.append(buffer, (u2_t)count));
	}
	rtrn scope.Close();
}

llc::err_t processWait(SProcessLinux & process, u2_t & exitCode) {
	if(0 >= process.PID)
		rtrn OS_ERROR;
	int status = 0;
	pid_t waited;
	do { waited = waitpid(process.PID, &status, 0); } while(0 > waited && EINTR == errno);
	if(0 > waited) {
		if(ECHILD == errno) process.PID = -1;
		rtrn OS_ERROR;
	}
	process.PID = -1;
	exitCode = WIFEXITED(status) ? (u2_t)WEXITSTATUS(status) : 128U + (u2_t)WTERMSIG(status);
	rtrn 0;
}

stct SProcessInput {
	SProcessLinux & Process;
	vcu0_t Input;
	llc::err_t Result = OS_ERROR;
};

sttc void * processInputWrite(void * parameter) {
	SProcessInput & context = *(SProcessInput*)parameter;
	context.Result = processWrite(context.Process, context.Input);
	rtrn 0;
}

stct auto_process_writer : auto_handler<pthread_t*, nullptr> {
	err_t Result = OS_ERROR;
	explicit auto_process_writer(pthread_t & writer) nxpt { Handle = &writer; }
	~auto_process_writer() { if_fail_e(Close()); }
	err_t Close() {
		if(!Handle) rtrn 0;
		cnst pthread_t closing = *Handle;
		Handle = 0;
		if(failed(Result) && 0 != pthread_cancel(closing)) Result = OS_ERROR;
		rtrn pthread_join(closing, 0) ? OS_ERROR : 0;
	}
};

llc::err_t processCommunicate(SProcessLinux & process, vcu0_t input, au0_t & output) {
	if(!input.size()) {
		if_fail_fe(processWrite(process, input));
		rtrn processRead(process, output);
	}
	SProcessInput context = {process, input};
	pthread_t writer;
	if(0 != pthread_create(&writer, 0, processInputWrite, &context))
		rtrn OS_ERROR;
	auto_process_writer scope{writer};
	scope.Result = processRead(process, output);
	cnst err_t joined = scope.Close();
	rtrn failed(joined) || failed(context.Result) ? OS_ERROR : scope.Result;
}
}
#endif

