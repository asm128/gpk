#include "gpk_typeint.h"

#ifndef GPK_CPOW_H
#define GPK_CPOW_H

namespace gpk
{
	tplt<size_t exp>	stincxp	float64_t	cpow	(float64_t	base)	{ return base * cpow<exp - 1>(base); }
	tplt<size_t exp>	stincxp	float32_t	cpow	(float32_t	base)	{ return base * cpow<exp - 1>(base); }
	tplt<size_t exp>	stincxp	uint8_t		cpow	(uint8_t	base)	{ return base * cpow<exp - 1>(base); }
	tplt<size_t exp>	stincxp	uint16_t	cpow	(uint16_t	base)	{ return base * cpow<exp - 1>(base); }
	tplt<size_t exp>	stincxp	uint32_t	cpow	(uint32_t	base)	{ return base * cpow<exp - 1>(base); }
	tplt<size_t exp>	stincxp	uint64_t	cpow	(uint64_t	base)	{ return base * cpow<exp - 1>(base); }
	tplt<size_t exp>	stincxp	int8_t		cpow	(int8_t		base)	{ return base * cpow<exp - 1>(base); }
	tplt<size_t exp>	stincxp	int16_t		cpow	(int16_t	base)	{ return base * cpow<exp - 1>(base); }
	tplt<size_t exp>	stincxp	int32_t		cpow	(int32_t	base)	{ return base * cpow<exp - 1>(base); }
	tplt<size_t exp>	stincxp	int64_t		cpow	(int64_t	base)	{ return base * cpow<exp - 1>(base); }

	tplt<>	inlcxpr float64_t	cpow<0>	(float64_t	/*base*/)	{ return 1; }
	tplt<>	inlcxpr float32_t	cpow<0>	(float32_t	/*base*/)	{ return 1; }
	tplt<>	inlcxpr uint8_t		cpow<0>	(uint8_t	/*base*/)	{ return 1; }
	tplt<>	inlcxpr uint16_t	cpow<0>	(uint16_t	/*base*/)	{ return 1; }
	tplt<>	inlcxpr uint32_t	cpow<0>	(uint32_t	/*base*/)	{ return 1; }
	tplt<>	inlcxpr uint64_t	cpow<0>	(uint64_t	/*base*/)	{ return 1; }
	tplt<>	inlcxpr int8_t		cpow<0>	(int8_t		/*base*/)	{ return 1; }
	tplt<>	inlcxpr int16_t		cpow<0>	(int16_t	/*base*/)	{ return 1; }
	tplt<>	inlcxpr int32_t		cpow<0>	(int32_t	/*base*/)	{ return 1; }
	tplt<>	inlcxpr int64_t		cpow<0>	(int64_t	/*base*/)	{ return 1; }

	stacxpr	uint8_t	ASCII_ALPHABET_LENGTH	= ('Z' - 'A');
	stacxpr	uint8_t	ASCII_LETTER_COUNT		= ASCII_ALPHABET_LENGTH * 2;
	stacxpr	uint8_t	ASCII_DIGIT_COUNT		= ASCII_LETTER_COUNT + 10;

	stincxp char	digit_ascii	(uint64_t value, const uint8_t base=10) { return char(value % base + ((value % base < 10) ? '0' : 'A' - 10)); }

	tplt<uint8_t exp, tpnm TValue>
	stincxp	char	digit		(const TValue value, const TValue base=10) { 
		return ::gpk::digit_ascii(value / ::gpk::cpow<exp>(base), (uint8_t)base);
	}
} // namespace

#endif // GPK_CPOW_H
