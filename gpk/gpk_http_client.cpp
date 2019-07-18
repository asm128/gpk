#include "gpk_http_client.h"
#include "gpk_find.h"
#include "gpk_parse.h"

#if defined(GPK_WINDOWS)
#	include <WS2tcpip.h>
#else
#	include <unistd.h>
#	include <netdb.h>
#	include <netinet/in.h>
#	include <sys/socket.h>
#	include <arpa/inet.h>
#endif

static	::gpk::error_t					httpRequestChunkedJoin			(const ::gpk::view_const_byte & body, ::gpk::array_pod<byte_t> & joined)		{
	uint32_t									iBegin							= 0;
	uint32_t									iStop							= 0;
	while(iBegin < (int32_t)body.size()) {
		iBegin									= iStop;
		iStop									= (uint32_t)::gpk::find('\n', body, iBegin);
		++iStop;	// skip \n
		::gpk::view_const_string					strSize;
		if(iStop <= body.size())
			strSize								= {&body[iBegin], (uint32_t)iStop - iBegin};
		else 
			break;
		uint64_t									sizeChunk						= 0;
		::gpk::parseArbitraryBaseInteger(16, "0123456789abcdef", strSize, &sizeChunk);
		if(0 == sizeChunk)
			break;
		joined.append(::gpk::view_const_byte{&body[iStop], (uint32_t)sizeChunk});
		iStop									+= (uint32_t)sizeChunk;
		iStop									+= 2;	// skip \n
	}
	return 0;
}

static	::gpk::error_t					httpClientRequestConstruct
	(	::gpk::HTTP_METHOD					method
	,	const ::gpk::view_const_string		& hostName
	,	const ::gpk::view_const_string		& path
	,	const ::gpk::view_const_string		& contentType
	,	const ::gpk::view_const_byte		& body
	,	::gpk::array_pod<byte_t>			& out_request
	) {
	::gpk::view_const_char						strMethod						= ::gpk::get_value_label(method);
	out_request								= strMethod;
	out_request.push_back(' ');
	out_request.append(path);
	out_request.append(::gpk::view_const_string{
		" HTTP/1.1"
		//"\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)"
		"\r\nHost: "
		});
	out_request.append(hostName);
	if((method != ::gpk::HTTP_METHOD_GET) && contentType.size()) {
		out_request.append(::gpk::view_const_string{"\r\nContent-Type: "});
		out_request.append(contentType);// 		application/x-www-form-urlencoded
	}
	out_request.append(::gpk::view_const_string{
		"\r\nAccept-Language: en-us"
		//"\r\nAccept-Encoding: gzip, deflate"
		//"\r\nConnection: Keep-Alive"
		"\r\n"
		"\r\n"
		});
	out_request.append(body);
	return 0;
}

