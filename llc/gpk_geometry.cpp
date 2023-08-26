#include "gpk_geometry.h"
#include "gpk_voxel.h"
#include "gpk_view_n3.h"
#include "gpk_sphere.h"

::gpk::error_t			gpk::geometryBuildBox		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsBox & params) {
	const uint32_t				offsetPositions				= geometry.Positions		.size(); geometry.Positions			.resize(offsetPositions			+ 24);
	const uint32_t				offsetNormals				= geometry.Normals			.size(); geometry.Normals			.resize(offsetNormals			+ 24);
	const uint32_t				offsetTextureCoords			= geometry.TextureCoords	.size(); geometry.TextureCoords		.resize(offsetTextureCoords		+ 24);
	const uint32_t				offsetPositionIndices		= geometry.PositionIndices	.size(); geometry.PositionIndices	.resize(offsetPositionIndices	+ 36);
	const ::gpk::vc3f32			vertices					= {&::gpk::VOXEL_FACE_VERTICES[0].A, 24};
	for(uint32_t iVertex = offsetPositions; iVertex < geometry.Positions.size(); ++iVertex)
		geometry.Positions[iVertex]	= params.Orientation.RotateVector(vertices[iVertex].Scaled(params.HalfSizes * 2.0f) - params.Origin);

	//memcpy(&geometry.Positions		[0], ::gpk::VOXEL_FACE_VERTICES	, geometry.Positions		.byte_count());
	for(uint32_t iVertex = offsetPositions; iVertex < geometry.Normals.size(); ++iVertex)
		geometry.Normals[iVertex]	= params.Orientation.RotateVector(geometry.Normals[iVertex]);
	memcpy(&geometry.Normals		[offsetNormals		], ::gpk::VOXEL_FACE_NORMALS.begin(), ::gpk::VOXEL_FACE_NORMALS	.byte_count());
	memcpy(&geometry.TextureCoords	[offsetTextureCoords], ::gpk::VOXEL_FACE_UV.begin()		, ::gpk::VOXEL_FACE_UV		.byte_count());
	const ::gpk::vcu16			indices						= {::gpk::VOXEL_FACE_INDICES_16[0], 36};
	for(uint32_t iIndex = offsetPositionIndices; iIndex < geometry.PositionIndices.size(); ++iIndex) 
		geometry.PositionIndices[iIndex] = indices[iIndex];

	return 0;
}

enum SQUARE_MODE 
	{ SQUARE_MODE_TOP_LEFT
	, SQUARE_MODE_TOP_RIGHT
	};

tplt<tpnm _tIndex>
static	::gpk::error_t	geometryBuildGridIndices	(::gpk::apod<_tIndex> & positionIndices, uint32_t vertexOffset, const ::gpk::n2u16 cellCount, bool counterClockwise, bool outward) {
	const uint16_t				pitch						= cellCount.x + 1;								// 2 3 
	const ::gpk::astu32<4>		indices						= counterClockwise ? ::gpk::astu32<4>{{1U, 0U, uint32_t(pitch + 1U), pitch}} : ::gpk::astu32<4>{{0U, 1U, pitch, uint32_t(pitch + 1U)}};	// 0 1 2 3
	const uint32_t				indices_modes[2][6]			=
		{ {indices[0], indices[2], indices[3], indices[0], indices[3], indices[1]}
		, {indices[0], indices[2], indices[1], indices[1], indices[2], indices[3]}
		};

	const ::gpk::n2u16			halfCount					= {uint16_t(cellCount.x >> 1U), uint16_t(cellCount.y >> 1U)};
	cellCount.enumerate([&positionIndices, halfCount, indices_modes, vertexOffset, outward](uint32_t & vertexIndex, ::gpk::n2u16 & coord) {
		const bool					reverseQuad 
			= ( (coord.x >= halfCount.x && coord.y <  halfCount.y)
			 || (coord.x <  halfCount.x && coord.y >= halfCount.y)
			);

		const ::gpk::vcu32			relative_indices			= indices_modes[((reverseQuad ? SQUARE_MODE_TOP_RIGHT : SQUARE_MODE_TOP_LEFT) + one_if(outward)) % 2];
		for(uint32_t i = 0; i < 6; ++i) {
			positionIndices.push_back(vertexOffset + vertexIndex + relative_indices[i]);
		}
	}, [](uint32_t & vertexIndex, ::gpk::n2u16 & ) { 
		++vertexIndex; 
	});
	return 0;
}

