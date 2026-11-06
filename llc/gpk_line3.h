#include "gpk_line.h"
#include "gpk_n3.h"

#ifndef GPK_LINE3_H_23627
#define GPK_LINE3_H_23627

namespace gpk
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	tplt<tpnm _tDimension>	struct line3 : public line<n3<_tDimension>>		{
		typedef	_tDimension		T;
		typedef	n3<T>	TVertex;
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
	tydf line3<f2_t> line3f2_t; tdcs line3f2_t line3f2_c;
	tydf line3<f3_t> line3f3_t; tdcs line3f3_t line3f3_c;
	tydf line3<u0_t> line3u0_t; tdcs line3u0_t line3u0_c;
	tydf line3<u1_t> line3u1_t; tdcs line3u1_t line3u1_c;
	tydf line3<u2_t> line3u2_t; tdcs line3u2_t line3u2_c;
	tydf line3<u3_t> line3u3_t; tdcs line3u3_t line3u3_c;
	tydf line3<s0_t> line3s0_t; tdcs line3s0_t line3s0_c;
	tydf line3<s1_t> line3s1_t; tdcs line3s1_t line3s1_c;
	tydf line3<s2_t> line3s2_t; tdcs line3s2_t line3s2_c;
	tydf line3<s3_t> line3s3_t; tdcs line3s3_t line3s3_c;

	tplt<tpnm T> stxp	T	orient2d3d	(const line3<T> & segment, const n2<T> & point)	noexcept	{ 
		return (segment.B.x - segment.A.x) * (point.y - segment.A.y) - (segment.B.y - segment.A.y) * (point.x - segment.A.x); 
	}
#pragma pack(pop)
} // namespace

#endif // GPK_LINE3_H_23627
