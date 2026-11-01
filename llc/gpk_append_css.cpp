#include "gpk_append_css.h"

gpk::err_t	gpk::appendCSSBorderWidth			(::gpk::string & output, u0_t borderWidth, bool absoluteSize)	{
	char				cssBuffer	[32]		= {};
	stxp char		cssFormat	[]			= "border-width:%" GPK_FMT_U2 "%s;";
	const char			* unit					= absoluteSize ? "px" : "%";
	const s2_t		len						= ::gpk::sprintf_s(cssBuffer, cssFormat, borderWidth, unit);
	if_fail_fe(output.append_string(cssBuffer));
	return len;
}
gpk::err_t	gpk::appendCSSControlWidth			(::gpk::string & output, ::gpk::u1_t outerLimit, bool absoluteSize)	{
	char				cssBuffer	[24]		= {};
	stxp char		cssFormat	[]			= "width:%" GPK_FMT_U2 "%s;";
	const char			* unit					= absoluteSize ? "px" : "%";
	const s2_t 		len						= ::gpk::sprintf_s(cssBuffer, cssFormat, outerLimit, unit);
	if_fail_fe(output.append_string(cssBuffer));
	return len;
}
gpk::err_t	gpk::appendCSSControlHeight		(::gpk::string & output, ::gpk::u1_t outerLimit, bool absoluteSize)	{
	char				cssBuffer	[24]		= {};
	stxp char		cssFormat	[]			= "height:%" GPK_FMT_U2 "%s;";
	const char			* unit					= absoluteSize ? "px" : "%";
	const s2_t 		len						= ::gpk::sprintf_s(cssBuffer, cssFormat, outerLimit, unit);
	if_fail_fe(output.append_string(cssBuffer));
	return len;
}
gpk::err_t	gpk::appendCSSControlSize			(::gpk::string & output, ::gpk::n2u1_t outerLimit, bool absoluteSize)	{
	return appendCSSControlWidth(output, outerLimit.x, absoluteSize) + appendCSSControlHeight(output, outerLimit.y, absoluteSize);
}
gpk::err_t	gpk::appendCSSSizeWithBorderWidthAbsolute	(::gpk::string & output, ::gpk::n2u1_t outerLimit, u0_t borderWidth)	{
	return appendCSSControlSize(output, ::gpk::n2s2_t{outerLimit.x - borderWidth * 2, outerLimit.y - borderWidth * 2}.u1(), true) + appendCSSBorderWidth(output, borderWidth);
}
gpk::err_t	gpk::appendCSSSizeWithBorderWidthRelative	(::gpk::string & output, ::gpk::n2u1_t outerLimit, u0_t borderWidth)	{
	//const ::gpk::n2u1_t	contentLimit 			= gpk::n2s2_t{outerLimit.x - borderWidth * 2, outerLimit.y - borderWidth * 2}.u1();
	char				cssBuffer	[128]		= {};
	stxp char		cssFormat	[]			= "width:calc(%" GPK_FMT_U2 "%% - %" GPK_FMT_U2 "px);height:calc(%" GPK_FMT_U2 "%% - %" GPK_FMT_U2 "px);";
	const s2_t 		len						= ::gpk::sprintf_s(cssBuffer, cssFormat, outerLimit.x, borderWidth * 2, outerLimit.y, borderWidth * 2);
	if_fail_fe(output.append_string(cssBuffer));
	return len + appendCSSBorderWidth(output, borderWidth);
}
gpk::err_t	gpk::appendCSSSizeWithBorderWidth	(::gpk::string & output, ::gpk::n2u1_t outerLimit, u0_t borderWidth, bool absoluteSize)	{
	return absoluteSize 
		? appendCSSSizeWithBorderWidthAbsolute(output, outerLimit, borderWidth)
		: appendCSSSizeWithBorderWidthRelative(output, outerLimit, borderWidth)
		;
}
