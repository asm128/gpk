#include "gpk_append_css.h"

gpk::err_t	gpk::appendCSSBorderWidth			(::gpk::achar & output, uint8_t borderWidth, bool absoluteSize)	{
	char				cssBuffer	[32]		= {};
	stacxpr char		cssFormat	[]			= "border-width:%u%s;";
	const char			* unit					= absoluteSize ? "px" : "%";
	const int32_t		len						= ::gpk::sprintf_s(cssBuffer, cssFormat, borderWidth, unit);
	gpk_necs(output.append_string(cssBuffer));
	return len;
}
gpk::err_t	gpk::appendCSSControlWidth			(::gpk::achar & output, uint16_t outerLimit, bool absoluteSize)	{
	char				cssBuffer	[24]		= {};
	stacxpr char		cssFormat	[]			= "width:%u%s;";
	const char			* unit					= absoluteSize ? "px" : "%";
	const int32_t 		len						= ::gpk::sprintf_s(cssBuffer, cssFormat, outerLimit, unit);
	gpk_necs(output.append_string(cssBuffer));
	return len;
}
gpk::err_t	gpk::appendCSSControlHeight		(::gpk::achar & output, uint16_t outerLimit, bool absoluteSize)	{
	char				cssBuffer	[24]		= {};
	stacxpr char		cssFormat	[]			= "height:%u%s;";
	const char			* unit					= absoluteSize ? "px" : "%";
	const int32_t 		len						= ::gpk::sprintf_s(cssBuffer, cssFormat, outerLimit, unit);
	gpk_necs(output.append_string(cssBuffer));
	return len;
}
gpk::err_t	gpk::appendCSSControlSize			(::gpk::achar & output, ::gpk::n2u16 outerLimit, bool absoluteSize)	{
	return appendCSSControlWidth(output, outerLimit.x, absoluteSize) + appendCSSControlHeight(output, outerLimit.y, absoluteSize);
}
gpk::err_t	gpk::appendCSSSizeWithBorderWidthAbsolute	(::gpk::achar & output, ::gpk::n2u16 outerLimit, uint8_t borderWidth)	{
	return appendCSSControlSize(output, ::gpk::n2i32{outerLimit.x - borderWidth * 2, outerLimit.y - borderWidth * 2}.u16(), true) + appendCSSBorderWidth(output, borderWidth);
}
gpk::err_t	gpk::appendCSSSizeWithBorderWidthRelative	(::gpk::achar & output, ::gpk::n2u16 outerLimit, uint8_t borderWidth)	{
	const ::gpk::n2u16	contentLimit 			= gpk::n2i32{outerLimit.x - borderWidth * 2, outerLimit.y - borderWidth * 2}.u16();
	char				cssBuffer	[128]		= {};
	stacxpr char		cssFormat	[]			= "width:calc(%u%% - %upx);height:calc(%u%% - %upx);";
	const int32_t 		len						= ::gpk::sprintf_s(cssBuffer, cssFormat, outerLimit.x, borderWidth * 2, outerLimit.y, borderWidth * 2);
	gpk_necs(output.append_string(cssBuffer));
	return len + appendCSSBorderWidth(output, borderWidth);
}
gpk::err_t	gpk::appendCSSSizeWithBorderWidth	(::gpk::achar & output, ::gpk::n2u16 outerLimit, uint8_t borderWidth, bool absoluteSize)	{
	return absoluteSize 
		? appendCSSSizeWithBorderWidthAbsolute(output, outerLimit, borderWidth)
		: appendCSSSizeWithBorderWidthRelative(output, outerLimit, borderWidth)
		;
}
