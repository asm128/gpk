/// Copyright 2010-2018 - asm128
#include "gpk_platform.h"

#ifndef GPK_DEBUG_H_97826389432654
#define GPK_DEBUG_H_97826389432654

#if defined(GPK_DEBUG_ENABLED)
#	if defined(GPK_ANDROID) || defined(GPK_LINUX)
#		define GPK_PLATFORM_CRT_BREAKPOINT()	do {} while(0)
#		define GPK_PLATFORM_CRT_CHECK_MEMORY()	do {} while(0)
#	elif defined(GPK_WINDOWS)
#		include <crtdbg.h>
#		define GPK_PLATFORM_CRT_BREAKPOINT		(void)_CrtDbgBreak
#		define GPK_PLATFORM_CRT_CHECK_MEMORY()	do {} while(0) // (void)_CrtCheckMemory
#	else
#		error "Platform not supported."
#	endif
#	define GPK_ERROR_PRINTF_ENABLED	
#	define GPK_WARNING_PRINTF_ENABLED	
#	define GPK_INFO_PRINTF_ENABLED	
//#	define GPK_SUCCESS_PRINTF_ENABLED	// Uncomment as needed
#	ifndef GPK_DISABLE_DEBUG_BREAK_ON_ERROR_LOG
#		define GPK_USE_DEBUG_BREAK_ON_ERROR_LOG
#	endif
#else
#	define GPK_PLATFORM_CRT_BREAKPOINT()		do {} while(0)
#	define GPK_PLATFORM_CRT_CHECK_MEMORY()		do {} while(0)
#	define GPK_ERROR_PRINTF_ENABLED	
#	define GPK_WARNING_PRINTF_ENABLED	// Uncomment as needed
//#	define GPK_INFO_PRINTF_ENABLED		// Uncomment as needed
//#	define GPK_SUCCESS_PRINTF_ENABLED	// Uncomment as needed
#endif

#endif // GPK_DEBUG_H_97826389432654