::gpk::error_t			gpk::geometryBuildGrid		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsGrid & params) {
	const uint32_t				vertexOffset				= geometry.Positions.size();
	const ::gpk::n2u16			vertexCount					= params.CellCount + ::gpk::n2u16{1, 1};

	// -- Generate texture coordinates
	const ::gpk::n2f64			cellUnits					= {1.0f / params.CellCount.x, 1.0f / params.CellCount.y};
	vertexCount.for_each([&geometry, &params, cellUnits](::gpk::n2u16 & coord) { geometry.TextureCoords.push_back(::gpk::n2u32{coord.x, (uint32_t)params.CellCount.y - coord.y}.f32().InPlaceScale(cellUnits).f32()); });

	// -- Generate normals
	geometry.Normals.resize(geometry.Normals.size() + vertexCount.Area(), params.Orientation.RotateVector({0, params.Reverse ? -1.0f : 1.0f, 0}));	// The normals are all the same for grids

	// -- Generate positions
	const ::gpk::n2f64			scale						= params.Size.f64().InPlaceScale(cellUnits);
	vertexCount.for_each([&geometry, scale, params](::gpk::n2u16 & coord) {
		const ::gpk::n3f64			position					= {coord.x * scale.x, 0, coord.y * scale.y};
		geometry.Positions.push_back(params.Orientation.RotateVector(position.f32() - params.Origin));
	});
	return ::geometryBuildGridIndices(geometry.PositionIndices, vertexOffset, params.CellCount, params.Reverse, params.Outward); 
}

::gpk::error_t			gpk::geometryBuildSphere	(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsSphere & params) {
	const uint32_t				vertexOffset				= geometry.Positions.size();
	const ::gpk::n2u16			vertexCount					= params.CellCount + ::gpk::n2u16{1, 1};

	// -- Generate texture coordinates
	const ::gpk::n2f64			cellUnits					= {1.0f / params.CellCount.x, 1.0f / params.CellCount.y};
	vertexCount.for_each([&geometry, &params, cellUnits](::gpk::n2u16 & coord) { geometry.TextureCoords.push_back(::gpk::n2u32{coord.x, (uint32_t)params.CellCount.y - coord.y}.f64().InPlaceScale(cellUnits).f32()); });

	// -- Generate normals and positions
	const double				reverseScale				= params.Reverse ? -1.0 : 1.0;
	const ::gpk::n2f64			sliceScale					= ::gpk::n2f64{::gpk::math_2pi * cellUnits.x * reverseScale * params.Circumference, ::gpk::math_pi * cellUnits.y};
	const double				sliceOffset					= 0; //::gpk::math_pi * (1.0 - params.Circumference) * .5;
	for(uint32_t y = 0; y < vertexCount.y; ++y) {
		const double				currentY					= sliceScale.y * y;
		const double				currentRadius				= sin(currentY);
		for(uint32_t x = 0; x < vertexCount.x; ++x) {
			const double				currentX					= sliceScale.x * x + sliceOffset;
			const ::gpk::n3f64			coord 						= ::gpk::n3f64{currentRadius * cos(currentX), -cos(currentY), currentRadius * sin(currentX)};
			gpk_necs(geometry.Normals	.push_back(params.Orientation.RotateVector((coord * reverseScale).f32())));
			gpk_necs(geometry.Positions	.push_back(params.Orientation.RotateVector((coord * params.Radius).f32() - params.Origin)));
		}
	}
	return ::geometryBuildGridIndices(geometry.PositionIndices, vertexOffset, params.CellCount, false, false); 
}

