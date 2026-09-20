/// Copyright 2009-2023 - asm128
#include "gpk_n2.h"
#include "gpk_slice.h"

#ifndef GPK_SLICE_N2
#define GPK_SLICE_N2

namespace gpk
{
#pragma pack(push, 1)
	typedef	slice<n2sc_t>	slice2char;
	typedef	slice<n2uc_t>	slice2uchar;
	typedef	slice<n2f2_t>	slice2f32;
	typedef	slice<n2f3_t>	slice2f64;
	typedef	slice<n2u0_t>	slice2u8;
	typedef	slice<n2u1_t>	slice2u16;
	typedef	slice<n2u2_t>	slice2u32;
	typedef	slice<n2u3_t>	slice2u64;
	typedef	slice<n2s0_t >	slice2i8;
	typedef	slice<n2s1_t>	slice2i16;
	typedef	slice<n2s2_t>	slice2i32;
	typedef	slice<n2s3_t>	slice2i64;
#pragma pack(pop)
} // namespace

#endif // GPK_SLICE_N2
