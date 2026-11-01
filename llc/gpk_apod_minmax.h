#include "gpk_array_pod.h"
#include "gpk_minmax.h"

#ifndef GPK_APOD_MINMAX_H
#define GPK_APOD_MINMAX_H

namespace gpk
{
	tplT using	aminmax	= ::gpk::apod<::gpk::minmax<T>>;

	tydf	::gpk::aminmax<sc_t>	aminmaxsc_t;	tdcs	aminmaxsc_t	aminmaxsc_c;
	tydf	::gpk::aminmax<uc_t>	aminmaxuc_t;	tdcs	aminmaxuc_t	aminmaxuc_c;
	tydf	::gpk::aminmax<u0_t>	aminmaxu0_t;	tdcs	aminmaxu0_t	aminmaxu0_c;
	tydf	::gpk::aminmax<u1_t>	aminmaxu1_t;	tdcs	aminmaxu1_t	aminmaxu1_c;
	tydf	::gpk::aminmax<u2_t>	aminmaxu2_t;	tdcs	aminmaxu2_t	aminmaxu2_c;
	tydf	::gpk::aminmax<u3_t>	aminmaxu3_t;	tdcs	aminmaxu3_t	aminmaxu3_c;
	tydf	::gpk::aminmax<s0_t>	aminmaxs0_t;	tdcs	aminmaxs0_t	aminmaxs0_c;
	tydf	::gpk::aminmax<s1_t>	aminmaxs1_t;	tdcs	aminmaxs1_t	aminmaxs1_c;
	tydf	::gpk::aminmax<s2_t>	aminmaxs2_t;	tdcs	aminmaxs2_t	aminmaxs2_c;
	tydf	::gpk::aminmax<s3_t>	aminmaxs3_t;	tdcs	aminmaxs3_t	aminmaxs3_c;
	tydf	::gpk::aminmax<f2_t>	aminmaxf2_t;	tdcs	aminmaxf2_t	aminmaxf2_c;
	tydf	::gpk::aminmax<f3_t>	aminmaxf3_t;	tdcs	aminmaxf3_t	aminmaxf3_c;
} // namespace gpk

#endif // GPK_APOD_MINMAX_H
