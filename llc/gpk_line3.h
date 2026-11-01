#include "gpk_line.h"
#include "gpk_n3.h"

#ifndef GPK_LINE3_H_23627
#define GPK_LINE3_H_23627

namespace gpk
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	tplt<tpnm _tDimension>	struct line3 : public line<::gpk::n3<_tDimension>>		{
		typedef	_tDimension		T;
		typedef	::gpk::n3<T>	TVertex;
		using line<TVertex>::	A;
		using line<TVertex>::	B;
		using line<TVertex>::	line;

		tplt<tpnm _tOther>
		line3<_tOther>			Cast				()		const	noexcept		{
			return
				{ A.tplt Cast<_tOther>()
				, B.tplt Cast<_tOther>()
				};
		}
	};
	typedef line3<float>	line3f2_t, line3f;
	typedef line3<double>	line3f64, line3d;
	typedef line3<u0_t>	line3u8;
	typedef line3<u1_t>	line3u1_t;
	typedef line3<u2_t>	line3u32;
	typedef line3<u3_t>	line3u64;
	typedef line3<s0_t>	line3s0_t;
	typedef line3<s1_t>	line3i16;
	typedef line3<s2_t>	line3i32;
	typedef line3<s3_t>	line3i64;

	tplt<tpnm T> stxp	T	orient2d3d	(const ::gpk::line3<T> & segment, const ::gpk::n2<T> & point)	noexcept	{ 
		return (segment.B.x - segment.A.x) * (point.y - segment.A.y) - (segment.B.y - segment.A.y) * (point.x - segment.A.x); 
	}
#pragma pack(pop)
} // namespace

#endif // GPK_LINE3_H_23627
