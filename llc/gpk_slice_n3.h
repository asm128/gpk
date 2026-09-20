/// Copyright 2009-2023 - asm128
#include "gpk_n3.h"
#include "gpk_slice.h"

#ifndef GPK_SLICE_N3
#define GPK_SLICE_N3

namespace gpk
{
#pragma pack(push, 1)
	typedef	slice<n3sc_t>	slice3char;
	typedef	slice<n3uc_t>	slice3uchar;
	typedef	slice<n3f2_t>	slice3f32;
	typedef	slice<n3f3_t>	slice3f64;
	typedef	slice<n3u0_t>	slice3u8;
	typedef	slice<n3u1_t>	slice3u16;
	typedef	slice<n3u2_t>	slice3u32;
	typedef	slice<n3u3_t>	slice3u64;
	typedef	slice<n3s0_t >	slice3i8;
	typedef	slice<n3s1_t>	slice3i16;
	typedef	slice<n3s2_t>	slice3i32;
	typedef	slice<n3s3_t>	slice3i64;

#pragma pack(pop)
} // namespace

#endif // GPK_SLICE_N3
