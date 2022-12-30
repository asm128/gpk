#include "gpk_stdsocket.h"
#include "gpk_http.h"

#ifndef GPK_HTTP_CLIENT_H_23482394
#define GPK_HTTP_CLIENT_H_23482394

namespace gpk
{
	struct SHTTPResponse {
		::gpk::apod<byte_t>					Body;
		::gpk::apod<byte_t>					HeaderData;
		::gpk::aobj<::gpk::TKeyValConstString>	Headers;
	};

	::gpk::error_t					httpClientRequest
		(	const ::gpk::SIPv4				& remoteAddress
		,	::gpk::HTTP_METHOD				method
		,	const ::gpk::vcs	& hostName
		,	const ::gpk::vcs	& path
		,	const ::gpk::vcs	& contentType
		,	const ::gpk::vcb	& body
		,	::gpk::apod<byte_t>		& out_received
		);

	::gpk::error_t					httpClientRequest
		(	const ::gpk::SIPv4				& remoteAddress
		,	::gpk::HTTP_METHOD				method
		,	const ::gpk::vcs	& hostName
		,	const ::gpk::vcs	& path
		,	const ::gpk::vcs	& contentType
		,	const ::gpk::vcb	& body
		,	::gpk::SHTTPResponse			& out_received
		);

	::gpk::error_t					httpsClientRequest
		(	const ::gpk::SIPv4				& remoteAddress
		,	::gpk::HTTP_METHOD				method
		,	const ::gpk::vcs	& hostName
		,	const ::gpk::vcs	& path
		,	const ::gpk::vcs	& contentType
		,	const ::gpk::vcb	& body
		,	::gpk::apod<byte_t>		& out_received
		);

	::gpk::error_t					urlDecode	(::gpk::vcc urlToDecode, ::gpk::apod<char_t> & decoded);

}

#endif // GPK_HTTP_CLIENT_H_23482394
