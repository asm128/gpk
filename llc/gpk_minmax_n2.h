/// Copyright 2009-2024 - ogarnd
#include "gpk_n2.h"
#include "gpk_minmax.h"

#ifndef GPK_MINMAX_N2
#define GPK_MINMAX_N2

namespace gpk
{
#pragma pack(push, 1)
	tplTusng	minmax2 = minmax<n2<T>>;	
	tydf	minmax2<uc_t>	minmax2uc_t;	tdcs	minmax2uc_t	minmax2uc_c;
	tydf	minmax2<sc_t>	minmax2sc_t;	tdcs	minmax2sc_t	minmax2sc_c;
	tydf	minmax2<u0_t>	minmax2u0_t;	tdcs	minmax2u0_t	minmax2u0_c;
	tydf	minmax2<u1_t>	minmax2u1_t;	tdcs	minmax2u1_t	minmax2u1_c;
	tydf	minmax2<u2_t>	minmax2u2_t;	tdcs	minmax2u2_t	minmax2u2_c;
	tydf	minmax2<u3_t>	minmax2u3_t;	tdcs	minmax2u3_t	minmax2u3_c;
	tydf	minmax2<s0_t>	minmax2s0_t;	tdcs	minmax2s0_t	minmax2s0_c;
	tydf	minmax2<s1_t>	minmax2s1_t;	tdcs	minmax2s1_t	minmax2s1_c;
	tydf	minmax2<s2_t>	minmax2s2_t;	tdcs	minmax2s2_t	minmax2s2_c;
	tydf	minmax2<s3_t>	minmax2s3_t;	tdcs	minmax2s3_t	minmax2s3_c;
	tydf	minmax2<f2_t>	minmax2f2_t;	tdcs	minmax2f2_t	minmax2f2_c;
	tydf	minmax2<f3_t>	minmax2f3_t;	tdcs	minmax2f3_t	minmax2f3_c;
#pragma pack(pop)
} // namespace

#endif // GPK_MINMAX_N2
