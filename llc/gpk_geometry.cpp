#include "gpk_geometry.h"
#include "gpk_voxel.h"
#include "gpk_view_n3.h"

enum SQUARE_MODE 
	{ SQUARE_MODE_TOP_LEFT
	, SQUARE_MODE_TOP_RIGHT
	};

template<typename _tIndex>
static	::gpk::error_t	geometryBuildGridIndices	(::gpk::apod<_tIndex> & positionIndices, uint32_t vertexOffset, const ::gpk::n2u16 cellCount, bool outward = false) {
	const uint16_t				pitch						= cellCount.x + 1;								// 2 3 
	const uint16_t				totalVertices				= pitch * (cellCount.y + 1);
	const uint32_t				indices[4]					= {0U, 1U, pitch, uint32_t(pitch + 1U)};	// 0 1 2 3
	const uint32_t				indices_modes[2][6]			=
		{ {indices[0], indices[2], indices[3], indices[0], indices[3], indices[1]}
		, {indices[0], indices[2], indices[1], indices[1], indices[2], indices[3]}
		};

	const ::gpk::n2u16			halfCount					= {uint16_t(cellCount.x >> 1U), uint16_t(cellCount.y >> 1U)};
	cellCount.enumerate([&positionIndices, totalVertices, halfCount, indices_modes, vertexOffset, outward](uint32_t & vertexIndex, ::gpk::n2u16 & coord) {
		const bool					reverseQuad 
			= ( (coord.x >= halfCount.x && coord.y <  halfCount.y)
			 || (coord.x <  halfCount.x && coord.y >= halfCount.y)
			);

		const ::gpk::vcu32			relative_indices			= indices_modes[((reverseQuad ? SQUARE_MODE_TOP_RIGHT : SQUARE_MODE_TOP_LEFT) + one_if(outward)) % 2];
		for(uint32_t i = 0; i < 6; ++i) {
			positionIndices.push_back(vertexOffset + ((vertexIndex + relative_indices[i]) % totalVertices));
		}
	}, [](uint32_t & vertexIndex, ::gpk::n2u16 & ) { 
		++vertexIndex; 
	});

	return 0;
}

::gpk::error_t			gpk::geometryBuildSphere	(::gpk::STrianglesIndexed & geometry, const ::gpk::SParamsSphere & params) {
	const ::gpk::n2f64			texCoordUnits				= {1.0 / params.CellCount.x, 1.0 / params.CellCount.y};
	const double				reverseScale				= params.Reverse ? -1.0 : 1.0;
	const ::gpk::n2f64			sliceScale					= ::gpk::n2f64{::gpk::math_2pi * texCoordUnits.x * reverseScale, ::gpk::math_pi * texCoordUnits.y};

	uint32_t					vertexOffset				= geometry.Positions.size();
	const ::gpk::n2u16			vertexCount					= params.CellCount + ::gpk::n2u16{1, 1};
	for(uint32_t y = 0; y < vertexCount.y; ++y) {
		const double				currentY					= sliceScale.y * y;
		const double				currentRadius				= sin(currentY);
		for(uint32_t x = 0; x < vertexCount.x; ++x)  {
			::gpk::n2f64				texcoord				= {x * texCoordUnits.x, 1.0 - (y * texCoordUnits.y)};
			geometry.TextureCoords.push_back(texcoord.f32());
			const double				currentX				= sliceScale.x * x;
			::gpk::n3f64				coord 					= {currentRadius * cos(currentX), -cos(currentY), currentRadius * sin(currentX)};
			geometry.Positions	.push_back((coord * params.Radius).f32() - params.Origin);
			geometry.Normals	.push_back((coord.Normalize() * reverseScale).f32());
		}
	}
	return ::geometryBuildGridIndices(geometry.PositionIndices, vertexOffset, params.CellCount); 
}

