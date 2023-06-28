#include "gpk_enum.h"

#ifndef GPK_HTTP_H_23627
#define GPK_HTTP_H_23627

namespace gpk
{
	GDEFINE_ENUM_TYPE(HTTP_METHOD, int8_t);
	GDEFINE_ENUM_VALUE(HTTP_METHOD, UNKNOWN		, 0);	// ???
	GDEFINE_ENUM_VALUE(HTTP_METHOD, GET			, 1);	// The GET method is used to retrieve information from the given server using a given URI. Requests using GET should only retrieve data and should have no other effect on the data.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, HEAD		, 2);	// Same as GET, but transfers the status line and header section only.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, POST		, 3);	// A POST request is used to send data to the server, for example, customer information, file upload, etc. using HTML forms.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, PUT			, 4);	// Replaces all current representations of the target resource with the uploaded content.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, DELETE		, 5);	// Removes all current representations of the target resource given by a URI.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, CONNECT		, 6);	// Establishes a tunnel to the server identified by a given URI.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, OPTIONS		, 7);	// Describes the communication options for the target resource.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, TRACE		, 8);	// Performs a message loop-back test along the path to the target resource.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, PATCH		, 9);	// The PATCH method is used to apply partial modifications to a resource.
	GDEFINE_ENUM_VALUE(HTTP_METHOD, COPY		,10);	//
	GDEFINE_ENUM_VALUE(HTTP_METHOD, LINK		,11);	//
	GDEFINE_ENUM_VALUE(HTTP_METHOD, UNLINK		,12);	//
	GDEFINE_ENUM_VALUE(HTTP_METHOD, LOCK		,13);	//
	GDEFINE_ENUM_VALUE(HTTP_METHOD, UNLOCK		,14);	//
	GDEFINE_ENUM_VALUE(HTTP_METHOD, PROPFIND	,15);	//
	GDEFINE_ENUM_VALUE(HTTP_METHOD, VIEW		,16);	//
	GDEFINE_ENUM_VALUE(HTTP_METHOD, PURGE		,17);	//

	GDEFINE_ENUM_TYPE(HTTP_STATUS, uint16_t);
	// 1xx Informational response			An informational response indicates that the request was received and understood. It is issued on a provisional basis while request processing continues. It alerts the client to wait for a final response. The message consists only of the status line and optional header fields, and is terminated by an empty line. As the HTTP/1.0 standard did not define any 1xx status codes, servers must not[note 1] send a 1xx response to an HTTP/1.0 compliant client except under experimental conditions.[4]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, UNKNOWN									, 0);	// ???
	GDEFINE_ENUM_VALUE(HTTP_STATUS, CONTINUE								, 100);		// The server, has received the request headers and the client should proceed to send the request body (in the case of a request for which a body needs to be sent; for example, a POST request). Sending a large request body to a server after a request has been rejected for inappropriate headers would be inefficient. To have a server check the request's headers, a client must send Expect: 100-continue as a header in its initial request and receive a 100 Continue status code in response before sending the body. If the client receives an error code such as 403 (Forbidden) or 405 (Method Not Allowed) then it shouldn't send the request's body. The response 417 Expectation Failed indicates that the request should be repeated without the Expect header as it indicates that the server doesn't support expectations (this is the case, for example, of HTTP/1.0 servers).[5]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, SWITCHING_PROTOCOLS						, 101);		// The requester has asked the server to switch protocols and the server has agreed to do so.[6]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, PROCESSING								, 102);		// (WebDAV; RFC 2518)	A WebDAV request may contain many sub-requests involving file operations, requiring a long time to complete the request. This code indicates that the server has received and is processing the request, but no response is available yet.[7] This prevents the client from timing out and assuming the request was lost.
	GDEFINE_ENUM_VALUE(HTTP_STATUS, EARLY_HINTS								, 103);		// (RFC 8297)			Used to return some response headers before final HTTP message.[8]

