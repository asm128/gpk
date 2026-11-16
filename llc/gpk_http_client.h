#include "gpk_stdsocket.h"
#include "gpk_http.h"

#ifndef GPK_HTTP_CLIENT_H_23627
#define GPK_HTTP_CLIENT_H_23627

nmsp gpk
{
	struct SHTTPResponse {
		string				Body;
		string				HeaderData;
		aobj<kv<vcst_t>>	Headers;
	};

	err_t				httpClientRequest
		(	cnst SIPv4End	& remoteAddress
		,	HTTP_METHOD		method
		,	vcst_t			hostName
		,	vcst_t			path
		,	vcst_t			contentType
		,	vcst_t			body
		,	string			& out_received
		);

	err_t				httpClientRequest
		(	cnst SIPv4End	& remoteAddress
		,	HTTP_METHOD		method
		,	vcst_t			hostName
		,	vcst_t			path
		,	vcst_t			contentType
		,	vcst_t			body
		,	SHTTPResponse	& out_received
		);

	//err_t					httpsClientRequest
	//	(	cnst SIPv4End			& remoteAddress
	//	,	HTTP_METHOD			method
	//	,	vcst_c			& hostName
	//	,	vcst_c			& path
	//	,	vcst_c			& contentType
	//	,	vcst_c			& body
	//	,	apod<sc_t>			& out_received
	//	);

	err_t					urlDecode	(vcst_t urlToDecode, string & decoded);

}

#endif // GPK_HTTP_CLIENT_H_23627
