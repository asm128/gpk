#define _CRT_SECURE_NO_WARNINGS
#include "gpk_cgi_runtime.h"
#include "gpk_process.h"
#include "gpk_parse.h"

using ::gpk::sc_c;

::gpk::error_t			gpk::httpRequestInit			(::gpk::SHTTPAPIRequest & requestReceived, const ::gpk::SCGIRuntimeValues & runtimeValues, const bool bLogCGIEnviron)	{
	const ::gpk::aobj<::gpk::TKeyValConstString>	& environViews					= runtimeValues.EnvironViews;
	::gpk::vcc								remoteAddr;
	const bool											isCGIEnviron					= -1 != ::gpk::find(::gpk::vcs{"REMOTE_ADDR"}, environViews, remoteAddr);	// Find out if the program is being called as a CGI script.
	requestReceived.IsCGIEnviron					= isCGIEnviron;
	if(bLogCGIEnviron && isCGIEnviron)
		::gpk::writeCGIEnvironToFile(environViews);

	{	// Try to load query from querystring and request body
		const int32_t										offset							= ::gpk::find(::gpk::vcs{"REQUEST_METHOD"}, ::gpk::view<const ::gpk::TKeyValConstString>{environViews.begin(), environViews.size()});
		::gpk::apod<sc_t>							enumValue						= (-1 == offset) ? ::gpk::vcs{"GET"} : environViews[offset].Val;
		requestReceived.Method							= ::gpk::get_value<::gpk::HTTP_METHOD>(enumValue);//::gpk::VALUE -1 == ::gpk::keyValVerify(environViews, "REQUEST_METHOD", "POST") && -1 == ::gpk::keyValVerify(environViews, "REQUEST_METHOD", "post") ? ::gpk::HTTP_METHOD_GET : ::gpk::HTTP_METHOD_POST;
		if(-1 == (int8_t)requestReceived.Method)
			requestReceived.Method							= ::gpk::HTTP_METHOD_GET;
		::gpk::find(::gpk::vcs{"SCRIPT_NAME"	}, environViews, requestReceived.Script);
		::gpk::find(::gpk::vcs{"PATH_INFO"		}, environViews, requestReceived.Path);
		::gpk::find(::gpk::vcs{"QUERY_STRING"	}, environViews, requestReceived.QueryString);
		::gpk::find(::gpk::vcs{"REMOTE_ADDR"	}, environViews, requestReceived.Ip);
		::gpk::find(::gpk::vcs{"REMOTE_IP"		}, environViews, requestReceived.Port);

		requestReceived.ContentBody						= runtimeValues.Content.Body;
		requestReceived.QueryStringElements				= runtimeValues.QueryStringElements;
		requestReceived.QueryStringKeyVals				= runtimeValues.QueryStringKeyVals;
	}
	if(false == isCGIEnviron && runtimeValues.EntryPointArgs.ArgsCommandLine.size() > 1) {	// Get query from command line instead of CGI environ
		requestReceived.Path							= ::gpk::vcs{runtimeValues.EntryPointArgs.ArgsCommandLine[1], (uint32_t)-1};
		if(requestReceived.Path.size() > 2) {
			if('"' == requestReceived.Path[0])
				requestReceived.Path							= {&requestReceived.Path[1], requestReceived.Path.size() - 2};

			const int32_t										queryStringStart					= ::gpk::find('?', requestReceived.Path);
			if(0 <= queryStringStart) {
				requestReceived.QueryString						= {&requestReceived.Path[queryStringStart + 1], requestReceived.Path.size() - (uint32_t)queryStringStart - 1};
				requestReceived.Path							= {&requestReceived.Path[0], (uint32_t)queryStringStart};

				//::gpk::vcs								querystring;
				::gpk::querystring_split(requestReceived.QueryString, requestReceived.QueryStringElements);
				requestReceived.QueryStringKeyVals.resize(requestReceived.QueryStringElements.size());
				for(uint32_t iKeyVal = 0; iKeyVal < requestReceived.QueryStringKeyVals.size(); ++iKeyVal) {
					::gpk::TKeyValConstString								& keyValDst				= requestReceived.QueryStringKeyVals[iKeyVal];
					::gpk::keyval_split(requestReceived.QueryStringElements[iKeyVal], keyValDst);
				}
			}
		}
	}
	if(requestReceived.Path.size() && requestReceived.Path[0] == '/')
		requestReceived.Path							= {requestReceived.Path.begin() + 1, requestReceived.Path.size() - 1};
	return isCGIEnviron ? 1 : 0;
}