::gpk::error_t			gpk::geometryBuildCircleSide(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsCircle & params) {
	const uint32_t				vertexOffset				= geometry.Positions.size();
	const uint32_t				vertexCount					= (uint32_t)params.Slices + 1;
	const double				cellUnits					= 1.0 / params.Slices;
	const double				reverseScale				= params.Reverse ? -1.0 : 1.0;
	const double				sliceScale					= cellUnits * ::gpk::math_2pi * params.Circumference * reverseScale;
	const double				sliceOffset					= 0; //::gpk::math_2pi * (1.0 - params.Circumference);

	// -- Generate positions and texture coordinates
	gpk_necs(geometry.Positions		.push_back(params.Orientation.RotateVector(-params.Origin)));	// The first vertex is the center of the circle
	gpk_necs(geometry.TextureCoords	.push_back(::gpk::n2f32{.5f, .5f}));					// The first vertex is the center of the circle
	for(uint32_t iVertex = 0; iVertex < vertexCount; ++iVertex) {
		double						angle						= sliceScale * iVertex + sliceOffset;
		gpk_necs(geometry.Positions		.push_back(params.Orientation.RotateVector(::gpk::n3f64{params.Radius}.RotateY(angle).f32() - params.Origin)));
		gpk_necs(geometry.TextureCoords	.push_back(::gpk::n2f64{.5}.Rotate(angle).f32() + ::gpk::n2f32{.5f, .5f}));
	}

	// -- Generate normals
	gpk_necs(geometry.Normals.resize(geometry.Normals.size() + vertexCount + 1, params.Orientation.RotateVector({0, 1, 0})));	// The normals are all the same for a circle

	for(uint32_t iSlice = 0, sliceCount = params.Slices + 1; iSlice < sliceCount; ++iSlice)
		gpk_necs(geometry.PositionIndices.append({vertexOffset + 0, vertexOffset + iSlice + 1, vertexOffset + iSlice + 0}));

	return 0; 
}

::gpk::error_t			gpk::geometryBuildCylinderWall	(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsCylinderWall & params) {
	const uint32_t				vertexOffset				= geometry.Positions.size();
	const ::gpk::n2u16			vertexCount					= params.CellCount + ::gpk::n2u16{1, 1};

	// -- Generate texture coordinates
	const ::gpk::n2f64			cellUnits					= {1.0f / params.CellCount.x, 1.0f / params.CellCount.y};
	vertexCount.for_each([&geometry, &params, cellUnits](::gpk::n2u16 & coord) { geometry.TextureCoords.push_back(::gpk::n2u32{coord.x, (uint32_t)params.CellCount.y - coord.y}.f64().InPlaceScale(cellUnits).f32()); });

	// -- Generate normals
	const double				reverseScale				= params.Reverse ? -1.0 : 1.0;
	const double				lengthScale					= cellUnits.y * params.Height;
	const ::gpk::n3f64			normalBase					= gpk::n3f64{params.Height, -(params.Radius.Max - params.Radius.Min)}.Normalize() * reverseScale;
	const double				sliceScale					= cellUnits.x * ::gpk::math_2pi * params.Circumference * reverseScale;
	const double				sliceOffset					= 0; //::gpk::math_pi * (1.0 - params.Circumference) * .5;
	vertexCount.for_each([&geometry, &normalBase, sliceScale, sliceOffset, &params](::gpk::n2u16 & coord) { geometry.Normals.push_back(params.Orientation.RotateVector((::gpk::n3f64{normalBase}.RotateY(sliceScale * coord.x + sliceOffset)).f32())); });

	// -- Generate positions
	for(uint32_t y = 0; y < vertexCount.y; ++y) {
		const double				radius						= ::gpk::interpolate_linear(params.Radius.Min, params.Radius.Max, cellUnits.y * y);
		for(uint32_t x = 0; x < vertexCount.x; ++x)
			geometry.Positions.push_back(params.Orientation.RotateVector(::gpk::n3f64{radius, (double)y * lengthScale}.RotateY(sliceScale * x + sliceOffset).f32() - params.Origin));
	}
	return ::geometryBuildGridIndices(geometry.PositionIndices, vertexOffset, params.CellCount, false, false); 
}

