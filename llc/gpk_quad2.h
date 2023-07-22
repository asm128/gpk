#include "gpk_quad.h"
#include "gpk_n2.h"

#ifndef GPK_QUAD2_H_23627
#define GPK_QUAD2_H_23627

namespace gpk 
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	tplt<tpnm _tDimension> 
	struct quad2 : public ::gpk::quad<::gpk::n2<_tDimension>> {
		typedef _tDimension		T;
		typedef	n2<T>			TVertex;
		typedef quad<TVertex>	TQuad;
		typedef quad2<T>		TQuad2;
		
		using quad<TVertex>		::A;
		using quad<TVertex>		::B;
		using quad<TVertex>		::C;
		using quad<TVertex>		::D;
		using quad<TVertex>		::quad;
	};
	typedef quad2<float>	quad2f32, quad2f;
	typedef quad2<double>	quad2f64, quad2d;
	typedef quad2<uint8_t>	quad2u8;
	typedef quad2<uint16_t>	quad2u16;
	typedef quad2<uint32_t>	quad2u32;
	typedef quad2<uint64_t>	quad2u64;
	typedef quad2<int8_t>	quad2i8;
	typedef quad2<int16_t>	quad2i16;
	typedef quad2<int32_t>	quad2i32;
	typedef quad2<int64_t>	quad2i64;
#pragma pack(pop)
}
#endif // GPK_QUAD2_H_23627