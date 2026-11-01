#include "gpk_cgi_runtime.h"
#include "gpk_process.h"
#include "gpk_parse.h"
#include "gpk_keyval_view.h"

GPK_USING_TYPEINT();
GPK_USING_APOD();
GPK_USING_VIEW();
GPK_USING_ASTATIC();

gpk::err_t			gpk::httpRequestInit			(SHTTPAPIRequest & requestReceived, cnst SCGIRuntimeValues & runtimeValues, const bool bLogCGIEnviron)	{
	view<cnst kv<vcst_t>>			environViews			= runtimeValues.RuntimeValues.EntryPointArgs.Environment;
	vcst_t							remoteAddr;
	const bool						isCGIEnviron					= -1 != ::gpk::find("REMOTE_ADDR", environViews, remoteAddr);	// Find out if the program is being called as a CGI script.
	requestReceived.IsCGIEnviron	= isCGIEnviron;
	if(bLogCGIEnviron && isCGIEnviron)
		writeCGIEnvironToFile(environViews);

	{	// Try to load query from querystring and request body
		const s2_t						offset				= ::gpk::find("REQUEST_METHOD", ::gpk::view<const ::gpk::kv<vcst_t>>{environViews.begin(), environViews.size()});
		::gpk::string					enumValue			= (-1 == offset) ? vcst_t("GET") : environViews[offset].Val;
		requestReceived.Method		= get_value<HTTP_METHOD>(enumValue);//::gpk::VALUE -1 == ::gpk::keyValVerify(environViews, "REQUEST_METHOD", "POST") && -1 == ::gpk::keyValVerify(environViews, "REQUEST_METHOD", "post") ? ::gpk::HTTP_METHOD_GET : ::gpk::HTTP_METHOD_POST;
		if(-1 == (s0_t)requestReceived.Method)
			requestReceived.Method							= HTTP_METHOD_GET;
		::gpk::find("SCRIPT_NAME", environViews, requestReceived.Script);
		::gpk::find("PATH_INFO", environViews, requestReceived.Path);
		::gpk::find("QUERY_STRING", environViews, requestReceived.QueryString);
		::gpk::find("REMOTE_ADDR", environViews, requestReceived.Ip);
		::gpk::find("REMOTE_IP", environViews, requestReceived.Port);

		requestReceived.ContentBody						= runtimeValues.Content.Body;
		requestReceived.QueryStringElements				= runtimeValues.QueryStringElements;
		requestReceived.QueryStringKeyVals				= runtimeValues.QueryStringKeyVals;
	}
	if(false == isCGIEnviron && runtimeValues.RuntimeValues.EntryPointArgs.Positionals.size()) {	// Get query from command line instead of CGI environ
		requestReceived.Path							= runtimeValues.RuntimeValues.EntryPointArgs.Positionals[0];
		if(requestReceived.Path.size() > 2) {
			if('"' == requestReceived.Path[0])
				requestReceived.Path							= {&requestReceived.Path[1], requestReceived.Path.size() - 2};

			const s2_t										queryStringStart					= ::gpk::find('?', requestReceived.Path);
			if(0 <= queryStringStart) {
				requestReceived.QueryString						= {&requestReceived.Path[queryStringStart + 1], requestReceived.Path.size() - (u2_t)queryStringStart - 1};
				requestReceived.Path							= {&requestReceived.Path[0], (u2_t)queryStringStart};

				//::gpk::vcst_t								querystring;
				::gpk::querystring_split(requestReceived.QueryString, requestReceived.QueryStringElements);
				requestReceived.QueryStringKeyVals.resize(requestReceived.QueryStringElements.size());
				for(u2_t iKeyVal = 0; iKeyVal < requestReceived.QueryStringKeyVals.size(); ++iKeyVal) {
					::gpk::kv<vcst_t>								& keyValDst				= requestReceived.QueryStringKeyVals[iKeyVal];
					::gpk::keyval_split(requestReceived.QueryStringElements[iKeyVal], keyValDst);
				}
			}
		}
	}
	if(requestReceived.Path.size() && requestReceived.Path[0] == '/')
		requestReceived.Path							= {requestReceived.Path.begin() + 1, requestReceived.Path.size() - 1};
	return isCGIEnviron ? 1 : 0;
}

