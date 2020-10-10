#include "gpk_cgi.h"
#include "gpk_tcpip.h"
#include "gpk_member_registry.h"
#include "gpk_runtime.h"
#include "gpk_http.h"

#ifndef GPK_CGI_RUNTIME_H_2938479283
#define GPK_CGI_RUNTIME_H_2938479283

namespace gpk
{
	static	const ::gpk::view_const_char							cgi_environ	[]		=
		{	::gpk::view_const_string{"AUTH_PASSWORD"		}
		,	::gpk::view_const_string{"AUTH_TYPE"			}
		,	::gpk::view_const_string{"AUTH_USER"			}
		,	::gpk::view_const_string{"CERT_COOKIE"			}
		,	::gpk::view_const_string{"CERT_FLAGS"			}
		,	::gpk::view_const_string{"CERT_ISSUER"			}
		,	::gpk::view_const_string{"CERT_KEYSIZE"			}
		,	::gpk::view_const_string{"CERT_SECRETKEYSIZE"	}
		,	::gpk::view_const_string{"CERT_SERIALNUMBER"	}
		,	::gpk::view_const_string{"CERT_SERVER_ISSUER"	}
		,	::gpk::view_const_string{"CERT_SERVER_SUBJECT"	}
		,	::gpk::view_const_string{"CERT_SUBJECT"			}
		,	::gpk::view_const_string{"CF_TEMPLATE_PATH"		}
		,	::gpk::view_const_string{"CONTENT_LENGTH"		}
		,	::gpk::view_const_string{"CONTENT_TYPE"			}
		,	::gpk::view_const_string{"CONTEXT_PATH"			}
		,	::gpk::view_const_string{"GATEWAY_INTERFACE"	}
		,	::gpk::view_const_string{"HTTPS"				}
		,	::gpk::view_const_string{"HTTPS_KEYSIZE"		}
		,	::gpk::view_const_string{"HTTPS_SECRETKEYSIZE"	}
		,	::gpk::view_const_string{"HTTPS_SERVER_ISSUER"	}
		,	::gpk::view_const_string{"HTTPS_SERVER_SUBJECT"	}
		,	::gpk::view_const_string{"HTTP_ACCEPT"			}
		,	::gpk::view_const_string{"HTTP_ACCEPT_ENCODING"	}
		,	::gpk::view_const_string{"HTTP_ACCEPT_LANGUAGE"	}
		,	::gpk::view_const_string{"HTTP_CONNECTION"		}
		,	::gpk::view_const_string{"HTTP_COOKIE"			}
		,	::gpk::view_const_string{"HTTP_HOST"			}
		,	::gpk::view_const_string{"HTTP_REFERER"			}
		,	::gpk::view_const_string{"HTTP_USER_AGENT"		}
		,	::gpk::view_const_string{"PATH_INFO"			}
		,	::gpk::view_const_string{"QUERY_STRING"			}
		,	::gpk::view_const_string{"REMOTE_ADDR"			}
		,	::gpk::view_const_string{"REMOTE_HOST"			}
		,	::gpk::view_const_string{"REMOTE_USER"			}
		,	::gpk::view_const_string{"REQUEST_METHOD"		}
		,	::gpk::view_const_string{"SCRIPT_NAME"			}
		,	::gpk::view_const_string{"SERVER_NAME"			}
		,	::gpk::view_const_string{"SERVER_PORT"			}
		,	::gpk::view_const_string{"SERVER_PORT_SECURE"	}
		,	::gpk::view_const_string{"SERVER_PROTOCOL"		}
		,	::gpk::view_const_string{"SERVER_SOFTWARE"		}
		,	::gpk::view_const_string{"WEB_SERVER_API"		}
		};

	enum CGI_MEDIA_TYPE
		{ CGI_MEDIA_TYPE_APPLICATION_JAVASCRIPT															// application/javascript
		, CGI_MEDIA_TYPE_APPLICATION_JSON																// application/json
		, CGI_MEDIA_TYPE_APPLICATION_X_WWW_FORM_URLENCODED												// application/x-www-form-urlencoded
		, CGI_MEDIA_TYPE_APPLICATION_XML																// application/xml
		, CGI_MEDIA_TYPE_APPLICATION_ZIP																// application/zip
		, CGI_MEDIA_TYPE_APPLICATION_PDF																// application/pdf
		, CGI_MEDIA_TYPE_APPLICATION_SQL																// application/sql
		, CGI_MEDIA_TYPE_APPLICATION_GRAPHQL															// application/graphql
		, CGI_MEDIA_TYPE_APPLICATION_LD_JSON															// application/ld+json
		, CGI_MEDIA_TYPE_APPLICATION_MSWORD_DOC															// application/msword (.doc)
		, CGI_MEDIA_TYPE_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_WORDPROCESSINGML_DOCUMENT_DOCX	// application/vnd.openxmlformats-officedocument.wordprocessingml.document(.docx)
		, CGI_MEDIA_TYPE_APPLICATION_VND_MS_EXCEL_XLS													// application/vnd.ms-excel (.xls)
		, CGI_MEDIA_TYPE_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_SPREADSHEETML_SHEET_XLSX			// application/vnd.openxmlformats-officedocument.spreadsheetml.sheet (.xlsx)
		, CGI_MEDIA_TYPE_APPLICATION_VND_MS_POWERPOINT_PPT												// application/vnd.ms-powerpoint (.ppt)
		, CGI_MEDIA_TYPE_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_PRESENTATIONML_PRESENTATION_PPTX	// application/vnd.openxmlformats-officedocument.presentationml.presentation (.pptx)
		, CGI_MEDIA_TYPE_APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT_ODT									// application/vnd.oasis.opendocument.text (.odt)
		, CGI_MEDIA_TYPE_AUDIO_MPEG																		// audio/mpeg
		, CGI_MEDIA_TYPE_AUDIO_OGG																		// audio/ogg
		, CGI_MEDIA_TYPE_MULTIPART_FORM_DATA															// multipart/form-data
		, CGI_MEDIA_TYPE_TEXT_CSS																		// text/css
		, CGI_MEDIA_TYPE_TEXT_HTML																		// text/html
		, CGI_MEDIA_TYPE_TEXT_XML																		// text/xml
		, CGI_MEDIA_TYPE_TEXT_CSV																		// text/csv
		, CGI_MEDIA_TYPE_TEXT_PLAIN																		// text/plain
		, CGI_MEDIA_TYPE_IMAGE_PNG																		// image/png
		, CGI_MEDIA_TYPE_IMAGE_JPEG																		// image/jpeg
		, CGI_MEDIA_TYPE_IMAGE_GIF																		// image/gif
		};

