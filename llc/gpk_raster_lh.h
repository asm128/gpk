#include "gpk_grid.h"
#include "gpk_matrix.h"
#include "gpk_color.h"
#include "gpk_geometry_lh.h"
#include "gpk_grid_color.h"
#include "gpk_matrix.h"
#include "gpk_matrix2.h"
#include "gpk_matrix3.h"

#include "gpk_array_pod.h"
#include "gpk_line2.h"
#include "gpk_line3.h"
#include "gpk_circle.h"

#include "gpk_functional.h"

GPK_USING_N2();
GPK_USING_N3();


#ifndef CED_DRAW_H_23627
#define CED_DRAW_H_23627

namespace gpk
{
	struct SLight3 {
		::gpk::n3f2_t	Position;
		f2_t			Range;
	};

	int				pixelBlend			(::gpk::g8bgra pixels, ::gpk::n2s1_t position, ::gpk::bgra color);
	int				setPixel			(::gpk::g8bgra pixels, ::gpk::n2s1_t position, ::gpk::bgra color);
	int				drawRectangle		(::gpk::g8bgra pixels, ::gpk::rect2s1_t  rectangle	, ::gpk::bgra color);
	int				drawCircle			(::gpk::g8bgra pixels, ::gpk::circlei16 circle		, ::gpk::bgra color);
	int				drawLine			(::gpk::g8bgra pixels, ::gpk::line2s1_t  line		, ::gpk::bgra color);
	int				drawLine			(::gpk::n2u1_c offscreenMetrics, ::gpk::line2s1_t line, ::gpk::apod<::gpk::n2s1_t> & pixelCoords);
	int				drawLine
		( const ::gpk::n2<u1_t>	offscreenMetrics
		, const ::gpk::line3f2_t		& lineFloat
		, ::gpk::apod<::gpk::n3f2_t>	& pixelCoords
		, ::gpk::grid<u2_t>	depthBuffer
		);
	int				drawTriangle		(::gpk::g8bgra pixels, const ::gpk::tri2s1_t & triangle, ::gpk::bgra color);
	int				drawTriangle		(::gpk::n2u2_t targetSize, const ::gpk::tri3f2_t & triangle, ::gpk::apod<::gpk::n2s1_t> & pixelCoords, ::gpk::apod<::gpk::trif2_t> & proportions, ::gpk::grid<u2_t> depthBuffer);
	int				drawTriangle		(::gpk::n2u2_t targetSize, const ::gpk::tri2f2_t & triangle, ::gpk::apod<::gpk::n2s1_t> & pixelCoords, ::gpk::apod<::gpk::trif2_t> & proportions);
	int				drawQuadTriangle
		( ::gpk::g8bgra					targetPixels
		, const ::gpk::SGeometryQuads	& geometry
		, const int						iTriangle
		, const ::gpk::m4f2_t			& matrixTransform
		, const ::gpk::m4f2_t			& matrixTransformView
		, const ::gpk::n3f2_t			& lightVector
		, const ::gpk::bgra				color
		, ::gpk::apod<::gpk::n2s1_t>	& pixelCoords
		, ::gpk::apod<::gpk::trif2_t>	& pixelVertexWeights
		, ::gpk::grid<u2_t>			depthBuffer
		);
	int				drawQuadTriangle
		( ::gpk::g8bgra						targetPixels
		, const ::gpk::SGeometryQuads		& geometry
		, const int							iTriangle
		, const ::gpk::m4f2_t				& matrixTransform
		, const ::gpk::m4f2_t				& matrixTransformView
		, const ::gpk::n3f2_t				& lightVector
		, ::gpk::apod<::gpk::n2s1_t>		& pixelCoords
		, ::gpk::apod<::gpk::trif2_t>		& pixelVertexWeights
		, ::gpk::gc8bgra					textureImage
		, ::gpk::apod<::gpk::n3f2_t>		& lightPoints
		, ::gpk::a8bgra						& lightColors
		, ::gpk::grid<u2_t>				depthBuffer
		);

	int				drawQuadTriangle
		( ::gpk::g8bgra						targetPixels
		, const ::gpk::SGeometryQuads		& geometry
		, const int							iTriangle
		, const ::gpk::m4f2_t				& matrixTransform
		, const ::gpk::m4f2_t				& matrixTransformView
		, const ::gpk::n3f2_t				& lightVector
		, ::gpk::apod<::gpk::n2s1_t>		& pixelCoords
		, ::gpk::apod<::gpk::trif2_t>		& pixelVertexWeights
		, ::gpk::gc8bgra					textureImage
		, ::gpk::apod<::gpk::n3f2_t>		& lightPoints
		, ::gpk::a8bgra						& lightColors
		, ::gpk::grid<u2_t>				depthBuffer
		, const ::gpk::function<::gpk::err_t(::gpk::g8bgra targetPixels, const ::gpk::n2s1_t & pixelCoord, const ::gpk::bgra & color)> & funcSetPixel
		);

