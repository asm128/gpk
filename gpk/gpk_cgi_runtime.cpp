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


::gpk::error_t										cgiLoadContentType				(::gpk::CGI_MEDIA_TYPE & contentType, const ::gpk::view_array<const char> & strContentType)	{ 
	static const ::gpk::view_const_string					content_types []				= 
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
		 if(((val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_JAVASCRIPT															) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_JSON																	) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_X_WWW_FORM_URLENCODED													) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_XML																	) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_ZIP																	) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_PDF																	) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_SQL																	) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_GRAPHQL																) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_LD_JSON																) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_MSWORD_DOC															) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_WORDPROCESSINGML_DOCUMENT_DOCX		) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_VND_MS_EXCEL_XLS														) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_SPREADSHEETML_SHEET_XLSX			) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_VND_MS_POWERPOINT_PPT													) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_PRESENTATIONML_PRESENTATION_PPTX	) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT_ODT										) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_AUDIO_MPEG																		) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_AUDIO_OGG																			) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_MULTIPART_FORM_DATA																) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_TEXT_CSS																			) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_TEXT_HTML																			) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_TEXT_XML																			) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_TEXT_CSV																			) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_TEXT_PLAIN																		) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_IMAGE_PNG																			) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_IMAGE_JPEG																		) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else if(((val = ::gpk::CGI_MEDIA_TYPE_IMAGE_GIF																			) == val) && 0 == strncmp(strContentType.begin(), content_types[val].begin(), content_types[val].size())) { contentType = val; }
	else 													
		return -1;
	return 0;
}

::gpk::error_t										cgiLoadAddr					(::gpk::SIPv4 & remoteIP, const ::gpk::view_array<const char>& strRemoteIP, const ::gpk::view_array<const char>& strRemotePort)	{ 
	if(strRemotePort.size())
		try {
			remoteIP.Port										= (uint16_t)::std::stoi(strRemotePort.begin());
		}
		catch(...) {
			remoteIP.Port										= 0;
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
			try {
				remoteIP.IP[iVal]									= (ubyte_t)::std::stoi({&strRemoteIP[iOffset], iEnd - iOffset});
			}
			catch(...) {
				remoteIP.IP[iVal]									= 0;
			}
			iOffset												= iEnd + 1;
			iEnd												= iOffset;
		}
	}
	return 0;
}

::gpk::error_t										cgiRuntimeValuesLoadEnv		(::gpk::SCGIRuntimeValues & webRuntimeValues)	{ 
	::load_env("QUERY_STRING"	, webRuntimeValues.QueryString		); 
	::load_env("CONTENT_TYPE"	, webRuntimeValues.ContentType		); 
	::load_env("CONTENT_LENGTH"	, webRuntimeValues.ContentLength	); 
	::load_env("REMOTE_ADDR"	, webRuntimeValues.StrRemoteIP		);
	::load_env("REMOTE_PORT"	, webRuntimeValues.StrRemotePort	);
	return 0;
}
::gpk::error_t										cgiLoadFormData				(::gpk::SCGIRuntimeValues & runtimeValues, const ::gpk::view_array<const char> & strContent)	{ 
	runtimeValues, strContent;
	return 0;
}

::gpk::error_t										cgiLoadContent				(::gpk::SCGIRuntimeValues & runtimeValues, const ::gpk::CGI_MEDIA_TYPE contentType, const ::gpk::view_array<const char> & strContent)	{ 
	runtimeValues;strContent;
	switch(contentType) {
	default: break;
	case ::gpk::CGI_MEDIA_TYPE_MULTIPART_FORM_DATA: ::cgiLoadFormData(runtimeValues, strContent); break;
	}
	return 0;
}

::gpk::error_t										gpk::cgiRuntimeValuesLoad	(::gpk::SCGIRuntimeValues & cgiRuntimeValues)	{ 
	::cgiRuntimeValuesLoadEnv(cgiRuntimeValues);
	//webRuntimeValues.QueryString						= "bt=1&width=100&height=200&m=test_cgi_module";
	//webRuntimeValues.QueryString						= "bt=1&width=720&height=640&frame=0&category=0&neighbor=6&m=tianadev";
	//webRuntimeValues.QueryString						= "bt=1&m=tianadev&width=1708&height=744&gallery=0&frame=1";
	::gpk::querystring_split({cgiRuntimeValues.QueryString.begin(), cgiRuntimeValues.QueryString.size()}, cgiRuntimeValues.QueryStringElements);
	cgiRuntimeValues.QueryStringKeyVals.resize(cgiRuntimeValues.QueryStringElements.size());
	for(uint32_t iKeyVal = 0; iKeyVal < cgiRuntimeValues.QueryStringKeyVals.size(); ++iKeyVal) {
		::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>	& keyValDst				= cgiRuntimeValues.QueryStringKeyVals[iKeyVal];
		::gpk::keyval_split(cgiRuntimeValues.QueryStringElements[iKeyVal], keyValDst);
	}
	::cgiLoadContentType(cgiRuntimeValues.Content.Type, cgiRuntimeValues.ContentType);
	::cgiLoadAddr(cgiRuntimeValues.RemoteIP, cgiRuntimeValues.StrRemoteIP, cgiRuntimeValues.StrRemotePort);
	try {
		cgiRuntimeValues.Content.Length						= cgiRuntimeValues.ContentLength.size() ? (uint32_t)::std::stoi(cgiRuntimeValues.ContentLength.begin()) : 0;
	}
	catch(...) {
		cgiRuntimeValues.Content.Length						= 0;
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
