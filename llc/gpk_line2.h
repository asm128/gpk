#include "gpk_line.h"
#include "gpk_n2.h"

#ifndef GPK_LINE2_H_23627
#define GPK_LINE2_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tpl_t struct line2 : public line<n2<_t>> {
		tydf	_t			T;
		tydf	n2<T>		TVertex;

		using	line<TVertex>	::A;
		using	line<TVertex>	::B;
		using	line<TVertex>	::line;

		tpl_t2
		inxp	line2<_t2>	Cast	()	csnx	{ return {A.tplt Cast<_t2>(), B.tplt Cast<_t2>()}; }
	};

	tydf line2<uc_t>	line2uc_t;	tdcs line2uc_t	line2uc_c;
	tydf line2<sc_t>	line2sc_t;	tdcs line2sc_t	line2sc_c;
	tydf line2<u0_t>	line2u0_t;	tdcs line2u0_t	line2u0_c;
	tydf line2<u1_t>	line2u1_t;	tdcs line2u1_t	line2u1_c;
	tydf line2<u2_t>	line2u2_t;	tdcs line2u2_t	line2u2_c;
	tydf line2<u3_t>	line2u3_t;	tdcs line2u3_t	line2u3_c;
	tydf line2<s0_t>	line2s0_t;	tdcs line2s0_t	line2s0_c;
	tydf line2<s1_t>	line2s1_t;	tdcs line2s1_t	line2s1_c;
	tydf line2<s2_t>	line2s2_t;	tdcs line2s2_t	line2s2_c;
	tydf line2<s3_t>	line2s3_t;	tdcs line2s3_t	line2s3_c;
	tydf line2<f2_t>	line2f2_t;	tdcs line2f2_t	line2f2_c;
	tydf line2<f3_t>	line2f3_t;	tdcs line2f3_t	line2f3_c;

	tplT stxp T		rise		(cnst line2<T> & segment)					nxpt	{ return segment.B.y - segment.A.y; }
	tplT stxp T		run			(cnst line2<T> & segment)					nxpt	{ return segment.B.x - segment.A.x; }
	tplT stxp T		slope		(cnst line2<T> & segment)							{ return rise(segment) / run(segment); }
	tplT stxp T		orient2d	(cnst line2<T> & segment, cnst n2<T> & point)	nxpt	{ return run(segment) * (point.y - segment.A.y) - rise(segment) * (point.x - segment.A.x); }
	tplT stin f3_t	determinant	(cnst line2<T> & segment)					nxpt	{ return determinant((f3_t)segment.A.x, (f3_t)segment.A.y, (f3_t)segment.B.x, (f3_t)segment.B.y); }
#pragma pack(pop)
}

#endif // GPK_LINE2_H_23627


