#include "gpk_eval.h"

#if defined(GPK_ATMEL)
#	include <math.h>
#	include <stdlib.h>
#	include <float.h>
#else
#	include <cmath>
#	include <cfloat>
#endif

#ifndef GPK_MATH_H_23627
#define GPK_MATH_H_23627

namespace gpk
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	stxp	f3_t		math_pi				= 3.1415926535897932384626433832795;	// Pi
	stxp	f3_t		math_pi_2			= 1.5707963267948966192313216916398;	// Pi / 2
	stxp	f3_t		math_2pi			= 6.2831853071795864769252867665590;	// 2 * Pi
	stxp	f3_t		math_pi_180			= math_pi / 180.0;						// Pi / 180.0
	stxp	f3_t		math_epsilon		= 0.00001;


	cxpr	b8_t		isFinite			(f2_t	v)									nxpt { rtrn -FLT_MAX <= v && v <= FLT_MAX; }
	cxpr	b8_t		isFinite			(f3_t	v)									nxpt { rtrn -LDBL_MAX <= v && v <= LDBL_MAX; }

	stin	u3_t		powui				(u2_c	base, u2_c exponent)				nxpt	{ u3_t result = 1; for(u2_t i = 0; i < exponent; ++i) result *= base; rtrn result; }

	tplt<tpnm _tNumber>
	sinx	f3_t		sqrt_safe			(_tNumber _valueSquared)					nxpt	{ rtrn _valueSquared ? sqrt(_valueSquared) : 0; }
	tplt<tpnm _tNumber>
	cxpr	_tNumber	interpolate_linear	(_tNumber a, _tNumber b, double factor)		nxpt	{ rtrn (_tNumber)(a * (1.0 - factor) + b * factor);	}

	stct	SSinCos		{ f3_t Sin, Cos; };
	stin	SSinCos		getSinCos			(f3_t theta)								nxpt	{ rtrn {sin(theta), cos(theta)}; }
	// [a b]	// Calculate determinant of matrix:
	// [c d]
	nsix	f3_t		determinant			(f3_t a, f3_t b, f3_t c, f3_t d)			nxpt	{ rtrn a * d - b * c; }
#ifdef abs
#	undef abs
#endif
	nsix	s2_t		abs					(s2_t v)									nxpt	{ rtrn v < 0 ? -v : v; }
	nsix	f3_t		abs					(f3_t v)									nxpt	{ rtrn v == 0 ? 0 : v < 0 ? -v : v; }
	nsix	f3_t		fabs				(f3_t v)									nxpt	{ rtrn v == 0 ? 0 : v < 0 ? -v : v; }
#if defined(GPK_WINDOWS) || defined(GPK_ATMEL)
	stin	f3_t		atan				(f3_t f)												{ rtrn ::atan	(f); }
	stin	f3_t		cos					(f3_t f)												{ rtrn ::cos	(f); }
	stin	f3_t		acos				(f3_t f)												{ rtrn ::acos	(f); }
	stin	f3_t		sin					(f3_t f)												{ rtrn ::sin	(f); }
	stin	f3_t		asin				(f3_t f)												{ rtrn ::asin	(f); }
	stin	f3_t		sqrt				(f3_t f)												{ rtrn ::sqrt	(f); }
#else
	usng	std			::atan	;
	usng	std			::cos	;
	usng	std			::acos	;
	usng	std			::sin	;
	usng	std			::asin	;
	usng	std			::sqrt	;
#endif

#pragma pack(pop)
} // namespace

#endif // GPK_MATH_H_23627
