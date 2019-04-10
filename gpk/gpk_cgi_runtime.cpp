#include "gpk_cgi_runtime.h"
#include <string>

static	::gpk::error_t								load_env	(const ::gpk::view_const_string& key, ::gpk::array_pod<char> & queryString)	{
	char													* qs				= {};
	size_t													ql					= 0;
	_dupenv_s(&qs, &ql, key.begin());
	if(0 == qs) {
		queryString.resize(0);
		return -1;
	}
	queryString.resize((uint32_t)ql + 1);
	memset(queryString.begin(), 0, queryString.size());
	memcpy(queryString.begin(), qs, ql);
	//queryString.resize(ql);
	queryString.resize((uint32_t)ql - ((0 == qs[ql - 1]) ? 1 : 0));
	free(qs);
	return 0;
}

::gpk::error_t										processKeyVal					(::gpk::SCGIFramework& webRuntimeValues, const ::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>& keyVal)	{ 
	if(0 == keyVal.Key.size())
		return -1;
	try {
			 if(keyVal.Key.size() == (::gpk::size("width"	) - 1) && 0 == memcmp("width"	, keyVal.Key.begin(), ::gpk::size("width"	) - 1)) webRuntimeValues.TargetSize.x = (uint16_t)::std::stoi(::std::string{keyVal.Val.begin(), keyVal.Val.size()});
		else if(keyVal.Key.size() == (::gpk::size("height"	) - 1) && 0 == memcmp("height"	, keyVal.Key.begin(), ::gpk::size("height"	) - 1)) webRuntimeValues.TargetSize.y = (uint16_t)::std::stoi(::std::string{keyVal.Val.begin(), keyVal.Val.size()});
	}
	catch(...){
		webRuntimeValues.TargetSize							= {123, 456};
		return -1;
	}
	try {
		if(keyVal.Val.size() && keyVal.Key.size() == (::gpk::size("bt") - 1) && 0 == memcmp("bt", keyVal.Key.begin(), ::gpk::size("bt") - 1)) if(1 == (uint16_t)::std::stoi(::std::string{keyVal.Val.begin(), keyVal.Val.size()})) webRuntimeValues.Bootstrapped = true;
	}
	catch(...){
		webRuntimeValues.Bootstrapped						= false;
		return -1;
	}
	if(keyVal.Val.size() && keyVal.Key.size() == (::gpk::size("m") - 1) && 0 == memcmp("m", keyVal.Key.begin(), ::gpk::size("m") - 1)) 
		webRuntimeValues.ModuleName							= keyVal.Val;
	return 0;
}

