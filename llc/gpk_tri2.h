#include "gpk_tri.h"
#include "gpk_n2.h"

#ifndef GPK_TRI2_H_230523
#define GPK_TRI2_H_230523

namespace gpk 
{
#pragma pack(push, 1)
	template<typename _tDimension>	struct tri2 : public tri<::gpk::n2<_tDimension>>		{
		typedef	_tDimension	T;
		typedef				::gpk::n2<T>	TVertex;	
		using tri<TVertex>	::A;
		using tri<TVertex>	::B;
		using tri<TVertex>	::C;
		using tri<TVertex>	::tri;

		template<typename _tOther>
		tri2<_tOther>		Cast					()											const	noexcept		{
			return
				{ A.template Cast<_tOther>()
				, B.template Cast<_tOther>()
				, C.template Cast<_tOther>()
				};
		}
		bool				CulledX					(const ::gpk::SMinMax<T>& minMax)	const	noexcept		{
			return ((A.x  < minMax.Min) && (B.x  < minMax.Min) && (C.x  < minMax.Min))
				|| ((A.x >= minMax.Max) && (B.x >= minMax.Max) && (C.x >= minMax.Max))
				;
		}
		bool				CulledY					(const ::gpk::SMinMax<T>& minMax)	const	noexcept		{
			return ((A.y  < minMax.Min) && (B.y  < minMax.Min) && (C.y  < minMax.Min))
				|| ((A.y >= minMax.Max) && (B.y >= minMax.Max) && (C.y >= minMax.Max))
				;
		}
	};
	typedef tri2<float>		tri2f32, tri2f;
	typedef tri2<double>	tri2f64, tri2d;
	typedef tri2<uint8_t>	tri2u8;
	typedef tri2<uint16_t>	tri2u16;
	typedef tri2<uint32_t>	tri2u32;
	typedef tri2<uint64_t>	tri2u64;
	typedef tri2<int8_t>	tri2i8;
	typedef tri2<int16_t>	tri2i16;
	typedef tri2<int32_t>	tri2i32;
	typedef tri2<int64_t>	tri2i64;
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

#endif // GPK_TRI2_H_230523