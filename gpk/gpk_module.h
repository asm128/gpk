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
#elif defined(GPK_ANDROID)
#	define GPK_STDCALL				
#	define GPK_MODULE_API_EXPORT	__attribute__((visibility("default")))
#	define GPK_MODULE_API_IMPORT
#else
//  do nothing and hope for the best?
#	define GPK_STDCALL				
#	define GPK_MODULE_API_EXPORT
#	define GPK_MODULE_API_IMPORT
#	error "Unknown dynamic link import/export semantics."
#endif

#define GPK_MODULE_FUNCTION_NAME(functionName)	GPK_func_##functionName

#define GPK_DECLARE_MODULE_FUNCTION(functionName, ...) 																	\
	extern "C"	::gpk::error_t		 GPK_STDCALL			functionName									(__VA_ARGS__);	\
	typedef		::gpk::error_t		(GPK_STDCALL			* GPK_MODULE_FUNCTION_NAME(functionName))		(__VA_ARGS__);	\


#if defined(GPK_WINDOWS)
#	define	GPK_LOAD_MODULE(moduleName)								LoadLibraryA	(moduleName)
#	define	GPK_FREE_MODULE(moduleAddr)								FreeLibrary		(moduleAddr)
#	define	GPK_LOAD_MODULE_FUNCTION(moduleHandle, symbolName)		((GPK_MODULE_FUNCTION_NAME(symbolName))GetProcAddress((HMODULE)moduleHandle, #symbolName))
#else
#	define	GPK_LOAD_MODULE(moduleName)								dlopen	(moduleName, RTLD_GLOBAL)
#	define	GPK_FREE_MODULE(moduleAddr)								dlclose	(moduleAddr)
#	define	GPK_LOAD_MODULE_FUNCTION(moduleHandle, symbolName)		((GPK_MODULE_FUNCTION_NAME(symbolName))dlsym(moduleHandle, #symbolName))
#endif

#if defined GPK_WINDOWS
#	define GPK_MODULE_EXTENSION "dll"
#else
#	define GPK_MODULE_EXTENSION "so"
#endif

//#define GPK_DECLARE_MODULE_EXPORT_FUNCTION(functionName, ...) 																	\
//	extern "C"	::gpk::error_t		 GPK_MODULE_API_EXPORT	functionName									(__VA_ARGS__);	\
//	typedef		::gpk::error_t		(GPK_MODULE_API_EXPORT	* GPK_MODULE_FUNCTION_NAME(functionName))		(__VA_ARGS__);	\
//
namespace gpk 
{
#pragma pack(push, 1)
	struct SModuleBase {
						void				* Handle				= 0;
	};
#pragma pack(pop)
}


#endif // GPK_MODULE_H
