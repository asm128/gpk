#include "gpk_append_css.h"

usns gpk;

sttc	gpk::err_t	appendCSSKeyVal		(string & output, vcst_t key, u2_t value, CSS_METRICS_UNIT metricsUnit)	{
	return append_strings(output, key, str(value), get_value_descv(metricsUnit), ';');
}
gpk::err_t	gpk::appendCSSBorderWidth	(string & output, u0_t borderWidth, CSS_METRICS_UNIT metricsUnit)	{
	return appendCSSKeyVal(output, "border-width:", borderWidth, metricsUnit);
}
gpk::err_t	gpk::appendCSSControlWidth	(string & output, u1_t outerLimit, CSS_METRICS_UNIT metricsUnit)	{
	return appendCSSKeyVal(output, "width:", outerLimit, metricsUnit);
}
gpk::err_t	gpk::appendCSSControlHeight	(string & output, u1_t outerLimit, CSS_METRICS_UNIT metricsUnit)	{
	return appendCSSKeyVal(output, "height:", outerLimit, metricsUnit);
}
gpk::err_t	gpk::appendCSSControlSize	(string & output, n2u1_t outerLimit, CSS_METRICS_UNIT metricsUnit)	{
	return appendCSSControlWidth(output, outerLimit.x, metricsUnit) + appendCSSControlHeight(output, outerLimit.y, metricsUnit);
}
sttc	gpk::err_t	appendCSSSizeWithBorderWidthAbsolute	(string & output, n2u1_t outerLimit, u0_t borderWidth, CSS_METRICS_UNIT emtricsUnit)	{
	return appendCSSControlSize(output, n2s2_t{outerLimit.x - borderWidth * 2, outerLimit.y - borderWidth * 2}.u1(), emtricsUnit) + appendCSSBorderWidth(output, borderWidth);
}
sttc gpk::err_t appendCSSSizeWithBorderWidthRelative(string & output, n2u1_t outerLimit, u0_t borderWidth) {
	gpk::err_t			len, borderLen;
	if_fail_fe(len = append_strings(output, "width:calc(", str(outerLimit.x), "% - ", str(borderWidth * 2), "px);height:calc(", str(outerLimit.y), "% - ", str(borderWidth * 2), "px);"));
	if_fail_fe(borderLen = appendCSSBorderWidth(output, borderWidth));
	return len + borderLen;
}
gpk::err_t	gpk::appendCSSSizeWithBorderWidth				(string & output, n2u1_t outerLimit, u0_t borderWidth, CSS_METRICS_UNIT metricsUnit)	{
	return metricsUnit == CSS_METRICS_UNIT_RELATIVE 
		? appendCSSSizeWithBorderWidthRelative(output, outerLimit, borderWidth)
		: appendCSSSizeWithBorderWidthAbsolute(output, outerLimit, borderWidth, metricsUnit)
		;
}
