/// Copyright 2009-2023 - asm128
#include "gpk_n2.h"
#include "gpk_range.h"

#ifndef GPK_RANGE_N2
#define GPK_RANGE_N2

namespace gpk
{
#pragma pack(push, 1)
	typedef	range<n2char>	range2char;
	typedef	range<n2uchar>	range2uchar;
	typedef	range<n2f32>	range2f32;
	typedef	range<n2f64>	range2f64;
	typedef	range<n2u8 >	range2u8;
	typedef	range<n2u16>	range2u16;
	typedef	range<n2u32>	range2u32;
	typedef	range<n2u64>	range2u64;
	typedef	range<n2i8 >	range2i8;
	typedef	range<n2i16>	range2i16;
	typedef	range<n2i32>	range2i32;
	typedef	range<n2i64>	range2i64;
#pragma pack(pop)
} // namespace

#endif // GPK_RANGE_N2
