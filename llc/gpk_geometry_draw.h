#include "gpk_geometry.h"

#include "gpk_tri.h"
#include "gpk_color.h"
#include "gpk_matrix.h"
#include "gpk_raster_lh.h"

#ifndef GPK_ENGINE_GEOMETRY_DRAW_H_23610
#define GPK_ENGINE_GEOMETRY_DRAW_H_23610

namespace gpk
{
	::gpk::error_t	drawTriangle
		( ::gpk::g8bgra		targetPixels
		, const ::gpk::STrianglesIndexed	& geometry
		, const int							iTriangle
		, const ::gpk::m4f32				& matrixTransform
		, const ::gpk::m4f32				& matrixTransformView
		, const ::gpk::n3f32				& lightVector
		, const ::gpk::rgbaf				& lightColor
		, ::gpk::apod<::gpk::n2i16>			& pixelCoords
		, ::gpk::apod<::gpk::trif32>		& pixelVertexWeights
		, ::gpk::gc8bgra	textureImage
		, ::gpk::apod<::gpk::SLight3>		& lightPoints
		, ::gpk::apod<::gpk::rgbaf>			& lightColors
		, ::gpk::grid<uint32_t>			depthBuffer
		);

}

#endif // GPK_ENGINE_GEOMETRY_DRAW_H_23610
