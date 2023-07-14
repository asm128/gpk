/// Copyright 2009-2023 - asm128
#include "gpk_n3.h"
#include "gpk_gauge.h"

#ifndef GPK_GAUGE_N3
#define GPK_GAUGE_N3

namespace gpk
{
#pragma pack(push, 1)
	typedef	gauge<n3char>	gauge3char;
	typedef	gauge<n3uchar>	gauge3uchar;
	typedef	gauge<n3f32>	gauge3f32;
	typedef	gauge<n3f64>	gauge3f64;
	typedef	gauge<n3u8 >	gauge3u8;
	typedef	gauge<n3u16>	gauge3u16;
	typedef	gauge<n3u32>	gauge3u32;
	typedef	gauge<n3u64>	gauge3u64;
	typedef	gauge<n3i8 >	gauge3i8;
	typedef	gauge<n3i16>	gauge3i16;
	typedef	gauge<n3i32>	gauge3i32;
	typedef	gauge<n3i64>	gauge3i64;
#pragma pack(pop)
} // namespace

#endif // GPK_GAUGE_N3
