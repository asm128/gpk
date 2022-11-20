#include "gpk_voxel.h"
#include "gpk_geometry_lh.h"

#ifndef GPK_VOXEL_GEOMETRY_H_098273498237423
#define GPK_VOXEL_GEOMETRY_H_098273498237423

namespace gpk
{
	struct SVoxelGeometry {
		::gpk::SGeometryIndexedTriangles			Geometry			= {};
		::gpk::array_pod<uint32_t>					VoxelIndices		= {};
		::gpk::array_pod<uint32_t>					FaceIndices			= {};
		::gpk::array_pod<::gpk::SRenderMaterial>	Materials			= {};
		::gpk::array_pod<::gpk::SGeometryGroup>		GeometrySlices	[6]	= {};
		::gpk::SAABBGeometry						AABBModel			= {};
		::gpk::SAABBGeometry						AABBSlices		[6]	= {};
	};

	::gpk::error_t									geometryVoxelModel	(::gpk::SVoxelGeometry & output, const ::gpk::SVoxelMap<uint8_t> & voxelMap);

} // namespace

#endif // GPK_VOXEL_GEOMETRY_H_098273498237423