	struct SCGIRequestContent {
		uint32_t																				Length						= {};
		::gpk::CGI_MEDIA_TYPE																	Type						= {};
		::gpk::array_pod<char>																	Body						= {};
	};

	struct SCGIRuntimeValues {
		::gpk::array_obj<::gpk::view_const_char>												QueryStringElements			= {};
		::gpk::array_obj<::gpk::TKeyValConstString>												QueryStringKeyVals			= {};
		::gpk::array_obj<::gpk::TKeyValConstString>												FormKeyVals					= {};
		::gpk::array_obj<::gpk::TKeyValConstString>												EnvironViews				= {};
		::gpk::SIPv4																			RemoteIP					= {};
		::gpk::SCGIRequestContent																Content						= {};
		::gpk::SStandardEntryPointArgs															EntryPointArgs				= {};
	};

	::gpk::error_t																			httpRequestInit				(::gpk::SHTTPAPIRequest & requestReceived, const ::gpk::SCGIRuntimeValues & runtimeValues, const bool bLogCGIEnviron);
	::gpk::error_t																			cgiRuntimeValuesLoad		(::gpk::SCGIRuntimeValues & cgiRuntimeValues, const ::gpk::view_array<const char_t *> & argv);
	::gpk::error_t																			cgiMain						(int argc, char** argv, char**envv);

	struct SCGIFramework {
		::gpk::view_const_char																	ModuleName					= {};
		::gpk::SCGIRuntimeValues																RuntimeValues				= {};
		::gpk::SCoord2<int32_t>																	TargetSize					= {};
		bool																					Bootstrapped				= false;
	};
} // namespace

#define GPK_DEFINE_CGI_MODULE_EXPORTS(_mainClass, _moduleTitle)																																																																		\
		::gpk::error_t																			setup					(_mainClass& applicationInstance);																																											\
		::gpk::error_t																			cleanup					(_mainClass& applicationInstance);																																											\
		::gpk::error_t																			update					(_mainClass& applicationInstance, bool systemRequestedExit);																																				\
		::gpk::error_t																			draw					(_mainClass& applicationInstance, ::gpk::array_pod<char_t>& output);																																		\
		::gpk::error_t	GPK_STDCALL																gpk_moduleVersion		()														{ return 1; }																																		\
		::gpk::error_t	GPK_STDCALL																gpk_moduleCreate		(void**	instanceApp, ::gpk::SCGIFramework* framework)	{ try { *instanceApp = new _mainClass{*framework};													return 0;		} catch(...) {} return -1; }	\
		::gpk::error_t	GPK_STDCALL																gpk_moduleDelete		(void**	instanceApp)									{ try { delete ((_mainClass*)*instanceApp);	*instanceApp = 0;										return 0;		} catch(...) {} return -1; }	\
		::gpk::error_t	GPK_STDCALL																gpk_moduleSetup			(void*	instanceApp)									{ try { const ::gpk::error_t result = setup		(*(_mainClass*)instanceApp);						return result;	} catch(...) {} return -1; }	\
		::gpk::error_t	GPK_STDCALL																gpk_moduleCleanup		(void*	instanceApp)									{ try { const ::gpk::error_t result = cleanup	(*(_mainClass*)instanceApp);						return result;	} catch(...) {} return -1; }	\
		::gpk::error_t	GPK_STDCALL																gpk_moduleUpdate		(void*	instanceApp, bool systemRequestedExit)			{ try { const ::gpk::error_t result = update	(*(_mainClass*)instanceApp, systemRequestedExit);	return result;	} catch(...) {} return -1; }	\
		::gpk::error_t	GPK_STDCALL																gpk_moduleRender		(void*	instanceApp, ::gpk::array_pod<char_t>& output)	{ try { const ::gpk::error_t result = draw		(*(_mainClass*)instanceApp, output);				return result;	} catch(...) {} return -1; }	\
		::gpk::error_t	GPK_STDCALL																gpk_moduleTitle			(char* out_title, uint32_t *maxCount)					{																																					\
	static constexpr const char																			mylmoduleTitle[]		= _moduleTitle;	\
	if(0 == out_title) 																															\
		return maxCount ? (*maxCount = ::gpk::size(mylmoduleTitle)) : ::gpk::size(mylmoduleTitle);												\
	memcpy(out_title, mylmoduleTitle, ::gpk::min(::gpk::size(mylmoduleTitle), *maxCount));														\
	return 0;																																	\
}
#endif // GPK_CGI_RUNTIME_H_2938479283
