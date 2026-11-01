#include "gpk_array_pod.h"
#include "gpk_slice.h"

#ifndef GPK_APOD_SLICE_H
#define GPK_APOD_SLICE_H

namespace gpk
{
	tplT using	aslice	= ::gpk::apod<::gpk::slice<T>>;

	tydf	::gpk::aslice<sc_t>	aslicesc_t;	tdcs	aslicesc_t	aslicesc_c;
	tydf	::gpk::aslice<uc_t>	asliceuc_t;	tdcs	asliceuc_t	asliceuc_c;
	tydf	::gpk::aslice<u0_t>	asliceu0_t;	tdcs	asliceu0_t	asliceu0_c;
	tydf	::gpk::aslice<u1_t>	asliceu1_t;	tdcs	asliceu1_t	asliceu1_c;
	tydf	::gpk::aslice<u2_t>	asliceu2_t;	tdcs	asliceu2_t	asliceu2_c;
	tydf	::gpk::aslice<u3_t>	asliceu3_t;	tdcs	asliceu3_t	asliceu3_c;
	tydf	::gpk::aslice<s0_t>	aslices0_t;	tdcs	aslices0_t	aslices0_c;
	tydf	::gpk::aslice<s1_t>	aslices1_t;	tdcs	aslices1_t	aslices1_c;
	tydf	::gpk::aslice<s2_t>	aslices2_t;	tdcs	aslices2_t	aslices2_c;
	tydf	::gpk::aslice<s3_t>	aslices3_t;	tdcs	aslices3_t	aslices3_c;
	tydf	::gpk::aslice<f2_t>	aslicef2_t;	tdcs	aslicef2_t	aslicef2_c;
	tydf	::gpk::aslice<f3_t>	aslicef3_t;	tdcs	aslicef3_t	aslicef3_c;
}

#endif // GPK_APOD_SLICE_H