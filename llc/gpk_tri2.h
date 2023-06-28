#include "gpk_tri.h"
#include "gpk_n2.h"

#ifndef GPK_TRI2_H_23627
#define GPK_TRI2_H_23627

namespace gpk 
{
#pragma pack(push, 1)
	template<typename _tDimension>	struct tri2 : public tri<::gpk::n2<_tDimension>>		{
		typedef	_tDimension		T;
		typedef	::gpk::n2<T>	TVertex;

		using	tri<TVertex>	::A;
		using	tri<TVertex>	::B;
		using	tri<TVertex>	::C;
		using	tri<TVertex>	::tri;

		inlcxpr	tri2<float>		f32			()		const	noexcept	{ return {A.f32(), B.f32(), C.f32()}; }
		inlcxpr	tri2<double>	f64			()		const	noexcept	{ return {A.f64(), B.f64(), C.f64()}; }
		inlcxpr	tri2<uint8_t>	i8			()		const	noexcept	{ return {A.i8 (), B.i8 (), C.i8 ()}; }
		inlcxpr	tri2<uint16_t>	i16			()		const	noexcept	{ return {A.i16(), B.i16(), C.i16()}; }
		inlcxpr	tri2<uint32_t>	i32			()		const	noexcept	{ return {A.i32(), B.i32(), C.i32()}; }
		inlcxpr	tri2<uint64_t>	i64			()		const	noexcept	{ return {A.i64(), B.i64(), C.i64()}; }
		inlcxpr	tri2<int8_t>	u8			()		const	noexcept	{ return {A.u8 (), B.u8 (), C.u8 ()}; }
		inlcxpr	tri2<int16_t>	u16			()		const	noexcept	{ return {A.u16(), B.u16(), C.u16()}; }
		inlcxpr	tri2<int32_t>	u32			()		const	noexcept	{ return {A.u32(), B.u32(), C.u32()}; }
		inlcxpr	tri2<int64_t>	u64			()		const	noexcept	{ return {A.u64(), B.u64(), C.u64()}; }

		template<typename _tOther>
		tri2<_tOther>			Cast		()		const	noexcept		{
			return
				{ A.template Cast<_tOther>()
				, B.template Cast<_tOther>()
				, C.template Cast<_tOther>()
				};
		}
		bool					CulledX		(const ::gpk::minmax<T>& minMax)	const	noexcept		{
			return ((A.x  < minMax.Min) && (B.x  < minMax.Min) && (C.x  < minMax.Min))
				|| ((A.x >= minMax.Max) && (B.x >= minMax.Max) && (C.x >= minMax.Max))
				;
		}
		bool					CulledY		(const ::gpk::minmax<T>& minMax)	const	noexcept		{
			return ((A.y  < minMax.Min) && (B.y  < minMax.Min) && (C.y  < minMax.Min))
				|| ((A.y >= minMax.Max) && (B.y >= minMax.Max) && (C.y >= minMax.Max))
				;
		}
	};
	typedef	tri2<char>		tri2char;
	typedef	tri2<uchar_t>	tri2uchar;
	typedef	tri2<float>		tri2f32;
	typedef	tri2<double>	tri2f64;
	typedef	tri2<uint8_t>	tri2u8;
	typedef	tri2<uint16_t>	tri2u16;
	typedef	tri2<uint32_t>	tri2u32;
	typedef	tri2<uint64_t>	tri2u64;
	typedef	tri2<int8_t>	tri2i8;
	typedef	tri2<int16_t>	tri2i16;
	typedef	tri2<int32_t>	tri2i32;
	typedef	tri2<int64_t>	tri2i64;

	typedef	minmax<tri2char>	minmaxtri2char;
	typedef	minmax<tri2uchar>	minmaxtri2uchar;
	typedef	minmax<tri2f32>		minmaxtri2f32;
	typedef	minmax<tri2f64>		minmaxtri2f64;
	typedef	minmax<tri2u8 >		minmaxtri2u8;
	typedef	minmax<tri2u16>		minmaxtri2u16;
	typedef	minmax<tri2u32>		minmaxtri2u32;
	typedef	minmax<tri2u64>		minmaxtri2u64;
	typedef	minmax<tri2i8 >		minmaxtri2i8;
	typedef	minmax<tri2i16>		minmaxtri2i16;
	typedef	minmax<tri2i32>		minmaxtri2i32;
	typedef	minmax<tri2i64>		minmaxtri2i64;
#pragma pack(pop)

	template<typename T>
	::gpk::tri2<T>&		translate		(::gpk::tri2<T> & triangle, const ::gpk::n2<T> & translation)	{
		triangle.A			+= translation;
		triangle.B			+= translation;
		triangle.C			+= translation;
		return triangle;
	}

	template<typename T>
	::gpk::n2<T>		triangleWeight	(const ::gpk::tri<T> & weights, const ::gpk::tri2<T> & values)	{ return values.A * weights.A + values.B * weights.B + values.C * weights.C; }
} // namespace 

#endif // GPK_TRI2_H_23627