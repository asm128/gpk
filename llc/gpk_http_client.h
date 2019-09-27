#include "gpk_stdsocket.h"
#include "gpk_http.h"

#ifndef GPK_HTTP_CLIENT_H_23482394
#define GPK_HTTP_CLIENT_H_23482394

namespace gpk
{
	struct SHTTPResponse {
		::gpk::array_pod<byte_t>					Body;
		::gpk::array_pod<byte_t>					HeaderData;
		::gpk::array_obj<::gpk::TKeyValConstString>	Headers;
	};

	::gpk::error_t					httpClientRequest
		(	const ::gpk::SIPv4				& remoteAddress
		,	::gpk::HTTP_METHOD				method
		,	const ::gpk::view_const_string	& hostName
		,	const ::gpk::view_const_string	& path
		,	const ::gpk::view_const_string	& contentType
		,	const ::gpk::view_const_byte	& body
		,	::gpk::array_pod<byte_t>		& out_received
		);

	::gpk::error_t					httpClientRequest
		(	const ::gpk::SIPv4				& remoteAddress
		,	::gpk::HTTP_METHOD				method
		,	const ::gpk::view_const_string	& hostName
		,	const ::gpk::view_const_string	& path
		,	const ::gpk::view_const_string	& contentType
		,	const ::gpk::view_const_byte	& body
		,	::gpk::SHTTPResponse			& out_received
		);

	::gpk::error_t					httpsClientRequest
		(	const ::gpk::SIPv4				& remoteAddress
		,	::gpk::HTTP_METHOD				method
		,	const ::gpk::view_const_string	& hostName
		,	const ::gpk::view_const_string	& path
		,	const ::gpk::view_const_string	& contentType
		,	const ::gpk::view_const_byte	& body
		,	::gpk::array_pod<byte_t>		& out_received
		);

	::gpk::error_t					urlDecode	(::gpk::view_const_char urlToDecode, ::gpk::array_pod<char_t> & decoded);

}

#endif // GPK_HTTP_CLIENT_H_23482394
