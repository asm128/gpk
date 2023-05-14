#include "gpk_stdsocket.h"
#include "gpk_http.h"

#ifndef GPK_HTTP_CLIENT_H_23482394
#define GPK_HTTP_CLIENT_H_23482394

namespace gpk
{
	struct SHTTPResponse {
		::gpk::apod<char>						Body;
		::gpk::apod<char>						HeaderData;
		::gpk::aobj<::gpk::TKeyValConstString>	Headers;
	};

	::gpk::error_t					httpClientRequest
		(	const ::gpk::SIPv4			& remoteAddress
		,	::gpk::HTTP_METHOD			method
		,	const ::gpk::vcs			& hostName
		,	const ::gpk::vcs			& path
		,	const ::gpk::vcs			& contentType
		,	const ::gpk::vcc			& body
		,	::gpk::apod<char>			& out_received
		);

	::gpk::error_t					httpClientRequest
		(	const ::gpk::SIPv4			& remoteAddress
		,	::gpk::HTTP_METHOD			method
		,	const ::gpk::vcs			& hostName
		,	const ::gpk::vcs			& path
		,	const ::gpk::vcs			& contentType
		,	const ::gpk::vcc			& body
		,	::gpk::SHTTPResponse		& out_received
		);

	//::gpk::error_t					httpsClientRequest
	//	(	const ::gpk::SIPv4			& remoteAddress
	//	,	::gpk::HTTP_METHOD			method
	//	,	const ::gpk::vcs			& hostName
	//	,	const ::gpk::vcs			& path
	//	,	const ::gpk::vcs			& contentType
	//	,	const ::gpk::vcc			& body
	//	,	::gpk::apod<char>			& out_received
	//	);

	::gpk::error_t					urlDecode	(::gpk::vcc urlToDecode, ::gpk::apod<char> & decoded);

}

#endif // GPK_HTTP_CLIENT_H_23482394
