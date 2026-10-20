#ifndef GPK_ATMEL

#include "gpk_geometry.h"
#include "gpk_voxel.h"
#include "gpk_view_n3.h"
#include "gpk_sphere.h"

::gpk::error_t			gpk::geometryBuildBox		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsBox & params) {
	const ::gpk::u2_t				offsetPositions				= geometry.Positions		.size(); geometry.Positions			.resize(offsetPositions			+ 24);
	const ::gpk::u2_t				offsetNormals				= geometry.Normals			.size(); geometry.Normals			.resize(offsetNormals			+ 24);
	const ::gpk::u2_t				offsetTextureCoords			= geometry.TextureCoords	.size(); geometry.TextureCoords		.resize(offsetTextureCoords		+ 24);
	const ::gpk::u2_t				offsetPositionIndices		= geometry.PositionIndices	.size(); geometry.PositionIndices	.resize(offsetPositionIndices	+ 36);
	const ::gpk::vcn3f2_t			vertices					= {&::gpk::VOXEL_FACE_VERTICES[0].A, 24};
	for(::gpk::u2_t iVertex = offsetPositions; iVertex < geometry.Positions.size(); ++iVertex)
		geometry.Positions[iVertex]	= params.Orientation.RotateVector(vertices[iVertex - offsetPositions].Scaled(params.HalfSizes * 2.0f) - params.Origin);

	memcpy(&geometry.Normals		[offsetNormals		], ::gpk::VOXEL_FACE_NORMALS.begin(), ::gpk::VOXEL_FACE_NORMALS	.byte_count());
	for(::gpk::u2_t iVertex = offsetNormals; iVertex < geometry.Normals.size(); ++iVertex)
		geometry.Normals[iVertex]	= params.Orientation.RotateVector(geometry.Normals[iVertex]);
	memcpy(&geometry.TextureCoords	[offsetTextureCoords], ::gpk::VOXEL_FACE_UV.begin()		, ::gpk::VOXEL_FACE_UV		.byte_count());
	const ::gpk::vcu1_t			indices						= {::gpk::VOXEL_FACE_INDICES_16[0], 36};
	for(::gpk::u2_t iIndex = offsetPositionIndices; iIndex < geometry.PositionIndices.size(); ++iIndex) 
		geometry.PositionIndices[iIndex] = offsetPositions + indices[iIndex - offsetPositionIndices];

	return 0;
}

enum SQUARE_MODE 
	{ SQUARE_MODE_TOP_LEFT
	, SQUARE_MODE_TOP_RIGHT
	};

