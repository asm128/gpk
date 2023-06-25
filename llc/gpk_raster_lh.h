#include "gpk_grid.h"
#include "gpk_matrix.h"
#include "gpk_color.h"
#include "gpk_geometry_lh.h"
#include "gpk_grid_color.h"

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

	int				pixelBlend			(::gpk::g8bgra pixels, ::gpk::n2i16 position, ::gpk::bgra color);
	int				setPixel			(::gpk::g8bgra pixels, ::gpk::n2i16 position, ::gpk::bgra color);
	int				drawRectangle		(::gpk::g8bgra pixels, ::gpk::rect2i16  rectangle	, ::gpk::bgra color);
	int				drawCircle			(::gpk::g8bgra pixels, ::gpk::circlei16 circle		, ::gpk::bgra color);
	int				drawLine			(::gpk::g8bgra pixels, ::gpk::line2i16  line		, ::gpk::bgra color);
	int				drawLine			(const ::gpk::n2u16 offscreenMetrics, ::gpk::line2i16 line, ::gpk::apod<::gpk::n2i16> & pixelCoords);
	int				drawLine
		( const ::gpk::n2<uint16_t>	offscreenMetrics
		, const ::gpk::line3f32		& lineFloat
		, ::gpk::apod<::gpk::n3f32>	& pixelCoords
		, ::gpk::grid<uint32_t>	depthBuffer
		);
	int				drawTriangle		(::gpk::g8bgra pixels, const ::gpk::tri2i16 & triangle, ::gpk::bgra color);
	int				drawTriangle		(::gpk::n2u32 targetSize, const ::gpk::tri3f32 & triangle, ::gpk::apod<::gpk::n2i16> & pixelCoords, ::gpk::apod<::gpk::trif32> & proportions, ::gpk::grid<uint32_t> depthBuffer);
	int				drawTriangle		(::gpk::n2u32 targetSize, const ::gpk::tri2f32 & triangle, ::gpk::apod<::gpk::n2i16> & pixelCoords, ::gpk::apod<::gpk::trif32> & proportions);
	int				drawQuadTriangle
		( ::gpk::g8bgra	targetPixels
		, const ::gpk::SGeometryQuads	& geometry
		, const int						iTriangle
		, const ::gpk::m4f32			& matrixTransform
		, const ::gpk::m4f32			& matrixTransformView
		, const ::gpk::n3f32			& lightVector
		, const ::gpk::bgra				color
		, ::gpk::apod<::gpk::n2i16>		& pixelCoords
		, ::gpk::apod<::gpk::trif32>	& pixelVertexWeights
		, ::gpk::grid<uint32_t>		depthBuffer
		);
	int				drawQuadTriangle
		( ::gpk::g8bgra		targetPixels
		, const ::gpk::SGeometryQuads		& geometry
		, const int							iTriangle
		, const ::gpk::m4f32				& matrixTransform
		, const ::gpk::m4f32				& matrixTransformView
		, const ::gpk::n3f32				& lightVector
		, ::gpk::apod<::gpk::n2i16>			& pixelCoords
		, ::gpk::apod<::gpk::trif32>		& pixelVertexWeights
		, ::gpk::gc8bgra	textureImage
		, ::gpk::apod<::gpk::n3f32>			& lightPoints
		, ::gpk::a8bgra			& lightColors
		, ::gpk::grid<uint32_t>			depthBuffer
		);

	int				drawQuadTriangle
		( ::gpk::g8bgra		targetPixels
		, const ::gpk::SGeometryQuads		& geometry
		, const int							iTriangle
		, const ::gpk::m4f32				& matrixTransform
		, const ::gpk::m4f32				& matrixTransformView
		, const ::gpk::n3f32				& lightVector
		, ::gpk::apod<::gpk::n2i16>			& pixelCoords
		, ::gpk::apod<::gpk::trif32>		& pixelVertexWeights
		, ::gpk::gc8bgra	textureImage
		, ::gpk::apod<::gpk::n3f32>			& lightPoints
		, ::gpk::a8bgra			& lightColors
		, ::gpk::grid<uint32_t>			depthBuffer
		, const ::std::function<::gpk::error_t(::gpk::g8bgra targetPixels, const ::gpk::n2i16 & pixelCoord, const ::gpk::bgra & color)> & funcSetPixel
		);

	int				drawQuadTriangle
		( const ::gpk::n2u32			& targetSize
		, ::gpk::tri3f32				triangle
		, const ::gpk::m4f32			& matrixTransformView
		, ::gpk::apod<::gpk::n2i16>		& pixelCoords
		, ::gpk::apod<::gpk::trif32>	& pixelVertexWeights
		, ::gpk::grid<uint32_t>		depthBuffer
		);
	int				drawQuadTriangle
		( const ::gpk::n2u32			& targetSize
		, const ::gpk::tri3f32			& triangleScreen
		, ::gpk::apod<::gpk::n2i16>		& pixelCoords
		, ::gpk::apod<::gpk::trif32>	& pixelVertexWeights
		, ::gpk::grid<uint32_t>		depthBuffer
		);
	int				drawTriangle
		( ::gpk::g8bgra		targetPixels
		, const ::gpk::SGeometryTriangles	& geometry
		, const int							iTriangle
		, const ::gpk::m4f32				& matrixTransform
		, const ::gpk::m4f32				& matrixTransformView
		, const ::gpk::n3f32				& lightVector
		, ::gpk::bgra						color
		, ::gpk::apod<::gpk::n2i16>			& pixelCoords
		, ::gpk::apod<::gpk::trif32>		& pixelVertexWeights
		, ::gpk::grid<uint32_t>			depthBuffer
		);
	int				drawTriangle
		( ::gpk::g8bgra		targetPixels
		, const ::gpk::SGeometryTriangles	& geometry
		, const int							iTriangle
		, const ::gpk::m4f32				& matrixTransform
		, const ::gpk::m4f32				& matrixTransformView
		, const ::gpk::n3f32				& lightVector
		, ::gpk::apod<::gpk::n2i16>			& pixelCoords
		, ::gpk::apod<::gpk::trif32>		& pixelVertexWeights
		, ::gpk::gc8bgra	textureImage
		, ::gpk::grid<uint32_t>			depthBuffer
		);
	int				drawTriangle
		( ::gpk::g8bgra		targetPixels
		, const ::gpk::SGeometryTriangles	& geometry
		, const int							iTriangle
		, const ::gpk::m4f32				& matrixTransform
		, const ::gpk::m4f32				& matrixTransformView
		, const ::gpk::n3f32				& lightVector
		, const ::gpk::rgbaf				& lightColor
		, ::gpk::apod<::gpk::n2i16>			& pixelCoords
		, ::gpk::apod<::gpk::trif32>		& pixelVertexWeights
		, ::gpk::gc8bgra	textureImage
		, ::gpk::apod<::gpk::SLight3>		& lightPoints
		, ::gpk::a8bgra			& lightColors
		, ::gpk::grid<uint32_t>			depthBuffer
		);

	int				drawPixels
		( ::gpk::g8bgra		targetPixels
		, const ::gpk::tri3f32				& triangleWorld
		, const ::gpk::n3f32				& normal
		, const ::gpk::tri2f32				& triangleTexCoords
		, const ::gpk::n3f32				& lightVector
		, ::gpk::apod<::gpk::n2i16>			& pixelCoords
		, ::gpk::apod<::gpk::trif32>		& pixelVertexWeights
		, ::gpk::gc8bgra	textureImage
		, ::gpk::apod<::gpk::n3f32>			& lightPoints
		, ::gpk::a8bgra			& lightColors
		, const ::std::function<::gpk::error_t(::gpk::g8bgra targetPixels, const ::gpk::n2i16 & pixelCoord, const ::gpk::bgra & color)> & funcSetPixel
		);

	int				drawPixels
		( ::gpk::g8bgra		targetPixels
		, const ::gpk::tri3f32				& triangleWorld
		, const ::gpk::n3f32				& normal
		, const ::gpk::tri2f32				& triangleTexCoords
		, const ::gpk::n3f32				& lightVector
		, ::gpk::apod<::gpk::n2i16>			& pixelCoords
		, ::gpk::apod<::gpk::trif32>		& pixelVertexWeights
		, ::gpk::gc8bgra	textureImage
		, ::gpk::apod<::gpk::n3f32>			& lightPoints
		, ::gpk::a8bgra			& lightColors
		);
} // namespace

#endif // CED_DRAW_H_29837429837
