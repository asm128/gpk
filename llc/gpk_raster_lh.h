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

nmsp gpk
{
	struct SLight3 {
		n3f2_t	Position;
		f2_t			Range;
	};

	int				pixelBlend			(g8bgra pixels, n2s1_t position, bgra color);
	int				setPixel			(g8bgra pixels, n2s1_t position, bgra color);
	int				drawRectangle		(g8bgra pixels, rect2s1_t  rectangle	, bgra color);
	int				drawCircle			(g8bgra pixels, circle<s1_t> circle		, bgra color);
	int				drawLine			(g8bgra pixels, line2s1_t  line		, bgra color);
	int				drawLine			(n2u1_c offscreenMetrics, line2s1_t line, apod<n2s1_t> & pixelCoords);
	int				drawLine
		( const n2<u1_t>	offscreenMetrics
		, const line3f2_t		& lineFloat
		, apod<n3f2_t>	& pixelCoords
		, grid<u2_t>	depthBuffer
		);
	int				drawTriangle		(g8bgra pixels, const tri2s1_t & triangle, bgra color);
	int				drawTriangle		(n2u2_t targetSize, const tri3f2_t & triangle, apod<n2s1_t> & pixelCoords, apod<trif2_t> & proportions, grid<u2_t> depthBuffer);
	int				drawTriangle		(n2u2_t targetSize, const tri2f2_t & triangle, apod<n2s1_t> & pixelCoords, apod<trif2_t> & proportions);
	int				drawQuadTriangle
		( g8bgra					targetPixels
		, const SGeometryQuads	& geometry
		, const int						iTriangle
		, const m4f2_t			& matrixTransform
		, const m4f2_t			& matrixTransformView
		, const n3f2_t			& lightVector
		, const bgra				color
		, apod<n2s1_t>	& pixelCoords
		, apod<trif2_t>	& pixelVertexWeights
		, grid<u2_t>			depthBuffer
		);
	int				drawQuadTriangle
		( g8bgra						targetPixels
		, const SGeometryQuads		& geometry
		, const int							iTriangle
		, const m4f2_t				& matrixTransform
		, const m4f2_t				& matrixTransformView
		, const n3f2_t				& lightVector
		, apod<n2s1_t>		& pixelCoords
		, apod<trif2_t>		& pixelVertexWeights
		, gc8bgra					textureImage
		, apod<n3f2_t>		& lightPoints
		, a8bgra						& lightColors
		, grid<u2_t>				depthBuffer
		);

	int				drawQuadTriangle
		( g8bgra						targetPixels
		, const SGeometryQuads		& geometry
		, const int							iTriangle
		, const m4f2_t				& matrixTransform
		, const m4f2_t				& matrixTransformView
		, const n3f2_t				& lightVector
		, apod<n2s1_t>		& pixelCoords
		, apod<trif2_t>		& pixelVertexWeights
		, gc8bgra					textureImage
		, apod<n3f2_t>		& lightPoints
		, a8bgra						& lightColors
		, grid<u2_t>				depthBuffer
		, const function<err_t(g8bgra targetPixels, const n2s1_t & pixelCoord, const bgra & color)> & funcSetPixel
		);

	int				drawQuadTriangle
		( n2u2_c						& targetSize
		, tri3f2_t					triangle
		, const m4f2_t				& matrixTransformView
		, apod<n2s1_t>		& pixelCoords
		, apod<trif2_t>		& pixelVertexWeights
		, grid<u2_t>				depthBuffer
		);
	int				drawQuadTriangle
		( n2u2_c						& targetSize
		, const tri3f2_t				& triangleScreen
		, apod<n2s1_t>		& pixelCoords
		, apod<trif2_t>		& pixelVertexWeights
		, grid<u2_t>				depthBuffer
		);
	int				drawTriangle
		( g8bgra						targetPixels
		, const SGeometryTriangles	& geometry
		, const int							iTriangle
		, const m4f2_t				& matrixTransform
		, const m4f2_t				& matrixTransformView
		, const n3f2_t				& lightVector
		, bgra						color
		, apod<n2s1_t>		& pixelCoords
		, apod<trif2_t>		& pixelVertexWeights
		, grid<u2_t>				depthBuffer
		);
	int				drawTriangle
		( g8bgra						targetPixels
		, const SGeometryTriangles	& geometry
		, const int							iTriangle
		, const m4f2_t				& matrixTransform
		, const m4f2_t				& matrixTransformView
		, const n3f2_t				& lightVector
		, apod<n2s1_t>		& pixelCoords
		, apod<trif2_t>		& pixelVertexWeights
		, gc8bgra					textureImage
		, grid<u2_t>				depthBuffer
		);
	int				drawTriangle
		( g8bgra						targetPixels
		, const SGeometryTriangles	& geometry
		, const int							iTriangle
		, const m4f2_t				& matrixTransform
		, const m4f2_t				& matrixTransformView
		, const n3f2_t				& lightVector
		, const rgbaf				& lightColor
		, apod<n2s1_t>		& pixelCoords
		, apod<trif2_t>		& pixelVertexWeights
		, gc8bgra					textureImage
		, apod<SLight3>		& lightPoints
		, a8bgra						& lightColors
		, grid<u2_t>				depthBuffer
		);

	int				drawPixels
		( g8bgra						targetPixels
		, const tri3f2_t				& triangleWorld
		, const n3f2_t				& normal
		, const tri2f2_t				& triangleTexCoords
		, const n3f2_t				& lightVector
		, apod<n2s1_t>		& pixelCoords
		, apod<trif2_t>		& pixelVertexWeights
		, gc8bgra					textureImage
		, apod<n3f2_t>		& lightPoints
		, a8bgra						& lightColors
		, const function<err_t(g8bgra targetPixels, n2s1_c & pixelCoord, const bgra & color)> & funcSetPixel
		);

	int				drawPixels
		( g8bgra						targetPixels
		, const tri3f2_t				& triangleWorld
		, const n3f2_t				& normal
		, const tri2f2_t				& triangleTexCoords
		, const n3f2_t				& lightVector
		, apod<n2s1_t>		& pixelCoords
		, apod<trif2_t>		& pixelVertexWeights
		, gc8bgra					textureImage
		, apod<n3f2_t>		& lightPoints
		, a8bgra						& lightColors
		);
} // nmsp

#endif // CED_DRAW_H_23627
