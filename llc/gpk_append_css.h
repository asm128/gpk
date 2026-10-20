#include "gpk_string_compose.h"
#include "gpk_n2.h"

#ifndef GPK_APPEND_CSS_H
#define GPK_APPEND_CSS_H

namespace gpk
{
	tpl_vtArgs	err_t	appendCSSTag							(string & output, gpk::vcst_t tagName, _tArgs&&... cssAttributes)					{ return gpk::append_strings(output, tagName, '{', cssAttributes..., '}', '\n'); }
				err_t	appendCSSBorderWidth					(string & output, u0_t borderWidth, bool absoluteSize = true);
				err_t	appendCSSControlWidth					(string & output, ::gpk::u1_t outerLimit, bool absoluteSize = false);
				err_t	appendCSSControlHeight					(string & output, ::gpk::u1_t outerLimit, bool absoluteSize = false);
				err_t	appendCSSControlSize					(string & output, n2u1_t outerLimit, bool absoluteSize = false);
				err_t	appendCSSSizeWithBorderWidthAbsolute	(string & output, n2u1_t outerLimit, u0_t borderWidth);
				err_t	appendCSSSizeWithBorderWidthRelative	(string & output, n2u1_t outerLimit, u0_t borderWidth);
				err_t	appendCSSSizeWithBorderWidth			(string & output, n2u1_t outerLimit, u0_t borderWidth, bool absoluteSize = false);
} // namespace 

#endif // GPK_APPEND_CSS_H