::gpk::error_t										cgiLoadContentType				(::gpk::CGI_MEDIA_TYPE & contentType, const ::gpk::view_array<const char> & strContentType)	{ 
		 if(0 == strcmp(strContentType.begin(), "application/javascript"                                                   )) { contentType = ::gpk::CGI_MEDIA_TYPE_APPLICATION_JAVASCRIPT															; }
	else if(0 == strcmp(strContentType.begin(), "application/json"                                                         )) { contentType = ::gpk::CGI_MEDIA_TYPE_APPLICATION_JSON																; }
	else if(0 == strcmp(strContentType.begin(), "application/x-www-form-urlencoded"                                        )) { contentType = ::gpk::CGI_MEDIA_TYPE_APPLICATION_X_WWW_FORM_URLENCODED												; }
	else if(0 == strcmp(strContentType.begin(), "application/xml"                                                          )) { contentType = ::gpk::CGI_MEDIA_TYPE_APPLICATION_XML																	; }
	else if(0 == strcmp(strContentType.begin(), "application/zip"                                                          )) { contentType = ::gpk::CGI_MEDIA_TYPE_APPLICATION_ZIP																	; }
	else if(0 == strcmp(strContentType.begin(), "application/pdf"                                                          )) { contentType = ::gpk::CGI_MEDIA_TYPE_APPLICATION_PDF																	; }
	else if(0 == strcmp(strContentType.begin(), "application/sql"                                                          )) { contentType = ::gpk::CGI_MEDIA_TYPE_APPLICATION_SQL																	; }
	else if(0 == strcmp(strContentType.begin(), "application/graphql"                                                      )) { contentType = ::gpk::CGI_MEDIA_TYPE_APPLICATION_GRAPHQL																; }
	else if(0 == strcmp(strContentType.begin(), "application/ld+json"                                                      )) { contentType = ::gpk::CGI_MEDIA_TYPE_APPLICATION_LD_JSON																; }
	else if(0 == strcmp(strContentType.begin(), "application/msword"                                                       )) { contentType = ::gpk::CGI_MEDIA_TYPE_APPLICATION_MSWORD_DOC															; }
	else if(0 == strcmp(strContentType.begin(), "application/vnd.openxmlformats-officedocument.wordprocessingml.document"  )) { contentType = ::gpk::CGI_MEDIA_TYPE_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_WORDPROCESSINGML_DOCUMENT_DOCX	; }
	else if(0 == strcmp(strContentType.begin(), "application/vnd.ms-excel"                                                 )) { contentType = ::gpk::CGI_MEDIA_TYPE_APPLICATION_VND_MS_EXCEL_XLS													; }
	else if(0 == strcmp(strContentType.begin(), "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"        )) { contentType = ::gpk::CGI_MEDIA_TYPE_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_SPREADSHEETML_SHEET_XLSX			; }
	else if(0 == strcmp(strContentType.begin(), "application/vnd.ms-powerpoint"                                            )) { contentType = ::gpk::CGI_MEDIA_TYPE_APPLICATION_VND_MS_POWERPOINT_PPT												; }
	else if(0 == strcmp(strContentType.begin(), "application/vnd.openxmlformats-officedocument.presentationml.presentation")) { contentType = ::gpk::CGI_MEDIA_TYPE_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_PRESENTATIONML_PRESENTATION_PPTX	; }
	else if(0 == strcmp(strContentType.begin(), "application/vnd.oasis.opendocument.text"                                  )) { contentType = ::gpk::CGI_MEDIA_TYPE_APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT_ODT										; }
	else if(0 == strcmp(strContentType.begin(), "audio/mpeg"                                                               )) { contentType = ::gpk::CGI_MEDIA_TYPE_AUDIO_MPEG																		; }
	else if(0 == strcmp(strContentType.begin(), "audio/ogg"                                                                )) { contentType = ::gpk::CGI_MEDIA_TYPE_AUDIO_OGG																		; }
	else if(0 == strcmp(strContentType.begin(), "multipart/form-data"                                                      )) { contentType = ::gpk::CGI_MEDIA_TYPE_MULTIPART_FORM_DATA																; }
	else if(0 == strcmp(strContentType.begin(), "text/css"                                                                 )) { contentType = ::gpk::CGI_MEDIA_TYPE_TEXT_CSS																		; }
	else if(0 == strcmp(strContentType.begin(), "text/html"                                                                )) { contentType = ::gpk::CGI_MEDIA_TYPE_TEXT_HTML																		; }
	else if(0 == strcmp(strContentType.begin(), "text/xml"                                                                 )) { contentType = ::gpk::CGI_MEDIA_TYPE_TEXT_XML																		; }
	else if(0 == strcmp(strContentType.begin(), "text/csv"                                                                 )) { contentType = ::gpk::CGI_MEDIA_TYPE_TEXT_CSV																		; }
	else if(0 == strcmp(strContentType.begin(), "text/plain"                                                               )) { contentType = ::gpk::CGI_MEDIA_TYPE_TEXT_PLAIN																		; }
	else if(0 == strcmp(strContentType.begin(), "image/png"                                                                )) { contentType = ::gpk::CGI_MEDIA_TYPE_IMAGE_PNG																		; }
	else if(0 == strcmp(strContentType.begin(), "image/jpeg"                                                               )) { contentType = ::gpk::CGI_MEDIA_TYPE_IMAGE_JPEG																		; }
	else if(0 == strcmp(strContentType.begin(), "image/gif"                                                                )) { contentType = ::gpk::CGI_MEDIA_TYPE_IMAGE_GIF																		; }
	else 
		return -1;
	return 0;
}

