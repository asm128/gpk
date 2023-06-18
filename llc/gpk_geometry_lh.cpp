#include "gpk_geometry_lh.h"

::gpk::error_t			gpk::geometryBuildTileListFromImage		(::gpk::gc8bgra image, ::gpk::apod<::gpk::STile> & out_tiles, uint32_t imagePitch)	{
	{
		::gpk::STile											newTile;
		for(uint32_t z = 0, maxZ = image.metrics().y; z < maxZ ; ++z)
		for(uint32_t x = 0, maxX = image.metrics().x; x < maxX ; ++x) {
			::gpk::rgbaf										currentPixel							= (0 == imagePitch) ? image[z][x] : image.begin()[z * imagePitch + x];
			float													pixelHeight								= float((currentPixel.r + (double)currentPixel.g + currentPixel.b) / 3.0);
			for(uint32_t iCorner = 0; iCorner < 4; ++iCorner)
				newTile.Height[iCorner]								= pixelHeight;
			newTile.Top											= 1;
			newTile.Front										= -1;
			newTile.Right										= -1;
			newTile.Flags										= 0;
			out_tiles.push_back(newTile);
		}
	}
	for(uint32_t z = 0; z < image.metrics().y; ++z)
	for(uint32_t x = 0; x < image.metrics().x - 1; ++x) {
		::gpk::STile											& currentTile							= out_tiles[z * image.metrics().y + x];
		::gpk::STile											& frontTile								= out_tiles[z * image.metrics().y + x + 1];
		if(currentTile.Height[1] != frontTile.Height[0] || currentTile.Height[3] != frontTile.Height[2])
			currentTile.Front									= 1;
	}
	for(uint32_t z = 0; z < image.metrics().y - 1; ++z)
	for(uint32_t x = 0; x < image.metrics().x; ++x) {
		::gpk::STile											& currentTile							= out_tiles[z		* image.metrics().y + x];
		::gpk::STile											& rightTile								= out_tiles[(z + 1)	* image.metrics().y + x];
		if(currentTile.Height[2] != rightTile.Height[0] || currentTile.Height[3] != rightTile.Height[1])
			currentTile.Right									= 1;
	}
	return 0;
}

