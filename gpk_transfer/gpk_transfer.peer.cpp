#include "gpk_transfer.h"
#include "gpk_crc.h"

#include <chrono>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>

#ifdef _WIN32
#	include <winsock2.h>
#	include <ws2tcpip.h>
#	include <windows.h>
#	include <io.h>
#else
#	include <arpa/inet.h>
#	include <netinet/in.h>
#	include <sys/socket.h>
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

static bool eventSend(socket_handle_t socketHandle, const ::gpk::EventTransfer & eventToSend) {
	::gpk::au0_t serialized;
	if(::gpk::failed(eventToSend.Save(serialized)) || serialized.size() > 1024U * 1024U)
		return false;
	const uint32_t sizeNetwork = ::htonl(serialized.size());
	return ::socketSendAll(socketHandle, &sizeNetwork, sizeof(sizeNetwork))
		&& ::socketSendAll(socketHandle, serialized.begin(), serialized.size());
}

static int eventReceive(socket_handle_t socketHandle, ::gpk::EventTransfer & eventReceived) {
	uint32_t sizeNetwork = 0;
	const int resultSize = ::socketReceiveAll(socketHandle, &sizeNetwork, sizeof(sizeNetwork));
	if(resultSize)
		return resultSize;
	const uint32_t sizeFrame = ::ntohl(sizeNetwork);
	if(0 == sizeFrame || sizeFrame > 1024U * 1024U)
		return -1;
	::gpk::au0_t serialized;
	if(::gpk::failed(serialized.resize(sizeFrame)))
		return -1;
	if(0 != ::socketReceiveAll(socketHandle, serialized.begin(), serialized.size()))
		return -1;
	::gpk::vcu0_t input = serialized;
	return ::gpk::failed(eventReceived.Load(input)) || input.size() ? -1 : 0;
}

static bool queueSend(socket_handle_t socketHandle, const ::gpk::TTransferQueue & queue) {
	for(::gpk::u2_t iEvent = 0; iEvent < queue.size(); ++iEvent)
		if(false == (bool)queue[iEvent] || false == ::eventSend(socketHandle, *queue[iEvent]))
			return false;
	return true;
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

static socket_handle_t socketListen(uint16_t port) {
	const socket_handle_t listener = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(SOCKET_HANDLE_INVALID == listener)
		return SOCKET_HANDLE_INVALID;
	int reuse = 1;
	::setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));
	sockaddr_in address = {};
	address.sin_family = AF_INET;
	address.sin_port = ::htons(port);
	address.sin_addr.s_addr = ::htonl(INADDR_LOOPBACK);
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

