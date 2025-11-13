/// Copyright 2009-2023 - asm128
#include "gpk_n2.h"
#include "gpk_range.h"

#ifndef GPK_RANGE_N2
#define GPK_RANGE_N2

namespace gpk
{
#pragma pack(push, 1)
	typedef	range<n2sc_t>	range2char;
	typedef	range<n2uc_t>	range2uchar;
	typedef	range<n2f2_t>	range2f32;
	typedef	range<n2f3_t>	range2f64;
	typedef	range<n2u0_t>	range2u8;
	typedef	range<n2u1_t>	range2u16;
	typedef	range<n2u2_t>	range2u32;
	typedef	range<n2u3_t>	range2u64;
	typedef	range<n2s0_t>	range2i8;
	typedef	range<n2s1_t>	range2i16;
	typedef	range<n2s2_t>	range2i32;
	typedef	range<n2s3_t>	range2i64;
#pragma pack(pop)
} // namespace

#endif // GPK_RANGE_N2
