/// Copyright 2010-2022 - asm128
#include "gpk_platform_globals.h"

// Uncomment these to toggle behavior
//#define GPK_DISABLE_DEBUG_BREAK_ON_ERROR_LOG
//#define GPK_KEEP_SYSTEM_ERROR_ON_ERROR_LOG

#ifndef GPK_DEBUG_H
#define GPK_DEBUG_H

#if defined(GPK_DEBUG_ENABLED)
namespace gpk { stacxpr size_t DEBUG_BUILD = (size_t)-1; } 
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
#	ifndef GPK_WINDOWS
#       ifndef GPK_ESP32
#		    define GPK_PLATFORM_CRT_BREAKPOINT()	do {} while(0)
#		    define GPK_PLATFORM_CRT_CHECK_MEMORY()	do {} while(0)
#     	else
#		    define GPK_PLATFORM_CRT_BREAKPOINT()	do {} while(0)
#       	ifndef GPK_DEBUG_ENABLED
#		        define GPK_PLATFORM_CRT_CHECK_MEMORY()	do {} while(0)
#       	else
#       		include <esp_heap_caps.h>
#       		include <freertos/FreeRTOS.h>
#       		include <freertos/task.h>
#       		define GPK_PLATFORM_CRT_CHECK_MEMORY() do { info_printf("Available RAM - Heap: %u bytes, Stack: %u bytes.", heap_caps_get_free_size(MALLOC_CAP_8BIT), uxTaskGetStackHighWaterMark(NULL)); } while(0)
#       	endif
#       endif
#	else
#		include <crtdbg.h>
#		if defined GPK_USE_DEBUG_BREAK_ON_ERROR_LOG
#			define GPK_PLATFORM_CRT_BREAKPOINT		(void)_CrtDbgBreak
#		else
#			define GPK_PLATFORM_CRT_BREAKPOINT()	do {} while(0)
#		endif
#		define GPK_PLATFORM_CRT_CHECK_MEMORY()	do {} while(0) // (void)_CrtCheckMemory
#	endif
#else
namespace gpk { stacxpr size_t DEBUG_BUILD = 0; } 
#	define GPK_PLATFORM_CRT_BREAKPOINT()		do {} while(0)
#	define GPK_PLATFORM_CRT_CHECK_MEMORY()		do {} while(0)
#	define GPK_ERROR_PRINTF_ENABLED
#	define GPK_WARNING_PRINTF_ENABLED	// Uncomment as needed
#	define GPK_INFO_PRINTF_ENABLED		// Uncomment as needed
//#	define GPK_SUCCESS_PRINTF_ENABLED	// Uncomment as needed
//#	define GPK_VERBOSE_PRINTF_ENABLED	// Uncomment as needed
#endif

#endif // GPK_DEBUG_H
