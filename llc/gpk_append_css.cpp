#include "gpk_append_css.h"

gpk::err_t	gpk::appendCSSBorderWidth			(::gpk::asc_t & output, uint8_t borderWidth, bool absoluteSize)	{
	char				cssBuffer	[32]		= {};
	stxp char		cssFormat	[]			= "border-width:%" GPK_FMT_U2 "%s;";
	const char			* unit					= absoluteSize ? "px" : "%";
	const int32_t		len						= ::gpk::sprintf_s(cssBuffer, cssFormat, borderWidth, unit);
	gpk_necs(output.append_string(cssBuffer));
	return len;
}
gpk::err_t	gpk::appendCSSControlWidth			(::gpk::asc_t & output, uint16_t outerLimit, bool absoluteSize)	{
	char				cssBuffer	[24]		= {};
	stxp char		cssFormat	[]			= "width:%" GPK_FMT_U2 "%s;";
	const char			* unit					= absoluteSize ? "px" : "%";
	const int32_t 		len						= ::gpk::sprintf_s(cssBuffer, cssFormat, outerLimit, unit);
	gpk_necs(output.append_string(cssBuffer));
	return len;
}
gpk::err_t	gpk::appendCSSControlHeight		(::gpk::asc_t & output, uint16_t outerLimit, bool absoluteSize)	{
	char				cssBuffer	[24]		= {};
	stxp char			cssFormat	[]			= "height:%" GPK_FMT_U2 "%s;";
	const char			* unit					= absoluteSize ? "px" : "%";
	const int32_t 		len						= ::gpk::sprintf_s(cssBuffer, cssFormat, outerLimit, unit);
	gpk_necs(output.append_string(cssBuffer));
	return len;
}
gpk::err_t	gpk::appendCSSControlSize			(::gpk::asc_t & output, ::gpk::n2u1_t outerLimit, bool absoluteSize)	{
	return appendCSSControlWidth(output, outerLimit.x, absoluteSize) + appendCSSControlHeight(output, outerLimit.y, absoluteSize);
}
gpk::err_t	gpk::appendCSSSizeWithBorderWidthAbsolute	(::gpk::asc_t & output, ::gpk::n2u1_t outerLimit, uint8_t borderWidth)	{
	return appendCSSControlSize(output, ::gpk::n2s2_t{outerLimit.x - borderWidth * 2, outerLimit.y - borderWidth * 2}.u1_t(), true) + appendCSSBorderWidth(output, borderWidth);
}
gpk::err_t	gpk::appendCSSSizeWithBorderWidthRelative	(::gpk::asc_t & output, ::gpk::n2u1_t outerLimit, uint8_t borderWidth)	{
	//const ::gpk::n2u1_t	contentLimit 			= gpk::n2s2_t{outerLimit.x - borderWidth * 2, outerLimit.y - borderWidth * 2}.u1();
	char				cssBuffer	[128]		= {};
	stxp char			cssFormat	[]			= "width:calc(%" GPK_FMT_U2 "%% - %" GPK_FMT_U2 "px);height:calc(%" GPK_FMT_U2 "%% - %" GPK_FMT_U2 "px);";
	const int32_t 		len						= ::gpk::sprintf_s(cssBuffer, cssFormat, outerLimit.x, borderWidth * 2, outerLimit.y, borderWidth * 2);
	gpk_necs(output.append_string(cssBuffer));
	return len + appendCSSBorderWidth(output, borderWidth);
}
gpk::err_t	gpk::appendCSSSizeWithBorderWidth	(::gpk::asc_t & output, ::gpk::n2u1_t outerLimit, uint8_t borderWidth, bool absoluteSize)	{
	return absoluteSize 
		? appendCSSSizeWithBorderWidthAbsolute(output, outerLimit, borderWidth)
		: appendCSSSizeWithBorderWidthRelative(output, outerLimit, borderWidth)
		;
}