::gpk::error_t										cgiLoadAddr				(::gpk::SIPv4 & remoteIP, const ::gpk::view_array<const char>& strRemoteIP, const ::gpk::view_array<const char>& strRemotePort)	{ 
	if(strRemotePort.size())
		try {
			remoteIP.Port						= (uint16_t)::std::stoi(strRemotePort.begin());
		}
		catch(...) {
			remoteIP.Port						= 0;
		}

	if(strRemoteIP.size()) {
		uint32_t iOffset	= 0; 
		uint32_t iEnd		= 0; 
		for(uint32_t iVal = 0; iVal < 4; ++iVal) {
			while(iEnd < strRemoteIP.size()) {
				char curChar = strRemoteIP[iEnd];
				if( curChar == '.' 
				 ||	curChar == ':' 
				 ||	curChar == '\0'
				)
					break;
				++iEnd;
			}
			try {
				remoteIP.IP[iVal]						= (ubyte_t)::std::stoi({&strRemoteIP[iOffset], iEnd - iOffset});
			}
			catch(...) {
				remoteIP.IP[iVal]						= 0;
			}
			iOffset													= iEnd + 1;
			iEnd													= iOffset;
		}
	}
	return 0;
}

::gpk::error_t										cgiRuntimeValuesLoadEnv			(::gpk::SCGIRuntimeValues & webRuntimeValues)	{ 
	::load_env("QUERY_STRING"	, webRuntimeValues.QueryString		); 
	::load_env("CONTENT_TYPE"	, webRuntimeValues.ContentType		); 
	::load_env("CONTENT_LENGTH"	, webRuntimeValues.ContentLength	); 
	::load_env("REMOTE_ADDR"	, webRuntimeValues.StrRemoteIP		);
	::load_env("REMOTE_PORT"	, webRuntimeValues.StrRemotePort	);
	return 0;
}
::gpk::error_t										gpk::cgiRuntimeValuesLoad		(::gpk::SCGIFramework & framework)	{ 
	::gpk::SCGIRuntimeValues								& webRuntimeValues				= framework.RuntimeValues;
	::cgiRuntimeValuesLoadEnv(webRuntimeValues);
	//webRuntimeValues.QueryString						= "bt=1&width=100&height=200&m=test_cgi_module";
	//webRuntimeValues.QueryString						= "bt=1&width=720&height=640&frame=0&category=0&neighbor=6&m=tianadev";
	//webRuntimeValues.QueryString						= "bt=1&m=tianadev&width=1708&height=744&gallery=0&frame=1";
	//webRuntimeValues.StrRemoteIP						= "129.166.3.1";
	//webRuntimeValues.StrRemotePort					= "500";

	::gpk::querystring_split({webRuntimeValues.QueryString.begin(), webRuntimeValues.QueryString.size()}, webRuntimeValues.QueryStringElements);
	webRuntimeValues.QueryStringKeyVals.resize(webRuntimeValues.QueryStringElements.size());
	for(uint32_t iKeyVal = 0; iKeyVal < webRuntimeValues.QueryStringKeyVals.size(); ++iKeyVal) {
		::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>	& keyValDst				= webRuntimeValues.QueryStringKeyVals[iKeyVal];
		::gpk::keyval_split(webRuntimeValues.QueryStringElements[iKeyVal], keyValDst);
		::processKeyVal(framework, webRuntimeValues.QueryStringKeyVals[iKeyVal]);
	}
	::cgiLoadContentType(webRuntimeValues.Content.Type, webRuntimeValues.ContentType);
	::cgiLoadAddr(webRuntimeValues.RemoteIP, webRuntimeValues.StrRemoteIP, webRuntimeValues.StrRemotePort);
	try {
		webRuntimeValues.Content.Length						= webRuntimeValues.ContentLength.size() ? (uint32_t)::std::stoi(webRuntimeValues.ContentLength.begin()) : 0;
	}
	catch(...) {
		webRuntimeValues.Content.Length						= 0;
	}
	webRuntimeValues.Content.Body.resize(webRuntimeValues.Content.Length);
	memset(webRuntimeValues.Content.Body.begin(), 0, webRuntimeValues.Content.Body.size());
	uint32_t												iChar							= 0;
	int														iArg							= 0;
	while(iChar < webRuntimeValues.Content.Length && (iArg = getc(stdin)) != -1)
		webRuntimeValues.Content.Body[iChar++]				= (char)iArg;
	//webRuntimeValues.Content.Body						= "Name:Pepo\r\nValue:Siso\r\n\r\nName:Pepe\r\nValue:Sise\r\n\r\nName:Pepa\r\nValue:Sisa\r\n\r\n";
	//webRuntimeValues.Content.Length					= webRuntimeValues.Content.Body.size();
	return 0;
}