static uint64_t transactionId() {
	return (uint64_t)::std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

static int runServer(uint16_t port, const fs::path & root) {
	const socket_handle_t listener = ::socketListen(port);
	if(SOCKET_HANDLE_INVALID == listener) {
		::fprintf(stderr, "Could not listen on 127.0.0.1:%u.\n", port);
		return 1;
	}
	::printf("WSL peer listening on 127.0.0.1:%u, root=%s\n", port, root.string().c_str());
	::fflush(stdout);
	const socket_handle_t client = ::accept(listener, nullptr, nullptr);
	::socketClose(listener);
	if(SOCKET_HANDLE_INVALID == client)
		return 1;
	::socketTimeout(client);
	SFileEndpoint endpoint = {root, root};
	const ::gpk::STransferConsumer consumer = ::fileConsumer(endpoint);
	::gpk::STransferState state;
	bool returnStarted = false;
	bool done = false;
	while(false == done) {
		::gpk::EventTransfer incoming;
		if(0 != ::eventReceive(client, incoming)) {
			::socketClose(client);
			return 1;
		}
		::gpk::TTransferQueue outgoing;
		const ::gpk::err_t result = ::gpk::transferProcess(state, consumer, incoming, outgoing);
		if(endpoint.CommitObserved && false == returnStarted) {
			::gpk::STransferManifest manifest;
			const uint64_t idReturn = endpoint.LastCommittedId ^ 0x8000000000000000ULL;
			if(false == ::manifestBuild(idReturn, endpoint.SourceRoot, endpoint.LastCommittedResource, manifest)
			|| ::gpk::failed(::gpk::transferEnqueueCreate(state, manifest, outgoing))) {
				::socketClose(client);
				return 1;
			}
			::printf("WSL committed '%s' and queued it for return.\n", endpoint.LastCommittedResource.c_str());
			endpoint.CommitObserved = false;
			returnStarted = true;
		}
		if(false == ::queueSend(client, outgoing)) {
			::socketClose(client);
			return 1;
		}
		if(::gpk::failed(result)) {
			::socketClose(client);
			return 1;
		}
		if(returnStarted && ::gpk::LIFECYCLE_COMMAND_DESTROY == incoming.Type)
			done = true;
	}
	::printf("WSL peer received final completion acknowledgement.\n");
	::socketClose(client);
	return 0;
}

static int runClient(const char * host, uint16_t port, const fs::path & sourceRoot, const fs::path & committedRoot, const ::std::string & resource) {
	const socket_handle_t server = ::socketConnect(host, port);
	if(SOCKET_HANDLE_INVALID == server) {
		::fprintf(stderr, "Could not connect to %s:%u.\n", host, port);
		return 1;
	}
	SFileEndpoint endpoint = {sourceRoot, committedRoot};
	const ::gpk::STransferConsumer consumer = ::fileConsumer(endpoint);
	::gpk::STransferState state;
	::gpk::STransferManifest manifest;
	if(false == ::manifestBuild(::transactionId(), sourceRoot, resource, manifest)) {
		::socketClose(server);
		return 1;
	}
	::gpk::TTransferQueue outgoing;
	if(::gpk::failed(::gpk::transferEnqueueCreate(state, manifest, outgoing)) || false == ::queueSend(server, outgoing)) {
		::socketClose(server);
		return 1;
	}
	::printf("Windows queued '%s' (%llu bytes) for WSL.\n", resource.c_str(), (unsigned long long)manifest.Size);
	while(false == endpoint.CommitObserved) {
		::gpk::EventTransfer incoming;
		if(0 != ::eventReceive(server, incoming)) {
			::socketClose(server);
			return 1;
		}
		outgoing.clear();
		const ::gpk::err_t result = ::gpk::transferProcess(state, consumer, incoming, outgoing);
		if(false == ::queueSend(server, outgoing)) {
			::socketClose(server);
			return 1;
		}
		if(::gpk::failed(result)) {
			::socketClose(server);
			return 1;
		}
	}
	::printf("Windows committed returned '%s'.\n", endpoint.LastCommittedResource.c_str());
	::socketClose(server);
	return 0;
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
	::printf("  %s --listen PORT ROOT\n", executable);
	::printf("  %s --connect HOST PORT SOURCE_ROOT COMMITTED_ROOT RESOURCE\n", executable);
	::printf("  %s --generate FILE SIZE\n", executable);
}

int main(int argumentCount, char ** arguments) {
	if(false == ::socketInitialize())
		return 1;
	int result = 1;
	if(4 == argumentCount && 0 == ::strcmp(arguments[1], "--listen"))
		result = ::runServer((uint16_t)::strtoul(arguments[2], nullptr, 10), arguments[3]);
	else if(7 == argumentCount && 0 == ::strcmp(arguments[1], "--connect"))
		result = ::runClient(arguments[2], (uint16_t)::strtoul(arguments[3], nullptr, 10), arguments[4], arguments[5], arguments[6]);
	else if(4 == argumentCount && 0 == ::strcmp(arguments[1], "--generate"))
		result = ::generateFile(arguments[2], ::strtoull(arguments[3], nullptr, 10));
	else
		::printUsage(arguments[0]);
	::socketShutdown();
	return result;
}
