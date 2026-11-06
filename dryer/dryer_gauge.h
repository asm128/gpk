#include "gpk_img_color.h"
#include "gpk_circle.h"
#include "gpk_tri.h"

#ifndef DRYER_GAUGE_H_92863423
#define DRYER_GAUGE_H_92863423

namespace dry
{
	usns gpk;
	struct SGaugeFeeder {
		::gpk::apod<::gpk::n3f2_t>							Vertices				= {};
		::gpk::apod<::gpk::tris1_t>							Indices					= {};
		s1_t												CurrentValue			= 0;
		s1_t												MaxValue				= 64;
		::gpk::img<::gpk::bgra>								Image					= {};

		stxp	::gpk::rgbaf								COLOR_MIN_DEFAULT		= ::gpk::GREEN;
		stxp	::gpk::rgbaf								COLOR_MID_DEFAULT		= ::gpk::ORANGE;
		stxp	::gpk::rgbaf								COLOR_MAX_DEFAULT		= ::gpk::RED;
		stxp	::gpk::rgbaf								COLOR_BKG_DEFAULT		= ::gpk::BROWN * .5 + ::gpk::DARKBLUE * .5;

		inline	s1_t										SetValue				(float weight)							noexcept	{ return CurrentValue = s1_t(weight * MaxValue); }
				s1_t										SetValue				(float currentValue, u2_t  maxValue)	noexcept	{ return SetValue(currentValue / maxValue); }
	};

	enum GAUGE : u0_t 
		{ GAUGE_TEMPERATURE	= 0
		, GAUGE_VACUUM
		, GAUGE_HUMIDITY
		, GAUGE_COUNT
		};

	struct SGaugeDryer {
		::dry::SGaugeFeeder		Gauges[GAUGE_COUNT]		= {};
		::gpk::img8bgra			Image					= {};
	};

	::gpk::err_t			gaugeBuildRadial		(::dry::SGaugeFeeder & gauge, const ::gpk::circlef2_t & gaugeMetrics, s1_t resolution, s1_t width);
	::gpk::err_t										gaugeImageUpdate		(::dry::SGaugeFeeder & gauge, ::gpk::grid<::gpk::bgra> target, ::gpk::rgbaf colorMin = ::dry::SGaugeFeeder::COLOR_MIN_DEFAULT, ::gpk::rgbaf colorMid = ::dry::SGaugeFeeder::COLOR_MID_DEFAULT, ::gpk::rgbaf colorMax = ::dry::SGaugeFeeder::COLOR_MAX_DEFAULT, ::gpk::bgra colorEmpty = ::dry::SGaugeFeeder::COLOR_BKG_DEFAULT, bool radialColor = true);
	inline	::gpk::err_t								gaugeImageUpdate		(::dry::SGaugeFeeder & gauge, ::gpk::rgbaf colorMin = ::dry::SGaugeFeeder::COLOR_MIN_DEFAULT, ::gpk::rgbaf colorMid = ::dry::SGaugeFeeder::COLOR_MID_DEFAULT, ::gpk::rgbaf colorMax = ::dry::SGaugeFeeder::COLOR_MAX_DEFAULT, ::gpk::bgra colorEmpty = ::dry::SGaugeFeeder::COLOR_BKG_DEFAULT, bool radialColor = true) {
		gauge.Image.Texels.fill({0, 0, 0, 0});
		return ::dry::gaugeImageUpdate(gauge, gauge.Image, colorMin, colorMid, colorMax, colorEmpty, radialColor);
	}
}

#endif // DRYER_GAUGE_H_92863423
