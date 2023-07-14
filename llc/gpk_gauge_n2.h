/// Copyright 2009-2023 - asm128
#include "gpk_n2.h"
#include "gpk_gauge.h"

#ifndef GPK_GAUGE_N2
#define GPK_GAUGE_N2

namespace gpk
{
#pragma pack(push, 1)
	typedef	gauge<n2char>	gauge2char;
	typedef	gauge<n2uchar>	gauge2uchar;
	typedef	gauge<n2f32>	gauge2f32;
	typedef	gauge<n2f64>	gauge2f64;
	typedef	gauge<n2u8 >	gauge2u8;
	typedef	gauge<n2u16>	gauge2u16;
	typedef	gauge<n2u32>	gauge2u32;
	typedef	gauge<n2u64>	gauge2u64;
	typedef	gauge<n2i8 >	gauge2i8;
	typedef	gauge<n2i16>	gauge2i16;
	typedef	gauge<n2i32>	gauge2i32;
	typedef	gauge<n2i64>	gauge2i64;
#pragma pack(pop)
} // namespace

#endif // GPK_GAUGE_N2
