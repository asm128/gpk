#include "gpk_array_pod.h"
#include "gpk_gauge.h"

#ifndef GPK_APOD_GAUGE_H
#define GPK_APOD_GAUGE_H

nmsp gpk
{
	tplt<tpnm _tDimension>	
	usng agauge = apod<gaugemax<_tDimension>>;

	tydf agauge<sc_t> agaugesc_t; tdcs agaugesc_t agaugesc_c;
	tydf agauge<uc_t> agaugeuc_t; tdcs agaugeuc_t agaugeuc_c;
	tydf agauge<f2_t> agaugef2_t; tdcs agaugef2_t agaugef2_c;
	tydf agauge<f3_t> agaugef3_t; tdcs agaugef3_t agaugef3_c;
	tydf agauge<u0_t> agaugeu0_t; tdcs agaugeu0_t agaugeu0_c;
	tydf agauge<u1_t> agaugeu1_t; tdcs agaugeu1_t agaugeu1_c;
	tydf agauge<u2_t> agaugeu2_t; tdcs agaugeu2_t agaugeu2_c;
	tydf agauge<u3_t> agaugeu3_t; tdcs agaugeu3_t agaugeu3_c;
	tydf agauge<s0_t> agauges0_t; tdcs agauges0_t agauges0_c;
	tydf agauge<s1_t> agauges1_t; tdcs agauges1_t agauges1_c;
	tydf agauge<s2_t> agauges2_t; tdcs agauges2_t agauges2_c;
	tydf agauge<s3_t> agauges3_t; tdcs agauges3_t agauges3_c;
}

#endif // GPK_APOD_GAUGE_H