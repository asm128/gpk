#include "gpk_voxel.h"
#include "gpk_geometry_lh.h"

#ifndef GPK_VOXEL_GEOMETRY_H_098273498237423
#define GPK_VOXEL_GEOMETRY_H_098273498237423

namespace gpk
{
	struct STrianglesIndexed {
		::gpk::apod<::gpk::n3f32>	Positions;
		::gpk::apod<::gpk::n3f32>	Normals;
		::gpk::apod<::gpk::n2f32>	TextureCoords;
		::gpk::au32					PositionIndices;
	};

	struct SVoxelGeometry {
		::gpk::STrianglesIndexed					Geometry			= {};
		::gpk::au32									VoxelIndices		= {};
		::gpk::au32									FaceIndices			= {};
		::gpk::apod<::gpk::SRenderMaterialPaletted>	Materials			= {};
		::gpk::apod<::gpk::SGeometryGroup>			GeometrySlices	[6]	= {};
		::gpk::SAABBGeometry						AABBModel			= {};
		::gpk::SAABBGeometry						AABBSlices		[6]	= {};
	};

	::gpk::error_t									geometryVoxelModel	(::gpk::SVoxelGeometry & output, const ::gpk::SVoxelMap<uint8_t> & voxelMap);

} // namespace

#endif // GPK_VOXEL_GEOMETRY_H_098273498237423