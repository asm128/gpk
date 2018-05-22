#include "gpk_platform.h"

#ifndef GPK_MODULE_H
#define GPK_MODULE_H

#if defined(GPK_WINDOWS)
#	define GPK_STDCALL				__stdcall
#	define GPK_MODULE_API_EXPORT	__declspec(dllexport)
#	define GPK_MODULE_API_IMPORT	__declspec(dllimport)
#elif defined(GPK_LINUX)
#	define GPK_STDCALL				
#	define GPK_MODULE_API_EXPORT	__attribute__((visibility("default")))
#	define GPK_MODULE_API_IMPORT
#else
//  do nothing and hope for the best?
#	define GPK_STDCALL				
#	define GPK_MODULE_API_EXPORT
#	define GPK_MODULE_API_IMPORT
#	pragma warning "Unknown dynamic link import/export semantics."
#endif

#define GPK_MODULE_FUNCTION_NAME(functionName)	GPK_func_##functionName

#define GPK_DECLARE_MODULE_FUNCTION(functionName, ...) 																	\
	extern "C"	::gpk::error_t		 GPK_STDCALL		functionName									(__VA_ARGS__);	\
	typedef		::gpk::error_t		(GPK_STDCALL		* GPK_MODULE_FUNCTION_NAME(functionName))		(__VA_ARGS__);	\

namespace gpk 
{
	struct SModuleBase {
						void				* Handle				= 0;
	};
}

#endif // GPK_MODULE_H
