#include "gpk_tri.h"
#include "gpk_n2.h"
#include "gpk_minmax.h"

#ifndef GPK_TRI2_H_23627
#define GPK_TRI2_H_23627

namespace gpk 
{
#pragma pack(push, 1)
	tplt<tpnm _tDimension>	struct tri2 : public tri<::gpk::n2<_tDimension>>		{
		typedef	_tDimension		T;
		typedef	::gpk::n2<T>	TVertex;

		using	tri<TVertex>	::A;
		using	tri<TVertex>	::B;
		using	tri<TVertex>	::C;
		using	tri<TVertex>	::tri;

		inlcxpr	tri2<float>		f2_t			()		const	noexcept	{ return {A.f2_t(), B.f2_t(), C.f2_t()}; }
		inlcxpr	tri2<double>	f3_t			()		const	noexcept	{ return {A.f3_t(), B.f3_t(), C.f3_t()}; }
		inlcxpr	tri2<uint8_t>	s0_t			()		const	noexcept	{ return {A.s0_t (), B.s0_t (), C.s0_t ()}; }
		inlcxpr	tri2<uint16_t>	s1_t			()		const	noexcept	{ return {A.s1_t(), B.s1_t(), C.s1_t()}; }
		inlcxpr	tri2<uint32_t>	s2_t			()		const	noexcept	{ return {A.s2_t(), B.s2_t(), C.s2_t()}; }
		inlcxpr	tri2<uint64_t>	s3_t			()		const	noexcept	{ return {A.s3_t(), B.s3_t(), C.s3_t()}; }
		inlcxpr	tri2<int8_t>	u0_t			()		const	noexcept	{ return {A.u0_t (), B.u0_t (), C.u0_t ()}; }
		inlcxpr	tri2<int16_t>	u1_t			()		const	noexcept	{ return {A.u1_t(), B.u1_t(), C.u1_t()}; }
		inlcxpr	tri2<int32_t>	u2_t			()		const	noexcept	{ return {A.u2_t(), B.u2_t(), C.u2_t()}; }
		inlcxpr	tri2<int64_t>	u3_t			()		const	noexcept	{ return {A.u3_t(), B.u3_t(), C.u3_t()}; }

		tplt<tpnm _tOther>
		tri2<_tOther>			Cast		()		const	noexcept		{
			return
				{ A.tplt Cast<_tOther>()
				, B.tplt Cast<_tOther>()
				, C.tplt Cast<_tOther>()
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
	typedef	tri2<sc_t>		tri2char;
	typedef	tri2<uc_t>	tri2uchar;
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

	tplt<tpnm T>
	::gpk::tri2<T>&		translate		(::gpk::tri2<T> & triangle, const ::gpk::n2<T> & translation)	{
		triangle.A			+= translation;
		triangle.B			+= translation;
		triangle.C			+= translation;
		return triangle;
	}

	tplt<tpnm T>
	::gpk::n2<T>		triangleWeight	(const ::gpk::tri<T> & weights, const ::gpk::tri2<T> & values)	{ return values.A * weights.A + values.B * weights.B + values.C * weights.C; }
} // namespace 

#endif // GPK_TRI2_H_23627