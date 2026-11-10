/// Copyright 2009-2023 - asm128
#include "gpk_tri.h"
#include "gpk_view.h"

#ifndef GPK_VIEW_TRI_2973498273423617
#define GPK_VIEW_TRI_2973498273423617

namespace gpk
{
#pragma pack(push, 1)
	typedef	::gpk::view<::gpk::trisc_t	>	vtric  , vtrichar ;
	typedef	::gpk::view<::gpk::triuc_t	>	vtriuc , vtriuchar;
	typedef	::gpk::view<::gpk::trif2_t	>	vtrif32;
	typedef	::gpk::view<::gpk::trif3_t	>	vtrif64;
	typedef	::gpk::view<::gpk::triu0_t	>	vtriu8 ;
	typedef	::gpk::view<::gpk::triu1_t	>	vtriu16;
	typedef	::gpk::view<::gpk::triu2_t	>	vtriu32;
	typedef	::gpk::view<::gpk::triu3_t	>	vtriu64;
	typedef	::gpk::view<::gpk::tris0_t	>	vtrii8 ;
	typedef	::gpk::view<::gpk::tris1_t	>	vtrii16;
	typedef	::gpk::view<::gpk::tris2_t	>	vtrii32;
	typedef	::gpk::view<::gpk::tris3_t	>	vtrii64;

	typedef	::gpk::view<const ::gpk::trisc_t	>	vctric  , vctrichar ;
	typedef	::gpk::view<const ::gpk::triuc_t	>	vctriuc , vctriuchar;
	typedef	::gpk::view<const ::gpk::trif2_t		>	vctrif32;
	typedef	::gpk::view<const ::gpk::trif3_t		>	vctrif64;
	typedef	::gpk::view<const ::gpk::triu0_t		>	vctriu8 ;
	typedef	::gpk::view<const ::gpk::triu1_t		>	vctriu16;
	typedef	::gpk::view<const ::gpk::triu2_t		>	vctriu32;
	typedef	::gpk::view<const ::gpk::triu3_t		>	vctriu64;
	typedef	::gpk::view<const ::gpk::tris0_t		>	vctrii8 ;
	typedef	::gpk::view<const ::gpk::tris1_t		>	vctrii16;
	typedef	::gpk::view<const ::gpk::tris2_t		>	vctrii32;
	typedef	::gpk::view<const ::gpk::tris3_t		>	vctrii64;

	tplt<tpnm _tDimension>	
	using	vtri		= ::gpk::view<::gpk::tri<_tDimension>>;

	tplt<tpnm _tDimension>	
	using	vctri	= ::gpk::view<const ::gpk::tri<_tDimension>>;
#pragma pack(pop)
} // namespace

#endif // GPK_VIEW_TRI_2973498273423617