//
::gpk::error_t			gpk::geometryBuildFigure0	(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsHelix & params) { //(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3f32 & gridCenter)	{
	const uint32_t				vertexOffset				= geometry.Positions.size();
	const ::gpk::n2u16			vertexCount					= params.CellCount + ::gpk::n2u16{1, 1};

	// -- Generate texture coordinates
	const ::gpk::n2f64			cellUnits					= {1.0f / params.CellCount.x, 1.0f / params.CellCount.y};
	geometry.TextureCoords	.reserve(vertexCount.Area());
	geometry.Positions		.reserve(vertexCount.Area());
	geometry.Normals		.reserve(vertexCount.Area());
	vertexCount.for_each([&geometry, &params, cellUnits](::gpk::n2u16 & coord) { geometry.TextureCoords.push_back(::gpk::n2u32{coord.x, (uint32_t)params.CellCount.y - coord.y}.f32().InPlaceScale(cellUnits).f32()); });

	//const double				lengthScale					= cellUnits.y * params.Length;
	const ::gpk::n2f64			sliceScale					= ::gpk::n2f64{::gpk::math_2pi * cellUnits.x, ::gpk::math_pi * cellUnits.y};
	for(uint32_t y = 0; y < vertexCount.y; ++y) {
		const double				radius						= ::gpk::interpolate_linear(params.Radius.Min, params.Radius.Max, cellUnits.y * y);
		for(uint32_t x = 0; x < vertexCount.x; ++x) {
			::gpk::n3f64				position					= {sin(sliceScale.y * y) * sin(sliceScale.x * x) * radius, sin(sliceScale.y * y) * sin(::gpk::math_pi * x / params.CellCount.x) * params.Length, cos(sliceScale.x * x) * radius};
			geometry.Positions.push_back(params.Orientation.RotateVector(position.f32() - params.Origin));
		}
	}
	const uint32_t				indexOffset				= geometry.PositionIndices.size();
	gpk_necs(::geometryBuildGridIndices(geometry.PositionIndices, vertexOffset, params.CellCount, false, false)); 
	for(uint32_t iVertexIndex = indexOffset; iVertexIndex < geometry.PositionIndices.size(); iVertexIndex += 3) {
		const ::gpk::n3f32	a	= geometry.Positions[geometry.PositionIndices[iVertexIndex + 0]];
		const ::gpk::n3f32	b	= geometry.Positions[geometry.PositionIndices[iVertexIndex + 1]];
		const ::gpk::n3f32	c	= geometry.Positions[geometry.PositionIndices[iVertexIndex + 2]];
		geometry.Normals.push_back(params.Orientation.RotateVector((a - b).Normalize().Cross((a - c).Normalize()).Normalize()));
	}
	return 0;
}

