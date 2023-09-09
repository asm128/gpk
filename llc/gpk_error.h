#include "gpk_typeint.h"

#ifndef GPK_ERROR_H_23627
#define GPK_ERROR_H_23627

namespace gpk
{
	typedef	int32_t	error_t, err_t;

	stincxp	bool	failed		(const error_t errorCode)	noexcept	{ return errorCode < 0;		}
	stincxp	bool	succeeded	(const error_t errorCode)	noexcept	{ return 0 <= errorCode;	}

	stacxpr	::gpk::error_t OS_OK                = 0;
	stacxpr	::gpk::error_t OS_ERROR             = ::gpk::OS_OK                - 1;
	stacxpr	::gpk::error_t OS_BUSY              = ::gpk::OS_ERROR             - 1;
	stacxpr	::gpk::error_t OS_TIMEOUT           = ::gpk::OS_BUSY              - 1;
	stacxpr	::gpk::error_t OS_FULL              = ::gpk::OS_TIMEOUT           - 1;
	stacxpr	::gpk::error_t OS_EMPTY             = ::gpk::OS_FULL              - 1;
	stacxpr	::gpk::error_t OS_OVERRUN           = ::gpk::OS_EMPTY             - 1;
	stacxpr	::gpk::error_t OS_NOT_AVAILABLE     = ::gpk::OS_OVERRUN           - 1;
	stacxpr	::gpk::error_t OS_NOT_FOUND         = ::gpk::OS_NOT_AVAILABLE     - 1;
	stacxpr	::gpk::error_t OS_INVALID_PARAMETER = ::gpk::OS_NOT_FOUND         - 1;
	stacxpr	::gpk::error_t OS_FORBIDDEN         = ::gpk::OS_INVALID_PARAMETER - 1;
	stacxpr	::gpk::error_t OS_RESTART           = ::gpk::OS_FORBIDDEN         - 1;
	stacxpr	::gpk::error_t OS_WAKE_UP           = ::gpk::OS_RESTART           - 1;
	stacxpr	::gpk::error_t OS_SLEEP             = ::gpk::OS_WAKE_UP           - 1;
	stacxpr	::gpk::error_t OS_OFFLINE           = ::gpk::OS_SLEEP             - 1;
	stacxpr	::gpk::error_t OS_CONNECTING        = ::gpk::OS_OFFLINE           - 1;
	stacxpr	::gpk::error_t OS_CONNECTED         = ::gpk::OS_CONNECTING        - 1;
	stacxpr	::gpk::error_t OS_MISSING_DATA      = ::gpk::OS_CONNECTED         - 1;
	stacxpr	::gpk::error_t OS_NO_MEMORY         = ::gpk::OS_MISSING_DATA      - 1;
} // namespace

#if !defined(errored)
#	define	errored(errVal)			(::gpk::error_t(errVal) < 0)
#endif
#if !defined(not_errored)
#	define	not_errored(errVal)		::gpk::succeeded(::gpk::error_t(errVal))
#endif

#ifndef if_failed
#	define	if_failed(errVal)		if(::gpk::error_t(errVal) < 0)
#endif

#ifndef if_succeeded
#	define	if_succeeded(errVal)	if(::gpk::error_t(errVal) >= 0)
#endif

#ifdef GPK_ESP32
#	define GPK_CRASH()	do { uint64_t * _tasdas = 0; for(uint32_t i = 0; i < 0xFFFFFFFF; ++i) { for(uint32_t j = 0; j < 1000; ++j) delay(1); base_log_print("I had to do something with this.\n"); } } while(0)	// No throw? Just crash.
#elif defined(GPK_ARDUINO) 
#	define GPK_CRASH()	do { uint64_t * _tasdas = 0; for(uint32_t i = 0; i < 0xFFFFFFFF; ++i) { for(uint32_t j = 0; j < 1000; ++j) delay(1); base_log_print_F("I had to do something with this.\n"); } } while(0)	// No throw? Just crash.
#else
#	define GPK_CRASH()	do { uint64_t * _tasdas = 0; for(uint32_t i = 0; i < 0xFFFFFFFF; ++i) _tasdas[i] = 0xFFFFFFFF00000000ULL; } while(0)	// No throw? Just crash.
#endif

#endif // GPK_ERROR_H_23627
