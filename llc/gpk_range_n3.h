/// Copyright 2009-2023 - asm128
#include "gpk_n3.h"
#include "gpk_range.h"

#ifndef GPK_RANGE_N3
#define GPK_RANGE_N3

namespace gpk
{
#pragma pack(push, 1)
	typedef	range<n3char>	range3char;
	typedef	range<n3uchar>	range3uchar;
	typedef	range<n3f2_t>	range3f32;
	typedef	range<n3f3_t>	range3f64;
	typedef	range<n3u0_t >	range3u8;
	typedef	range<n3u16>	range3u16;
	typedef	range<n3u32>	range3u32;
	typedef	range<n3u64>	range3u64;
	typedef	range<n3i8 >	range3i8;
	typedef	range<n3i16>	range3i16;
	typedef	range<n3s2_t>	range3i32;
	typedef	range<n3i64>	range3i64;
#pragma pack(pop)
} // namespace

#endif // GPK_RANGE_N3
