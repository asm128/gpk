#include "gpk_http_client.h"

::gpk::error_t					httpClientRequestConstruct
	(	::gpk::HTTP_METHOD				method
	,	const ::gpk::view_const_string	& hostName
	,	const ::gpk::view_const_string	& path
	,	const ::gpk::view_const_string	& contentType
	,	const ::gpk::view_const_byte	& body
	,	::gpk::array_pod<byte_t>		& out_request
	) {
	::gpk::view_const_char				strMethod					= ::gpk::get_value_label(method);
	out_request						= strMethod;
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
::gpk::error_t					httpClientRequest			
	(	::gpk::SHTTPClient				& clientToConnect
	,	::gpk::HTTP_METHOD				method
	,	const ::gpk::view_const_string	& hostName
	,	const ::gpk::view_const_string	& path
	,	const ::gpk::view_const_string	& contentType
	,	const ::gpk::view_const_byte	& body
	,	::gpk::array_pod<byte_t>		& out_received
	) {
	::gpk::array_pod<byte_t>			bytesRequest;
	::httpClientRequestConstruct(method, hostName, path, contentType, body, bytesRequest);
	(void)clientToConnect;
	(void)out_received;
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