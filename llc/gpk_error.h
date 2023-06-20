#include "gpk_typeint.h"

#ifndef GPK_ERROR_H_91823749823
#define GPK_ERROR_H_91823749823

namespace gpk
{
	typedef						int32_t					error_t;

	stincxp		bool					failed					(const error_t errorCode)		noexcept	{ return errorCode < 0;		}
	stincxp		bool					succeeded				(const error_t errorCode)		noexcept	{ return 0 <= errorCode;	}

	stacxpr	::gpk::error_t OS_OK                = 0;
	stacxpr	::gpk::error_t OS_ERROR             = -1;
	stacxpr	::gpk::error_t OS_BUSY              = -2;
	stacxpr	::gpk::error_t OS_TIMEOUT           = -3;
	stacxpr	::gpk::error_t OS_FULL              = -4;
	stacxpr	::gpk::error_t OS_EMPTY             = -5;
	stacxpr	::gpk::error_t OS_OVERRUN           = -6;
	stacxpr	::gpk::error_t OS_NOT_AVAILABLE     = -7;
	stacxpr	::gpk::error_t OS_NOT_FOUND         = -8;
	stacxpr	::gpk::error_t OS_INVALID_PARAMETER = -9;
	stacxpr	::gpk::error_t OS_FORBIDDEN         = -10;
}

#if !defined(errored)
#	define						errored(errVal)								((::gpk::error_t)(errVal) < 0) //(::gpk::failed(errVal))
#endif
#if !defined(not_errored)
#	define						not_errored(errVal)							::gpk::succeeded((::gpk::error_t)(errVal))
#endif

#define GPK_CRASH()			{ uint64_t * _tasdas = 0; for(uint32_t i = 0; i < 0xFFFFFFFF; ++i) _tasdas[i] = 0xFFFFFFFF00000000ULL; }	// No throw? Just crash.

#endif // GPK_ERROR_H_91823749823
