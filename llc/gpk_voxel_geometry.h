#include "gpk_voxel.h"
#include "gpk_geometry_lh.h"

#ifndef GPK_VOXEL_GEOMETRY_H_098273498237423
#define GPK_VOXEL_GEOMETRY_H_098273498237423

namespace gpk
{
	struct SVoxelGeometry {
		::gpk::SGeometryIndexedTriangles					Geometry			= {};
		::gpk::apod<uint32_t>							VoxelIndices		= {};
		::gpk::apod<uint32_t>							FaceIndices			= {};
		::gpk::apod<::gpk::SRenderMaterialPaletted>	Materials			= {};
		::gpk::apod<::gpk::SGeometryGroup>				GeometrySlices	[6]	= {};
		::gpk::SAABBGeometry								AABBModel			= {};
		::gpk::SAABBGeometry								AABBSlices		[6]	= {};
	};

	::gpk::error_t									geometryVoxelModel	(::gpk::SVoxelGeometry & output, const ::gpk::SVoxelMap<uint8_t> & voxelMap);

} // namespace

#endif // GPK_VOXEL_GEOMETRY_H_098273498237423