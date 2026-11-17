#include "gpk_process_broker.h"

#include "gpk_json_expression.h"
#include "gpk_parse.h"
#include "gpk_string_compose.h"

#include <process.h>

usns gpk;

sttc	void	closeHandle				(HANDLE & handle) {
	if(handle && INVALID_HANDLE_VALUE != handle)
		::CloseHandle(handle);
	handle								= INVALID_HANDLE_VALUE;
}

sttc	void	closeProcessHandles		(::gpk::SProcessBrokerJob & job) {
	::closeHandle(job.IOHandles.ChildStd_IN_Read);
	::closeHandle(job.IOHandles.ChildStd_IN_Write);
	::closeHandle(job.IOHandles.ChildStd_OUT_Read);
	::closeHandle(job.IOHandles.ChildStd_OUT_Write);
	::closeHandle(job.IOHandles.ChildStd_ERR_Read);
	::closeHandle(job.IOHandles.ChildStd_ERR_Write);
	::closeHandle(job.Process.ProcessInfo.hProcess);
	::closeHandle(job.Process.ProcessInfo.hThread);
}

sttc	::gpk::err_t	initHandles		(::gpk::SProcessHandles & handles) {
	SECURITY_ATTRIBUTES			saAttr					= {sizeof(SECURITY_ATTRIBUTES)};
	saAttr.bInheritHandle							= TRUE;
	saAttr.lpSecurityDescriptor					= nullptr;
	if_true_fef(false == (b8_t)::CreatePipe				(&handles.ChildStd_OUT_Read, &handles.ChildStd_OUT_Write, &saAttr, 0), "Stdout CreatePipe: '%s'.", "Failed to create a pipe for the child process's STDOUT.");
	if_true_fef(false == (b8_t)::SetHandleInformation	(handles.ChildStd_OUT_Read, HANDLE_FLAG_INHERIT, 0), "Stdout SetHandleInformation: '%s'.", "Failed to keep the child process from inheriting the STDOUT read handle.");
	if_true_fef(false == (b8_t)::CreatePipe				(&handles.ChildStd_ERR_Read, &handles.ChildStd_ERR_Write, &saAttr, 0), "Stderr CreatePipe: '%s'.", "Failed to create a pipe for the child process's STDERR.");
	if_true_fef(false == (b8_t)::SetHandleInformation	(handles.ChildStd_ERR_Read, HANDLE_FLAG_INHERIT, 0), "Stderr SetHandleInformation: '%s'.", "Failed to keep the child process from inheriting the STDERR read handle.");
	if_true_fef(false == (b8_t)::CreatePipe				(&handles.ChildStd_IN_Read, &handles.ChildStd_IN_Write, &saAttr, 0), "Stdin CreatePipe: '%s'.", "Failed to create a pipe for the child process's STDIN.");
	if_true_fef(false == (b8_t)::SetHandleInformation	(handles.ChildStd_IN_Write, HANDLE_FLAG_INHERIT, 0), "Stdin SetHandleInformation: '%s'.", "Failed to keep the child process from inheriting the STDIN write handle.");
	return 0;
}

sttc	::gpk::err_t	createChildProcess
	( ::gpk::SProcess	& process
	, ::gpk::vu0_t			environmentBlock
	, ::gpk::vcst_t		appPath
	, ::gpk::vsc_t		commandLine
	) {
	stxp	uint32_t		creationFlags				= CREATE_SUSPENDED;
	::closeHandle(process.ProcessInfo.hProcess);
	::closeHandle(process.ProcessInfo.hThread);
	if_true_fef(false == (b8_t)::CreateProcessA
		( appPath.begin()
		, commandLine.begin()
		, nullptr
		, nullptr
		, true
		, creationFlags
		, environmentBlock.begin()
		, nullptr
		, &process.StartInfo
		, &process.ProcessInfo
		), "Failed to create process '%s'.", appPath.begin());
	info_printf("Creating process '%s' with command line '%s'.", appPath.begin(), commandLine.begin());
	::ResumeThread(process.ProcessInfo.hThread);
	return 0;
}

