#include "gpk_n3.h"

#ifndef GPK_RECT3_H_23627
#define GPK_RECT3_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplT struct rect3 {
		n3<T>	Offset, Size;

		GPK_DEFAULT_OPERATOR(rect3<T>, Offset == other.Offset && Size == other.Size);

		tpl_t2
		inxp	rect3<_t2>	Cast	()	csnx	{ return {Offset.tplt Cast<_t2>(), Size.tplt Cast<_t2>()}; }

		inxp	rect3<uc_t>	uc		()	csnx	{ return Cast<uc_t>(); }
		inxp	rect3<sc_t>	sc		()	csnx	{ return Cast<sc_t>(); }
		inxp	rect3<u0_t>	u0		()	csnx	{ return Cast<u0_t>(); }
		inxp	rect3<u1_t>	u1		()	csnx	{ return Cast<u1_t>(); }
		inxp	rect3<u2_t>	u2		()	csnx	{ return Cast<u2_t>(); }
		inxp	rect3<u3_t>	u3		()	csnx	{ return Cast<u3_t>(); }
		inxp	rect3<s0_t>	s0		()	csnx	{ return Cast<s0_t>(); }
		inxp	rect3<s1_t>	s1		()	csnx	{ return Cast<s1_t>(); }
		inxp	rect3<s2_t>	s2		()	csnx	{ return Cast<s2_t>(); }
		inxp	rect3<s3_t>	s3		()	csnx	{ return Cast<s3_t>(); }
		inxp	rect3<f2_t>	f2		()	csnx	{ return Cast<f2_t>(); }
		inxp	rect3<f3_t>	f3		()	csnx	{ return Cast<f3_t>(); }

		inxp	n3<T>	Limit	()	csnx	{ return Offset + Size; }
	};
#pragma pack(pop)

	tydf rect3<uc_t>	rect3uc_t;	tdcs rect3uc_t	rect3uc_c;
	tydf rect3<sc_t>	rect3sc_t;	tdcs rect3sc_t	rect3sc_c;
	tydf rect3<u0_t>	rect3u0_t;	tdcs rect3u0_t	rect3u0_c;
	tydf rect3<u1_t>	rect3u1_t;	tdcs rect3u1_t	rect3u1_c;
	tydf rect3<u2_t>	rect3u2_t;	tdcs rect3u2_t	rect3u2_c;
	tydf rect3<u3_t>	rect3u3_t;	tdcs rect3u3_t	rect3u3_c;
	tydf rect3<s0_t>	rect3s0_t;	tdcs rect3s0_t	rect3s0_c;
	tydf rect3<s1_t>	rect3s1_t;	tdcs rect3s1_t	rect3s1_c;
	tydf rect3<s2_t>	rect3s2_t;	tdcs rect3s2_t	rect3s2_c;
	tydf rect3<s3_t>	rect3s3_t;	tdcs rect3s3_t	rect3s3_c;
	tydf rect3<f2_t>	rect3f2_t;	tdcs rect3f2_t	rect3f2_c;
	tydf rect3<f3_t>	rect3f3_t;	tdcs rect3f3_t	rect3f3_c;
}

#endif // GPK_RECT3_H_23627
