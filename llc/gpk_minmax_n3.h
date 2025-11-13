/// Copyright 2009-2023 - asm128
#include "gpk_n3.h"
#include "gpk_minmax.h"

#ifndef GPK_MINMAX_N3
#define GPK_MINMAX_N3

namespace gpk
{
#pragma pack(push, 1)
	typedef	minmax<n3sc_t>	minmax3char;
	typedef	minmax<n3uc_t>	minmax3uchar;
	typedef	minmax<n3f2_t>	minmax3f32;
	typedef	minmax<n3f3_t>	minmax3f64;
	typedef	minmax<n3u0_t>	minmax3u8;
	typedef	minmax<n3u1_t>	minmax3u16;
	typedef	minmax<n3u2_t>	minmax3u32;
	typedef	minmax<n3u3_t>	minmax3u64;
	typedef	minmax<n3s0_t>	minmax3i8;
	typedef	minmax<n3s1_t>	minmax3i16;
	typedef	minmax<n3s2_t>	minmax3i32;
	typedef	minmax<n3s3_t>	minmax3i64;
#pragma pack(pop)
} // namespace

#endif // GPK_MINMAX_N3