static	::gpk::err_t	cgiLoadContentType			(::gpk::CGI_MEDIA_TYPE & contentType, const ::gpk::view<::gpk::sc_c> & strContentType)	{
	if_true_vef(-1, 0 == strContentType.size(), "%s", "No input string");
	static	::gpk::view<sc_c>					content_types []			=
		{ "application/javascript"
		, "application/json"
		, "application/x-www-form-urlencoded"
		, "application/xml"
		, "application/zip"
		, "application/pdf"
		, "application/sql"
		, "application/graphql"
		, "application/ld+json"
		, "application/msword"
		, "application/vnd.openxmlformats-officedocument.wordprocessingml.document"
		, "application/vnd.ms-excel"
		, "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"
		, "application/vnd.ms-powerpoint"
		, "application/vnd.openxmlformats-officedocument.presentationml.presentation"
		, "application/vnd.oasis.opendocument.text"
		, "audio/mpeg"
		, "audio/ogg"
		, "multipart/form-data"
		, "text/css"
		, "text/html"
		, "text/xml"
		, "text/csv"
		, "text/plain"
		, "image/png"
		, "image/jpeg"
		, "image/gif"
		};

	::gpk::CGI_MEDIA_TYPE									val;
	(val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_JAVASCRIPT															); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_JSON																); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_X_WWW_FORM_URLENCODED												); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_XML																); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_ZIP																); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_PDF																); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_SQL																); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_GRAPHQL															); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_LD_JSON															); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_MSWORD_DOC															); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_WORDPROCESSINGML_DOCUMENT_DOCX	); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_VND_MS_EXCEL_XLS													); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_SPREADSHEETML_SHEET_XLSX			); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_VND_MS_POWERPOINT_PPT												); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_PRESENTATIONML_PRESENTATION_PPTX	); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT_ODT									); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_AUDIO_MPEG																		); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_AUDIO_OGG																		); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_MULTIPART_FORM_DATA															); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_TEXT_CSS																		); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_TEXT_HTML																		); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_TEXT_XML																		); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_TEXT_CSV																		); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_TEXT_PLAIN																		); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_IMAGE_PNG																		); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_IMAGE_JPEG																		); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	(val = ::gpk::CGI_MEDIA_TYPE_IMAGE_GIF																		); if(0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	return 0;
}

static	::gpk::err_t	cgiLoadAddr		(::gpk::SIPv4End & remoteIP, ::gpk::view<sc_c> & strRemoteIP, const ::gpk::view<::gpk::sc_c>& strRemotePort) {
	remoteIP.Port			= 0;
	::gpk::parseIntegerDecimal(strRemotePort, remoteIP.Port);

	if(strRemoteIP.size()) {
		u2_t					iOffset			= 0;
		u2_t					iEnd			= 0;
		remoteIP.IP				= 0;
		for(u0_t iVal = 0; iVal < 4; ++iVal) {
			while(iEnd < strRemoteIP.size()) {
				::gpk::sc_c curChar = strRemoteIP[iEnd];
				if( curChar == '.'
				 ||	curChar == ':'
				 ||	curChar == '\0'
				 || (iEnd - iOffset) > 3	// 3 digit max
				)
					break;
				++iEnd;
			}
			u0_t			valueRead	= 0;
			::gpk::parseIntegerDecimal({&strRemoteIP[iOffset], iEnd - iOffset}, valueRead);

			remoteIP.IP	|= ::gpk::byte_to<u2_t>(valueRead, iVal);

			iOffset		= iEnd + 1;
			iEnd		= iOffset;
		}
		if(0 == strRemotePort.size() && iOffset < strRemoteIP.size()) {
			if_fail_fe(gpk::parseIntegerDecimal({&strRemoteIP[iOffset], strRemoteIP.size() - iOffset}, remoteIP.Port));
		}
	}
	return 0;
}

