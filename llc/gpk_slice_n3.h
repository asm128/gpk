/// Copyright 2009-2023 - asm128
#include "gpk_n3.h"
#include "gpk_slice.h"

#ifndef GPK_SLICE_N3
#define GPK_SLICE_N3

namespace gpk
{
#pragma pack(push, 1)
	typedef	slice<n3char>	slice3char;
	typedef	slice<n3uchar>	slice3uchar;
	typedef	slice<n3f32>	slice3f32;
	typedef	slice<n3f64>	slice3f64;
	typedef	slice<n3u8 >	slice3u8;
	typedef	slice<n3u16>	slice3u16;
	typedef	slice<n3u32>	slice3u32;
	typedef	slice<n3u64>	slice3u64;
	typedef	slice<n3i8 >	slice3i8;
	typedef	slice<n3i16>	slice3i16;
	typedef	slice<n3i32>	slice3i32;
	typedef	slice<n3i64>	slice3i64;
#pragma pack(pop)
} // namespace

#endif // GPK_SLICE_N3
