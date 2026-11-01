#include "dryer_gauge.h"
#include "gpk_raster_lh.h"
#include "gpk_bitmap_target.h"

::gpk::error_t					dry::gaugeBuildRadial			(::dry::SGaugeFeeder & gauge, const ::gpk::SCircle<float> & gaugeMetrics, int16_t resolution, int16_t width) {
	const double						stepUnit						= (1.0 / resolution) * ::gpk::math_2pi;
	const ::gpk::SSphere<float>			sphereMetrics					= {gaugeMetrics.Radius, {gaugeMetrics.Center.x, gaugeMetrics.Center.y, .5f}};
	for(int16_t iStep = 0, stepCount = resolution; iStep < stepCount; ++iStep) {
		::gpk::SPairSinCos					sinCos							= {sin(iStep * stepUnit), -cos(iStep * stepUnit)};
		const double						finalRadius						= gaugeMetrics.Radius; //::gpk::interpolate_linear(gaugeMetrics.Radius, gaugeMetrics.Radius * .5, ::gpk::clamp(abs(sinCos.Cos), 0.0, 1.0)); //
 		const ::gpk::SCoord3<double>		relativePosSmall				=
			{ sinCos.Sin * (finalRadius - width)
			, sinCos.Cos * (finalRadius - width)
			};
		const ::gpk::SCoord3<double>		relativePos				=
			{ sinCos.Sin * finalRadius
	  		, sinCos.Cos * finalRadius
			};
		gauge.Vertices.push_back(sphereMetrics.Center + relativePosSmall.Cast<float>());
		gauge.Vertices.push_back(sphereMetrics.Center + relativePos.Cast<float>());
	}
	for(int16_t iStep = 0, stepCount = (int16_t)(resolution * 2) - 3; iStep < stepCount; ++iStep) {
		gauge.Indices.push_back({int16_t(0 + iStep), int16_t(1 + iStep), int16_t(2 + iStep)});
		gauge.Indices.push_back({int16_t(1 + iStep), int16_t(3 + iStep), int16_t(2 + iStep)});
	}
	const int16_t						iStep							= int16_t(gauge.Vertices.size() - 2);
	gauge.Indices.push_back({int16_t(0 + iStep), int16_t(1 + iStep), 0});
	gauge.Indices.push_back({int16_t(1 + iStep), 1, 0});
	gauge.MaxValue					= resolution;
	return 0;
}

::gpk::error_t					dry::gaugeImageUpdate			(::dry::SGaugeFeeder & gauge, ::gpk::view_grid<::gpk::SColorBGRA> target, ::gpk::SColorBGRA colorMin, ::gpk::SColorBGRA colorMid, ::gpk::SColorBGRA colorMax)  {
	static ::gpk::SImage<uint32_t>		dummyDepth;
	const ::gpk::SCoord3<float>			center3							= (gauge.Vertices[1] - gauge.Vertices[gauge.Vertices.size() / 2 + 1]) / 2 + gauge.Vertices[gauge.Vertices.size() / 2 + 1];
	const ::gpk::SCoord2<float>			center2							= {center3.x, center3.y};
	const double						radiusLarge						= (center3 - gauge.Vertices[1]).Length();
	const double						radiusSmall						= (center3 - gauge.Vertices[0]).Length();
	const double						radiusCenter					= (radiusLarge - radiusSmall) / 2 + radiusSmall;
	dummyDepth.resize(target.metrics(), 0xFFFFFFFFU);
	::gpk::array_pod<::gpk::SCoord2<int16_t>>			pixelCoords;
	::gpk::array_pod<::gpk::STriangleWeights<float>>	triangleWeights;
	for(uint32_t iTriangle = 0, triangleCount = gauge.Indices.size(); iTriangle < triangleCount; ++iTriangle) {
		const ::gpk::STriangle3<float>		triangleCoords					=
			{ gauge.Vertices[gauge.Indices[iTriangle].A]
			, gauge.Vertices[gauge.Indices[iTriangle].B]
			, gauge.Vertices[gauge.Indices[iTriangle].C]
			};
		const double						colorFactor						= ::gpk::min(1.0, iTriangle / (double)triangleCount);
		::gpk::SColorBGRA					finalColor;
		const bool							isEmptyGauge					= ((iTriangle + 2) >> 1) >= (uint32_t)gauge.CurrentValue * 2;
		finalColor						= isEmptyGauge
			? ::gpk::SColorBGRA{::gpk::BROWN} + ::gpk::SColorBGRA{::gpk::DARKBLUE * .5}
			: (colorFactor < .5)
				? ::gpk::interpolate_linear(colorMin, colorMid, (::gpk::min(1.0, iTriangle * 2.25 / (double)triangleCount)))
				: ::gpk::interpolate_linear(colorMid, colorMax, (colorFactor - .5) * 2)
				;
		pixelCoords.clear();
		::gpk::drawTriangle(target.metrics(), triangleCoords, pixelCoords, triangleWeights, dummyDepth);
		for(uint32_t iPixelCoords = 0; iPixelCoords < pixelCoords.size(); ++iPixelCoords) {
			const ::gpk::SCoord2<int16_t>		pixelCoord						= pixelCoords[iPixelCoords];
//#define GAUGE_NO_SHADING
#ifndef GAUGE_NO_SHADING
			const ::gpk::SCoord2<float>			floatCoord						= pixelCoord.Cast<float>();
			const double						distanceFromCenter				= (floatCoord - center2).Length();
			const double						distanceFromRadiusCenter		= fabs(distanceFromCenter - radiusCenter) / ((radiusLarge - radiusSmall) / 2);
			finalColor.a					= (uint8_t)::gpk::clamp((1 - distanceFromRadiusCenter) * 255, 0.0, 255.0);
#endif
			target[pixelCoord.y][pixelCoord.x] = finalColor;
		}
	}
	return 0;
}