::gpk::error_t			gpk::geometryBuildHelixHalf	(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsHelix & params) {
	const uint32_t				vertexOffset				= geometry.Positions.size();
	const ::gpk::n2u16			vertexCount					= params.CellCount + ::gpk::n2u16{1, 1};

	// -- Generate texture coordinates
	const ::gpk::n2f64			cellUnits					= {1.0f / params.CellCount.x, 1.0f / params.CellCount.y};
	geometry.TextureCoords	.reserve(vertexCount.Area());
	geometry.Positions		.reserve(vertexCount.Area());
	geometry.Normals		.reserve(vertexCount.Area());
	vertexCount.for_each([&geometry, &params, cellUnits](::gpk::n2u16 & coord) { geometry.TextureCoords.push_back(::gpk::n2u32{coord.x, (uint32_t)params.CellCount.y - coord.y}.f32().InPlaceScale(cellUnits).f32()); });

	//const double				lengthScale					= cellUnits.y * params.Length;
	const ::gpk::n2f64			sliceScale					= ::gpk::n2f64{::gpk::math_2pi * cellUnits.x, ::gpk::math_pi * cellUnits.y};
	for(uint32_t y = 0; y < vertexCount.y; ++y) {
		const double				radius						= ::gpk::interpolate_linear(params.Radius.Min, params.Radius.Max, cellUnits.y * y);
		for(uint32_t x = 0; x < vertexCount.x; ++x) {
			::gpk::n3f64				position					= {sin(sliceScale.y * y) * sin(sliceScale.x * x) * radius, sin(sliceScale.y * y) * cos(::gpk::math_pi * x / params.CellCount.x) * params.Length, cos(sliceScale.x * x) * radius};
			geometry.Positions.push_back(params.Orientation.RotateVector(position.f32() - params.Origin));
		}
	}

	const uint32_t				indexOffset				= geometry.PositionIndices.size();
	gpk_necs(::geometryBuildGridIndices(geometry.PositionIndices, vertexOffset, params.CellCount, false, false)); 
	for(uint32_t iVertexIndex = indexOffset; iVertexIndex < geometry.PositionIndices.size(); iVertexIndex += 3) {
		const ::gpk::n3f32	a	= geometry.Positions[geometry.PositionIndices[iVertexIndex + 0]];
		const ::gpk::n3f32	b	= geometry.Positions[geometry.PositionIndices[iVertexIndex + 1]];
		const ::gpk::n3f32	c	= geometry.Positions[geometry.PositionIndices[iVertexIndex + 2]];
		geometry.Normals.push_back(params.Orientation.RotateVector((a - b).Normalize().Cross((a - c).Normalize()).Normalize()));
	}
	return 0;
}


::gpk::error_t			gpk::geometryBuildHelix		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsHelix & params)	{
	const uint32_t				vertexOffset				= geometry.Positions.size();
	gpk_necs(::gpk::geometryBuildHelixHalf(geometry, params));
	const uint32_t				vertexCount					= geometry.Positions.size() - vertexOffset;
	gpk_necs(geometry.TextureCoords	.append({&geometry.TextureCoords[vertexOffset], vertexCount}));
	gpk_necs(geometry.Positions		.append({&geometry.Positions	[vertexOffset], vertexCount}));
	gpk_necs(geometry.Normals		.append({&geometry.Normals		[vertexOffset], vertexCount}));
	geometry.Positions	.for_each([](::gpk::n3f32 & coord){ coord.z *= -1; coord.y *= -1; }, vertexOffset);
	geometry.Normals	.for_each([](::gpk::n3f32 & coord){ coord.z *= -1; coord.y *= -1; }, vertexOffset);
	return 0;
}

::gpk::error_t			gpk::geometryBuildRingSide	(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsRingSide & params) {
	const uint32_t				vertexOffset				= geometry.Positions.size();
	::gpk::SParamsGrid			paramsGrid					= {{}, {params.Slices, 1}, params.Reverse};
	gpk_necall(::gpk::geometryBuildGrid(geometry, paramsGrid), "params.Slices: %i", params.Slices);
	const uint32_t				vertexCount					= geometry.Positions.size() - vertexOffset;
	const double				sliceScale					= ::gpk::math_2pi * params.Circumference;
	geometry.Positions.enumerate([sliceScale, params, &geometry](const uint32_t & index, const ::gpk::n3f32 & position) {
			const double				weight						= position.x * sliceScale;
			const ::gpk::SSinCos		sinCos						= {sin(weight), -cos(weight)};
			const ::gpk::n3f64			relativePosOuter			= {sinCos.Sin * params.Radius.Max, 0, sinCos.Cos * params.Radius.Max};
			const ::gpk::n3f64			relativePosInner			= {sinCos.Sin * params.Radius.Min, 0, sinCos.Cos * params.Radius.Min};
			::gpk::n3f32				& posOuter					= geometry.Positions[index];
			::gpk::n3f32				& posInner					= geometry.Positions[index + params.Slices + 1];
			posInner				= params.Orientation.RotateVector((relativePosInner.f32() * -1.f) - params.Origin);
			posOuter				= params.Orientation.RotateVector((relativePosOuter.f32() * -1.f) - params.Origin);
		}, vertexOffset, vertexOffset + (vertexCount >> 1)
	);
	return 0;
}

