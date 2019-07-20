#include "gpk_error.h"
#include "gpk_debug.h"
#include "gpk_size.h"
#include "gpk_string.h"

#include <cstdio>

#ifndef GPK_LOG_H_8927349654687654365
#define GPK_LOG_H_8927349654687654365

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
	void															_gpk_print_system_errors						(const char* prefix, uint32_t prefixLen);
	void															_base_debug_print								(const char* prefix, uint32_t prefixLen);

#define base_debug_print(prefix, prefixLen)	::gpk::_base_debug_print(prefix, (uint32_t)prefixLen)

	template<const size_t _sizePrefix, typename... TArgs>
	void															_gpk_debug_printf								(int severity, const char (&prefix)[_sizePrefix], uint32_t prefixLength, const char* format, const TArgs... args)			{
#if defined(GPK_CONSOLE_LOG_ENABLED)
		printf("%s", prefix);
#endif
		base_debug_print(prefix, prefixLength);
		char																customDynamicString	[8192]						= {0};
#if !defined(GPK_WINDOWS)
		const size_t														stringLength									= sprintf(customDynamicString, format, args...);
#else
		const size_t														stringLength									= sprintf_s(customDynamicString, format, args...);
#endif
#if defined(GPK_CONSOLE_LOG_ENABLED)
		printf("%s\n", customDynamicString);
#endif
		base_debug_print(customDynamicString, (int)stringLength);
		base_debug_print("\n", 1);
		if(2 >= severity)
			::gpk::_gpk_print_system_errors(prefix, prefixLength);
	}

#if defined(GPK_WINDOWS)
	template<size_t _sizePrefix, typename... TArgs>
	void															gpk_debug_printf								(int severity, int32_t line, const char (&prefixFormat)[_sizePrefix], const char * format, const TArgs... args)		{
		static char															prefixString	[_sizePrefix + 8]	= {};
		static const int 													prefixLength									= ::sprintf_s(prefixString, prefixFormat, severity, line);
		::gpk::_gpk_debug_printf(severity, prefixString, prefixLength == -1 ? 0 : prefixLength, format, args...);
	}
#else
	template<size_t _sizePrefix, typename... TArgs>
	void															gpk_debug_printf								(int severity, int32_t line, const char (&prefixFormat)[_sizePrefix], const char * function, const char * format, const TArgs... args)		{
		static char															prefixString	[_sizePrefix + 1024]			= {};
		static const int 													prefixLength									= ::sprintf(prefixString, prefixFormat, severity, __LINE__, function);
		::gpk::_gpk_debug_printf(severity, prefixString, prefixLength == -1 ? 0 : prefixLength, format, args...);
	}
#endif
	template<typename... _tArgs>	inline	void					dummy		(_tArgs...)		{}
}



#if !defined(GPK_WINDOWS)
#	define debug_printf(severity, severityStr, format, ...)			::gpk::gpk_debug_printf(severity, __LINE__, ":%u:" severityStr ":" __FILE__ "(%u){%s}:", __func__, format, ## __VA_ARGS__)
#else
#	define debug_printf(severity, severityStr, format, ...)			::gpk::gpk_debug_printf(severity, __LINE__, ":%u:" severityStr ":" __FILE__ "(%u){" __FUNCTION__ "}:", format, __VA_ARGS__)
#endif

#ifndef always_printf
#	define always_printf(format, ...)								debug_printf(3, "info"		, format, __VA_ARGS__)
#endif

#ifndef error_printf
#	if defined (GPK_ERROR_PRINTF_ENABLED)
#		define error_printf(format, ...)								do { debug_printf(1, "error"	, format, __VA_ARGS__); GPK_PLATFORM_CRT_BREAKPOINT(); } while(0)
#	else
#		define error_printf(format, ...)								do { ; GPK_PLATFORM_CRT_BREAKPOINT(); } while(0)
#	endif
#endif

#ifndef warning_printf
#	if defined (GPK_WARNING_PRINTF_ENABLED)
#		define warning_printf(format, ...)								debug_printf(2, "warning"	, format, __VA_ARGS__)
#	else
#		define warning_printf(format, ...)								do { ::gpk::dummy(__VA_ARGS__); } while(0)
#	endif
#endif

#ifndef info_printf
#	if defined (GPK_INFO_PRINTF_ENABLED)
#		define info_printf(format, ...)									debug_printf(3, "info"		, format, __VA_ARGS__)
#	else
#		define info_printf(format, ...)									do { ::gpk::dummy(__VA_ARGS__); } while(0)
#	endif
#endif

#ifndef success_printf
#	if defined (GPK_SUCCESS_PRINTF_ENABLED)
#		define success_printf(format, ...)								debug_printf(4, "info"		, format, __VA_ARGS__)
#	else
#		define success_printf(format, ...)								do { ::gpk::dummy(__VA_ARGS__); } while(0)
#	endif
#endif

