#include "gpk_tri.h"
#include "gpk_n3.h"
#include "gpk_minmax.h"

#ifndef GPK_TRI3_H_23627
#define GPK_TRI3_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tpl_t struct tri3 : public tri<n3<_t>> {
		tydf	_t			T;
		tydf	n3<T>		TVertex;

		using	tri<TVertex>	::A;
		using	tri<TVertex>	::B;
		using	tri<TVertex>	::C;
		using	tri<TVertex>	::tri;

		inxp	tri3<uc_t>		uc		()	csnx	{ return {A.uc(), B.uc(), C.uc()}; }
		inxp	tri3<sc_t>		sc		()	csnx	{ return {A.sc(), B.sc(), C.sc()}; }
		inxp	tri3<u0_t>		u0		()	csnx	{ return {A.u0(), B.u0(), C.u0()}; }
		inxp	tri3<u1_t>		u1		()	csnx	{ return {A.u1(), B.u1(), C.u1()}; }
		inxp	tri3<u2_t>		u2		()	csnx	{ return {A.u2(), B.u2(), C.u2()}; }
		inxp	tri3<u3_t>		u3		()	csnx	{ return {A.u3(), B.u3(), C.u3()}; }
		inxp	tri3<s0_t>		s0		()	csnx	{ return {A.s0(), B.s0(), C.s0()}; }
		inxp	tri3<s1_t>		s1		()	csnx	{ return {A.s1(), B.s1(), C.s1()}; }
		inxp	tri3<s2_t>		s2		()	csnx	{ return {A.s2(), B.s2(), C.s2()}; }
		inxp	tri3<s3_t>		s3		()	csnx	{ return {A.s3(), B.s3(), C.s3()}; }
		inxp	tri3<f2_t>		f2		()	csnx	{ return {A.f2(), B.f2(), C.f2()}; }
		inxp	tri3<f3_t>		f3		()	csnx	{ return {A.f3(), B.f3(), C.f3()}; }

		tpl_t2 tri3<_t2>	Cast	()	csnx	{
			return
				{ A.tplt Cast<_t2>()
				, B.tplt Cast<_t2>()
				, C.tplt Cast<_t2>()
				};
		}
		cxpr	bool			CulledX			(cnst minmax<T> & limits)	csnx	{
			return ((A.x  < limits.Min) && (B.x  < limits.Min) && (C.x  < limits.Min))
				|| ((A.x >= limits.Max) && (B.x >= limits.Max) && (C.x >= limits.Max))
				;
		}
		cxpr	bool			CulledY			(cnst minmax<T> & limits)	csnx	{
			return ((A.y  < limits.Min) && (B.y  < limits.Min) && (C.y  < limits.Min))
				|| ((A.y >= limits.Max) && (B.y >= limits.Max) && (C.y >= limits.Max))
				;
		}
		cxpr	bool			CulledZ			(cnst minmax<T> & limits)	csnx	{
			return ((A.z  < limits.Min) && (B.z  < limits.Min) && (C.z  < limits.Min))
				|| ((A.z >= limits.Max) && (B.z >= limits.Max) && (C.z >= limits.Max))
				;
		}
		cxpr	bool			CulledZSpecial	(cnst minmax<T> & limits)	csnx	{
			return ((A.z <= limits.Min) || (B.z <= limits.Min) || (C.z <= limits.Min))
				|| ((A.z >= limits.Max) && (B.z >= limits.Max) && (C.z >= limits.Max))
				;
		}
		cxpr	bool			ClipZ			()								csnx	{
			return A.z < 0 || A.z >= 1 || B.z < 0 || B.z >= 1 || C.z < 0 || C.z >= 1;
		}
		tri3<T> &			Scale			(cnst TVertex & scale)			nxpt	{
			A.Scale(scale);
			B.Scale(scale);
			C.Scale(scale);
			return *this;
		}
		tri3<T> &			Translate		(cnst TVertex & translation)	nxpt	{
			A += translation;
			B += translation;
			C += translation;
			return *this;
		}
	};

	tydf	tri3<uc_t>	tri3uc_t;	tdcs tri3uc_t	tri3uc_c;
	tydf	tri3<sc_t>	tri3sc_t;	tdcs tri3sc_t	tri3sc_c;
	tydf	tri3<u0_t>	tri3u0_t;	tdcs tri3u0_t	tri3u0_c;
	tydf	tri3<u1_t>	tri3u1_t;	tdcs tri3u1_t	tri3u1_c;
	tydf	tri3<u2_t>	tri3u2_t;	tdcs tri3u2_t	tri3u2_c;
	tydf	tri3<u3_t>	tri3u3_t;	tdcs tri3u3_t	tri3u3_c;
	tydf	tri3<s0_t>	tri3s0_t;	tdcs tri3s0_t	tri3s0_c;
	tydf	tri3<s1_t>	tri3s1_t;	tdcs tri3s1_t	tri3s1_c;
	tydf	tri3<s2_t>	tri3s2_t;	tdcs tri3s2_t	tri3s2_c;
	tydf	tri3<s3_t>	tri3s3_t;	tdcs tri3s3_t	tri3s3_c;
	tydf	tri3<f2_t>	tri3f2_t;	tdcs tri3f2_t	tri3f2_c;
	tydf	tri3<f3_t>	tri3f3_t;	tdcs tri3f3_t	tri3f3_c;

	tydf	minmax<tri3uc_t>	minmaxtri3uc_t;	tdcs minmaxtri3uc_t	minmaxtri3uc_c;
	tydf	minmax<tri3sc_t>	minmaxtri3sc_t;	tdcs minmaxtri3sc_t	minmaxtri3sc_c;
	tydf	minmax<tri3u0_t>	minmaxtri3u0_t;	tdcs minmaxtri3u0_t	minmaxtri3u0_c;
	tydf	minmax<tri3u1_t>	minmaxtri3u1_t;	tdcs minmaxtri3u1_t	minmaxtri3u1_c;
	tydf	minmax<tri3u2_t>	minmaxtri3u2_t;	tdcs minmaxtri3u2_t	minmaxtri3u2_c;
	tydf	minmax<tri3u3_t>	minmaxtri3u3_t;	tdcs minmaxtri3u3_t	minmaxtri3u3_c;
	tydf	minmax<tri3s0_t>	minmaxtri3s0_t;	tdcs minmaxtri3s0_t	minmaxtri3s0_c;
	tydf	minmax<tri3s1_t>	minmaxtri3s1_t;	tdcs minmaxtri3s1_t	minmaxtri3s1_c;
	tydf	minmax<tri3s2_t>	minmaxtri3s2_t;	tdcs minmaxtri3s2_t	minmaxtri3s2_c;
	tydf	minmax<tri3s3_t>	minmaxtri3s3_t;	tdcs minmaxtri3s3_t	minmaxtri3s3_c;
	tydf	minmax<tri3f2_t>	minmaxtri3f2_t;	tdcs minmaxtri3f2_t	minmaxtri3f2_c;
	tydf	minmax<tri3f3_t>	minmaxtri3f3_t;	tdcs minmaxtri3f3_t	minmaxtri3f3_c;
#pragma pack(pop)

	tplT tri3<T> &	translate		(tri3<T> & triangle, cnst n3<T> & translation)	nxpt	{ return triangle.Translate(translation); }
	tplT tri3<T> &	scale			(tri3<T> & triangle, cnst n3<T> & scaling)		nxpt	{ return triangle.Scale(scaling); }
	tplT n3<T>		triangleWeight	(cnst tri<T> & weights, cnst tri3<T> & values)	nxpt	{ return values.A * weights.A + values.B * weights.B + values.C * weights.C; }
}

#endif // GPK_TRI3_H_23627
