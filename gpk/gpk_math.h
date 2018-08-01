#include "gpk_typeint.h"
#include "gpk_eval.h"

#include <cmath>

#ifndef GPK_MATH_H_0237409283740982740928374
#define GPK_MATH_H_0237409283740982740928374

namespace gpk
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	static constexpr		const double			math_pi					= 3.1415926535897932384626433832795;	// Pi
	static constexpr		const double			math_pi_2				= 1.5707963267948966192313216916398;	// Pi / 2
	static constexpr		const double			math_2pi				= 6.2831853071795864769252867665590;	// 2 * Pi
	static constexpr		const double			math_pi_180				= ::gpk::math_pi / 180.0;				// Pi / 180.0
	static constexpr		const double			math_epsilon			= 0.00001;

	template<typename _tNumber>	
	static inline constexpr	double					sqrt_safe				(_tNumber _valueSquared)													noexcept	{ return _valueSquared ? sqrt(_valueSquared) : 0;		}
	
	template<typename _tNumber>	
	static inline constexpr	_tNumber				clamp					(const _tNumber _value, const _tNumber _min, const _tNumber _max)			noexcept	{ return ::gpk::min(_max, ::gpk::max(_min, _value));	}

	template<typename _tNumber>	
	constexpr				_tNumber				interpolate_linear		(const _tNumber a, const _tNumber b, const double factor)					noexcept	{ return (_tNumber)(b * factor + a * (1.0 - factor));	}

	struct					SPairSinCos				{ double Sin, Cos; };	

	static inline			SPairSinCos				getSinCos				(double theta)																noexcept	{ return {sin(theta), cos(theta)};						}

	// Calculate determinant of matrix:
	// [a b]
	// [c d]
	constexpr				double					determinant				(double a, double b, double c, double d)									noexcept	{ return a * d - b * c; }
#pragma pack(pop)
}

#endif // GPK_MATH_H_0237409283740982740928374
