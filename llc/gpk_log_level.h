#include "gpk_log_core.h"

#include "gpk_size.h"
#include "gpk_cstring.h"

#ifdef GPK_ATMEL
#	include <stdio.h>
#	include <string.h>
#else
#	include <cstdio>
#endif

#ifndef GPK_LOG_LEVEL_H
#define GPK_LOG_LEVEL_H

namespace gpk
{
	err_t			debug_print_prefix				(s0_t severity, sc_c * path, u2_t line, sc_c * function);
#ifndef GPK_ARDUINO
	void			_gpk_print_system_errors		(sc_c * prefix, u2_t prefixLen);
#endif
#ifndef GPK_ATMEL
	tplt<u2_t fmtLen, tpnm... TArgs>
	sttc	void	_gpk_debug_printf				(s0_t severity, sc_c * path, u2_t line, sc_c * function, sc_c (&format)[fmtLen], cnst TArgs... args)	{
		debug_print_prefix((int8_t)severity, path, line, function);
#else
	tplt<tpnm... TArgs>
	sttc	void	_gpk_debug_printf				(sc_c * function, cnst __FlashStringHelper * format, cnst TArgs... args)			{
		base_log_print_F("{");
		base_log_print(function);
		base_log_print_F("}:");
#endif

#ifdef GPK_ESP32 // Use dynamic string buffer to save flash space.
		/*if(format) */{
			u2_c bufferSize = u2_t(strlen(format) + 1024 * 8);
			if(char * customDynamicString = (char*)malloc(bufferSize)) {
				u2_c 				stringLength		= (u2_t)snprintf(customDynamicString, bufferSize - 2U, format, args...);
				customDynamicString[min(stringLength, bufferSize - 2U)] = '\n';
				customDynamicString[min(stringLength + 1, bufferSize - 1U)] = 0;
				base_log_write(customDynamicString, (int)min(stringLength + 1U, bufferSize - 1U));
				free(customDynamicString);
			}
		}
#else
#	if defined(GPK_ATMEL) || defined(ESP8266)
		sc_t					customDynamicString	[128]		= {};
		u2_c 					stringLength					= (u2_t)snprintf_P(customDynamicString, szof(customDynamicString) - 1U, (sc_c*)format, args...);
		customDynamicString[min(stringLength, szof(customDynamicString) - 1U)] = '\n';
	#else
		sc_t					customDynamicString	[fmtLen + 1024 * 32]	= {};
		u2_c 					stringLength					= (u2_t)::gpk::sprintf_s(customDynamicString, format, args...);
		customDynamicString[min(stringLength, szof(customDynamicString) - 2U)] = '\n';
		if(2 >= severity)
			::gpk::_gpk_print_system_errors("", 0);
#	endif
		base_log_write(customDynamicString, min(szof(customDynamicString), stringLength + 1U));
#endif
	}

	tpl_vtArgs
	sinx	void*		nully				(_tArgs&&...)		{ return 0; }

}

#ifdef GPK_WINDOWS
#	define gpk_debug_printf(severity, format, ...)	::gpk::_gpk_debug_printf(severity, __FILE__, __LINE__, __FUNCTION__, format, __VA_ARGS__)
#else
#	ifdef GPK_ATMEL
#		define gpk_debug_printf(severity, format, ...)	do{ /*base_log_print_F(__FILE__ "(" GPK_TOSTRING(__LINE__) ")");*/ ::gpk::_gpk_debug_printf(__func__, F(format), ##__VA_ARGS__); } while(0) //::gpk::_gpk_debug_printf("(" GPK_TOSTRING(__LINE__) ")", "")
#	else
#		define gpk_debug_printf(severity, format, ...)	::gpk::_gpk_debug_printf(severity, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__)
#	endif
#endif

#ifndef always_printf
#ifndef logf_always
#if !defined(GPK_WINDOWS)
#	define always_printf(format, ...)					gpk_debug_printf(3, format, ##__VA_ARGS__)
#else
#	define always_printf(format, ...)					gpk_debug_printf(3, format, __VA_ARGS__)
#endif
#endif
#endif

#ifndef error_printf
#ifndef logf_error
#	if defined (GPK_ERROR_PRINTF_ENABLED)
#ifndef GPK_WINDOWS
#		define error_printf(format, ...)					do { gpk_debug_printf(1, format, ##__VA_ARGS__); GPK_PLATFORM_CRT_BREAKPOINT(); } while(0)
#else
#		define error_printf(format, ...)					do { gpk_debug_printf(1, format, __VA_ARGS__); GPK_PLATFORM_CRT_BREAKPOINT(); } while(0)
#endif
#	else
#		define error_printf(format, ...)					do { ::gpk::dummy(__VA_ARGS__); GPK_PLATFORM_CRT_BREAKPOINT(); } while(0)
#	endif
#endif
#endif

#ifndef warning_printf
#ifndef logf_warning
#	if defined (GPK_WARNING_PRINTF_ENABLED)
#ifndef GPK_WINDOWS
#		define warning_printf(format, ...)					gpk_debug_printf(2, format, ##__VA_ARGS__)
#else
#		define warning_printf(format, ...)					gpk_debug_printf(2, format, __VA_ARGS__)
#endif
#	else
#		define warning_printf(format, ...)					do { ::gpk::dummy(__VA_ARGS__); } while(0)
#	endif
#endif
#endif

#ifndef info_printf
#ifndef logf_info
#	if defined (GPK_INFO_PRINTF_ENABLED)
#ifndef GPK_WINDOWS
#		define info_printf(format, ...)						gpk_debug_printf(3, format, ##__VA_ARGS__)
#else
#		define info_printf(format, ...)						gpk_debug_printf(3, format, __VA_ARGS__)
#endif
#	else
#		define info_printf(format, ...)						do { ::gpk::dummy(__VA_ARGS__); } while(0)
#	endif
#endif
#endif

#ifndef success_printf
#ifndef logf_success
#	if defined (GPK_SUCCESS_PRINTF_ENABLED)
#ifndef GPK_WINDOWS
#		define success_printf(format, ...)					gpk_debug_printf(4, format, ##__VA_ARGS__)
#else
#		define success_printf(format, ...)					gpk_debug_printf(4, format, __VA_ARGS__)
#endif
#	else
#		define success_printf(format, ...)					do { ::gpk::dummy(__VA_ARGS__); } while(0)
#	endif
#endif
#endif

#ifndef verbose_printf
#ifndef logf_verbose
#	if defined (GPK_VERBOSE_PRINTF_ENABLED)
#ifndef GPK_WINDOWS
#		define verbose_printf(format, ...)					gpk_debug_printf(4, format, ##__VA_ARGS__)
#else
#		define verbose_printf(format, ...)					gpk_debug_printf(4, format, __VA_ARGS__)
#endif
#	else
#		define verbose_printf(format, ...)					do { ::gpk::dummy(__VA_ARGS__); } while(0)
#	endif
#endif
#endif

//
#define gpk_rv_hrcall(retVal, hr_call)					do { ::HRESULT errCall_ = (hr_call); if FAILED(errCall_) { gpk_debug_printf(0, "%s -> %" GPK_FMT_S2 " (0x%X): '%s'.", #hr_call, errCall_, ::gpk::getWindowsErrorAsString(errCall_).begin()); return retVal; } } while(0)
#define gpk_rve_hrcall(retVal, hr_call, format, ...)	do { ::HRESULT errCall_ = (hr_call); if FAILED(errCall_) { gpk_debug_printf(0, "%s -> %" GPK_FMT_S2 " (0x%X): '%s' " format, #hr_call, errCall_, ::gpk::getWindowsErrorAsString(errCall_).begin(), __VA_ARGS__); return retVal; } } while(0)

#define gpk_hrcall(hr_call)				do { gpk_rv_hrcall (-1, hr_call)				; } while(0)		// HRESULT call.
#define gpk_hrecall(hr_call, ...)		do { gpk_rve_hrcall(-1, hr_call, __VA_ARGS__)	; } while(0)		// HRESULT call.

#define gpk_necs(gpkl_call)					do { cnst auto _grr_rslt_val = gpkl_call; if(0 > ::gpk::err_t(_grr_rslt_val)) { error_printf("%s -> %" GPK_FMT_S2 "", #gpkl_call, _grr_rslt_val); return -1; } } while(0) // Non-		propagable error call string.
#ifdef GPK_WINDOWS
#	define gpk_necall(gpkl_call, format, ...)	do { cnst auto _grr_rslt_val = gpkl_call; if(0 > ::gpk::err_t(_grr_rslt_val)) { error_printf("%s -> %" GPK_FMT_S2 ": " format, #gpkl_call, _grr_rslt_val, __VA_ARGS__); return -1; } } while(0)		// Non-propagable error call.
#else
#	define gpk_necall(gpkl_call, format, ...)	do { cnst auto _grr_rslt_val = gpkl_call; if(0 > ::gpk::err_t(_grr_rslt_val)) { error_printf("%s -> %" GPK_FMT_S2 ": " format, #gpkl_call, _grr_rslt_val, ##__VA_ARGS__); return -1; } } while(0)		// Non-propagable error call.
#endif

#ifndef gthrow_if
#	define gsthrow_if(condition)	if_true_log_and_throw(error_printf, (condition))
#	ifndef GPK_WINDOWS
#		define gthrow_if(condition, format, ...)	if_true_logf_and_throw(error_printf, (condition), format, ##__VA_ARGS__)
#	else
#		ifndef GPK_ANDROID
#			define gthrow_if(condition, format, ...)	if_true_logf_and_throw(error_printf, (condition), format, ##__VA_ARGS__)
#		else
#			define gthrow_if(condition, format, ...)	if_true_logf_and_throw(error_printf, (condition), format, __VA_ARGS__)
#		endif
#	endif
#endif

// -- Helpers for the different log levels set during build or runtime.

// - Conditional log
#define if_true_log_always(condition)								if_true_log						(always_printf , (condition))
#define if_true_log_error(condition)								if_true_log						(error_printf  , (condition))
#define if_true_log_warning(condition)								if_true_log						(warning_printf, (condition))
#define if_true_log_info(condition)									if_true_log						(info_printf   , (condition))
#define if_true_log_verbose(condition)								if_true_log						(verbose_printf, (condition))
#define if_fail_log_always(condition)								if_fail_log						(always_printf , (condition))
#define if_fail_log_error(condition)								if_fail_log						(error_printf  , (condition))
#define if_fail_log_warning(condition)								if_fail_log						(warning_printf, (condition))
#define if_fail_log_info(condition)									if_fail_log						(info_printf   , (condition))
#define if_fail_log_verbose(condition)								if_fail_log						(verbose_printf, (condition))
#define if_zero_log_always(condition)								if_zero_log						(always_printf , (condition))
#define if_zero_log_error(condition)								if_zero_log						(error_printf  , (condition))
#define if_zero_log_warning(condition)								if_zero_log						(warning_printf, (condition))
#define if_zero_log_info(condition)									if_zero_log						(info_printf   , (condition))
#define if_zero_log_verbose(condition)								if_zero_log						(verbose_printf, (condition))
#define if_true_log_and_break_always(condition)						if_true_log_and_break			(always_printf , (condition))
#define if_true_log_and_break_error(condition)						if_true_log_and_break			(error_printf  , (condition))
#define if_true_log_and_break_warning(condition)					if_true_log_and_break			(warning_printf, (condition))
#define if_true_log_and_break_info(condition)						if_true_log_and_break			(info_printf   , (condition))
#define if_true_log_and_break_verbose(condition)					if_true_log_and_break			(verbose_printf, (condition))
#define if_fail_log_and_break_always(condition)						if_fail_log_and_break			(always_printf , (condition))
#define if_fail_log_and_break_error(condition)						if_fail_log_and_break			(error_printf  , (condition))
#define if_fail_log_and_break_warning(condition)					if_fail_log_and_break			(warning_printf, (condition))
#define if_fail_log_and_break_info(condition)						if_fail_log_and_break			(info_printf   , (condition))
#define if_fail_log_and_break_verbose(condition)					if_fail_log_and_break			(verbose_printf, (condition))
#define if_zero_log_and_break_always(condition)						if_zero_log_and_break			(always_printf , (condition))
#define if_zero_log_and_break_error(condition)						if_zero_log_and_break			(error_printf  , (condition))
#define if_zero_log_and_break_warning(condition)					if_zero_log_and_break			(warning_printf, (condition))
#define if_zero_log_and_break_info(condition)						if_zero_log_and_break			(info_printf   , (condition))
#define if_zero_log_and_break_verbose(condition)					if_zero_log_and_break			(verbose_printf, (condition))
#define if_true_log_and_continue_always(condition)					if_true_log_and_continue		(always_printf , (condition))
#define if_true_log_and_continue_error(condition)					if_true_log_and_continue		(error_printf  , (condition))
#define if_true_log_and_continue_warning(condition)					if_true_log_and_continue		(warning_printf, (condition))
#define if_true_log_and_continue_info(condition)					if_true_log_and_continue		(info_printf   , (condition))
#define if_true_log_and_continue_verbose(condition)					if_true_log_and_continue		(verbose_printf, (condition))
#define if_fail_log_and_continue_always(condition)					if_fail_log_and_continue		(always_printf , (condition))
#define if_fail_log_and_continue_error(condition)					if_fail_log_and_continue		(error_printf  , (condition))
#define if_fail_log_and_continue_warning(condition)					if_fail_log_and_continue		(warning_printf, (condition))
#define if_fail_log_and_continue_info(condition)					if_fail_log_and_continue		(info_printf   , (condition))
#define if_fail_log_and_continue_verbose(condition)					if_fail_log_and_continue		(verbose_printf, (condition))
#define if_zero_log_and_continue_always(condition)					if_zero_log_and_continue		(always_printf , (condition))
#define if_zero_log_and_continue_error(condition)					if_zero_log_and_continue		(error_printf  , (condition))
#define if_zero_log_and_continue_warning(condition)					if_zero_log_and_continue		(warning_printf, (condition))
#define if_zero_log_and_continue_info(condition)					if_zero_log_and_continue		(info_printf   , (condition))
#define if_zero_log_and_continue_verbose(condition)					if_zero_log_and_continue		(verbose_printf, (condition))
#define if_true_log_and_throw_always(condition)						if_true_log_and_throw			(always_printf , (condition))
#define if_true_log_and_throw_error(condition)						if_true_log_and_throw			(error_printf  , (condition))
#define if_true_log_and_throw_warning(condition)					if_true_log_and_throw			(warning_printf, (condition))
#define if_true_log_and_throw_info(condition)						if_true_log_and_throw			(info_printf   , (condition))
#define if_true_log_and_throw_verbose(condition)					if_true_log_and_throw			(verbose_printf, (condition))
#define if_fail_log_and_throw_always(condition)						if_fail_log_and_throw			(always_printf , (condition))
#define if_fail_log_and_throw_error(condition)						if_fail_log_and_throw			(error_printf  , (condition))
#define if_fail_log_and_throw_warning(condition)					if_fail_log_and_throw			(warning_printf, (condition))
#define if_fail_log_and_throw_info(condition)						if_fail_log_and_throw			(info_printf   , (condition))
#define if_fail_log_and_throw_verbose(condition)					if_fail_log_and_throw			(verbose_printf, (condition))
#define if_zero_log_and_throw_always(condition)						if_zero_log_and_throw			(always_printf , (condition))
#define if_zero_log_and_throw_error(condition)						if_zero_log_and_throw			(error_printf  , (condition))
#define if_zero_log_and_throw_warning(condition)					if_zero_log_and_throw			(warning_printf, (condition))
#define if_zero_log_and_throw_info(condition)						if_zero_log_and_throw			(info_printf   , (condition))
#define if_zero_log_and_throw_verbose(condition)					if_zero_log_and_throw			(verbose_printf, (condition))
#define if_true_log_and_return_always(condition)					if_true_log_and_return			(always_printf , (condition))
#define if_true_log_and_return_error(condition)						if_true_log_and_return			(error_printf  , (condition))
#define if_true_log_and_return_warning(condition)					if_true_log_and_return			(warning_printf, (condition))
#define if_true_log_and_return_info(condition)						if_true_log_and_return			(info_printf   , (condition))
#define if_true_log_and_return_verbose(condition)					if_true_log_and_return			(verbose_printf, (condition))
#define if_fail_log_and_return_always(condition)					if_fail_log_and_return			(always_printf , (condition))
#define if_fail_log_and_return_error(condition)						if_fail_log_and_return			(error_printf  , (condition))
#define if_fail_log_and_return_warning(condition)					if_fail_log_and_return			(warning_printf, (condition))
#define if_fail_log_and_return_info(condition)						if_fail_log_and_return			(info_printf   , (condition))
#define if_fail_log_and_return_verbose(condition)					if_fail_log_and_return			(verbose_printf, (condition))
#define if_zero_log_and_return_always(condition)					if_zero_log_and_return			(always_printf , (condition))
#define if_zero_log_and_return_error(condition)						if_zero_log_and_return			(error_printf  , (condition))
#define if_zero_log_and_return_warning(condition)					if_zero_log_and_return			(warning_printf, (condition))
#define if_zero_log_and_return_info(condition)						if_zero_log_and_return			(info_printf   , (condition))
#define if_zero_log_and_return_verbose(condition)					if_zero_log_and_return			(verbose_printf, (condition))
#define if_true_log_and_return_value_always(value, condition)		if_true_log_and_return_value	(always_printf , (value), (condition))
#define if_true_log_and_return_value_error(value, condition)		if_true_log_and_return_value	(error_printf  , (value), (condition))
#define if_true_log_and_return_value_warning(value, condition)		if_true_log_and_return_value	(warning_printf, (value), (condition))
#define if_true_log_and_return_value_info(value, condition)			if_true_log_and_return_value	(info_printf   , (value), (condition))
#define if_true_log_and_return_value_verbose(value, condition)		if_true_log_and_return_value	(verbose_printf, (value), (condition))
#define if_fail_log_and_return_value_always(value, condition)		if_fail_log_and_return_value	(always_printf , (value), (condition))
#define if_fail_log_and_return_value_error(value, condition)		if_fail_log_and_return_value	(error_printf  , (value), (condition))
#define if_fail_log_and_return_value_warning(value, condition)		if_fail_log_and_return_value	(warning_printf, (value), (condition))
#define if_fail_log_and_return_value_info(value, condition)			if_fail_log_and_return_value	(info_printf   , (value), (condition))
#define if_fail_log_and_return_value_verbose(value, condition)		if_fail_log_and_return_value	(verbose_printf, (value), (condition))
#define if_zero_log_and_return_value_always(value, condition)		if_zero_log_and_return_value	(always_printf , (value), (condition))
#define if_zero_log_and_return_value_error(value, condition)		if_zero_log_and_return_value	(error_printf  , (value), (condition))
#define if_zero_log_and_return_value_warning(value, condition)		if_zero_log_and_return_value	(warning_printf, (value), (condition))
#define if_zero_log_and_return_value_info(value, condition)			if_zero_log_and_return_value	(info_printf   , (value), (condition))
#define if_zero_log_and_return_value_verbose(value, condition)		if_zero_log_and_return_value	(verbose_printf, (value), (condition))
#define if_true_log_and_fail_always(condition)						if_true_log_and_fail(always_printf , (condition))
#define if_true_log_and_fail_error(condition)						if_true_log_and_fail(error_printf  , (condition))
#define if_true_log_and_fail_warning(condition)						if_true_log_and_fail(warning_printf, (condition))
#define if_true_log_and_fail_info(condition)						if_true_log_and_fail(info_printf   , (condition))
#define if_true_log_and_fail_verbose(condition)						if_true_log_and_fail(verbose_printf, (condition))
#define if_fail_log_and_fail_always(condition)						if_fail_log_and_fail(always_printf , (condition))
#define if_fail_log_and_fail_error(condition)						if_fail_log_and_fail(error_printf  , (condition))
#define if_fail_log_and_fail_warning(condition)						if_fail_log_and_fail(warning_printf, (condition))
#define if_fail_log_and_fail_info(condition)						if_fail_log_and_fail(info_printf   , (condition))
#define if_fail_log_and_fail_verbose(condition)						if_fail_log_and_fail(verbose_printf, (condition))
#define if_zero_log_and_fail_always(condition)						if_zero_log_and_fail(always_printf , (condition))
#define if_zero_log_and_fail_error(condition)						if_zero_log_and_fail(error_printf  , (condition))
#define if_zero_log_and_fail_warning(condition)						if_zero_log_and_fail(warning_printf, (condition))
#define if_zero_log_and_fail_info(condition)						if_zero_log_and_fail(info_printf   , (condition))
#define if_zero_log_and_fail_verbose(condition)						if_zero_log_and_fail(verbose_printf, (condition))

#define if_true_logf_always(condition, format, ...)								if_true_logf					(always_printf  , (condition), format, __VA_ARGS__)
#define if_true_logf_error(condition, format, ...)								if_true_logf					(error_printf   , (condition), format, __VA_ARGS__)
#define if_true_logf_warning(condition, format, ...)							if_true_logf					(warning_printf , (condition), format, __VA_ARGS__)
#define if_true_logf_info(condition, format, ...)								if_true_logf					(info_printf    , (condition), format, __VA_ARGS__)
#define if_true_logf_verbose(condition, format, ...)							if_true_logf					(verbose_printf , (condition), format, __VA_ARGS__)
#define if_fail_logf_always(condition, format, ...)								if_fail_logf					(always_printf  , (condition), format, __VA_ARGS__)
#define if_fail_logf_error(condition, format, ...)								if_fail_logf					(error_printf   , (condition), format, __VA_ARGS__)
#define if_fail_logf_warning(condition, format, ...)							if_fail_logf					(warning_printf , (condition), format, __VA_ARGS__)
#define if_fail_logf_info(condition, format, ...)								if_fail_logf					(info_printf    , (condition), format, __VA_ARGS__)
#define if_fail_logf_verbose(condition, format, ...)							if_fail_logf					(verbose_printf , (condition), format, __VA_ARGS__)
#define if_zero_logf_always(condition, format, ...)								if_zero_logf					(always_printf  , (condition), format, __VA_ARGS__)
#define if_zero_logf_error(condition, format, ...)								if_zero_logf					(error_printf   , (condition), format, __VA_ARGS__)
#define if_zero_logf_warning(condition, format, ...)							if_zero_logf					(warning_printf , (condition), format, __VA_ARGS__)
#define if_zero_logf_info(condition, format, ...)								if_zero_logf					(info_printf    , (condition), format, __VA_ARGS__)
#define if_zero_logf_verbose(condition, format, ...)							if_zero_logf					(verbose_printf , (condition), format, __VA_ARGS__)
#define if_true_logf_and_break_always(condition, format, ...)					if_true_logf_and_break			(always_printf  , (condition), format, __VA_ARGS__)
#define if_true_logf_and_break_error(condition, format, ...)					if_true_logf_and_break			(error_printf   , (condition), format, __VA_ARGS__)
#define if_true_logf_and_break_warning(condition, format, ...)					if_true_logf_and_break			(warning_printf , (condition), format, __VA_ARGS__)
#define if_true_logf_and_break_info(condition, format, ...)						if_true_logf_and_break			(info_printf    , (condition), format, __VA_ARGS__)
#define if_true_logf_and_break_verbose(condition, format, ...)					if_true_logf_and_break			(verbose_printf , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_break_always(condition, format, ...)					if_fail_logf_and_break			(always_printf  , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_break_error(condition, format, ...)					if_fail_logf_and_break			(error_printf   , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_break_warning(condition, format, ...)					if_fail_logf_and_break			(warning_printf , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_break_info(condition, format, ...)						if_fail_logf_and_break			(info_printf    , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_break_verbose(condition, format, ...)					if_fail_logf_and_break			(verbose_printf , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_break_always(condition, format, ...)					if_zero_logf_and_break			(always_printf  , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_break_error(condition, format, ...)					if_zero_logf_and_break			(error_printf   , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_break_warning(condition, format, ...)					if_zero_logf_and_break			(warning_printf , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_break_info(condition, format, ...)						if_zero_logf_and_break			(info_printf    , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_break_verbose(condition, format, ...)					if_zero_logf_and_break			(verbose_printf , (condition), format, __VA_ARGS__)
#define if_true_logf_and_continue_always(condition, format, ...)				if_true_logf_and_continue		(always_printf  , (condition), format, __VA_ARGS__)
#define if_true_logf_and_continue_error(condition, format, ...)					if_true_logf_and_continue		(error_printf   , (condition), format, __VA_ARGS__)
#define if_true_logf_and_continue_warning(condition, format, ...)				if_true_logf_and_continue		(warning_printf , (condition), format, __VA_ARGS__)
#define if_true_logf_and_continue_info(condition, format, ...)					if_true_logf_and_continue		(info_printf    , (condition), format, __VA_ARGS__)
#define if_true_logf_and_continue_verbose(condition, format, ...)				if_true_logf_and_continue		(verbose_printf , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_continue_always(condition, format, ...)				if_fail_logf_and_continue		(always_printf  , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_continue_error(condition, format, ...)					if_fail_logf_and_continue		(error_printf   , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_continue_warning(condition, format, ...)				if_fail_logf_and_continue		(warning_printf , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_continue_info(condition, format, ...)					if_fail_logf_and_continue		(info_printf    , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_continue_verbose(condition, format, ...)				if_fail_logf_and_continue		(verbose_printf , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_continue_always(condition, format, ...)				if_zero_logf_and_continue		(always_printf  , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_continue_error(condition, format, ...)					if_zero_logf_and_continue		(error_printf   , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_continue_warning(condition, format, ...)				if_zero_logf_and_continue		(warning_printf , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_continue_info(condition, format, ...)					if_zero_logf_and_continue		(info_printf    , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_continue_verbose(condition, format, ...)				if_zero_logf_and_continue		(verbose_printf , (condition), format, __VA_ARGS__)
#define if_true_logf_and_throw_always(condition, format, ...)					if_true_logf_and_throw			(always_printf  , (condition), format, __VA_ARGS__)
#define if_true_logf_and_throw_error(condition, format, ...)					if_true_logf_and_throw			(error_printf   , (condition), format, __VA_ARGS__)
#define if_true_logf_and_throw_warning(condition, format, ...)					if_true_logf_and_throw			(warning_printf , (condition), format, __VA_ARGS__)
#define if_true_logf_and_throw_info(condition, format, ...)						if_true_logf_and_throw			(info_printf    , (condition), format, __VA_ARGS__)
#define if_true_logf_and_throw_verbose(condition, format, ...)					if_true_logf_and_throw			(verbose_printf , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_throw_always(condition, format, ...)					if_fail_logf_and_throw			(always_printf  , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_throw_error(condition, format, ...)					if_fail_logf_and_throw			(error_printf   , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_throw_warning(condition, format, ...)					if_fail_logf_and_throw			(warning_printf , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_throw_info(condition, format, ...)						if_fail_logf_and_throw			(info_printf    , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_throw_verbose(condition, format, ...)					if_fail_logf_and_throw			(verbose_printf , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_throw_always(condition, format, ...)					if_zero_logf_and_throw			(always_printf  , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_throw_error(condition, format, ...)					if_zero_logf_and_throw			(error_printf   , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_throw_warning(condition, format, ...)					if_zero_logf_and_throw			(warning_printf , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_throw_info(condition, format, ...)						if_zero_logf_and_throw			(info_printf    , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_throw_verbose(condition, format, ...)					if_zero_logf_and_throw			(verbose_printf , (condition), format, __VA_ARGS__)
#define if_true_logf_and_return_always(condition, format, ...)					if_true_logf_and_return			(always_printf  , (condition), format, __VA_ARGS__)
#define if_true_logf_and_return_error(condition, format, ...)					if_true_logf_and_return			(error_printf   , (condition), format, __VA_ARGS__)
#define if_true_logf_and_return_warning(condition, format, ...)					if_true_logf_and_return			(warning_printf , (condition), format, __VA_ARGS__)
#define if_true_logf_and_return_info(condition, format, ...)					if_true_logf_and_return			(info_printf    , (condition), format, __VA_ARGS__)
#define if_true_logf_and_return_verbose(condition, format, ...)					if_true_logf_and_return			(verbose_printf , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_return_always(condition, format, ...)					if_fail_logf_and_return			(always_printf  , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_return_error(condition, format, ...)					if_fail_logf_and_return			(error_printf   , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_return_warning(condition, format, ...)					if_fail_logf_and_return			(warning_printf , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_return_info(condition, format, ...)					if_fail_logf_and_return			(info_printf    , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_return_verbose(condition, format, ...)					if_fail_logf_and_return			(verbose_printf , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_return_always(condition, format, ...)					if_zero_logf_and_return			(always_printf  , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_return_error(condition, format, ...)					if_zero_logf_and_return			(error_printf   , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_return_warning(condition, format, ...)					if_zero_logf_and_return			(warning_printf , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_return_info(condition, format, ...)					if_zero_logf_and_return			(info_printf    , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_return_verbose(condition, format, ...)					if_zero_logf_and_return			(verbose_printf , (condition), format, __VA_ARGS__)
#define if_true_logf_and_return_value_always(value, condition, format, ...)		if_true_logf_and_return_value	(always_printf  , (value), (condition), format, __VA_ARGS__)
#define if_true_logf_and_return_value_error(value, condition, format, ...)		if_true_logf_and_return_value	(error_printf   , (value), (condition), format, __VA_ARGS__)
#define if_true_logf_and_return_value_warning(value, condition, format, ...)	if_true_logf_and_return_value	(warning_printf , (value), (condition), format, __VA_ARGS__)
#define if_true_logf_and_return_value_info(value, condition, format, ...)		if_true_logf_and_return_value	(info_printf    , (value), (condition), format, __VA_ARGS__)
#define if_true_logf_and_return_value_verbose(value, condition, format, ...)	if_true_logf_and_return_value	(verbose_printf , (value), (condition), format, __VA_ARGS__)
#define if_fail_logf_and_return_value_always(value, condition, format, ...)		if_fail_logf_and_return_value	(always_printf  , (value), (condition), format, __VA_ARGS__)
#define if_fail_logf_and_return_value_error(value, condition, format, ...)		if_fail_logf_and_return_value	(error_printf   , (value), (condition), format, __VA_ARGS__)
#define if_fail_logf_and_return_value_warning(value, condition, format, ...)	if_fail_logf_and_return_value	(warning_printf , (value), (condition), format, __VA_ARGS__)
#define if_fail_logf_and_return_value_info(value, condition, format, ...)		if_fail_logf_and_return_value	(info_printf    , (value), (condition), format, __VA_ARGS__)
#define if_fail_logf_and_return_value_verbose(value, condition, format, ...)	if_fail_logf_and_return_value	(verbose_printf , (value), (condition), format, __VA_ARGS__)
#define if_zero_logf_and_return_value_always(value, condition, format, ...)		if_zero_logf_and_return_value	(always_printf  , (value), (condition), format, __VA_ARGS__)
#define if_zero_logf_and_return_value_error(value, condition, format, ...)		if_zero_logf_and_return_value	(error_printf   , (value), (condition), format, __VA_ARGS__)
#define if_zero_logf_and_return_value_warning(value, condition, format, ...)	if_zero_logf_and_return_value	(warning_printf , (value), (condition), format, __VA_ARGS__)
#define if_zero_logf_and_return_value_info(value, condition, format, ...)		if_zero_logf_and_return_value	(info_printf    , (value), (condition), format, __VA_ARGS__)
#define if_zero_logf_and_return_value_verbose(value, condition, format, ...)	if_zero_logf_and_return_value	(verbose_printf , (value), (condition), format, __VA_ARGS__)
#define if_true_logf_and_fail_always(condition, format, ...)	if_true_logf_and_fail(always_printf  , (condition), format, __VA_ARGS__)
#define if_true_logf_and_fail_error(condition, format, ...)		if_true_logf_and_fail(error_printf   , (condition), format, __VA_ARGS__)
#define if_true_logf_and_fail_warning(condition, format, ...)	if_true_logf_and_fail(warning_printf , (condition), format, __VA_ARGS__)
#define if_true_logf_and_fail_info(condition, format, ...)		if_true_logf_and_fail(info_printf    , (condition), format, __VA_ARGS__)
#define if_true_logf_and_fail_verbose(condition, format, ...)	if_true_logf_and_fail(verbose_printf , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_fail_always(condition, format, ...)	if_fail_logf_and_fail(always_printf  , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_fail_error(condition, format, ...)		if_fail_logf_and_fail(error_printf   , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_fail_warning(condition, format, ...)	if_fail_logf_and_fail(warning_printf , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_fail_info(condition, format, ...)		if_fail_logf_and_fail(info_printf    , (condition), format, __VA_ARGS__)
#define if_fail_logf_and_fail_verbose(condition, format, ...)	if_fail_logf_and_fail(verbose_printf , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_fail_always(condition, format, ...)	if_zero_logf_and_fail(always_printf  , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_fail_error(condition, format, ...)		if_zero_logf_and_fail(error_printf   , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_fail_warning(condition, format, ...)	if_zero_logf_and_fail(warning_printf , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_fail_info(condition, format, ...)		if_zero_logf_and_fail(info_printf    , (condition), format, __VA_ARGS__)
#define if_zero_logf_and_fail_verbose(condition, format, ...)	if_zero_logf_and_fail(verbose_printf , (condition), format, __VA_ARGS__)

#define if_null_log_always							if_zero_log_always
#define if_null_log_error							if_zero_log_error
#define if_null_log_warning							if_zero_log_warning
#define if_null_log_info							if_zero_log_info
#define if_null_log_verbose							if_zero_log_verbose
#define if_null_log_and_break_always				if_zero_log_and_break_always
#define if_null_log_and_break_error					if_zero_log_and_break_error
#define if_null_log_and_break_warning				if_zero_log_and_break_warning
#define if_null_log_and_break_info					if_zero_log_and_break_info
#define if_null_log_and_break_verbose				if_zero_log_and_break_verbose
#define if_null_log_and_continue_always				if_zero_log_and_continue_always
#define if_null_log_and_continue_error				if_zero_log_and_continue_error
#define if_null_log_and_continue_warning			if_zero_log_and_continue_warning
#define if_null_log_and_continue_info				if_zero_log_and_continue_info
#define if_null_log_and_continue_verbose			if_zero_log_and_continue_verbose
#define if_null_log_and_throw_always				if_zero_log_and_throw_always
#define if_null_log_and_throw_error					if_zero_log_and_throw_error
#define if_null_log_and_throw_warning				if_zero_log_and_throw_warning
#define if_null_log_and_throw_info					if_zero_log_and_throw_info
#define if_null_log_and_throw_verbose				if_zero_log_and_throw_verbose
#define if_null_log_and_return_always				if_zero_log_and_return_always
#define if_null_log_and_return_error				if_zero_log_and_return_error
#define if_null_log_and_return_warning				if_zero_log_and_return_warning
#define if_null_log_and_return_info					if_zero_log_and_return_info
#define if_null_log_and_return_verbose				if_zero_log_and_return_verbose
#define if_null_log_and_return_value_always			if_zero_log_and_return_value_always
#define if_null_log_and_return_value_error			if_zero_log_and_return_value_error
#define if_null_log_and_return_value_warning		if_zero_log_and_return_value_warning
#define if_null_log_and_return_value_info			if_zero_log_and_return_value_info
#define if_null_log_and_return_value_verbose		if_zero_log_and_return_value_verbose
#define if_null_log_and_fail_always					if_zero_log_and_fail_always
#define if_null_log_and_fail_error					if_zero_log_and_fail_error
#define if_null_log_and_fail_warning				if_zero_log_and_fail_warning
#define if_null_log_and_fail_info					if_zero_log_and_fail_info
#define if_null_log_and_fail_verbose				if_zero_log_and_fail_verbose

#define if_null_logf_always						if_zero_logf_always
#define if_null_logf_error						if_zero_logf_error
#define if_null_logf_warning					if_zero_logf_warning
#define if_null_logf_info						if_zero_logf_info
#define if_null_logf_verbose					if_zero_logf_verbose
#define if_null_logf_and_break_always			if_zero_logf_and_break_always
#define if_null_logf_and_break_error			if_zero_logf_and_break_error
#define if_null_logf_and_break_warning			if_zero_logf_and_break_warning
#define if_null_logf_and_break_info				if_zero_logf_and_break_info
#define if_null_logf_and_break_verbose			if_zero_logf_and_break_verbose
#define if_null_logf_and_continue_always		if_zero_logf_and_continue_always
#define if_null_logf_and_continue_error			if_zero_logf_and_continue_error
#define if_null_logf_and_continue_warning		if_zero_logf_and_continue_warning
#define if_null_logf_and_continue_info			if_zero_logf_and_continue_info
#define if_null_logf_and_continue_verbose		if_zero_logf_and_continue_verbose
#define if_null_logf_and_throw_always			if_zero_logf_and_throw_always
#define if_null_logf_and_throw_error			if_zero_logf_and_throw_error
#define if_null_logf_and_throw_warning			if_zero_logf_and_throw_warning
#define if_null_logf_and_throw_info				if_zero_logf_and_throw_info
#define if_null_logf_and_throw_verbose			if_zero_logf_and_throw_verbose
#define if_null_logf_and_return_always			if_zero_logf_and_return_always
#define if_null_logf_and_return_error			if_zero_logf_and_return_error
#define if_null_logf_and_return_warning			if_zero_logf_and_return_warning
#define if_null_logf_and_return_info			if_zero_logf_and_return_info
#define if_null_logf_and_return_value_always	if_zero_logf_and_return_value_always
#define if_null_logf_and_return_value_error		if_zero_logf_and_return_value_error
#define if_null_logf_and_return_value_warning	if_zero_logf_and_return_value_warning
#define if_null_logf_and_return_value_info		if_zero_logf_and_return_value_info
#define if_null_logf_and_return_value_verbose	if_zero_logf_and_return_value_verbose
#define if_null_logf_and_fail_always			if_zero_logf_and_fail_always
#define if_null_logf_and_fail_error				if_zero_logf_and_fail_error
#define if_null_logf_and_fail_warning			if_zero_logf_and_fail_warning
#define if_null_logf_and_fail_info				if_zero_logf_and_fail_info
#define if_null_logf_and_fail_verbose			if_zero_logf_and_fail_verbose
//
#define if_true_a  if_true_log_always
#define if_true_e  if_true_log_error
#define if_true_w  if_true_log_warning
#define if_true_i  if_true_log_info
#define if_true_v  if_true_log_verbose
#define if_fail_a  if_fail_log_always
#define if_fail_e  if_fail_log_error
#define if_fail_w  if_fail_log_warning
#define if_fail_i  if_fail_log_info
#define if_fail_v  if_fail_log_verbose
#define if_zero_a  if_zero_log_always
#define if_zero_e  if_zero_log_error
#define if_zero_w  if_zero_log_warning
#define if_zero_i  if_zero_log_info
#define if_zero_v  if_zero_log_verbose
#define if_null_a  if_null_log_always
#define if_null_e  if_null_log_error
#define if_null_w  if_null_log_warning
#define if_null_i  if_null_log_info
#define if_null_v  if_null_log_verbose
#define if_true_af if_true_logf_always
#define if_true_ef if_true_logf_error
#define if_true_wf if_true_logf_warning
#define if_true_if if_true_logf_info
#define if_true_vf if_true_logf_verbose
#define if_fail_af if_fail_logf_always
#define if_fail_ef if_fail_logf_error
#define if_fail_wf if_fail_logf_warning
#define if_fail_if if_fail_logf_info
#define if_fail_vf if_fail_logf_verbose
#define if_zero_af if_zero_logf_always
#define if_zero_ef if_zero_logf_error
#define if_zero_wf if_zero_logf_warning
#define if_zero_if if_zero_logf_info
#define if_zero_vf if_zero_logf_verbose
#define if_null_af if_null_logf_always
#define if_null_ef if_null_logf_error
#define if_null_wf if_null_logf_warning
#define if_null_if if_null_logf_info
#define if_null_vf if_null_logf_verbose

//
#define if_true_ba  if_true_log_and_break_always
#define if_true_be  if_true_log_and_break_error
#define if_true_bw  if_true_log_and_break_warning
#define if_true_bi  if_true_log_and_break_info
#define if_true_bv  if_true_log_and_break_verbose
#define if_fail_ba  if_fail_log_and_break_always
#define if_fail_be  if_fail_log_and_break_error
#define if_fail_bw  if_fail_log_and_break_warning
#define if_fail_bi  if_fail_log_and_break_info
#define if_fail_bv  if_fail_log_and_break_verbose
#define if_zero_ba  if_zero_log_and_break_always
#define if_zero_be  if_zero_log_and_break_error
#define if_zero_bw  if_zero_log_and_break_warning
#define if_zero_bi  if_zero_log_and_break_info
#define if_zero_bv  if_zero_log_and_break_verbose
#define if_null_ba  if_null_log_and_break_always
#define if_null_be  if_null_log_and_break_error
#define if_null_bw  if_null_log_and_break_warning
#define if_null_bi  if_null_log_and_break_info
#define if_null_bv  if_null_log_and_break_verbose
#define if_true_baf if_true_logf_and_break_always
#define if_true_bef if_true_logf_and_break_error
#define if_true_bwf if_true_logf_and_break_warning
#define if_true_bif if_true_logf_and_break_info
#define if_true_bvf if_true_logf_and_break_verbose
#define if_fail_baf if_fail_logf_and_break_always
#define if_fail_bef if_fail_logf_and_break_error
#define if_fail_bwf if_fail_logf_and_break_warning
#define if_fail_bif if_fail_logf_and_break_info
#define if_fail_bvf if_fail_logf_and_break_verbose
#define if_zero_baf if_zero_logf_and_break_always
#define if_zero_bef if_zero_logf_and_break_error
#define if_zero_bwf if_zero_logf_and_break_warning
#define if_zero_bif if_zero_logf_and_break_info
#define if_zero_bvf if_zero_logf_and_break_verbose
#define if_null_baf if_null_logf_and_break_always
#define if_null_bef if_null_logf_and_break_error
#define if_null_bwf if_null_logf_and_break_warning
#define if_null_bif if_null_logf_and_break_info
#define if_null_bvf if_null_logf_and_break_verbose
//
#define if_true_ca  if_true_log_and_continue_always
#define if_true_ce  if_true_log_and_continue_error
#define if_true_cw  if_true_log_and_continue_warning
#define if_true_ci  if_true_log_and_continue_info
#define if_true_cv  if_true_log_and_continue_verbose
#define if_fail_ca  if_fail_log_and_continue_always
#define if_fail_ce  if_fail_log_and_continue_error
#define if_fail_cw  if_fail_log_and_continue_warning
#define if_fail_ci  if_fail_log_and_continue_info
#define if_fail_cv  if_fail_log_and_continue_verbose
#define if_zero_ca  if_zero_log_and_continue_always
#define if_zero_ce  if_zero_log_and_continue_error
#define if_zero_cw  if_zero_log_and_continue_warning
#define if_zero_ci  if_zero_log_and_continue_info
#define if_zero_cv  if_zero_log_and_continue_verbose
#define if_null_ca  if_null_log_and_continue_always
#define if_null_ce  if_null_log_and_continue_error
#define if_null_cw  if_null_log_and_continue_warning
#define if_null_ci  if_null_log_and_continue_info
#define if_null_cv  if_null_log_and_continue_verbose
#define if_true_caf if_true_logf_and_continue_always
#define if_true_cef if_true_logf_and_continue_error
#define if_true_cwf if_true_logf_and_continue_warning
#define if_true_cif if_true_logf_and_continue_info
#define if_true_cvf if_true_logf_and_continue_verbose
#define if_fail_caf if_fail_logf_and_continue_always
#define if_fail_cef if_fail_logf_and_continue_error
#define if_fail_cwf if_fail_logf_and_continue_warning
#define if_fail_cif if_fail_logf_and_continue_info
#define if_fail_cvf if_fail_logf_and_continue_verbose
#define if_zero_caf if_zero_logf_and_continue_always
#define if_zero_cef if_zero_logf_and_continue_error
#define if_zero_cwf if_zero_logf_and_continue_warning
#define if_zero_cif if_zero_logf_and_continue_info
#define if_zero_cvf if_zero_logf_and_continue_verbose
#define if_null_caf if_null_logf_and_continue_always
#define if_null_cef if_null_logf_and_continue_error
#define if_null_cwf if_null_logf_and_continue_warning
#define if_null_cif if_null_logf_and_continue_info
#define if_null_cvf if_null_logf_and_continue_verbose
//
#define if_true_va  if_true_log_and_return_value_always
#define if_true_ve  if_true_log_and_return_value_error
#define if_true_vw  if_true_log_and_return_value_warning
#define if_true_vi  if_true_log_and_return_value_info
#define if_true_vv  if_true_log_and_return_value_verbose
#define if_fail_va  if_fail_log_and_return_value_always
#define if_fail_ve  if_fail_log_and_return_value_error
#define if_fail_vw  if_fail_log_and_return_value_warning
#define if_fail_vi  if_fail_log_and_return_value_info
#define if_fail_vv  if_fail_log_and_return_value_verbose
#define if_zero_va  if_zero_log_and_return_value_always
#define if_zero_ve  if_zero_log_and_return_value_error
#define if_zero_vw  if_zero_log_and_return_value_warning
#define if_zero_vi  if_zero_log_and_return_value_info
#define if_zero_vv  if_zero_log_and_return_value_verbose
#define if_null_va  if_null_log_and_return_value_always
#define if_null_ve  if_null_log_and_return_value_error
#define if_null_vw  if_null_log_and_return_value_warning
#define if_null_vi  if_null_log_and_return_value_info
#define if_null_vv  if_null_log_and_return_value_verbose
#define if_true_vaf if_true_logf_and_return_value_always
#define if_true_vef if_true_logf_and_return_value_error
#define if_true_vwf if_true_logf_and_return_value_warning
#define if_true_vif if_true_logf_and_return_value_info
#define if_true_vvf if_true_logf_and_return_value_verbose
#define if_fail_vaf if_fail_logf_and_return_value_always
#define if_fail_vef if_fail_logf_and_return_value_error
#define if_fail_vwf if_fail_logf_and_return_value_warning
#define if_fail_vif if_fail_logf_and_return_value_info
#define if_fail_vvf if_fail_logf_and_return_value_verbose
#define if_zero_vaf if_zero_logf_and_return_value_always
#define if_zero_vef if_zero_logf_and_return_value_error
#define if_zero_vwf if_zero_logf_and_return_value_warning
#define if_zero_vif if_zero_logf_and_return_value_info
#define if_zero_vvf if_zero_logf_and_return_value_verbose
#define if_null_vaf if_null_logf_and_return_value_always
#define if_null_vef if_null_logf_and_return_value_error
#define if_null_vwf if_null_logf_and_return_value_warning
#define if_null_vif if_null_logf_and_return_value_info
#define if_null_vvf if_null_logf_and_return_value_verbose
////
#define if_true_ra  if_true_log_and_return_always
#define if_true_re  if_true_log_and_return_error
#define if_true_rw  if_true_log_and_return_warning
#define if_true_ri  if_true_log_and_return_info
#define if_true_rv  if_true_log_and_return_verbose
#define if_fail_ra  if_fail_log_and_return_always
#define if_fail_re  if_fail_log_and_return_error
#define if_fail_rw  if_fail_log_and_return_warning
#define if_fail_ri  if_fail_log_and_return_info
#define if_fail_rv  if_fail_log_and_return_verbose
#define if_zero_ra  if_zero_log_and_return_always
#define if_zero_re  if_zero_log_and_return_error
#define if_zero_rw  if_zero_log_and_return_warning
#define if_zero_ri  if_zero_log_and_return_info
#define if_zero_rv  if_zero_log_and_return_verbose
#define if_null_ra  if_null_log_and_return_always
#define if_null_re  if_null_log_and_return_error
#define if_null_rw  if_null_log_and_return_warning
#define if_null_ri  if_null_log_and_return_info
#define if_null_rv  if_null_log_and_return_verbose
#define if_true_raf if_true_logf_and_return_always
#define if_true_ref if_true_logf_and_return_error
#define if_true_rwf if_true_logf_and_return_warning
#define if_true_rif if_true_logf_and_return_info
#define if_true_rvf if_true_logf_and_return_verbose
#define if_fail_raf if_fail_logf_and_return_always
#define if_fail_ref if_fail_logf_and_return_error
#define if_fail_rwf if_fail_logf_and_return_warning
#define if_fail_rif if_fail_logf_and_return_info
#define if_fail_rvf if_fail_logf_and_return_verbose
#define if_zero_raf if_zero_logf_and_return_always
#define if_zero_ref if_zero_logf_and_return_error
#define if_zero_rwf if_zero_logf_and_return_warning
#define if_zero_rif if_zero_logf_and_return_info
#define if_zero_rvf if_zero_logf_and_return_verbose
#define if_null_raf if_null_logf_and_return_always
#define if_null_ref if_null_logf_and_return_error
#define if_null_rwf if_null_logf_and_return_warning
#define if_null_rif if_null_logf_and_return_info
#define if_null_rvf if_null_logf_and_return_verbose
//
#define if_true_fa  if_true_log_and_fail_always
#define if_true_fe  if_true_log_and_fail_error
#define if_true_fw  if_true_log_and_fail_warning
#define if_true_fi  if_true_log_and_fail_info
#define if_true_fv  if_true_log_and_fail_verbose
#define if_fail_fa  if_fail_log_and_fail_always
#define if_fail_fe  if_fail_log_and_fail_error
#define if_fail_fw  if_fail_log_and_fail_warning
#define if_fail_fi  if_fail_log_and_fail_info
#define if_fail_fv  if_fail_log_and_fail_verbose
#define if_zero_fa  if_zero_log_and_fail_always
#define if_zero_fe  if_zero_log_and_fail_error
#define if_zero_fw  if_zero_log_and_fail_warning
#define if_zero_fi  if_zero_log_and_fail_info
#define if_zero_fv  if_zero_log_and_fail_verbose
#define if_null_fa  if_null_log_and_fail_always
#define if_null_fe  if_null_log_and_fail_error
#define if_null_fw  if_null_log_and_fail_warning
#define if_null_fi  if_null_log_and_fail_info
#define if_null_fv  if_null_log_and_fail_verbose
#define if_true_faf if_true_logf_and_fail_always
#define if_true_fef if_true_logf_and_fail_error
#define if_true_fwf if_true_logf_and_fail_warning
#define if_true_fif if_true_logf_and_fail_info
#define if_true_fvf if_true_logf_and_fail_verbose
#define if_fail_faf if_fail_logf_and_fail_always
#define if_fail_fef if_fail_logf_and_fail_error
#define if_fail_fwf if_fail_logf_and_fail_warning
#define if_fail_fif if_fail_logf_and_fail_info
#define if_fail_fvf if_fail_logf_and_fail_verbose
#define if_zero_faf if_zero_logf_and_fail_always
#define if_zero_fef if_zero_logf_and_fail_error
#define if_zero_fwf if_zero_logf_and_fail_warning
#define if_zero_fif if_zero_logf_and_fail_info
#define if_zero_fvf if_zero_logf_and_fail_verbose
#define if_null_faf if_null_logf_and_fail_always
#define if_null_fef if_null_logf_and_fail_error
#define if_null_fwf if_null_logf_and_fail_warning
#define if_null_fif if_null_logf_and_fail_info
#define if_null_fvf if_null_logf_and_fail_verbose
//
#define if_true_ta	if_true_log_and_throw_always
#define if_true_te	if_true_log_and_throw_error
#define if_true_tw	if_true_log_and_throw_warning
#define if_true_ti	if_true_log_and_throw_info
#define if_true_tv	if_true_log_and_throw_verbose
#define if_fail_ta	if_fail_log_and_throw_always
#define if_fail_te	if_fail_log_and_throw_error
#define if_fail_tw	if_fail_log_and_throw_warning
#define if_fail_ti	if_fail_log_and_throw_info
#define if_fail_tv	if_fail_log_and_throw_verbose
#define if_zero_ta	if_zero_log_and_throw_always
#define if_zero_te	if_zero_log_and_throw_error
#define if_zero_tw	if_zero_log_and_throw_warning
#define if_zero_ti	if_zero_log_and_throw_info
#define if_zero_tv	if_zero_log_and_throw_verbose
#define if_null_ta	if_null_log_and_throw_always
#define if_null_te	if_null_log_and_throw_error
#define if_null_tw	if_null_log_and_throw_warning
#define if_null_ti	if_null_log_and_throw_info
#define if_null_tv	if_null_log_and_throw_verbose
#define if_true_taf	if_true_logf_and_throw_always
#define if_true_tef	if_true_logf_and_throw_error
#define if_true_twf	if_true_logf_and_throw_warning
#define if_true_tif	if_true_logf_and_throw_info
#define if_true_tvf	if_true_logf_and_throw_verbose
#define if_fail_taf	if_fail_logf_and_throw_always
#define if_fail_tef	if_fail_logf_and_throw_error
#define if_fail_twf	if_fail_logf_and_throw_warning
#define if_fail_tif	if_fail_logf_and_throw_info
#define if_fail_tvf	if_fail_logf_and_throw_verbose
#define if_zero_taf	if_zero_logf_and_throw_always
#define if_zero_tef	if_zero_logf_and_throw_error
#define if_zero_twf	if_zero_logf_and_throw_warning
#define if_zero_tif	if_zero_logf_and_throw_info
#define if_zero_tvf	if_zero_logf_and_throw_verbose
#define if_null_taf	if_null_logf_and_throw_always
#define if_null_tef	if_null_logf_and_throw_error
#define if_null_twf	if_null_logf_and_throw_warning
#define if_null_tif	if_null_logf_and_throw_info
#define if_null_tvf	if_null_logf_and_throw_verbose
//
#define a_if       if_true_a
#define e_if       if_true_e
#define w_if       if_true_w
#define i_if       if_true_i
#define v_if       if_true_v
#define a_if_fail  if_fail_a
#define e_if_fail  if_fail_e
#define w_if_fail  if_fail_w
#define i_if_fail  if_fail_i
#define v_if_fail  if_fail_v
#define a_if_zero  if_zero_a
#define e_if_zero  if_zero_e
#define w_if_zero  if_zero_w
#define i_if_zero  if_zero_i
#define v_if_zero  if_zero_v
#define a_if_null  if_null_a
#define e_if_null  if_null_e
#define w_if_null  if_null_w
#define i_if_null  if_null_i
#define v_if_null  if_null_v
#define af_if      if_true_af
#define ef_if      if_true_ef
#define wf_if      if_true_wf
#define if_if      if_true_if
#define vf_if      if_true_vf
#define af_if_fail if_fail_af
#define ef_if_fail if_fail_ef
#define wf_if_fail if_fail_wf
#define if_if_fail if_fail_if
#define vf_if_fail if_fail_vf
#define af_if_zero if_zero_af
#define ef_if_zero if_zero_ef
#define wf_if_zero if_zero_wf
#define if_if_zero if_zero_if
#define vf_if_zero if_zero_vf
#define af_if_null if_null_af
#define ef_if_null if_null_ef
#define wf_if_null if_null_wf
#define if_if_null if_null_if
#define vf_if_null if_null_vf
//
#define va_if       if_true_va
#define ve_if       if_true_ve
#define vw_if       if_true_vw
#define vi_if       if_true_vi
#define vv_if       if_true_vv
#define va_if_fail  if_fail_va
#define ve_if_fail  if_fail_ve
#define vw_if_fail  if_fail_vw
#define vi_if_fail  if_fail_vi
#define vv_if_fail  if_fail_vv
#define va_if_zero  if_zero_va
#define ve_if_zero  if_zero_ve
#define vw_if_zero  if_zero_vw
#define vi_if_zero  if_zero_vi
#define vv_if_zero  if_zero_vv
#define va_if_null  if_null_va
#define ve_if_null  if_null_ve
#define vw_if_null  if_null_vw
#define vi_if_null  if_null_vi
#define vv_if_null  if_null_vv
#define vaf_if      if_true_vaf
#define vef_if      if_true_vef
#define vwf_if      if_true_vwf
#define vif_if      if_true_vif
#define vvf_if      if_true_vvf
#define vaf_if_fail if_fail_vaf
#define vef_if_fail if_fail_vef
#define vwf_if_fail if_fail_vwf
#define vif_if_fail if_fail_vif
#define vvf_if_fail if_fail_vvf
#define vaf_if_zero if_zero_vaf
#define vef_if_zero if_zero_vef
#define vwf_if_zero if_zero_vwf
#define vif_if_zero if_zero_vif
#define vvf_if_zero if_zero_vvf
#define vaf_if_null if_null_vaf
#define vef_if_null if_null_vef
#define vwf_if_null if_null_vwf
#define vif_if_null if_null_vif
#define vvf_if_null if_null_vvf
//
#define fa_if       if_true_fa
#define fe_if       if_true_fe
#define fw_if       if_true_fw
#define fi_if       if_true_fi
#define fv_if       if_true_fv
#define fa_if_fail  if_fail_fa
#define fe_if_fail  if_fail_fe
#define fw_if_fail  if_fail_fw
#define fi_if_fail  if_fail_fi
#define fv_if_fail  if_fail_fv
#define fa_if_zero  if_zero_fa
#define fe_if_zero  if_zero_fe
#define fw_if_zero  if_zero_fw
#define fi_if_zero  if_zero_fi
#define fv_if_zero  if_zero_fv
#define fa_if_null  if_null_fa
#define fe_if_null  if_null_fe
#define fw_if_null  if_null_fw
#define fi_if_null  if_null_fi
#define fv_if_null  if_null_fv
#define faf_if      if_true_faf
#define fef_if      if_true_fef
#define fwf_if      if_true_fwf
#define fif_if      if_true_fif
#define fvf_if      if_true_fvf
#define faf_if_fail if_fail_faf
#define fef_if_fail if_fail_fef
#define fwf_if_fail if_fail_fwf
#define fif_if_fail if_fail_fif
#define fvf_if_fail if_fail_fvf
#define faf_if_zero if_zero_faf
#define fef_if_zero if_zero_fef
#define fwf_if_zero if_zero_fwf
#define fif_if_zero if_zero_fif
#define fvf_if_zero if_zero_fvf
#define faf_if_null if_null_faf
#define fef_if_null if_null_fef
#define fwf_if_null if_null_fwf
#define fif_if_null if_null_fif
#define fvf_if_null if_null_fvf
//
#define ba_if       if_true_ba
#define be_if       if_true_be
#define bw_if       if_true_bw
#define bi_if       if_true_bi
#define bv_if       if_true_bv
#define ba_if_fail  if_fail_ba
#define be_if_fail  if_fail_be
#define bw_if_fail  if_fail_bw
#define bi_if_fail  if_fail_bi
#define bv_if_fail  if_fail_bv
#define ba_if_zero  if_zero_ba
#define be_if_zero  if_zero_be
#define bw_if_zero  if_zero_bw
#define bi_if_zero  if_zero_bi
#define bv_if_zero  if_zero_bv
#define ba_if_null  if_null_ba
#define be_if_null  if_null_be
#define bw_if_null  if_null_bw
#define bi_if_null  if_null_bi
#define bv_if_null  if_null_bv
#define baf_if      if_true_baf
#define bef_if      if_true_bef
#define bwf_if      if_true_bwf
#define bif_if      if_true_bif
#define bvf_if      if_true_bvf
#define baf_if_fail if_fail_baf
#define bef_if_fail if_fail_bef
#define bwf_if_fail if_fail_bwf
#define bif_if_fail if_fail_bif
#define bvf_if_fail if_fail_bvf
#define baf_if_zero if_zero_baf
#define bef_if_zero if_zero_bef
#define bwf_if_zero if_zero_bwf
#define bif_if_zero if_zero_bif
#define bvf_if_zero if_zero_bvf
#define baf_if_null if_null_baf
#define bef_if_null if_null_bef
#define bwf_if_null if_null_bwf
#define bif_if_null if_null_bif
#define bvf_if_null if_null_bvf
//
#define ca_if       if_true_ca
#define ce_if       if_true_ce
#define cw_if       if_true_cw
#define ci_if       if_true_ci
#define cv_if       if_true_cv
#define ca_if_fail  if_fail_ca
#define ce_if_fail  if_fail_ce
#define cw_if_fail  if_fail_cw
#define ci_if_fail  if_fail_ci
#define cv_if_fail  if_fail_cv
#define ca_if_zero  if_zero_ca
#define ce_if_zero  if_zero_ce
#define cw_if_zero  if_zero_cw
#define ci_if_zero  if_zero_ci
#define cv_if_zero  if_zero_cv
#define ca_if_null  if_null_ca
#define ce_if_null  if_null_ce
#define cw_if_null  if_null_cw
#define ci_if_null  if_null_ci
#define cv_if_null  if_null_cv
#define caf_if      if_true_caf
#define cef_if      if_true_cef
#define cwf_if      if_true_cwf
#define cif_if      if_true_cif
#define cvf_if      if_true_cvf
#define caf_if_fail if_fail_caf
#define cef_if_fail if_fail_cef
#define cwf_if_fail if_fail_cwf
#define cif_if_fail if_fail_cif
#define cvf_if_fail if_fail_cvf
#define caf_if_zero if_zero_caf
#define cef_if_zero if_zero_cef
#define cwf_if_zero if_zero_cwf
#define cif_if_zero if_zero_cif
#define cvf_if_zero if_zero_cvf
#define caf_if_null if_null_caf
#define cef_if_null if_null_cef
#define cwf_if_null if_null_cwf
#define cif_if_null if_null_cif
#define cvf_if_null if_null_cvf
//
#define a_ecall  if_fail_a
#define e_ecall  if_fail_e
#define w_ecall  if_fail_w
#define i_ecall  if_fail_i
#define v_ecall  if_fail_v
#define va_ecall if_fail_va
#define ve_ecall if_fail_ve
#define vw_ecall if_fail_vw
#define vi_ecall if_fail_vi
#define vv_ecall if_fail_vv
#define ba_ecall if_fail_ba
#define be_ecall if_fail_be
#define bw_ecall if_fail_bw
#define bi_ecall if_fail_bi
#define bv_ecall if_fail_bv
#define ca_ecall if_fail_ca
#define ce_ecall if_fail_ce
#define cw_ecall if_fail_cw
#define ci_ecall if_fail_ci
#define cv_ecall if_fail_cv

#define a_ecallf  if_fail_af
#define e_ecallf  if_fail_ef
#define w_ecallf  if_fail_wf
#define i_ecallf  if_fail_if
#define v_ecallf  if_fail_vf
#define va_ecallf if_fail_vaf
#define ve_ecallf if_fail_vef
#define vw_ecallf if_fail_vwf
#define vi_ecallf if_fail_vif
#define vv_ecallf if_fail_vvf
#define ba_ecallf if_fail_baf
#define be_ecallf if_fail_bef
#define bw_ecallf if_fail_bwf
#define bi_ecallf if_fail_bif
#define bv_ecallf if_fail_bvf
#define ca_ecallf if_fail_caf
#define ce_ecallf if_fail_cef
#define cw_ecallf if_fail_cwf
#define ci_ecallf if_fail_cif
#define cv_ecallf if_fail_cvf

#define gpk_fail_if_eq(str_format, i2x_left, i2x_right) if_true_fef((i2x_left) == (i2x_right), str_format, (i2x_left), (i2x_right))
#define gpk_fail_if_ne(str_format, i2x_left, i2x_right) if_true_fef((i2x_left) != (i2x_right), str_format, (i2x_left), (i2x_right))
#define gpk_fail_if_lt(str_format, i2x_left, i2x_right) if_true_fef((i2x_left)  < (i2x_right), str_format, (i2x_left), (i2x_right))
#define gpk_fail_if_le(str_format, i2x_left, i2x_right) if_true_fef((i2x_left) <= (i2x_right), str_format, (i2x_left), (i2x_right))
#define gpk_fail_if_ge(str_format, i2x_left, i2x_right) if_true_fef((i2x_left) >= (i2x_right), str_format, (i2x_left), (i2x_right))
#define gpk_fail_if_gt(str_format, i2x_left, i2x_right) if_true_fef((i2x_left)  > (i2x_right), str_format, (i2x_left), (i2x_right))

#define fail_if_eq2u(i2u_left, i2u_right) gpk_fail_if_eq(GPK_FMT_EQ_U2, i2u_t(i2u_left), i2u_t(i2u_right))
#define fail_if_ne2u(i2u_left, i2u_right) gpk_fail_if_ne(GPK_FMT_NE_U2, i2u_t(i2u_left), i2u_t(i2u_right))
#define fail_if_lt2u(i2u_left, i2u_right) gpk_fail_if_lt(GPK_FMT_LT_U2, i2u_t(i2u_left), i2u_t(i2u_right))
#define fail_if_le2u(i2u_left, i2u_right) gpk_fail_if_le(GPK_FMT_LE_U2, i2u_t(i2u_left), i2u_t(i2u_right))
#define fail_if_ge2u(i2u_left, i2u_right) gpk_fail_if_ge(GPK_FMT_GE_U2, i2u_t(i2u_left), i2u_t(i2u_right))
#define fail_if_gt2u(i2u_left, i2u_right) gpk_fail_if_gt(GPK_FMT_GT_U2, i2u_t(i2u_left), i2u_t(i2u_right))
#define fail_if_eq2s(i2s_left, i2s_right) gpk_fail_if_eq(GPK_FMT_EQ_S2, i2s_t(i2s_left), i2s_t(i2s_right))
#define fail_if_ne2s(i2s_left, i2s_right) gpk_fail_if_ne(GPK_FMT_NE_S2, i2s_t(i2s_left), i2s_t(i2s_right))
#define fail_if_lt2s(i2s_left, i2s_right) gpk_fail_if_lt(GPK_FMT_LT_S2, i2s_t(i2s_left), i2s_t(i2s_right))
#define fail_if_le2s(i2s_left, i2s_right) gpk_fail_if_le(GPK_FMT_LE_S2, i2s_t(i2s_left), i2s_t(i2s_right))
#define fail_if_ge2s(i2s_left, i2s_right) gpk_fail_if_ge(GPK_FMT_GE_S2, i2s_t(i2s_left), i2s_t(i2s_right))
#define fail_if_gt2s(i2s_left, i2s_right) gpk_fail_if_gt(GPK_FMT_GT_S2, i2s_t(i2s_left), i2s_t(i2s_right))
#define fail_if_eq3u(i3u_left, i3u_right) gpk_fail_if_eq(GPK_FMT_EQ_U3, i3u_t(i3u_left), i3u_t(i3u_right))
#define fail_if_ne3u(i3u_left, i3u_right) gpk_fail_if_ne(GPK_FMT_NE_U3, i3u_t(i3u_left), i3u_t(i3u_right))
#define fail_if_lt3u(i3u_left, i3u_right) gpk_fail_if_lt(GPK_FMT_LT_U3, i3u_t(i3u_left), i3u_t(i3u_right))
#define fail_if_le3u(i3u_left, i3u_right) gpk_fail_if_le(GPK_FMT_LE_U3, i3u_t(i3u_left), i3u_t(i3u_right))
#define fail_if_ge3u(i3u_left, i3u_right) gpk_fail_if_ge(GPK_FMT_GE_U3, i3u_t(i3u_left), i3u_t(i3u_right))
#define fail_if_gt3u(i3u_left, i3u_right) gpk_fail_if_gt(GPK_FMT_GT_U3, i3u_t(i3u_left), i3u_t(i3u_right))
#define fail_if_eq3s(i3s_left, i3s_right) gpk_fail_if_eq(GPK_FMT_EQ_S3, i3s_t(i3s_left), i3s_t(i3s_right))
#define fail_if_ne3s(i3s_left, i3s_right) gpk_fail_if_ne(GPK_FMT_NE_S3, i3s_t(i3s_left), i3s_t(i3s_right))
#define fail_if_lt3s(i3s_left, i3s_right) gpk_fail_if_lt(GPK_FMT_LT_S3, i3s_t(i3s_left), i3s_t(i3s_right))
#define fail_if_le3s(i3s_left, i3s_right) gpk_fail_if_le(GPK_FMT_LE_S3, i3s_t(i3s_left), i3s_t(i3s_right))
#define fail_if_ge3s(i3s_left, i3s_right) gpk_fail_if_ge(GPK_FMT_GE_S3, i3s_t(i3s_left), i3s_t(i3s_right))
#define fail_if_gt3s(i3s_left, i3s_right) gpk_fail_if_gt(GPK_FMT_GT_S3, i3s_t(i3s_left), i3s_t(i3s_right))

#define fail_if_equ fail_if_eq2u
#define fail_if_neu fail_if_ne2u
#define fail_if_ltu fail_if_lt2u
#define fail_if_leu fail_if_le2u
#define fail_if_geu fail_if_ge2u
#define fail_if_gtu fail_if_gt2u
#define fail_if_eqs fail_if_eq2s
#define fail_if_nes fail_if_ne2s
#define fail_if_lts fail_if_lt2s
#define fail_if_les fail_if_le2s
#define fail_if_ges fail_if_ge2s
#define fail_if_gts fail_if_gt2s

#endif // GPK_LOG_LEVEL_H
