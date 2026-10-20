#include "gpk_line.h"
#include "gpk_n3.h"

#ifndef GPK_LINE3_H_23627
#define GPK_LINE3_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tpl_t struct line3 : public line<n3<_t>> {
		tydf	_t			T;
		tydf	n3<T>		TVertex;

		using	line<TVertex>	::A;
		using	line<TVertex>	::B;
		using	line<TVertex>	::line;

		tpl_t2
		inxp	line3<_t2>	Cast	()	csnx	{ return {A.tplt Cast<_t2>(), B.tplt Cast<_t2>()}; }
	};

	tydf line3<uc_t>	line3uc_t;	tdcs line3uc_t	line3uc_c;
	tydf line3<sc_t>	line3sc_t;	tdcs line3sc_t	line3sc_c;
	tydf line3<u0_t>	line3u0_t;	tdcs line3u0_t	line3u0_c;
	tydf line3<u1_t>	line3u1_t;	tdcs line3u1_t	line3u1_c;
	tydf line3<u2_t>	line3u2_t;	tdcs line3u2_t	line3u2_c;
	tydf line3<u3_t>	line3u3_t;	tdcs line3u3_t	line3u3_c;
	tydf line3<s0_t>	line3s0_t;	tdcs line3s0_t	line3s0_c;
	tydf line3<s1_t>	line3s1_t;	tdcs line3s1_t	line3s1_c;
	tydf line3<s2_t>	line3s2_t;	tdcs line3s2_t	line3s2_c;
	tydf line3<s3_t>	line3s3_t;	tdcs line3s3_t	line3s3_c;
	tydf line3<f2_t>	line3f2_t;	tdcs line3f2_t	line3f2_c;
	tydf line3<f3_t>	line3f3_t;	tdcs line3f3_t	line3f3_c;

	tplT stxp T	orient2d3d	(cnst line3<T> & segment, cnst n2<T> & point) nxpt {
		return (segment.B.x - segment.A.x) * (point.y - segment.A.y) - (segment.B.y - segment.A.y) * (point.x - segment.A.x);
	}
#pragma pack(pop)
}

#endif // GPK_LINE3_H_23627


