#include "gpk_tri.h"
#include "gpk_n3.h"

#ifndef GPK_TRI3_H_230523
#define GPK_TRI3_H_230523

namespace gpk 
{
#pragma pack(push, 1)
	template<typename _tDimension>	struct tri3 : public tri<::gpk::n3<_tDimension>>		{
		typedef	::gpk::n3<_tDimension>	TVertex;
		using tri<TVertex>	::A;
		using tri<TVertex>	::B;
		using tri<TVertex>	::C;
		using tri<TVertex>	::tri;

		template<typename _tOther>
		tri3<_tOther>		Cast					()		const	noexcept		{
			return
				{ A.template Cast<_tOther>()
				, B.template Cast<_tOther>()
				, C.template Cast<_tOther>()
				};
		}
		bool				CulledX					(const ::gpk::SMinMax<_tDimension>& minMax)	const	noexcept		{
			return ((A.x  < minMax.Min) && (B.x  < minMax.Min) && (C.x  < minMax.Min))
				|| ((A.x >= minMax.Max) && (B.x >= minMax.Max) && (C.x >= minMax.Max))
				;
		}
		bool				CulledY					(const ::gpk::SMinMax<_tDimension>& minMax)	const	noexcept		{
			return ((A.y  < minMax.Min) && (B.y  < minMax.Min) && (C.y  < minMax.Min))
				|| ((A.y >= minMax.Max) && (B.y >= minMax.Max) && (C.y >= minMax.Max))
				;
		}
		bool				CulledZ					(const ::gpk::SMinMax<_tDimension>& minMax)	const	noexcept		{
			return ((A.z  < minMax.Min) && (B.z  < minMax.Min) && (C.z  < minMax.Min))
				|| ((A.z >= minMax.Max) && (B.z >= minMax.Max) && (C.z >= minMax.Max))
				;
		}
		bool				CulledZSpecial			(const ::gpk::SMinMax<_tDimension>& minMax)	const	noexcept		{
			return ((A.z <= minMax.Min) || (B.z <= minMax.Min) || (C.z <= minMax.Min))
				|| ((A.z >= minMax.Max) && (B.z >= minMax.Max) && (C.z >= minMax.Max))
				;
		}
		bool				ClipZ					()											const	noexcept		{
			if(A.z < 0 || A.z >= 1) return true;
			if(B.z < 0 || B.z >= 1) return true;
			if(C.z < 0 || C.z >= 1) return true;
			return false;
		}
		tri3<_tDimension>&	Scale					(const ::gpk::n3<_tDimension> & scale)				noexcept		{
			A.Scale(scale);
			B.Scale(scale);
			C.Scale(scale);
			return *this;
		}
		tri3<_tDimension>&	Translate				(const ::gpk::n3<_tDimension> & translation)		noexcept		{
			A					+= translation;
			B					+= translation;
			C					+= translation;
			return *this;
		}
	};
	typedef tri3<float>		tri3f32, tri3f;
	typedef tri3<double>	tri3f64, tri3d;
	typedef tri3<uint8_t>	tri3u8;
	typedef tri3<uint16_t>	tri3u16;
	typedef tri3<uint32_t>	tri3u32;
	typedef tri3<uint64_t>	tri3u64;
	typedef tri3<int8_t>	tri3i8;
	typedef tri3<int16_t>	tri3i16;
	typedef tri3<int32_t>	tri3i32;
	typedef tri3<int64_t>	tri3i64;
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

#endif // GPK_TRI3_H_230523