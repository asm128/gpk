#include "gpk_array.h"
#include "gpk_matrix.h"
#include "gpk_tri2.h"

#ifndef GPK_GEOMETRY_H
#define GPK_GEOMETRY_H

namespace gpk
{
	template <typename _tUnit>
	struct SModelGeometry {
		::gpk::apod<::gpk::tri3<_tUnit>>	Positions									;
		::gpk::apod<::gpk::n3  <_tUnit>>	NormalsTriangle								;
		::gpk::apod<::gpk::tri3<_tUnit>>	NormalsVertex								;
		::gpk::apod<::gpk::tri2<_tUnit>>	UVs											;
	};

	template <typename _tElement, typename _tIndex>
	struct SModelBufferIndexed {
		::gpk::apod<_tIndex>				Indices										;
		::gpk::apod<_tElement>				Values										;
	};

	template <typename _tAxis, typename _tIndex>
	struct SModelGeometryIndexed {
		::gpk::SModelBufferIndexed<::gpk::n3	<_tAxis>, _tIndex>	Positions					;
		::gpk::SModelBufferIndexed<::gpk::n3	<_tAxis>, _tIndex>	NormalsTriangle				;
		::gpk::SModelBufferIndexed<::gpk::tri3	<_tAxis>, _tIndex>	NormalsVertex				;
		::gpk::SModelBufferIndexed<::gpk::tri2	<_tAxis>, _tIndex>	UVs							;
		::gpk::apod<_tIndex>										PositionRemap				;

	};

	template <typename _tUnit>
	struct SModelPivot {
		::gpk::n3	<_tUnit>						Scale										;
		::gpk::quat	<_tUnit>						Orientation									;
		::gpk::n3	<_tUnit>						Position									;
	};

	// --- Geometry generation: Cube.
	::gpk::error_t										generateCubePositions					(::gpk::apod<::gpk::tri3f32> & out_Positions	);
	::gpk::error_t										generateCubeNormalsTriangle				(::gpk::apod<::gpk::n3f32  > & out_Normals	);
	::gpk::error_t										generateCubeNormalsVertex				(::gpk::apod<::gpk::tri3f32> & out_Normals	);
	::gpk::error_t										generateCubeUV							(::gpk::apod<::gpk::tri2f32> & out_UV		);
	::gpk::error_t										generateCubeGeometry
		( ::gpk::apod<::gpk::tri3f32> & out_Positions
		, ::gpk::apod<::gpk::n3f32  > & out_Normals
		, ::gpk::apod<::gpk::tri3f32> & out_NormalsVertex
		, ::gpk::apod<::gpk::tri2f32> & out_UV
		);
	stainli	::gpk::error_t			generateCubeGeometry					(::gpk::SModelGeometry<float>& out_Geometry)	{
		return ::gpk::generateCubeGeometry
			( out_Geometry.Positions
			, out_Geometry.NormalsTriangle
			, out_Geometry.NormalsVertex
			, out_Geometry.UVs
			);
	}

	::gpk::error_t					generateGridPositions					(const ::gpk::n2u16 & gridMetrics, ::gpk::apod<::gpk::tri3f32> & out_Positions	);
	::gpk::error_t					generateGridNormalsTriangle				(const ::gpk::n2u16 & gridMetrics, ::gpk::apod<::gpk::n3f32  > & out_Normals	);
	::gpk::error_t					generateGridNormalsVertex				(const ::gpk::n2u16 & gridMetrics, ::gpk::apod<::gpk::tri3f32> & out_Normals	);
	::gpk::error_t					generateGridUV			(const ::gpk::n2u16 & gridMetrics, ::gpk::apod<::gpk::tri2f32> & out_UV		);
	::gpk::error_t					generateGridGeometry
		( const ::gpk::n2u16			& gridMetrics
		, ::gpk::apod<::gpk::tri3f32>	& out_Positions
		, ::gpk::apod<::gpk::n3f32  >	& out_Normals
		, ::gpk::apod<::gpk::tri3f32>	& out_NormalsVertex
		, ::gpk::apod<::gpk::tri2f32>	& out_UV
		);
	stainli	::gpk::error_t			generateGridGeometry					(const ::gpk::n2u16	& gridMetrics, ::gpk::SModelGeometry<float>& out_Geometry)	{
		return ::gpk::generateGridGeometry
			( gridMetrics
			, out_Geometry.Positions
			, out_Geometry.NormalsTriangle
			, out_Geometry.NormalsVertex
			, out_Geometry.UVs
			);
	}


	struct SRenderCache {
		::gpk::apod<::gpk::n2i16>		TrianglePixelCoords					= {};
		::gpk::apod<::gpk::trif32>		TrianglePixelWeights				= {};
		::gpk::apod<::gpk::n2i16>		WireframePixelCoords				= {};

		::gpk::apod<::gpk::tri3f32>		TransformedNormalsVertex			= {};

		::gpk::ai32						Triangle3dIndices					= {};
		::gpk::apod<::gpk::tri3f32>		Triangle3dToDraw					= {};
		::gpk::apod<::gpk::tri3f32>		Triangle3dWorld						= {};

		uint32_t						TrianglesDrawn						= 0;
		uint32_t						PixelsDrawn							= 0;
		uint32_t						PixelsSkipped						= 0;
	};

} // namespace

#endif // GPK_GEOMETRY_H
