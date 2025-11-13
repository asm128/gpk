#include "gpk_tri.h"
#include "gpk_n3.h"
#include "gpk_minmax.h"

#ifndef GPK_TRI3_H_23627
#define GPK_TRI3_H_23627

namespace gpk 
{
#pragma pack(push, 1)
	tplt<tpnm _tDimension>	struct tri3 : public tri<::gpk::n3<_tDimension>>		{
		typedef	_tDimension		T;
		typedef	::gpk::n3<T>	TVertex;

		using	tri<TVertex>	::A;
		using	tri<TVertex>	::B;
		using	tri<TVertex>	::C;
		using	tri<TVertex>	::tri;

		inlcxpr	tri3<float>		f2_t			()		const	noexcept	{ return {A.f2_t(), B.f2_t(), C.f2_t()}; }
		inlcxpr	tri3<double>	f3_t			()		const	noexcept	{ return {A.f3_t(), B.f3_t(), C.f3_t()}; }
		inlcxpr	tri3<uint8_t>	s0_t			()		const	noexcept	{ return {A.s0_t(), B.s0_t(), C.s0_t()}; }
		inlcxpr	tri3<uint16_t>	s1_t			()		const	noexcept	{ return {A.s1_t(), B.s1_t(), C.s1_t()}; }
		inlcxpr	tri3<uint32_t>	s2_t			()		const	noexcept	{ return {A.s2_t(), B.s2_t(), C.s2_t()}; }
		inlcxpr	tri3<uint64_t>	s3_t			()		const	noexcept	{ return {A.s3_t(), B.s3_t(), C.s3_t()}; }
		inlcxpr	tri3<int8_t>	u0_t			()		const	noexcept	{ return {A.u0_t(), B.u0_t(), C.u0_t()}; }
		inlcxpr	tri3<int16_t>	u1_t			()		const	noexcept	{ return {A.u1_t(), B.u1_t(), C.u1_t()}; }
		inlcxpr	tri3<int32_t>	u2_t			()		const	noexcept	{ return {A.u2_t(), B.u2_t(), C.u2_t()}; }
		inlcxpr	tri3<int64_t>	u3_t			()		const	noexcept	{ return {A.u3_t(), B.u3_t(), C.u3_t()}; }

		tplt<tpnm _tOther>
		tri3<_tOther>			Cast		()		const	noexcept		{
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
		bool					CulledZ		(const ::gpk::minmax<T>& minMax)	const	noexcept		{
			return ((A.z  < minMax.Min) && (B.z  < minMax.Min) && (C.z  < minMax.Min))
				|| ((A.z >= minMax.Max) && (B.z >= minMax.Max) && (C.z >= minMax.Max))
				;
		}
		bool					CulledZSpecial	(const ::gpk::minmax<T>& minMax)	const	noexcept		{
			return ((A.z <= minMax.Min) || (B.z <= minMax.Min) || (C.z <= minMax.Min))
				|| ((A.z >= minMax.Max) && (B.z >= minMax.Max) && (C.z >= minMax.Max))
				;
		}
		bool					ClipZ		()									const	noexcept		{
			if(A.z < 0 || A.z >= 1) return true;
			if(B.z < 0 || B.z >= 1) return true;
			if(C.z < 0 || C.z >= 1) return true;
			return false;
		}
		tri3<T>&				Scale					(const TVertex & scale)				noexcept		{
			A.Scale(scale);
			B.Scale(scale);
			C.Scale(scale);
			return *this;
		}
		tri3<T>&				Translate				(const TVertex & translation)		noexcept		{
			A						+= translation;
			B						+= translation;
			C						+= translation;
			return *this;
		}
	};
	typedef	tri3<sc_t>		tri3char;
	typedef	tri3<uc_t>	tri3uchar;
	typedef	tri3<float>		tri3f32;
	typedef	tri3<double>	tri3f64;
	typedef	tri3<uint8_t>	tri3u8;
	typedef	tri3<uint16_t>	tri3u16;
	typedef	tri3<uint32_t>	tri3u32;
	typedef	tri3<uint64_t>	tri3u64;
	typedef	tri3<int8_t>	tri3i8;
	typedef	tri3<int16_t>	tri3i16;
	typedef	tri3<int32_t>	tri3i32;
	typedef	tri3<int64_t>	tri3i64;

	typedef	minmax<tri3char>	minmaxtri3char;
	typedef	minmax<tri3uchar>	minmaxtri3uchar;
	typedef	minmax<tri3f32>		minmaxtri3f32;
	typedef	minmax<tri3f64>		minmaxtri3f64;
	typedef	minmax<tri3u8 >		minmaxtri3u8;
	typedef	minmax<tri3u16>		minmaxtri3u16;
	typedef	minmax<tri3u32>		minmaxtri3u32;
	typedef	minmax<tri3u64>		minmaxtri3u64;
	typedef	minmax<tri3i8 >		minmaxtri3i8;
	typedef	minmax<tri3i16>		minmaxtri3i16;
	typedef	minmax<tri3i32>		minmaxtri3i32;
	typedef	minmax<tri3i64>		minmaxtri3i64;
#pragma pack(pop)

	tplt<tpnm T>
	tri3<T>&		translate			(::gpk::tri3<T> & triangle, const ::gpk::n3<T> & translation)	{
		triangle.A		+= translation;
		triangle.B		+= translation;
		triangle.C		+= translation;
		return triangle;
	}
	tplt<tpnm T>
	tri3<T>&		scale				(::gpk::tri3<T> & triangle, const ::gpk::n3<T> & scale)			{
		triangle.A.Scale(scale);
		triangle.B.Scale(scale);
		triangle.C.Scale(scale);
		return triangle;
	}

	tplt<tpnm T>
	::gpk::n3<T>	triangleWeight		(const ::gpk::tri<T> & weights, const ::gpk::tri3<T> & values)	{ return values.A * weights.A + values.B * weights.B + values.C * weights.C; }
} // namespace 

#endif // GPK_TRI3_H_23627