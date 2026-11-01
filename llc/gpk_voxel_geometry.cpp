#include "gpk_voxel_geometry.h"

GPK_USING_TYPEINT();


static	gpk::err_t geometryVoxelFace
	( ::gpk::SGeometryBuffers						& geometry
	, const ::gpk::n3f2_t							& voxelPos
	, const ::gpk::view<const ::gpk::n3f2_t>	& rawVertices
	, const ::gpk::view<const u0_t>				& rawIndices
	) {
	::gpk::n3f2_t				vertices [4]				= {}; 
	u2_t					indices  [6]				= {}; 
	u2_t					offsetVertex				= geometry.Positions.size();
	for(u2_t iVertex = 0; iVertex < 4; ++iVertex)
		vertices[iVertex] = rawVertices[iVertex] + voxelPos; 

	for(u2_t iIndex = 0; iIndex < 6; ++iIndex)
		indices[iIndex] = rawIndices[iIndex] + offsetVertex;

	if_fail_fe(geometry.Positions			.append(vertices));
	if_fail_fe(geometry.PositionIndices	.append(indices));
	return 0;
}

gpk::err_t 		gpk::geometryVoxelModel		(::gpk::SVoxelGeometry & output, const ::gpk::SVoxelMap<u0_t> & voxelMap) {
	const ::gpk::n3<u0_t>	dimensions				= voxelMap.Dimensions;

	::gpk::SGeometryBuffers	& geometry				= output.Geometry;
	::gpk::apod<::gpk::SRenderMaterialPaletted>		& materials				= output.Materials;

	::gpk::SAABBGeometry		& aabbModel				= output.AABBModel;
	for(u2_t i = 0; i < 8; ++i) {
		aabbModel.Vertices[i]								= ::gpk::VOXEL_VERTICES[i];
		aabbModel.Vertices[i].Scale(dimensions.f2());
	}

	::gpk::view<const ::gpk::bgra>	rgba					= voxelMap.Palette;
	if(0 == rgba.size())
		rgba				= ::gpk::VOXEL_PALETTE;

	const ::gpk::view<const ::gpk::SVoxel<u0_t>>	voxels					= voxelMap.Voxels;
	for(u2_t iFace = 0; iFace < 6; ++iFace) {
		::gpk::apod<::gpk::SGeometryGroup>	& faceSlices			= output.GeometrySlices[iFace];
		::gpk::SAABBGeometry				& aabbSlice				= output.AABBSlices[iFace];

		const ::gpk::n3s0_t					faceDelta				= ::gpk::VOXEL_DELTAS			[iFace];
		::gpk::vcu0_t						rawIndices				= ::gpk::VOXEL_FACE_INDICES		[iFace];
		::gpk::view<const ::gpk::n3f2_t>	rawVertices				= {&::gpk::VOXEL_FACE_VERTICES	[iFace].A, 4};
		::gpk::view<const ::gpk::n2f2_t>	rawTexCoord				= {&::gpk::VOXEL_FACE_UV		[iFace].A, 4};

		geometry.Normals.push_back(::gpk::VOXEL_NORMALS[iFace]);
		geometry.TextureCoords.append(rawTexCoord);
		for(u2_t iValue = 0; iValue < rgba.size(); ++iValue) {
			::gpk::SGeometryGroup				newGroup					= {};
			newGroup.Geometry				= 0;
			newGroup.Image					= iValue;
			newGroup.Material				= iValue;
			newGroup.Modes.IndexMode		= ::gpk::INDEX_MODE_LIST;
			newGroup.Modes.PrimitiveType	= ::gpk::PRIMITIVE_TYPE_TRIANGLE;
			newGroup.Slice.Offset			= (u1_t)geometry.PositionIndices.size();
			::gpk::minmax<::gpk::n3f2_t>			aabbLimits					= {};

			for(u2_t iVoxel = 0; iVoxel < voxels.size(); ++iVoxel) {
				const ::gpk::SVoxel<u0_t>		voxel						= voxels[iVoxel];
				if(voxel.ColorIndex != iValue)
					continue;

				aabbLimits.Min.x = ::gpk::min(aabbLimits.Min.x, (f2_t)voxel.Position.x);
				aabbLimits.Min.y = ::gpk::min(aabbLimits.Min.y, (f2_t)voxel.Position.y);
				aabbLimits.Min.z = ::gpk::min(aabbLimits.Min.z, (f2_t)voxel.Position.z);
				aabbLimits.Max.x = ::gpk::max(aabbLimits.Max.x, (f2_t)voxel.Position.x);
				aabbLimits.Max.y = ::gpk::max(aabbLimits.Max.y, (f2_t)voxel.Position.y);
				aabbLimits.Max.z = ::gpk::max(aabbLimits.Max.z, (f2_t)voxel.Position.z);

				::gpk::bgra							color						= rgba[iValue ? iValue - 1 : 0];
				u0_t								adjacentValue				= {};
				voxelMap.GetValue((voxel.Position.s2() + faceDelta.s2()).u0(), adjacentValue);
				bool								renderFace					= 0 == adjacentValue || color.a < 0xFF;
				if(false == renderFace)
					continue;

				::geometryVoxelFace(geometry, voxel.Position.f2(), rawVertices, rawIndices);
				if_fail_fe(output.FaceIndices.push_back(iFace));
				if_fail_fe(output.VoxelIndices.push_back(iVoxel));
			}
			newGroup.Slice.Count						= geometry.PositionIndices.size() / 3;
			if(newGroup.Slice.Count) {
				faceSlices.push_back(newGroup);
				materials.push_back({rgba[iValue], (u1_t)iValue});
				// -- top
				aabbSlice.Vertices[0] = {aabbLimits.Min.x, aabbLimits.Max.y, aabbLimits.Min.z};
				aabbSlice.Vertices[1] = {aabbLimits.Max.x, aabbLimits.Max.y, aabbLimits.Min.z};
				aabbSlice.Vertices[2] = {aabbLimits.Min.x, aabbLimits.Max.y, aabbLimits.Max.z};
				aabbSlice.Vertices[3] = {aabbLimits.Max.x, aabbLimits.Max.y, aabbLimits.Max.z};

				// -- bottom
				aabbSlice.Vertices[4] = {aabbLimits.Min.x, aabbLimits.Min.y, aabbLimits.Min.z};
				aabbSlice.Vertices[5] = {aabbLimits.Max.x, aabbLimits.Min.y, aabbLimits.Min.z};
				aabbSlice.Vertices[6] = {aabbLimits.Min.x, aabbLimits.Min.y, aabbLimits.Max.z};
				aabbSlice.Vertices[7] = {aabbLimits.Max.x, aabbLimits.Min.y, aabbLimits.Max.z};
			}
		}
	}
	return 0;
}
