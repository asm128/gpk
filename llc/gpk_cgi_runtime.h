#include "gpk_cgi.h"
#include "gpk_tcpip.h"
#include "gpk_member_registry.h"
#include "gpk_runtime.h"
#include "gpk_http.h"
#include "gpk_n2.h"

#ifndef GPK_CGI_RUNTIME_H_23627
#define GPK_CGI_RUNTIME_H_23627

namespace gpk
{
	stxp	vcst_c		cgi_environ	[]		=
		{	"AUTH_PASSWORD"
		,	"AUTH_TYPE"
		,	"AUTH_USER"
		,	"CERT_COOKIE"
		,	"CERT_FLAGS"
		,	"CERT_ISSUER"
		,	"CERT_KEYSIZE"
		,	"CERT_SECRETKEYSIZE"
		,	"CERT_SERIALNUMBER"
		,	"CERT_SERVER_ISSUER"
		,	"CERT_SERVER_SUBJECT"
		,	"CERT_SUBJECT"
		,	"CF_TEMPLATE_PATH"
		,	"CONTENT_LENGTH"
		,	"CONTENT_TYPE"
		,	"CONTEXT_PATH"
		,	"GATEWAY_INTERFACE"
		,	"HTTPS"
		,	"HTTPS_KEYSIZE"
		,	"HTTPS_SECRETKEYSIZE"
		,	"HTTPS_SERVER_ISSUER"
		,	"HTTPS_SERVER_SUBJECT"
		,	"HTTP_ACCEPT"
		,	"HTTP_ACCEPT_ENCODING"
		,	"HTTP_ACCEPT_LANGUAGE"
		,	"HTTP_CONNECTION"
		,	"HTTP_COOKIE"
		,	"HTTP_HOST"
		,	"HTTP_REFERER"
		,	"HTTP_USER_AGENT"
		,	"PATH_INFO"
		,	"QUERY_STRING"
		,	"REMOTE_ADDR"
		,	"REMOTE_HOST"
		,	"REMOTE_USER"
		,	"REQUEST_METHOD"
		,	"SCRIPT_NAME"
		,	"SERVER_NAME"
		,	"SERVER_PORT"
		,	"SERVER_PORT_SECURE"
		,	"SERVER_PROTOCOL"
		,	"SERVER_SOFTWARE"
		,	"WEB_SERVER_API"
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
		u2_t				Length						= {};
		CGI_MEDIA_TYPE		Type						= {};
		string				Body						= {};
	};

	struct SCGIRuntimeValues {
		SRuntimeValues		& RuntimeValues				;
		aobj<vcst_t>		QueryStringElements			= {};
		aobj<kv<vcst_t>>	QueryStringKeyVals			= {};
		aobj<kv<vcst_t>>	FormKeyVals					= {};
		SIPv4End			RemoteIP					= {};
		SCGIRequestContent	Content						= {};

							SCGIRuntimeValues			(SRuntimeValues & runtimeValues) : RuntimeValues(runtimeValues) {}
	};

	err_t					httpRequestInit				(SHTTPAPIRequest & requestReceived, const SCGIRuntimeValues & runtimeValues, bool bLogCGIEnviron);
	err_t					cgiRuntimeValuesLoad		(SCGIRuntimeValues & cgiRuntimeValues);
	err_t					cgiMain						(SRuntimeValues & runtimeValues);

	struct SCGIFramework {
		SCGIRuntimeValues	RuntimeValues				;
		vcst_t				ModuleName					= {};
		n2s2_t				TargetSize					= {};
		bool				Bootstrapped				= false;
	};
} // namespace

#define GPK_DEFINE_CGI_MODULE_EXPORTS(_mainClass, _moduleTitle)										\
gpk::err_t 			setup					(_mainClass& app);								\
gpk::err_t 			cleanup					(_mainClass& app);								\
gpk::err_t 			update					(_mainClass& app, bool systemRequestedExit);	\
gpk::err_t 			draw					(_mainClass& app, ::gpk::string & output);	\
gpk::err_t GPK_STDCALL	gpk_moduleVersion		()														{ return 1; }	\
gpk::err_t GPK_STDCALL	gpk_moduleCreate		(void**	instanceApp, ::gpk::SCGIFramework * framework)	{ try { *instanceApp = new _mainClass{*framework};													return 0;		} catch(...) {} return -1; }	\
gpk::err_t GPK_STDCALL	gpk_moduleDelete		(void**	instanceApp)									{ try { delete ((_mainClass*)*instanceApp);	*instanceApp = 0;										return 0;		} catch(...) {} return -1; }	\
gpk::err_t GPK_STDCALL	gpk_moduleSetup			(void*	instanceApp)									{ try { const ::gpk::err_t result = setup		(*(_mainClass*)instanceApp);						return result;	} catch(...) {} return -1; }	\
gpk::err_t GPK_STDCALL	gpk_moduleCleanup		(void*	instanceApp)									{ try { const ::gpk::err_t result = cleanup	(*(_mainClass*)instanceApp);						return result;	} catch(...) {} return -1; }	\
gpk::err_t GPK_STDCALL	gpk_moduleUpdate		(void*	instanceApp, bool systemRequestedExit)			{ try { const ::gpk::err_t result = update	(*(_mainClass*)instanceApp, systemRequestedExit);	return result;	} catch(...) {} return -1; }	\
gpk::err_t GPK_STDCALL	gpk_moduleRender		(void*	instanceApp, ::gpk::apod<sc_t>& output)			{ try { const ::gpk::err_t result = draw		(*(_mainClass*)instanceApp, output);				return result;	} catch(...) {} return -1; }	\
gpk::err_t GPK_STDCALL	gpk_moduleTitle			(sc_t* out_title, u2_t *maxCount)					{	\
	stxp	sc_c					mylmoduleTitle[]		= _moduleTitle;							\
	if(0 == out_title) 																				\
		return maxCount ? (*maxCount = ::gpk::size(mylmoduleTitle)) : ::gpk::size(mylmoduleTitle);	\
	memcpy(out_title, mylmoduleTitle, ::gpk::min(::gpk::size(mylmoduleTitle), *maxCount));			\
	return 0;																						\
}
#endif // GPK_CGI_RUNTIME_H_23627
