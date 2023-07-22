#include "gpk_quad.h"
#include "gpk_n3.h"

#ifndef GPK_QUAD3_H_23627
#define GPK_QUAD3_H_23627

namespace gpk 
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	tplt<tpnm _tDimension> 
	struct quad3 : public ::gpk::quad<::gpk::n3<_tDimension>> {
		typedef _tDimension		T;
		typedef n3<T>			TVertex;
		typedef quad<TVertex>	TQuad;
		typedef quad3<T>		TQuad3;

		using quad<TVertex>		::A;
		using quad<TVertex>		::B;
		using quad<TVertex>		::C;
		using quad<TVertex>		::D;
		using quad<TVertex>		::quad;
	};
	typedef quad3<float>	quad3f32, quad3f;
	typedef quad3<double>	quad3f64, quad3d;
	typedef quad3<uint8_t>	quad3u8;
	typedef quad3<uint16_t>	quad3u16;
	typedef quad3<uint32_t>	quad3u32;
	typedef quad3<uint64_t>	quad3u64;
	typedef quad3<int8_t>	quad3i8;
	typedef quad3<int16_t>	quad3i16;
	typedef quad3<int32_t>	quad3i32;
	typedef quad3<int64_t>	quad3i64;
#pragma pack(pop)
}
#endif // GPK_QUAD3_H_23627