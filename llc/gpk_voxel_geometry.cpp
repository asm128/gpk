#include "gpk_voxel_geometry.h"


static	::gpk::error_t	geometryVoxelFace
	( ::gpk::STrianglesIndexed						& geometry
	, const ::gpk::n3f32							& voxelPos
	, const ::gpk::view<const ::gpk::n3f32>	& rawVertices
	, const ::gpk::view<const uint8_t>				& rawIndices
	) {
	::gpk::n3f32				vertices [4]				= {}; 
	uint32_t					indices  [6]				= {}; 
	uint32_t					offsetVertex				= geometry.Positions.size();
	for(uint32_t iVertex = 0; iVertex < 4; ++iVertex)
		vertices[iVertex] = rawVertices[iVertex] + voxelPos; 

	for(uint32_t iIndex = 0; iIndex < 6; ++iIndex)
		indices[iIndex] = rawIndices[iIndex] + offsetVertex;

	gpk_necs(geometry.Positions			.append(vertices));
	gpk_necs(geometry.PositionIndices	.append(indices));
	return 0;
}

::gpk::error_t			gpk::geometryVoxelModel		(::gpk::SVoxelGeometry & output, const ::gpk::SVoxelMap<uint8_t> & voxelMap) {
	const ::gpk::n3<uint8_t>	dimensions				= voxelMap.Dimensions;

	::gpk::STrianglesIndexed	& geometry				= output.Geometry;
	::gpk::apod<::gpk::SRenderMaterialPaletted>		& materials				= output.Materials;

	::gpk::SAABBGeometry		& aabbModel				= output.AABBModel;
	for(uint32_t i = 0; i < 8; ++i) {
		aabbModel.Vertices[i]								= ::gpk::VOXEL_VERTICES[i];
		aabbModel.Vertices[i].Scale(dimensions.Cast<float>());
	}

	::gpk::view<const ::gpk::bgra>	rgba					= voxelMap.Palette;
	if(0 == rgba.size())
		rgba				= ::gpk::VOXEL_PALETTE;

	const ::gpk::view<const ::gpk::SVoxel<uint8_t>>	voxels					= voxelMap.Voxels;
	for(uint32_t iFace = 0; iFace < 6; ++iFace) {
		::gpk::apod<::gpk::SGeometryGroup>	& faceSlices			= output.GeometrySlices[iFace];
		::gpk::SAABBGeometry				& aabbSlice				= output.AABBSlices[iFace];

		const ::gpk::n3<int8_t>				faceDelta				= ::gpk::VOXEL_DELTAS			[iFace];
		::gpk::view<const uint8_t>			rawIndices				= ::gpk::VOXEL_FACE_INDICES		[iFace];
		::gpk::view<const ::gpk::n3f32>		rawVertices				= {&::gpk::VOXEL_FACE_VERTICES	[iFace].A, 4};
		::gpk::view<const ::gpk::n2f32>		rawTexCoord				= {&::gpk::VOXEL_FACE_UV		[iFace].A, 4};

		geometry.Normals.push_back(::gpk::VOXEL_NORMALS[iFace]);
		geometry.TextureCoords.append(rawTexCoord);
		for(uint32_t iValue = 0; iValue < rgba.size(); ++iValue) {
			::gpk::SGeometryGroup				newGroup					= {};
			newGroup.Geometry				= 0;
			newGroup.Image					= iValue;
			newGroup.Material				= iValue;
			newGroup.Modes.IndexMode		= ::gpk::INDEX_MODE_LIST;
			newGroup.Modes.PrimitiveType	= ::gpk::PRIMITIVE_TYPE_TRIANGLE;
			newGroup.Slice.Offset			= (uint16_t)geometry.PositionIndices.size();
			::gpk::SMinMax<::gpk::n3f32>		aabbLimits					= {};

			for(uint32_t iVoxel = 0; iVoxel < voxels.size(); ++iVoxel) {
				const ::gpk::SVoxel<uint8_t>		voxel						= voxels[iVoxel];
				if(voxel.ColorIndex != iValue)
					continue;

				aabbLimits.Min.x = ::gpk::min(aabbLimits.Min.x, (float)voxel.Position.x);
				aabbLimits.Min.y = ::gpk::min(aabbLimits.Min.y, (float)voxel.Position.y);
				aabbLimits.Min.z = ::gpk::min(aabbLimits.Min.z, (float)voxel.Position.z);
				aabbLimits.Max.x = ::gpk::max(aabbLimits.Max.x, (float)voxel.Position.x);
				aabbLimits.Max.y = ::gpk::max(aabbLimits.Max.y, (float)voxel.Position.y);
				aabbLimits.Max.z = ::gpk::max(aabbLimits.Max.z, (float)voxel.Position.z);

				::gpk::bgra							color						= rgba[iValue ? iValue - 1 : 0];
				uint8_t								adjacentValue				= {};
				voxelMap.GetValue((voxel.Position.i32() + faceDelta.i32()).u8(), adjacentValue);
				bool								renderFace					= 0 == adjacentValue || color.a < 0xFF;
				if(false == renderFace)
					continue;

				::geometryVoxelFace(geometry, voxel.Position.f32(), rawVertices, rawIndices);
				gpk_necs(output.FaceIndices.push_back(iFace));
				gpk_necs(output.VoxelIndices.push_back(iVoxel));
			}
			newGroup.Slice.Count						= geometry.PositionIndices.size() / 3;
			if(newGroup.Slice.Count) {
				faceSlices.push_back(newGroup);
				materials.push_back({rgba[iValue], (uint16_t)iValue});
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
