#include "gpk_http_client.h"
#include "gpk_parse.h"
#include "gpk_stdstring.h"
#include "gpk_enum.h"
#include "gpk_keyval_view.h"

#ifndef GPK_ATMEL		

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

GPK_USING_ASTATIC();
GPK_USING_APOD();
GPK_USING_VIEW();
GPK_USING_TYPEINT();
using gpk::string;

gpk::err_t 		gpk::urlDecode					(vcst_t urlToDecode, string & decoded)		{
	u2_t					decodedCharacters				= 0;
	for(u2_t iChar = 0; iChar < urlToDecode.size() - 2; ++iChar) {
		const sc_t					currentChar						= urlToDecode[iChar];
		if('%' != currentChar) {
			if_fail_fe(decoded.push_back((sc_t)(u0_t)currentChar));
			continue;
		}
		u3_t					decodedByte						= 0;
		if_fail_fe(gpk::parseIntegerHexadecimal({&urlToDecode[++iChar], 2}, decodedByte));
		if_fail_fe(decoded.push_back((sc_t)(u0_t)decodedByte));
		++iChar;
		++decodedCharacters;
	}
	return decodedCharacters;
}

static	gpk::err_t httpRequestChunkedJoin			(vcst_t body, string & joined)		{
	u2_t									iBegin							= 0;
	u2_t									iStop							= 0;
	while(iBegin < body.size()) {
		iBegin									= iStop;
		iStop									= (u2_t)::gpk::find('\n', body, iBegin);
		++iStop;	// skip \n
		::gpk::vcst_t					strSize;
		if(iStop <= body.size())
			strSize								= {&body[iBegin], (u2_t)iStop - iBegin};
		else
			break;
		u3_t									sizeChunk						= 0;
		::gpk::parseIntegerHexadecimal(strSize, sizeChunk);
		if(0 == sizeChunk)
			break;
		joined.append({&body[iStop], (u2_t)sizeChunk});
		iStop									+= (u2_t)sizeChunk;
		iStop									+= 2;	// skip \n
	}
	return 0;
}