::gpk::error_t			gpk::geometryBuildGridFromTileList		(::gpk::SGeometryQuads & geometry, ::gpk::grid<const ::gpk::STile> tiles, ::gpk::n2f32 gridCenter, const ::gpk::n3f32 & scale) {
	::gpk::n2f32									texCoordUnits						= {1.0f / tiles.metrics().x, 1.0f / tiles.metrics().y};
	for(uint32_t z = 0; z < tiles.metrics().y; ++z)
	for(uint32_t x = 0; x < tiles.metrics().x; ++x) {
		::gpk::n2f32									texcoords	[4]				=
			{ {(x		) * texCoordUnits.x, (z		) * texCoordUnits.y}
			, {(x + 1	) * texCoordUnits.x, (z		) * texCoordUnits.y}
			, {(x		) * texCoordUnits.x, (z + 1	) * texCoordUnits.y}
			, {(x + 1	) * texCoordUnits.x, (z + 1	) * texCoordUnits.y}
			};
		const ::gpk::STile										& currentTile							= tiles[z][x];
		{
			geometry.TextureCoords.push_back(
				{ texcoords[0]
				, texcoords[2]
				, texcoords[1]
				});
			geometry.TextureCoords.push_back(
				{ texcoords[1]
				, texcoords[2]
				, texcoords[3]
				});
		}
		{ // top
			::gpk::n3f32									coords	[4]			=
				{ {x + 0.f, currentTile.Height[0], (z + 0) * -1.f}
				, {x + 0.f, currentTile.Height[1], (z + 1) * -1.f}
				, {x + 1.f, currentTile.Height[2], (z + 0) * -1.f}
				, {x + 1.f, currentTile.Height[3], (z + 1) * -1.f}
				};
			if(coords[0].y || coords[1].y || coords[2].y || coords[3].y) {
				::gpk::tri3f32								triangleA			= {coords[0], coords[2], coords[1]};//
				::gpk::tri3f32								triangleB			= {coords[1], coords[2], coords[3]};//
				triangleA.Scale(scale);
				triangleB.Scale(scale);
				triangleA.Translate({-gridCenter.x, 0, -gridCenter.y});
				triangleB.Translate({-gridCenter.x, 0, -gridCenter.y});
				geometry.Triangles	.push_back(triangleA);
				geometry.Triangles	.push_back(triangleB);
				geometry.Normals.push_back((triangleB.A - triangleB.B).Normalize().Cross((triangleA.A - triangleA.B).Normalize()).Normalize().f32());
				//geometry.Normals.push_back((triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().f32());
			}
		}
		if(0 <= currentTile.Front || (x == (tiles.metrics().x - 1) && (0 != currentTile.Height[1] || 0 != currentTile.Height[3]))) {
			::gpk::n3f32									coords[4];
			if(0 <= currentTile.Front) { // front
				const ::gpk::STile										& frontTile			= tiles[z][x + 1];
				coords[0]											= {x + 1.f, currentTile.Height	[1], (z + 0) * -1.f};
				coords[1]											= {x + 1.f, currentTile.Height	[3], (z + 1) * -1.f};
				coords[2]											= {x + 1.f, frontTile.Height	[0], (z + 0) * -1.f};
				coords[3]											= {x + 1.f, frontTile.Height	[2], (z + 1) * -1.f};
			}
			else {
				coords[0]											= {x + 1.f, currentTile.Height	[1], (z + 0) * -1.f};
				coords[1]											= {x + 1.f, currentTile.Height	[3], (z + 1) * -1.f};
				coords[2]											= {x + 1.f, 0, (z + 0) * -1.f};
				coords[3]											= {x + 1.f, 0, (z + 1) * -1.f};
			}
			::gpk::tri3f32								triangleA			= {coords[0], coords[2], coords[1]};//{coords[0], coords[2], coords[1]};
			::gpk::tri3f32								triangleB			= {coords[1], coords[2], coords[3]};//{coords[1], coords[2], coords[3]};
			triangleA.Scale(scale);
			triangleB.Scale(scale);
			triangleA.Translate({-gridCenter.x, 0, -gridCenter.y});
			triangleB.Translate({-gridCenter.x, 0, -gridCenter.y});
			geometry.Triangles	.push_back(triangleA);
			geometry.Triangles	.push_back(triangleB);
			//geometry.Normals.push_back((triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().f32());
			geometry.Normals.push_back((triangleB.A - triangleB.B).Normalize().Cross((triangleA.A - triangleA.B).Normalize()).Normalize().f32());
			geometry.TextureCoords.push_back(
				{ texcoords[0]
				, texcoords[2]
				, texcoords[1]
				});
			geometry.TextureCoords.push_back(
				{ texcoords[1]
				, texcoords[2]
				, texcoords[3]
				});
		}
		if(0 <= currentTile.Right || ((z == tiles.metrics().y - 1) && (0 != currentTile.Height[2] || 0 != currentTile.Height[3]))) {
			::gpk::n3f32									coords[4];
			if(0 <= currentTile.Right) {
				const ::gpk::STile										& rightTile			= tiles[z + 1][x];
				coords[0]											= {x + 0.f, currentTile.Height	[2], (z + 1) * -1.f};
				coords[1]											= {x + 1.f, currentTile.Height	[3], (z + 1) * -1.f};
				coords[2]											= {x + 0.f, rightTile.Height	[0], (z + 1) * -1.f};
				coords[3]											= {x + 1.f, rightTile.Height	[1], (z + 1) * -1.f};
			}
			else {
				coords[0]											= {x + 0.f, currentTile.Height	[2], (z + 1) * -1.f};
				coords[1]											= {x + 1.f, currentTile.Height	[3], (z + 1) * -1.f};
				coords[2]											= {x + 0.f, 0, (z + 1) * -1.f};
				coords[3]											= {x + 1.f, 0, (z + 1) * -1.f};
			}
			::gpk::tri3f32								triangleA			= {coords[0], coords[1], coords[2]};
			::gpk::tri3f32								triangleB			= {coords[1], coords[3], coords[2]};
			triangleA.Scale(scale);
			triangleB.Scale(scale);
			triangleA.Translate({-gridCenter.x, 0, -gridCenter.y});
			triangleB.Translate({-gridCenter.x, 0, -gridCenter.y});
			geometry.Triangles	.push_back(triangleA);
			geometry.Triangles	.push_back(triangleB);
			geometry.Normals.push_back((triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().f32());
			//geometry.Normals.push_back((triangleB.A - triangleB.B).Normalize().Cross((triangleA.A - triangleA.B).Normalize()).Normalize().f32());
			geometry.TextureCoords.push_back(
				{ texcoords[0]
				, texcoords[2]
				, texcoords[1]
				});
			geometry.TextureCoords.push_back(
				{ texcoords[1]
				, texcoords[2]
				, texcoords[3]
				});
		}
	}
	return 0;
}




//stacxpr	::gpk::n3i8	geometryIndexedSquarePositions		[]			= {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}};
//stacxpr	int8_t		geometryIndexedSquareIndices		[]			= {0, 1, 2, 1, 3, 2};
//stacxpr	::gpk::n3i8	geometryIndexedSquareNormal			[]			= {{0, 0, 1}};
//stacxpr	int8_t		geometryIndexedSquareNormalIndices	[]			= {0};
//stacxpr	::gpk::n3i8	geometryIndexedSquareUV				[]			= {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}};
//
//stacxpr	int8_t		geometryIndexedSquareUVIndices		[]			= {0, 1, 2, 1, 3, 2};
//
//// Vertex coordinates for cube faces
//stacxpr	::gpk::n3i8	geometryIndexedCubePositions		[8]			=
//	{ {0, 0, 0}, {0, 1, 0}, {1, 0, 0}, {1, 1, 0}
//	, {0, 0, 1}, {0, 1, 1}, {1, 0, 1}, {1, 1, 1}
//	};
//
//stacxpr	int8_t		geometryIndexedCubeIndices			[12 * 3]	=
//	{ 0, 2, 1, 1, 2, 3 //
//	, 
//	};

// Vertex coordinates for cube faces
stacxpr	::gpk::tri3i8	geometryCube	[12]	=
	{ {{0, 0, 0}, {0, 1, 0}, {1, 0, 0}}	// Right	- first
	, {{0, 1, 0}, {1, 1, 0}, {1, 0, 0}}	// Right	- second

	, {{0, 1, 0}, {0, 0, 0}, {0, 0, 1}}	// Back		- first
	, {{0, 1, 1}, {0, 1, 0}, {0, 0, 1}}	// Back		- second

	, {{0, 0, 1}, {0, 0, 0}, {1, 0, 0}}	// Bottom	- first
	, {{1, 0, 1}, {0, 0, 1}, {1, 0, 0}}	// Bottom	- second

	, {{0, 1, 1}, {0, 0, 1}, {1, 0, 1}}	// Left		- first
	, {{1, 1, 1}, {0, 1, 1}, {1, 0, 1}}	// Left		- second

	, {{1, 1, 0}, {1, 1, 1}, {1, 0, 1}}	// Front	- first
	, {{1, 0, 0}, {1, 1, 0}, {1, 0, 1}}	// Front	- second

	, {{0, 1, 1}, {1, 1, 1}, {1, 1, 0}}	// Top		- first
	, {{0, 1, 0}, {0, 1, 1}, {1, 1, 0}}	// Top		- second
	};

// Vertex coordinates for cube faces
stacxpr	::gpk::tri2i8	texCoordCubeYUp	[12]	=
	{ {{0, 1}, {0, 0}, {1, 1}}	// Right	- first
	, {{0, 0}, {1, 0}, {1, 1}}	// Right	- second
	//{} //
	, {{1, 0}, {1, 1}, {0, 1}}	// Back		- first
	, {{0, 0}, {1, 0}, {0, 1}}	// Back		- second
	//{} //
	, {{0, 0}, {1, 0}, {1, 1}}	// Bottom	- first
	, {{0, 1}, {0, 0}, {1, 1}}	// Bottom	- second
	//{} //
	, {{1, 0}, {1, 1}, {0, 1}}	// Left		- second
	, {{0, 0}, {1, 0}, {0, 1}}	// Left		- first
	//{} //
	, {{0, 0}, {1, 0}, {1, 1}}	// Front	- first
	, {{0, 1}, {0, 0}, {1, 1}}	// Front	- second
	//{} //
	, {{1, 1}, {0, 1}, {0, 0}}	// Top		- first
	, {{1, 0}, {1, 1}, {0, 0}}	// Top		- second
	};

stacxpr	::gpk::n3i8		geometryNormals	[6]		=
	{ { 0, 0,-1} // Right
	, {-1, 0, 0} // Back
	, { 0,-1, 0} // Bottom
	, { 0, 0, 1} // Left
	, { 1, 0, 0} // Front
	, { 0, 1, 0} // Top
	};

::gpk::error_t			gpk::geometryBuildCube	(SGeometryQuads & geometry, const ::gpk::n3f32 & scale, const ::gpk::n3f32 & translation)	{
	const uint32_t											firstTriangle					= geometry.Triangles.size(); 
	gpk_necs(::gpk::geometryBuildCube(geometry, scale));
	for(uint32_t iTri = firstTriangle; iTri < geometry.Triangles.size(); ++iTri) 
		geometry.Triangles[iTri].Translate(translation);
	return 0;
}

::gpk::error_t			gpk::geometryBuildCube	(SGeometryQuads & geometry, const ::gpk::n3f32 & scale)	{
	const uint32_t				triangleOffset			= geometry.Triangles.size();
	geometry.Triangles		.resize(triangleOffset + (uint32_t)::gpk::size(geometryCube));
	geometry.Normals		.resize(triangleOffset + (uint32_t)::gpk::size(geometryNormals));
	geometry.TextureCoords	.resize(triangleOffset + (uint32_t)::gpk::size(texCoordCubeYUp));

	for(uint32_t iTriangle = 0, countTriangles = (uint32_t)::gpk::size(geometryCube); iTriangle < countTriangles; ++iTriangle) {
		::gpk::tri3f32				& newTriangle		= geometry.Triangles[triangleOffset + iTriangle];
		newTriangle				= geometryCube[iTriangle].f32();
		newTriangle.A			-= {.5, .5, .5};
		newTriangle.B			-= {.5, .5, .5};
		newTriangle.C			-= {.5, .5, .5};
		newTriangle.Scale(scale);

		::gpk::tri2f32				& newTriangleTex	= geometry.TextureCoords[triangleOffset + iTriangle];
		newTriangleTex			= ::texCoordCubeYUp[iTriangle].f32();

		::gpk::n3f32				& newNormal			= geometry.Normals[(triangleOffset + iTriangle) / 2];
		::gpk::n3f32				normal				= (newTriangle.A - newTriangle.B).Normalize().Cross((newTriangle.A - newTriangle.C).Normalize());
		normal.Normalize();
		newNormal				= normal; //geometryNormals[iTriangle / 2].f32();
	}
	return 0;
}

::gpk::error_t			gpk::geometryBuildGrid	(SGeometryQuads & geometry, ::gpk::n2u16 gridSize, ::gpk::n2f32 gridCenter, const ::gpk::n3f32 & scale)	{
	::gpk::n2f32				texCoordUnits			= {1.0f / gridSize.x, 1.0f / gridSize.y};
	for(uint32_t z = 0; z < gridSize.y; ++z)
	for(uint32_t x = 0; x < gridSize.x; ++x)  {
		::gpk::n3f32				coords	[4]				=
			{ {0, 0, 0}
			, {0, 0, 1}
			, {1, 0, 0}
			, {1, 0, 1}
			};
		::gpk::n2f32				texcoordsOffset			= {x * texCoordUnits.x, z * texCoordUnits.y};
		::gpk::n2f32				texcoords	[4]			=
			{ {0.0f * texCoordUnits.x + texcoordsOffset.x, 0.0f * texCoordUnits.y + texcoordsOffset.y}
			, {0.0f * texCoordUnits.x + texcoordsOffset.x, 1.0f * texCoordUnits.y + texcoordsOffset.y}
			, {1.0f * texCoordUnits.x + texcoordsOffset.x, 0.0f * texCoordUnits.y + texcoordsOffset.y}
			, {1.0f * texCoordUnits.x + texcoordsOffset.x, 1.0f * texCoordUnits.y + texcoordsOffset.y}
			};
		::gpk::tri2f32				triangleATex			= {texcoords[0], texcoords[1], texcoords[2]};
		::gpk::tri2f32				triangleBTex			= {texcoords[1], texcoords[3], texcoords[2]};
		::gpk::tri3f32				triangleA				= {coords[0], coords[1], coords[2]};
		::gpk::tri3f32				triangleB				= {coords[1], coords[3], coords[2]};
		triangleA.A				+= {(float)x, 0, (float)z};
		triangleA.B				+= {(float)x, 0, (float)z};
		triangleA.C				+= {(float)x, 0, (float)z};
		triangleB.A				+= {(float)x, 0, (float)z};
		triangleB.B				+= {(float)x, 0, (float)z};
		triangleB.C				+= {(float)x, 0, (float)z};
		triangleA.Scale(scale);
		triangleB.Scale(scale);

		triangleA.Translate(::gpk::n3f32{gridCenter.x, 0, gridCenter.y} * -1);
		triangleB.Translate(::gpk::n3f32{gridCenter.x, 0, gridCenter.y} * -1);
		geometry.Triangles		.push_back(triangleA);
		geometry.Triangles		.push_back(triangleB);
		geometry.TextureCoords	.push_back(triangleATex);
		geometry.TextureCoords	.push_back(triangleBTex);
		::gpk::n3f32				normal					= (triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize());
		normal.Normalize();
		geometry.Normals		.push_back(normal);
	}
	return 0;
}

//
::gpk::error_t			gpk::geometryBuildFigure0	(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3f32 & gridCenter)	{
	(void)radius;
	for(uint32_t z = 0; z < stacks; ++z)
	for(uint32_t x = 0; x < slices; ++x)  {
		::gpk::n2f32				texcoords	[4]			=
			{ {0, 0}
			, {0, 1}
			, {1, 0}
			, {1, 1}
			};
		::gpk::n3f64				coords	[4]				=
			{ {sin(::gpk::math_pi * z		/ stacks	) * cos(::gpk::math_2pi * x			/ slices), sin(::gpk::math_pi * z		/ stacks) * sin(::gpk::math_2pi * x			/ slices), cos(::gpk::math_pi * x		/slices)}
			, {sin(::gpk::math_pi * z		/ stacks	) * cos(::gpk::math_2pi * (x + 1)	/ slices), sin(::gpk::math_pi * z		/ stacks) * sin(::gpk::math_2pi * (x + 1)	/ slices), cos(::gpk::math_pi * (x + 1) /slices)}
			, {sin(::gpk::math_pi * (z + 1)	/ stacks	) * cos(::gpk::math_2pi * x			/ slices), sin(::gpk::math_pi * (z + 1)	/ stacks) * sin(::gpk::math_2pi * x			/ slices), cos(::gpk::math_pi * x		/slices)}
			, {sin(::gpk::math_pi * (z + 1)	/ stacks	) * cos(::gpk::math_2pi * (x + 1)	/ slices), sin(::gpk::math_pi * (z + 1)	/ stacks) * sin(::gpk::math_2pi * (x + 1)	/ slices), cos(::gpk::math_pi * (x + 1)	/slices)}
			};
		::gpk::tri3f32				triangleA				= {coords[0].f32() * radius, coords[1].f32() * radius, coords[2].f32() * radius};
		::gpk::tri3f32				triangleB				= {coords[1].f32() * radius, coords[3].f32() * radius, coords[2].f32() * radius};
		::gpk::tri2f32				triangleATex			= {texcoords[0], texcoords[1], texcoords[2]};
		::gpk::tri2f32				triangleBTex			= {texcoords[1], texcoords[3], texcoords[2]};
		triangleA.Translate(gridCenter * -1);
		triangleB.Translate(gridCenter * -1);
		geometry.Triangles		.push_back(triangleA);
		geometry.Triangles		.push_back(triangleB);
		geometry.Normals		.push_back((triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().f32());
		//geometry.Normals		.push_back((coords[0] - coords[1]).Normalize().Cross((coords[1] - coords[3]).Normalize()).Normalize().f32());
		geometry.TextureCoords	.push_back(triangleATex);
		geometry.TextureCoords	.push_back(triangleBTex);
	}
	return 0;
}

::gpk::error_t			gpk::geometryBuildFigure1	(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3f32 & gridCenter)	{
	(void)radius;
	for(uint32_t z = 0; z < stacks; ++z)
	for(uint32_t x = 0; x < slices; ++x)  {
		::gpk::n2f32									texcoords	[4]			=
			{ {0, 0}
			, {0, 1}
			, {1, 0}
			, {1, 1}
			};
		::gpk::n3f64									coords	[4]				=
			{ {sin(::gpk::math_pi * z		/ stacks) * sin(::gpk::math_2pi * x			/ slices), sin(::gpk::math_pi * z		/ stacks	) * cos(::gpk::math_2pi * x			/ slices), cos(::gpk::math_pi * x		/slices)}
			, {sin(::gpk::math_pi * z		/ stacks) * sin(::gpk::math_2pi * (x + 1)	/ slices), sin(::gpk::math_pi * z		/ stacks	) * cos(::gpk::math_2pi * (x + 1)	/ slices), cos(::gpk::math_pi * (x + 1) /slices)}
			, {sin(::gpk::math_pi * (z + 1)	/ stacks) * sin(::gpk::math_2pi * x			/ slices), sin(::gpk::math_pi * (z + 1)	/ stacks	) * cos(::gpk::math_2pi * x			/ slices), cos(::gpk::math_pi * x		/slices)}
			, {sin(::gpk::math_pi * (z + 1)	/ stacks) * sin(::gpk::math_2pi * (x + 1)	/ slices), sin(::gpk::math_pi * (z + 1)	/ stacks	) * cos(::gpk::math_2pi * (x + 1)	/ slices), cos(::gpk::math_pi * (x + 1)	/slices)}
			};
		::gpk::tri3f32								triangleA				= {coords[0].f32() * radius, coords[1].f32() * radius, coords[2].f32() * radius};
		::gpk::tri3f32								triangleB				= {coords[1].f32() * radius, coords[3].f32() * radius, coords[2].f32() * radius};
		::gpk::tri2f32								triangleATex			= {texcoords[0], texcoords[1], texcoords[2]};
		::gpk::tri2f32								triangleBTex			= {texcoords[1], texcoords[3], texcoords[2]};
		triangleA.Translate(gridCenter * -1);
		triangleB.Translate(gridCenter * -1);
		geometry.Triangles		.push_back(triangleA);
		geometry.Triangles		.push_back(triangleB);
		geometry.Normals		.push_back((triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().f32());
		//geometry.Normals		.push_back((coords[0] - coords[1]).Normalize().Cross((coords[1] - coords[3]).Normalize()).Normalize().f32());
		geometry.TextureCoords	.push_back(triangleATex);
		geometry.TextureCoords	.push_back(triangleBTex);
	}
	return 0;
}

::gpk::error_t			gpk::geometryBuildHalfHelix	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3f32 & gridCenter, const ::gpk::n3f32 & scale)	{
	::gpk::n2f32									texCoordUnits				= {1.0f / slices, 1.0f / stacks};
	for(uint32_t z = 0; z < stacks; ++z)
	for(uint32_t x = 0; x < slices; ++x)  {
		::gpk::n2f32									texcoords	[4]			=
			{ {(x		) * texCoordUnits.x, (z		) * texCoordUnits.y}
			, {(x		) * texCoordUnits.x, (z + 1	) * texCoordUnits.y}
			, {(x + 1	) * texCoordUnits.x, (z		) * texCoordUnits.y}
			, {(x + 1	) * texCoordUnits.x, (z + 1	) * texCoordUnits.y}
			};
		::gpk::n3f64									coords	[4]				=
			{ {sin(::gpk::math_pi * z		/ stacks) * sin(::gpk::math_2pi * x			/ slices), sin(::gpk::math_pi * z		/ stacks	) * cos(::gpk::math_pi * x			/slices), cos(::gpk::math_2pi * x		/ slices)}
			, {sin(::gpk::math_pi * z		/ stacks) * sin(::gpk::math_2pi * (x + 1)	/ slices), sin(::gpk::math_pi * z		/ stacks	) * cos(::gpk::math_pi * (x + 1)	/slices), cos(::gpk::math_2pi * (x + 1)	/ slices)}
			, {sin(::gpk::math_pi * (z + 1)	/ stacks) * sin(::gpk::math_2pi * x			/ slices), sin(::gpk::math_pi * (z + 1)	/ stacks	) * cos(::gpk::math_pi * x			/slices), cos(::gpk::math_2pi * x		/ slices)}
			, {sin(::gpk::math_pi * (z + 1)	/ stacks) * sin(::gpk::math_2pi * (x + 1)	/ slices), sin(::gpk::math_pi * (z + 1)	/ stacks	) * cos(::gpk::math_pi * (x + 1)	/slices), cos(::gpk::math_2pi * (x + 1)	/ slices)}
			};
		::gpk::tri3f32								triangleA				= {coords[0].f32() * radius, coords[1].f32() * radius, coords[2].f32() * radius};
		::gpk::tri3f32								triangleB				= {coords[1].f32() * radius, coords[3].f32() * radius, coords[2].f32() * radius};
		::gpk::tri2f32								triangleATex			= {texcoords[0], texcoords[1], texcoords[2]};
		::gpk::tri2f32								triangleBTex			= {texcoords[1], texcoords[3], texcoords[2]};
		triangleA.Scale(scale);
		triangleB.Scale(scale);
		triangleA.Translate(gridCenter * -1);
		triangleB.Translate(gridCenter * -1);
		geometry.Triangles		.push_back(triangleA);
		geometry.Triangles		.push_back(triangleB);
		geometry.Normals		.push_back((triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().f32());
		geometry.TextureCoords	.push_back(triangleATex);
		geometry.TextureCoords	.push_back(triangleBTex);
	}
	return 0;
}

::gpk::error_t			gpk::geometryBuildHelix		(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3f32 & gridCenter, const ::gpk::n3f32 & scale)	{
	::gpk::n2f32									texCoordUnits				= {1.0f / slices, 1.0f / stacks};
	for(uint32_t z = 0; z < stacks; ++z)
	for(uint32_t x = 0; x < slices; ++x)  {
		::gpk::n2f32									texcoords	[4]			=
			{ {(x		) * texCoordUnits.x, (z		) * texCoordUnits.y}
			, {(x		) * texCoordUnits.x, (z + 1	) * texCoordUnits.y}
			, {(x + 1	) * texCoordUnits.x, (z		) * texCoordUnits.y}
			, {(x + 1	) * texCoordUnits.x, (z + 1	) * texCoordUnits.y}
			};
		::gpk::n3f64									coords	[4]				=
			{ {sin(::gpk::math_pi * z		/ stacks) * sin(::gpk::math_2pi * x			/ slices), sin(::gpk::math_pi * z		/ stacks	) * cos(::gpk::math_pi * x			/slices), cos(::gpk::math_2pi * x		/ slices)}
			, {sin(::gpk::math_pi * z		/ stacks) * sin(::gpk::math_2pi * (x + 1)	/ slices), sin(::gpk::math_pi * z		/ stacks	) * cos(::gpk::math_pi * (x + 1)	/slices), cos(::gpk::math_2pi * (x + 1)	/ slices)}
			, {sin(::gpk::math_pi * (z + 1)	/ stacks) * sin(::gpk::math_2pi * x			/ slices), sin(::gpk::math_pi * (z + 1)	/ stacks	) * cos(::gpk::math_pi * x			/slices), cos(::gpk::math_2pi * x		/ slices)}
			, {sin(::gpk::math_pi * (z + 1)	/ stacks) * sin(::gpk::math_2pi * (x + 1)	/ slices), sin(::gpk::math_pi * (z + 1)	/ stacks	) * cos(::gpk::math_pi * (x + 1)	/slices), cos(::gpk::math_2pi * (x + 1)	/ slices)}
			};
		::gpk::tri3f32								triangleA				= {coords[0].f32() * radius, coords[1].f32() * radius, coords[2].f32() * radius};
		::gpk::tri3f32								triangleB				= {coords[1].f32() * radius, coords[3].f32() * radius, coords[2].f32() * radius};
		::gpk::tri2f32								triangleATex			= {texcoords[0], texcoords[1], texcoords[2]};
		::gpk::tri2f32								triangleBTex			= {texcoords[1], texcoords[3], texcoords[2]};
		::gpk::tri3f32								triangleC				= triangleA;
		::gpk::tri3f32								triangleD				= triangleB;
		triangleA.Translate({0, 0, -radius});
		triangleB.Translate({0, 0, -radius});
		triangleC.Translate({0, 0, -radius});
		triangleD.Translate({0, 0, -radius});
		triangleC.Scale({-1, 1, -1});
		triangleD.Scale({-1, 1, -1});

		triangleA.Scale(scale);
		triangleB.Scale(scale);
		triangleC.Scale(scale);
		triangleD.Scale(scale);

		triangleA.Translate(gridCenter * -1);
		triangleB.Translate(gridCenter * -1);
		triangleC.Translate(gridCenter * -1);
		triangleD.Translate(gridCenter * -1);
		::gpk::n3f32									normal					= (triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().f32();
		geometry.Triangles		.push_back(triangleA);
		geometry.Triangles		.push_back(triangleB);
		geometry.Triangles		.push_back(triangleC);
		geometry.Triangles		.push_back(triangleD);
		geometry.Normals		.push_back(normal);
		normal.Scale({-1, -1, 0});
		geometry.Normals		.push_back(normal);
		geometry.TextureCoords	.push_back(triangleATex);
		geometry.TextureCoords	.push_back(triangleBTex);
		geometry.TextureCoords	.push_back(triangleATex);
		geometry.TextureCoords	.push_back(triangleBTex);
	}
	return 0;
}

//
::gpk::error_t			gpk::geometryBuildSphere	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3f32 & gridCenter)	{
	::gpk::n2f32									texCoordUnits				= {1.0f / slices, 1.0f / stacks};
	for(uint32_t z = 0; z < stacks; ++z)
	for(uint32_t x = 0; x < slices; ++x)  {
		{
			::gpk::n2f32									texcoords	[4]				=
				{ {(z		) * texCoordUnits.y, (x		) * texCoordUnits.x}
				, {(z		) * texCoordUnits.y, (x + 1	) * texCoordUnits.x}
				, {(z + 1	) * texCoordUnits.y, (x		) * texCoordUnits.x}
				, {(z + 1	) * texCoordUnits.y, (x + 1	) * texCoordUnits.x}
				};
			geometry.TextureCoords.push_back(
				{ texcoords[0]
				, texcoords[2]
				, texcoords[1]
				});
			geometry.TextureCoords.push_back(
				{ texcoords[1]
				, texcoords[2]
				, texcoords[3]
				});
		}
		::gpk::n3f64									coords	[4]				=
			{ {sin(::gpk::math_pi * x		/slices) * cos(::gpk::math_2pi * z			/ stacks), cos(::gpk::math_pi * x		/slices), sin(::gpk::math_pi * x		/ slices) * sin(::gpk::math_2pi * z			/ stacks)}
			, {sin(::gpk::math_pi * (x + 1)	/slices) * cos(::gpk::math_2pi * z			/ stacks), cos(::gpk::math_pi * (x + 1)	/slices), sin(::gpk::math_pi * (x + 1)	/ slices) * sin(::gpk::math_2pi * z			/ stacks)}
			, {sin(::gpk::math_pi * x		/slices) * cos(::gpk::math_2pi * (z + 1)	/ stacks), cos(::gpk::math_pi * x		/slices), sin(::gpk::math_pi * x		/ slices) * sin(::gpk::math_2pi * (z + 1)	/ stacks)}
			, {sin(::gpk::math_pi * (x + 1)	/slices) * cos(::gpk::math_2pi * (z + 1)	/ stacks), cos(::gpk::math_pi * (x + 1)	/slices), sin(::gpk::math_pi * (x + 1)	/ slices) * sin(::gpk::math_2pi * (z + 1)	/ stacks)}
			};
		{
			::gpk::tri3f32								triangleA			= {coords[0].f32() * radius, coords[2].f32() * radius, coords[1].f32() * radius};
			::gpk::tri3f32								triangleB			= {coords[1].f32() * radius, coords[2].f32() * radius, coords[3].f32() * radius};
			triangleA.Translate(gridCenter * -1);
			triangleB.Translate(gridCenter * -1);
			geometry.Triangles	.push_back(triangleA);
			geometry.Triangles	.push_back(triangleB);
		}
		{
			::gpk::n3f32									normal				= coords[0].f32();
			normal												+= coords[1].f32();
			normal												+= coords[2].f32();
			normal												+= coords[3].f32();
			normal												/= 4;
			normal.Normalize();
			geometry.Normals.push_back(normal);
		}
	}
	return 0;
}

//
::gpk::error_t			gpk::geometryBuildCylinder	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radiusYMin, float radiusYMax, const ::gpk::n3f32 & gridCenter, const ::gpk::n3f32 & scale)	{
	::gpk::n2f32									texCoordUnits				= {1.0f / slices, 1.0f / stacks};
	const double											radiusUnit					= 1.0 / stacks;
	for(uint32_t y = 0; y < stacks; ++y) {
		double													radius						= ::gpk::interpolate_linear(radiusYMin, radiusYMax, radiusUnit * y);
		double													radiusNext					= ::gpk::interpolate_linear(radiusYMin, radiusYMax, radiusUnit * (y + 1));
		for(uint32_t z = 0; z < slices; ++z) {
			{
				::gpk::n2f32									texcoords	[4]				=
					{ {(z		) * texCoordUnits.x, (y		) * texCoordUnits.y}
					, {(z + 1	) * texCoordUnits.x, (y		) * texCoordUnits.y}
					, {(z		) * texCoordUnits.x, (y	+ 1	) * texCoordUnits.y}
					, {(z + 1	) * texCoordUnits.x, (y + 1	) * texCoordUnits.y}
					};
				geometry.TextureCoords.push_back(
					{ texcoords[0]
					, texcoords[2]
					, texcoords[1]
					});
				geometry.TextureCoords.push_back(
					{ texcoords[1]
					, texcoords[2]
					, texcoords[3]
					});
			}
			::gpk::n3f64									coords	[4]				=
				{ {1 * radius, (double)(y		)}
				, {1 * radius, (double)(y		)}
				, {1 * radiusNext, (double)(y + 1	)}
				, {1 * radiusNext, (double)(y + 1	)}
				};
			coords[0].RotateY(::gpk::math_2pi / slices * (z + 0));
			coords[1].RotateY(::gpk::math_2pi / slices * (z + 1));
			coords[2].RotateY(::gpk::math_2pi / slices * (z + 0));
			coords[3].RotateY(::gpk::math_2pi / slices * (z + 1));

			{
				::gpk::tri3f32								triangleA			= {coords[0].f32(), coords[2].f32(), coords[1].f32()};
				::gpk::tri3f32								triangleB			= {coords[1].f32(), coords[2].f32(), coords[3].f32()};
				triangleA.Scale(scale);
				triangleB.Scale(scale);
				triangleA.Translate(gridCenter * -1);
				triangleB.Translate(gridCenter * -1);

				geometry.Triangles	.push_back(triangleA);
				geometry.Triangles	.push_back(triangleB);
				geometry.Normals	.push_back((triangleB.A - triangleB.B).Normalize().Cross((triangleA.A - triangleA.B).Normalize()).Normalize().f32());
			}
		}
	}
	return 0;
}


::gpk::error_t			gpk::geometryBuildTender	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3f32 & gridCenter, const ::gpk::n3f32 & scale)	{
	::gpk::n2f32									texCoordUnits				= {1.0f / slices, 1.0f / stacks};
	for(uint32_t z = 0; z < stacks; ++z)
	for(uint32_t x = 0; x < slices; ++x)  {
		{
			::gpk::n2f32									texcoords	[4]				=
				{ {(z		) * texCoordUnits.y, (x		) * texCoordUnits.x}
				, {(z		) * texCoordUnits.y, (x + 1	) * texCoordUnits.x}
				, {(z + 1	) * texCoordUnits.y, (x		) * texCoordUnits.x}
				, {(z + 1	) * texCoordUnits.y, (x + 1	) * texCoordUnits.x}
				};
			geometry.TextureCoords.push_back(
				{ texcoords[0]
				, texcoords[2]
				, texcoords[1]
				});
			geometry.TextureCoords.push_back(
				{ texcoords[1]
				, texcoords[2]
				, texcoords[3]
				});
		}
		//const ::gpk::SSinCos									pairSinCos				= ::gpk::getSinCos(::gpk::math_2pi * z / slices);
		//const double												px						= x * pairSinCos.Cos - z * pairSinCos.Sin;
		//z														= (x * pairSinCos.Sin + z * pairSinCos.Cos);
		//x														= px;
		(void)radius;
		::gpk::n3f64									coords	[4]				=
			{ {1 * radius, (double)(z		)}
			, {1 * radius, (double)(z		)}
			, {1 * radius, (double)(z + 1	)}
			, {1 * radius, (double)(z + 1	)}
			};
		coords[0].RotateY(::gpk::math_2pi / slices * (x + 0));
		coords[1].RotateY(::gpk::math_2pi / slices * (x + 1));
		coords[2].RotateY(::gpk::math_2pi / slices * (x + 0));
		coords[3].RotateY(::gpk::math_2pi / slices * (x + 1));
		coords[0].Normalize();
		coords[1].Normalize();
		coords[2].Normalize();
		coords[3].Normalize();

		{
			::gpk::tri3f32								triangleA			= {coords[0].f32(), coords[2].f32(), coords[1].f32()};
			::gpk::tri3f32								triangleB			= {coords[1].f32(), coords[2].f32(), coords[3].f32()};
			triangleA.Scale(scale);
			triangleB.Scale(scale);
			triangleA.Translate(gridCenter * -1);
			triangleB.Translate(gridCenter * -1);
			geometry.Triangles	.push_back(triangleA);
			geometry.Triangles	.push_back(triangleB);
			geometry.Normals	.push_back((triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().f32());
		}
	}
	return 0;
}

//
//
//::gpk::error_t			gpk::geometryBuildSphere	(SGeometryTriangles & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3f32 & gridCenter)	{
//	::gpk::n2f32									texCoordUnits				= {1.0f / slices, 1.0f / stacks};
//	for(uint32_t z = 0; z < stacks; ++z)
//	for(uint32_t x = 0; x < slices; ++x)  {
//		{
//			::gpk::n2f32									texcoords	[4]				=
//				{ {(z		) * texCoordUnits.y, (x		) * texCoordUnits.x}
//				, {(z		) * texCoordUnits.y, (x + 1	) * texCoordUnits.x}
//				, {(z + 1	) * texCoordUnits.y, (x		) * texCoordUnits.x}
//				, {(z + 1	) * texCoordUnits.y, (x + 1	) * texCoordUnits.x}
//				};
//			geometry.TextureCoords.push_back(
//				{ texcoords[0]
//				, texcoords[2]
//				, texcoords[1]
//				});
//			geometry.TextureCoords.push_back(
//				{ texcoords[1]
//				, texcoords[2]
//				, texcoords[3]
//				});
//		}
//		::gpk::n3f64									coords		[4]				=
//			{ {sin(::gpk::math_pi * x		/slices) * cos(::gpk::math_2pi * z			/ stacks), cos(::gpk::math_pi * x		/slices), sin(::gpk::math_pi * x		/ slices) * sin(::gpk::math_2pi * z			/ stacks)}
//			, {sin(::gpk::math_pi * (x + 1)	/slices) * cos(::gpk::math_2pi * z			/ stacks), cos(::gpk::math_pi * (x + 1) /slices), sin(::gpk::math_pi * (x + 1)	/ slices) * sin(::gpk::math_2pi * z			/ stacks)}
//			, {sin(::gpk::math_pi * x		/slices) * cos(::gpk::math_2pi * (z + 1)	/ stacks), cos(::gpk::math_pi * x		/slices), sin(::gpk::math_pi * x		/ slices) * sin(::gpk::math_2pi * (z + 1)	/ stacks)}
//			, {sin(::gpk::math_pi * (x + 1)	/slices) * cos(::gpk::math_2pi * (z + 1)	/ stacks), cos(::gpk::math_pi * (x + 1)	/slices), sin(::gpk::math_pi * (x + 1)	/ slices) * sin(::gpk::math_2pi * (z + 1)	/ stacks)}
//			};
//		{
//			geometry.Normals.push_back(
//				{ coords[0].Normalize().f32()
//				, coords[2].Normalize().f32()
//				, coords[1].Normalize().f32()
//				});
//			geometry.Normals.push_back(
//				{ coords[1].Normalize().f32()
//				, coords[2].Normalize().f32()
//				, coords[3].Normalize().f32()
//				});
//		}
//		{
//			::gpk::tri3f32								triangleA			= {(coords[0] * radius).f32(), (coords[2] * radius).f32(), (coords[1] * radius).f32()};
//			::gpk::tri3f32								triangleB			= {(coords[1] * radius).f32(), (coords[2] * radius).f32(), (coords[3] * radius).f32()};
//			triangleA.Translate(gridCenter * -1);
//			triangleB.Translate(gridCenter * -1);
//			geometry.Triangles	.push_back(triangleA);
//			geometry.Triangles	.push_back(triangleB);
//		}
//	}
//	return 0;
//}