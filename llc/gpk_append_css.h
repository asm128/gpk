#include "gpk_string_compose.h"
#include "gpk_n2.h"
#include "gpk_enum.h"

#ifndef GPK_APPEND_CSS_H
#define GPK_APPEND_CSS_H

namespace gpk
{
	tpl_vtArgs
	err_t	appendCSSTag							(string & output, vcst_t tagName, _tArgs&&... cssAttributes)					{ return gpk::append_strings(output, tagName, '{', cssAttributes..., '}', '\n'); }

	GDEFINE_ENUM_TYPE(CSS_METRICS_UNIT, u0_t);
	GDEFINE_ENUM_VALUED(CSS_METRICS_UNIT, RELATIVE, 0, "%");
	GDEFINE_ENUM_VALUED(CSS_METRICS_UNIT, ABSOLUTE_PX, 1, "px");
	GDEFINE_ENUM_VALUED(CSS_METRICS_UNIT, ABSOLUTE_PT, 2, "pt");
	err_t	appendCSSBorderWidth					(string & output, u0_t	 borderWidth	, CSS_METRICS_UNIT metricsUnit = CSS_METRICS_UNIT_ABSOLUTE_PX);
	err_t	appendCSSControlWidth					(string & output, u1_t	 outerLimit		, CSS_METRICS_UNIT metricsUnit = CSS_METRICS_UNIT_RELATIVE);
	err_t	appendCSSControlHeight					(string & output, u1_t	 outerLimit		, CSS_METRICS_UNIT metricsUnit = CSS_METRICS_UNIT_RELATIVE);
	err_t	appendCSSControlSize					(string & output, n2u1_t outerLimit		, CSS_METRICS_UNIT metricsUnit = CSS_METRICS_UNIT_RELATIVE);
	err_t	appendCSSSizeWithBorderWidth			(string & output, n2u1_t outerLimit		, u0_t borderWidth, CSS_METRICS_UNIT metricsUnit = CSS_METRICS_UNIT_RELATIVE);
} // namespace 

#endif // GPK_APPEND_CSS_H
