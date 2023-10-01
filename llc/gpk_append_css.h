#include "gpk_string_compose.h"

#ifndef GPK_APPEND_CSS_H
#define GPK_APPEND_CSS_H

namespace gpk
{
	tplt<tpnm... _tArgs>
	gpk::err_t			appendCSSTag							(::gpk::achar & output, gpk::vcs tagName, _tArgs&&... cssAttributes)					{ return gpk::append_strings(output, tagName, '{', cssAttributes..., '}', '\n'); }	
	gpk::err_t			appendCSSBorderWidth					(::gpk::achar & output, uint8_t borderWidth, bool absoluteSize = true);
	gpk::err_t			appendCSSControlWidth					(::gpk::achar & output, uint16_t outerLimit, bool absoluteSize = false);
	gpk::err_t			appendCSSControlHeight					(::gpk::achar & output, uint16_t outerLimit, bool absoluteSize = false);
	gpk::err_t			appendCSSControlSize					(::gpk::achar & output, ::gpk::n2u16 outerLimit, bool absoluteSize = false);
	gpk::err_t			appendCSSSizeWithBorderWidthAbsolute	(::gpk::achar & output, ::gpk::n2u16 outerLimit, uint8_t borderWidth);
	gpk::err_t			appendCSSSizeWithBorderWidthRelative	(::gpk::achar & output, ::gpk::n2u16 outerLimit, uint8_t borderWidth);
	gpk::err_t			appendCSSSizeWithBorderWidth			(::gpk::achar & output, ::gpk::n2u16 outerLimit, uint8_t borderWidth, bool absoluteSize = false);
} // namespace 

#endif // GPK_APPEND_CSS_H