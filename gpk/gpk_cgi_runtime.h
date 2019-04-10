#include "gpk_cgi.h"
#include "gpk_tcpip.h"

#ifndef GPK_CGI_RUNTIME_H_2938479283
#define GPK_CGI_RUNTIME_H_2938479283

namespace gpk
{
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
		uint32_t																				Length				= {};
		::gpk::CGI_MEDIA_TYPE																	Type				= {};
		::gpk::array_pod<char>																	Body				= {};
	};

	struct SCGIRuntimeValues {
		::gpk::array_pod<char>																	QueryString			= {};
		::gpk::array_obj<::gpk::view_const_string>												QueryStringElements	= {};
		::gpk::array_obj<::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>>	QueryStringKeyVals	= {};
		::gpk::array_obj<::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>>	FormKeyVals			= {};
		::gpk::array_pod<char>																	ContentLength		= {};
		//::gpk::array_pod<char>																	ContentBody			= {};
		::gpk::array_pod<char>																	ContentType			= {};
		::gpk::array_pod<char>																	StrRemoteIP			= {};
		::gpk::array_pod<char>																	StrRemotePort		= {};
		::gpk::SIPv4																			RemoteIP			= {};
		::gpk::SCGIRequestContent																Content				= {};
	};

	struct SCGIFramework {
		::gpk::view_const_string							ModuleName					= "";
		::gpk::SCGIRuntimeValues							RuntimeValues				= {};
		::gpk::SCoord2<int32_t>								TargetSize					= {};
		bool												Bootstrapped				= false;
	};

	::gpk::error_t										cgiRuntimeValuesLoad		(::gpk::SCGIFramework & framework);
} // gpk

#define GPK_DEFINE_CGI_MODULE_EXPORTS(_mainClass, _moduleTitle)																																																														\
		::gpk::error_t										setup										(_mainClass& applicationInstance);																																											\
		::gpk::error_t										cleanup										(_mainClass& applicationInstance);																																											\
		::gpk::error_t										update										(_mainClass& applicationInstance, bool systemRequestedExit);																																				\
		::gpk::error_t										draw										(_mainClass& applicationInstance, ::gpk::array_pod<char_t>& output);																																		\
		::gpk::error_t	GPK_STDCALL							gpk_moduleVersion							()														{ return 1; }																																		\
		::gpk::error_t	GPK_STDCALL							gpk_moduleCreate							(void**	instanceApp, ::gpk::SCGIFramework* framework)	{ try { *instanceApp = new _mainClass{*framework};													return 0;		} catch(...) {} return -1; }	\
		::gpk::error_t	GPK_STDCALL							gpk_moduleDelete							(void**	instanceApp)									{ try { delete ((_mainClass*)*instanceApp);	*instanceApp = 0;										return 0;		} catch(...) {} return -1; }	\
		::gpk::error_t	GPK_STDCALL							gpk_moduleSetup								(void*	instanceApp)									{ try { const ::gpk::error_t result = setup		(*(_mainClass*)instanceApp);						return result;	} catch(...) {} return -1; }	\
		::gpk::error_t	GPK_STDCALL							gpk_moduleCleanup							(void*	instanceApp)									{ try { const ::gpk::error_t result = cleanup	(*(_mainClass*)instanceApp);						return result;	} catch(...) {} return -1; }	\
		::gpk::error_t	GPK_STDCALL							gpk_moduleUpdate							(void*	instanceApp, bool systemRequestedExit)			{ try { const ::gpk::error_t result = update	(*(_mainClass*)instanceApp, systemRequestedExit);	return result;	} catch(...) {} return -1; }	\
		::gpk::error_t	GPK_STDCALL							gpk_moduleRender							(void*	instanceApp, ::gpk::array_pod<char_t>& output)	{ try { const ::gpk::error_t result = draw		(*(_mainClass*)instanceApp, output);				return result;	} catch(...) {} return -1; }	\
		::gpk::error_t	GPK_STDCALL							gpk_moduleTitle								(char* out_title, uint32_t *maxCount)					{																																					\
	static constexpr const char										mylmoduleTitle[]							= _moduleTitle;	\
	if(0 == out_title) 																											\
		return maxCount ? (*maxCount = ::gpk::size(mylmoduleTitle)) : ::gpk::size(mylmoduleTitle);								\
	memcpy(out_title, mylmoduleTitle, ::gpk::min(::gpk::size(mylmoduleTitle), *maxCount));										\
	return 0;																													\
}
#endif // GPK_CGI_RUNTIME_H_2938479283