#include "gpk_string_compose.h"
#include "gpk_n2.h"

#ifndef GPK_APPEND_CSS_H
#define GPK_APPEND_CSS_H

namespace gpk
{
	tpl_vtArgs	err_t	appendCSSTag							(::gpk::asc_t & output, gpk::vcst_t tagName, _tArgs&&... cssAttributes)					{ return gpk::append_strings(output, tagName, '{', cssAttributes..., '}', '\n'); }	
				err_t	appendCSSBorderWidth					(::gpk::asc_t & output, uint8_t borderWidth, bool absoluteSize = true);
				err_t	appendCSSControlWidth					(::gpk::asc_t & output, uint16_t outerLimit, bool absoluteSize = false);
				err_t	appendCSSControlHeight					(::gpk::asc_t & output, uint16_t outerLimit, bool absoluteSize = false);
				err_t	appendCSSControlSize					(::gpk::asc_t & output, ::gpk::n2u1_t outerLimit, bool absoluteSize = false);
				err_t	appendCSSSizeWithBorderWidthAbsolute	(::gpk::asc_t & output, ::gpk::n2u1_t outerLimit, uint8_t borderWidth);
				err_t	appendCSSSizeWithBorderWidthRelative	(::gpk::asc_t & output, ::gpk::n2u1_t outerLimit, uint8_t borderWidth);
				err_t	appendCSSSizeWithBorderWidth			(::gpk::asc_t & output, ::gpk::n2u1_t outerLimit, uint8_t borderWidth, bool absoluteSize = false);
} // namespace 

#endif // GPK_APPEND_CSS_H