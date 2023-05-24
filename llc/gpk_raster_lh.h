#include "gpk_view_grid.h"
#include "gpk_matrix.h"
#include "gpk_color.h"
#include "gpk_geometry_lh.h"

#include "gpk_array_pod.h"
#include "gpk_line2.h"
#include "gpk_line3.h"
#include "gpk_circle.h"

#include <functional>

#ifndef CED_DRAW_H_29837429837
#define CED_DRAW_H_29837429837

namespace gpk
{
	struct SLight3 {
		::gpk::n3f32	Position;
		float			Range;
	};

	int								pixelBlend			(::gpk::view2d<::gpk::bgra> pixels, ::gpk::n2i16 position	, ::gpk::bgra color);
	int								setPixel			(::gpk::view2d<::gpk::bgra> pixels, ::gpk::n2i16 position	, ::gpk::bgra color);
	int								drawRectangle		(::gpk::view2d<::gpk::bgra> pixels, ::gpk::rect2	<int16_t> rectangle	, ::gpk::bgra color);
	int								drawCircle			(::gpk::view2d<::gpk::bgra> pixels, ::gpk::SCircle		<int16_t> circle	, ::gpk::bgra color);
	int								drawLine			(::gpk::view2d<::gpk::bgra> pixels, ::gpk::line2		<int16_t> line		, ::gpk::bgra color);
	int								drawLine			(const ::gpk::n2<uint16_t>	offscreenMetrics, ::gpk::line2		<int16_t> line		, ::gpk::apod<::gpk::n2i16> & pixelCoords);
	int								drawLine
		( const ::gpk::n2<uint16_t>		offscreenMetrics
		, const ::gpk::line3<float>		& lineFloat
		, ::gpk::apod<::gpk::n3f32>		& pixelCoords
		, ::gpk::view2d<uint32_t>		depthBuffer
		);
	int								drawTriangle		(::gpk::view2d<::gpk::bgra> pixels	, const ::gpk::tri2<int16_t>	& triangle	, ::gpk::bgra color);
	int								drawTriangle		(::gpk::n2<uint32_t> targetSize		, const ::gpk::tri3<float>	& triangle, ::gpk::apod<::gpk::n2i16> & pixelCoords, ::gpk::apod<::gpk::tri<float>> & proportions, ::gpk::view2d<uint32_t> depthBuffer);
	int								drawTriangle		(::gpk::n2<uint32_t> targetSize		, const ::gpk::tri2<float>	& triangle	, ::gpk::apod<::gpk::n2i16> & pixelCoords, ::gpk::apod<::gpk::tri<float>> & proportions);
	int								drawQuadTriangle
		( ::gpk::view2d<::gpk::bgra>				targetPixels
		, const ::gpk::SGeometryQuads						& geometry
		, const int											iTriangle
		, const ::gpk::m4<float>						& matrixTransform
		, const ::gpk::m4<float>						& matrixTransformView
		, const ::gpk::n3<float>						& lightVector
		, const ::gpk::bgra							color
		, ::gpk::apod<::gpk::n2i16>			& pixelCoords
		, ::gpk::apod<::gpk::tri<float>>	& pixelVertexWeights
		, ::gpk::view2d<uint32_t>						depthBuffer
		);
	int								drawQuadTriangle
		( ::gpk::view2d<::gpk::bgra>				targetPixels
		, const ::gpk::SGeometryQuads						& geometry
		, const int											iTriangle
		, const ::gpk::m4<float>						& matrixTransform
		, const ::gpk::m4<float>						& matrixTransformView
		, const ::gpk::n3<float>						& lightVector
		, ::gpk::apod<::gpk::n2i16>			& pixelCoords
		, ::gpk::apod<::gpk::tri<float>>	& pixelVertexWeights
		, ::gpk::view2d<const ::gpk::bgra>			textureImage
		, ::gpk::apod<::gpk::n3<float>>			& lightPoints
		, ::gpk::apod<::gpk::bgra>				& lightColors
		, ::gpk::view2d<uint32_t>						depthBuffer
		);

	int								drawQuadTriangle
		( ::gpk::view2d<::gpk::bgra>				targetPixels
		, const ::gpk::SGeometryQuads						& geometry
		, const int											iTriangle
		, const ::gpk::m4<float>						& matrixTransform
		, const ::gpk::m4<float>						& matrixTransformView
		, const ::gpk::n3<float>						& lightVector
		, ::gpk::apod<::gpk::n2i16>			& pixelCoords
		, ::gpk::apod<::gpk::tri<float>>	& pixelVertexWeights
		, ::gpk::view2d<const ::gpk::bgra>			textureImage
		, ::gpk::apod<::gpk::n3<float>>			& lightPoints
		, ::gpk::apod<::gpk::bgra>				& lightColors
		, ::gpk::view2d<uint32_t>						depthBuffer
		, const ::std::function<::gpk::error_t(::gpk::view2d<::gpk::bgra> targetPixels, const ::gpk::n2i16 & pixelCoord, const ::gpk::bgra & color)> & funcSetPixel
		);

