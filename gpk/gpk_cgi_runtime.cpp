#include "gpk_cgi_runtime.h"
#include <string>

static	::gpk::error_t								load_env					(const ::gpk::view_const_string& key, ::gpk::array_pod<char> & out_value)	{
	char													* qs						= {};
	size_t													ql							= 0;
	_dupenv_s(&qs, &ql, key.begin());
	if(0 == qs) {
		warning_printf("Environment variable '%s' not found.", key.begin());
		out_value.resize(0);
		return -1;
	}
	gpk_necall(out_value.resize((uint32_t)ql + 1), "%s", "Out of memory?");
	memset(out_value.begin(), 0, out_value.size());
	memcpy(out_value.begin(), qs, ql);
	gpk_necall(out_value.resize(out_value.begin() ? (uint32_t)strlen(out_value.begin()) + 1 : 0), "%s", "Out of memory?");
	free(qs);
	return 0;
}

static	::gpk::error_t								cgiLoadContentType			(::gpk::CGI_MEDIA_TYPE & contentType, const ::gpk::view_array<const char> & strContentType)	{ 
	static const ::gpk::view_const_string					content_types []			= 
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

::gpk::error_t										cgiRuntimeValuesLoadEnv		(::gpk::SCGIRuntimeValues & cgiRuntimeValues)	{ 
	::load_env("AUTH_PASSWORD"			, cgiRuntimeValues.Environment.AUTH_PASSWORD		);
	::load_env("AUTH_TYPE"				, cgiRuntimeValues.Environment.AUTH_TYPE			);
	::load_env("AUTH_USER"				, cgiRuntimeValues.Environment.AUTH_USER			);
	::load_env("CERT_COOKIE"			, cgiRuntimeValues.Environment.CERT_COOKIE			);
	::load_env("CERT_FLAGS"				, cgiRuntimeValues.Environment.CERT_FLAGS			);
	::load_env("CERT_ISSUER"			, cgiRuntimeValues.Environment.CERT_ISSUER			);
	::load_env("CERT_KEYSIZE"			, cgiRuntimeValues.Environment.CERT_KEYSIZE			);
	::load_env("CERT_SECRETKEYSIZE"		, cgiRuntimeValues.Environment.CERT_SECRETKEYSIZE	);
	::load_env("CERT_SERIALNUMBER"		, cgiRuntimeValues.Environment.CERT_SERIALNUMBER	);
	::load_env("CERT_SERVER_ISSUER"		, cgiRuntimeValues.Environment.CERT_SERVER_ISSUER	);
	::load_env("CERT_SERVER_SUBJECT"	, cgiRuntimeValues.Environment.CERT_SERVER_SUBJECT	);
	::load_env("CERT_SUBJECT"			, cgiRuntimeValues.Environment.CERT_SUBJECT			);
	::load_env("CF_TEMPLATE_PATH"		, cgiRuntimeValues.Environment.CF_TEMPLATE_PATH		);
	::load_env("CONTENT_LENGTH"			, cgiRuntimeValues.Environment.CONTENT_LENGTH		);
	::load_env("CONTENT_TYPE"			, cgiRuntimeValues.Environment.CONTENT_TYPE			);
	::load_env("CONTEXT_PATH"			, cgiRuntimeValues.Environment.CONTEXT_PATH			);
	::load_env("GATEWAY_INTERFACE"		, cgiRuntimeValues.Environment.GATEWAY_INTERFACE	);
	::load_env("HTTPS"					, cgiRuntimeValues.Environment.HTTPS				);
	::load_env("HTTPS_KEYSIZE"			, cgiRuntimeValues.Environment.HTTPS_KEYSIZE		);
	::load_env("HTTPS_SECRETKEYSIZE"	, cgiRuntimeValues.Environment.HTTPS_SECRETKEYSIZE	);
	::load_env("HTTPS_SERVER_ISSUER"	, cgiRuntimeValues.Environment.HTTPS_SERVER_ISSUER	);
	::load_env("HTTPS_SERVER_SUBJECT"	, cgiRuntimeValues.Environment.HTTPS_SERVER_SUBJECT	);
	::load_env("HTTP_ACCEPT"			, cgiRuntimeValues.Environment.HTTP_ACCEPT			);
	::load_env("HTTP_ACCEPT_ENCODING"	, cgiRuntimeValues.Environment.HTTP_ACCEPT_ENCODING	);
	::load_env("HTTP_ACCEPT_LANGUAGE"	, cgiRuntimeValues.Environment.HTTP_ACCEPT_LANGUAGE	);
	::load_env("HTTP_CONNECTION"		, cgiRuntimeValues.Environment.HTTP_CONNECTION		);
	::load_env("HTTP_COOKIE"			, cgiRuntimeValues.Environment.HTTP_COOKIE			);
	::load_env("HTTP_HOST"				, cgiRuntimeValues.Environment.HTTP_HOST			);
	::load_env("HTTP_REFERER"			, cgiRuntimeValues.Environment.HTTP_REFERER			);
	::load_env("HTTP_USER_AGENT"		, cgiRuntimeValues.Environment.HTTP_USER_AGENT		);
	::load_env("QUERY_STRING"			, cgiRuntimeValues.Environment.QUERY_STRING			);
	::load_env("REMOTE_ADDR"			, cgiRuntimeValues.Environment.REMOTE_ADDR			);
	::load_env("REMOTE_HOST"			, cgiRuntimeValues.Environment.REMOTE_HOST			);
	::load_env("REMOTE_USER"			, cgiRuntimeValues.Environment.REMOTE_USER			);
	::load_env("REQUEST_METHOD"			, cgiRuntimeValues.Environment.REQUEST_METHOD		);
	::load_env("SCRIPT_NAME"			, cgiRuntimeValues.Environment.SCRIPT_NAME			);
	::load_env("SERVER_NAME"			, cgiRuntimeValues.Environment.SERVER_NAME			);
	::load_env("SERVER_PORT"			, cgiRuntimeValues.Environment.SERVER_PORT			);
	::load_env("SERVER_PORT_SECURE"		, cgiRuntimeValues.Environment.SERVER_PORT_SECURE	);
	::load_env("SERVER_PROTOCOL"		, cgiRuntimeValues.Environment.SERVER_PROTOCOL		);
	::load_env("SERVER_SOFTWARE"		, cgiRuntimeValues.Environment.SERVER_SOFTWARE		);
	::load_env("WEB_SERVER_API"			, cgiRuntimeValues.Environment.WEB_SERVER_API		);
	return 0;
}
::gpk::error_t										cgiLoadFormData				(::gpk::SCGIRuntimeValues & runtimeValues, const ::gpk::view_array<const char> & strContent)	{ 
	runtimeValues, strContent;
	return 0;
}

::gpk::error_t										cgiLoadContent				(::gpk::SCGIRuntimeValues & runtimeValues, const ::gpk::CGI_MEDIA_TYPE contentType, const ::gpk::view_array<const char> & strContent)	{ 
	runtimeValues;
	strContent;
	switch(contentType) {
	default: break;
	case ::gpk::CGI_MEDIA_TYPE_MULTIPART_FORM_DATA: 
		::cgiLoadFormData(runtimeValues, strContent); 
		break;
	}
	return 0;
}

::gpk::error_t										gpk::cgiRuntimeValuesLoad	(::gpk::SCGIRuntimeValues & cgiRuntimeValues)	{ 
	::cgiRuntimeValuesLoadEnv(cgiRuntimeValues);
	cgiRuntimeValues.QueryString						= {cgiRuntimeValues.Environment.QUERY_STRING	.begin(), (uint32_t)strlen(cgiRuntimeValues.Environment.QUERY_STRING	.begin())};	
	cgiRuntimeValues.ContentType						= {cgiRuntimeValues.Environment.CONTENT_TYPE	.begin(), (uint32_t)strlen(cgiRuntimeValues.Environment.CONTENT_TYPE	.begin())};			
	cgiRuntimeValues.ContentLength						= {cgiRuntimeValues.Environment.CONTENT_LENGTH	.begin(), (uint32_t)strlen(cgiRuntimeValues.Environment.CONTENT_LENGTH	.begin())};	
	cgiRuntimeValues.StrRemoteIP						= {cgiRuntimeValues.Environment.REMOTE_ADDR		.begin(), (uint32_t)strlen(cgiRuntimeValues.Environment.REMOTE_ADDR		.begin())};		
	cgiRuntimeValues.StrRemotePort						= {cgiRuntimeValues.Environment.REMOTE_PORT		.begin(), (uint32_t)strlen(cgiRuntimeValues.Environment.REMOTE_PORT		.begin())};	
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
