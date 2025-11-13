#include "gpk_array_pod.h"
#include "gpk_array_obj.h"

#ifndef GPK_ARRAY_H_23627
#define GPK_ARRAY_H_23627

namespace gpk
{
	tplT	using	aaobj		= ::gpk::aobj<::gpk::aobj<T>>; 
	tplT	using	aapod		= ::gpk::aobj<::gpk::apod<T>>; 

	tydf	::gpk::aapod<uc_t>	aauc_t, aauchar		;
	tydf	::gpk::aapod<sc_t>	aasc_t, aachar		;
	tydf	::gpk::aapod<u0_t>	aau0_t, aauint8		, aau8;
	tydf	::gpk::aapod<u1_t>	aau1_t, aauint16	, aau16;
	tydf	::gpk::aapod<u2_t>	aau2_t, aauint32	, aau32;
	tydf	::gpk::aapod<u3_t>	aau3_t, aauint64	, aau64;
	tydf	::gpk::aapod<s0_t>	aas0_t, aaint8		, aai8;
	tydf	::gpk::aapod<s1_t>	aas1_t, aaint16		, aai16;
	tydf	::gpk::aapod<s2_t>	aas2_t, aaint32		, aai32;
	tydf	::gpk::aapod<s3_t>	aas3_t, aaint64		, aai64;
	tydf	::gpk::aapod<f2_t>	aaf2_t, aafloat		, aaf32;
	tydf	::gpk::aapod<f3_t>	aaf3_t, aadouble	, aaf64;
} // namespace 

#endif // GPK_ARRAY_H_23627