	int								drawQuadTriangle
		( const ::gpk::n2<uint32_t>					& targetSize
		, ::gpk::tri3<float>					triangle
		, const ::gpk::m4<float>						& matrixTransformView
		, ::gpk::apod<::gpk::n2i16>			& pixelCoords
		, ::gpk::apod<::gpk::tri<float>>	& pixelVertexWeights
		, ::gpk::view2d<uint32_t>						depthBuffer
		);
	int								drawQuadTriangle
		( const ::gpk::n2<uint32_t>					& targetSize
		, const ::gpk::tri3<float>					& triangleScreen
		, ::gpk::apod<::gpk::n2i16>			& pixelCoords
		, ::gpk::apod<::gpk::tri<float>>	& pixelVertexWeights
		, ::gpk::view2d<uint32_t>						depthBuffer
		);
	int								drawTriangle
		( ::gpk::view2d<::gpk::bgra>				targetPixels
		, const ::gpk::SGeometryTriangles					& geometry
		, const int											iTriangle
		, const ::gpk::m4<float>						& matrixTransform
		, const ::gpk::m4<float>						& matrixTransformView
		, const ::gpk::n3<float>						& lightVector
		, ::gpk::bgra									color
		, ::gpk::apod<::gpk::n2i16>			& pixelCoords
		, ::gpk::apod<::gpk::tri<float>>	& pixelVertexWeights
		, ::gpk::view2d<uint32_t>						depthBuffer
		);
	int								drawTriangle
		( ::gpk::view2d<::gpk::bgra>				targetPixels
		, const ::gpk::SGeometryTriangles					& geometry
		, const int											iTriangle
		, const ::gpk::m4<float>						& matrixTransform
		, const ::gpk::m4<float>						& matrixTransformView
		, const ::gpk::n3<float>						& lightVector
		, ::gpk::apod<::gpk::n2i16>			& pixelCoords
		, ::gpk::apod<::gpk::tri<float>>	& pixelVertexWeights
		, ::gpk::view2d<const ::gpk::bgra>			textureImage
		, ::gpk::view2d<uint32_t>						depthBuffer
		);
	int								drawTriangle
		( ::gpk::view2d<::gpk::bgra>			targetPixels
		, const ::gpk::SGeometryTriangles					& geometry
		, const int											iTriangle
		, const ::gpk::m4<float>						& matrixTransform
		, const ::gpk::m4<float>						& matrixTransformView
		, const ::gpk::n3<float>						& lightVector
		, const ::gpk::SColorFloat							& lightColor
		, ::gpk::apod<::gpk::n2i16>			& pixelCoords
		, ::gpk::apod<::gpk::tri<float>>	& pixelVertexWeights
		, ::gpk::view2d<const ::gpk::bgra>			textureImage
		, ::gpk::apod<::gpk::SLight3>					& lightPoints
		, ::gpk::apod<::gpk::bgra>				& lightColors
		, ::gpk::view2d<uint32_t>						depthBuffer
		);
	int								drawTriangle
		( ::gpk::view2d<::gpk::bgra>			targetPixels
		, const ::gpk::SGeometryIndexedTriangles			& geometry
		, const int											iTriangle
		, const ::gpk::m4<float>						& matrixTransform
		, const ::gpk::m4<float>						& matrixTransformView
		, const ::gpk::n3<float>						& lightVector
		, const ::gpk::SColorFloat							& lightColor
		, ::gpk::apod<::gpk::n2i16>			& pixelCoords
		, ::gpk::apod<::gpk::tri<float>>	& pixelVertexWeights
		, ::gpk::view2d<const ::gpk::bgra>			textureImage
		, ::gpk::apod<::gpk::SLight3>					& lightPoints
		, ::gpk::apod<::gpk::SColorFloat>				& lightColors
		, ::gpk::view2d<uint32_t>						depthBuffer
		);

	int													drawPixels
		( ::gpk::view2d<::gpk::bgra>				targetPixels
		, const ::gpk::tri3<float>					& triangleWorld
		, const ::gpk::n3<float>					& normal
		, const ::gpk::tri2<float>					& triangleTexCoords
		, const ::gpk::n3<float>					& lightVector
		, ::gpk::apod<::gpk::n2i16>			& pixelCoords
		, ::gpk::apod<::gpk::tri<float>>	& pixelVertexWeights
		, ::gpk::view2d<const ::gpk::bgra>			textureImage
		, ::gpk::apod<::gpk::n3<float>>			& lightPoints
		, ::gpk::apod<::gpk::bgra>				& lightColors
		, const ::std::function<::gpk::error_t(::gpk::view2d<::gpk::bgra> targetPixels, const ::gpk::n2i16 & pixelCoord, const ::gpk::bgra & color)> & funcSetPixel
		);

	int													drawPixels
		( ::gpk::view2d<::gpk::bgra>				targetPixels
		, const ::gpk::tri3<float>					& triangleWorld
		, const ::gpk::n3<float>					& normal
		, const ::gpk::tri2<float>					& triangleTexCoords
		, const ::gpk::n3<float>					& lightVector
		, ::gpk::apod<::gpk::n2i16>			& pixelCoords
		, ::gpk::apod<::gpk::tri<float>>	& pixelVertexWeights
		, ::gpk::view2d<const ::gpk::bgra>			textureImage
		, ::gpk::apod<::gpk::n3<float>>			& lightPoints
		, ::gpk::apod<::gpk::bgra>				& lightColors
		);
} // namespace

#endif // CED_DRAW_H_29837429837
