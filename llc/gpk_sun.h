#include "gpk_n3.h"

#ifndef GPK_SUN_H_23627
#define GPK_SUN_H_23627

namespace gpk
{
	stacxpr	double	SCALE_DAY_SECONDS				= 1.0 / 86400;
	stacxpr	double	SCALE_DAY_SECONDS_TO_RADIANS	= ::gpk::math_2pi * SCALE_DAY_SECONDS;
	stacxpr	double	SCALE_DAY_MINUTES_TO_RADIANS	= SCALE_DAY_SECONDS_TO_RADIANS * 60;

	::gpk::n3f32	calcSunPosition					();
	double			calcSunlightFactor				(double daylightRatioExtra = .5, uint32_t daylightOffsetMinutes = 90);
} // namespace

#endif // GPK_SUN_H_23627
