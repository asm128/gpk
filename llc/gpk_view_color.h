#include "gpk_color.h"
#include "gpk_view.h"

#ifndef GPK_VIEW_COLOR_H_90238749823611
#define GPK_VIEW_COLOR_H_90238749823611

namespace gpk
{
	typedef	::gpk::view<::gpk::rgbaf	>		vrgbaf	, vfrgba	;
	typedef	::gpk::view<::gpk::bgra		>		vbgra	, v8bgra	;
	typedef	::gpk::view<::gpk::bgr		>		vbgr	, v8bgr		;
	typedef	::gpk::view<::gpk::rgba		>		vrgba	, v8rgba	;
	typedef	::gpk::view<::gpk::rgb		>		vrgb	, v8rgb		;

	typedef	::gpk::view<const ::gpk::rgbaf	>	vcrgbaf	, vcfrgba	;
	typedef	::gpk::view<const ::gpk::bgra	>	vcbgra	, vc8bgra	;
	typedef	::gpk::view<const ::gpk::bgr	>	vcbgr	, vc8bgr		;
	typedef	::gpk::view<const ::gpk::rgba	>	vcrgba	, vc8rgba	;
	typedef	::gpk::view<const ::gpk::rgb	>	vcrgb	, vc8rgb		;
} // namespace

#endif // GPK_VIEW_COLOR_H_90238749823611
