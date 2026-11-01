#include "gpk_log_core.h"

#include "gpk_size.h"
#include "gpk_cstring.h"
#include "gpk_memory.h"


#ifndef GPK_LOG_H
#define GPK_LOG_H

#if defined(GPK_ATMEL) || defined(ESP8266)
#	define GPK_PLATFORM_LOG_FORMAT(format)	((::gpk::sc_c*)format)
#else
#	define GPK_PLATFORM_LOG_FORMAT(format)	(format)
#endif

namespace gpk
{
	err_t			debug_print_prefix				(s0_t severity, sc_c * path, u2_t line, sc_c * function);
#ifndef GPK_ARDUINO
	void			_gpk_print_system_errors		(sc_c * prefix, u2_t prefixLen);
#endif
#if !defined(GPK_ARDUINO) || defined(GPK_ESP32)
	tplt<u2_t fmtLen, tpnm... TArgs>
	sttc	err_t	debug_printf					(s0_t severity, sc_c * path, u2_t line, sc_c * function, sc_c (&format)[fmtLen], cnst TArgs... args) {
		debug_print_prefix(severity, path, line, function);

#ifdef GPK_ESP32
		u2_c			bufferSize						= u2_t(strlen(format) + 1024 * 8);
		sc_t			* formatted						= {};
		if(0 == ::gpk::malloc(formatted, bufferSize))
			rtrn -1;
		::gpk::auto_free		formattedOwner;
		formattedOwner.Handle							= formatted;
		cnst err_t		formatLength					= ::gpk::sprintf_s(formatted, bufferSize - 1U, format, printf_arg(args)...);
		if(0 > formatLength)
			rtrn formatLength;
		u2_t			outputLength					= min(u2_t(formatLength), bufferSize - 2U);
		formatted[outputLength++] = '\n';
		rtrn log_write(formatted, outputLength);
#else
		sc_t			formatted	[fmtLen + 1024 * 32]	= {};
		cnst err_t		formatLength					= ::gpk::sprintf_s(formatted, format, printf_arg(args)...);
		if(0 > formatLength)
			rtrn formatLength;

		u2_t			outputLength					= min(u2_t(formatLength), ::gpk::size(formatted) - 2U);
		formatted[outputLength++] = '\n';
		if(2 >= severity)
			_gpk_print_system_errors("", 0);
		rtrn log_write(formatted, outputLength);
#endif
	}
#endif
#if defined(GPK_ARDUINO) && !defined(GPK_ESP32)
#ifndef GPK_ATMEL
	tplt<u2_t fmtLen, tpnm... TArgs>
	sttc	err_t	_gpk_debug_printf				(s0_t severity, sc_c * path, u2_t line, sc_c * function, sc_c (&format)[fmtLen], cnst TArgs... args)	{
		debug_print_prefix((s0_t)severity, path, line, function);
#else
	tplt<tpnm... TArgs>
	sttc	err_t	_gpk_debug_printf				(sc_c * function, cnst __FlashStringHelper * format, cnst TArgs... args)			{
		log_print_F("{");
		log_print(function);
		log_print_F("}:");
#endif

		sc_t					customDynamicString	[128]		= {};
		u2_c 					stringLength					= (u2_t)::gpk::sprintf_s(customDynamicString, GPK_PLATFORM_LOG_FORMAT(format), printf_arg(args)...);
		customDynamicString[min(stringLength, szof(customDynamicString) - 1U)] = '\n';
		return log_write(customDynamicString, min(szof(customDynamicString), stringLength + 1U));
	}
#endif

	tpl_vtArgs
	sinx	void*		nully				(_tArgs&&...)		{ return 0; }

}

#ifdef GPK_ATMEL
#	define gpk_debug_printf(severity, format, ...)	::gpk::_gpk_debug_printf(__func__, F(format), ##__VA_ARGS__)
#elif defined(GPK_ESP32)
#	define gpk_debug_printf(severity, ...)			::gpk::debug_printf(severity, __FILE__, __LINE__, __func__, __VA_ARGS__)
#elif defined(GPK_WINDOWS)
#	define gpk_debug_printf(severity, ...)			::gpk::debug_printf(severity, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#elif defined(GPK_ARDUINO)
#	define gpk_debug_printf(severity, ...)			::gpk::_gpk_debug_printf(severity, __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#	define gpk_debug_printf(severity, ...)			::gpk::debug_printf(severity, __FILE__, __LINE__, __func__, __VA_ARGS__)
#endif

#define always_printf(...)						gpk_debug_printf(3, __VA_ARGS__)

#if defined (GPK_ERROR_PRINTF_ENABLED)
#	define error_printf(...)						do { gpk_debug_printf(1, __VA_ARGS__); ::gpk::debugBreak(); } while(0)
#else
#	define error_printf(format, ...)					do { ::gpk::dummy(__VA_ARGS__); ::gpk::debugBreak(); } while(0)
#endif

#if defined (GPK_WARNING_PRINTF_ENABLED)
#	define warning_printf(...)						gpk_debug_printf(2, __VA_ARGS__)
#else
#	define warning_printf(format, ...)					do { ::gpk::dummy(__VA_ARGS__); } while(0)
#endif

#if defined (GPK_INFO_PRINTF_ENABLED)
#	define info_printf(...)							gpk_debug_printf(3, __VA_ARGS__)
#else
#	define info_printf(format, ...)						do { ::gpk::dummy(__VA_ARGS__); } while(0)
#endif

#if defined (GPK_SUCCESS_PRINTF_ENABLED)
#	define success_printf(...)						gpk_debug_printf(4, __VA_ARGS__)
#else
#	define success_printf(format, ...)					do { ::gpk::dummy(__VA_ARGS__); } while(0)
#endif

#if defined (GPK_VERBOSE_PRINTF_ENABLED)
#	define verbose_printf(...)						gpk_debug_printf(4, __VA_ARGS__)
#else
#	define verbose_printf(format, ...)					do { ::gpk::dummy(__VA_ARGS__); } while(0)
#endif

//
#define gpk_rv_hrcall(retVal, hr_call)					do { ::HRESULT errCall_ = (hr_call); if FAILED(errCall_) { gpk_debug_printf(0, "%s -> %" GPK_FMT_S2 " (0x%X): '%s'.", #hr_call, errCall_, ::gpk::getWindowsErrorAsString(errCall_).begin()); return retVal; } } while(0)
#define gpk_rve_hrcall(retVal, hr_call, format, ...)	do { ::HRESULT errCall_ = (hr_call); if FAILED(errCall_) { gpk_debug_printf(0, "%s -> %" GPK_FMT_S2 " (0x%X): '%s' " format, #hr_call, errCall_, ::gpk::getWindowsErrorAsString(errCall_).begin(), __VA_ARGS__); return retVal; } } while(0)

#define gpk_hrcall(hr_call)				do { gpk_rv_hrcall (-1, hr_call)				; } while(0)		// HRESULT call.
#define gpk_hrecall(hr_call, ...)		do { gpk_rve_hrcall(-1, hr_call, __VA_ARGS__)	; } while(0)		// HRESULT call.


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
#define if_null_logf_and_return_verbose			if_zero_logf_and_return_verbose
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
#define gpk_fail_if_eq(str_format, i2x_left, i2x_right) if_true_fef((i2x_left) == (i2x_right), str_format, (i2x_left), (i2x_right))
#define gpk_fail_if_ne(str_format, i2x_left, i2x_right) if_true_fef((i2x_left) != (i2x_right), str_format, (i2x_left), (i2x_right))
#define gpk_fail_if_lt(str_format, i2x_left, i2x_right) if_true_fef((i2x_left)  < (i2x_right), str_format, (i2x_left), (i2x_right))
#define gpk_fail_if_le(str_format, i2x_left, i2x_right) if_true_fef((i2x_left) <= (i2x_right), str_format, (i2x_left), (i2x_right))
#define gpk_fail_if_ge(str_format, i2x_left, i2x_right) if_true_fef((i2x_left) >= (i2x_right), str_format, (i2x_left), (i2x_right))
#define gpk_fail_if_gt(str_format, i2x_left, i2x_right) if_true_fef((i2x_left)  > (i2x_right), str_format, (i2x_left), (i2x_right))
//
#endif // GPK_LOG_H
