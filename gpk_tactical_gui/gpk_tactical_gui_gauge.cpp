#include "gpk_tactical_gui_gauge.h"
#include "gpk_raster_lh.h"
#include "gpk_bitmap_target.h"
#include "gpk_sphere.h"

::gpk::error_t	dry::gaugeBuildRadial		(::dry::SGaugeFeeder & gauge, const ::gpk::circlef32 & gaugeMetrics, int16_t resolution, int16_t width) {
	const double				stepUnit		= (1.0 / resolution) * ::gpk::math_2pi;
	const ::gpk::spheref32	sphereMetrics	= {gaugeMetrics.Radius, {gaugeMetrics.Center.x, gaugeMetrics.Center.y, .5f}};
	const double				outerRadius		= gaugeMetrics.Radius;
	const double				innerRadius		= outerRadius - width;
	for(int16_t iStep = 0, stepCount = resolution; iStep < stepCount; ++iStep) {
		const ::gpk::SSinCos	sinCos			= {sin(iStep * stepUnit), -cos(iStep * stepUnit)};
		const ::gpk::n3f3_t	relativePosSmall	=
			{ sinCos.Sin * innerRadius
			, sinCos.Cos * innerRadius
			};
		const ::gpk::n3f3_t	relativePos		=
			{ sinCos.Sin * outerRadius
			, sinCos.Cos * outerRadius
			};
		gauge.Vertices.push_back(sphereMetrics.Center + relativePosSmall.f2_t());
		gauge.Vertices.push_back(sphereMetrics.Center + relativePos.f2_t());
	}
	for(int16_t iStep = 0, stepCount = (int16_t)(resolution * 2) - 3; iStep < stepCount; ++iStep) {
		gauge.Indices.push_back({int16_t(0 + iStep), int16_t(1 + iStep), int16_t(2 + iStep)});
		gauge.Indices.push_back({int16_t(1 + iStep), int16_t(3 + iStep), int16_t(2 + iStep)});
	}
	const int16_t	iStep	= int16_t(gauge.Vertices.size() - 2);
	gauge.Indices.push_back({int16_t(0 + iStep), int16_t(1 + iStep), 0});
	gauge.Indices.push_back({int16_t(1 + iStep), 1, 0});
	gauge.MaxValue = resolution;
	return 0;
}

::gpk::error_t	dry::gaugeImageUpdate
	( ::dry::SGaugeFeeder & gauge
	, ::gpk::grid<::gpk::bgra> target
	, ::gpk::rgbaf colorMin
	, ::gpk::rgbaf colorMid
	, ::gpk::rgbaf colorMax
	, ::gpk::bgra colorEmpty
	, bool radialColor
	) {
	static ::gpk::imgu32	dummyDepth;
	const ::gpk::n3f2_t	center3					= (gauge.Vertices[1] - gauge.Vertices[gauge.Vertices.size() / 2 + 1]) / 2 + gauge.Vertices[gauge.Vertices.size() / 2 + 1];
	const ::gpk::n2f2_t	center2					= {center3.x, center3.y};
	const double			radiusLarge				= (center3 - gauge.Vertices[1]).Length();
	const double			radiusSmall				= (center3 - gauge.Vertices[0]).Length();
	const double			radiusHalfWidth			= (radiusLarge - radiusSmall) / 2;
	const double			radiusCenter				= radiusHalfWidth + radiusSmall;
	dummyDepth.resize(target.metrics(), 0xFFFFFFFFU);
	::gpk::apod<::gpk::n2s1_t>	pixelCoords;
	::gpk::apod<::gpk::trif32>	triangleWeights;
	for(uint32_t iTriangle = 0, triangleCount = gauge.Indices.size(); iTriangle < triangleCount; ++iTriangle) {
		const ::gpk::tri3f32	triangleCoords			= {
			  gauge.Vertices[gauge.Indices[iTriangle].A]
			, gauge.Vertices[gauge.Indices[iTriangle].B]
			, gauge.Vertices[gauge.Indices[iTriangle].C]
			};
		const double			colorFactor				= radialColor ? ::gpk::min(1.0, iTriangle / (double)triangleCount) : gauge.CurrentValue / (float)gauge.MaxValue;
		::gpk::rgbaf			finalColor;
		const bool				isEmptyGauge				= ((iTriangle + 2) >> 1) >= (uint32_t)gauge.CurrentValue * 2;
		finalColor = isEmptyGauge
			? ::gpk::rgbaf(colorEmpty)
			: (colorFactor < .5)
				? ::gpk::interpolate_linear(colorMin, colorMid, ::gpk::min(1.0, iTriangle * 2.25 / (double)triangleCount))
				: ::gpk::interpolate_linear(colorMid, colorMax, (colorFactor - .5) * 2)
			;
		finalColor.Clamp();
		pixelCoords.clear();
		::gpk::drawTriangle(target.metrics(), triangleCoords, pixelCoords, triangleWeights, dummyDepth);
		for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
			const ::gpk::n2s1_t	pixelCoord					= pixelCoords[iPixelCoord];
//#define GAUGE_NO_SHADING
#ifndef GAUGE_NO_SHADING
			const ::gpk::n2f2_t	floatCoord					= pixelCoord.f2_t();
			const double			distanceFromCenter			= (floatCoord - center2).Length();
			const double			distanceFromRadiusCenter	= fabs(distanceFromCenter - radiusCenter) / radiusHalfWidth;
			finalColor.a = (float)(1.f - distanceFromRadiusCenter);
#else
			(void)radiusCenter;
#endif
			::gpk::bgra & targetPixel = target[pixelCoord.y][pixelCoord.x];
			targetPixel = ::gpk::interpolate_linear(::gpk::rgbaf{targetPixel}, finalColor, finalColor.a);
			targetPixel.a = uint8_t(finalColor.a * 255);
		}
	}
	return 0;
}
