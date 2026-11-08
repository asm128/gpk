/// Copyright 2009-2023 - asm128
#include "gpk_n3.h"
#include "gpk_gauge.h"

#ifndef GPK_GAUGE_N3
#define GPK_GAUGE_N3

nmsp gpk
{
#pragma pack(push, 1)
	tydf gaugemax<n3sc_t> gauge3sc_t; tdcs gauge3sc_t gauge3sc_c;
	tydf gaugemax<n3uc_t> gauge3uc_t; tdcs gauge3uc_t gauge3uc_c;
	tydf gaugemax<n3f2_t> gauge3f2_t; tdcs gauge3f2_t gauge3f2_c;
	tydf gaugemax<n3f3_t> gauge3f3_t; tdcs gauge3f3_t gauge3f3_c;
	tydf gaugemax<n3u0_t> gauge3u0_t; tdcs gauge3u0_t gauge3u0_c;
	tydf gaugemax<n3u1_t> gauge3u1_t; tdcs gauge3u1_t gauge3u1_c;
	tydf gaugemax<n3u2_t> gauge3u2_t; tdcs gauge3u2_t gauge3u2_c;
	tydf gaugemax<n3u3_t> gauge3u3_t; tdcs gauge3u3_t gauge3u3_c;
	tydf gaugemax<n3s0_t> gauge3s0_t; tdcs gauge3s0_t gauge3s0_c;
	tydf gaugemax<n3s1_t> gauge3s1_t; tdcs gauge3s1_t gauge3s1_c;
	tydf gaugemax<n3s2_t> gauge3s2_t; tdcs gauge3s2_t gauge3s2_c;
	tydf gaugemax<n3s3_t> gauge3s3_t; tdcs gauge3s3_t gauge3s3_c;
#pragma pack(pop)
} // nmsp

#endif // GPK_GAUGE_N3
