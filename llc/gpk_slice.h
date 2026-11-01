#include "gpk_typeint.h"

#ifndef GPK_SLICE_H
#define GPK_SLICE_H

namespace gpk
{
#pragma pack(push, 1)
	tpl_t
	struct slice {
		tydf	_t						T;
		T								Begin, End;

		GPK_DEFAULT_OPERATOR(slice <T>, Begin  == other.Begin  && End   == other.End  );

		tpl_t2	inxp	slice<_t2>		Cast		()		csnx	{ return{(_t2)Begin , (_t2)End  }; }

				inxp	slice<uc_t>		uc			()		csnx	{ return Cast<uc_t>(); }
				inxp	slice<sc_t>		sc			()		csnx	{ return Cast<sc_t>(); }
				inxp	slice<u0_t>		u0			()		csnx	{ return Cast<u0_t>(); }
				inxp	slice<u1_t>		u1			()		csnx	{ return Cast<u1_t>(); }
				inxp	slice<u2_t>		u2			()		csnx	{ return Cast<u2_t>(); }
				inxp	slice<u3_t>		u3			()		csnx	{ return Cast<u3_t>(); }
				inxp	slice<s0_t>		s0			()		csnx	{ return Cast<s0_t>(); }
				inxp	slice<s1_t>		s1			()		csnx	{ return Cast<s1_t>(); }
				inxp	slice<s2_t>		s2			()		csnx	{ return Cast<s2_t>(); }
				inxp	slice<s3_t>		s3			()		csnx	{ return Cast<s3_t>(); }
				inxp	slice<f2_t>		f2			()		csnx	{ return Cast<f2_t>(); }
				inxp	slice<f3_t>		f3			()		csnx	{ return Cast<f3_t>(); }
	};
#pragma pack(pop)
	tydf	slice<uc_t>		sliceuc_t;	tdcs sliceuc_t	sliceuc_c;
	tydf	slice<sc_t>		slicesc_t;	tdcs slicesc_t	slicesc_c;
	tydf	slice<u0_t>		sliceu0_t;	tdcs sliceu0_t	sliceu0_c;
	tydf	slice<u1_t>		sliceu1_t;	tdcs sliceu1_t	sliceu1_c;
	tydf	slice<u2_t>		sliceu2_t;	tdcs sliceu2_t	sliceu2_c;
	tydf	slice<u3_t>		sliceu3_t;	tdcs sliceu3_t	sliceu3_c;
	tydf	slice<s0_t>		slices0_t;	tdcs slices0_t	slices0_c;
	tydf	slice<s1_t>		slices1_t;	tdcs slices1_t	slices1_c;
	tydf	slice<s2_t>		slices2_t;	tdcs slices2_t	slices2_c;
	tydf	slice<s3_t>		slices3_t;	tdcs slices3_t	slices3_c;
	tydf	slice<f2_t>		slicef2_t;	tdcs slicef2_t	slicef2_c;
	tydf	slice<f3_t>		slicef3_t;	tdcs slicef3_t	slicef3_c;
}

#endif // GPK_SLICE_H
