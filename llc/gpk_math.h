#include "gpk_typeint.h"
#include "gpk_eval.h"

#if defined(GPK_ATMEL)
#	include <math.h>
#else
#	include <cmath>
#endif

#ifndef GPK_MATH_H_0237409283740982740928374
#define GPK_MATH_H_0237409283740982740928374

namespace gpk
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	static constexpr		const double	math_pi					= 3.1415926535897932384626433832795;	// Pi
	static constexpr		const double	math_pi_2				= 1.5707963267948966192313216916398;	// Pi / 2
	static constexpr		const double	math_2pi				= 6.2831853071795864769252867665590;	// 2 * Pi
	static constexpr		const double	math_pi_180				= ::gpk::math_pi / 180.0;				// Pi / 180.0
	static constexpr		const double	math_epsilon			= 0.00001;

	static inline			uint64_t		powui					(const uint32_t	base, const uint32_t	exponent)					noexcept	{ uint64_t result = 1; for(uint32_t i = 0; i < exponent; ++i) result *= base; return result;	}

	template<typename _tNumber>
	static inline constexpr	double			sqrt_safe				(_tNumber _valueSquared)											noexcept	{ return _valueSquared ? sqrt(_valueSquared) : 0;		}

	template<typename _tNumber>
	static inline constexpr	_tNumber		clamp					(const _tNumber _value, const _tNumber _min, const _tNumber _max)	noexcept	{ return ::gpk::min(_max, ::gpk::max(_min, _value));	}

	template<typename _tNumber>
	constexpr				_tNumber		interpolate_linear		(const _tNumber a, const _tNumber b, const double factor)			noexcept	{ return (_tNumber)(a * (1.0 - factor) + b * factor);	}

	struct					SPairSinCos		{ double Sin, Cos; };

	static inline			SPairSinCos		getSinCos				(double theta)														noexcept	{ return {sin(theta), cos(theta)};						}

	// Calculate determinant of matrix:
	// [a b]
	// [c d]
	constexpr				double			determinant				(double a, double b, double c, double d)							noexcept	{ return a * d - b * c; }

#if defined(GPK_ATMEL)
	static inline int		abs		(int	f) { return ::abs	(f); }
	static inline double	fabs	(double	f) { return ::fabs	(f); }
	static inline double	atan	(double	f) { return ::atan	(f); }
	static inline double	acos	(double	f) { return ::acos	(f); }
	static inline double	sin		(double	f) { return ::sin	(f); }
	static inline double	asin	(double	f) { return ::asin	(f); }
#else
	using	std::abs	;
	using	std::fabs	;
	using	std::atan	;
	using	std::acos	;
	using	std::sin	;
	using	std::asin	;
#endif
#pragma pack(pop)
}

#endif // GPK_MATH_H_0237409283740982740928374
