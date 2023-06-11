#include "gpk_color.h"
#include "gpk_array_pod.h"

#ifndef GPK_ARRAY_COLOR_H_90238749823611
#define GPK_ARRAY_COLOR_H_90238749823611

namespace gpk
{
	typedef	::gpk::apod<::gpk::rgbaf	>		afrgba	;
	typedef	::gpk::apod<::gpk::bgra		>		a8bgra	;
	typedef	::gpk::apod<::gpk::bgr		>		a8bgr	;
	typedef	::gpk::apod<::gpk::rgba		>		a8rgba	;
	typedef	::gpk::apod<::gpk::rgb		>		a8rgb	;
	typedef	::gpk::apod<const ::gpk::rgbaf	>	afcrgba;
	typedef	::gpk::apod<const ::gpk::bgra	>	a8cbgra	;
	typedef	::gpk::apod<const ::gpk::bgr	>	a8cbgr	;
	typedef	::gpk::apod<const ::gpk::rgba	>	a8crgba	;
	typedef	::gpk::apod<const ::gpk::rgb	>	a8crgb	;
} // namespace

#endif // GPK_ARRAY_COLOR_H_90238749823611
