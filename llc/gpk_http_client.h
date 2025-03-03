#include "gpk_stdsocket.h"
#include "gpk_http.h"

#ifndef GPK_HTTP_CLIENT_H_23627
#define GPK_HTTP_CLIENT_H_23627

namespace gpk
{
	struct SHTTPResponse {
		::gpk::apod<sc_t>						Body;
		::gpk::apod<sc_t>						HeaderData;
		::gpk::aobj<::gpk::TKeyValConstString>	Headers;
	};

	::gpk::error_t					httpClientRequest
		(	const ::gpk::SIPv4End			& remoteAddress
		,	::gpk::HTTP_METHOD			method
		,	const ::gpk::vcs			& hostName
		,	const ::gpk::vcs			& path
		,	const ::gpk::vcs			& contentType
		,	const ::gpk::vcc			& body
		,	::gpk::apod<sc_t>			& out_received
		);

	::gpk::error_t					httpClientRequest
		(	const ::gpk::SIPv4End			& remoteAddress
		,	::gpk::HTTP_METHOD			method
		,	const ::gpk::vcs			& hostName
		,	const ::gpk::vcs			& path
		,	const ::gpk::vcs			& contentType
		,	const ::gpk::vcc			& body
		,	::gpk::SHTTPResponse		& out_received
		);

	//::gpk::error_t					httpsClientRequest
	//	(	const ::gpk::SIPv4End			& remoteAddress
	//	,	::gpk::HTTP_METHOD			method
	//	,	const ::gpk::vcs			& hostName
	//	,	const ::gpk::vcs			& path
	//	,	const ::gpk::vcs			& contentType
	//	,	const ::gpk::vcc			& body
	//	,	::gpk::apod<sc_t>			& out_received
	//	);

	::gpk::error_t					urlDecode	(::gpk::vcc urlToDecode, ::gpk::apod<sc_t> & decoded);

}

#endif // GPK_HTTP_CLIENT_H_23627