static	::gpk::error_t	cgiLoadContentType			(::gpk::CGI_MEDIA_TYPE & contentType, const ::gpk::view<::gpk::sc_c> & strContentType)	{
	ree_if(0 == strContentType.size(), "%s", "No input string");
	static	const ::gpk::vcc					content_types []			=
		{ ::gpk::vcs{"application/javascript"													}
		, ::gpk::vcs{"application/json"															}
		, ::gpk::vcs{"application/x-www-form-urlencoded"										}
		, ::gpk::vcs{"application/xml"															}
		, ::gpk::vcs{"application/zip"															}
		, ::gpk::vcs{"application/pdf"															}
		, ::gpk::vcs{"application/sql"															}
		, ::gpk::vcs{"application/graphql"														}
		, ::gpk::vcs{"application/ld+json"														}
		, ::gpk::vcs{"application/msword"														}
		, ::gpk::vcs{"application/vnd.openxmlformats-officedocument.wordprocessingml.document"	}
		, ::gpk::vcs{"application/vnd.ms-excel"													}
		, ::gpk::vcs{"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"		}
		, ::gpk::vcs{"application/vnd.ms-powerpoint"											}
		, ::gpk::vcs{"application/vnd.openxmlformats-officedocument.presentationml.presentation"}
		, ::gpk::vcs{"application/vnd.oasis.opendocument.text"									}
		, ::gpk::vcs{"audio/mpeg"																}
		, ::gpk::vcs{"audio/ogg"																}
		, ::gpk::vcs{"multipart/form-data"														}
		, ::gpk::vcs{"text/css"																	}
		, ::gpk::vcs{"text/html"																}
		, ::gpk::vcs{"text/xml"																	}
		, ::gpk::vcs{"text/csv"																	}
		, ::gpk::vcs{"text/plain"																}
		, ::gpk::vcs{"image/png"																}
		, ::gpk::vcs{"image/jpeg"																}
		, ::gpk::vcs{"image/gif"																}
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

static	::gpk::error_t	cgiLoadAddr		(::gpk::SIPv4End & remoteIP, const ::gpk::vcc & strRemoteIP, const ::gpk::view<::gpk::sc_c>& strRemotePort)	{
	remoteIP.Port			= 0;
	::gpk::parseIntegerDecimal(strRemotePort, remoteIP.Port);

	if(strRemoteIP.size()) {
		uint32_t					iOffset			= 0;
		uint32_t					iEnd			= 0;
		remoteIP.IP				= 0;
		for(uint8_t iVal = 0; iVal < 4; ++iVal) {
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
			uint8_t			valueRead	= 0;
			::gpk::parseIntegerDecimal({&strRemoteIP[iOffset], iEnd - iOffset}, valueRead);

			remoteIP.IP	|= ::gpk::byte_to<uint32_t>(valueRead, iVal);

			iOffset		= iEnd + 1;
			iEnd		= iOffset;
		}
		if(0 == strRemotePort.size() && iOffset < strRemoteIP.size()) {
			gpk_necs(gpk::parseIntegerDecimal({&strRemoteIP[iOffset], strRemoteIP.size() - iOffset}, remoteIP.Port));
		}
	}
	return 0;
}

static	::gpk::error_t	cgiLoadFormData	(::gpk::SCGIRuntimeValues & runtimeValues, const ::gpk::view<::gpk::sc_c> & strContent)	{
	(void)runtimeValues, (void)strContent;
	return 0;
}

static	::gpk::error_t	cgiLoadContent	(::gpk::SCGIRuntimeValues & runtimeValues, const ::gpk::CGI_MEDIA_TYPE contentType, const ::gpk::view<::gpk::sc_c> & strContent)	{
	switch(contentType) {
	default: break;
	case ::gpk::CGI_MEDIA_TYPE_MULTIPART_FORM_DATA:
		::cgiLoadFormData(runtimeValues, strContent);
		break;
	}
	return 0;
}

::gpk::error_t			gpk::cgiRuntimeValuesLoad	(::gpk::SCGIRuntimeValues & cgiRuntimeValues, const ::gpk::view<::gpk::sc_c *> & argv)	{
	cgiRuntimeValues.EntryPointArgs.ArgsCommandLine	= argv;
	::gpk::aobj<::gpk::TKeyValConstString>	& environViews	= cgiRuntimeValues.EnvironViews;
	::gpk::environmentBlockFromEnviron(cgiRuntimeValues.EntryPointArgs.EnvironmentBlock);
	::gpk::environmentBlockViews(cgiRuntimeValues.EntryPointArgs.EnvironmentBlock, environViews);
	//for(uint32_t iEnviron = 0; iEnviron < environViews.size(); ++iEnviron)
	//	info_printf("CGI Environ (original): '%s = %s'.", ::gpk::toString(environViews[iEnviron].Key).begin(), ::gpk::toString(environViews[iEnviron].Val).begin());

	{
		::gpk::vcs					querystring;
		::gpk::find(::gpk::vcs{"QUERY_STRING"}, environViews, querystring);
		::gpk::querystring_split(querystring, cgiRuntimeValues.QueryStringElements);
		cgiRuntimeValues.QueryStringKeyVals.resize(cgiRuntimeValues.QueryStringElements.size());
		for(uint32_t iKeyVal = 0; iKeyVal < cgiRuntimeValues.QueryStringKeyVals.size(); ++iKeyVal) {
			::gpk::TKeyValConstString	& keyValDst				= cgiRuntimeValues.QueryStringKeyVals[iKeyVal];
			::gpk::keyval_split(cgiRuntimeValues.QueryStringElements[iKeyVal], keyValDst);
		}
	}
	{
		::gpk::vcs					contentype;
		::gpk::find(::gpk::vcs{"CONTENT_TYPE"}, environViews, contentype);
		if(contentype.size())
			::cgiLoadContentType(cgiRuntimeValues.Content.Type, contentype);
	}
	{
		::gpk::vcs					remoteIP	;
		::gpk::vcs					remotePORT	;
		::gpk::find(::gpk::vcs{"REMOTE_IP"	}, environViews, remoteIP	);
		::gpk::find(::gpk::vcs{"REMOTE_PORT"}, environViews, remotePORT	);
		::cgiLoadAddr(cgiRuntimeValues.RemoteIP, remoteIP, remotePORT);
	}
	{
		::gpk::vcs								contentLength;
		::gpk::find(::gpk::vcs{"CONTENT_LENGTH"}	, environViews, contentLength);
		cgiRuntimeValues.Content.Length						= 0;
		::gpk::parseIntegerDecimal(contentLength, cgiRuntimeValues.Content.Length);
	}
	cgiRuntimeValues.Content.Body.resize(cgiRuntimeValues.Content.Length);
	fread(cgiRuntimeValues.Content.Body.begin(), 1, cgiRuntimeValues.Content.Length, stdin);
	//memset(cgiRuntimeValues.Content.Body.begin(), 0, cgiRuntimeValues.Content.Body.size());
	//uint32_t												iChar							= 0;
	//int														iArg							= 0;
	//while(iChar < cgiRuntimeValues.Content.Length && (iArg = getc(stdin)) != -1)
	//	cgiRuntimeValues.Content.Body[iChar++]				= (sc_t)iArg;

	::cgiLoadContent(cgiRuntimeValues, cgiRuntimeValues.Content.Type, cgiRuntimeValues.Content.Body);
	return 0;
}
