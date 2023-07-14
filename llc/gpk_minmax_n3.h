/// Copyright 2009-2023 - asm128
#include "gpk_n3.h"
#include "gpk_minmax.h"

#ifndef GPK_MINMAX_N3
#define GPK_MINMAX_N3

namespace gpk
{
#pragma pack(push, 1)
	typedef	minmax<n3char>	minmax3char;
	typedef	minmax<n3uchar>	minmax3uchar;
	typedef	minmax<n3f32>	minmax3f32;
	typedef	minmax<n3f64>	minmax3f64;
	typedef	minmax<n3u8 >	minmax3u8;
	typedef	minmax<n3u16>	minmax3u16;
	typedef	minmax<n3u32>	minmax3u32;
	typedef	minmax<n3u64>	minmax3u64;
	typedef	minmax<n3i8 >	minmax3i8;
	typedef	minmax<n3i16>	minmax3i16;
	typedef	minmax<n3i32>	minmax3i32;
	typedef	minmax<n3i64>	minmax3i64;
#pragma pack(pop)
} // namespace

#endif // GPK_MINMAX_N3