#include"gpk_cgi.h"

#ifndef GPK_CGI_RUNTIME_H_2938479283
#define GPK_CGI_RUNTIME_H_2938479283

namespace gpk
{
		struct SCGIRuntimeValues {
		::gpk::array_pod<char>																	QueryString			= {};
		::gpk::array_obj<::gpk::view_const_string>												QueryStringElements	= {};
		::gpk::array_obj<::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>>	QueryStringKeyVals	= {};
		::gpk::array_pod<char>																	ContentLength		= {};
		::gpk::array_pod<char>																	ContentType			= {};
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
	static constexpr const char mylmoduleTitle[] = _moduleTitle;									\
	if(0 == out_title) 																				\
		return maxCount ? (*maxCount = ::gpk::size(mylmoduleTitle)) : ::gpk::size(mylmoduleTitle);	\
	memcpy(out_title, mylmoduleTitle, ::gpk::min(::gpk::size(mylmoduleTitle), *maxCount));			\
	return 0;																						\
}
#endif // GPK_CGI_RUNTIME_H_2938479283