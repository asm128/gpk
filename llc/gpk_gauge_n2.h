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
	typedef	gauge<n2f2_t>	gauge2f32;
	typedef	gauge<n2f3_t>	gauge2f64;
	typedef	gauge<n2u0_t >	gauge2u8;
	typedef	gauge<n2u1_t>	gauge2u16;
	typedef	gauge<n2u2_t>	gauge2u32;
	typedef	gauge<n2u3_t>	gauge2u64;
	typedef	gauge<n2s0_t >	gauge2i8;
	typedef	gauge<n2s1_t>	gauge2i16;
	typedef	gauge<n2s2_t>	gauge2i32;
	typedef	gauge<n2i64>	gauge2i64;
#pragma pack(pop)
} // namespace

#endif // GPK_GAUGE_N2
