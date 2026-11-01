#include "gpk_img_color.h"
#include "gpk_circle.h"
#include "gpk_tri.h"

#ifndef DRYER_GAUGE_H_92863423
#define DRYER_GAUGE_H_92863423

namespace dry
{
	struct SGaugeFeeder {
		::gpk::apod<::gpk::n3f2_t>							Vertices				= {};
		::gpk::apod<::gpk::trii16>							Indices					= {};
		int16_t												CurrentValue			= 0;
		int16_t												MaxValue				= 64;
		::gpk::img<::gpk::bgra>								Image					= {};

		stacxpr	::gpk::rgbaf								COLOR_MIN_DEFAULT		= ::gpk::GREEN;
		stacxpr	::gpk::rgbaf								COLOR_MID_DEFAULT		= ::gpk::ORANGE;
		stacxpr	::gpk::rgbaf								COLOR_MAX_DEFAULT		= ::gpk::RED;
		stacxpr	::gpk::rgbaf								COLOR_BKG_DEFAULT		= ::gpk::BROWN * .5 + ::gpk::DARKBLUE * .5;

		inline	int16_t										SetValue				(float weight)							noexcept	{ return CurrentValue = int16_t(weight * MaxValue); }
				int16_t										SetValue				(float currentValue, uint32_t maxValue)	noexcept	{ return SetValue(currentValue / maxValue); }
	};

	enum GAUGE : uint8_t
		{ GAUGE_TEMPERATURE	= 0
		, GAUGE_VACUUM
		, GAUGE_HUMIDITY
		, GAUGE_COUNT
		};

	struct SGaugeDryer {
		::dry::SGaugeFeeder		Gauges[GAUGE_COUNT]		= {};
		::gpk::img8bgra			Image					= {};
	};

	::gpk::error_t			gaugeBuildRadial		(::dry::SGaugeFeeder & gauge, const ::gpk::circlef32 & gaugeMetrics, int16_t resolution, int16_t width);
	::gpk::error_t										gaugeImageUpdate		(::dry::SGaugeFeeder & gauge, ::gpk::grid<::gpk::bgra> target, ::gpk::rgbaf colorMin = ::dry::SGaugeFeeder::COLOR_MIN_DEFAULT, ::gpk::rgbaf colorMid = ::dry::SGaugeFeeder::COLOR_MID_DEFAULT, ::gpk::rgbaf colorMax = ::dry::SGaugeFeeder::COLOR_MAX_DEFAULT, ::gpk::bgra colorEmpty = ::dry::SGaugeFeeder::COLOR_BKG_DEFAULT, bool radialColor = true);
	inline	::gpk::error_t								gaugeImageUpdate		(::dry::SGaugeFeeder & gauge, ::gpk::rgbaf colorMin = ::dry::SGaugeFeeder::COLOR_MIN_DEFAULT, ::gpk::rgbaf colorMid = ::dry::SGaugeFeeder::COLOR_MID_DEFAULT, ::gpk::rgbaf colorMax = ::dry::SGaugeFeeder::COLOR_MAX_DEFAULT, ::gpk::bgra colorEmpty = ::dry::SGaugeFeeder::COLOR_BKG_DEFAULT, bool radialColor = true) {
		gauge.Image.Texels.fill({0, 0, 0, 0});
		return ::dry::gaugeImageUpdate(gauge, gauge.Image, colorMin, colorMid, colorMax, colorEmpty, radialColor);
	}
}

#endif // DRYER_GAUGE_H_92863423
