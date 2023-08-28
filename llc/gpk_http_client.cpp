#include "gpk_http_client.h"
#include "gpk_parse.h"
#include "gpk_stdstring.h"

#ifdef GPK_WINDOWS
#	include <WS2tcpip.h>
#else
#	include <unistd.h>
#	include <netdb.h>
#	include <netinet/in.h>
#	include <sys/socket.h>
#	include <arpa/inet.h>
#	include <ctype.h>
#endif

::gpk::error_t			gpk::urlDecode					(::gpk::vcc urlToDecode, ::gpk::apod<char> & decoded)		{
	uint32_t					decodedCharacters				= 0;
	for(uint32_t iChar = 0; iChar < urlToDecode.size() - 2; ++iChar) {
		const char					currentChar						= urlToDecode[iChar];
		if('%' != currentChar) {
			gpk_necs(decoded.push_back((char)(uint8_t)currentChar));
			continue;
		}
		uint64_t					decodedByte						= 0;
		gpk_necs(::gpk::parseIntegerHexadecimal({&urlToDecode[++iChar], 2}, decodedByte));
		gpk_necs(decoded.push_back((char)(uint8_t)decodedByte));
		++iChar;
		++decodedCharacters;
	}
	return decodedCharacters;
}

static	::gpk::error_t	httpRequestChunkedJoin			(const ::gpk::vcc & body, ::gpk::apod<char> & joined)		{
	uint32_t									iBegin							= 0;
	uint32_t									iStop							= 0;
	while(iBegin < body.size()) {
		iBegin									= iStop;
		iStop									= (uint32_t)::gpk::find('\n', body, iBegin);
		++iStop;	// skip \n
		::gpk::vcs					strSize;
		if(iStop <= body.size())
			strSize								= {&body[iBegin], (uint32_t)iStop - iBegin};
		else
			break;
		uint64_t									sizeChunk						= 0;
		::gpk::parseIntegerHexadecimal(strSize, sizeChunk);
		if(0 == sizeChunk)
			break;
		joined.append({&body[iStop], (uint32_t)sizeChunk});
		iStop									+= (uint32_t)sizeChunk;
		iStop									+= 2;	// skip \n
	}
	return 0;
}

