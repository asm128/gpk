#include "gpk_array.h"
#include "gpk_matrix.h"


#ifndef GPK_GEOMETRY_H
#define GPK_GEOMETRY_H

namespace gpk
{
	template <typename _tUnit>
	struct SModelGeometry {
					::gpk::array_pod<::gpk::STriangle3D	<_tUnit>>		Positions									;
					::gpk::array_pod<::gpk::SCoord3		<_tUnit>>		NormalsTriangle								;
					::gpk::array_pod<::gpk::STriangle3D	<_tUnit>>		NormalsVertex								;
					::gpk::array_pod<::gpk::STriangle2D	<_tUnit>>		UVs											;
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
					::gpk::SModelBufferIndexed<::gpk::STriangle3D	<_tAxis>, _tIndex>	NormalsVertex				;
					::gpk::SModelBufferIndexed<::gpk::STriangle2D	<_tAxis>, _tIndex>	UVs							;
					::gpk::array_pod<_tIndex>											PositionRemap				;

	};

	template <typename _tUnit>
	struct SModelPivot {
					::gpk::SCoord3		<_tUnit>						Scale										;
					::gpk::SQuaternion	<_tUnit>						Orientation									;
					::gpk::SCoord3		<_tUnit>						Position									;
	};

	template <typename _tUnit>
	struct SModelTransform {
					::gpk::SMatrix4		<_tUnit>						World										;
					::gpk::SMatrix4		<_tUnit>						WorldInverse								;
	};

	template <typename _tUnit>
	struct SModelBase {
					::gpk::SModelGeometry	<_tUnit>					Geometry									= {};
					::gpk::SModelPivot		<_tUnit>					Pivot										= {};
	};

	// --- Geometry generation: Cube.
				::gpk::error_t										generateCubePositions					(::gpk::array_pod<::gpk::STriangle3D<float>>& out_Positions	);
				::gpk::error_t										generateCubeNormalsTriangle				(::gpk::array_pod<::gpk::SCoord3	<float>>& out_Normals	);
				::gpk::error_t										generateCubeNormalsVertex				(::gpk::array_pod<::gpk::STriangle3D<float>>& out_Normals	);
				::gpk::error_t										generateCubeUV							(::gpk::array_pod<::gpk::STriangle2D<float>>& out_UV		);
				::gpk::error_t										generateCubeGeometry
					( ::gpk::array_pod<::gpk::STriangle3D	<float>> & out_Positions
					, ::gpk::array_pod<::gpk::SCoord3		<float>> & out_Normals
					, ::gpk::array_pod<::gpk::STriangle3D	<float>> & out_NormalsVertex
					, ::gpk::array_pod<::gpk::STriangle2D	<float>> & out_UV
					);
				static inline ::gpk::error_t						generateCubeGeometry					(::gpk::SModelGeometry<float>& out_Geometry)	{
					return ::gpk::generateCubeGeometry
						( out_Geometry.Positions
						, out_Geometry.NormalsTriangle
						, out_Geometry.NormalsVertex
						, out_Geometry.UVs
						);
				}

				::gpk::error_t										generateGridPositions					(const ::gpk::SCoord2<uint32_t> & gridMetrics, ::gpk::array_pod<::gpk::STriangle3D	<float>>& out_Positions	);
				::gpk::error_t										generateGridNormalsTriangle				(const ::gpk::SCoord2<uint32_t> & gridMetrics, ::gpk::array_pod<::gpk::SCoord3		<float>>& out_Normals	);
				::gpk::error_t										generateGridNormalsVertex				(const ::gpk::SCoord2<uint32_t> & gridMetrics, ::gpk::array_pod<::gpk::STriangle3D	<float>>& out_Normals	);
				::gpk::error_t										generateGridUV							(const ::gpk::SCoord2<uint32_t> & gridMetrics, ::gpk::array_pod<::gpk::STriangle2D	<float>>& out_UV		);
				::gpk::error_t										generateGridGeometry
					( const ::gpk::SCoord2<uint32_t>					& gridMetrics
					, ::gpk::array_pod<::gpk::STriangle3D	<float>>	& out_Positions
					, ::gpk::array_pod<::gpk::SCoord3		<float>>	& out_Normals
					, ::gpk::array_pod<::gpk::STriangle3D	<float>>	& out_NormalsVertex
					, ::gpk::array_pod<::gpk::STriangle2D	<float>>	& out_UV
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
} // namespace

#endif // GPK_GEOMETRY_H
