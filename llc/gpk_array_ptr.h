#include "gpk_aobj_pobj.h"
#include "gpk_aobj_ppod.h"
#include "gpk_array_pod.h"

#ifndef GPK_ARRAY_PTR_H_23627
#define GPK_ARRAY_PTR_H_23627

namespace gpk
{
	tplt<tpnm T> usng		papod		= pobj<apod<T>>;
	tplt<tpnm T> usng		apapod		= aobj<papod<T>>;

	tydf	papod<uc_t>		pauc_t;
	tydf	papod<sc_t>		pasc_t;
	tydf	papod<u0_t>		pau0_t;
	tydf	papod<u1_t>		pau1_t;
	tydf	papod<u2_t>		pau2_t;
	tydf	papod<u3_t>		pau3_t;
	tydf	papod<s0_t>		pas0_t;
	tydf	papod<s1_t>		pas1_t;
	tydf	papod<s2_t>		pas2_t;
	tydf	papod<s3_t>		pas3_t;
	tydf	papod<f2_t>		paf2_t;
	tydf	papod<f3_t>		paf3_t;

	tydf	aobj<pauc_t>	apauc_t;
	tydf	aobj<pasc_t>	apasc_t;
	tydf	aobj<pau0_t>	apau0_t;
	tydf	aobj<pau1_t>	apau1_t;
	tydf	aobj<pau2_t>	apau2_t;
	tydf	aobj<pau3_t>	apau3_t;
	tydf	aobj<pas0_t>	apas0_t;
	tydf	aobj<pas1_t>	apas1_t;
	tydf	aobj<pas2_t>	apas2_t;
	tydf	aobj<pas3_t>	apas3_t;
	tydf	aobj<paf2_t>	apaf2_t;
	tydf	aobj<paf3_t>	apaf3_t;
} // namespace

#endif // GPK_ARRAY_PTR_H_23627


