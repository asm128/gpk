#include "gpk_view_grid.h"
#include "gpk_matrix.h"
#include "gpk_color.h"
#include "gpk_geometry_lh.h"

#include "gpk_array.h"

#include <functional>

#ifndef CED_DRAW_H_29837429837
#define CED_DRAW_H_29837429837

namespace gpk
{
	struct SLight3 {
		::gpk::n3<float>			Position;
		float							Range;
	};

	int								pixelBlend			(::gpk::view2d<::gpk::SColorBGRA> pixels, ::gpk::n2		<int16_t> position	, ::gpk::SColorBGRA color);
	int								setPixel			(::gpk::view2d<::gpk::SColorBGRA> pixels, ::gpk::n2		<int16_t> position	, ::gpk::SColorBGRA color);
	int								drawRectangle		(::gpk::view2d<::gpk::SColorBGRA> pixels, ::gpk::SRectangle2	<int16_t> rectangle	, ::gpk::SColorBGRA color);
	int								drawCircle			(::gpk::view2d<::gpk::SColorBGRA> pixels, ::gpk::SCircle		<int16_t> circle	, ::gpk::SColorBGRA color);
	int								drawLine			(::gpk::view2d<::gpk::SColorBGRA> pixels, ::gpk::SLine2		<int16_t> line		, ::gpk::SColorBGRA color);
	int								drawLine			(const ::gpk::n2<uint16_t>	offscreenMetrics, ::gpk::SLine2		<int16_t> line		, ::gpk::apod<::gpk::n2<int16_t>> & pixelCoords);
	int								drawLine
		( const ::gpk::n2<uint16_t>				offscreenMetrics
		, const ::gpk::SLine3<float>					& lineFloat
		, ::gpk::apod<::gpk::n3<float>>		& pixelCoords
		, ::gpk::view2d<uint32_t>					depthBuffer
		);
	int								drawTriangle		(::gpk::view2d<::gpk::SColorBGRA> pixels	, const ::gpk::STriangle2<int16_t>	& triangle	, ::gpk::SColorBGRA color);
	int								drawTriangle		(::gpk::n2<uint32_t> targetSize		, const ::gpk::STriangle3<float>	& triangle, ::gpk::apod<::gpk::n2<int16_t>> & pixelCoords, ::gpk::apod<::gpk::STriangle<float>> & proportions, ::gpk::view2d<uint32_t> depthBuffer);
	int								drawTriangle		(::gpk::n2<uint32_t> targetSize		, const ::gpk::STriangle2<float>	& triangle	, ::gpk::apod<::gpk::n2<int16_t>> & pixelCoords, ::gpk::apod<::gpk::STriangle<float>> & proportions);
	int								drawQuadTriangle
		( ::gpk::view2d<::gpk::SColorBGRA>				targetPixels
		, const ::gpk::SGeometryQuads						& geometry
		, const int											iTriangle
		, const ::gpk::SMatrix4<float>						& matrixTransform
		, const ::gpk::SMatrix4<float>						& matrixTransformView
		, const ::gpk::n3<float>						& lightVector
		, const ::gpk::SColorBGRA							color
		, ::gpk::apod<::gpk::n2<int16_t>>			& pixelCoords
		, ::gpk::apod<::gpk::STriangle<float>>	& pixelVertexWeights
		, ::gpk::view2d<uint32_t>						depthBuffer
		);
	int								drawQuadTriangle
		( ::gpk::view2d<::gpk::SColorBGRA>				targetPixels
		, const ::gpk::SGeometryQuads						& geometry
		, const int											iTriangle
		, const ::gpk::SMatrix4<float>						& matrixTransform
		, const ::gpk::SMatrix4<float>						& matrixTransformView
		, const ::gpk::n3<float>						& lightVector
		, ::gpk::apod<::gpk::n2<int16_t>>			& pixelCoords
		, ::gpk::apod<::gpk::STriangle<float>>	& pixelVertexWeights
		, ::gpk::view2d<const ::gpk::SColorBGRA>			textureImage
		, ::gpk::apod<::gpk::n3<float>>			& lightPoints
		, ::gpk::apod<::gpk::SColorBGRA>				& lightColors
		, ::gpk::view2d<uint32_t>						depthBuffer
		);

	int								drawQuadTriangle
		( ::gpk::view2d<::gpk::SColorBGRA>				targetPixels
		, const ::gpk::SGeometryQuads						& geometry
		, const int											iTriangle
		, const ::gpk::SMatrix4<float>						& matrixTransform
		, const ::gpk::SMatrix4<float>						& matrixTransformView
		, const ::gpk::n3<float>						& lightVector
		, ::gpk::apod<::gpk::n2<int16_t>>			& pixelCoords
		, ::gpk::apod<::gpk::STriangle<float>>	& pixelVertexWeights
		, ::gpk::view2d<const ::gpk::SColorBGRA>			textureImage
		, ::gpk::apod<::gpk::n3<float>>			& lightPoints
		, ::gpk::apod<::gpk::SColorBGRA>				& lightColors
		, ::gpk::view2d<uint32_t>						depthBuffer
		, const ::std::function<::gpk::error_t(::gpk::view2d<::gpk::SColorBGRA> targetPixels, const ::gpk::n2<int16_t> & pixelCoord, const ::gpk::SColorBGRA & color)> & funcSetPixel
		);

