#include "gpk_transfer.h"
#include "gpk_crc.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#	include <winsock2.h>
#	include <ws2tcpip.h>
#	include <windows.h>
#	include <io.h>
#else
#	include <arpa/inet.h>
#	include <netinet/in.h>
#	include <sys/socket.h>
#	include <sys/stat.h>
#	include <sys/wait.h>
#	include <unistd.h>
#endif

namespace fs = ::std::filesystem;

#ifdef _WIN32
using socket_handle_t = SOCKET;
static constexpr socket_handle_t SOCKET_HANDLE_INVALID = INVALID_SOCKET;
#else
using socket_handle_t = int;
static constexpr socket_handle_t SOCKET_HANDLE_INVALID = -1;
#endif

static void socketClose(socket_handle_t socketHandle) {
	if(SOCKET_HANDLE_INVALID == socketHandle)
		return;
#ifdef _WIN32
	::closesocket(socketHandle);
#else
	::close(socketHandle);
#endif
}

static bool socketInitialize() {
#ifdef _WIN32
	WSADATA data = {};
	return 0 == ::WSAStartup(MAKEWORD(2, 2), &data);
#else
	return true;
#endif
}

static void socketShutdown() {
#ifdef _WIN32
	::WSACleanup();
#endif
}

static void socketTimeout(socket_handle_t socketHandle) {
#ifdef _WIN32
	const DWORD timeout = 30000;
	::setsockopt(socketHandle, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
	::setsockopt(socketHandle, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
#else
	const timeval timeout = {30, 0};
	::setsockopt(socketHandle, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	::setsockopt(socketHandle, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
#endif
}

static bool socketSendAll(socket_handle_t socketHandle, const void * data, size_t size) {
	const char * cursor = (const char*)data;
	while(size) {
#ifdef _WIN32
		const int sent = ::send(socketHandle, cursor, (int)size, 0);
#else
		const ssize_t sent = ::send(socketHandle, cursor, size, 0);
#endif
		if(0 >= sent)
			return false;
		cursor += sent;
		size -= (size_t)sent;
	}
	return true;
}

// 0: success, 1: orderly close, -1: error.
static int socketReceiveAll(socket_handle_t socketHandle, void * data, size_t size) {
	char * cursor = (char*)data;
	while(size) {
#ifdef _WIN32
		const int received = ::recv(socketHandle, cursor, (int)size, 0);
#else
		const ssize_t received = ::recv(socketHandle, cursor, size, 0);
#endif
		if(0 == received)
			return 1;
		if(0 > received)
			return -1;
		cursor += received;
		size -= (size_t)received;
	}
	return 0;
}

enum class PEER_FRAME : uint8_t {
	HELLO		= 1,
	TRANSFER	= 2,
	RESULT		= 3,
};

static constexpr uint32_t PEER_FRAME_MAX = 16U * 1024U * 1024U;

static bool frameSend(socket_handle_t socketHandle, PEER_FRAME type, const void * data, size_t size) {
	if(size > PEER_FRAME_MAX)
		return false;
	const uint8_t typeValue = (uint8_t)type;
	const uint32_t sizeNetwork = ::htonl((uint32_t)size);
	return ::socketSendAll(socketHandle, &typeValue, sizeof(typeValue))
		&& ::socketSendAll(socketHandle, &sizeNetwork, sizeof(sizeNetwork))
		&& (0 == size || ::socketSendAll(socketHandle, data, size));
}

static int frameReceive(socket_handle_t socketHandle, PEER_FRAME & type, ::gpk::au0_t & payload) {
	uint8_t typeValue = 0;
	int result = ::socketReceiveAll(socketHandle, &typeValue, sizeof(typeValue));
	if(result)
		return result;
	uint32_t sizeNetwork = 0;
	result = ::socketReceiveAll(socketHandle, &sizeNetwork, sizeof(sizeNetwork));
	if(result)
		return result;
	const uint32_t sizeFrame = ::ntohl(sizeNetwork);
	if(sizeFrame > PEER_FRAME_MAX || typeValue < (uint8_t)PEER_FRAME::HELLO || typeValue > (uint8_t)PEER_FRAME::RESULT)
		return -1;
	payload.clear();
	if(::gpk::failed(payload.resize(sizeFrame)))
		return -1;
	if(sizeFrame && 0 != ::socketReceiveAll(socketHandle, payload.begin(), payload.size()))
		return -1;
	type = (PEER_FRAME)typeValue;
	return 0;
}

static bool eventSend(socket_handle_t socketHandle, const ::gpk::EventTransfer & eventToSend) {
	::gpk::au0_t serialized;
	if(::gpk::failed(eventToSend.Save(serialized)) || serialized.size() > 1024U * 1024U)
		return false;
	return ::frameSend(socketHandle, PEER_FRAME::TRANSFER, serialized.begin(), serialized.size());
}

static int eventLoad(::gpk::vcu0_t serialized, ::gpk::EventTransfer & eventReceived) {
	::gpk::vcu0_t input = serialized;
	return ::gpk::failed(eventReceived.Load(input)) || input.size() ? -1 : 0;
}

struct SPeerPlan {
	::std::string	Resource	= {};
	::std::string	Request		= {};
	bool			Run			= false;
};

static bool helloSend(socket_handle_t socketHandle, const SPeerPlan & plan) {
	::gpk::au0_t payload;
	::gpk::au0_t request;
	const ::gpk::u0_t version = 1;
	const ::gpk::u0_t sending = plan.Resource.empty() ? 0 : 1;
	const ::gpk::u0_t run = plan.Run ? 1 : 0;
	if(::gpk::failed(request.resize((::gpk::u2_t)plan.Request.size())))
		return false;
	if(plan.Request.size())
		::memcpy(request.begin(), plan.Request.data(), plan.Request.size());
	if(::gpk::failed(::gpk::savePOD(payload, version))
	|| ::gpk::failed(::gpk::savePOD(payload, sending))
	|| ::gpk::failed(::gpk::savePOD(payload, run))
	|| ::gpk::failed(::gpk::saveView(payload, request)))
		return false;
	return ::frameSend(socketHandle, PEER_FRAME::HELLO, payload.begin(), payload.size());
}

static bool helloLoad(::gpk::vcu0_t payload, SPeerPlan & plan) {
	::gpk::u0_t version = 0;
	::gpk::u0_t sending = 0;
	::gpk::u0_t run = 0;
	::gpk::vcu0_t request;
	if(::gpk::failed(::gpk::loadPOD(payload, version))
	|| ::gpk::failed(::gpk::loadPOD(payload, sending))
	|| ::gpk::failed(::gpk::loadPOD(payload, run))
	|| ::gpk::failed(::gpk::loadView(payload, request))
	|| payload.size()
	|| 1 != version || 1 < sending || 1 < run || (run && false == sending))
		return false;
	plan.Resource = sending ? "pending" : "";
	plan.Request.assign((const char*)request.begin(), request.size());
	plan.Run = 0 != run;
	return true;
}

static bool queueSend(socket_handle_t socketHandle, const ::gpk::TTransferQueue & queue) {
	for(::gpk::u2_t iEvent = 0; iEvent < queue.size(); ++iEvent)
		if(false == (bool)queue[iEvent] || false == ::eventSend(socketHandle, *queue[iEvent]))
			return false;
	return true;
}

static bool resumeSendBounded
	( socket_handle_t socketHandle
	, ::gpk::STransferState & state
	, const ::gpk::STransferConsumer & consumer
	, const ::gpk::STransferManifest & manifest
	, const ::gpk::EventTransfer & resumeEvent
	) {
	::gpk::vcu0_t input = resumeEvent.Data;
	::gpk::STransferRanges durable;
	if(::gpk::failed(durable.Load(input)) || input.size() || durable.Id != manifest.Id || 0 == state.BlockSize)
		return false;

	::std::vector<::gpk::STransferRange> ranges;
	ranges.reserve(durable.Ranges.size());
	for(::gpk::u2_t iRange = 0; iRange < durable.Ranges.size(); ++iRange) {
		::gpk::STransferRange range = durable.Ranges[iRange];
		if(0 == range.Count || range.Offset >= manifest.Size)
			continue;
		if(range.Count > manifest.Size - range.Offset)
			range.Count = manifest.Size - range.Offset;
		ranges.push_back(range);
	}
	::std::sort(ranges.begin(), ranges.end(), [] (const ::gpk::STransferRange & a, const ::gpk::STransferRange & b) { return a.Offset < b.Offset; });

	auto sendMissing = [&] (::gpk::u3_t begin, ::gpk::u3_t end) {
		while(begin < end) {
			const ::gpk::STransferRange range = {begin, ::gpk::min((::gpk::u3_t)state.BlockSize, end - begin)};
			::gpk::apod<::gpk::STransferRange> requested;
			if(::gpk::failed(requested.push_back(range)))
				return false;
			::gpk::EventTransfer readEvent;
			if(::gpk::failed(::gpk::transferEventRead(readEvent, manifest.Id, requested)))
				return false;
			::gpk::TTransferQueue outgoing;
			if(::gpk::failed(::gpk::transferProcess(state, consumer, readEvent, outgoing)) || false == ::queueSend(socketHandle, outgoing))
				return false;
			begin += range.Count;
		}
		return true;
	};

	::gpk::u3_t cursor = 0;
	for(const ::gpk::STransferRange & range : ranges) {
		if(cursor < range.Offset && false == sendMissing(cursor, range.Offset))
			return false;
		const ::gpk::u3_t rangeEnd = range.Offset + range.Count;
		if(rangeEnd > cursor)
			cursor = rangeEnd;
	}
	return cursor >= manifest.Size || sendMissing(cursor, manifest.Size);
}

static bool resourceValid(const ::std::string & resource) {
	if(resource.empty() || '/' == resource[0] || '\\' == resource[0] || ::std::string::npos != resource.find('\\') || ::std::string::npos != resource.find(':'))
		return false;
	size_t offset = 0;
	while(offset <= resource.size()) {
		const size_t separator = resource.find('/', offset);
		const size_t length = (::std::string::npos == separator ? resource.size() : separator) - offset;
		const ::std::string component = resource.substr(offset, length);
		if(component.empty() || "." == component || ".." == component)
			return false;
		if(::std::string::npos == separator)
			break;
		offset = separator + 1;
	}
	return true;
}

static bool manifestResource(const ::gpk::STransferManifest & manifest, ::std::string & resource) {
	resource.assign((const char*)manifest.Resource.begin(), manifest.Resource.size());
	return ::resourceValid(resource);
}

static bool resourcePath(const fs::path & root, const ::gpk::STransferManifest & manifest, fs::path & output) {
	::std::string resource;
	if(false == ::manifestResource(manifest, resource))
		return false;
	output = root / fs::path(resource);
	return true;
}

static bool fileFlush(FILE * file) {
	if(0 != ::fflush(file))
		return false;
#ifdef _WIN32
	return 0 == ::_commit(::_fileno(file));
#else
	return 0 == ::fsync(::fileno(file));
#endif
}

static bool fileSeek(FILE * file, uint64_t offset) {
#ifdef _WIN32
	return 0 == ::_fseeki64(file, (__int64)offset, SEEK_SET);
#else
	return 0 == ::fseeko(file, (off_t)offset, SEEK_SET);
#endif
}

static FILE * fileOpen(const fs::path & path, const char * mode) {
#ifdef _WIN32
	FILE * file = nullptr;
	return 0 == ::fopen_s(&file, path.string().c_str(), mode) ? file : nullptr;
#else
	return ::fopen(path.string().c_str(), mode);
#endif
}

static bool fileDigest(const fs::path & filePath, uint64_t & sizeFile, uint64_t & digest) {
	::std::error_code error;
	sizeFile = fs::file_size(filePath, error);
	if(error || sizeFile > 0xFFFFFFFFULL)
		return false;
	::gpk::au0_t bytes;
	if(::gpk::failed(bytes.resize((::gpk::u2_t)sizeFile)))
		return false;
	::std::ifstream input(filePath, ::std::ios::binary);
	if(false == input.good())
		return false;
	if(sizeFile)
		input.read((char*)bytes.begin(), (::std::streamsize)sizeFile);
	if(false == input.good() && false == input.eof())
		return false;
	::gpk::u3_t digestFramework = 0;
	if(0 != ::gpk::crcGenerate(bytes, digestFramework))
		return false;
	digest = (uint64_t)digestFramework;
	return true;
}

static bool manifestBuild(uint64_t id, const fs::path & sourceRoot, const ::std::string & resource, ::gpk::STransferManifest & manifest) {
	if(false == ::resourceValid(resource))
		return false;
	uint64_t sizeFile = 0;
	uint64_t digest = 0;
	if(false == ::fileDigest(sourceRoot / fs::path(resource), sizeFile, digest))
		return false;
	manifest = {};
	manifest.Id = id;
	manifest.Size = sizeFile;
	if(::gpk::failed(manifest.Resource.resize((::gpk::u2_t)resource.size())))
		return false;
	if(resource.size())
		::memcpy(manifest.Resource.begin(), resource.data(), resource.size());
	if(::gpk::failed(manifest.Digest.resize(sizeof(digest))))
		return false;
	::memcpy(manifest.Digest.begin(), &digest, sizeof(digest));
	return true;
}

struct SFileEndpoint {
	fs::path		SourceRoot			= {};
	fs::path		CommittedRoot		= {};
	bool			CommitObserved		= false;
	uint64_t		LastCommittedId		= 0;
	::std::string	LastCommittedResource	= {};
	fs::path		LastCommittedPath	= {};
};

static bool transferPaths(const SFileEndpoint & endpoint, const ::gpk::STransferManifest & manifest, fs::path & committed, fs::path & provisional, fs::path & ranges) {
	if(false == ::resourcePath(endpoint.CommittedRoot, manifest, committed))
		return false;
	provisional = fs::path(committed.string() + ".part." + ::std::to_string(manifest.Id));
	ranges = fs::path(committed.string() + ".ranges." + ::std::to_string(manifest.Id));
	return true;
}

static ::gpk::STransferConsumer fileConsumer(SFileEndpoint & endpoint) {
	::gpk::STransferConsumer consumer;
	consumer.ResolveCommitted = [&endpoint](const ::gpk::STransferManifest & manifest) {
		fs::path committed, provisional, ranges;
		if(false == ::transferPaths(endpoint, manifest, committed, provisional, ranges))
			return (::gpk::err_t)-1;
		::std::error_code error;
		fs::create_directories(committed.parent_path(), error);
		return error ? (::gpk::err_t)-1 : (::gpk::err_t)0;
	};
	consumer.ResolveProvisional = [&endpoint](const ::gpk::STransferManifest & manifest) {
		fs::path committed, provisional, ranges;
		if(false == ::transferPaths(endpoint, manifest, committed, provisional, ranges))
			return (::gpk::err_t)-1;
		::std::error_code error;
		if(fs::exists(provisional, error) && !error && fs::file_size(provisional, error) == manifest.Size && !error)
			return (::gpk::err_t)0;
		fs::remove(ranges, error);
		FILE * file = ::fileOpen(provisional, "w+b");
		if(nullptr == file)
			return (::gpk::err_t)-1;
		bool success = true;
		if(manifest.Size)
			success = ::fileSeek(file, manifest.Size - 1) && EOF != ::fputc(0, file);
		success = success && ::fileFlush(file);
		::fclose(file);
		return success ? (::gpk::err_t)0 : (::gpk::err_t)-1;
	};
	consumer.ReadSourceRange = [&endpoint](const ::gpk::STransferManifest & manifest, const ::gpk::STransferRange & range, ::gpk::au0_t & output) {
		fs::path source;
		if(false == ::resourcePath(endpoint.SourceRoot, manifest, source) || range.Count > 0xFFFFFFFFULL)
			return (::gpk::err_t)-1;
		FILE * file = ::fileOpen(source, "rb");
		if(nullptr == file)
			return (::gpk::err_t)-1;
		const bool resized = ::gpk::succeeded(output.resize((::gpk::u2_t)range.Count));
		const bool success = resized && ::fileSeek(file, range.Offset) && range.Count == ::fread(output.begin(), 1, (size_t)range.Count, file);
		::fclose(file);
		return success ? (::gpk::err_t)0 : (::gpk::err_t)-1;
	};
	consumer.WriteProvisionalRange = [&endpoint](const ::gpk::STransferManifest & manifest, const ::gpk::STransferRange & range, ::gpk::vcu0_t data) {
		fs::path committed, provisional, ranges;
		if(false == ::transferPaths(endpoint, manifest, committed, provisional, ranges) || data.size() != range.Count)
			return (::gpk::err_t)-1;
		FILE * file = ::fileOpen(provisional, "r+b");
		if(nullptr == file)
			return (::gpk::err_t)-1;
		bool success = ::fileSeek(file, range.Offset) && data.size() == ::fwrite(data.begin(), 1, data.size(), file) && ::fileFlush(file);
		::fclose(file);
		if(false == success)
			return (::gpk::err_t)-1;
		file = ::fileOpen(ranges, "ab");
		if(nullptr == file)
			return (::gpk::err_t)-1;
		success = 1 == ::fwrite(&range, sizeof(range), 1, file) && ::fileFlush(file);
		::fclose(file);
		return success ? (::gpk::err_t)0 : (::gpk::err_t)-1;
	};
	consumer.QueryDurableRanges = [&endpoint](const ::gpk::STransferManifest & manifest, ::gpk::apod<::gpk::STransferRange> & output) {
		output.clear();
		fs::path committed, provisional, ranges;
		if(false == ::transferPaths(endpoint, manifest, committed, provisional, ranges))
			return (::gpk::err_t)-1;
		FILE * file = ::fileOpen(ranges, "rb");
		if(nullptr == file)
			return (::gpk::err_t)0;
		::gpk::STransferRange range;
		while(1 == ::fread(&range, sizeof(range), 1, file))
			if(::gpk::failed(output.push_back(range))) {
				::fclose(file);
				return (::gpk::err_t)-1;
			}
		::fclose(file);
		return (::gpk::err_t)0;
	};
	consumer.ValidateProvisional = [&endpoint](const ::gpk::STransferManifest & manifest) {
		fs::path committed, provisional, ranges;
		if(false == ::transferPaths(endpoint, manifest, committed, provisional, ranges) || manifest.Digest.size() != sizeof(uint64_t))
			return (::gpk::err_t)-1;
		uint64_t sizeFile = 0;
		uint64_t digest = 0;
		uint64_t expected = 0;
		::memcpy(&expected, manifest.Digest.begin(), sizeof(expected));
		return ::fileDigest(provisional, sizeFile, digest) && sizeFile == manifest.Size && digest == expected ? (::gpk::err_t)0 : (::gpk::err_t)-1;
	};
	consumer.CommitProvisional = [&endpoint](const ::gpk::STransferManifest & manifest) {
		fs::path committed, provisional, ranges;
		if(false == ::transferPaths(endpoint, manifest, committed, provisional, ranges))
			return (::gpk::err_t)-1;
#ifdef _WIN32
		if(false == ::MoveFileExA(provisional.string().c_str(), committed.string().c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
			return (::gpk::err_t)-1;
#else
		if(0 != ::rename(provisional.string().c_str(), committed.string().c_str()))
			return (::gpk::err_t)-1;
#endif
		::std::error_code error;
		fs::remove(ranges, error);
		::std::string resource;
		if(false == ::manifestResource(manifest, resource))
			return (::gpk::err_t)-1;
		endpoint.CommitObserved = true;
		endpoint.LastCommittedId = manifest.Id;
		endpoint.LastCommittedResource = resource;
		endpoint.LastCommittedPath = committed;
		return (::gpk::err_t)0;
	};
	consumer.RollbackProvisional = [&endpoint](const ::gpk::STransferManifest & manifest) {
		fs::path committed, provisional, ranges;
		if(false == ::transferPaths(endpoint, manifest, committed, provisional, ranges))
			return (::gpk::err_t)-1;
		::std::error_code error;
		fs::remove(provisional, error);
		error.clear();
		fs::remove(ranges, error);
		return (::gpk::err_t)0;
	};
	return consumer;
}

struct SExecutionResult {
	bool			Launched	= false;
	int32_t			ExitCode	= -1;
	::gpk::au0_t	Output		= {};
};

static void executionOutputAppend(::gpk::au0_t & output, const void * data, size_t size) {
	static constexpr size_t OUTPUT_MAX = 1024U * 1024U;
	if(output.size() >= OUTPUT_MAX || 0 == size)
		return;
	const size_t available = OUTPUT_MAX - output.size();
	const size_t count = size < available ? size : available;
	const ::gpk::u2_t offset = output.size();
	if(::gpk::failed(output.resize(offset + (::gpk::u2_t)count)))
		return;
	::memcpy(output.begin() + offset, data, count);
}

static bool resultSend(socket_handle_t socketHandle, const SExecutionResult & result) {
	::gpk::au0_t payload;
	const ::gpk::u0_t launched = result.Launched ? 1 : 0;
	if(::gpk::failed(::gpk::savePOD(payload, launched))
	|| ::gpk::failed(::gpk::savePOD(payload, result.ExitCode))
	|| ::gpk::failed(::gpk::saveView(payload, result.Output)))
		return false;
	return ::frameSend(socketHandle, PEER_FRAME::RESULT, payload.begin(), payload.size());
}

static bool resultLoad(::gpk::vcu0_t payload, SExecutionResult & result) {
	::gpk::u0_t launched = 0;
	::gpk::vcu0_t output;
	if(::gpk::failed(::gpk::loadPOD(payload, launched))
	|| 1 < launched
	|| ::gpk::failed(::gpk::loadPOD(payload, result.ExitCode))
	|| ::gpk::failed(::gpk::loadView(payload, output))
	|| payload.size())
		return false;
	result.Launched = 0 != launched;
	result.Output = output;
	return true;
}

static SExecutionResult executableRun(const fs::path & executable) {
	SExecutionResult result;
#ifdef _WIN32
	SECURITY_ATTRIBUTES security = {sizeof(security), nullptr, TRUE};
	HANDLE pipeRead = nullptr;
	HANDLE pipeWrite = nullptr;
	if(false == (bool)::CreatePipe(&pipeRead, &pipeWrite, &security, 0))
		return result;
	if(false == (bool)::SetHandleInformation(pipeRead, HANDLE_FLAG_INHERIT, 0)) {
		::CloseHandle(pipeRead);
		::CloseHandle(pipeWrite);
		return result;
	}

	STARTUPINFOA startup = {};
	startup.cb = sizeof(startup);
	startup.dwFlags = STARTF_USESTDHANDLES;
	startup.hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);
	startup.hStdOutput = pipeWrite;
	startup.hStdError = pipeWrite;
	PROCESS_INFORMATION process = {};
	const ::std::string executableText = executable.string();
	::std::string commandLine = '"' + executableText + '"';
	::std::vector<char> commandMutable(commandLine.begin(), commandLine.end());
	commandMutable.push_back(0);
	const ::std::string workingDirectory = executable.parent_path().string();
	const BOOL created = ::CreateProcessA
		( executableText.c_str(), commandMutable.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW
		, nullptr, workingDirectory.empty() ? nullptr : workingDirectory.c_str(), &startup, &process
		);
	::CloseHandle(pipeWrite);
	if(false == (bool)created) {
		::CloseHandle(pipeRead);
		return result;
	}
	result.Launched = true;
	char buffer[4096];
	DWORD readCount = 0;
	while(::ReadFile(pipeRead, buffer, sizeof(buffer), &readCount, nullptr) && readCount)
		::executionOutputAppend(result.Output, buffer, readCount);
	::CloseHandle(pipeRead);
	::WaitForSingleObject(process.hProcess, INFINITE);
	DWORD exitCode = 0;
	if(::GetExitCodeProcess(process.hProcess, &exitCode))
		result.ExitCode = (int32_t)exitCode;
	::CloseHandle(process.hThread);
	::CloseHandle(process.hProcess);
#else
	const ::std::string executableText = executable.string();
	const ::std::string argumentZero = executable.filename().string();
	const ::std::string workingDirectory = executable.parent_path().string();
	if(0 != ::chmod(executableText.c_str(), 0700))
		return result;
	int outputPipe[2] = {-1, -1};
	if(0 != ::pipe(outputPipe))
		return result;
	const pid_t child = ::fork();
	if(0 == child) {
		::close(outputPipe[0]);
		::dup2(outputPipe[1], STDOUT_FILENO);
		::dup2(outputPipe[1], STDERR_FILENO);
		::close(outputPipe[1]);
		if(false == workingDirectory.empty())
			::chdir(workingDirectory.c_str());
		::execl(executableText.c_str(), argumentZero.c_str(), (char*)nullptr);
		::_exit(127);
	}
	::close(outputPipe[1]);
	if(0 > child) {
		::close(outputPipe[0]);
		return result;
	}
	result.Launched = true;
	char buffer[4096];
	ssize_t readCount = 0;
	while(0 < (readCount = ::read(outputPipe[0], buffer, sizeof(buffer))))
		::executionOutputAppend(result.Output, buffer, (size_t)readCount);
	::close(outputPipe[0]);
	int status = 0;
	if(child == ::waitpid(child, &status, 0)) {
		if(WIFEXITED(status))
			result.ExitCode = WEXITSTATUS(status);
		else if(WIFSIGNALED(status))
			result.ExitCode = 128 + WTERMSIG(status);
	}
#endif
	return result;
}

static socket_handle_t socketListen(uint16_t port) {
	const socket_handle_t listener = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(SOCKET_HANDLE_INVALID == listener)
		return SOCKET_HANDLE_INVALID;
	int reuse = 1;
	::setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));
	sockaddr_in address = {};
	address.sin_family = AF_INET;
	address.sin_port = ::htons(port);
	address.sin_addr.s_addr = ::htonl(INADDR_ANY);
	if(0 != ::bind(listener, (const sockaddr*)&address, sizeof(address)) || 0 != ::listen(listener, 1)) {
		::socketClose(listener);
		return SOCKET_HANDLE_INVALID;
	}
	return listener;
}

static socket_handle_t socketConnect(const char * host, uint16_t port) {
	for(uint32_t iAttempt = 0; iAttempt < 100; ++iAttempt) {
		const socket_handle_t socketHandle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(SOCKET_HANDLE_INVALID == socketHandle)
			return SOCKET_HANDLE_INVALID;
		sockaddr_in address = {};
		address.sin_family = AF_INET;
		address.sin_port = ::htons(port);
		if(1 == ::inet_pton(AF_INET, host, &address.sin_addr) && 0 == ::connect(socketHandle, (const sockaddr*)&address, sizeof(address))) {
			::socketTimeout(socketHandle);
			return socketHandle;
		}
		::socketClose(socketHandle);
		::std::this_thread::sleep_for(::std::chrono::milliseconds(100));
	}
	return SOCKET_HANDLE_INVALID;
}

static uint64_t transactionId(bool connector) {
	const uint64_t role = connector ? 0x8000000000000000ULL : 0;
	return role | ((uint64_t)::std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0x7FFFFFFFFFFFFFFFULL);
}

static int peerSession(socket_handle_t peer, const fs::path & sourceRoot, const fs::path & committedRoot, const SPeerPlan & localPlan, bool connector) {
	SPeerPlan effectiveLocalPlan = localPlan;
	PEER_FRAME frameType = {};
	::gpk::au0_t framePayload;
	SPeerPlan remotePlan;
	if(connector) {
		if(false == ::helloSend(peer, effectiveLocalPlan)
		|| 0 != ::frameReceive(peer, frameType, framePayload)
		|| PEER_FRAME::HELLO != frameType
		|| false == ::helloLoad(framePayload, remotePlan)
		|| false == remotePlan.Request.empty())
			return 1;
	}
	else {
		if(0 != ::frameReceive(peer, frameType, framePayload)
		|| PEER_FRAME::HELLO != frameType
		|| false == ::helloLoad(framePayload, remotePlan)
		|| (false == remotePlan.Request.empty() && false == ::resourceValid(remotePlan.Request)))
			return 1;
		if(false == remotePlan.Request.empty()) {
			if(false == effectiveLocalPlan.Resource.empty() && effectiveLocalPlan.Resource != remotePlan.Request)
				return 1;
			effectiveLocalPlan.Resource = remotePlan.Request;
		}
		if(false == ::helloSend(peer, effectiveLocalPlan))
			return 1;
	}

	SFileEndpoint endpoint = {sourceRoot, committedRoot};
	const ::gpk::STransferConsumer consumer = ::fileConsumer(endpoint);
	::gpk::STransferState state;
	const bool localSending = false == effectiveLocalPlan.Resource.empty();
	const bool remoteSending = false == remotePlan.Resource.empty();
	bool localTransferDone = false == localSending;
	bool remoteTransferDone = false == remoteSending;
	bool localResultDone = false == effectiveLocalPlan.Run;
	bool remoteResultSent = false == remotePlan.Run;
	int remoteExecutionResult = 0;
	uint64_t localId = 0;
	bool localStarted = false;
	::gpk::STransferManifest localManifest;

	if(localSending) {
		localId = ::transactionId(connector);
		if(false == ::manifestBuild(localId, sourceRoot, effectiveLocalPlan.Resource, localManifest))
			return 1;
	}
	auto startLocalTransfer = [&]() {
		if(false == localSending || localStarted)
			return true;
		::gpk::TTransferQueue outgoing;
		if(::gpk::failed(::gpk::transferEnqueueCreate(state, localManifest, outgoing)) || false == ::queueSend(peer, outgoing))
			return false;
		localStarted = true;
		::printf("Queued '%s' (%llu bytes) for peer.\n", effectiveLocalPlan.Resource.c_str(), (unsigned long long)localManifest.Size);
		::fflush(stdout);
		return true;
	};
	// The connector sends first. If it has nothing to send, the listener starts.
	// Serializing the directions prevents two large send queues from filling both
	// TCP windows while neither peer is reading.
	if(localSending && (connector || false == remoteSending) && false == startLocalTransfer())
		return 1;

	while(false == localTransferDone || false == remoteTransferDone || false == localResultDone || false == remoteResultSent) {
		framePayload.clear();
		if(0 != ::frameReceive(peer, frameType, framePayload))
			return 1;
		if(PEER_FRAME::RESULT == frameType) {
			if(false == effectiveLocalPlan.Run || localResultDone)
				return 1;
			SExecutionResult result;
			if(false == ::resultLoad(framePayload, result))
				return 1;
			::printf("Remote execution: launched=%s exit=%d output-bytes=%u\n", result.Launched ? "yes" : "no", result.ExitCode, result.Output.size());
			if(result.Output.size())
				::fwrite(result.Output.begin(), 1, result.Output.size(), stdout);
			if(result.Output.size() && '\n' != result.Output[result.Output.size() - 1])
				::fputc('\n', stdout);
			::fflush(stdout);
			remoteExecutionResult = result.Launched ? result.ExitCode : 1;
			localResultDone = true;
			continue;
		}
		if(PEER_FRAME::TRANSFER != frameType)
			return 1;

		::gpk::EventTransfer incoming;
		if(0 != ::eventLoad(framePayload, incoming))
			return 1;
		if(localSending && ::gpk::LIFECYCLE_COMMAND_RESUME == incoming.Type) {
			if(false == ::resumeSendBounded(peer, state, consumer, localManifest, incoming))
				return 1;
			continue;
		}
		uint64_t completedId = 0;
		if(::gpk::LIFECYCLE_COMMAND_DESTROY == incoming.Type || ::gpk::LIFECYCLE_COMMAND_STOP == incoming.Type) {
			::gpk::vcu0_t commandData = incoming.Data;
			if(::gpk::failed(::gpk::loadPOD(commandData, completedId)) || commandData.size())
				return 1;
		}

		::gpk::TTransferQueue outgoing;
		const ::gpk::err_t processResult = ::gpk::transferProcess(state, consumer, incoming, outgoing);
		if(false == ::queueSend(peer, outgoing) || ::gpk::failed(processResult))
			return 1;
		if(completedId == localId && localSending) {
			if(::gpk::LIFECYCLE_COMMAND_STOP == incoming.Type)
				return 1;
			localTransferDone = true;
		}
		if(endpoint.CommitObserved) {
			::printf("Committed '%s' from peer.\n", endpoint.LastCommittedResource.c_str());
			remoteTransferDone = true;
			if(remotePlan.Run) {
				const SExecutionResult execution = ::executableRun(endpoint.LastCommittedPath);
				if(false == ::resultSend(peer, execution))
					return 1;
				remoteResultSent = true;
			}
			endpoint.CommitObserved = false;
			if(false == startLocalTransfer())
				return 1;
			::fflush(stdout);
		}
	}
	return remoteExecutionResult;
}

static int runListener(uint16_t port, const fs::path & sourceRoot, const fs::path & committedRoot, const SPeerPlan & plan, bool once) {
	const socket_handle_t listener = ::socketListen(port);
	if(SOCKET_HANDLE_INVALID == listener) {
		::fprintf(stderr, "Could not listen on 0.0.0.0:%u.\n", port);
		return 1;
	}
	::printf("GPK transfer peer listening on 0.0.0.0:%u, source=%s, committed=%s\n", port, sourceRoot.string().c_str(), committedRoot.string().c_str());
	::fflush(stdout);
	int result = 0;
	do {
		const socket_handle_t peer = ::accept(listener, nullptr, nullptr);
		if(SOCKET_HANDLE_INVALID == peer) {
			result = 1;
			break;
		}
		::socketTimeout(peer);
		const int sessionResult = ::peerSession(peer, sourceRoot, committedRoot, plan, false);
		::socketClose(peer);
		if(sessionResult) {
			::fprintf(stderr, "GPK transfer peer session failed.\n");
			result = sessionResult;
		}
	} while(false == once);
	::socketClose(listener);
	return result;
}

static int runConnector(const char * host, uint16_t port, const fs::path & sourceRoot, const fs::path & committedRoot, const SPeerPlan & plan) {
	const socket_handle_t peer = ::socketConnect(host, port);
	if(SOCKET_HANDLE_INVALID == peer) {
		::fprintf(stderr, "Could not connect to %s:%u.\n", host, port);
		return 1;
	}
	const int result = ::peerSession(peer, sourceRoot, committedRoot, plan, true);
	::socketClose(peer);
	return result;
}

static int generateFile(const fs::path & path, uint64_t size) {
	::std::error_code error;
	fs::create_directories(path.parent_path(), error);
	if(error)
		return 1;
	FILE * file = ::fileOpen(path, "wb");
	if(nullptr == file)
		return 1;
	for(uint64_t iByte = 0; iByte < size; ++iByte) {
		const uint8_t value = (uint8_t)((iByte * 31U + 7U) & 0xFFU);
		if(1 != ::fwrite(&value, 1, 1, file)) {
			::fclose(file);
			return 1;
		}
	}
	const bool success = ::fileFlush(file);
	::fclose(file);
	return success ? 0 : 1;
}

static void printUsage(const char * executable) {
	::printf("Usage:\n");
	::printf("  %s --listen PORT SOURCE_ROOT COMMITTED_ROOT [RESOURCE [--run]]\n", executable);
	::printf("  %s --listen-once PORT SOURCE_ROOT COMMITTED_ROOT [RESOURCE [--run]]\n", executable);
	::printf("  %s --connect HOST PORT SOURCE_ROOT COMMITTED_ROOT [RESOURCE [--run]] [--pull REMOTE_RESOURCE]\n", executable);
	::printf("  %s --generate FILE SIZE\n", executable);
}

static bool planParse(int argumentCount, char ** arguments, int resourceIndex, SPeerPlan & plan) {
	for(int iArgument = resourceIndex; iArgument < argumentCount; ++iArgument) {
		if(0 == ::strcmp(arguments[iArgument], "--run")) {
			if(plan.Resource.empty() || plan.Run)
				return false;
			plan.Run = true;
		}
		else if(0 == ::strcmp(arguments[iArgument], "--pull")) {
			if(false == plan.Request.empty() || ++iArgument >= argumentCount)
				return false;
			plan.Request = arguments[iArgument];
		}
		else {
			if(false == plan.Resource.empty())
				return false;
			plan.Resource = arguments[iArgument];
		}
	}
	if(plan.Run && plan.Resource.empty())
		return false;
	if(false == plan.Resource.empty() && false == ::resourceValid(plan.Resource))
		return false;
	if(false == plan.Request.empty() && false == ::resourceValid(plan.Request))
			return false;
	return true;
}

int main(int argumentCount, char ** arguments) {
	if(false == ::socketInitialize())
		return 1;
	int result = 1;
	SPeerPlan plan;
	if(5 <= argumentCount && (0 == ::strcmp(arguments[1], "--listen") || 0 == ::strcmp(arguments[1], "--listen-once")) && ::planParse(argumentCount, arguments, 5, plan) && plan.Request.empty())
		result = ::runListener((uint16_t)::strtoul(arguments[2], nullptr, 10), arguments[3], arguments[4], plan, 0 == ::strcmp(arguments[1], "--listen-once"));
	else if(6 <= argumentCount && 0 == ::strcmp(arguments[1], "--connect") && ::planParse(argumentCount, arguments, 6, plan))
		result = ::runConnector(arguments[2], (uint16_t)::strtoul(arguments[3], nullptr, 10), arguments[4], arguments[5], plan);
	else if(4 == argumentCount && 0 == ::strcmp(arguments[1], "--generate"))
		result = ::generateFile(arguments[2], ::strtoull(arguments[3], nullptr, 10));
	else
		::printUsage(arguments[0]);
	::socketShutdown();
	return result;
}
