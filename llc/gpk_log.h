#include "gpk_error.h"
#include "gpk_chrono.h" 
#include "gpk_size.h"

#ifdef GPK_ATMEL
#	include <stdio.h>
#	include <string.h>
#	include "gpk_eval.h"
#else
#	include "gpk_eval.h"
#	include <cstdio>
#endif

#ifdef GPK_ARDUINO
#	include <WString.h>
#endif

#ifndef GPK_LOG_H_23627
#define GPK_LOG_H_23627

//#define GPK_NULLIFY_ALL_CONDITIONAL_ERROR_CHECKING
#if !defined(DEBUG) && !defined(_DEBUG)
#	if defined(GPK_NULLIFY_ALL_CONDITIONAL_ERROR_CHECKING)
#		define	GPK_NULLIFY_NECALL
#		define	GPK_NULLIFY_CONDITIONAL_RETERR
#		define	GPK_NULLIFY_CONDITIONAL_LOG
#		define	GPK_NULLIFY_CONDITIONAL_THROW
#	else
#		//ifndef GPK_NULLIFY_NECALL
#		//	define GPK_NULLIFY_NECALL
#		//endif
#		//ifndef GPK_NULLIFY_CONDITIONAL_RETERR
#		//	define GPK_NULLIFY_CONDITIONAL_RETERR
#		//endif
#		//ifndef GPK_NULLIFY_CONDITIONAL_LOG
#		//	define GPK_NULLIFY_CONDITIONAL_LOG
#		//endif
#		//ifndef GPK_NULLIFY_CONDITIONAL_THROW
#		//	define GPK_NULLIFY_CONDITIONAL_THROW
#		//endif
#	endif
#endif
namespace gpk
{
	typedef	void		(*log_write_t)				(const char * text, uint32_t textLen);
	typedef	void		(*log_print_t)				(const char * text);

