/// Copyright 2009-2023 - asm138
#include "gpk_tri.h"
#include "gpk_array_pod.h"

#ifndef GPK_APOD_TRI_3973498373423617
#define GPK_APOD_TRI_3973498373423617

namespace gpk
{
#pragma pack(push, 1)
	typedef	::gpk::apod<::gpk::trisc_t	>	atric	, atrichar		;
	typedef	::gpk::apod<::gpk::triuc_t>	atriuc	, atriuchar		;
	typedef	::gpk::apod<::gpk::trif2_t	>	atrif32	;
	typedef	::gpk::apod<::gpk::trif3_t	>	atrif64	;
	typedef	::gpk::apod<::gpk::triu0_t	>	atriu8	;
	typedef	::gpk::apod<::gpk::triu1_t	>	atriu16	;
	typedef	::gpk::apod<::gpk::triu2_t	>	atriu32	;
	typedef	::gpk::apod<::gpk::triu3_t	>	atriu64	;
	typedef	::gpk::apod<::gpk::tris0_t	>	atrii8		;
	typedef	::gpk::apod<::gpk::tris1_t	>	atrii16	;
	typedef	::gpk::apod<::gpk::tris2_t	>	atrii32	;
	typedef	::gpk::apod<::gpk::tris3_t	>	atrii64	;

	tplt<tpnm _tVertex>
	using		atri	= ::gpk::apod<::gpk::tri<_tVertex>>;
#pragma pack(pop)
} // namespace

#endif // GPK_APOD_TRI_3973498373423617
