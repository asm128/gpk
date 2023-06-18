#include "gpk_geometry.h"
#include "gpk_voxel.h"
#include "gpk_view_n3.h"

enum SQUARE_MODE 
	{ SQUARE_MODE_TOP_LEFT
	, SQUARE_MODE_TOP_RIGHT
	};

template<typename _tIndex>
static	::gpk::error_t	geometryBuildSpheroidIndices(::gpk::apod<_tIndex> & positionIndices, uint32_t vertexOffset, const ::gpk::n2u16 cellCount, bool reverseTriangles = false) {// uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3f32 & gridCenter)	{
	const uint16_t				pitch					= cellCount.x + 1;								// 2 3 
	const uint32_t				indices[4]				= {0U, 1U, pitch, uint32_t(pitch + 1U)};	// 0 1 2 3
	const uint32_t				indices_modes[2][6]		=
		{ {indices[0], indices[2], indices[3], indices[0], indices[3], indices[1]}
		, {indices[0], indices[2], indices[1], indices[1], indices[2], indices[3]}
		};

	const ::gpk::n2u16			halfCount				= {uint16_t(cellCount.x >> 1U), uint16_t(cellCount.y >> 1U)};
	cellCount.enumerate([&positionIndices, halfCount, indices_modes, vertexOffset, reverseTriangles](uint32_t & index, ::gpk::n2u16 & coord){
		const bool					reverseQuad 
			= ( (coord.x >= halfCount.x && coord.y <  halfCount.y)
			 || (coord.x <  halfCount.x && coord.y >= halfCount.y)
			);

		const ::gpk::vcu32			relative_indices		= indices_modes[((reverseQuad ? SQUARE_MODE_TOP_RIGHT : SQUARE_MODE_TOP_LEFT) + one_if(reverseTriangles)) % 2];
		for(uint32_t i = 0; i < 6; ++i) {
			positionIndices.push_back(vertexOffset + index + relative_indices[i]);
		}
	}, [](uint32_t & index, ::gpk::n2u16 & ){ 
		++index; 
	});

	return 0;
}

::gpk::error_t			gpk::geometryBuildSphere	(STrianglesIndexed & geometry, const ::gpk::SParamsSphere & params) {// uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3f32 & gridCenter)	{
	const ::gpk::n2f32			texCoordUnits				= {1.0f / params.Slices, 1.0f / params.Stacks};
	const float					scale						= params.Reverse ? -1.0f : 1.0f;

	uint32_t					vertexOffset				= geometry.Positions.size();
	for(uint32_t y = 0; y < (uint32_t)params.Stacks + 1; ++y) {
		const double				currentY					= ::gpk::math_pi * y / params.Slices;
		const double				currentRadius				= sin(currentY);
		for(uint32_t x = 0; x < (uint32_t)params.Slices + 1; ++x)  {
			::gpk::n2f32				texcoord				= {x * texCoordUnits.x, 1.0f - (y * texCoordUnits.y)};
			geometry.TextureCoords.push_back(texcoord);
			const double				currentX				= ::gpk::math_2pi * x / params.Stacks;
			::gpk::n3f64				coord 					= {currentRadius * cos(currentX), -cos(currentY), currentRadius * sin(currentX) * scale};
			geometry.Positions	.push_back((coord * params.Radius).f32() - params.Center);
			geometry.Normals	.push_back(coord.Normalize().f32() * scale);
		}
	}

	::geometryBuildSpheroidIndices(geometry.PositionIndices, vertexOffset, {params.Slices, params.Stacks}); 
	return 0;
}