sttc	::gpk::err_t	writeToPipe			(::gpk::SProcessHandles & handles, ::gpk::vcu0_t bytesToSend) {
	uint32_t					offsetWritten				= 0;
	while(offsetWritten < bytesToSend.size()) {
		DWORD						dwWritten					= 0;
		if_true_fef(false == (b8_t)::WriteFile(handles.ChildStd_IN_Write, &bytesToSend[offsetWritten], bytesToSend.size() - offsetWritten, &dwWritten, nullptr), "%s", "Failed to write to child process' standard input.");
		if_true_fef(0 == dwWritten, "%s", "No bytes were written to child process' standard input.");
		offsetWritten							+= dwWritten;
	}
	::closeHandle(handles.ChildStd_IN_Write);
	return 0;
}

sttc	gpk::err_t	readFromPipe			(HANDLE pipe, ::gpk::au0_t & readBytes) {
	stxp	u2_c			bufferSize					= 1024 * 1024;
	gpk::au0_t				buffer;
	if_fail_fe(buffer.resize(bufferSize));
	for(;;) {
		DWORD					childProcessBytesRead		= 0;
		if(false == (b8_t)::ReadFile(pipe, buffer.begin(), buffer.size(), &childProcessBytesRead, 0)) {
			cnst DWORD				lastError					= ::GetLastError();
			if_true_bw(ERROR_BROKEN_PIPE == lastError);
			if_true_fef(true, "Failed to read child process' standard output. Windows error: %lu.", lastError);
		}
		if_zero_bi(childProcessBytesRead);
		if_fail_fe(readBytes.append(buffer.begin(), childProcessBytesRead));
	}
	return 0;
}

sttc	void	threadReadError			(void * jobAddress) {
	::gpk::SProcessBrokerJob	& job						= *(::gpk::SProcessBrokerJob*)jobAddress;
	job.ErrorResult								= ::readFromPipe(job.IOHandles.ChildStd_ERR_Read, job.ErrorOutput);
	gpk_sync_increment(job.ErrorDone);
}

sttc	gpk::err_t	processRequest			(::gpk::SProcessBrokerJob & job) {
	gpk::err_c			result						= [&job]() -> ::gpk::err_t {
		if_fail_fe(::initHandles(job.IOHandles));
		job.Process.StartInfo.hStdError				= job.IOHandles.ChildStd_ERR_Write;
		job.Process.StartInfo.hStdOutput				= job.IOHandles.ChildStd_OUT_Write;
		job.Process.StartInfo.hStdInput				= job.IOHandles.ChildStd_IN_Read;
		job.Process.StartInfo.dwFlags				|= STARTF_USESTDHANDLES;

		::gpk::vu0_t				environmentBlock				= job.Request;
		cnst ::gpk::vcu0_t		packet						= job.Request;
		cnst ::gpk::err_t	contentOffset				= packet.find(::gpk::vcu0_t{(cnst uint8_t*)"\0", 2});
		if_true_fef(0 > contentOffset, "%s", "Failed to find the CGI environment block terminator.");

		if_fail_fe(::createChildProcess(job.Process, environmentBlock, job.CommandLineApp, job.CommandLine));
		::closeHandle(job.IOHandles.ChildStd_OUT_Write);
		::closeHandle(job.IOHandles.ChildStd_ERR_Write);
		::closeHandle(job.IOHandles.ChildStd_IN_Read);

		if((uP_t)-1L == ::_beginthread(::threadReadError, 0, &job)) {
			job.ErrorResult							= -1;
			::closeHandle(job.IOHandles.ChildStd_ERR_Read);
			gpk_sync_increment(job.ErrorDone);
		}
		::gpk::err_t				inputWriteResult				= 0;
		::gpk::u2_c				contentBegin					= (uint32_t)contentOffset + 2;
		if(packet.size() > contentBegin)
			inputWriteResult						= ::writeToPipe(job.IOHandles, {&packet[contentBegin], packet.size() - contentBegin});
		else
			::closeHandle(job.IOHandles.ChildStd_IN_Write);

		cnst ::gpk::err_t	responseReadResult			= ::readFromPipe(job.IOHandles.ChildStd_OUT_Read, job.Response);
		while(false == gpk_sync_compare_exchange(job.ErrorDone, 1, 1))
			::gpk::sleep(1);
		if_fail_fe(inputWriteResult);
		if_fail_fe(responseReadResult);
		if_fail_fe(job.ErrorResult);
		return 0;
	}();
	::closeProcessHandles(job);
	return result;
}

