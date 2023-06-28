#include "gpk_color.h"
#include "gpk_view.h"

#ifndef GPK_VIEW_COLOR_H_23627
#define GPK_VIEW_COLOR_H_23627

namespace gpk
{
	typedef	::gpk::view<::gpk::frgba	>		vrgbaf	, vfrgba	;
	typedef	::gpk::view<::gpk::u8bgra	>		vbgra	, v8bgra	;
	typedef	::gpk::view<::gpk::u8bgr	>		vbgr	, v8bgr		;
	typedef	::gpk::view<::gpk::u8rgba	>		vrgba	, v8rgba	;
	typedef	::gpk::view<::gpk::u8rgb	>		vrgb	, v8rgb		;

	typedef	::gpk::view<const ::gpk::frgba	>	vcrgbaf	, vcfrgba	;
	typedef	::gpk::view<const ::gpk::u8bgra	>	vcbgra	, vc8bgra	;
	typedef	::gpk::view<const ::gpk::u8bgr	>	vcbgr	, vc8bgr		;
	typedef	::gpk::view<const ::gpk::u8rgba	>	vcrgba	, vc8rgba	;
	typedef	::gpk::view<const ::gpk::u8rgb	>	vcrgb	, vc8rgb		;
} // namespace

#endif // GPK_VIEW_COLOR_H_23627
