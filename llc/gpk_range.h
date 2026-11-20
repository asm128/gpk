#include "gpk_typeint.h"

#ifndef GPK_RANGE_H
#define GPK_RANGE_H

nmsp gpk
{
#pragma pack(push, 1)
	tpl_t	stct range {	
		tydf		_t	T;
		T							Offset, Count;	
		GPK_DEFAULT_OPERATOR(range <T>, Offset == other.Offset && Count == other.Count); 
		inxp b8_t					empty		()	cnst nxpt { rtrn 0 == Count; }

		tpl_t2
		inxp	range<_t2>			Cast		()		csnx	{ return{(_t2)Offset, (_t2)Count}; } 
		inxp	range<uc_t>			uc			()		csnx	{ return Cast<uc_t>(); }
		inxp	range<sc_t>			sc			()		csnx	{ return Cast<sc_t>(); }
		inxp	range<u0_t>			u0			()		csnx	{ return Cast<u0_t>(); }
		inxp	range<u1_t>			u1			()		csnx	{ return Cast<u1_t>(); }
		inxp	range<u2_t>			u2			()		csnx	{ return Cast<u2_t>(); }
		inxp	range<u3_t>			u3			()		csnx	{ return Cast<u3_t>(); }
		inxp	range<s0_t>			s0			()		csnx	{ return Cast<s0_t>(); }
		inxp	range<s1_t>			s1			()		csnx	{ return Cast<s1_t>(); }
		inxp	range<s2_t>			s2			()		csnx	{ return Cast<s2_t>(); }
		inxp	range<s3_t>			s3			()		csnx	{ return Cast<s3_t>(); }
		inxp	range<f2_t>			f2			()		csnx	{ return Cast<f2_t>(); }
		inxp	range<f3_t>			f3			()		csnx	{ return Cast<f3_t>(); }
	};
#pragma pack(pop)

	tydf	range<uc_t>	rangeuc_t;	tydf	cnst rangeuc_t	rangeuc_c;
	tydf	range<sc_t>	rangesc_t;	tydf	cnst rangesc_t	rangesc_c;
	tydf	range<u0_t>	rangeu0_t;	tydf	cnst rangeu0_t	rangeu0_c;
	tydf	range<u1_t>	rangeu1_t;	tydf	cnst rangeu1_t	rangeu1_c;
	tydf	range<u2_t>	rangeu2_t;	tydf	cnst rangeu2_t	rangeu2_c;
	tydf	range<u3_t>	rangeu3_t;	tydf	cnst rangeu3_t	rangeu3_c;
	tydf	range<s0_t>	ranges0_t;	tydf	cnst ranges0_t	ranges0_c;
	tydf	range<s1_t>	ranges1_t;	tydf	cnst ranges1_t	ranges1_c;
	tydf	range<s2_t>	ranges2_t;	tydf	cnst ranges2_t	ranges2_c;
	tydf	range<s3_t>	ranges3_t;	tydf	cnst ranges3_t	ranges3_c;
	tydf	range<f2_t>	rangef2_t;	tydf	cnst rangef2_t	rangef2_c;
	tydf	range<f3_t>	rangef3_t;	tydf	cnst rangef3_t	rangef3_c;
}

#endif // GPK_RANGE_H
