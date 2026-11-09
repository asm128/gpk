#include "gpk_geometry.h"

#include "gpk_tri.h"
#include "gpk_color.h"
#include "gpk_matrix.h"
#include "gpk_raster_lh.h"

#ifndef GPK_ENGINE_GEOMETRY_DRAW_H_23627
#define GPK_ENGINE_GEOMETRY_DRAW_H_23627

nmsp gpk
{
	gpk::err_t drawTriangle
		( g8bgra					targetPixels
		, const SGeometryBuffers	& geometry
		, const u2_t				iTriangle
		, const m4f2_t				& matrixTransform
		, const m4f2_t				& matrixTransformView
		, const n3f2_t				& lightVector
		, const rgbaf				& lightColor
		, apod<n2s1_t>				& pixelCoords
		, apod<trif2_t>				& pixelVertexWeights
		, gc8bgra					textureImage
		, apod<SLight3>				& lightPoints
		, apod<rgbaf>				& lightColors
		, grid<u2_t>				depthBuffer
		);
}

#endif // GPK_ENGINE_GEOMETRY_DRAW_H_23627
