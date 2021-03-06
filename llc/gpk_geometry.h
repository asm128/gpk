#include "gpk_array.h"
#include "gpk_matrix.h"


#ifndef GPK_GEOMETRY_H
#define GPK_GEOMETRY_H

namespace gpk
{
	template <typename _tUnit>
	struct SModelGeometry {
					::gpk::array_pod<::gpk::STriangle3	<_tUnit>>		Positions									;
					::gpk::array_pod<::gpk::SCoord3		<_tUnit>>		NormalsTriangle								;
					::gpk::array_pod<::gpk::STriangle3	<_tUnit>>		NormalsVertex								;
					::gpk::array_pod<::gpk::STriangle2	<_tUnit>>		UVs											;
	};

	template <typename _tElement, typename _tIndex>
	struct SModelBufferIndexed {
					::gpk::array_pod<_tIndex>							Indices										;
					::gpk::array_pod<_tElement>							Values										;
	};

	template <typename _tAxis, typename _tIndex>
	struct SModelGeometryIndexed {
					::gpk::SModelBufferIndexed<::gpk::SCoord3		<_tAxis>, _tIndex>	Positions					;
					::gpk::SModelBufferIndexed<::gpk::SCoord3		<_tAxis>, _tIndex>	NormalsTriangle				;
					::gpk::SModelBufferIndexed<::gpk::STriangle3	<_tAxis>, _tIndex>	NormalsVertex				;
					::gpk::SModelBufferIndexed<::gpk::STriangle2	<_tAxis>, _tIndex>	UVs							;
					::gpk::array_pod<_tIndex>											PositionRemap				;

	};

	template <typename _tUnit>
	struct SModelPivot {
					::gpk::SCoord3		<_tUnit>						Scale										;
					::gpk::SQuaternion	<_tUnit>						Orientation									;
					::gpk::SCoord3		<_tUnit>						Position									;
	};

	// --- Geometry generation: Cube.
				::gpk::error_t										generateCubePositions					(::gpk::array_pod<::gpk::STriangle3<float>>& out_Positions	);
				::gpk::error_t										generateCubeNormalsTriangle				(::gpk::array_pod<::gpk::SCoord3	<float>>& out_Normals	);
				::gpk::error_t										generateCubeNormalsVertex				(::gpk::array_pod<::gpk::STriangle3<float>>& out_Normals	);
				::gpk::error_t										generateCubeUV							(::gpk::array_pod<::gpk::STriangle2<float>>& out_UV		);
				::gpk::error_t										generateCubeGeometry
					( ::gpk::array_pod<::gpk::STriangle3	<float>> & out_Positions
					, ::gpk::array_pod<::gpk::SCoord3		<float>> & out_Normals
					, ::gpk::array_pod<::gpk::STriangle3	<float>> & out_NormalsVertex
					, ::gpk::array_pod<::gpk::STriangle2	<float>> & out_UV
					);
				static inline ::gpk::error_t						generateCubeGeometry					(::gpk::SModelGeometry<float>& out_Geometry)	{
					return ::gpk::generateCubeGeometry
						( out_Geometry.Positions
						, out_Geometry.NormalsTriangle
						, out_Geometry.NormalsVertex
						, out_Geometry.UVs
						);
				}

				::gpk::error_t										generateGridPositions					(const ::gpk::SCoord2<uint32_t> & gridMetrics, ::gpk::array_pod<::gpk::STriangle3	<float>>& out_Positions	);
				::gpk::error_t										generateGridNormalsTriangle				(const ::gpk::SCoord2<uint32_t> & gridMetrics, ::gpk::array_pod<::gpk::SCoord3		<float>>& out_Normals	);
				::gpk::error_t										generateGridNormalsVertex				(const ::gpk::SCoord2<uint32_t> & gridMetrics, ::gpk::array_pod<::gpk::STriangle3	<float>>& out_Normals	);
				::gpk::error_t										generateGridUV							(const ::gpk::SCoord2<uint32_t> & gridMetrics, ::gpk::array_pod<::gpk::STriangle2	<float>>& out_UV		);
				::gpk::error_t										generateGridGeometry
					( const ::gpk::SCoord2<uint32_t>					& gridMetrics
					, ::gpk::array_pod<::gpk::STriangle3	<float>>	& out_Positions
					, ::gpk::array_pod<::gpk::SCoord3		<float>>	& out_Normals
					, ::gpk::array_pod<::gpk::STriangle3	<float>>	& out_NormalsVertex
					, ::gpk::array_pod<::gpk::STriangle2	<float>>	& out_UV
					);
				static inline ::gpk::error_t						generateGridGeometry					(const ::gpk::SCoord2<uint32_t>	& gridMetrics, ::gpk::SModelGeometry<float>& out_Geometry)	{
					return ::gpk::generateGridGeometry
						( gridMetrics
						, out_Geometry.Positions
						, out_Geometry.NormalsTriangle
						, out_Geometry.NormalsVertex
						, out_Geometry.UVs
						);
				}


	struct SRenderCache {
						::gpk::array_pod<::gpk::SCoord2<int32_t>>						TrianglePixelCoords					= {};
						::gpk::array_pod<::gpk::STriangleWeights<double>>				TrianglePixelWeights				= {};
						::gpk::array_pod<::gpk::SCoord2<int32_t>>						WireframePixelCoords				= {};

						::gpk::array_pod<::gpk::STriangle3<float>>						TransformedNormalsVertex			= {};

						::gpk::array_pod<int32_t>										Triangle3dIndices					= {};
						::gpk::array_pod<::gpk::STriangle3<float>>						Triangle3dToDraw					= {};
						::gpk::array_pod<::gpk::STriangle3<float>>						Triangle3dWorld						= {};

						uint32_t														TrianglesDrawn						= 0;
						uint32_t														PixelsDrawn							= 0;
						uint32_t														PixelsSkipped						= 0;
	};

} // namespace

#endif // GPK_GEOMETRY_H
