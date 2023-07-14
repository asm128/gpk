/// Copyright 2009-2023 - asm128
#include "gpk_n2.h"
#include "gpk_minmax.h"

#ifndef GPK_MINMAX_N2
#define GPK_MINMAX_N2

namespace gpk
{
#pragma pack(push, 1)
	typedef	minmax<n2char>	minmax2char;
	typedef	minmax<n2uchar>	minmax2uchar;
	typedef	minmax<n2f32>	minmax2f32;
	typedef	minmax<n2f64>	minmax2f64;
	typedef	minmax<n2u8 >	minmax2u8;
	typedef	minmax<n2u16>	minmax2u16;
	typedef	minmax<n2u32>	minmax2u32;
	typedef	minmax<n2u64>	minmax2u64;
	typedef	minmax<n2i8 >	minmax2i8;
	typedef	minmax<n2i16>	minmax2i16;
	typedef	minmax<n2i32>	minmax2i32;
	typedef	minmax<n2i64>	minmax2i64;
#pragma pack(pop)
} // namespace

#endif // GPK_MINMAX_N2
