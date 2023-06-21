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
	const uint32_t				vertexOffset				= geometry.Positions.size();
	const ::gpk::n2u16			vertexCount					= params.CellCount + ::gpk::n2u16{1, 1};

	// -- Generate texture coordinates
	const ::gpk::n2f64			cellUnits					= {1.0f / params.CellCount.x, 1.0f / params.CellCount.y};
	vertexCount.for_each([&geometry, &params, cellUnits](::gpk::n2u16 & coord) { geometry.TextureCoords.push_back(::gpk::n2u32{coord.x, (uint32_t)params.CellCount.y - coord.y}.f64().InPlaceScale(cellUnits).f32()); });

	// -- Generate normals and positions
	const double				reverseScale				= params.Reverse ? -1.0 : 1.0;
	const ::gpk::n2f64			sliceScale					= ::gpk::n2f64{::gpk::math_2pi * cellUnits.x * reverseScale, ::gpk::math_pi * cellUnits.y};
	for(uint32_t y = 0; y < vertexCount.y; ++y) {
		const double				currentY					= sliceScale.y * y;
		const double				currentRadius				= sin(currentY);
		for(uint32_t x = 0; x < vertexCount.x; ++x) {
			const double				currentX					= sliceScale.x * x;
			const ::gpk::n3f64			coord 						= ::gpk::n3f64{currentRadius * cos(currentX), -cos(currentY), currentRadius * sin(currentX)};
			geometry.Positions	.push_back((coord * params.Radius).f32() - params.Origin);
			geometry.Normals	.push_back((coord * reverseScale).f32());
		}
	}
	return ::geometryBuildGridIndices(geometry.PositionIndices, vertexOffset, params.CellCount); 
}

//
::gpk::error_t			gpk::geometryBuildCylinder	(::gpk::STrianglesIndexed & geometry, const ::gpk::SParamsCylinder & params) {
	const uint32_t				vertexOffset				= geometry.Positions.size();
	const ::gpk::n2u16			vertexCount					= params.CellCount + ::gpk::n2u16{1, 1};

	// -- Generate texture coordinates
	const ::gpk::n2f64			cellUnits					= {1.0f / params.CellCount.x, 1.0f / params.CellCount.y};
	vertexCount.for_each([&geometry, &params, cellUnits](::gpk::n2u16 & coord) { geometry.TextureCoords.push_back(::gpk::n2u32{coord.x, (uint32_t)params.CellCount.y - coord.y}.f64().InPlaceScale(cellUnits).f32()); });

	// -- Generate normals
	const ::gpk::n3f64			normalBase					= gpk::n3f64{cellUnits.y, (params.Radius.Max - params.Radius.Min) * cellUnits.x}.Normalize();
	const double				reverseScale				= params.Reverse ? -1.0 : 1.0;
	const double				sliceScale					= cellUnits.x * ::gpk::math_2pi * params.DiameterRatio * reverseScale;
	vertexCount.for_each([&geometry, &normalBase, sliceScale, reverseScale](::gpk::n2u16 & coord) { geometry.Normals.push_back((::gpk::n3f64{normalBase}.RotateY(sliceScale * coord.x) * reverseScale).f32()); });

	// -- Generate positions
	for(uint32_t y = 0; y < vertexCount.y; ++y) {
		const double				radius						= ::gpk::interpolate_linear(params.Radius.Min, params.Radius.Max, cellUnits.y * y);
		for(uint32_t x = 0; x < vertexCount.x; ++x)
			geometry.Positions.push_back(::gpk::n3f64{radius, (double)y}.RotateY(sliceScale * x).f32() - params.Origin);
	}

	return ::geometryBuildGridIndices(geometry.PositionIndices, vertexOffset, params.CellCount); 
}

::gpk::error_t			gpk::geometryBuildGrid		(::gpk::STrianglesIndexed & geometry, const ::gpk::SParamsGrid & params) {
	const uint32_t				vertexOffset				= geometry.Positions.size();
	const ::gpk::n2u16			vertexCount					= params.CellCount + ::gpk::n2u16{1, 1};

	// -- Generate texture coordinates
	const ::gpk::n2f64			cellUnits					= {1.0f / params.CellCount.x, 1.0f / params.CellCount.y};

	vertexCount.for_each([&geometry, &params, cellUnits](::gpk::n2u16 & coord) { geometry.TextureCoords.push_back(::gpk::n2u32{coord.x, (uint32_t)params.CellCount.y - coord.y}.f32().InPlaceScale(cellUnits).f32()); });
	// -- Generate normals
	geometry.Normals.resize(vertexCount.Area(), {0, 1, 0});	// The normals are all the same for grids

	// -- Generate positions
	const ::gpk::n2f64			scale						= params.Size.f64().InPlaceScale(cellUnits);
	const ::gpk::n3f64			center						= {params.Origin.x, 0, params.Origin.y};
	vertexCount.for_each([&geometry, scale, center](::gpk::n2u16 & coord) {
		::gpk::n3f64				position					= {coord.x * scale.x, 0, coord.y * scale.y};
		geometry.Positions.push_back((position - center).f32());
	});

	return ::geometryBuildGridIndices(geometry.PositionIndices, vertexOffset, params.CellCount, params.Outward); 
}

::gpk::error_t			gpk::geometryBuildBox		(::gpk::STrianglesIndexed & geometry, const ::gpk::SParamsBox & params) {
	const uint32_t				offsetPositions				= geometry.Positions		.size(); geometry.Positions			.resize(geometry.Positions		.size() + 24);
	const uint32_t				offsetNormals				= geometry.Normals			.size(); geometry.Normals			.resize(geometry.Normals		.size() + 24);
	const uint32_t				offsetTextureCoords			= geometry.TextureCoords	.size(); geometry.TextureCoords		.resize(geometry.TextureCoords	.size() + 24);
	const uint32_t				offsetPositionIndices		= geometry.PositionIndices	.size(); geometry.PositionIndices	.resize(geometry.PositionIndices.size() + 36);
	const ::gpk::vc3f32			vertices					= {&::gpk::VOXEL_FACE_VERTICES[0].A, 24};
	for(uint32_t iVertex = offsetPositions; iVertex < geometry.Positions.size(); ++iVertex)
		geometry.Positions[iVertex]	= (vertices[iVertex] - params.Origin).Scale(params.HalfSizes * 2.0f);

	//memcpy(&geometry.Positions		[0], ::gpk::VOXEL_FACE_VERTICES	, geometry.Positions		.byte_count());
	memcpy(&geometry.Normals		[offsetNormals		], ::gpk::VOXEL_FACE_NORMALS, geometry.Normals		.byte_count());
	memcpy(&geometry.TextureCoords	[offsetTextureCoords], ::gpk::VOXEL_FACE_UV		, geometry.TextureCoords.byte_count());
	const ::gpk::vcu16			indices						= {::gpk::VOXEL_FACE_INDICES_16[0], 36};
	for(uint32_t iIndex = offsetPositionIndices; iIndex < geometry.PositionIndices.size(); ++iIndex) 
		geometry.PositionIndices[iIndex] = indices[iIndex];

	return 0;
}
