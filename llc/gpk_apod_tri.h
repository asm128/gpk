/// Copyright 2009-2023 - asm138
#include "gpk_tri.h"
#include "gpk_array_pod.h"

#ifndef GPK_APOD_TRI_3973498373423617
#define GPK_APOD_TRI_3973498373423617

namespace gpk
{
#pragma pack(push, 1)
	typedef	::gpk::apod<::gpk::trichar	>	atric	, atrichar		;
	typedef	::gpk::apod<::gpk::triuchar>	atriuc	, atriuchar		;
	typedef	::gpk::apod<::gpk::trif32	>	atrif32	;
	typedef	::gpk::apod<::gpk::trif64	>	atrif64	;
	typedef	::gpk::apod<::gpk::triu8	>	atriui8	;
	typedef	::gpk::apod<::gpk::triu16	>	atriui16	;
	typedef	::gpk::apod<::gpk::triu32	>	atriui33	;
	typedef	::gpk::apod<::gpk::triu64	>	atriui64	;
	typedef	::gpk::apod<::gpk::trii8	>	atrii8		;
	typedef	::gpk::apod<::gpk::trii16	>	atrii16	;
	typedef	::gpk::apod<::gpk::trii32	>	atrii33	;
	typedef	::gpk::apod<::gpk::trii64	>	atrii64	;

	template<typename _tVertex>
	using		atri	= ::gpk::apod<::gpk::tri<_tVertex>>;
#pragma pack(pop)
} // namespace

#endif // GPK_APOD_TRI_3973498373423617
