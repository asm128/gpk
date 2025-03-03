/// Copyright 2010-2022 - ogarnd
#include "gpk_platform_globals.h"

#ifdef GPK_ATMEL
#   include <stddef.h>
#else
#   include <cstddef>
#endif

// Uncomment these to toggle behavior
//#define GPK_DISABLE_DEBUG_BREAK_ON_ERROR_LOG
//#define GPK_KEEP_SYSTEM_ERROR_ON_ERROR_LOG

#ifndef GPK_DEBUG_H
#define GPK_DEBUG_H

#ifndef GPK_DEBUG_ENABLED
namespace gpk { stxp size_t DEBUG_BUILD = 0; } 
#	define GPK_PLATFORM_CRT_BREAKPOINT()		do {} while(0)
#	define GPK_PLATFORM_CRT_CHECK_MEMORY()		do {} while(0)
//#	ifndef ARDUINOJSON_ENABLE_COMMENTS
#		define GPK_ERROR_PRINTF_ENABLED
#		define GPK_WARNING_PRINTF_ENABLED	// Uncomment as needed
//#	endif
//#	define GPK_INFO_PRINTF_ENABLED		// Uncomment as needed
//#	define GPK_SUCCESS_PRINTF_ENABLED	// Uncomment as needed
//#	define GPK_VERBOSE_PRINTF_ENABLED	// Uncomment as needed
#else
namespace gpk { stxp size_t DEBUG_BUILD = (size_t)-1; } 
#	define GPK_ERROR_PRINTF_ENABLED
#	define GPK_WARNING_PRINTF_ENABLED
#	define GPK_INFO_PRINTF_ENABLED
//#	define GPK_SUCCESS_PRINTF_ENABLED	// Uncomment as needed
//#	define GPK_VERBOSE_PRINTF_ENABLED
#	ifndef GPK_DISABLE_DEBUG_BREAK_ON_ERROR_LOG
#		define GPK_USE_DEBUG_BREAK_ON_ERROR_LOG
#	endif
#	ifndef GPK_KEEP_SYSTEM_ERROR_ON_ERROR_LOG
#		define GPK_CLEAR_SYSTEM_ERROR_ON_ERROR_LOG
#	endif
#	ifdef GPK_WINDOWS
#		include <crtdbg.h>
#		if defined GPK_USE_DEBUG_BREAK_ON_ERROR_LOG
#			define GPK_PLATFORM_CRT_BREAKPOINT		(void)_CrtDbgBreak
#		else
#			define GPK_PLATFORM_CRT_BREAKPOINT()	do {} while(0)
#		endif
#		define GPK_PLATFORM_CRT_CHECK_MEMORY()	do {} while(0) // (void)_CrtCheckMemory
#	else
#		define GPK_PLATFORM_CRT_BREAKPOINT()	do {} while(0)
#		ifndef GPK_ESP32
#			define GPK_PLATFORM_CRT_CHECK_MEMORY()	do {} while(0)
#		else
#			include <esp_heap_caps.h>
#			include <freertos/FreeRTOS.h>
#			include <freertos/task.h>
#			define GPK_PLATFORM_CRT_CHECK_MEMORY() do { info_printf("Available RAM - Heap: %" GPK_FMT_U2 " bytes, Stack: %" GPK_FMT_U2 " bytes.", heap_caps_get_free_size(MALLOC_CAP_8BIT), uxTaskGetStackHighWaterMark(NULL)); } while(0)
#		endif
#	endif
#endif // GPK_DEBUG_ENABLED

#endif // GPK_DEBUG_H