static	gpk::err_t httpClientRequestConstruct
	(	::gpk::HTTP_METHOD	method
	,	vcst_t				hostName
	,	vcst_t				path
	,	vcst_t				contentType
	,	vcst_t				body
	,	string				& out_request
	) {
	vcst_t						strMethod				= ::gpk::get_value_label(method);
	out_request					= strMethod;
	out_request.push_back(' ');
	out_request.append(path);
	out_request.append(::gpk::vcst_t{
		" HTTP/1.1"
		//"\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)"
		"\r\nHost: "
		});
	out_request.append(hostName);
	if((method != ::gpk::HTTP_METHOD_GET) && contentType.size()) {
		out_request.append("\r\nContent-Type: ");
		out_request.append(contentType);// 		application/x-www-form-urlencoded
	}
	out_request.append(::gpk::vcst_t{
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
gpk::err_t			gpk::httpClientRequest
	(	const SIPv4End		& clientToConnect
	,	HTTP_METHOD			method
	,	vcst_t				hostName
	,	vcst_t				path
	,	vcst_t				contentType
	,	vcst_t				body
	,	SHTTPResponse		& out_received
	) {
	string				bytesRequest;
	if_fail_fe(::httpClientRequestConstruct(method, hostName, path, contentType, body, bytesRequest));
	const SIPv4End		& address						= clientToConnect;

	addrinfo					hints							= {};
    hints.ai_family			= AF_UNSPEC;
    hints.ai_socktype		= SOCK_STREAM;
	sc_t						addr[32]						= {};
	sc_t						port[32]						= {};
	snprintf(addr, ::gpk::size(addr) - 2, "%u.%u.%u.%u", GPK_IPV4_EXPAND_IP(address));
	snprintf(port, ::gpk::size(port) - 2, "%u", (0 == address.Port) ? 80 : address.Port);
	addrinfo					* servinfo						= 0;
    s2_t						rv								= getaddrinfo(addr, port, &hints, &servinfo);
#if defined(GPK_ESP32) || defined(GPK_ARDUINO)
	if_true_vef(-1, 0 != rv, "getaddrinfo: %i.", rv);
#else
	if_true_vef(-1, 0 != rv, "getaddrinfo: %i-%s.", rv, gai_strerror(rv));
#endif
	::gpk::auto_socket_close	sockfd;

    // loop through all the results and connect to the first we can
	addrinfo									* currentServinfo				= 0;
    for(currentServinfo = servinfo; currentServinfo != NULL; currentServinfo = currentServinfo->ai_next) {
        if_true_ce(INVALID_SOCKET == (sockfd.Handle = socket(currentServinfo->ai_family, currentServinfo->ai_socktype, currentServinfo->ai_protocol)));
        if(-1 == connect(sockfd, currentServinfo->ai_addr, (int)currentServinfo->ai_addrlen)) {
            sockfd.close();
            error_printf("%s", "client: connect");
            continue;
        }
        break;
    }
    if_true_vef(-1, 0 == currentServinfo, "failed to connect to server: %u.%u.%u.%u:%u", GPK_IPV4_EXPAND(address))
	else
		info_printf("Connected to server: %u.%u.%u.%u:%u", GPK_IPV4_EXPAND(address));

	sc_t										strAddress	[INET6_ADDRSTRLEN]	= {};
    inet_ntop(currentServinfo->ai_family, get_in_addr((struct sockaddr *)currentServinfo->ai_addr), strAddress, ::gpk::size(strAddress));
    freeaddrinfo(servinfo); // all done with this structure

    int											numbytes						= 0;
	info_printf("Sending request: \n%.*s", GPK_VIEW_ARGS(bytesRequest));
    if_fail_fef(numbytes = send(sockfd, bytesRequest.begin(), bytesRequest.size(), 0), "sockfd:%i", sockfd.Handle);

	::gpk::string						buf								= {};
	if_fail_fef(buf.resize(1024*1024*16, 0), "%s", "Failed to resize buffer. Out of memory?");
	u2_t									totalBytes						= 0;
	u2_t									stopOfHeader					= 0;
	u2_t									contentLength					= 0;
	bool										bChunked						= false;
	//const ::gpk::vcst_t				viewEndOfChunk					= "\r\n0\r\n\r\n";
    while(-1 != (numbytes = recv(sockfd, &buf[totalBytes], ::gpk::min(buf.size() - totalBytes - 1, 1U), 0)) && 0 != numbytes) {
		if_true_vef(-1, (totalBytes + numbytes) > buf.size(), "%s", "Rquest too big.");
		totalBytes								+= numbytes;
		if(0 == stopOfHeader && totalBytes >= 4) {
			gpk::err_t 						offsetStopOfHeader				= ::gpk::find_string("\r\n\r\n", {buf.begin(), totalBytes}, totalBytes - 4);
			if(0 <= offsetStopOfHeader) {
				// here we should do something in order to detect the content size
				stopOfHeader						= offsetStopOfHeader;
				info_printf("Header stop at position %u.", (u2_t)stopOfHeader);

				for(u2_t iByte = 0, sizeHeader = stopOfHeader; iByte < sizeHeader; ++iByte)
					buf[iByte]							= (s0_t)::tolower(buf[iByte]);

				bChunked							= 0 <= ::gpk::find_string("chunked", {buf.begin(), totalBytes});

				s2_t									offsetContentLength				= ::gpk::find_string("content-length", {buf.begin(), totalBytes});
				if(-1 == offsetContentLength) // no content
					continue;
				s2_t									offsetCristobal					= (u2_t)::gpk::find(':', {buf.begin(), totalBytes}, offsetContentLength);
				s2_t									offsetEndLine					= (u2_t)::gpk::find('\r', {buf.begin(), totalBytes}, offsetCristobal);
				if(0 > offsetEndLine)
					offsetEndLine						= (u2_t)::gpk::find('\n', {buf.begin(), totalBytes}, offsetCristobal);

				s2_t digitsLengthStart	= 0;
				s2_t digitsLengthStop	= offsetEndLine;
				for(s2_t iOffset = offsetCristobal + 1; iOffset < offsetEndLine; ++iOffset) {
					if(buf[iOffset] == ' ') {
						if(0 == digitsLengthStart)
							continue;
						else
							digitsLengthStop	= iOffset;
					}
					if(0 == digitsLengthStart)
						digitsLengthStart	= iOffset;
				}
				::gpk::vcst_t				strContentLength				= {&buf[digitsLengthStart], (u2_t)(digitsLengthStop - digitsLengthStart)};
				::gpk::parseIntegerDecimal(strContentLength, contentLength);
			}
		}
		//else if(stopOfHeader && totalBytes > stopOfHeader && bChunked) {
		//	//gpk::err_t 						offsetStopOfChunks				= ::gpk::find_string(viewEndOfChunk, {&buf[totalBytes - 7], totalBytes - 7});
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

	::gpk::vcst_t						contentReceived					= {};
	if(stopOfHeader >= buf.size() - 4) {
		info_printf("Fixed header %u stop to position %u.", (u2_t)stopOfHeader, buf.size());
		stopOfHeader							= buf.size();
	}

	for(u2_t iByte = 0, sizeHeader = stopOfHeader; iByte < sizeHeader; ++iByte)
		buf[iByte]								= (s0_t)::tolower(buf[iByte]);

	aobj<vcst_t>					headerLines;
	out_received.HeaderData		= ::gpk::vcst_t{buf.begin(), (u2_t)stopOfHeader};

	::gpk::split(::gpk::vcst_t{out_received.HeaderData}, '\n', headerLines);
	bChunked					= false;
	out_received.Headers.resize(headerLines.size());
	for(u2_t iLine = 0; iLine < headerLines.size(); ++iLine) {
		::gpk::kv<vcst_t>					& header						= out_received.Headers[iLine];
		if(-1 == ::gpk::token_split(':', headerLines[iLine], header)) {
			header.Key								= headerLines[iLine];
			//::gpk::view_char							strLine							= {(sc_t*)header.Key.begin(), header.Key.size()};
			//::gpk::tolower(strLine);
		}
		else {
			::gpk::string		strLine							= header.Key;
			::gpk::tolower(strLine);
			info_printf("\n%.*s", GPK_VIEW_ARGS(strLine));
			if(header.Key == "transfer-encoding") {
				if(0 <= ::gpk::find_string("chunked", header.Val))
					bChunked								= true;
			}
		}
	}

	if(stopOfHeader < buf.size())
		contentReceived							= {&buf[stopOfHeader], totalBytes - stopOfHeader};

	::gpk::string					joined;
	if(bChunked) {
		info_printf("\nChunked: %s", bChunked ? "true" : "false");
		::httpRequestChunkedJoin({&contentReceived[4], contentReceived.size()-4}, joined);
		contentReceived							= joined;
	}

	gpk::err_t 						offsetStopOfHeader				= ::gpk::find_string("\r\n\r\n", {contentReceived.begin(), contentReceived.size()});
	out_received.Body					= (0 == offsetStopOfHeader) ? ::gpk::vcst_t{contentReceived.begin() + 4, contentReceived.size() - 4} : contentReceived;
	return 0;
}

gpk::err_t 		gpk::httpClientRequest
	(	const SIPv4End	& clientToConnect
	,	HTTP_METHOD		method
	,	vcst_t			hostName
	,	vcst_t			path
	,	vcst_t			contentType
	,	vcst_t			body
	,	string			& out_received
	) {
	::gpk::SHTTPResponse						response;
	if_fail_fe(gpk::httpClientRequest
		( clientToConnect
		, method
		, hostName
		, path
		, contentType
		, body
		, response
		));
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

#endif