sttc	void	threadProcessRequest		(void * jobAddress) {
	::gpk::SProcessBrokerJob	& job			= *(::gpk::SProcessBrokerJob*)jobAddress;
	if_true_block_log(error_printf, ::gpk::failed(job.Result = ::processRequest(job)) && 0 == job.Response.size(), ) {
		stxp	::gpk::vcst_c	failureResponse		= "LLS process broker failed to execute the configured process.";
		if_fail_re(job.Response.append(failureResponse.bytes()));
	}
	gpk_sync_increment(job.Done);
}

gpk::err_t	gpk::processBrokerSetup		(SProcessBroker & broker, u1_t port, s1_t adapter) {
	broker.Port								= port;
	broker.Adapter							= adapter;
	broker.CommandLineApp					= broker.ProcessFileName;
	broker.CommandLine						= broker.ProcessMockPath;
	if(broker.ProcessParams.size()) {
		broker.CommandLine.push_back(' ');
		broker.CommandLine.append(broker.ProcessParams);
	}
	broker.CommandLine.push_back(0);
	broker.CommandLineApp.push_back(0);
	if_fail_fef(serverStart(broker.Server, broker.Port, broker.Adapter), "Failed to start process broker on port %u.", broker.Port);
	broker.Started								= true;
	return 0;
}

gpk::err_t	gpk::processBrokerUpdate		(SProcessBroker & broker) {
	aobj<TPacketQueue>	receivedPerClient;
	if_fail_fe(serverPayloadCollect(broker.Server, receivedPerClient));
	for(uint32_t iClient = 0; iClient < receivedPerClient.size(); ++iClient) {
		pobj<SConnection>	connection;
		{
			::gpk::lock_guard			lock						(broker.Server.Mutex);
			if(iClient < broker.Server.Clients.size())
				connection							= broker.Server.Clients[iClient];
		}
		if(!connection)
			continue;
		for(uint32_t iMessage = 0; iMessage < receivedPerClient[iClient].size(); ++iMessage) {
			cnst pobj<SMessage>	& message			= receivedPerClient[iClient][iMessage];
			if(!message)
				continue;
			pobj<SProcessBrokerJob>	job;
			job.create();
			job->Connection							= connection;
			job->Request								= message->Payload;
			job->CommandLineApp						= broker.CommandLineApp;
			job->CommandLine							= broker.CommandLine;
			if_fail_fe(broker.Jobs.push_back(job));
			if((uP_t)-1L == ::_beginthread(::threadProcessRequest, 0, job.operator->())) {
				job->Result								= -1;
				stxp cnst sc_t	failureResponse[]				= "LLS process broker failed to start its request worker.";
				job->Response.append((cnst uint8_t*)failureResponse, size(failureResponse) - 1);
				gpk_sync_increment(job->Done);
			}
		}
	}

	for(uint32_t iJob = 0; iJob < broker.Jobs.size();) {
		pobj<SProcessBrokerJob>	job				= broker.Jobs[iJob];
		if(!job || false == gpk_sync_compare_exchange(job->Done, 1, 1)) {
			++iJob;
			continue;
		}
		if(job->Connection && job->Response.size()) {
			::gpk::lock_guard			lock						(broker.Server.Mutex);
			if_fail_e(connectionPushData(*job->Connection, job->Connection->Queue, job->Response, true, true));
		}
		if_fail_fe(broker.Jobs.remove_unordered(iJob));
	}
	return 0;
}

gpk::err_t	gpk::processBrokerShutdown	(SProcessBroker & broker) {
	if(broker.Started)
		if_fail_w(serverStop(broker.Server));
	broker.Started								= false;
	while(broker.Jobs.size()) {
		for(uint32_t iJob = 0; iJob < broker.Jobs.size();) {
			pobj<SProcessBrokerJob>	& job		= broker.Jobs[iJob];
			if(job && false == gpk_sync_compare_exchange(job->Done, 1, 1)) {
				++iJob;
				continue;
			}
			if_fail_fe(broker.Jobs.remove_unordered(iJob));
		}
		if(broker.Jobs.size())
			::gpk::sleep(1);
	}
	return 0;
}