	int				drawQuadTriangle
		( ::gpk::n2u2_c						& targetSize
		, ::gpk::tri3f2_t					triangle
		, const ::gpk::m4f2_t				& matrixTransformView
		, ::gpk::apod<::gpk::n2s1_t>		& pixelCoords
		, ::gpk::apod<::gpk::trif2_t>		& pixelVertexWeights
		, ::gpk::grid<u2_t>				depthBuffer
		);
	int				drawQuadTriangle
		( ::gpk::n2u2_c						& targetSize
		, const ::gpk::tri3f2_t				& triangleScreen
		, ::gpk::apod<::gpk::n2s1_t>		& pixelCoords
		, ::gpk::apod<::gpk::trif2_t>		& pixelVertexWeights
		, ::gpk::grid<u2_t>				depthBuffer
		);
	int				drawTriangle
		( ::gpk::g8bgra						targetPixels
		, const ::gpk::SGeometryTriangles	& geometry
		, const int							iTriangle
		, const ::gpk::m4f2_t				& matrixTransform
		, const ::gpk::m4f2_t				& matrixTransformView
		, const ::gpk::n3f2_t				& lightVector
		, ::gpk::bgra						color
		, ::gpk::apod<::gpk::n2s1_t>		& pixelCoords
		, ::gpk::apod<::gpk::trif2_t>		& pixelVertexWeights
		, ::gpk::grid<u2_t>				depthBuffer
		);
	int				drawTriangle
		( ::gpk::g8bgra						targetPixels
		, const ::gpk::SGeometryTriangles	& geometry
		, const int							iTriangle
		, const ::gpk::m4f2_t				& matrixTransform
		, const ::gpk::m4f2_t				& matrixTransformView
		, const ::gpk::n3f2_t				& lightVector
		, ::gpk::apod<::gpk::n2s1_t>		& pixelCoords
		, ::gpk::apod<::gpk::trif2_t>		& pixelVertexWeights
		, ::gpk::gc8bgra					textureImage
		, ::gpk::grid<u2_t>				depthBuffer
		);
	int				drawTriangle
		( ::gpk::g8bgra						targetPixels
		, const ::gpk::SGeometryTriangles	& geometry
		, const int							iTriangle
		, const ::gpk::m4f2_t				& matrixTransform
		, const ::gpk::m4f2_t				& matrixTransformView
		, const ::gpk::n3f2_t				& lightVector
		, const ::gpk::rgbaf				& lightColor
		, ::gpk::apod<::gpk::n2s1_t>		& pixelCoords
		, ::gpk::apod<::gpk::trif2_t>		& pixelVertexWeights
		, ::gpk::gc8bgra					textureImage
		, ::gpk::apod<::gpk::SLight3>		& lightPoints
		, ::gpk::a8bgra						& lightColors
		, ::gpk::grid<u2_t>				depthBuffer
		);

	int				drawPixels
		( ::gpk::g8bgra						targetPixels
		, const ::gpk::tri3f2_t				& triangleWorld
		, const ::gpk::n3f2_t				& normal
		, const ::gpk::tri2f2_t				& triangleTexCoords
		, const ::gpk::n3f2_t				& lightVector
		, ::gpk::apod<::gpk::n2s1_t>		& pixelCoords
		, ::gpk::apod<::gpk::trif2_t>		& pixelVertexWeights
		, ::gpk::gc8bgra					textureImage
		, ::gpk::apod<::gpk::n3f2_t>		& lightPoints
		, ::gpk::a8bgra						& lightColors
		, const ::gpk::function<::gpk::err_t(::gpk::g8bgra targetPixels, ::gpk::n2s1_c & pixelCoord, const ::gpk::bgra & color)> & funcSetPixel
		);

	int				drawPixels
		( ::gpk::g8bgra						targetPixels
		, const ::gpk::tri3f2_t				& triangleWorld
		, const ::gpk::n3f2_t				& normal
		, const ::gpk::tri2f2_t				& triangleTexCoords
		, const ::gpk::n3f2_t				& lightVector
		, ::gpk::apod<::gpk::n2s1_t>		& pixelCoords
		, ::gpk::apod<::gpk::trif2_t>		& pixelVertexWeights
		, ::gpk::gc8bgra					textureImage
		, ::gpk::apod<::gpk::n3f2_t>		& lightPoints
		, ::gpk::a8bgra						& lightColors
		);
} // namespace

#endif // CED_DRAW_H_23627
