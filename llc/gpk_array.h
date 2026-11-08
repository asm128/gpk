#include "gpk_array_obj.h"
#include "gpk_array_static.h"
#include "gpk_string.h"

#ifndef GPK_ARRAY_H_23627
#define GPK_ARRAY_H_23627

nmsp gpk
{
	tplT	usng	aaobj		= aobj<aobj<T>>; 
	tplT	usng	aapod		= aobj<apod<T>>;

	tydf	aapod<uc_t>		aauc_t;	tdcs aauc_t aauc_c;
	tydf	aapod<sc_t>		aasc_t;	tdcs aasc_t aasc_c;
	tydf	aapod<u0_t>		aau0_t;	tdcs aau0_t aau0_c;
	tydf	aapod<u1_t>		aau1_t;	tdcs aau1_t aau1_c;
	tydf	aapod<u2_t>		aau2_t;	tdcs aau2_t aau2_c;
	tydf	aapod<u3_t>		aau3_t;	tdcs aau3_t aau3_c;
	tydf	aapod<s0_t>		aas0_t;	tdcs aas0_t aas0_c;
	tydf	aapod<s1_t>		aas1_t;	tdcs aas1_t aas1_c;
	tydf	aapod<s2_t>		aas2_t;	tdcs aas2_t aas2_c;
	tydf	aapod<s3_t>		aas3_t;	tdcs aas3_t aas3_c;
	tydf	aapod<f2_t>		aaf2_t;	tdcs aaf2_t aaf2_c;
	tydf	aapod<f3_t>		aaf3_t;	tdcs aaf3_t aaf3_c;
	tydf	aobj<string>	astr_t; tdcs astr_t astr_c;
} // nmsp 

#define GPK_USING_ARRAY()					\
	GPK_USING_APOD		();					\
	GPK_USING_AOBJ		();					\
	GPK_USING_ASTATIC	();					\
	usng	::gpk::aauc_t, ::gpk::aauc_c 	\
		,	::gpk::aasc_t, ::gpk::aasc_c 	\
		,	::gpk::aau0_t, ::gpk::aau0_c 	\
		,	::gpk::aau1_t, ::gpk::aau1_c 	\
		,	::gpk::aau2_t, ::gpk::aau2_c 	\
		,	::gpk::aau3_t, ::gpk::aau3_c 	\
		,	::gpk::aas0_t, ::gpk::aas0_c 	\
		,	::gpk::aas1_t, ::gpk::aas1_c 	\
		,	::gpk::aas2_t, ::gpk::aas2_c 	\
		,	::gpk::aas3_t, ::gpk::aas3_c 	\
		,	::gpk::aaf2_t, ::gpk::aaf2_c 	\
		,	::gpk::aaf3_t, ::gpk::aaf3_c 	\
		,	::gpk::astr_t, ::gpk::astr_c	\
		,	::gpk::string


#endif // GPK_ARRAY_H_23627