//
::gpk::error_t			gpk::geometryBuildCylinder	(::gpk::STrianglesIndexed & geometry, const ::gpk::SParamsCylinder & params) {
	const ::gpk::n2f64			texCoordUnits				= {1.0 / params.CellCount.x, 1.0 / params.CellCount.y};
	const double				radiusUnit					= texCoordUnits.y;
	const double				reverseScale				= params.Reverse ? -1.0 : 1.0;
	double						sliceScale					= texCoordUnits.x * ::gpk::math_2pi * params.DiameterRatio * reverseScale;

	const uint32_t				vertexOffset				= geometry.Positions.size();
	const ::gpk::n2u16			vertexCount					= params.CellCount + ::gpk::n2u16{1, 1};
	for(uint32_t y = 0; y < vertexCount.y; ++y) {
		const double				radius						= ::gpk::interpolate_linear(params.Radius.Min, params.Radius.Max, radiusUnit * y);
		const ::gpk::n3f64			coord						= {radius, (double)y};
		for(uint32_t x = 0; x < vertexCount.x; ++x) {
			::gpk::n2f64				texcoord					= {x * texCoordUnits.x, 1.0 - y * texCoordUnits.y};
			geometry.TextureCoords.push_back(texcoord.f32());
			::gpk::n3f32				pos							= gpk::n3f64{coord}.RotateY(sliceScale * x).f32();
			geometry.Positions.push_back(pos - params.Origin);
			geometry.Normals  .push_back(pos.Normalize() * reverseScale);
		}
	}

	return ::geometryBuildGridIndices(geometry.PositionIndices, vertexOffset, params.CellCount); 
}

::gpk::error_t			gpk::geometryBuildGrid		(::gpk::STrianglesIndexed & geometry, const ::gpk::SParamsGrid & params)	{
	const ::gpk::n2f64			cellScale					= {1.0 / params.CellCount.x, 1.0 / params.CellCount.y};
	const ::gpk::n2f64			texCoordUnits				= cellScale;
	const ::gpk::n2f64			scale						= params.Size.f64().InPlaceScale(cellScale);
	const ::gpk::n3f64			center						= {params.Origin.x, 0, params.Origin.y};

	const uint32_t				vertexOffset				= geometry.Positions.size();
	const ::gpk::n2u16			vertexCount					= params.CellCount + ::gpk::n2u16{1, 1};
	vertexCount.for_each([&geometry, scale, center, texCoordUnits](::gpk::n2u16 & coord) {
		::gpk::n3f64				position					= {coord.x * scale.x, 0, coord.y * scale.y};
		geometry.Positions		.push_back((position - center).f32());
		geometry.TextureCoords	.push_back(coord.f32().InPlaceScale(texCoordUnits).f32());
	});
	geometry.Normals.resize(geometry.Positions.size(), {0, 1, 0});

	return ::geometryBuildGridIndices(geometry.PositionIndices, vertexOffset, params.CellCount, params.Outward); 
}

::gpk::error_t			gpk::geometryBuildBox		(::gpk::STrianglesIndexed & geometry, const ::gpk::SParamsBox & params) {
	const uint32_t				offsetPositions				= geometry.Positions		.size(); geometry.Positions			.resize(geometry.Positions		.size() + 24);
	const uint32_t				offsetNormals				= geometry.Normals			.size(); geometry.Normals			.resize(geometry.Normals		.size() + 24);
	const uint32_t				offsetTextureCoords			= geometry.TextureCoords	.size(); geometry.TextureCoords		.resize(geometry.TextureCoords	.size() + 24);
	const uint32_t				offsetPositionIndices		= geometry.PositionIndices	.size(); geometry.PositionIndices	.resize(geometry.PositionIndices.size() + 36);
	const ::gpk::vc3f32			vertices					= {&::gpk::VOXEL_FACE_VERTICES[0].A, 24};
	for(uint32_t iVertex = offsetPositions; iVertex < geometry.Positions.size(); ++iVertex)
		geometry.Positions[iVertex] = (vertices[iVertex] - params.Origin).Scale(params.HalfSizes.f32() * 2.0f);

	//memcpy(&geometry.Positions		[0], ::gpk::VOXEL_FACE_VERTICES	, geometry.Positions		.byte_count());
	memcpy(&geometry.Normals		[offsetNormals		], ::gpk::VOXEL_FACE_NORMALS, geometry.Normals		.byte_count());
	memcpy(&geometry.TextureCoords	[offsetTextureCoords], ::gpk::VOXEL_FACE_UV		, geometry.TextureCoords.byte_count());
	const ::gpk::vcu16			indices						= {::gpk::VOXEL_FACE_INDICES_16[0], 36};
	for(uint32_t iIndex = offsetPositionIndices; iIndex < geometry.PositionIndices.size(); ++iIndex) 
		geometry.PositionIndices[iIndex] = indices[iIndex];

	return 0;
}
