/// Copyright 2009-2023 - asm128
#include "gpk_tri.h"
#include "gpk_view.h"

#ifndef GPK_VIEW_TRI_2973498273423617
#define GPK_VIEW_TRI_2973498273423617

namespace gpk
{
#pragma pack(push, 1)
	typedef	::gpk::view<::gpk::trichar	>	vtric  , vtrichar ;
	typedef	::gpk::view<::gpk::triuchar	>	vtriuc , vtriuchar;
	typedef	::gpk::view<::gpk::trif32	>	vtrif32;
	typedef	::gpk::view<::gpk::trif64	>	vtrif64;
	typedef	::gpk::view<::gpk::triu8	>	vtriu8 ;
	typedef	::gpk::view<::gpk::triu16	>	vtriu16;
	typedef	::gpk::view<::gpk::triu32	>	vtriu32;
	typedef	::gpk::view<::gpk::triu64	>	vtriu64;
	typedef	::gpk::view<::gpk::trii8	>	vtrii8 ;
	typedef	::gpk::view<::gpk::trii16	>	vtrii16;
	typedef	::gpk::view<::gpk::trii32	>	vtrii32;
	typedef	::gpk::view<::gpk::trii64	>	vtrii64;

	typedef	::gpk::view<const ::gpk::trichar	>	vctric  , vctrichar ;
	typedef	::gpk::view<const ::gpk::triuchar	>	vctriuc , vctriuchar;
	typedef	::gpk::view<const ::gpk::trif32		>	vctrif32;
	typedef	::gpk::view<const ::gpk::trif64		>	vctrif64;
	typedef	::gpk::view<const ::gpk::triu8		>	vctriu8 ;
	typedef	::gpk::view<const ::gpk::triu16		>	vctriu16;
	typedef	::gpk::view<const ::gpk::triu32		>	vctriu32;
	typedef	::gpk::view<const ::gpk::triu64		>	vctriu64;
	typedef	::gpk::view<const ::gpk::trii8		>	vctrii8 ;
	typedef	::gpk::view<const ::gpk::trii16		>	vctrii16;
	typedef	::gpk::view<const ::gpk::trii32		>	vctrii32;
	typedef	::gpk::view<const ::gpk::trii64		>	vctrii64;

	template<typename _tDimension>	
	using	vtri		= ::gpk::view<::gpk::tri<_tDimension>>;

	template<typename _tDimension>	
	using	vctri	= ::gpk::view<const ::gpk::tri<_tDimension>>;
#pragma pack(pop)
} // namespace

#endif // GPK_VIEW_TRI_2973498273423617