	int								drawQuadTriangle
		( const ::gpk::n2<uint32_t>					& targetSize
		, ::gpk::STriangle3			<float>					triangle
		, const ::gpk::SMatrix4<float>						& matrixTransformView
		, ::gpk::apod<::gpk::n2<int16_t>>			& pixelCoords
		, ::gpk::apod<::gpk::STriangle<float>>	& pixelVertexWeights
		, ::gpk::view2d<uint32_t>						depthBuffer
		);
	int								drawQuadTriangle
		( const ::gpk::n2<uint32_t>					& targetSize
		, const ::gpk::STriangle3	<float>					& triangleScreen
		, ::gpk::apod<::gpk::n2<int16_t>>			& pixelCoords
		, ::gpk::apod<::gpk::STriangle<float>>	& pixelVertexWeights
		, ::gpk::view2d<uint32_t>						depthBuffer
		);
	int								drawTriangle
		( ::gpk::view2d<::gpk::SColorBGRA>				targetPixels
		, const ::gpk::SGeometryTriangles					& geometry
		, const int											iTriangle
		, const ::gpk::SMatrix4<float>						& matrixTransform
		, const ::gpk::SMatrix4<float>						& matrixTransformView
		, const ::gpk::n3<float>						& lightVector
		, ::gpk::SColorBGRA									color
		, ::gpk::apod<::gpk::n2<int16_t>>			& pixelCoords
		, ::gpk::apod<::gpk::STriangle<float>>	& pixelVertexWeights
		, ::gpk::view2d<uint32_t>						depthBuffer
		);
	int								drawTriangle
		( ::gpk::view2d<::gpk::SColorBGRA>				targetPixels
		, const ::gpk::SGeometryTriangles					& geometry
		, const int											iTriangle
		, const ::gpk::SMatrix4<float>						& matrixTransform
		, const ::gpk::SMatrix4<float>						& matrixTransformView
		, const ::gpk::n3<float>						& lightVector
		, ::gpk::apod<::gpk::n2<int16_t>>			& pixelCoords
		, ::gpk::apod<::gpk::STriangle<float>>	& pixelVertexWeights
		, ::gpk::view2d<const ::gpk::SColorBGRA>			textureImage
		, ::gpk::view2d<uint32_t>						depthBuffer
		);
	int								drawTriangle
		( ::gpk::view2d<::gpk::SColorBGRA>			targetPixels
		, const ::gpk::SGeometryTriangles					& geometry
		, const int											iTriangle
		, const ::gpk::SMatrix4<float>						& matrixTransform
		, const ::gpk::SMatrix4<float>						& matrixTransformView
		, const ::gpk::n3<float>						& lightVector
		, const ::gpk::SColorFloat							& lightColor
		, ::gpk::apod<::gpk::n2<int16_t>>			& pixelCoords
		, ::gpk::apod<::gpk::STriangle<float>>	& pixelVertexWeights
		, ::gpk::view2d<const ::gpk::SColorBGRA>			textureImage
		, ::gpk::apod<::gpk::SLight3>					& lightPoints
		, ::gpk::apod<::gpk::SColorBGRA>				& lightColors
		, ::gpk::view2d<uint32_t>						depthBuffer
		);
	int								drawTriangle
		( ::gpk::view2d<::gpk::SColorBGRA>			targetPixels
		, const ::gpk::SGeometryIndexedTriangles			& geometry
		, const int											iTriangle
		, const ::gpk::SMatrix4<float>						& matrixTransform
		, const ::gpk::SMatrix4<float>						& matrixTransformView
		, const ::gpk::n3<float>						& lightVector
		, const ::gpk::SColorFloat							& lightColor
		, ::gpk::apod<::gpk::n2<int16_t>>			& pixelCoords
		, ::gpk::apod<::gpk::STriangle<float>>	& pixelVertexWeights
		, ::gpk::view2d<const ::gpk::SColorBGRA>			textureImage
		, ::gpk::apod<::gpk::SLight3>					& lightPoints
		, ::gpk::apod<::gpk::SColorFloat>				& lightColors
		, ::gpk::view2d<uint32_t>						depthBuffer
		);

	int													drawPixels
		( ::gpk::view2d<::gpk::SColorBGRA>				targetPixels
		, const ::gpk::STriangle3	<float>					& triangleWorld
		, const ::gpk::n3		<float>					& normal
		, const ::gpk::STriangle2	<float>					& triangleTexCoords
		, const ::gpk::n3		<float>					& lightVector
		, ::gpk::apod<::gpk::n2<int16_t>>			& pixelCoords
		, ::gpk::apod<::gpk::STriangle<float>>	& pixelVertexWeights
		, ::gpk::view2d<const ::gpk::SColorBGRA>			textureImage
		, ::gpk::apod<::gpk::n3<float>>			& lightPoints
		, ::gpk::apod<::gpk::SColorBGRA>				& lightColors
		, const ::std::function<::gpk::error_t(::gpk::view2d<::gpk::SColorBGRA> targetPixels, const ::gpk::n2<int16_t> & pixelCoord, const ::gpk::SColorBGRA & color)> & funcSetPixel
		);

	int													drawPixels
		( ::gpk::view2d<::gpk::SColorBGRA>				targetPixels
		, const ::gpk::STriangle3	<float>					& triangleWorld
		, const ::gpk::n3		<float>					& normal
		, const ::gpk::STriangle2	<float>					& triangleTexCoords
		, const ::gpk::n3		<float>					& lightVector
		, ::gpk::apod<::gpk::n2<int16_t>>			& pixelCoords
		, ::gpk::apod<::gpk::STriangle<float>>	& pixelVertexWeights
		, ::gpk::view2d<const ::gpk::SColorBGRA>			textureImage
		, ::gpk::apod<::gpk::n3<float>>			& lightPoints
		, ::gpk::apod<::gpk::SColorBGRA>				& lightColors
		);
} // namespace

#endif // CED_DRAW_H_29837429837
