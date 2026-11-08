#include "gpk_connection_ip.h"

#include <cstring>

using ::gpk::sc_t;

#if defined(GPK_WINDOWS)
tydf int socklen_t;
#endif

static ::gpk::err_t connectionReceive(::gpk::SSocketConnection & client, const ::gpk::SCommand & command, ::gpk::au0_t & packet) {
	::gpk::SPayloadHeader header = {};
	sockaddr_in remote = {};
	socklen_t remoteSize = (socklen_t)sizeof(remote);
	const int headerBytes = ::recvfrom(client.Socket.Handle, (::gpk::sc_t*)&header, (int)sizeof(header), MSG_PEEK, (sockaddr*)&remote, &remoteSize);
	if(headerBytes < 0) {
#if defined(GPK_WINDOWS)
		if(WSAGetLastError() != WSAEMSGSIZE) {
			error_printf("%s", "Could not receive payload header.");
			return -1;
		}
#endif
	}
	if(::gpk::ENDPOINT_COMMAND_TYPE_RESPONSE == command.Type) {
		if_fail_fe(packet.resize(sizeof(header)));
		::memcpy(packet.begin(), &header, sizeof(header));
		return 0;
	}
	if_fail_fe(packet.resize((::gpk::u2_t)sizeof(header) + header.Size));
	::memset(packet.begin(), 0, packet.size());
	remoteSize = (socklen_t)sizeof(remote);
	const int received = ::recvfrom(client.Socket.Handle, (::gpk::sc_t*)packet.begin(), (int)packet.size(), MSG_PEEK, (sockaddr*)&remote, &remoteSize);
	if(received < 0) {
#if defined(GPK_WINDOWS)
		if(WSAGetLastError() != WSAEMSGSIZE) {
			error_printf("%s", "Could not receive payload data.");
			return -1;
		}
		warning_printf("%s", "Failed to receive all of the payload data.");
#else
		error_printf("%s", "Failed to receive all of the payload data.");
		return -1;
#endif
	}
	else if(0 == received) {
		info_printf("The peer has performed an orderly shutdown: '%s'", "https://linux.die.net/man/3/recvfrom");
		return 1;
	}
	if_true_vef(-1, received != (int)packet.size(), "Packet size received doesn't match with header size. Received size: %u. Expected: %u.", received, packet.size());
	return 0;
}

::gpk::err_t gpk::connectionSendQueue(SConnection & connection, SSocketConnection & client, apobj<SMessage> & messageCacheSent, apobj<SMessage> & messageCacheSend) {
	const FConnectionSend send = [&client](const vcu0_t & frame) -> err_t {
		sockaddr_in remote = {};
		::gpk::tcpipAddressToSockaddr(client.Address, remote);
		const int sent = ::sendto(client.Socket.Handle, (const sc_t*)frame.begin(), (int)frame.size(), 0, (sockaddr*)&remote, sizeof(remote));
		return sent == (int)frame.size() ? 0 : -1;
	};
	return ::gpk::connectionSendQueue(connection, messageCacheSent, messageCacheSend, send);
}

::gpk::err_t gpk::connectionHandleCommand(SConnection & connection, SSocketConnection & client, SCommand & command, au0_t & receiveBuffer) {
	const FConnectionReceive receive = [&client, &command](au0_t & packet) -> err_t {
		return ::connectionReceive(client, command, packet);
	};
	const err_t result = ::gpk::connectionHandleCommand(connection, command, receive, receiveBuffer);
	if(0 == result && ENDPOINT_COMMAND_TYPE_REQUEST == command.Type && ENDPOINT_COMMAND_DISCONNECT == command.Command)
		client.Socket.close();
	return result;
}