::gpk::error_t			gpk::geometryBuildRingFlat	(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsRingSide & params) {
	gpk_necs(::gpk::geometryBuildRingSide(geometry, params));
	::gpk::SParamsRingSide		paramsSide					= params;
	paramsSide.Reverse		= true;
	gpk_necs(::gpk::geometryBuildRingSide(geometry, paramsSide));
	return 0;
}

::gpk::error_t			gpk::geometryBuildDisc(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsDisc & params) {
	::gpk::SParamsCircle		paramsSide					= params;
	gpk_necs(::gpk::geometryBuildCircleSide(geometry, params));
	paramsSide.Reverse		= true;
	gpk_necs(::gpk::geometryBuildCircleSide(geometry, paramsSide));
	return 0;
}

::gpk::error_t			gpk::geometryBuildRing		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsTube & params) {
	::gpk::SParamsRingSide		paramsSide					= {};
	paramsSide.Origin		= params.Origin;
	paramsSide.Slices		= params.CellCount.x;
	paramsSide.Radius		= params.RadiusYMin;
	paramsSide.Reverse		= !params.Reverse;	// looking down
	paramsSide.Circumference= params.Circumference;
	paramsSide.Orientation	= params.Orientation;
	gpk_necs(::gpk::geometryBuildRingSide(geometry, paramsSide));
	{
		::gpk::SParamsCylinderWall	paramsWall					= {};
		paramsWall.Origin		= params.Origin;
		paramsWall.CellCount	= params.CellCount;
		paramsWall.Radius		= {params.RadiusYMin.Max, params.RadiusYMax.Max};
		paramsWall.Circumference= params.Circumference;
		paramsWall.Height		= params.Height;
		paramsWall.Orientation	= params.Orientation;
		paramsWall.Reverse		= params.Reverse;
		gpk_necs(::gpk::geometryBuildCylinderWall(geometry, paramsWall));
		////
		paramsWall.Radius		= {params.RadiusYMin.Min, params.RadiusYMax.Min};
		paramsWall.Reverse		= !paramsWall.Reverse;
		gpk_necs(::gpk::geometryBuildCylinderWall(geometry, paramsWall));
	}
	//
	paramsSide.Origin.y		-= params.Height;
	paramsSide.Radius		= params.RadiusYMax;
	paramsSide.Reverse		= !paramsSide.Reverse;	// looking up
	gpk_necs(::gpk::geometryBuildRingSide(geometry, paramsSide));
	return 0;
}

::gpk::error_t			gpk::geometryBuildCylinder		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsCylinderWall & params) {
	::gpk::SParamsCircle		cap							= {};
	cap.Origin				= params.Origin;
	cap.Radius				= params.Radius.Min;
	cap.Reverse				= !params.Reverse;
	cap.Orientation			= params.Orientation;
	gpk_necs(::gpk::geometryBuildCircleSide(geometry, cap));

	gpk_necs(::gpk::geometryBuildCylinderWall(geometry, params));

	cap.Reverse				= !cap.Reverse;
	cap.Radius				= params.Radius.Max;
	cap.Origin.y			-= params.Height;
	gpk_necs(::gpk::geometryBuildCircleSide(geometry, cap));
	return 0;
}