#ifndef verbose_printf
#	if defined (GPK_VERBOSE_PRINTF_ENABLED)
#		define verbose_printf(format, ...)								debug_printf(4, "info"		, format, __VA_ARGS__)
#	else
#		define verbose_printf(format, ...)								do { ::gpk::dummy(__VA_ARGS__); } while(0)
#	endif
#endif

#if defined (GPK_WINDOWS)
#	define gpk_throw(...)											throw(__VA_ARGS__)
#else
#	define gpk_throw(...)											do { char * nulp = 0; int i = 0; while(++i) nulp[i] = (char)i; ::gpk::dummy(__VA_ARGS__); } while(0)
#endif

#ifndef gthrow_if
#	ifndef GPK_NULLIFY_CONDITIONAL_THROW
#		define gthrow_if(condition, format, ...)						if(condition) { error_printf	(format, __VA_ARGS__); base_debug_print("Condition: " #condition "\n", (uint32_t)-1); gpk_throw("");	}
#	else
/	/#	pragma warning(disable:4552)	// this is required because "condition" may have no side effect.
/	/#	pragma warning(disable:4553)	// this is required because "condition" may have no side effect.
#		define gthrow_if(condition, format, ...)						if(condition) do{::gpk::dummy(__VA_ARGS__); } while(0)
#	endif
#endif

#ifndef gerror_if
#	ifndef GPK_NULLIFY_CONDITIONAL_LOG
#			define gerror_if(condition, format, ...)						if(condition) { error_printf	(format, __VA_ARGS__); base_debug_print("Condition: " #condition "\n", (uint32_t)-1); 						}
#			define gwarn_if(condition, format, ...)							if(condition) { warning_printf	(format, __VA_ARGS__); base_debug_print("Condition: " #condition "\n", (uint32_t)-1); 						}
#			define ginfo_if(condition, format, ...)							if(condition) { info_printf		(format, __VA_ARGS__); base_debug_print("Condition: " #condition "\n", (uint32_t)-1); 						}
#	else
//#	pragma warning(disable:4552)	// this is required because "condition" may have no side effect.
//#	pragma warning(disable:4553)	// this is required because "condition" may have no side effect.
#		define gerror_if(condition, format, ...)						if(condition) { do{ ::gpk::dummy(__VA_ARGS__); } while(0); }
#		define gwarn_if(condition, format, ...)							if(condition) { do{ ::gpk::dummy(__VA_ARGS__); } while(0); }
#		define ginfo_if(condition, format, ...)							if(condition) { do{ ::gpk::dummy(__VA_ARGS__); } while(0); }
#	endif
#endif

#ifndef ret_gerror_if
#	define ret_gerror_if(condition, format, ...)					if(condition) { error_printf	(format, __VA_ARGS__); return;			}
#	define ret_gwarn_if(condition, format, ...)						if(condition) { warning_printf	(format, __VA_ARGS__); return;			}
#	define ret_ginfo_if(condition, format, ...)						if(condition) { info_printf		(format, __VA_ARGS__); return;			}
#endif

#ifndef break_gerror_if
#	define break_gerror_if(condition, format, ...)					if(condition) { error_printf	(format, __VA_ARGS__); break;			}
#	define break_gwarn_if(condition, format, ...)					if(condition) { warning_printf	(format, __VA_ARGS__); break;			}
#	define break_ginfo_if(condition, format, ...)					if(condition) { info_printf		(format, __VA_ARGS__); break;			}
#endif

#ifndef continue_gerror_if
#	define continue_gerror_if(condition, format, ...)				if(condition) { error_printf	(format, __VA_ARGS__); continue;		}
#	define continue_gwarn_if(condition, format, ...)				if(condition) { warning_printf	(format, __VA_ARGS__); continue;		}
#	define continue_ginfo_if(condition, format, ...)				if(condition) { info_printf		(format, __VA_ARGS__); continue;		}
#endif

#ifndef retval_gerror_if
#	define retval_gerror_if(retVal, condition, format, ...)			if(condition) { error_printf	(format, __VA_ARGS__); return retVal;	}
#	define retval_gwarn_if(retVal, condition, format, ...)			if(condition) { warning_printf	(format, __VA_ARGS__); return retVal;	}
#	define retval_ginfo_if(retVal, condition, format, ...)			if(condition) { info_printf		(format, __VA_ARGS__); return retVal;	}
#endif

#ifndef retnul_gerror_if
#	define retnul_gerror_if(condition, format, ...)					retval_gerror_if	( 0, condition, format, __VA_ARGS__)
#	define retnul_gwarn_if(condition, format, ...)					retval_gwarn_if		( 0, condition, format, __VA_ARGS__)
#	define retnul_ginfo_if(condition, format, ...)					retval_ginfo_if		( 0, condition, format, __VA_ARGS__)
#endif