	//_2XX_SUCCESS						, 						This class of status codes indicates the action requested by the client was received, understood and accepted.[2]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, OK										, 200);	// Standard response for successful HTTP requests. The actual response will depend on the request method used. In a GET request, the response will contain an entity corresponding to the requested resource. In a POST request, the response will contain an entity describing or containing the result of the action.[9]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, CREATED									, 201);	// The request has been fulfilled, resulting in the creation of a new resource.[10]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, ACCEPTED								, 202);	// The request has been accepted for processing, but the processing has not been completed. The request might or might not be eventually acted upon, and may be disallowed when processing occurs.[11]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, NON_AUTHORITATIVE_INFORMATION			, 203);	// (since HTTP/1.1) The server is a transforming proxy (e.g. a Web accelerator) that received a 200 OK from its origin, but is returning a modified version of the origin's response.[12][13]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, NO_CONTENT								, 204);	// The server successfully processed the request and is not returning any content.[14]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, RESET_CONTENT							, 205);	// The server successfully processed the request, but is not returning any content. Unlike a 204 response, this response requires that the requester reset the document view.[15]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, PARTIAL_CONTENT							, 206);	// (RFC 7233) The server is delivering only part of the resource (byte serving) due to a range header sent by the client. The range header is used by HTTP clients to enable resuming of interrupted downloads, or split a download into multiple simultaneous streams.[16]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, MULTI_STATUS							, 207);	// (WebDAV; RFC 4918) The message body that follows is by default an XML message and can contain a number of separate response codes, depending on how many sub-requests were made.[17]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, ALREADY_REPORTED						, 208);	// (WebDAV; RFC 5842) The members of a DAV binding have already been enumerated in a preceding part of the (multistatus) response, and are not being included again.
	GDEFINE_ENUM_VALUE(HTTP_STATUS, IM_USED									, 226);	// (RFC 3229) The server has fulfilled a request for the resource, and the response is a representation of the result of one or more instance-manipulations applied to the current instance.[18]

	//_3XX_REDIRECTION						 					This class of status code indicates the client must take additional action to complete the request. Many of these status codes are used in URL redirection.[2]
	//										 					A user agent may carry out the additional action with no user interaction only if the method used in the second request is GET or HEAD. A user agent may automatically redirect a request. A user agent should detect and intervene to prevent cyclical redirects.[19]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, MULTIPLE_CHOICES						, 300);		// Indicates multiple options for the resource from which the client may choose (via agent-driven content negotiation). For example, this code could be used to present multiple video format options, to list files with different filename extensions, or to suggest word-sense disambiguation.[20]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, MOVED_PERMANENTLY						, 301);		// This and all future requests should be directed to the given URI.[21]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, FOUND									, 302);		// (Previously "Moved temporarily") Tells the client to look at (browse to) another URL. 302 has been superseded by 303 and 307. This is an example of industry practice contradicting the standard. The HTTP/1.0 specification (RFC 1945) required the client to perform a temporary redirect (the original describing phrase was "Moved Temporarily"),[22] but popular browsers implemented 302 with the functionality of a 303 See Other. Therefore, HTTP/1.1 added status codes 303 and 307 to distinguish between the two behaviours.[23] However, some Web applications and frameworks use the 302 status code as if it were the 303.[24]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, SEE_OTHER								, 303);		// (since HTTP/1.1) The response to the request can be found under another URI using the GET method. When received in response to a POST (or PUT/DELETE), the client should presume that the server has received the data and should issue a new GET request to the given URI.[25]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, NOT_MODIFIED							, 304);		// (RFC 7232) Indicates that the resource has not been modified since the version specified by the request headers If-Modified-Since or If-None-Match. In such case, there is no need to retransmit the resource since the client still has a previously-downloaded copy.[26]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, USE_PROXY								, 305);		// (since HTTP/1.1) The requested resource is available only through a proxy, the address for which is provided in the response. For security reasons, many HTTP clients (such as Mozilla Firefox and Internet Explorer) do not obey this status code.[27]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, SWITCH_PROXY							, 306);		// No longer used. Originally meant "Subsequent requests should use the specified proxy."[28]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, TEMPORARY_REDIRECT						, 307);		// (since HTTP/1.1) In this case, the request should be repeated with another URI; however, future requests should still use the original URI. In contrast to how 302 was historically implemented, the request method is not allowed to be changed when reissuing the original request. For example, a POST request should be repeated using another POST request.[29]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, PERMANENT_REDIRECT						, 308);		// (RFC 7538) The request and all future requests should be repeated using another URI. 307 and 308 parallel the behaviors of 302 and 301, but do not allow the HTTP method to change. So, for example, submitting a form to a permanently redirected resource may continue smoothly.[30]

