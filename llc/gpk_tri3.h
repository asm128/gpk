#include "gpk_tri.h"
#include "gpk_n3.h"

#ifndef GPK_TRI3_H_23627
#define GPK_TRI3_H_23627

namespace gpk 
{
#pragma pack(push, 1)
	template<typename _tDimension>	struct tri3 : public tri<::gpk::n3<_tDimension>>		{
		typedef	_tDimension		T;
		typedef	::gpk::n3<T>	TVertex;

		using	tri<TVertex>	::A;
		using	tri<TVertex>	::B;
		using	tri<TVertex>	::C;
		using	tri<TVertex>	::tri;

		inlcxpr	tri3<float>		f32			()		const	noexcept	{ return {A.f32(), B.f32(), C.f32()}; }
		inlcxpr	tri3<double>	f64			()		const	noexcept	{ return {A.f64(), B.f64(), C.f64()}; }
		inlcxpr	tri3<uint8_t>	i8			()		const	noexcept	{ return {A.i8 (), B.i8 (), C.i8 ()}; }
		inlcxpr	tri3<uint16_t>	i16			()		const	noexcept	{ return {A.i16(), B.i16(), C.i16()}; }
		inlcxpr	tri3<uint32_t>	i32			()		const	noexcept	{ return {A.i32(), B.i32(), C.i32()}; }
		inlcxpr	tri3<uint64_t>	i64			()		const	noexcept	{ return {A.i64(), B.i64(), C.i64()}; }
		inlcxpr	tri3<int8_t>	u8			()		const	noexcept	{ return {A.u8 (), B.u8 (), C.u8 ()}; }
		inlcxpr	tri3<int16_t>	u16			()		const	noexcept	{ return {A.u16(), B.u16(), C.u16()}; }
		inlcxpr	tri3<int32_t>	u32			()		const	noexcept	{ return {A.u32(), B.u32(), C.u32()}; }
		inlcxpr	tri3<int64_t>	u64			()		const	noexcept	{ return {A.u64(), B.u64(), C.u64()}; }

		template<typename _tOther>
		tri3<_tOther>			Cast		()		const	noexcept		{
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
	typedef	tri3<char>		tri3char;
	typedef	tri3<uchar_t>	tri3uchar;
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

	template<typename T>
	tri3<T>&		translate			(::gpk::tri3<T> & triangle, const ::gpk::n3<T> & translation)	{
		triangle.A		+= translation;
		triangle.B		+= translation;
		triangle.C		+= translation;
		return triangle;
	}
	template<typename T>
	tri3<T>&		scale				(::gpk::tri3<T> & triangle, const ::gpk::n3<T> & scale)			{
		triangle.A.Scale(scale);
		triangle.B.Scale(scale);
		triangle.C.Scale(scale);
		return triangle;
	}

	template<typename T>
	::gpk::n3<T>	triangleWeight		(const ::gpk::tri<T> & weights, const ::gpk::tri3<T> & values)	{ return values.A * weights.A + values.B * weights.B + values.C * weights.C; }
} // namespace 

#endif // GPK_TRI3_H_23627