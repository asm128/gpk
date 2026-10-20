#include "gpk_eval.h"

#if defined(GPK_ATMEL)
#	include <math.h>
#	include <stdlib.h>
#else
#	include <cmath>
#endif

#ifndef GPK_MATH_H_23627
#define GPK_MATH_H_23627

namespace gpk
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	stxp	double		math_pi				= 3.1415926535897932384626433832795;	// Pi
	stxp	double		math_pi_2			= 1.5707963267948966192313216916398;	// Pi / 2
	stxp	double		math_2pi			= 6.2831853071795864769252867665590;	// 2 * Pi
	stxp	double		math_pi_180			= ::gpk::math_pi / 180.0;				// Pi / 180.0
	stxp	double		math_epsilon		= 0.00001;

	stin	uint64_t	powui				(u2_c	base, u2_c exponent)				noexcept	{ uint64_t result = 1; for(::gpk::u2_t i = 0; i < exponent; ++i) result *= base; return result;	}

	tplt<tpnm _tNumber>
	sinx	double		sqrt_safe			(_tNumber _valueSquared)									noexcept	{ return _valueSquared ? sqrt(_valueSquared) : 0;		}
	tplt<tpnm _tNumber>
	cxpr	_tNumber	interpolate_linear	(const _tNumber a, const _tNumber b, const double factor)	noexcept	{ return (_tNumber)(a * (1.0 - factor) + b * factor);	}

	stct SSinCos { double Sin, Cos; };

	stin	SSinCos		getSinCos			(double theta)												noexcept	{ return {sin(theta), cos(theta)};						}

	// Calculate determinant of matrix:
	// [a b]
	// [c d]
	cxpr	double		determinant			(double a, double b, double c, double d)					noexcept	{ return a * d - b * c; }

#if defined(GPK_WINDOWS) || defined(GPK_ATMEL)
#	if defined(GPK_WINDOWS)
	stin int		abs		(int	f) { return ::abs	(f); }
	stin double	abs		(double	f) { return ::abs	(f); }
#	else
#	ifdef abs
#		undef abs
#	endif
	stin int		abs		(int	f) { return ::abs	(f); }
	stin double	abs		(double	f) { return ::abs	(f); }
#	endif
	stin double	fabs	(double	f) { return ::fabs	(f); }
	stin double	atan	(double	f) { return ::atan	(f); }
	stin double	cos		(double	f) { return ::cos	(f); }
	stin double	acos	(double	f) { return ::acos	(f); }
	stin double	sin		(double	f) { return ::sin	(f); }
	stin double	asin	(double	f) { return ::asin	(f); }
	stin double	sqrt	(double	f) { return ::sqrt	(f); }
#else
	using	std::abs	;
	using	std::fabs	;
	using	std::atan	;
	using	std::cos	;
	using	std::acos	;
	using	std::sin	;
	using	std::asin	;
	using	std::sqrt	;
#endif

#pragma pack(pop)
} // namespace

#endif // GPK_MATH_H_23627


