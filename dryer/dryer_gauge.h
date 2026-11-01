#include "gpk_img_color.h"

#ifndef DRYER_GAUGE_H_92863423
#define DRYER_GAUGE_H_92863423

namespace dry
{
	struct SGaugeFeeder {
		::gpk::apod<::gpk::n3f2_t>							Vertices				= {};
		::gpk::apod<::gpk::STriangleWeights<int16_t>>		Indices					= {};
		int16_t												CurrentValue			= 0;
		int16_t												MaxValue				= 64;
		::gpk::img8bgra										Image					= {};

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

	::gpk::error_t			gaugeBuildRadial		(::dry::SGaugeFeeder & gauge, const ::gpk::SCircle<float> & gaugeMetrics, int16_t resolution, int16_t width);
	::gpk::error_t			gaugeBuildHorizontal	(::dry::SGaugeFeeder & gauge, const ::gpk::SCircle<float> & gaugeMetrics, int16_t resolution, int16_t width);
	::gpk::error_t										gaugeImageUpdate		(::dry::SGaugeFeeder & gauge, ::gpk::grid<::gpk::SColorBGRA> target, ::gpk::SColorBGRA colorMin = ::gpk::GREEN, ::gpk::SColorBGRA colorMid = ::gpk::ORANGE, ::gpk::SColorBGRA colorMax = ::gpk::LIGHTRED);
	inline	::gpk::error_t								gaugeImageUpdate		(::dry::SGaugeFeeder & gauge, ::gpk::SColorBGRA colorMin = ::gpk::GREEN, ::gpk::SColorBGRA colorMid = ::gpk::ORANGE, ::gpk::SColorBGRA colorMax = ::gpk::LIGHTRED) {
		gauge.Image.Texels.fill({0, 0, 0, 0});
		return ::dry::gaugeImageUpdate(gauge, gauge.Image, colorMin, colorMid, colorMax);
	}
	::gpk::error_t										dryerGaugeBuild			(::dry::SGaugeFeeder & gauge, const ::gpk::SCircle<float> & gaugeMetrics, int16_t resolutionTemp, int16_t resolutionVac);
	::gpk::error_t										dryerGaugeImageUpdate	(::dry::SGaugeFeeder & gauge, ::gpk::minmax<::gpk::SColorBGRA> colorsTemperature, ::gpk::minmax<::gpk::SColorBGRA> colorsVacuum);
}

#endif // DRYER_GAUGE_H_92863423