// get sockaddr, IPv4 or IPv6:
void *									get_in_addr						(sockaddr *sa)			{ return (sa->sa_family == AF_INET) ? &(((struct sockaddr_in*)sa)->sin_addr) : (void*)&(((struct sockaddr_in6*)sa)->sin6_addr); }
::gpk::error_t							gpk::httpClientRequest			
	(	const ::gpk::SIPv4					& clientToConnect
	,	::gpk::HTTP_METHOD					method
	,	const ::gpk::view_const_string		& hostName
	,	const ::gpk::view_const_string		& path
	,	const ::gpk::view_const_string		& contentType
	,	const ::gpk::view_const_byte		& body
	,	::gpk::array_pod<byte_t>			& out_received
	) {
	::gpk::array_pod<byte_t>					bytesRequest;
	gpk_necall(::httpClientRequestConstruct(method, hostName, path, contentType, body, bytesRequest), "%s", "unknown error");
	const ::gpk::SIPv4							& address						= clientToConnect;

	addrinfo									hints							= {};
    hints.ai_family							= AF_UNSPEC;
    hints.ai_socktype						= SOCK_STREAM;
	char										addr[32]						= {};
	char										port[32]						= {};
	sprintf_s(addr, "%u.%u.%u.%u", GPK_IPV4_EXPAND_IP(address));
	sprintf_s(port, "%u", address.Port);
	addrinfo									* servinfo						= 0;
    int32_t										rv								= getaddrinfo(addr, port, &hints, &servinfo);
	ree_if(0 != rv, "getaddrinfo: %s", gai_strerror(rv));
	::gpk::auto_socket_close					sockfd;

    // loop through all the results and connect to the first we can
	addrinfo									* currentServinfo				= 0;
    for(currentServinfo = servinfo; currentServinfo != NULL; currentServinfo = currentServinfo->ai_next) {
        ce_if(INVALID_SOCKET == (sockfd.Handle = socket(currentServinfo->ai_family, currentServinfo->ai_socktype, currentServinfo->ai_protocol)), "%s", "client: socket");
        if(-1 == connect(sockfd, currentServinfo->ai_addr, (int)currentServinfo->ai_addrlen)) {
            gpk_safe_closesocket(sockfd.Handle);
            error_printf("%s", "client: connect");
            continue;
        }
        break;
    }
    ree_if(0 == currentServinfo, "failed to connect to server: %u.%u.%u.%u:%u", GPK_IPV4_EXPAND(address));

	char										strAddress	[INET6_ADDRSTRLEN]	= {};
    inet_ntop(currentServinfo->ai_family, get_in_addr((struct sockaddr *)currentServinfo->ai_addr), strAddress, ::gpk::size(strAddress));
    printf("client: connecting to %s\n", strAddress);
    freeaddrinfo(servinfo); // all done with this structure

    int											numbytes						= 0;  
    gpk_necall(numbytes = send(sockfd, bytesRequest.begin(), bytesRequest.size(), 0), "%s", "Failed to send request.");

	::gpk::array_pod<char>						buf								= {};
	buf.resize(1024*1024*64);
	uint32_t									totalBytes						= 0;
    while(-1 != (numbytes = recv(sockfd, &buf[totalBytes], buf.size() - totalBytes - 1, 0)) && 0 != numbytes) {
		totalBytes								+= numbytes;
		continue;
	}
	if(totalBytes < buf.size()) {
		buf[totalBytes]							= '\0';
		buf.resize(totalBytes);
	}

	uint32_t									stopOfHeader					= (uint32_t)::gpk::find_sequence_pod(::gpk::view_const_string{"\r\n\r\n"}, {buf.begin(), buf.size()});
	::gpk::view_const_byte						httpheaderReceived				= buf;
	::gpk::view_const_byte						contentReceived					= {};
	if(stopOfHeader >= buf.size() - 4) 
		stopOfHeader							= buf.size();
	info_printf("Header stop at position %u.", (uint32_t)stopOfHeader);

	for(uint32_t iByte = 0, sizeHeader = stopOfHeader; iByte < sizeHeader; ++iByte)
		buf[iByte]								= (byte_t)::tolower(buf[iByte]);

	::gpk::array_obj<::gpk::view_const_byte>	headerLines;
	httpheaderReceived						= {buf.begin(), (uint32_t)stopOfHeader};
	::gpk::split(httpheaderReceived, '\n', headerLines);
	bool										bChunked						= false;
	for(uint32_t iLine = 0; iLine < headerLines.size(); ++iLine) {
		::gpk::array_pod<char_t>					strLine							= headerLines[iLine];
		strLine.push_back(0);
		info_printf("\n%s", strLine.begin());
		if(0 <= ::gpk::find_sequence_pod(::gpk::view_const_string{"chunked"}, ::gpk::view_const_char{strLine})) 
			bChunked								= true;
	}

	if(stopOfHeader + 4 < buf.size())
		contentReceived							= {&buf[stopOfHeader + 4], buf.size() - stopOfHeader + 4};

	::gpk::array_pod<byte_t>					joined;
	if(bChunked) {
		info_printf("\nChunked: %s", bChunked ? "true" : "false");
		::httpRequestChunkedJoin(contentReceived, joined);
		contentReceived							= joined;
	}

	if(contentReceived.size()) 
		out_received							= contentReceived;
	return 0;
}

// GET /hello.htm HTTP/1.1
// User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)
// Host: www.tutorialspoint.com
// Accept-Language: en-us
// Accept-Encoding: gzip, deflate
// Connection: Keep-Alive
// Here we are not sending any request data to the server because we are fetching a plain HTML page from the server. Connection is a general-header, and the rest of the headers are request headers. The following example shows how to send form data to the server using request message body:
// 
// POST /cgi-bin/process.cgi HTTP/1.1
// User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)
// Host: www.tutorialspoint.com
// Content-Type: application/x-www-form-urlencoded
// Content-Length: length
// Accept-Language: en-us
// Accept-Encoding: gzip, deflate
// Connection: Keep-Alive
// 
// licenseID=string&content=string&/paramsXML=string
// Here the given URL /cgi-bin/process.cgi will be used to process the passed data and accordingly, a response will be returned. Here content-type tells the server that the passed data is a simple web form data and length will be the actual length of the data put in the message body. The following example shows how you can pass plain XML to your web server:
// 
// POST /cgi-bin/process.cgi HTTP/1.1
// User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)
// Host: www.tutorialspoint.com
// Content-Type: text/xml; charset=utf-8
// Content-Length: length
// Accept-Language: en-us
// Accept-Encoding: gzip, deflate
// Connection: Keep-Alive
// 
// <?xml version="1.0" encoding="utf-8"?>
// <string xmlns="http://clearforest.com/">string</string>
