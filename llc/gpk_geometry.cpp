#include "gpk_geometry.h"

::gpk::error_t			gpk::geometryBuildSphere	(STrianglesIndexed & geometry, const ::gpk::SParamsSphere & params) {// uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3f32 & gridCenter)	{
	const ::gpk::n2f32			texCoordUnits				= {1.0f / params.Slices, 1.0f / params.Stacks};
	uint32_t					vertexOffset				= geometry.Positions.size();
	const float					scale						= params.Reverse ? -1.0f : 1.0f;
	for(uint32_t y = 0; y < (uint32_t)params.Stacks + 1; ++y) {
		const double				currentY					= ::gpk::math_pi * y / params.Slices;
		const double				currentRadius				= sin(currentY);
		for(uint32_t x = 0; x < (uint32_t)params.Slices + 1; ++x)  {
			::gpk::n2f32				texcoord				= {x * texCoordUnits.x, y * texCoordUnits.y};
			geometry.TextureCoords.push_back(texcoord);
			const double				currentX				= ::gpk::math_2pi * x / params.Stacks;
			::gpk::n3f64				coord 					= {currentRadius * cos(currentX), cos(currentY), currentRadius * sin(currentX) * scale};
			geometry.Positions	.push_back((coord * params.Radius).f32() - params.Center);
			geometry.Normals	.push_back(coord.Normalize().f32() * scale);
		}
	}
	for(uint32_t y = 0; y < params.Stacks; ++y) {
		for(uint32_t z = 0; z < params.Slices; ++z) {
			uint32_t					indices		[4]			= {vertexOffset + 0, vertexOffset + 1, (vertexOffset + params.Slices + 1), (vertexOffset + params.Slices + 2)};
			geometry.PositionIndices.push_back(indices[0]);
			geometry.PositionIndices.push_back(indices[1]);
			geometry.PositionIndices.push_back(indices[2]);
			geometry.PositionIndices.push_back(indices[1]);
			geometry.PositionIndices.push_back(indices[3]);
			geometry.PositionIndices.push_back(indices[2]);
			++vertexOffset;
		}
		++vertexOffset;
	}

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
			::gpk::n2f32				texcoord					= {z * texCoordUnits.x, y * texCoordUnits.y};
			geometry.TextureCoords.push_back(texcoord);

			::gpk::n3f64				coord						= {radius, (double)y};
			coord.RotateY(::gpk::math_2pi * texCoordUnits.x * z * params.DiameterRatio); 
			coord.z					*= scale;
			geometry.Normals  .push_back(coord.f32().Normalize() * scale);
			geometry.Positions.push_back(coord.f32() - params.Center);
		}
	}

	for(uint32_t y = 0; y < params.Stacks; ++y) {
		for(uint32_t z = 0; z < params.Slices; ++z) {
			uint32_t					indices		[4]				= {vertexOffset + 0, vertexOffset + 1, (vertexOffset + params.Slices + 1), (vertexOffset + params.Slices + 2)};
			geometry.PositionIndices.push_back(indices[0]);
			geometry.PositionIndices.push_back(indices[2]);
			geometry.PositionIndices.push_back(indices[1]);
			geometry.PositionIndices.push_back(indices[1]);
			geometry.PositionIndices.push_back(indices[2]);
			geometry.PositionIndices.push_back(indices[3]);
			++vertexOffset;
		}
		++vertexOffset;
	}

	return 0;
}

enum SQUARE_MODE 
	{ SQUARE_MODE_TOP_LEFT
	, SQUARE_MODE_TOP_RIGHT
	};

::gpk::error_t			gpk::geometryBuildGrid	(::gpk::STrianglesIndexed & geometry, const ::gpk::SParamsGrid & params)	{
	const ::gpk::n2f32			texCoordUnits			= {1.0f / params.CellCount.x, 1.0f / params.CellCount.y};
	const ::gpk::n3f32			scale					= {1.0f / params.CellCount.x, 1, 1.0f / params.CellCount.y};
	uint32_t					vertexOffset			= geometry.Positions.size();

	for(uint32_t z = 0, zStop = params.CellCount.y + 1; z < zStop; ++z)
	for(uint32_t x = 0, xStop = params.CellCount.x + 1; x < xStop; ++x) {
		::gpk::n3f32				position				= {(float)x, 0, (float)z};
		position.Scale(scale);
		position				-= {.5f, 0, .5f};

		geometry.Positions		.push_back(position);
		geometry.TextureCoords	.push_back({x * texCoordUnits.x, z * texCoordUnits.y});
		geometry.Normals		.push_back({0, 1, 0});
	}

	const uint16_t				pitch					= params.CellCount.x;
	const uint32_t				indices[4]				= {0U, 1U, pitch, uint32_t(pitch + 1U)};
	const uint32_t				indices_modes[2][6]		=
		{ {indices[0], indices[1], indices[3], indices[0], indices[3], indices[2]}
		, {indices[0], indices[1], indices[2], indices[1], indices[3], indices[0]}
		};

	for(uint32_t z = 0, zStop = params.CellCount.y; z < zStop; ++z) {
		for(uint32_t x = 0, xStop = params.CellCount.x; x < xStop; ++x) {
			const bool					reverseQuad 
				= ( (x >= uint32_t(params.CellCount.x >> 1U) && z <  uint32_t(params.CellCount.y >> 1U))
				 || (x <  uint32_t(params.CellCount.x >> 1U) && z >= uint32_t(params.CellCount.y >> 1U))
				);

			const ::gpk::vcu32			relative_indices		= indices_modes[((reverseQuad ? SQUARE_MODE_TOP_RIGHT : SQUARE_MODE_TOP_LEFT) + one_if(params.ReverseTriangles)) % 2];
			for(uint32_t i = 0; i < 6; ++i) {
				geometry.PositionIndices.push_back(vertexOffset + relative_indices[i]);
			}
			vertexOffset += 4;
		}
		++vertexOffset;
	}
	return 0;
}
