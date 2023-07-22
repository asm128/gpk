#include "gpk_sun.h"
#include <ctime>

double					gpk::calcSunlightFactor(double daylightRatioExtra, uint32_t daylightOffsetMinutes)	{ 
	const double				sunAngleFactor			= ::gpk::calcSunPosition().Dot(::gpk::n3f32{0, 1}.RotateZ(::gpk::SCALE_DAY_MINUTES_TO_RADIANS * daylightOffsetMinutes));
	return ::gpk::clamped(sunAngleFactor + daylightRatioExtra, 0.0, 1.0);
}

::gpk::n3f32			gpk::calcSunPosition	()	{ 
	const time_t				now						= time(0);
	tm							ltm						= {};
	localtime_s(&ltm, &now);

	const uint32_t				hourInSeconds			= ltm.tm_hour * 60 * 60;
	const uint32_t				minsInSeconds			= ltm.tm_min * 60;
	const uint32_t				daySeconds				= hourInSeconds + minsInSeconds + ltm.tm_sec;

	return ::gpk::n3f32{0, -1}.RotateZ(::gpk::SCALE_DAY_SECONDS_TO_RADIANS * daySeconds);
}
