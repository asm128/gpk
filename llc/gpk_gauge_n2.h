/// Copyright 2009-2023 - asm128
#include "gpk_n2.h"
#include "gpk_gauge.h"

#ifndef GPK_GAUGE_N2
#define GPK_GAUGE_N2

nmsp gpk
{
#pragma pack(push, 1)
	tydf gaugemax<n2sc_t> gauge2sc_t; tdcs gauge2sc_t gauge2sc_c;
	tydf gaugemax<n2uc_t> gauge2uc_t; tdcs gauge2uc_t gauge2uc_c;
	tydf gaugemax<n2f2_t> gauge2f2_t; tdcs gauge2f2_t gauge2f2_c;
	tydf gaugemax<n2f3_t> gauge2f3_t; tdcs gauge2f3_t gauge2f3_c;
	tydf gaugemax<n2u0_t> gauge2u0_t; tdcs gauge2u0_t gauge2u0_c;
	tydf gaugemax<n2u1_t> gauge2u1_t; tdcs gauge2u1_t gauge2u1_c;
	tydf gaugemax<n2u2_t> gauge2u2_t; tdcs gauge2u2_t gauge2u2_c;
	tydf gaugemax<n2u3_t> gauge2u3_t; tdcs gauge2u3_t gauge2u3_c;
	tydf gaugemax<n2s0_t> gauge2s0_t; tdcs gauge2s0_t gauge2s0_c;
	tydf gaugemax<n2s1_t> gauge2s1_t; tdcs gauge2s1_t gauge2s1_c;
	tydf gaugemax<n2s2_t> gauge2s2_t; tdcs gauge2s2_t gauge2s2_c;
	tydf gaugemax<n2s3_t> gauge2s3_t; tdcs gauge2s3_t gauge2s3_c;
#pragma pack(pop)
} // nmsp

#endif // GPK_GAUGE_N2