#ifndef reterr_gerror_if
#	ifndef GPK_NULLIFY_CONDITIONAL_RETERR
#		define reterr_gerror_if(condition, format, ...)					retval_gerror_if	(-1, condition, format, __VA_ARGS__)
#		define reterr_gwarn_if(condition, format, ...)					retval_gwarn_if		(-1, condition, format, __VA_ARGS__)
#		define reterr_ginfo_if(condition, format, ...)					retval_ginfo_if		(-1, condition, format, __VA_ARGS__)
#	else
#		pragma warning(disable:4552)	// this is required because "condition" may have no side effect.
#		pragma warning(disable:4553)	// this is required because "condition" may have no side effect.
#		define reterr_gerror_if(condition, format, ...)					if(condition) { do{::gpk::dummy(__VA_ARGS__); } while(0); }
#		define reterr_gwarn_if(condition, format, ...)					if(condition) { do{::gpk::dummy(__VA_ARGS__); } while(0); }
#		define reterr_ginfo_if(condition, format, ...)					if(condition) { do{::gpk::dummy(__VA_ARGS__); } while(0); }
#	endif
#endif

#ifndef retwarn_gerror_if
#	define retwarn_gerror_if(condition, format, ...)				retval_gerror_if( 1, condition, format, __VA_ARGS__)
#	define retwarn_gwarn_if(condition, format, ...)					retval_gwarn_if	( 1, condition, format, __VA_ARGS__)
#	define retwarn_ginfo_if(condition, format, ...)					retval_ginfo_if	( 1, condition, format, __VA_ARGS__)
#endif