tplt<tpnm _tIndex>
static	::gpk::error_t	geometryBuildGridIndices	(::gpk::apod<_tIndex> & positionIndices, ::gpk::u2_t vertexOffset, const ::gpk::n2u1_t cellCount, bool counterClockwise, bool outward) {
	const ::gpk::u1_t				pitch						= cellCount.x + 1;								// 2 3 
	const ::gpk::astu2_t<4>		indices						= counterClockwise ? ::gpk::astu2_t<4>{{1U, 0U, ::gpk::u2_t(pitch + 1U), pitch}} : ::gpk::astu2_t<4>{{0U, 1U, pitch, ::gpk::u2_t(pitch + 1U)}};	// 0 1 2 3
	const ::gpk::u2_t				indices_modes[2][6]			=
		{ {indices[0], indices[2], indices[3], indices[0], indices[3], indices[1]}
		, {indices[0], indices[2], indices[1], indices[1], indices[2], indices[3]}
		};
	const ::gpk::n2u1_t			halfCount					= {::gpk::u1_t(cellCount.x >> 1U), ::gpk::u1_t(cellCount.y >> 1U)};
	cellCount.enumerate([&positionIndices, halfCount, indices_modes, vertexOffset, outward](::gpk::u2_t & vertexIndex, ::gpk::n2u1_t & coord) {
		const bool					reverseQuad 
			= ( (coord.x >= halfCount.x && coord.y <  halfCount.y)
			 || (coord.x <  halfCount.x && coord.y >= halfCount.y)
			);
		const ::gpk::vcu2_t			relative_indices			= indices_modes[((reverseQuad ? SQUARE_MODE_TOP_RIGHT : SQUARE_MODE_TOP_LEFT) + one_if(outward)) % 2];
		for(::gpk::u2_t i = 0; i < 6; ++i) {
			positionIndices.push_back(vertexOffset + vertexIndex + relative_indices[i]);
		}
	}, [](::gpk::u2_t & vertexIndex, ::gpk::n2u1_t & ) { 
		++vertexIndex; 
	});
	return 0;
}
::gpk::error_t			gpk::geometryBuildGrid		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsGrid & params) {
	const ::gpk::u2_t				vertexOffset				= geometry.Positions.size();
	const ::gpk::n2u1_t			vertexCount					= params.CellCount + ::gpk::n2u1_t{1, 1};

	// -- Generate texture coordinates
	const ::gpk::n2f3_t			cellUnits					= {1.0f / params.CellCount.x, 1.0f / params.CellCount.y};
	vertexCount.for_each([&geometry, &params, cellUnits](::gpk::n2u1_t & coord) { geometry.TextureCoords.push_back(::gpk::n2u2_t{coord.x, (::gpk::u2_t)params.CellCount.y - coord.y}.f2().InPlaceScale(cellUnits).f2()); });

	// -- Generate normals
	geometry.Normals.resize(geometry.Normals.size() + vertexCount.Area(), params.Orientation.RotateVector({0, params.Reverse ? -1.0f : 1.0f, 0}));	// The normals are all the same for grids

	// -- Generate positions
	const ::gpk::n2f3_t			scale						= params.Size.f3().InPlaceScale(cellUnits);
	vertexCount.for_each([&geometry, scale, params](::gpk::n2u1_t & coord) {
		const ::gpk::n3f3_t			position					= {coord.x * scale.x, 0, coord.y * scale.y};
		geometry.Positions.push_back(params.Orientation.RotateVector(position.f2() - params.Origin));
	});
	return ::geometryBuildGridIndices(geometry.PositionIndices, vertexOffset, params.CellCount, params.Reverse, params.Outward); 
}
::gpk::error_t			gpk::geometryBuildSphere	(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsSphere & params) {
	const ::gpk::u2_t				 vertexOffset				= geometry.Positions.size();
	const ::gpk::n2u1_t			vertexCount					= params.CellCount + ::gpk::n2u1_t{1, 1};

	// -- Generate texture coordinates
	const ::gpk::n2f3_t			cellUnits					= {1.0f / params.CellCount.x, 1.0f / params.CellCount.y};
	vertexCount.for_each([&geometry, &params, cellUnits](::gpk::n2u1_t & coord) { geometry.TextureCoords.push_back(::gpk::n2u2_t{coord.x, (::gpk::u2_t)params.CellCount.y - coord.y}.f3().InPlaceScale(cellUnits).f2()); });

	// -- Generate normals and positions
	const double				reverseScale				= params.Reverse ? -1.0 : 1.0;
	const ::gpk::n2f3_t			sliceScale					= ::gpk::n2f3_t{::gpk::math_2pi * cellUnits.x * reverseScale * params.Circumference, ::gpk::math_pi * cellUnits.y};
	const double				sliceOffset					= 0; //::gpk::math_pi * (1.0 - params.Circumference) * .5;
	for(::gpk::u2_t y = 0; y < vertexCount.y; ++y) {
		const double				currentY					= sliceScale.y * y;
		const double				currentRadius				= sin(currentY);
		for(::gpk::u2_t x = 0; x < vertexCount.x; ++x) {
			const double				currentX					= sliceScale.x * x + sliceOffset;
			const ::gpk::n3f3_t			coord 						= ::gpk::n3f3_t{currentRadius * cos(currentX), -cos(currentY), currentRadius * sin(currentX)};
			gpk_necs(geometry.Normals	.push_back(params.Orientation.RotateVector((coord * reverseScale).f2())));
			gpk_necs(geometry.Positions	.push_back(params.Orientation.RotateVector((coord * params.Radius).f2() - params.Origin)));
		}
	}
	return ::geometryBuildGridIndices(geometry.PositionIndices, vertexOffset, params.CellCount, false, false); 
}
::gpk::error_t			gpk::geometryBuildCircleSide(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsCircle & params) {
	const ::gpk::u2_t				vertexOffset				= geometry.Positions.size();
	const ::gpk::u2_t				vertexCount					= (::gpk::u2_t)params.Slices + 1;
	const double				cellUnits					= 1.0 / params.Slices;
	const double				reverseScale				= params.Reverse ? -1.0 : 1.0;
	const double				sliceScale					= cellUnits * ::gpk::math_2pi * params.Circumference * reverseScale;
	const double				sliceOffset					= 0; //::gpk::math_2pi * (1.0 - params.Circumference);

	// -- Generate positions and texture coordinates
	gpk_necs(geometry.Positions		.push_back(params.Orientation.RotateVector(-params.Origin)));	// The first vertex is the center of the circle
	gpk_necs(geometry.TextureCoords	.push_back(::gpk::n2f2_t{.5f, .5f}));					// The first vertex is the center of the circle
	for(::gpk::u2_t iVertex = 0; iVertex < vertexCount; ++iVertex) {
		double						angle						= sliceScale * iVertex + sliceOffset;
		gpk_necs(geometry.Positions		.push_back(params.Orientation.RotateVector(::gpk::n3f3_t{params.Radius}.RotateY(angle).f2() - params.Origin)));
		gpk_necs(geometry.TextureCoords	.push_back(::gpk::n2f3_t{.5}.Rotate(angle).f2() + ::gpk::n2f2_t{.5f, .5f}));
	}

	// -- Generate normals
	gpk_necs(geometry.Normals.resize(geometry.Normals.size() + vertexCount + 1, params.Orientation.RotateVector({0, 1, 0})));	// The normals are all the same for a circle

	for(::gpk::u2_t iSlice = 0, sliceCount = params.Slices + 1; iSlice < sliceCount; ++iSlice)
		gpk_necs(geometry.PositionIndices.append({vertexOffset + 0, vertexOffset + iSlice + 1, vertexOffset + iSlice + 0}));

	return 0; 
}
::gpk::error_t			gpk::geometryBuildCylinderWall	(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsCylinderWall & params) {
	const ::gpk::u2_t				vertexOffset				= geometry.Positions.size();
	const ::gpk::n2u1_t			vertexCount					= params.CellCount + ::gpk::n2u1_t{1, 1};

	// -- Generate texture coordinates
	const ::gpk::n2f3_t			cellUnits					= {1.0f / params.CellCount.x, 1.0f / params.CellCount.y};
	vertexCount.for_each([&geometry, &params, cellUnits](::gpk::n2u1_t & coord) { geometry.TextureCoords.push_back(::gpk::n2u2_t{coord.x, (::gpk::u2_t)params.CellCount.y - coord.y}.f3().InPlaceScale(cellUnits).f2()); });

	// -- Generate normals
	const double				reverseScale				= params.Reverse ? -1.0 : 1.0;
	const double				lengthScale					= cellUnits.y * params.Height;
	const ::gpk::n3f3_t			normalBase					= gpk::n3f3_t{params.Height, -(params.Radius.Max - params.Radius.Min)}.Normalize() * reverseScale;
	const double				sliceScale					= cellUnits.x * ::gpk::math_2pi * params.Circumference * reverseScale;
	const double				sliceOffset					= 0; //::gpk::math_pi * (1.0 - params.Circumference) * .5;
	vertexCount.for_each([&geometry, &normalBase, sliceScale, sliceOffset, &params](::gpk::n2u1_t & coord) { geometry.Normals.push_back(params.Orientation.RotateVector((::gpk::n3f3_t{normalBase}.RotateY(sliceScale * coord.x + sliceOffset)).f2())); });

	// -- Generate positions
	for(::gpk::u2_t y = 0; y < vertexCount.y; ++y) {
		const double				radius						= ::gpk::interpolate_linear(params.Radius.Min, params.Radius.Max, cellUnits.y * y);
		for(::gpk::u2_t x = 0; x < vertexCount.x; ++x)
			geometry.Positions.push_back(params.Orientation.RotateVector(::gpk::n3f3_t{radius, (double)y * lengthScale}.RotateY(sliceScale * x + sliceOffset).f2() - params.Origin));
	}
	return ::geometryBuildGridIndices(geometry.PositionIndices, vertexOffset, params.CellCount, false, false); 
}
//
::gpk::error_t			gpk::geometryBuildFigure0	(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsHelix & params) { //(::gpk::SGeometryQuads & geometry, ::gpk::u2_t stacks, ::gpk::u2_t slices, float radius, const ::gpk::n3f2_t & gridCenter)	{
	const ::gpk::u2_t				vertexOffset				= geometry.Positions.size();
	const ::gpk::n2u1_t			vertexCount					= params.CellCount + ::gpk::n2u1_t{1, 1};

	// -- Generate texture coordinates
	const ::gpk::n2f3_t			cellUnits					= {1.0f / params.CellCount.x, 1.0f / params.CellCount.y};
	geometry.TextureCoords	.reserve(vertexCount.Area());
	geometry.Positions		.reserve(vertexCount.Area());
	geometry.Normals		.reserve(vertexCount.Area());
	vertexCount.for_each([&geometry, &params, cellUnits](::gpk::n2u1_t & coord) { geometry.TextureCoords.push_back(::gpk::n2u2_t{coord.x, (::gpk::u2_t)params.CellCount.y - coord.y}.f2().InPlaceScale(cellUnits).f2()); });

	//const double				lengthScale					= cellUnits.y * params.Length;
	const ::gpk::n2f3_t			sliceScale					= ::gpk::n2f3_t{::gpk::math_2pi * cellUnits.x, ::gpk::math_pi * cellUnits.y};
	for(::gpk::u2_t y = 0; y < vertexCount.y; ++y) {
		const double				radius						= ::gpk::interpolate_linear(params.Radius.Min, params.Radius.Max, cellUnits.y * y);
		for(::gpk::u2_t x = 0; x < vertexCount.x; ++x) {
			::gpk::n3f3_t				position					= {sin(sliceScale.y * y) * sin(sliceScale.x * x) * radius, sin(sliceScale.y * y) * sin(::gpk::math_pi * x / params.CellCount.x) * params.Length, cos(sliceScale.x * x) * radius};
			geometry.Positions.push_back(params.Orientation.RotateVector(position.f2() - params.Origin));
		}
	}
	const ::gpk::u2_t				indexOffset				= geometry.PositionIndices.size();
	gpk_necs(::geometryBuildGridIndices(geometry.PositionIndices, vertexOffset, params.CellCount, false, false)); 
	for(::gpk::u2_t iVertexIndex = indexOffset; iVertexIndex < geometry.PositionIndices.size(); iVertexIndex += 3) {
		const ::gpk::n3f2_t	a	= geometry.Positions[geometry.PositionIndices[iVertexIndex + 0]];
		const ::gpk::n3f2_t	b	= geometry.Positions[geometry.PositionIndices[iVertexIndex + 1]];
		const ::gpk::n3f2_t	c	= geometry.Positions[geometry.PositionIndices[iVertexIndex + 2]];
		geometry.Normals.push_back(params.Orientation.RotateVector((a - b).Normalize().Cross((a - c).Normalize()).Normalize()));
	}
	return 0;
}
::gpk::error_t			gpk::geometryBuildHelixHalf	(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsHelix & params) {
	const ::gpk::u2_t				vertexOffset				= geometry.Positions.size();
	const ::gpk::n2u1_t			vertexCount					= params.CellCount + ::gpk::n2u1_t{1, 1};

	// -- Generate texture coordinates
	const ::gpk::n2f3_t			cellUnits					= {1.0f / params.CellCount.x, 1.0f / params.CellCount.y};
	geometry.TextureCoords	.reserve(vertexCount.Area());
	geometry.Positions		.reserve(vertexCount.Area());
	geometry.Normals		.reserve(vertexCount.Area());
	vertexCount.for_each([&geometry, &params, cellUnits](::gpk::n2u1_t & coord) { geometry.TextureCoords.push_back(::gpk::n2u2_t{coord.x, (::gpk::u2_t)params.CellCount.y - coord.y}.f2().InPlaceScale(cellUnits).f2()); });

	//const double				lengthScale					= cellUnits.y * params.Length;
	const ::gpk::n2f3_t			sliceScale					= ::gpk::n2f3_t{::gpk::math_2pi * cellUnits.x, ::gpk::math_pi * cellUnits.y};
	for(::gpk::u2_t y = 0; y < vertexCount.y; ++y) {
		const double				radius						= ::gpk::interpolate_linear(params.Radius.Min, params.Radius.Max, cellUnits.y * y);
		for(::gpk::u2_t x = 0; x < vertexCount.x; ++x) {
			::gpk::n3f3_t				position					= {sin(sliceScale.y * y) * sin(sliceScale.x * x) * radius, sin(sliceScale.y * y) * cos(::gpk::math_pi * x / params.CellCount.x) * params.Length, cos(sliceScale.x * x) * radius};
			geometry.Positions.push_back(params.Orientation.RotateVector(position.f2() - params.Origin));
		}
	}

	const ::gpk::u2_t				indexOffset				= geometry.PositionIndices.size();
	gpk_necs(::geometryBuildGridIndices(geometry.PositionIndices, vertexOffset, params.CellCount, false, false)); 
	for(::gpk::u2_t iVertexIndex = indexOffset; iVertexIndex < geometry.PositionIndices.size(); iVertexIndex += 3) {
		const ::gpk::n3f2_t	a	= geometry.Positions[geometry.PositionIndices[iVertexIndex + 0]];
		const ::gpk::n3f2_t	b	= geometry.Positions[geometry.PositionIndices[iVertexIndex + 1]];
		const ::gpk::n3f2_t	c	= geometry.Positions[geometry.PositionIndices[iVertexIndex + 2]];
		geometry.Normals.push_back(params.Orientation.RotateVector((a - b).Normalize().Cross((a - c).Normalize()).Normalize()));
	}
	return 0;
}
::gpk::error_t			gpk::geometryBuildHelix		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsHelix & params)	{
	const ::gpk::u2_t				vertexOffset				= geometry.Positions.size();
	const ::gpk::u2_t				normalOffset				= geometry.Normals.size();
	const ::gpk::u2_t				indexOffset				= geometry.PositionIndices.size();
	gpk_necs(gpk::geometryBuildHelixHalf(geometry, params));
	const ::gpk::u2_t				vertexCount					= geometry.Positions.size() - vertexOffset;
	const ::gpk::u2_t				normalCount					= geometry.Normals.size() - normalOffset;
	const ::gpk::u2_t				indexCount					= geometry.PositionIndices.size() - indexOffset;
	gpk_necs(geometry.TextureCoords	.append({&geometry.TextureCoords[vertexOffset], vertexCount}));
	gpk_necs(geometry.Positions		.append({&geometry.Positions	[vertexOffset], vertexCount}));
	gpk_necs(geometry.Normals		.append({&geometry.Normals		[normalOffset], normalCount}));
	for(::gpk::u2_t iIndex = 0; iIndex < indexCount; ++iIndex)
		gpk_necs(geometry.PositionIndices.push_back(geometry.PositionIndices[indexOffset + iIndex] + vertexCount));
	geometry.Positions	.for_each([](::gpk::n3f2_t & coord){ coord.z *= -1; coord.y *= -1; return 0; }, vertexOffset + vertexCount);
	geometry.Normals	.for_each([](::gpk::n3f2_t & coord){ coord.z *= -1; coord.y *= -1; return 0; }, normalOffset + normalCount);
	return 0;
}
::gpk::error_t			gpk::geometryBuildRingSide	(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsRingSide & params) {
	const ::gpk::u2_t				vertexOffset				= geometry.Positions.size();
	::gpk::SParamsGrid			paramsGrid					= {{}, {params.Slices, 1}, params.Reverse};
	gpk_necall(gpk::geometryBuildGrid(geometry, paramsGrid), "params.Slices: %i", params.Slices);
	const ::gpk::u2_t				vertexCount					= geometry.Positions.size() - vertexOffset;
	const double				sliceScale					= ::gpk::math_2pi * params.Circumference;
	geometry.Positions.enumerate([sliceScale, params, &geometry](const ::gpk::u2_t & index, const ::gpk::n3f2_t & position) {
			const double				weight						= position.x * sliceScale;
			const ::gpk::SSinCos		sinCos						= {sin(weight), -cos(weight)};
			const ::gpk::n3f3_t			relativePosOuter			= {sinCos.Sin * params.Radius.Max, 0, sinCos.Cos * params.Radius.Max};
			const ::gpk::n3f3_t			relativePosInner			= {sinCos.Sin * params.Radius.Min, 0, sinCos.Cos * params.Radius.Min};
			::gpk::n3f2_t				& posOuter					= geometry.Positions[index];
			::gpk::n3f2_t				& posInner					= geometry.Positions[index + params.Slices + 1];
			posInner				= params.Orientation.RotateVector((relativePosInner.f2() * -1.f) - params.Origin);
			posOuter				= params.Orientation.RotateVector((relativePosOuter.f2() * -1.f) - params.Origin);
			return 0; 
		}, vertexOffset, vertexOffset + (vertexCount >> 1)
	);
	return 0;
}
::gpk::error_t			gpk::geometryBuildRingFlat	(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsRingSide & params) {
	gpk_necs(gpk::geometryBuildRingSide(geometry, params));
	::gpk::SParamsRingSide		paramsSide					= params;
	paramsSide.Reverse		= true;
	gpk_necs(gpk::geometryBuildRingSide(geometry, paramsSide));
	return 0;
}
::gpk::error_t			gpk::geometryBuildDisc(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsDisc & params) {
	::gpk::SParamsCircle		paramsSide					= params;
	gpk_necs(gpk::geometryBuildCircleSide(geometry, params));
	paramsSide.Reverse		= true;
	gpk_necs(gpk::geometryBuildCircleSide(geometry, paramsSide));
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
	gpk_necs(gpk::geometryBuildRingSide(geometry, paramsSide));
	{
		::gpk::SParamsCylinderWall	paramsWall					= {};
		paramsWall.Origin		= params.Origin;
		paramsWall.CellCount	= params.CellCount;
		paramsWall.Radius		= {params.RadiusYMin.Max, params.RadiusYMax.Max};
		paramsWall.Circumference= params.Circumference;
		paramsWall.Height		= params.Height;
		paramsWall.Orientation	= params.Orientation;
		paramsWall.Reverse		= params.Reverse;
		gpk_necs(gpk::geometryBuildCylinderWall(geometry, paramsWall));
		////
		paramsWall.Radius		= {params.RadiusYMin.Min, params.RadiusYMax.Min};
		paramsWall.Reverse		= !paramsWall.Reverse;
		gpk_necs(gpk::geometryBuildCylinderWall(geometry, paramsWall));
	}
	//
	paramsSide.Origin.y		-= params.Height;
	paramsSide.Radius		= params.RadiusYMax;
	paramsSide.Reverse		= !paramsSide.Reverse;	// looking up
	gpk_necs(gpk::geometryBuildRingSide(geometry, paramsSide));
	return 0;
}
::gpk::error_t			gpk::geometryBuildCylinder		(::gpk::SGeometryBuffers & geometry, const ::gpk::SParamsCylinderWall & params) {
	::gpk::SParamsCircle		cap							= {};
	cap.Origin				= params.Origin;
	cap.Radius				= params.Radius.Min;
	cap.Reverse				= !params.Reverse;
	cap.Orientation			= params.Orientation;
	gpk_necs(gpk::geometryBuildCircleSide(geometry, cap));

	gpk_necs(gpk::geometryBuildCylinderWall(geometry, params));

	cap.Reverse				= !cap.Reverse;
	cap.Radius				= params.Radius.Max;
	cap.Origin.y			-= params.Height;
	gpk_necs(gpk::geometryBuildCircleSide(geometry, cap));
	return 0;
}
#endif


