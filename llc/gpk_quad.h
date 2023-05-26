#include "gpk_typeint.h"

#ifndef GPK_QUAD_H_230523
#define GPK_QUAD_H_230523

namespace gpk 
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	template<typename _tVertex> 
	struct quad	{
		typedef _tVertex	T;
		typedef quad<T>		TQuad;

		T					A, B, C, D;

		cnstxpr		quad	()									= default;
		cnstxpr		quad	(const TQuad & other)				= default;
		cnstxpr		quad	(const T & a, const T & b, const T & c, const T & d)	: A(a), B(b), C(c), D(d)	{}

		GPK_DEFAULT_OPERATOR_NE(TQuad, A == other.A && B == other.B && C == other.C && D == other.D);
	};
	typedef quad<float>		quadf32, quadf;
	typedef quad<double>	quadf64, quadd;
	typedef quad<uint8_t>	quadu8;
	typedef quad<uint16_t>	quadu16;
	typedef quad<uint32_t>	quadu32;
	typedef quad<uint64_t>	quadu64;
	typedef quad<int8_t>	quadi8;
	typedef quad<int16_t>	quadi16;
	typedef quad<int32_t>	quadi32;
	typedef quad<int64_t>	quadi64;
#pragma pack(pop)
}

#endif // GPK_QUAD_H_230523
