#include "gpk_eval.h"

#if defined(GPK_ATMEL)
#	include <math.h>
#elif defined(GPK_WINDOWS)
#	include <math.h>
#else
#	include <cmath>
#endif

#ifndef GPK_MATH_H_23627
#define GPK_MATH_H_23627

namespace gpk
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	stacxpr	double		math_pi				= 3.1415926535897932384626433832795;	// Pi
	stacxpr	double		math_pi_2			= 1.5707963267948966192313216916398;	// Pi / 2
	stacxpr	double		math_2pi			= 6.2831853071795864769252867665590;	// 2 * Pi
	stacxpr	double		math_pi_180			= ::gpk::math_pi / 180.0;				// Pi / 180.0
	stacxpr	double		math_epsilon		= 0.00001;

	stainli	uint64_t	powui				(const uint32_t	base, const uint32_t exponent)				noexcept	{ uint64_t result = 1; for(uint32_t i = 0; i < exponent; ++i) result *= base; return result;	}

	template<typename _tNumber>
	stincxp	double		sqrt_safe			(_tNumber _valueSquared)									noexcept	{ return _valueSquared ? sqrt(_valueSquared) : 0;		}
	template<typename _tNumber>
	cnstxpr	_tNumber	interpolate_linear	(const _tNumber a, const _tNumber b, const double factor)	noexcept	{ return (_tNumber)(a * (1.0 - factor) + b * factor);	}

	struct SSinCos { double Sin, Cos; };

	stainli	SSinCos		getSinCos			(double theta)												noexcept	{ return {sin(theta), cos(theta)};						}

	// Calculate determinant of matrix:
	// [a b]
	// [c d]
	cnstxpr	double		determinant			(double a, double b, double c, double d)					noexcept	{ return a * d - b * c; }

#if defined(GPK_ATMEL)
	stainli int		abs		(int	f) { return ::abs	(f); }
	stainli double	abs		(double	f) { return ::abs	(f); }
	stainli double	fabs	(double	f) { return ::fabs	(f); }
	stainli double	atan	(double	f) { return ::atan	(f); }
	stainli double	cos		(double	f) { return ::cos	(f); }
	stainli double	acos	(double	f) { return ::acos	(f); }
	stainli double	sin		(double	f) { return ::sin	(f); }
	stainli double	asin	(double	f) { return ::asin	(f); }
	stainli double	sqrt	(double	f) { return ::sqrt	(f); }
#elif defined(GPK_WINDOWS)
	stainli int		abs		(int	f) { return ::abs	(f); }
	stainli double	abs		(double	f) { return ::abs	(f); }
	stainli double	fabs	(double	f) { return ::fabs	(f); }
	stainli double	atan	(double	f) { return ::atan	(f); }
	stainli double	cos		(double	f) { return ::cos	(f); }
	stainli double	acos	(double	f) { return ::acos	(f); }
	stainli double	sin		(double	f) { return ::sin	(f); }
	stainli double	asin	(double	f) { return ::asin	(f); }
	stainli double	sqrt	(double	f) { return ::sqrt	(f); }
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
}

#endif // GPK_MATH_H_23627
