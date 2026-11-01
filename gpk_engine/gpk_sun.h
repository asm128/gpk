#include "gpk_n3.h"

#ifndef GPK_SUN_H_23627
#define GPK_SUN_H_23627

namespace gpk
{
	stxp	double	SCALE_DAY_SECONDS				= 1.0 / 86400;
	stxp	double	SCALE_DAY_SECONDS_TO_RADIANS	= ::gpk::math_2pi * SCALE_DAY_SECONDS;
	stxp	double	SCALE_DAY_MINUTES_TO_RADIANS	= SCALE_DAY_SECONDS_TO_RADIANS * 60;

	::gpk::n3f2_t	calcSunPosition					();
	double			calcSunlightFactor				(double daylightRatioExtra = .5, u2_t daylightOffsetMinutes = 90);
} // namespace

#endif // GPK_SUN_H_23627