#if defined (GPK_ERROR_PRINTF_ENABLED)
// Non-propagable retval_error call.
#	define gpk_rve_ecall(retVal, gpkl_call, format, ...) do {																														\
		::gpk::error_t gpk_errCall_ = (gpkl_call);  																																\
		if(gpk_errCall_ < 0) {																																						\
			debug_printf(0, "error", "%s: 0x%X.", #gpkl_call, gpk_errCall_);																										\
			error_printf(format, __VA_ARGS__); 																																		\
			return retVal; 																																							\
		}																																											\
		else {																																										\
			success_printf("%s: Success (0x%X).", #gpkl_call, gpk_errCall_);																										\
		}																																											\
	} while(0)

// Non-propagable retval_error error-warning call.
#	define gpk_rve_ewcall(retVal, gpkl_call, format, ...) do {																														\
		if(::gpk::error_t gpk_errCall_ = (gpkl_call)) { 																															\
			if(gpk_errCall_ < 0) {																																					\
				debug_printf(0, "error", "%s: 0x%X.", #gpkl_call, gpk_errCall_);																									\
				error_printf(format, __VA_ARGS__); 																																	\
				return retval; 																																						\
			}																																										\
			else {																																									\
				warning_printf("%s: 0x%X.", #gpkl_call, gpk_errCall_);																												\
			}																																										\
		}																																											\
		else {																																										\
			success_printf("%s: Success (0x%X).", #gpkl_call, gpk_errCall_);																										\
		}																																											\
	} while(0)

//
#	define gpk_rv_hrcall(retVal, hr_call) do {																																		\
		::HRESULT errCall_ = (hr_call);  																																			\
		if FAILED(errCall_) {																																						\
			debug_printf(0, "error", "%s: (0x%X) : '%s'.", #hr_call, errCall_, ::gpk::getWindowsErrorAsString(errCall_).begin());													\
			return retVal; 																																							\
		}																																											\
		else {																																										\
			success_printf("%s: Success (0x%X).", #hr_call, errCall_);																												\
		}																																											\
	} while(0)

//
#	define gpk_rve_hrcall(retVal, hr_call, format, ...) do {																														\
		::HRESULT errCall_ = (hr_call);  																																			\
		if FAILED(errCall_) {																																						\
			debug_printf(0, "error", "%s: (0x%X) : '%s' - " format, #hr_call, errCall_, ::gpk::getWindowsErrorAsString(errCall_).begin(), __VA_ARGS__);								\
			return retVal; 																																							\
		}																																											\
		else {																																										\
			success_printf("%s: Success (0x%X).", #hr_call, errCall_);																												\
		}																																											\
	} while(0)

// --------------------------------------------------------------------
// Propagable retval_error call.
#	define gpk_pecall(gpkl_call, ...) do {																																			\
		::gpk::error_t gpk_errCall_ = (gpkl_call);  																																\
		if(gpk_errCall_ < 0) {																																						\
			debug_printf(0, "error", "%s: 0x%X", #gpkl_call, gpk_errCall_);																											\
			error_printf(__VA_ARGS__); 																																				\
			return gpk_errCall_; 																																					\
		}																																											\
		else {																																										\
			success_printf("%s: Success (0x%X).", #gpkl_call, gpk_errCall_);																										\
		}																																											\
	} while(0)

// Propagable retval_error error-warning call.
#	define gpk_pewcall(gpkl_call, ...) do {																																			\
		if(::gpk::error_t gpk_errCall_ = (gpkl_call)) { 																															\
			if(gpk_errCall_ < 0) {																																					\
				debug_printf(0, "error", "%s: 0x%X", #gpkl_call, gpk_errCall_);																										\
				error_printf(__VA_ARGS__); 																																			\
				return gpk_errCall_; 																																				\
			}																																										\
			else {																																									\
				warning_printf("%s: 0x%X.", #gpkl_call, gpk_errCall_);																												\
			}																																										\
		}																																											\
		else {																																										\
			success_printf("%s: Success (0x%X).", #gpkl_call, gpk_errCall_);																										\
		}																																											\
	} while(0)

#else
#	define gpk_rve_ecall(retval, gpkl_call, ...) do {																																\
		if(::gpk::failed(gpkl_call))  																																				\
			return retval; 																																							\
	} while(0)

#	define gpk_rve_hrcall(retval, gpkl_call, ...) do {																																\
		if(FAILED(gpkl_call))  																																						\
			return retval; 																																							\
	} while(0)

#	define gpk_rv_hrcall(retval, gpkl_call) do {																																	\
		if(FAILED(gpkl_call))  																																						\
			return retval; 																																							\
	} while(0)

#	define gpk_pecall(gpkl_call, ...) do {																																			\
		::gpk::error_t gpk_errCall_ = (gpkl_call);																																	\
		if(::gpk::failed(gpk_errCall_)) 																																			\
			return gpk_errCall_; 																																					\
	} while(0)

#	define gpk_rve_ewcall											gpk_rve_ecall	// Non-propagable retval_error error-warning call.
#	define gpk_pewcall												gpk_pecall			// Propagable retval_error error-warning call.
#endif

#ifndef GPK_NULLIFY_NECALL
#	define gpk_necall(gpkl_call, ...)								gpk_rve_ecall (-1, gpkl_call, __VA_ARGS__)	// Non-propagable error call.
#	define gpk_newcall(gpkl_call, ...)								gpk_rve_ewcall(-1, gpkl_call, __VA_ARGS__)	// Non-propagable error-warning call.
#	define gpk_hrcall(hr_call)										gpk_rv_hrcall (-1, hr_call)					// HRESULT call.
#	define gpk_hrecall(hr_call, ...)								gpk_rve_hrcall(-1, hr_call, __VA_ARGS__)	// HRESULT call.
#else
#	define gpk_necall(gpkl_call, ...)								do{gpkl_call; ::gpk::dummy(__VA_ARGS__); } while(0) // Non-propagable error call.
#	define gpk_newcall(gpkl_call, ...)								do{gpkl_call; ::gpk::dummy(__VA_ARGS__); } while(0) // Non-propagable error-warning call.
#	define gpk_hrcall(hr_call)										do{hr_call	; ::gpk::dummy(__VA_ARGS__); } while(0) // HRESULT call.
#	define gpk_hrecall(hr_call, ...)								do{hr_call	; ::gpk::dummy(__VA_ARGS__); } while(0) // HRESULT call.
#endif

#ifndef e_if
#	define e_if														gerror_if
#	define w_if														gwarn_if
#	define i_if														ginfo_if
#endif

#ifndef re_if
#	define re_if													ret_gerror_if
#	define rw_if													ret_gwarn_if
#	define ri_if													ret_ginfo_if
#endif

#ifndef be_if
#	define be_if													break_gerror_if
#	define bw_if													break_gwarn_if
#	define bi_if													break_ginfo_if
#endif 

#ifndef ce_if
#	define ce_if													continue_gerror_if
#	define cw_if													continue_gwarn_if
#	define ci_if													continue_ginfo_if
#endif 

#ifndef rve_if
#	define rve_if													retval_gerror_if
#	define rvw_if													retval_gwarn_if
#	define rvi_if													retval_ginfo_if
#endif 

#ifndef rne_if
#	define rne_if													retnul_gerror_if
#	define rnw_if													retnul_gwarn_if
#	define rni_if													retnul_ginfo_if
#endif 

#ifndef ree_if
#	define ree_if													reterr_gerror_if
#	define rew_if													reterr_gwarn_if
#	define rei_if													reterr_ginfo_if
#endif 

#ifndef rwe_if
#	define rwe_if													retwarn_gerror_if
#	define rww_if													retwarn_gwarn_if
#	define rwi_if													retwarn_ginfo_if
#endif 

#endif // GPK_LOG_H_8927349654687654365
