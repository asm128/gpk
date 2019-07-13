#define _CRT_SECURE_NO_WARNINGS
#include "gpk_cgi_runtime.h"
#include "gpk_process.h"

#include <string>

static	::gpk::error_t								cgiLoadContentType			(::gpk::CGI_MEDIA_TYPE & contentType, const ::gpk::view_array<const char> & strContentType)	{ 
	ree_if(0 == strContentType.size(), "%s", "No input string");
	static	const ::gpk::view_const_string					content_types []			= 
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

::gpk::error_t										cgiLoadAddr					(::gpk::SIPv4 & remoteIP, const ::gpk::view_array<const char>& strRemoteIP, const ::gpk::view_array<const char>& strRemotePort)	{ 
	if(strRemotePort.size()) {
#if defined(GPK_DISABLE_CPP_EXCEPTIONS)
		remoteIP.Port										= (uint16_t)::std::stoi(strRemotePort.begin());
#else
		try {
			remoteIP.Port										= (uint16_t)::std::stoi(strRemotePort.begin());
		}
		catch(...) {
			remoteIP.Port										= 0;
		}
#endif
	}

	if(strRemoteIP.size()) {
		uint32_t												iOffset						= 0; 
		uint32_t												iEnd						= 0; 
		for(uint32_t iVal = 0; iVal < 4; ++iVal) {
			while(iEnd < strRemoteIP.size()) {
				char curChar = strRemoteIP[iEnd];
				if( curChar == '.' 
				 ||	curChar == ':' 
				 ||	curChar == '\0'
				 || (iEnd - iOffset) > 3	// 3 digit max
				)
					break;
				++iEnd;
			}
#if defined(GPK_DISABLE_CPP_EXCEPTIONS)
			remoteIP.IP[iVal]									= (ubyte_t)::std::stoi({&strRemoteIP[iOffset], iEnd - iOffset});
#else
			try {
				remoteIP.IP[iVal]									= (ubyte_t)::std::stoi({&strRemoteIP[iOffset], iEnd - iOffset});
			}
			catch(...) {
				remoteIP.IP[iVal]									= 0;
			}
#endif
			iOffset												= iEnd + 1;
			iEnd												= iOffset;
		}
	}
	return 0;
}

::gpk::error_t										cgiLoadFormData				(::gpk::SCGIRuntimeValues & runtimeValues, const ::gpk::view_array<const char> & strContent)	{ 
	(void)runtimeValues, (void)strContent;
	return 0;
}

::gpk::error_t										cgiLoadContent				(::gpk::SCGIRuntimeValues & runtimeValues, const ::gpk::CGI_MEDIA_TYPE contentType, const ::gpk::view_array<const char> & strContent)	{ 
	switch(contentType) {
	default: break;
	case ::gpk::CGI_MEDIA_TYPE_MULTIPART_FORM_DATA: 
		::cgiLoadFormData(runtimeValues, strContent); 
		break;
	}
	return 0;
}

::gpk::error_t										gpk::cgiRuntimeValuesLoad	(::gpk::SCGIRuntimeValues & cgiRuntimeValues, const ::gpk::view_array<const char_t *> & argv)	{ 
	cgiRuntimeValues.EntryPointArgs.ArgsCommandLine		= argv;
	::gpk::array_obj<::gpk::TKeyValConstString>				environBlockViews;
	::gpk::environmentBlockFromEnviron(cgiRuntimeValues.EntryPointArgs.EnvironmentBlock);
	::gpk::environmentBlockViews(cgiRuntimeValues.EntryPointArgs.EnvironmentBlock, environBlockViews);
	{
		::gpk::view_const_string								querystring;
		::gpk::find("QUERY_STRING", environBlockViews, querystring);
		::gpk::querystring_split(querystring, cgiRuntimeValues.QueryStringElements);
		cgiRuntimeValues.QueryStringKeyVals.resize(cgiRuntimeValues.QueryStringElements.size());
		for(uint32_t iKeyVal = 0; iKeyVal < cgiRuntimeValues.QueryStringKeyVals.size(); ++iKeyVal) {
			::gpk::TKeyValConstString								& keyValDst				= cgiRuntimeValues.QueryStringKeyVals[iKeyVal];
			::gpk::keyval_split(cgiRuntimeValues.QueryStringElements[iKeyVal], keyValDst);
		}
	}
	{
		::gpk::view_const_string								contentype;
		::gpk::find("CONTENT_TYPE", environBlockViews, contentype);
		if(contentype.size())
			::cgiLoadContentType(cgiRuntimeValues.Content.Type, contentype);
	}
	{
		::gpk::view_const_string								remoteIP	;
		::gpk::view_const_string								remotePORT	;
		::gpk::find("REMOTE_IP"			, environBlockViews, remoteIP	);
		::gpk::find("REMOTE_PORT"		, environBlockViews, remotePORT	);
		::cgiLoadAddr(cgiRuntimeValues.RemoteIP, remoteIP, remotePORT);
	}
	{
		::gpk::view_const_string								contentLength;
		::gpk::find("CONTENT_LENGTH"	, environBlockViews, contentLength);
#if defined(GPK_DISABLE_CPP_EXCEPTIONS)
		cgiRuntimeValues.Content.Length						= contentLength.size() ? (uint32_t)::std::stoi(contentLength.begin()) : 0;
#else
		try {
			cgiRuntimeValues.Content.Length						= contentLength.size() ? (uint32_t)::std::stoi(contentLength.begin()) : 0;
		}
		catch(...) {
			cgiRuntimeValues.Content.Length						= 0;
		}
#endif
	}
	cgiRuntimeValues.Content.Body.resize(cgiRuntimeValues.Content.Length);
	memset(cgiRuntimeValues.Content.Body.begin(), 0, cgiRuntimeValues.Content.Body.size());
	uint32_t												iChar							= 0;
	int														iArg							= 0;
	while(iChar < cgiRuntimeValues.Content.Length && (iArg = getc(stdin)) != -1)
		cgiRuntimeValues.Content.Body[iChar++]				= (char)iArg;

	::cgiLoadContent(cgiRuntimeValues, cgiRuntimeValues.Content.Type, cgiRuntimeValues.Content.Body);
	return 0;
}
