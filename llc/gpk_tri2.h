#include "gpk_tri.h"
#include "gpk_n2.h"
#include "gpk_minmax.h"

#ifndef GPK_TRI2_H_23627
#define GPK_TRI2_H_23627

namespace gpk 
{
#pragma pack(push, 1)
	tpl_t struct tri2 : public tri<::gpk::n2<_t>>		{
		tydf	_t				T;
		tydf	::gpk::n2<T>	TVertex;

		using	tri<TVertex>	::A;
		using	tri<TVertex>	::B;
		using	tri<TVertex>	::C;
		using	tri<TVertex>	::tri;

		inxp	tri2<uc_t>		uc		()	csnx	{ return {A.uc(), B.uc(), C.uc()}; }
		inxp	tri2<sc_t>		sc		()	csnx	{ return {A.sc(), B.sc(), C.sc()}; }
		inxp	tri2<u0_t>		u0		()	csnx	{ return {A.u0(), B.u0(), C.u0()}; }
		inxp	tri2<u1_t>		u1		()	csnx	{ return {A.u1(), B.u1(), C.u1()}; }
		inxp	tri2<u2_t>		u2		()	csnx	{ return {A.u2(), B.u2(), C.u2()}; }
		inxp	tri2<u3_t>		u3		()	csnx	{ return {A.u3(), B.u3(), C.u3()}; }
		inxp	tri2<s0_t>		s0		()	csnx	{ return {A.s0(), B.s0(), C.s0()}; }
		inxp	tri2<s1_t>		s1		()	csnx	{ return {A.s1(), B.s1(), C.s1()}; }
		inxp	tri2<s2_t>		s2		()	csnx	{ return {A.s2(), B.s2(), C.s2()}; }
		inxp	tri2<s3_t>		s3		()	csnx	{ return {A.s3(), B.s3(), C.s3()}; }
		inxp	tri2<f2_t>		f2		()	csnx	{ return {A.f2(), B.f2(), C.f2()}; }
		inxp	tri2<f3_t>		f3		()	csnx	{ return {A.f3(), B.f3(), C.f3()}; }

		tpl_t2	tri2<_t2>		Cast	()	csnx	{
			return
				{ A.tplt Cast<_t2>()
				, B.tplt Cast<_t2>()
				, C.tplt Cast<_t2>()
				};
		}
		cxpr	bool			CulledX		(cnst ::gpk::minmax<T>& minMax)	csnx	{
			return ((A.x  < minMax.Min) && (B.x  < minMax.Min) && (C.x  < minMax.Min))
				|| ((A.x >= minMax.Max) && (B.x >= minMax.Max) && (C.x >= minMax.Max))
				;
		}
		cxpr	bool			CulledY		(cnst ::gpk::minmax<T>& minMax)	csnx	{
			return ((A.y  < minMax.Min) && (B.y  < minMax.Min) && (C.y  < minMax.Min))
				|| ((A.y >= minMax.Max) && (B.y >= minMax.Max) && (C.y >= minMax.Max))
				;
		}
	};
	tydf	tri2<uc_t>	tri2uc_t;	tdcs tri2uc_t	tri2uc_c;
	tydf	tri2<sc_t>	tri2sc_t;	tdcs tri2sc_t	tri2sc_c;
	tydf	tri2<u0_t>	tri2u0_t;	tdcs tri2u0_t	tri2u0_c;
	tydf	tri2<u1_t>	tri2u1_t;	tdcs tri2u1_t	tri2u1_c;
	tydf	tri2<u2_t>	tri2u2_t;	tdcs tri2u2_t	tri2u2_c;
	tydf	tri2<u3_t>	tri2u3_t;	tdcs tri2u3_t	tri2u3_c;
	tydf	tri2<s0_t>	tri2s0_t;	tdcs tri2s0_t	tri2s0_c;
	tydf	tri2<s1_t>	tri2s1_t;	tdcs tri2s1_t	tri2s1_c;
	tydf	tri2<s2_t>	tri2s2_t;	tdcs tri2s2_t	tri2s2_c;
	tydf	tri2<s3_t>	tri2s3_t;	tdcs tri2s3_t	tri2s3_c;
	tydf	tri2<f2_t>	tri2f2_t;	tdcs tri2f2_t	tri2f2_c;
	tydf	tri2<f3_t>	tri2f3_t;	tdcs tri2f3_t	tri2f3_c;

	tydf	minmax<tri2uc_t>	minmaxtri2uc_t;	tdcs minmaxtri2uc_t	minmaxtri2uc_c;
	tydf	minmax<tri2sc_t>	minmaxtri2sc_t;	tdcs minmaxtri2sc_t	minmaxtri2sc_c;
	tydf	minmax<tri2u0_t>	minmaxtri2u0_t;	tdcs minmaxtri2u0_t	minmaxtri2u0_c;
	tydf	minmax<tri2u1_t>	minmaxtri2u1_t;	tdcs minmaxtri2u1_t	minmaxtri2u1_c;
	tydf	minmax<tri2u2_t>	minmaxtri2u2_t;	tdcs minmaxtri2u2_t	minmaxtri2u2_c;
	tydf	minmax<tri2u3_t>	minmaxtri2u3_t;	tdcs minmaxtri2u3_t	minmaxtri2u3_c;
	tydf	minmax<tri2s0_t>	minmaxtri2s0_t;	tdcs minmaxtri2s0_t	minmaxtri2s0_c;
	tydf	minmax<tri2s1_t>	minmaxtri2s1_t;	tdcs minmaxtri2s1_t	minmaxtri2s1_c;
	tydf	minmax<tri2s2_t>	minmaxtri2s2_t;	tdcs minmaxtri2s2_t	minmaxtri2s2_c;
	tydf	minmax<tri2s3_t>	minmaxtri2s3_t;	tdcs minmaxtri2s3_t	minmaxtri2s3_c;
	tydf	minmax<tri2f2_t>	minmaxtri2f2_t;	tdcs minmaxtri2f2_t	minmaxtri2f2_c;
	tydf	minmax<tri2f3_t>	minmaxtri2f3_t;	tdcs minmaxtri2f3_t	minmaxtri2f3_c;
#pragma pack(pop)

	tplT	::gpk::n2<T>	triangleWeight	(cnst ::gpk::tri<T> & weights, cnst ::gpk::tri2<T> & values)	nxpt	{ return values.A * weights.A + values.B * weights.B + values.C * weights.C; }
	tplT	::gpk::tri2<T>&	translate		(::gpk::tri2<T> & triangle, cnst ::gpk::n2<T> & translation)	nxpt	{
		triangle.A			+= translation;
		triangle.B			+= translation;
		triangle.C			+= translation;
		return triangle;
	}
} // namespace 

#endif // GPK_TRI2_H_23627