static	::gpk::error_t	httpClientRequestConstruct
	(	::gpk::HTTP_METHOD		method
	,	const ::gpk::vcs		& hostName
	,	const ::gpk::vcs		& path
	,	const ::gpk::vcs		& contentType
	,	const ::gpk::vcc		& body
	,	::gpk::apod<char>		& out_request
	) {
	::gpk::vcc						strMethod						= ::gpk::get_value_label(method);
	out_request					= strMethod;
	out_request.push_back(' ');
	out_request.append(path);
	out_request.append(::gpk::vcs{
		" HTTP/1.1"
		//"\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)"
		"\r\nHost: "
		});
	out_request.append(hostName);
	if((method != ::gpk::HTTP_METHOD_GET) && contentType.size()) {
		out_request.append(::gpk::vcs{"\r\nContent-Type: "});
		out_request.append(contentType);// 		application/x-www-form-urlencoded
	}
	out_request.append(::gpk::vcs{
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
void *					get_in_addr						(sockaddr *sa)			{ return (sa->sa_family == AF_INET) ? &(((struct sockaddr_in*)sa)->sin_addr) : (void*)&(((struct sockaddr_in6*)sa)->sin6_addr); }
::gpk::error_t			gpk::httpClientRequest
	(	const ::gpk::SIPv4		& clientToConnect
	,	::gpk::HTTP_METHOD		method
	,	const ::gpk::vcs		& hostName
	,	const ::gpk::vcs		& path
	,	const ::gpk::vcs		& contentType
	,	const ::gpk::vcc		& body
	,	::gpk::SHTTPResponse	& out_received
	) {
	::gpk::achar				bytesRequest;
	gpk_necall(::httpClientRequestConstruct(method, hostName, path, contentType, body, bytesRequest), "%s", "unknown error");
	const ::gpk::SIPv4							& address						= clientToConnect;

	addrinfo									hints							= {};
    hints.ai_family							= AF_UNSPEC;
    hints.ai_socktype						= SOCK_STREAM;
	char										addr[32]						= {};
	char										port[32]						= {};
	snprintf(addr, ::gpk::size(addr) - 2, "%u.%u.%u.%u", GPK_IPV4_EXPAND_IP(address));
	snprintf(port, ::gpk::size(port) - 2, "%u", (0 == address.Port) ? 80 : address.Port);
	addrinfo									* servinfo						= 0;
    int32_t										rv								= getaddrinfo(addr, port, &hints, &servinfo);
#if defined(GPK_ESP32) || defined(GPK_ARDUINO)
	ree_if(0 != rv, "getaddrinfo: %i.", rv);
#else
	ree_if(0 != rv, "getaddrinfo: %i-%s.", rv, gai_strerror(rv));
#endif
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
    ree_if(0 == currentServinfo, "failed to connect to server: %u.%u.%u.%u:%u", GPK_IPV4_EXPAND(address))
	else
		info_printf("Connected to server: %u.%u.%u.%u:%u", GPK_IPV4_EXPAND(address));

	char										strAddress	[INET6_ADDRSTRLEN]	= {};
    inet_ntop(currentServinfo->ai_family, get_in_addr((struct sockaddr *)currentServinfo->ai_addr), strAddress, ::gpk::size(strAddress));
    freeaddrinfo(servinfo); // all done with this structure

    int											numbytes						= 0;
	info_printf("Sending request: \n%s", ::gpk::toString(bytesRequest).begin());
    gpk_necall(numbytes = send(sockfd, bytesRequest.begin(), bytesRequest.size(), 0), "%s", "Failed to send request.");

	::gpk::apod<char>						buf								= {};
	gpk_necall(buf.resize(1024*1024*16, 0), "%s", "Failed to resize buffer. Out of memory?");
	uint32_t									totalBytes						= 0;
	uint32_t									stopOfHeader					= 0;
	uint32_t									contentLength					= 0;
	bool										bChunked						= false;
	//const ::gpk::vcs				viewEndOfChunk					= "\r\n0\r\n\r\n";
    while(-1 != (numbytes = recv(sockfd, &buf[totalBytes], ::gpk::min(buf.size() - totalBytes - 1, 1U), 0)) && 0 != numbytes) {
		ree_if((totalBytes + numbytes) > buf.size(), "%s", "Rquest too big.");
		totalBytes								+= numbytes;
		if(0 == stopOfHeader && totalBytes >= 4) {
			::gpk::error_t							offsetStopOfHeader				= ::gpk::find_string("\r\n\r\n", {buf.begin(), totalBytes}, totalBytes - 4);
			if(0 <= offsetStopOfHeader) {
				// here we should do something in order to detect the content size
				stopOfHeader						= offsetStopOfHeader;
				info_printf("Header stop at position %u.", (uint32_t)stopOfHeader);

				for(uint32_t iByte = 0, sizeHeader = stopOfHeader; iByte < sizeHeader; ++iByte)
					buf[iByte]							= (int8_t)::tolower(buf[iByte]);

				bChunked							= 0 <= ::gpk::find_sequence_pod(::gpk::vcs{"chunked"}, {buf.begin(), totalBytes});

				int32_t									offsetContentLength				= ::gpk::find_string("content-length", {buf.begin(), totalBytes});
				if(-1 == offsetContentLength) // no content
					continue;
				int32_t									offsetCristobal					= (uint32_t)::gpk::find(':', {buf.begin(), totalBytes}, offsetContentLength);
				int32_t									offsetEndLine					= (uint32_t)::gpk::find('\r', {buf.begin(), totalBytes}, offsetCristobal);
				if(0 > offsetEndLine)
					offsetEndLine						= (uint32_t)::gpk::find('\n', {buf.begin(), totalBytes}, offsetCristobal);

				int32_t digitsLengthStart	= 0;
				int32_t digitsLengthStop	= offsetEndLine;
				for(int32_t iOffset = offsetCristobal + 1; iOffset < offsetEndLine; ++iOffset) {
					if(buf[iOffset] == ' ') {
						if(0 == digitsLengthStart)
							continue;
						else
							digitsLengthStop	= iOffset;
					}
					if(0 == digitsLengthStart)
						digitsLengthStart	= iOffset;
				}
				::gpk::vcs				strContentLength				= {&buf[digitsLengthStart], (uint32_t)(digitsLengthStop - digitsLengthStart)};
				::gpk::parseIntegerDecimal(strContentLength, contentLength);
			}
		}
		//else if(stopOfHeader && totalBytes > stopOfHeader && bChunked) {
		//	//::gpk::error_t							offsetStopOfChunks				= ::gpk::find_sequence_pod(viewEndOfChunk, {&buf[totalBytes - 7], totalBytes - 7});
		//	//if(0 == offsetStopOfChunks)
		//	//	break;
		//}
		if(contentLength != 0 && contentLength + stopOfHeader == totalBytes)
			break;
		continue;
	}
	if(totalBytes < buf.size()) {
		buf[totalBytes]							= '\0';
		buf.resize(totalBytes);	// this doesn't fail
#if defined(GPK_WINDOWS)
		OutputDebugString(buf.begin());
#endif
	}

	::gpk::vcc						contentReceived					= {};
	if(stopOfHeader >= buf.size() - 4) {
		info_printf("Fixed header %u stop to position %u.", (uint32_t)stopOfHeader, buf.size());
		stopOfHeader							= buf.size();
	}

	for(uint32_t iByte = 0, sizeHeader = stopOfHeader; iByte < sizeHeader; ++iByte)
		buf[iByte]								= (int8_t)::tolower(buf[iByte]);

	::gpk::aobj<::gpk::vcc>			headerLines;
	out_received.HeaderData					= {buf.begin(), (uint32_t)stopOfHeader};

	::gpk::split(::gpk::vcc{out_received.HeaderData}, '\n', headerLines);
	bChunked						= false;
	out_received.Headers.resize(headerLines.size());
	for(uint32_t iLine = 0; iLine < headerLines.size(); ++iLine) {
		::gpk::TKeyValConstString					& header						= out_received.Headers[iLine];
		if(-1 == ::gpk::token_split(':', headerLines[iLine], header)) {
			header.Key								= {headerLines[iLine].begin(), headerLines[iLine].size()};
			//::gpk::view_char							strLine							= {(char*)header.Key.begin(), header.Key.size()};
			//::gpk::tolower(strLine);
		}
		else {
			::gpk::view<char>							strLine							= {(char*)header.Key.begin(), header.Key.size()};
			::gpk::tolower(strLine);
			info_printf("\n%s", ::gpk::toString(strLine).begin());
			if(::gpk::vcs{"transfer-encoding"} == header.Key) {
				if(0 <= ::gpk::find_sequence_pod(::gpk::vcs{"chunked"}, header.Val))
					bChunked								= true;
			}
		}
	}

	if(stopOfHeader < buf.size())
		contentReceived							= {&buf[stopOfHeader], totalBytes - stopOfHeader};

	::gpk::apod<char>					joined;
	if(bChunked) {
		info_printf("\nChunked: %s", bChunked ? "true" : "false");
		::httpRequestChunkedJoin({&contentReceived[4], contentReceived.size()-4}, joined);
		contentReceived							= joined;
	}

	::gpk::error_t							offsetStopOfHeader				= ::gpk::find_sequence_pod(::gpk::vcs{"\r\n\r\n"}, {contentReceived.begin(), contentReceived.size()});
	out_received.Body					= (0 == offsetStopOfHeader) ? ::gpk::vcc{contentReceived.begin() + 4, contentReceived.size() - 4} : contentReceived;
	return 0;
}

::gpk::error_t			gpk::httpClientRequest
	(	const ::gpk::SIPv4	& clientToConnect
	,	::gpk::HTTP_METHOD	method
	,	const ::gpk::vcs	& hostName
	,	const ::gpk::vcs	& path
	,	const ::gpk::vcs	& contentType
	,	const ::gpk::vcc	& body
	,	::gpk::apod<char>	& out_received
	) {
	::gpk::SHTTPResponse						response;
	gpk_necall(::gpk::httpClientRequest
		( clientToConnect
		, method
		, hostName
		, path
		, contentType
		, body
		, response
		), "%s", "Request failed.");
	out_received							= response.Body;
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