	void				_gpk_print_system_errors		(const char * prefix, uint32_t prefixLen);
	void				_base_log_print					(const char * text);
	void				_base_log_write					(const char * text, uint32_t textLen);

#ifdef GPK_ATMEL
	typedef	void		(*log_print_P_t)				(const __FlashStringHelper * text);
	void				_base_log_print_P				(const __FlashStringHelper * text);
#endif

#define base_log_write(text, textLen)	::gpk::_base_log_write(text, textLen)
#define base_log_print(text)			::gpk::_base_log_print(text)
#ifdef GPK_ATMEL
#	define base_log_print_F(text)			::gpk::_base_log_print_P(F(text))
#	define base_log_print_P(text)			::gpk::_base_log_print_P(text)
#	define condition_print(condition)		::gpk::_base_log_print_P(F("Condition: (" #condition ").\n"))
#else
#	define condition_print(condition)		::gpk::_base_log_print("Condition: (" #condition ").\n")
#endif

#if defined(GPK_WINDOWS)
	tplt<size_t prefixLength, tpnm... TArgs>
	static	void		_gpk_debug_printf				(int severity, const char (&prefix)[prefixLength], const char* format, const TArgs... args)			{
		char					timeString	[64]				= {};
		size_t					stringLength					= snprintf(timeString, sizeof(timeString) - 2, "%llu|", ::gpk::timeCurrentInMs());
		base_log_write(timeString, (int)stringLength);
		base_log_write(prefix, prefixLength);
#else
#ifndef GPK_ATMEL
	tplt<size_t prefixLength, tpnm... TArgs>
	static	void		_gpk_debug_printf				(int severity, const char (&prefix)[prefixLength], const char* function, const char* format, const TArgs... args)			{
		char					timeString	[64]				= {};
		size_t					stringLength					= snprintf(timeString, sizeof(timeString) - 2, "%llu|", ::gpk::timeCurrentInMs());
		base_log_write(timeString, (int)stringLength);
		base_log_write(prefix, strlen(prefix));
		base_log_write("{", 1);
		base_log_write(function, strlen(function));
		base_log_write("}:", 2);
#else
	tplt<tpnm... TArgs>
	static	void		_gpk_debug_printf				(const char* function, const __FlashStringHelper* format, const TArgs... args)			{
		base_log_print_F("{");
		base_log_print(function);
		base_log_print_F("}:");
#endif
#endif
#ifdef GPK_ESP32
		if(format) {
			const uint32_t bufferSize = uint32_t(strlen(format) + 1024 * 16);
			if(char * customDynamicString = (char*)malloc(bufferSize)) {
				stringLength	= snprintf(customDynamicString, bufferSize - 2, format, args...);
				customDynamicString[::gpk::min(stringLength, bufferSize - 2)] = '\n';
				customDynamicString[::gpk::min(stringLength + 1, bufferSize - 1)] = 0;
				base_log_write(customDynamicString, (int)stringLength);
				free(customDynamicString);
			}
		}
#else
#	if defined(GPK_ARDUINO)
#		ifdef min
#			undef min
#		endif
#		ifdef max
#			undef max
#		endif
		char					customDynamicString	[64]		= {};
#	else
		char					customDynamicString	[1024 * 12]	= {};
#	endif

#ifdef GPK_ATMEL
		size_t 					stringLength					= snprintf_P(customDynamicString, sizeof(customDynamicString) - 1, (const char*)format, args...);
#else
		stringLength		= snprintf(customDynamicString, sizeof(customDynamicString) - 2, format, args...);
		customDynamicString[::gpk::min(stringLength, sizeof(customDynamicString)-2)] = '\n';
#endif
		base_log_write(customDynamicString, (uint32_t)stringLength);
#endif

#ifndef GPK_ATMEL
		if(2 >= severity)
			::gpk::_gpk_print_system_errors(prefix, prefixLength);
#endif
	}
	tplt<tpnm... _tArgs>
	stincxp	void		dummy		(_tArgs&&...)		{}
	tplt<tpnm... _tArgs>
	stincxp	void*		nully		(_tArgs&&...)		{ return 0; }
}

#define	GPK_STRINGIFY(x) #x
#define	GPK_TOSTRING(x)  GPK_STRINGIFY(x)

#if !defined(GPK_WINDOWS)
#ifdef GPK_ATMEL
#	define debug_printf(severity, severityStr, format, ...)	do{ base_log_print_F(#severity "|" severityStr "|" __FILE__ "(" GPK_TOSTRING(__LINE__) ")"); ::gpk::_gpk_debug_printf(__func__, F(format), ##__VA_ARGS__); base_log_print_F("\n"); } while(0) //::gpk::_gpk_debug_printf("(" GPK_TOSTRING(__LINE__) ")", "")
#else
#	define debug_printf(severity, severityStr, format, ...)	::gpk::_gpk_debug_printf(severity, #severity "|" severityStr "|" __FILE__ "(" GPK_TOSTRING(__LINE__) ")", __func__, format, ##__VA_ARGS__)
#endif
#else
#	define debug_printf(severity, severityStr, format, ...)	::gpk::_gpk_debug_printf(severity, #severity "|" severityStr "|" __FILE__ "(" GPK_TOSTRING(__LINE__) "){" __FUNCTION__ "}:", format, __VA_ARGS__)
#endif

#ifndef always_printf
#if !defined(GPK_WINDOWS)
#	define always_printf(format, ...)					debug_printf(3, "info", format, ##__VA_ARGS__)
#else
#	define always_printf(format, ...)					debug_printf(3, "info", format, __VA_ARGS__)
#endif
#endif

#ifndef error_printf
#	if defined (GPK_ERROR_PRINTF_ENABLED)
#if !defined(GPK_WINDOWS)
#		define error_printf(format, ...)					do { debug_printf(1, "error", format, ##__VA_ARGS__); GPK_PLATFORM_CRT_BREAKPOINT(); } while(0)
#		define error_printf_nb(format, ...)					debug_printf(1, "error", format, ##__VA_ARGS__)
#else
#		define error_printf(format, ...)					do { debug_printf(1, "error", format, __VA_ARGS__); GPK_PLATFORM_CRT_BREAKPOINT(); } while(0)
#		define error_printf_nb(format, ...)					debug_printf(1, "error", format, __VA_ARGS__)
#endif
#	else
#		define error_printf(format, ...)					do { ::gpk::dummy(__VA_ARGS__); GPK_PLATFORM_CRT_BREAKPOINT(); } while(0)
#	endif
#endif

#ifndef warning_printf
#	if defined (GPK_WARNING_PRINTF_ENABLED)
#if !defined(GPK_WINDOWS)
#		define warning_printf(format, ...)					debug_printf(2, "warning"	, format, ##__VA_ARGS__)
#else
#		define warning_printf(format, ...)					debug_printf(2, "warning"	, format, __VA_ARGS__)
#endif
#	else
#		define warning_printf(format, ...)					do { ::gpk::dummy(__VA_ARGS__); } while(0)
#	endif
#endif

#ifndef info_printf
#	if defined (GPK_INFO_PRINTF_ENABLED)
#if !defined(GPK_WINDOWS)
#		define info_printf(format, ...)						debug_printf(3, "info"		, format, ##__VA_ARGS__)
#else
#		define info_printf(format, ...)						debug_printf(3, "info"		, format, __VA_ARGS__)
#endif
#	else
#		define info_printf(format, ...)						do { ::gpk::dummy(__VA_ARGS__); } while(0)
#	endif
#endif

#ifndef success_printf
#	if defined (GPK_SUCCESS_PRINTF_ENABLED)
#if !defined(GPK_WINDOWS)
#		define success_printf(format, ...)					debug_printf(4, "info"		, format, ##__VA_ARGS__)
#else
#		define success_printf(format, ...)					debug_printf(4, "info"		, format, __VA_ARGS__)
#endif
#	else
#		define success_printf(format, ...)					do { ::gpk::dummy(__VA_ARGS__); } while(0)
#	endif
#endif

#ifndef verbose_printf
#	if defined (GPK_VERBOSE_PRINTF_ENABLED)
#if !defined(GPK_WINDOWS)
#		define verbose_printf(format, ...)					debug_printf(4, "info"		, format, ##__VA_ARGS__)
#else
#		define verbose_printf(format, ...)					debug_printf(4, "info"		, format, __VA_ARGS__)
#endif
#	else
#		define verbose_printf(format, ...)					do { ::gpk::dummy(__VA_ARGS__); } while(0)
#	endif
#endif

#if defined (GPK_WINDOWS)
#	define gpk_throw(...)								throw(__VA_ARGS__)
#else
#	define gpk_throw(...)								do { char * nulp = 0; unsigned int i = 0; while(++i) nulp[i] = (char)i; ::gpk::dummy(__VA_ARGS__); } while(0)
#endif

#ifndef gthrow_if
#	ifndef GPK_NULLIFY_CONDITIONAL_THROW
#if !defined(GPK_WINDOWS)
#		define gthrow_if(condition, format, ...)	if(condition) { condition_print(condition); error_printf(format, ##__VA_ARGS__); gpk_throw("");	} 
#		define gsthrow_if(condition)				if(condition) { condition_print(condition); gpk_throw("");	} 
#	else
#		define gthrow_if(condition, format, ...)	if(condition) { condition_print(condition); error_printf(format, __VA_ARGS__); gpk_throw(""); } 
#		define gsthrow_if(condition)				if(condition) { condition_print(condition); gpk_throw("");	}													  
#	endif
#	else
//#	pragma warning(disable:4552)	// this was required because "condition" may have had no side effect.
//#	pragma warning(disable:4553)	// this was required because "condition" may have had no side effect.
#		define gthrow_if(condition, format, ...)	if(condition) do{ ::gpk::dummy(__VA_ARGS__); } while(0)
#	endif
#endif

#ifndef gerror_if
#	ifndef GPK_NULLIFY_CONDITIONAL_LOG
#		define gserror_if(condition)							if(condition) { condition_print(condition); }
#		define gswarn_if(condition)								if(condition) { condition_print(condition); }
#		define gsinfo_if(condition)								if(condition) { condition_print(condition); }
#		if !defined(GPK_WINDOWS)
#			define gerror_if(condition, format, ...)				if(condition) { condition_print(condition); error_printf	(format, ##__VA_ARGS__);} 
#			define gwarn_if(condition, format, ...)					if(condition) { condition_print(condition); warning_printf(format, ##__VA_ARGS__);} 
#			define ginfo_if(condition, format, ...)					if(condition) { condition_print(condition); info_printf	(format, ##__VA_ARGS__);} 
#		else							
#			define gerror_if(condition, format, ...)				if(condition) { condition_print(condition); error_printf	(format, __VA_ARGS__);	} 
#			define gwarn_if(condition, format, ...)					if(condition) { condition_print(condition); warning_printf(format, __VA_ARGS__);	} 
#			define ginfo_if(condition, format, ...)					if(condition) { condition_print(condition); info_printf	(format, __VA_ARGS__);	} 
#		endif
#	else
//#	pragma warning(disable:4552)	// this is required because "condition" may have no side effect.
//#	pragma warning(disable:4553)	// this is required because "condition" may have no side effect.
#		define gerror_if(condition, format, ...)				if(condition) { ::gpk::dummy(__VA_ARGS__); } 
#		define gwarn_if(condition, format, ...)					if(condition) { ::gpk::dummy(__VA_ARGS__); } 
#		define ginfo_if(condition, format, ...)					if(condition) { ::gpk::dummy(__VA_ARGS__); } 
#		define gserror_if(condition)							if(condition) {} 
#		define gswarn_if(condition)								if(condition) {} 
#		define gsinfo_if(condition)								if(condition) {} 
#	endif
#endif

#ifndef ret_gerror_if
#	define ret_gserror_if(condition)						if(condition) { condition_print(condition); return; } 
#	define ret_gswarn_if(condition)							if(condition) { condition_print(condition); return; } 
#	define ret_gsinfo_if(condition)							if(condition) { condition_print(condition); return; } 
#	define ret_gerror_if(condition, format, ...)			if(condition) { condition_print(condition); error_printf	(format, __VA_ARGS__); return;	} 
#	define ret_gwarn_if(condition, format, ...)				if(condition) { condition_print(condition); warning_printf	(format, __VA_ARGS__); return;	} 
#	define ret_ginfo_if(condition, format, ...)				if(condition) { condition_print(condition); info_printf	(format, __VA_ARGS__); return;	} 
#endif

#ifndef break_gerror_if
#	define break_gserror_if(condition)						if(condition) { condition_print(condition); break;	}
#	define break_gswarn_if(condition)						if(condition) { condition_print(condition); break;	}
#	define break_gsinfo_if(condition)						if(condition) { condition_print(condition); break;	}
#	define break_gsverbose_if(condition)					if(condition) { condition_print(condition); break;	}
#	define break_gerror_if(condition, format, ...)			if(condition) { condition_print(condition); error_printf	(format, __VA_ARGS__); break;	}
#	define break_gwarn_if(condition, format, ...)			if(condition) { condition_print(condition); warning_printf	(format, __VA_ARGS__); break;	}
#	define break_ginfo_if(condition, format, ...)			if(condition) { condition_print(condition); info_printf	(format, __VA_ARGS__); break;	}
#	define break_gverbose_if(condition, format, ...)		if(condition) { condition_print(condition); verbose_printf	(format, __VA_ARGS__); break;	}
#endif

#ifndef continue_gerror_if
#	define continue_gserror_if(condition)					if(condition) { condition_print(condition); continue; }
#	define continue_gswarn_if(condition)					if(condition) { condition_print(condition); continue; }
#	define continue_gsinfo_if(condition)					if(condition) { condition_print(condition); continue; }
#	define continue_gerror_if(condition, format, ...)		if(condition) { condition_print(condition); error_printf	(format, __VA_ARGS__); continue; }
#	define continue_gwarn_if(condition, format, ...)		if(condition) { condition_print(condition); warning_printf	(format, __VA_ARGS__); continue; }
#	define continue_ginfo_if(condition, format, ...)		if(condition) { condition_print(condition); info_printf	(format, __VA_ARGS__); continue; }
#endif

#ifndef retval_gerror_if
#	define retval_gserror_if(retVal, condition)					if(condition) { condition_print(condition); return retVal; } 
#	define retval_gswarn_if(retVal, condition)					if(condition) { condition_print(condition); return retVal; } 
#	define retval_gsinfo_if(retVal, condition)					if(condition) { condition_print(condition); return retVal; } 
#	define retval_gsverbose_if(retVal, condition)				if(condition) { condition_print(condition); return retVal; } 
#	define retval_gsalways_if(retVal, condition)				if(condition) { condition_print(condition); return retVal; } 
#	define retval_gerror_if(retVal, condition, format, ...)		if(condition) { condition_print(condition); error_printf	(format, __VA_ARGS__); return retVal; } 
#	define retval_gwarn_if(retVal, condition, format, ...)		if(condition) { condition_print(condition); warning_printf	(format, __VA_ARGS__); return retVal; } 
#	define retval_ginfo_if(retVal, condition, format, ...)		if(condition) { condition_print(condition); info_printf	(format, __VA_ARGS__); return retVal; } 
#	define retval_gverbose_if(retVal, condition, format, ...)	if(condition) { condition_print(condition); verbose_printf	(format, __VA_ARGS__); return retVal; } 
#	define retval_galways_if(retVal, condition, format, ...)	if(condition) { condition_print(condition); always_printf	(format, __VA_ARGS__); return retVal; } 
#endif

#ifndef retnul_gerror_if
#	define retnul_gserror_if(condition)						retval_gserror_if	(0, condition)
#	define retnul_gswarn_if(condition)						retval_gswarn_if	(0, condition)
#	define retnul_gsinfo_if(condition)						retval_gsinfo_if	(0, condition)
#	define retnul_gerror_if(condition, format, ...)			retval_gerror_if	(0, condition, format, __VA_ARGS__)
#	define retnul_gwarn_if(condition, format, ...)			retval_gwarn_if		(0, condition, format, __VA_ARGS__)
#	define retnul_ginfo_if(condition, format, ...)			retval_ginfo_if		(0, condition, format, __VA_ARGS__)
#endif

#ifndef reterr_gerror_if
#	ifndef GPK_NULLIFY_CONDITIONAL_RETERR
#		define reterr_gserror_if(condition)						retval_gserror_if	(-1, condition)
#		define reterr_gswarn_if(condition)						retval_gswarn_if	(-1, condition)
#		define reterr_gsinfo_if(condition)						retval_gsinfo_if	(-1, condition)
#		define reterr_gerror_if(condition, format, ...)			retval_gerror_if	(-1, condition, format, __VA_ARGS__)
#		define reterr_gwarn_if(condition, format, ...)			retval_gwarn_if		(-1, condition, format, __VA_ARGS__)
#		define reterr_ginfo_if(condition, format, ...)			retval_ginfo_if		(-1, condition, format, __VA_ARGS__)
#	else
#		pragma warning(disable:4552)	// this is required because "condition" may have no side effect.
#		pragma warning(disable:4553)	// this is required because "condition" may have no side effect.
#		define reterr_gserror_if(condition)						if(condition) {}
#		define reterr_gswarn_if(condition)						if(condition) {}
#		define reterr_gsinfo_if(condition)						if(condition) {}
#		define reterr_gerror_if(condition, format, ...)			if(condition) { ::gpk::dummy(__VA_ARGS__); }
#		define reterr_gwarn_if(condition, format, ...)			if(condition) { ::gpk::dummy(__VA_ARGS__); }
#		define reterr_ginfo_if(condition, format, ...)			if(condition) { ::gpk::dummy(__VA_ARGS__); }
#	endif
#endif

#ifndef retwarn_gerror_if
#	define retwarn_gserror_if(condition)					retval_gserror_if	(1, condition)
#	define retwarn_gswarn_if(condition)						retval_gswarn_if	(1, condition)
#	define retwarn_gsinfo_if(condition)						retval_gsinfo_if	(1, condition)
#	define retwarn_gerror_if(condition, format, ...)		retval_gerror_if	(1, condition, format, __VA_ARGS__)
#	define retwarn_gwarn_if(condition, format, ...)			retval_gwarn_if		(1, condition, format, __VA_ARGS__)
#	define retwarn_ginfo_if(condition, format, ...)			retval_ginfo_if		(1, condition, format, __VA_ARGS__)
#endif


#if defined (GPK_ERROR_PRINTF_ENABLED)
// Non-propagable retval_error call.
#	define gpk_rve_ecall(retVal, gpkl_call, format, ...) do {				\
		::gpk::error_t gpk_errCall_ = (gpkl_call);  						\
		if(gpk_errCall_ < 0) {												\
			condition_print(gpkl_call);										\
			debug_printf(0, "error", "0x%X.", gpk_errCall_);				\
			error_printf(format, __VA_ARGS__); 								\
			return retVal; 													\
		}																	\
		else {																\
			success_printf("%s: Success (0x%X).", #gpkl_call, gpk_errCall_);																			\
		}																	\
	} while(0)

// Non-propagable retval_error error-warning call.
#	define gpk_rve_ewcall(retVal, gpkl_call, format, ...) do {				\
		if(::gpk::error_t gpk_errCall_ = (gpkl_call)) { 					\
			if(gpk_errCall_ < 0) {											\
				condition_print(gpkl_call);										\
				debug_printf(0, "error", "0x%X.", gpk_errCall_);				\
				error_printf(format, __VA_ARGS__); 							\
				return retval; 												\
			}																\
			else {															\
				warning_printf("%s: 0x%X.", #gpkl_call, gpk_errCall_);				\
			}																\
		}																	\
		else {																\
			success_printf("%s: Success (0x%X).", #gpkl_call, gpk_errCall_);																			\
		}																	\
	} while(0)

//
#	define gpk_rv_hrcall(retVal, hr_call) do {								\
		::HRESULT errCall_ = (hr_call);  									\
		if FAILED(errCall_) {												\
			debug_printf(0, "error", "%s: (0x%X) : '%s'.", #hr_call, errCall_, ::gpk::getWindowsErrorAsString(errCall_).begin());						\
			return retVal; 													\
		}																	\
		else {																\
			success_printf("%s: Success (0x%X).", #hr_call, errCall_);				\
		}																	\
	} while(0)

//
#	define gpk_rve_hrcall(retVal, hr_call, format, ...) do {				\
		::HRESULT errCall_ = (hr_call);  									\
		if FAILED(errCall_) {												\
			debug_printf(0, "error", "%s: (0x%X) : '%s' - " format, #hr_call, errCall_, ::gpk::getWindowsErrorAsString(errCall_).begin(), __VA_ARGS__);	\
			return retVal; 													\
		}																	\
		else {																\
			success_printf("%s: Success (0x%X).", #hr_call, errCall_);				\
		}																	\
	} while(0)

// --------------------------------------------------------------------
// Propagable retval_error call.
#	define gpk_pecall(gpkl_call, ...) do {									\
		::gpk::error_t gpk_errCall_ = (gpkl_call);  						\
		if(gpk_errCall_ < 0) {												\
			debug_printf(0, "error", "%s: 0x%X", #gpkl_call, gpk_errCall_);			\
			error_printf(__VA_ARGS__); 										\
			return gpk_errCall_; 											\
		}																	\
		else {																\
			success_printf("%s: Success (0x%X).", #gpkl_call, gpk_errCall_);																			\
		}																	\
	} while(0)

// Propagable retval_error error-warning call.
#	define gpk_pewcall(gpkl_call, ...) do {									\
		if(::gpk::error_t gpk_errCall_ = (gpkl_call)) { 					\
			if(gpk_errCall_ < 0) {											\
				debug_printf(0, "error", "%s: 0x%X", #gpkl_call, gpk_errCall_);																			\
				error_printf(__VA_ARGS__); 									\
				return gpk_errCall_; 										\
			}																\
			else {															\
				warning_printf("%s: 0x%X.", #gpkl_call, gpk_errCall_);				\
			}																\
		}																	\
		else {																\
			success_printf("%s: Success (0x%X).", #gpkl_call, gpk_errCall_);																			\
		}																	\
	} while(0)

#else
#	define gpk_rve_ecall(retval, gpkl_call, ...) do {						\
		if(::gpk::failed(gpkl_call))  										\
			return retval; 													\
	} while(0)

#	define gpk_rve_hrcall(retval, gpkl_call, ...) do {						\
		if(FAILED(gpkl_call))  												\
			return retval; 													\
	} while(0)

#	define gpk_rv_hrcall(retval, gpkl_call) do {							\
		if(FAILED(gpkl_call))  												\
			return retval; 													\
	} while(0)

#	define gpk_pecall(gpkl_call, ...) do {									\
		::gpk::error_t gpk_errCall_ = (gpkl_call);							\
		if(::gpk::failed(gpk_errCall_)) 									\
			return gpk_errCall_; 											\
	} while(0)

#	define gpk_rve_ewcall					gpk_rve_ecall	// Non-propagable retval_error error-warning call.
#	define gpk_pewcall						gpk_pecall			// Propagable retval_error error-warning call.
#endif

#ifndef GPK_NULLIFY_NECALL
#	define gpk_necall(gpkl_call, ...)		gpk_rve_ecall (-1, gpkl_call, __VA_ARGS__)	// Non-propagable error call.
#	define gpk_necs(gpkl_call, ...)			gpk_rve_ecall (-1, gpkl_call, "%s", "")		// Non-propagable error call string.
#	define gpk_newcall(gpkl_call, ...)		gpk_rve_ewcall(-1, gpkl_call, __VA_ARGS__)	// Non-propagable error-warning call.
#	define gpk_hrcall(hr_call)				gpk_rv_hrcall (-1, hr_call)					// HRESULT call.
#	define gpk_hrecall(hr_call, ...)		gpk_rve_hrcall(-1, hr_call, __VA_ARGS__)	// HRESULT call.
#else
#	define gpk_necall(gpkl_call, ...)		do{gpkl_call; ::gpk::dummy(__VA_ARGS__); } while(0) // Non-propagable error call.
#	define gpk_necs(gpkl_call, ...)			do{gpkl_call; ::gpk::dummy(__VA_ARGS__); } while(0) // Non-propagable error call string.
#	define gpk_newcall(gpkl_call, ...)		do{gpkl_call; ::gpk::dummy(__VA_ARGS__); } while(0) // Non-propagable error-warning call.
#	define gpk_hrcall(hr_call)				do{hr_call	; ::gpk::dummy(__VA_ARGS__); } while(0) // HRESULT call.
#	define gpk_hrecall(hr_call, ...)		do{hr_call	; ::gpk::dummy(__VA_ARGS__); } while(0) // HRESULT call.
#endif

#ifndef e_if
#	define e_if								gerror_if
#	define w_if								gwarn_if
#	define i_if								ginfo_if
#	define es_if							gserror_if
#	define ws_if							gswarn_if
#	define is_if							gsinfo_if
#endif

#ifndef re_if
#	define re_if							ret_gerror_if
#	define rw_if							ret_gwarn_if
#	define ri_if							ret_ginfo_if
#endif

#ifndef be_if
#	define be_if							break_gerror_if
#	define bw_if							break_gwarn_if
#	define bi_if							break_ginfo_if
#	define bes_if							break_gserror_if
#	define bws_if							break_gswarn_if
#	define bis_if							break_gsinfo_if
#endif

#ifndef ce_if
#	define ce_if							continue_gerror_if
#	define cw_if							continue_gwarn_if
#	define ci_if							continue_ginfo_if
#	define ces_if							continue_gserror_if
#	define cws_if							continue_gswarn_if
#	define cis_if							continue_gsinfo_if
#endif

#ifndef rve_if
#	define rve_if							retval_gerror_if
#	define rvw_if							retval_gwarn_if
#	define rvi_if							retval_ginfo_if
#	define rva_if							retval_galways_if
#	define rves_if							retval_gserror_if
#	define rvws_if							retval_gswarn_if
#	define rvis_if							retval_gsinfo_if
#	define rvas_if							retval_gsalways_if

#	define rve_if_failed(retVal, condition, format, ...)	rve_if(retVal, ::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define rvw_if_failed(retVal, condition, format, ...)	rvw_if(retVal, ::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define rvi_if_failed(retVal, condition, format, ...)	rvi_if(retVal, ::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define rva_if_failed(retVal, condition, format, ...)	rva_if(retVal, ::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define rves_if_failed(retVal, condition)				rves_if(retVal, ::gpk::failed(::gpk::error_t(condition)))
#	define rvws_if_failed(retVal, condition)				rvws_if(retVal, ::gpk::failed(::gpk::error_t(condition)))
#	define rvis_if_failed(retVal, condition)				rvis_if(retVal, ::gpk::failed(::gpk::error_t(condition)))
#	define rvas_if_failed(retVal, condition)				rvas_if(retVal, ::gpk::failed(::gpk::error_t(condition)))

#endif

#ifndef rne_if
#	define rne_if							retnul_gerror_if
#	define rnw_if							retnul_gwarn_if
#	define rni_if							retnul_ginfo_if
#	define rnes_if							retnul_gserror_if
#	define rnws_if							retnul_gswarn_if
#	define rnis_if							retnul_gsinfo_if
#endif

#ifndef ree_if
#	define ree_if							reterr_gerror_if
#	define rew_if							reterr_gwarn_if
#	define rei_if							reterr_ginfo_if
#	define rees_if							reterr_gserror_if
#	define rews_if							reterr_gswarn_if
#	define reis_if							reterr_gsinfo_if

#	define re_if							ret_gerror_if
#	define rw_if							ret_gwarn_if
#	define ri_if							ret_ginfo_if
#	define res_if							ret_gserror_if
#	define rws_if							ret_gswarn_if
#	define ris_if							ret_gsinfo_if

#	define ree_if_failed(condition, format, ...)	ree_if(::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define rew_if_failed(condition, format, ...)	rew_if(::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define rei_if_failed(condition, format, ...)	rei_if(::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define rea_if_failed(condition, format, ...)	rea_if(::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define rees_if_failed(condition)				rees_if(::gpk::failed(::gpk::error_t(condition)))
#	define rews_if_failed(condition)				rews_if(::gpk::failed(::gpk::error_t(condition)))
#	define reis_if_failed(condition)				reis_if(::gpk::failed(::gpk::error_t(condition)))
#	define reas_if_failed(condition)				reas_if(::gpk::failed(::gpk::error_t(condition)))

#	define re_if_failed(condition, format, ...)	re_if(::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define rw_if_failed(condition, format, ...)	rw_if(::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define ri_if_failed(condition, format, ...)	ri_if(::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define ra_if_failed(condition, format, ...)	ra_if(::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define res_if_failed(condition)				res_if(::gpk::failed(::gpk::error_t(condition)))
#	define rws_if_failed(condition)				rws_if(::gpk::failed(::gpk::error_t(condition)))
#	define ris_if_failed(condition)				ris_if(::gpk::failed(::gpk::error_t(condition)))
#	define ras_if_failed(condition)				ras_if(::gpk::failed(::gpk::error_t(condition)))


#	define e_if_failed(condition, format, ...)	e_if(::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define w_if_failed(condition, format, ...)	w_if(::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define i_if_failed(condition, format, ...)	i_if(::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define a_if_failed(condition, format, ...)	a_if(::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define es_if_failed(condition)				es_if(::gpk::failed(::gpk::error_t(condition)))
#	define ws_if_failed(condition)				ws_if(::gpk::failed(::gpk::error_t(condition)))
#	define is_if_failed(condition)				is_if(::gpk::failed(::gpk::error_t(condition)))
#	define as_if_failed(condition)				as_if(::gpk::failed(::gpk::error_t(condition)))

#	define ce_if_failed(condition, format, ...)	ce_if(::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define cw_if_failed(condition, format, ...)	cw_if(::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define ci_if_failed(condition, format, ...)	ci_if(::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define ces_if_failed(condition)				ces_if(::gpk::failed(::gpk::error_t(condition)))
#	define cws_if_failed(condition)				cws_if(::gpk::failed(::gpk::error_t(condition)))
#	define cis_if_failed(condition)				cis_if(::gpk::failed(::gpk::error_t(condition)))

#	define be_if_failed(condition, format, ...)	be_if(::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define bw_if_failed(condition, format, ...)	bw_if(::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define bi_if_failed(condition, format, ...)	bi_if(::gpk::failed(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define bes_if_failed(condition)				bes_if(::gpk::failed(::gpk::error_t(condition)))
#	define bws_if_failed(condition)				bws_if(::gpk::failed(::gpk::error_t(condition)))
#	define bis_if_failed(condition)				bis_if(::gpk::failed(::gpk::error_t(condition)))

///

#	define ree_if_succeeded(condition, format, ...)	ree_if(::gpk::succeeded(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define rew_if_succeeded(condition, format, ...)	rew_if(::gpk::succeeded(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define rei_if_succeeded(condition, format, ...)	rei_if(::gpk::succeeded(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define rea_if_succeeded(condition, format, ...)	rea_if(::gpk::succeeded(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define rees_if_succeeded(condition)				rees_if(::gpk::succeeded(::gpk::error_t(condition)))
#	define rews_if_succeeded(condition)				rews_if(::gpk::succeeded(::gpk::error_t(condition)))
#	define reis_if_succeeded(condition)				reis_if(::gpk::succeeded(::gpk::error_t(condition)))
#	define reas_if_succeeded(condition)				reas_if(::gpk::succeeded(::gpk::error_t(condition)))

#	define re_if_succeeded(condition, format, ...)	re_if(::gpk::succeeded(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define rw_if_succeeded(condition, format, ...)	rw_if(::gpk::succeeded(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define ri_if_succeeded(condition, format, ...)	ri_if(::gpk::succeeded(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define ra_if_succeeded(condition, format, ...)	ra_if(::gpk::succeeded(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define res_if_succeeded(condition)				res_if(::gpk::succeeded(::gpk::error_t(condition)))
#	define rws_if_succeeded(condition)				rws_if(::gpk::succeeded(::gpk::error_t(condition)))
#	define ris_if_succeeded(condition)				ris_if(::gpk::succeeded(::gpk::error_t(condition)))
#	define ras_if_succeeded(condition)				ras_if(::gpk::succeeded(::gpk::error_t(condition)))


#	define e_if_succeeded(condition, format, ...)	e_if(::gpk::succeeded(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define w_if_succeeded(condition, format, ...)	w_if(::gpk::succeeded(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define i_if_succeeded(condition, format, ...)	i_if(::gpk::succeeded(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define a_if_succeeded(condition, format, ...)	a_if(::gpk::succeeded(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define es_if_succeeded(condition)				es_if(::gpk::succeeded(::gpk::error_t(condition)))
#	define ws_if_succeeded(condition)				ws_if(::gpk::succeeded(::gpk::error_t(condition)))
#	define is_if_succeeded(condition)				is_if(::gpk::succeeded(::gpk::error_t(condition)))
#	define as_if_succeeded(condition)				as_if(::gpk::succeeded(::gpk::error_t(condition)))

#	define ce_if_succeeded(condition, format, ...)	ce_if(::gpk::succeeded(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define cw_if_succeeded(condition, format, ...)	cw_if(::gpk::succeeded(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define ci_if_succeeded(condition, format, ...)	ci_if(::gpk::succeeded(::gpk::error_t(condition)), format, __VA_ARGS__)
#	define ces_if_succeeded(condition)				ces_if(::gpk::succeeded(::gpk::error_t(condition)))
#	define cws_if_succeeded(condition)				cws_if(::gpk::succeeded(::gpk::error_t(condition)))
#	define cis_if_succeeded(condition)				cis_if(::gpk::succeeded(::gpk::error_t(condition)))
#endif

#ifndef rwe_if
#	define rwe_if							retwarn_gerror_if
#	define rww_if							retwarn_gwarn_if
#	define rwi_if							retwarn_ginfo_if
#	define rwes_if							retwarn_gserror_if
#	define rwws_if							retwarn_gswarn_if
#	define rwis_if							retwarn_gsinfo_if
#endif

#endif // GPK_LOG_H_23627
