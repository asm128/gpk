#include "gpk_typeint.h"

#ifndef GPK_ERROR_H_91823749823
#define GPK_ERROR_H_91823749823

namespace gpk
{
	typedef						int32_t					error_t;

	static inline constexpr		bool					failed					(const error_t errorCode)		noexcept	{ return errorCode < 0;		}
	static inline constexpr		bool					succeeded				(const error_t errorCode)		noexcept	{ return 0 <= errorCode;	}
}

#if !defined(errored)
#	define						errored(errVal)								((errVal) < 0) //(::gpk::failed(errVal))
#endif
#if !defined(not_errored)
#	define						not_errored(errVal)							(::gpk::succeeded(errVal))
#endif

#define GPK_CRASH()			{ uint64_t * _tasdas = 0; for(uint32_t i = 0; i < 0xFFFFFFFF; ++i) _tasdas[i] = 0xFFFFFFFF00000000ULL; }	// No throw? Just crash.

#endif // GPK_ERROR_H_91823749823
