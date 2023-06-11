#include "gpk_color.h"
#include "gpk_view.h"

#ifndef GPK_VIEW_COLOR_H_90238749823611
#define GPK_VIEW_COLOR_H_90238749823611

namespace gpk
{
	typedef	::gpk::view<::gpk::rgbaf	>		vrgbaf	;
	typedef	::gpk::view<::gpk::bgra		>		vbgra	;
	typedef	::gpk::view<::gpk::bgr		>		vbgr	;
	typedef	::gpk::view<::gpk::rgba		>		vrgba	;
	typedef	::gpk::view<::gpk::rgb		>		vrgb	;

	typedef	::gpk::view<const ::gpk::rgbaf	>	vcrgbaf;
	typedef	::gpk::view<const ::gpk::bgra	>	vcbgra	;
	typedef	::gpk::view<const ::gpk::bgr	>	vcbgr	;
	typedef	::gpk::view<const ::gpk::rgba	>	vcrgba	;
	typedef	::gpk::view<const ::gpk::rgb	>	vcrgb	;
} // namespace

#endif // GPK_VIEW_COLOR_H_90238749823611