static	::gpk::err_t	cgiLoadFormData	(::gpk::SCGIRuntimeValues & runtimeValues, const ::gpk::view<::gpk::sc_c> & strContent)	{
	(void)runtimeValues, (void)strContent;
	return 0;
}

static	::gpk::err_t	cgiLoadContent	(::gpk::SCGIRuntimeValues & runtimeValues, const ::gpk::CGI_MEDIA_TYPE contentType, const ::gpk::view<::gpk::sc_c> & strContent)	{
	switch(contentType) {
	default: break;
	case ::gpk::CGI_MEDIA_TYPE_MULTIPART_FORM_DATA:
		::cgiLoadFormData(runtimeValues, strContent);
		break;
	}
	return 0;
}

::gpk::err_t			gpk::cgiRuntimeValuesLoad	(::gpk::SCGIRuntimeValues & cgiRuntimeValues)	{
	::gpk::view<cnst ::gpk::kv<vcst_t>>	environViews			= cgiRuntimeValues.RuntimeValues.EntryPointArgs.Environment;
	//for(u2_t iEnviron = 0; iEnviron < environViews.size(); ++iEnviron)
	//	info_printf("CGI Environ (original): '%s = %s'.", GPK_VIEW_ARGS(environViews[iEnviron].Key).begin(), GPK_VIEW_ARGS(environViews[iEnviron].Val).begin());

	{
		::gpk::vcst_t					querystring;
		::gpk::find("QUERY_STRING", environViews, querystring);
		::gpk::querystring_split(querystring, cgiRuntimeValues.QueryStringElements);
		cgiRuntimeValues.QueryStringKeyVals.resize(cgiRuntimeValues.QueryStringElements.size());
		for(u2_t iKeyVal = 0; iKeyVal < cgiRuntimeValues.QueryStringKeyVals.size(); ++iKeyVal) {
			::gpk::kv<vcst_t>	& keyValDst				= cgiRuntimeValues.QueryStringKeyVals[iKeyVal];
			::gpk::keyval_split(cgiRuntimeValues.QueryStringElements[iKeyVal], keyValDst);
		}
	}
	{
		::gpk::vcst_t					contentype;
		::gpk::find("CONTENT_TYPE", environViews, contentype);
		if(contentype.size())
			::cgiLoadContentType(cgiRuntimeValues.Content.Type, contentype);
	}
	{
		::gpk::vcst_t					remoteIP	;
		::gpk::vcst_t					remotePORT	;
		::gpk::find("REMOTE_IP", environViews, remoteIP	);
		::gpk::find("REMOTE_PORT", environViews, remotePORT	);
		::cgiLoadAddr(cgiRuntimeValues.RemoteIP, remoteIP, remotePORT);
	}
	{
		::gpk::vcst_t								contentLength;
		::gpk::find("CONTENT_LENGTH"	, environViews, contentLength);
		cgiRuntimeValues.Content.Length						= 0;
		::gpk::parseIntegerDecimal(contentLength, cgiRuntimeValues.Content.Length);
	}
	cgiRuntimeValues.Content.Body.resize(cgiRuntimeValues.Content.Length);
	fread(cgiRuntimeValues.Content.Body.begin(), 1, cgiRuntimeValues.Content.Length, stdin);
	//memset(cgiRuntimeValues.Content.Body.begin(), 0, cgiRuntimeValues.Content.Body.size());
	//u2_t												iChar							= 0;
	//int														iArg							= 0;
	//while(iChar < cgiRuntimeValues.Content.Length && (iArg = getc(stdin)) != -1)
	//	cgiRuntimeValues.Content.Body[iChar++]				= (sc_t)iArg;

	::cgiLoadContent(cgiRuntimeValues, cgiRuntimeValues.Content.Type, cgiRuntimeValues.Content.Body);
	return 0;
}