//
::gpk::error_t			gpk::geometryBuildCylinder	(STrianglesIndexed & geometry, const ::gpk::SParamsCylinder & params) {//, uint32_t stacks, uint32_t slices, float radiusYMin, float radiusYMax, const ::gpk::n3f32 & gridCenter, const ::gpk::n3f32 & scale, bool negateNormals, float diameterRatio)	{
	const ::gpk::n2f32			texCoordUnits				= {1.0f / params.Slices, 1.0f / params.Stacks};
	const double				radiusUnit					= 1.0 / params.Stacks;
	const float					scale						= params.Reverse ? -1.0f : 1.0f;

	uint32_t					vertexOffset				= geometry.Positions.size();
	for(uint32_t y = 0; y < (uint32_t)params.Stacks + 1; ++y) {
		double						radius						= ::gpk::interpolate_linear(params.Radius.Min, params.Radius.Max, radiusUnit * y);
		for(uint32_t z = 0; z < (uint32_t)params.Slices + 1; ++z) {
			::gpk::n2f32				texcoord					= {z * texCoordUnits.x, 1.f - y * texCoordUnits.y};
			geometry.TextureCoords.push_back(texcoord);

			::gpk::n3f64				coord						= {radius, (double)y};
			coord.RotateY(::gpk::math_2pi * texCoordUnits.x * z * params.DiameterRatio); 
			coord.z					*= scale;
			geometry.Normals  .push_back(coord.f32().Normalize() * scale);
			geometry.Positions.push_back(coord.f32() - params.Center);
		}
	}

	::geometryBuildSpheroidIndices(geometry.PositionIndices, vertexOffset, {params.Slices, params.Stacks}); 
	return 0;
}

::gpk::error_t			gpk::geometryBuildGrid	(::gpk::STrianglesIndexed & geometry, const ::gpk::SParamsGrid & params)	{
	const ::gpk::n2f64			cellScale				= {1.0 / params.CellCount.x, 1.0 / params.CellCount.y};
	const ::gpk::n2f64			texCoordUnits			= params.Size.f64().InPlaceScale(cellScale);
	const ::gpk::n2f64			scale					= params.Size.f64().InPlaceScale(cellScale);
	const ::gpk::n3f64			center					= {params.Center.x, 0, params.Center.y};

	const uint32_t				vertexOffset			= geometry.Positions.size();
	(params.CellCount + ::gpk::n2u16{1, 1}).for_each([&geometry, scale, center, texCoordUnits](::gpk::n2u16 & coord) {
		::gpk::n3f64				position				= {coord.x * scale.x, 0, coord.y * scale.y};
		position				-= center;

		geometry.Positions		.push_back(position.f32());
		geometry.TextureCoords	.push_back(coord.f64().InPlaceScale(texCoordUnits).f32());
		geometry.Normals		.push_back({0, 1, 0});
	});

	::geometryBuildSpheroidIndices(geometry.PositionIndices, vertexOffset, params.CellCount, params.ReverseTriangles); 
	return 0;
}

::gpk::error_t			gpk::geometryBuildBox	(::gpk::STrianglesIndexed & geometry, const ::gpk::SParamsBox & params) {
	const uint32_t				offsetPositions			= geometry.Positions		.size(); geometry.Positions			.resize(geometry.Positions		.size() + 24);
	const uint32_t				offsetNormals			= geometry.Normals			.size(); geometry.Normals			.resize(geometry.Normals		.size() + 24);
	const uint32_t				offsetTextureCoords		= geometry.TextureCoords	.size(); geometry.TextureCoords		.resize(geometry.TextureCoords	.size() + 24);
	const uint32_t				offsetPositionIndices	= geometry.PositionIndices	.size(); geometry.PositionIndices	.resize(geometry.PositionIndices.size() + 36);
	const ::gpk::vc3f32			vertices				= {&::gpk::VOXEL_FACE_VERTICES[0].A, 24};
	for(uint32_t iVertex = offsetPositions; iVertex < geometry.Positions.size(); ++iVertex)
		geometry.Positions[iVertex] = (vertices[iVertex] - params.Center).Scale(params.HalfSizes.f32() * 2.0f);

	//memcpy(&geometry.Positions		[0], ::gpk::VOXEL_FACE_VERTICES	, geometry.Positions		.byte_count());
	memcpy(&geometry.Normals		[offsetNormals		], ::gpk::VOXEL_FACE_NORMALS, geometry.Normals		.byte_count());
	memcpy(&geometry.TextureCoords	[offsetTextureCoords], ::gpk::VOXEL_FACE_UV		, geometry.TextureCoords.byte_count());
	const ::gpk::vcu16			indices					= {::gpk::VOXEL_FACE_INDICES_16[0], 36};
	for(uint32_t iIndex = offsetPositionIndices; iIndex < geometry.PositionIndices.size(); ++iIndex) 
		geometry.PositionIndices[iIndex] = indices[iIndex];

	return 0;
}
