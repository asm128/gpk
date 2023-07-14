/// Copyright 2009-2023 - asm128
#include "gpk_n2.h"
#include "gpk_slice.h"

#ifndef GPK_SLICE_N2
#define GPK_SLICE_N2

namespace gpk
{
#pragma pack(push, 1)
	typedef	slice<n2char>	slice2char;
	typedef	slice<n2uchar>	slice2uchar;
	typedef	slice<n2f32>	slice2f32;
	typedef	slice<n2f64>	slice2f64;
	typedef	slice<n2u8 >	slice2u8;
	typedef	slice<n2u16>	slice2u16;
	typedef	slice<n2u32>	slice2u32;
	typedef	slice<n2u64>	slice2u64;
	typedef	slice<n2i8 >	slice2i8;
	typedef	slice<n2i16>	slice2i16;
	typedef	slice<n2i32>	slice2i32;
	typedef	slice<n2i64>	slice2i64;
#pragma pack(pop)
} // namespace

#endif // GPK_SLICE_N2