	//_4XX_CLIENT_ERRORS					 					This class of status code is intended for situations in which the error seems to have been caused by the client. Except when responding to a HEAD request, the server should include an entity containing an explanation of the error situation, and whether it is a temporary or permanent condition. These status codes are applicable to any request method. User agents should display any included entity to the user.
	GDEFINE_ENUM_VALUE(HTTP_STATUS, BAD_REQUEST								, 400);		// The server cannot or will not process the request due to an apparent client error (e.g., malformed request syntax, size too large, invalid request message framing, or deceptive request routing).[32]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, UNAUTHORIZED							, 401);		// (RFC 7235) Similar to 403 Forbidden, but specifically for use when authentication is required and has failed or has not yet been provided. The response must include a WWW-Authenticate header field containing a challenge applicable to the requested resource. See Basic access authentication and Digest access authentication.[33] 401 semantically means "unauthorised",[34] the user does not have valid authentication credentials for the target resource.
	// Note: Some sites incorrectly issue HTTP 401 when an IP address is banned from the website (usually the website domain) and that specific address is refused permission to access a website.[citation needed]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, PAYMENT_REQUIRED						, 402);		// Reserved for future use. The original intention was that this code might be used as part of some form of digital cash or micropayment scheme, as proposed, for example, by GNU Taler,[35] but that has not yet happened, and this code is not usually used. Google Developers API uses this status if a particular developer has exceeded the daily limit on requests.[36] Sipgate uses this code if an account does not have sufficient funds to start a call.[37] Shopify uses this code when the store has not paid their fees and is temporarily disabled.[38]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, FORBIDDEN								, 403);		// The request contained valid data and was understood by the server, but the server is refusing action. This may be due to the user not having the necessary permissions for a resource or needing an account of some sort, or attempting a prohibited action (e.g. creating a duplicate record where only one is allowed). This code is also typically used if the request provided authentication via the WWW-Authenticate header field, but the server did not accept that authentication. The request SHOULD NOT be repeated.
	GDEFINE_ENUM_VALUE(HTTP_STATUS, NOT_FOUND								, 404);		// The requested resource could not be found but may be available in the future. Subsequent requests by the client are permissible.
	GDEFINE_ENUM_VALUE(HTTP_STATUS, STATUS_NOT_ALLOWED						, 405);		// A request method is not supported for the requested resource; for example, a GET request on a form that requires data to be presented via POST, or a PUT request on a read-only resource.
	GDEFINE_ENUM_VALUE(HTTP_STATUS, NOT_ACCEPTABLE							, 406);		// The requested resource is capable of generating only content not acceptable according to the Accept headers sent in the request.[39] See Content negotiation.
	GDEFINE_ENUM_VALUE(HTTP_STATUS, PROXY_AUTHENTICATION_REQUIRED			, 407);		// (RFC 7235) The client must first authenticate itself with the proxy.[40]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, REQUEST_TIMEOUT							, 408);		// The server timed out waiting for the request. According to HTTP specifications: "The client did not produce a request within the time that the server was prepared to wait. The client MAY repeat the request without modifications at any later time."[41]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, CONFLICT								, 409);		// Indicates that the request could not be processed because of conflict in the current state of the resource, such as an edit conflict between multiple simultaneous updates.
	GDEFINE_ENUM_VALUE(HTTP_STATUS, GONE									, 410);		// Indicates that the resource requested is no longer available and will not be available again. This should be used when a resource has been intentionally removed and the resource should be purged. Upon receiving a 410 status code, the client should not request the resource in the future. Clients such as search engines should remove the resource from their indices.[42] Most use cases do not require clients and search engines to purge the resource, and a "404 Not Found" may be used instead.
	GDEFINE_ENUM_VALUE(HTTP_STATUS, LENGTH_REQUIRED							, 411);		// The request did not specify the length of its content, which is required by the requested resource.[43]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, PRECONDITION_FAILED						, 412);		// (RFC 7232) The server does not meet one of the preconditions that the requester put on the request header fields.[44][45]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, PAYLOAD_TOO_LARGE						, 413);		// (RFC 7231) The request is larger than the server is willing or able to process. Previously called "Request Entity Too Large".[46]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, URI_TOO_LONG							, 414);		// (RFC 7231) The URI provided was too long for the server to process. Often the result of too much data being encoded as a query-string of a GET request, in which case it should be converted to a POST request.[47] Called "Request-URI Too Long" previously.[48]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, UNSUPPORTED_MEDIA_TYPE					, 415);		// (RFC 7231) The request entity has a media type which the server or resource does not support. For example, the client uploads an image as image/svg+xml, but the server requires that images use a different format.[49]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, RANGE_NOT_SATISFIABLE					, 416);		// (RFC 7233) The client has asked for a portion of the file (byte serving), but the server cannot supply that portion. For example, if the client asked for a part of the file that lies beyond the end of the file.[50] Called "Requested Range Not Satisfiable" previously.[51]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, EXPECTATION_FAILED						, 417);		// The server cannot meet the requirements of the Expect request-header field.[52]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, IM_A_TEAPOT								, 418);		// (RFC 2324, RFC 7168) This code was defined in 1998 as one of the traditional IETF April Fools' jokes, in RFC 2324, Hyper Text Coffee Pot Control Protocol, and is not expected to be implemented by actual HTTP servers. The RFC specifies this code should be returned by teapots requested to brew coffee.[53] This HTTP status is used as an Easter egg in some websites, including Google.com.[54][55]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, MISDIRECTED_REQUEST						, 421);		// (RFC 7540) The request was directed at a server that is not able to produce a response[56] (for example because of connection reuse).[57]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, UNPROCESSABLE_ENTITY					, 422);		// (WebDAV; RFC 4918) The request was well-formed but was unable to be followed due to semantic errors.[17]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, LOCKED									, 423);		// (WebDAV; RFC 4918) The resource that is being accessed is locked.[17]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, FAILED_DEPENDENCY						, 424);		// (WebDAV; RFC 4918) The request failed because it depended on another request and that request failed (e.g., a PROPPATCH).[17]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, TOO_EARLY								, 425);		// (RFC 8470) Indicates that the server is unwilling to risk processing a request that might be replayed.
	GDEFINE_ENUM_VALUE(HTTP_STATUS, UPGRADE_REQUIRED						, 426);		// The client should switch to a different protocol such as TLS/1.0, given in the Upgrade header field.[58]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, PRECONDITION_REQUIRED					, 428);		// (RFC 6585) The origin server requires the request to be conditional. Intended to prevent the 'lost update' problem, where a client GETs a resource's state, modifies it, and PUTs it back to the server, when meanwhile a third party has modified the state on the server, leading to a conflict.[59]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, TOO_MANY_REQUESTS						, 429);		// (RFC 6585) The user has sent too many requests in a given amount of time. Intended for use with rate-limiting schemes.[59]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, REQUEST_HEADER_FIELDS_TOO_LARGE			, 431);		// (RFC 6585) The server is unwilling to process the request because either an individual header field, or all the header fields collectively, are too large.[59]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, UNAVAILABLE_FOR_LEGAL_REASONS			, 451);		// (RFC 7725) A server operator has received a legal demand to deny access to a resource or to a set of resources that includes the requested resource.[60] The code 451 was chosen as a reference to the novel Fahrenheit 451 (see the Acknowledgements in the RFC).
	//_5XX_SERVER_ERRORS					, 			THE_SERVER_FAILEd to fulfill a request. Response status codes beginning with the digit "5" indicate cases in which the server is aware that it has encountered an error or is otherwise incapable of performing the request. Except when responding to a HEAD request, the server should include an entity containing an explanation of the error situation, and indicate whether it is a temporary or permanent condition. Likewise, user agents should display any included entity to the user. These response codes are applicable to any request method.[62]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, INTERNAL_SERVER_ERROR					, 500);		// A generic error message, given when an unexpected condition was encountered and no more specific message is suitable.[63]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, NOT_IMPLEMENTED							, 501);		// The server either does not recognize the request method, or it lacks the ability to fulfil the request. Usually this implies future availability (e.g., a new feature of a web-service API).[64]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, BAD_GATEWAY								, 502);		// The server was acting as a gateway or proxy and received an invalid response from the upstream server.[65]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, SERVICE_UNAVAILABLE						, 503);		// The server cannot handle the request (because it is overloaded or down for maintenance). Generally, this is a temporary state.[66]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, GATEWAY_TIMEOUT							, 504);		// The server was acting as a gateway or proxy and did not receive a timely response from the upstream server.[67]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, HTTP_VERSION_NOT_SUPPORTED				, 505);		// The server does not support the HTTP protocol version used in the request.[68]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, VARIANT_ALSO_NEGOTIATES					, 506);		// (RFC 2295) Transparent content negotiation for the request results in a circular reference.[69]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, INSUFFICIENT_STORAGE					, 507);		// (WebDAV; RFC 4918) The server is unable to store the representation needed to complete the request.[17]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, LOOP_DETECTED							, 508);		// (WebDAV; RFC 5842) The server detected an infinite loop while processing the request (sent instead of 208 Already Reported).
	GDEFINE_ENUM_VALUE(HTTP_STATUS, NOT_EXTENDED							, 510);		// (RFC 2774) Further extensions to the request are required for the server to fulfil it.[70]
	GDEFINE_ENUM_VALUE(HTTP_STATUS, NETWORK_AUTHENTICATION_REQUIRED			, 511);		// (RFC 6585) The client needs to authenticate to gain network access. Intended for use by intercepting proxies used to control access to the network (e.g., "captive portals" used to require agreement to Terms of Service before granting full Internet access via a Wi-Fi hotspot).[59]

// // UNOFFICIAL_CODES
// // THE_FOLLOWING_CODES_ARE_NOT_SPECIFIED_BY_ANY_STANdard.
// 103_CHECKPOINT								// Used in the resumable requests proposal to resume aborted PUT or POST requests.[71]
// 218_THIS_IS_FINE								// (Apache Web Server) Used as a catch-all error condition for allowing response bodies to flow through Apache when ProxyErrorOverride is enabled. When ProxyErrorOverride is enabled in Apache, response bodies that contain a status code of 4xx or 5xx are automatically discarded by Apache in favor of a generic response or a custom response specified by the ErrorDocument directive.[72]
// 419_PAGE_EXPIRED								// (Laravel Framework) Used by the Laravel Framework when a CSRF Token is missing or expired.
// 420_METHOD_FAILURE							// (Spring Framework) A deprecated response used by the Spring Framework when a method has failed.[73]
// 420_ENHANCE_YOUR_CALM						// (Twitter) Returned by version 1 of the Twitter Search and Trends API when the client is being rate limited; versions 1.1 and later use the 429 Too Many Requests response code instead.[74]
// 430_REQUEST_HEADER_FIELDS_TOO_LARGE			// (Shopify) Used by Shopify, instead of the 429 Too Many Requests response code, when too many URLs are requested within a certain time frame.[75]
// 450_BLOCKED_BY_WINDOWS_PARENTAL_CONTROLS		// (Microsoft) The Microsoft extension code indicated when Windows Parental Controls are turned on and are blocking access to the requested webpage.[76]
// 498_INVALID_TOKEN							// (Esri) Returned by ArcGIS for Server. Code 498 indicates an expired or otherwise invalid token.[77]
// 499_TOKEN_REQUIRED							// (Esri) Returned by ArcGIS for Server. Code 499 indicates that a token is required but was not submitted.[77]
// 509_BANDWIDTH_LIMIT_EXCEEDED					// (Apache Web Server/cPanel) The server has exceeded the bandwidth specified by the server administrator; this is often used by shared hosting providers to limit the bandwidth of customers.[78]
// 526_INVALID_SSL_CERTIFICATE					// Used by Cloudflare and Cloud Foundry's gorouter to indicate failure to validate the SSL/TLS certificate that the origin server presented.
// 530_SITE_IS_FROZEN							// Used by the Pantheon web platform to indicate a site that has been frozen due to inactivity.[79]
// 598_NETWORK_READ_TIMEOUT_ERROR				// (Informal convention) Used by some HTTP proxies to signal a network read timeout behind the proxy to a client in front of the proxy.[80]
//
// // INTERNET_INFORMATION_SERVICES
// // MICROSOFT'S_INTERNET_INFORMATION_SERVICES_WEB_SERver expands the 4xx error space to signal errors with the client's request.
// 440_LOGIN_TIME-OUT							// The client's session has expired and must log in again.[81]
// 449_RETRY_WITH								// The server cannot honour the request because the user has not provided the required information.[82]
// 451_REDIRECT									// Used in Exchange ActiveSync when either a more efficient server is available or the server cannot access the users' mailbox.[83] The client is expected to re-run the HTTP AutoDiscover operation to find a more appropriate server.[84]
//
// // nginx
// // THE_NGINX_WEB_SERVER_SOFTWARE_EXPANDS_THE_4XX_ERRor space to signal issues with the client's request.[85][86]
// 444_NO_RESPONSE								// Used internally[87] to instruct the server to return no information to the client and close the connection immediately.
// 494_REQUEST_HEADER_TOO_LARGE					// Client sent too large request or too long header line.
// 495_SSL_CERTIFICATE_ERROR					// An expansion of the 400 Bad Request response code, used when the client has provided an invalid client certificate.
// 496_SSL_CERTIFICATE_REQUIRED					// An expansion of the 400 Bad Request response code, used when a client certificate is required but not provided.
// 497_HTTP_REQUEST_SENT_TO_HTTPS_PORT			// An expansion of the 400 Bad Request response code, used when the client has made a HTTP request to a port listening for HTTPS requests.
// 499_CLIENT_CLOSED_REQUEST					// Used when the client has closed the request before the server could send a response.
//
// // CLOUDFLARE
// // CLOUDFLARE'S_REVERSE_PROXY_SERVICE_EXPANDS_THE_5Xx series of errors space to signal issues with the origin server.[88]
// 520_UNKNOWN_ERROR							// The 520 error is used as a "catch-all response for when the origin server returns something unexpected", listing connection resets, large headers, and empty or invalid responses as common triggers.
// 521_WEB_SERVER_IS_DOWN						//The origin server has refused the connection from Cloudflare.
// 522_CONNECTION_TIMED_OUT						// Cloudflare could not negotiate a TCP handshake with the origin server.
// 523_ORIGIN_IS_UNREACHABLE					// Cloudflare could not reach the origin server; for example, if the DNS records for the origin server are incorrect.
// 524_A_TIMEOUT_OCCURRED						// Cloudflare was able to complete a TCP connection to the origin server, but did not receive a timely HTTP response.
// 525_SSL_HANDSHAKE_FAILED						// Cloudflare could not negotiate a SSL/TLS handshake with the origin server.
// 526_INVALID_SSL_CERTIFICATE					// Cloudflare could not validate the SSL certificate on the origin web server.
// 527_RAILGUN_ERROR							// Indicates that the request timed out or failed after the WAN connection had been established.[89]
// 530_ORIGIN_DNS_ERROR							// Indicates that the requested host name could not be resolved on the Cloudflare network to an origin server.[90]
//
// // AWS_ELASTIC_LOAD_BALANCER
// // AMAZON'S_ELASTIC_LOAD_BALANCING_ADDS_A_FEW_CUSTOM 4xx return codes
// 460											// Client closed the connection with the load balancer before the idle timeout period elapsed. Typically when client timeout is sooner than the Elastic Load Balancer's timeout.[91]
// 463											// The load balancer received an X-Forwarded-For request header with more than 30 IP addresses.[91]

#pragma pack(push, 1)
	struct SHTTPAPIRequest {
		::gpk::aobj<::gpk::vcc>					QueryStringElements			= {};
		::gpk::aobj<::gpk::TKeyValConstString>	QueryStringKeyVals			= {};
		::gpk::vcc								Ip							= {};
		::gpk::vcc								Port						= {};
		::gpk::vcc								Script						= {};
		::gpk::vcc								Path						= {};
		::gpk::vcc								QueryString					= {};
		::gpk::vcc								ContentBody					= {};
		::gpk::HTTP_METHOD						Method						= {};
		bool									IsCGIEnviron				= false;
	};
#pragma pack(pop)
} // namespace

#endif // GPK_HTTP_H_23627
