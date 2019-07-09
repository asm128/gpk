#include "gpk_cgi.h"
#include "gpk_tcpip.h"
#include "gpk_member_registry.h"
#include "gpk_runtime.h"

#ifndef GPK_CGI_RUNTIME_H_2938479283
#define GPK_CGI_RUNTIME_H_2938479283

namespace gpk
{
	struct SCGIEnvironment {
		GPKMOND(::gpk, array_pod<char_t>,	AUTH_PASSWORD			) = "";
		GPKMOND(::gpk, array_pod<char_t>,	AUTH_TYPE				) = "";
		GPKMOND(::gpk, array_pod<char_t>,	AUTH_USER				) = "";
		GPKMOND(::gpk, array_pod<char_t>,	CERT_COOKIE				) = "";
		GPKMOND(::gpk, array_pod<char_t>,	CERT_FLAGS				) = "";
		GPKMOND(::gpk, array_pod<char_t>,	CERT_ISSUER				) = "";
		GPKMOND(::gpk, array_pod<char_t>,	CERT_KEYSIZE			) = "";
		GPKMOND(::gpk, array_pod<char_t>,	CERT_SECRETKEYSIZE		) = "";
		GPKMOND(::gpk, array_pod<char_t>,	CERT_SERIALNUMBER		) = "";
		GPKMOND(::gpk, array_pod<char_t>,	CERT_SERVER_ISSUER		) = "";
		GPKMOND(::gpk, array_pod<char_t>,	CERT_SERVER_SUBJECT		) = "";
		GPKMOND(::gpk, array_pod<char_t>,	CERT_SUBJECT			) = "";
		GPKMOND(::gpk, array_pod<char_t>,	CF_TEMPLATE_PATH		) = "";
		GPKMOND(::gpk, array_pod<char_t>,	CONTENT_LENGTH			) = "";
		GPKMOND(::gpk, array_pod<char_t>,	CONTENT_TYPE			) = "";
		GPKMOND(::gpk, array_pod<char_t>,	CONTEXT_PATH			) = "";
		GPKMOND(::gpk, array_pod<char_t>,	GATEWAY_INTERFACE		) = "";
		GPKMOND(::gpk, array_pod<char_t>,	HTTPS					) = "";
		GPKMOND(::gpk, array_pod<char_t>,	HTTPS_KEYSIZE			) = "";
		GPKMOND(::gpk, array_pod<char_t>,	HTTPS_SECRETKEYSIZE		) = "";
		GPKMOND(::gpk, array_pod<char_t>,	HTTPS_SERVER_ISSUER		) = "";
		GPKMOND(::gpk, array_pod<char_t>,	HTTPS_SERVER_SUBJECT	) = "";
		GPKMOND(::gpk, array_pod<char_t>,	HTTP_ACCEPT				) = "";
		GPKMOND(::gpk, array_pod<char_t>,	HTTP_ACCEPT_ENCODING	) = "";
		GPKMOND(::gpk, array_pod<char_t>,	HTTP_ACCEPT_LANGUAGE	) = "";
		GPKMOND(::gpk, array_pod<char_t>,	HTTP_CONNECTION			) = "";
		GPKMOND(::gpk, array_pod<char_t>,	HTTP_COOKIE				) = "";
		GPKMOND(::gpk, array_pod<char_t>,	HTTP_HOST				) = "";
		GPKMOND(::gpk, array_pod<char_t>,	HTTP_REFERER			) = "";
		GPKMOND(::gpk, array_pod<char_t>,	HTTP_USER_AGENT			) = "";
		GPKMOND(::gpk, array_pod<char_t>,	QUERY_STRING			) = "";
		GPKMOND(::gpk, array_pod<char_t>,	REMOTE_ADDR				) = "";
		GPKMOND(::gpk, array_pod<char_t>,	REMOTE_HOST				) = "";
		GPKMOND(::gpk, array_pod<char_t>,	REMOTE_USER				) = "";
		GPKMOND(::gpk, array_pod<char_t>,	REQUEST_METHOD			) = "";
		GPKMOND(::gpk, array_pod<char_t>,	SCRIPT_NAME				) = "";
		GPKMOND(::gpk, array_pod<char_t>,	SERVER_NAME				) = "";
		GPKMOND(::gpk, array_pod<char_t>,	SERVER_PORT				) = "";
		GPKMOND(::gpk, array_pod<char_t>,	SERVER_PORT_SECURE		) = "";
		GPKMOND(::gpk, array_pod<char_t>,	SERVER_PROTOCOL			) = "";
		GPKMOND(::gpk, array_pod<char_t>,	SERVER_SOFTWARE			) = "";
		GPKMOND(::gpk, array_pod<char_t>,	WEB_SERVER_API			) = "";
		GPKMOND(::gpk, array_pod<char_t>,	DOCUMENT_ROOT			) = "";// The root directory of your server
		GPKMOND(::gpk, array_pod<char_t>,	PATH					) = "";// The system path your server is running under
		GPKMOND(::gpk, array_pod<char_t>,	REMOTE_PORT				) = "";// The port the visitor is connected to on the web server
		GPKMOND(::gpk, array_pod<char_t>,	REQUEST_URI				) = "";// The interpreted pathname of the requested document or CGI (relative to the document root)
		GPKMOND(::gpk, array_pod<char_t>,	SCRIPT_FILENAME			) = "";// The full pathname of the current CGI
		GPKMOND(::gpk, array_pod<char_t>,	SERVER_ADMIN			) = "";// The email address for your server's webmaster
		GPKM_REGISTRY
			(	GPKM_NAME(AUTH_PASSWORD			)
			,	GPKM_NAME(AUTH_TYPE				)
			,	GPKM_NAME(AUTH_USER				)
			,	GPKM_NAME(CERT_COOKIE			)
			,	GPKM_NAME(CERT_FLAGS			)
			,	GPKM_NAME(CERT_ISSUER			)
			,	GPKM_NAME(CERT_KEYSIZE			)
			,	GPKM_NAME(CERT_SECRETKEYSIZE	)
			,	GPKM_NAME(CERT_SERIALNUMBER		)
			,	GPKM_NAME(CERT_SERVER_ISSUER	)
			,	GPKM_NAME(CERT_SERVER_SUBJECT	)
			,	GPKM_NAME(CERT_SUBJECT			)
			,	GPKM_NAME(CF_TEMPLATE_PATH		)
			,	GPKM_NAME(CONTENT_LENGTH		)
			,	GPKM_NAME(CONTENT_TYPE			)
			,	GPKM_NAME(CONTEXT_PATH			)
			,	GPKM_NAME(GATEWAY_INTERFACE		)
			,	GPKM_NAME(HTTPS					)
			,	GPKM_NAME(HTTPS_KEYSIZE			)
			,	GPKM_NAME(HTTPS_SECRETKEYSIZE	)
			,	GPKM_NAME(HTTPS_SERVER_ISSUER	)
			,	GPKM_NAME(HTTPS_SERVER_SUBJECT	)
			,	GPKM_NAME(HTTP_ACCEPT			)
			,	GPKM_NAME(HTTP_ACCEPT_ENCODING	)
			,	GPKM_NAME(HTTP_ACCEPT_LANGUAGE	)
			,	GPKM_NAME(HTTP_CONNECTION		)
			,	GPKM_NAME(HTTP_COOKIE			)
			,	GPKM_NAME(HTTP_HOST				)
			,	GPKM_NAME(HTTP_REFERER			)
			,	GPKM_NAME(HTTP_USER_AGENT		)
			,	GPKM_NAME(QUERY_STRING			)
			,	GPKM_NAME(REMOTE_ADDR			)
			,	GPKM_NAME(REMOTE_HOST			)
			,	GPKM_NAME(REMOTE_USER			)
			,	GPKM_NAME(REQUEST_METHOD		)
			,	GPKM_NAME(SCRIPT_NAME			)
			,	GPKM_NAME(SERVER_NAME			)
			,	GPKM_NAME(SERVER_PORT			)
			,	GPKM_NAME(SERVER_PORT_SECURE	)
			,	GPKM_NAME(SERVER_PROTOCOL		)
			,	GPKM_NAME(SERVER_SOFTWARE		)
			,	GPKM_NAME(WEB_SERVER_API		)
			,	GPKM_NAME(DOCUMENT_ROOT			)
			,	GPKM_NAME(PATH					)
			,	GPKM_NAME(REMOTE_PORT			)
			,	GPKM_NAME(REQUEST_URI			)
			,	GPKM_NAME(SCRIPT_FILENAME		)
			,	GPKM_NAME(SERVER_ADMIN			)
			);
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
		::gpk::view_const_string																QueryString					= {};
		::gpk::array_obj<::gpk::view_const_string>												QueryStringElements			= {};
		::gpk::array_obj<::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>>	QueryStringKeyVals			= {};
		::gpk::array_obj<::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>>	FormKeyVals					= {};
		::gpk::view_const_string																ContentLength				= {};
		::gpk::view_const_string																ContentType					= {};
		::gpk::view_const_string																StrRemoteIP					= {};
		::gpk::view_const_string																StrRemotePort				= {};
		::gpk::SIPv4																			RemoteIP					= {};
		::gpk::SCGIRequestContent																Content						= {};
		::gpk::SCGIEnvironment																	Environment					= {};
		::gpk::SStandardEntryPointArgs															EntryPointArgs				= {};
	};

	struct SCGIFramework {
		::gpk::view_const_string																ModuleName					= "";
		::gpk::SCGIRuntimeValues																RuntimeValues				= {};
		::gpk::SCoord2<int32_t>																	TargetSize					= {};
		bool																					Bootstrapped				= false;
	};

	::gpk::error_t																			cgiRuntimeValuesLoad		(::gpk::SCGIRuntimeValues & cgiRuntimeValues, const ::gpk::view_array<const char_t *> & argv);
} // gpk

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