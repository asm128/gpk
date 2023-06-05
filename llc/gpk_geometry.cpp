#include "gpk_geometry.h"

// Vertex coordinates for cube faces
stacxpr	const ::gpk::tri3<float>					geometryCubePositions			[12]						=
	{ {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}	// Right	- first
	, {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}	// Right	- second
	, {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}	// Back		- first
	, {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}	// Back		- second
	, {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}	// Bottom	- first
	, {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}	// Bottom	- second
	, {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}	// Left		- first
	, {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}}	// Left		- second
	, {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}	// Front	- first
	, {{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}	// Front	- second
	, {{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}	// Top		- first
	, {{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}}	// Top		- second
	};

stacxpr	const ::gpk::n3f32						geometryCubeNormalsTriangle		[12]						=
	{ {0.0f, 0.0f, -1.0f}	// Right	- first
	, {0.0f, 0.0f, -1.0f}	// Right	- second

	, {-1.0f, 0.0f, 0.0f}	// Back		- first
	, {-1.0f, 0.0f, 0.0f}	// Back		- second

	, {0.0f, -1.0f, 0.0f}	// Bottom	- first
	, {0.0f, -1.0f, 0.0f}	// Bottom	- second

	, {0.0f, 0.0f, 1.0f}	// Left		- first
	, {0.0f, 0.0f, 1.0f}	// Left		- second

	, {1.0f, 0.0f, 0.0f}	// Front	- first
	, {1.0f, 0.0f, 0.0f}	// Front	- second

	, {0.0f, 1.0f, 0.0f}	// Top		- first
	, {0.0f, 1.0f, 0.0f}	// Top		- second
	};

// -----------------------------------------------------
stacxpr	const ::gpk::tri3<float>					geometryCubeNormalsVertex		[12]						=
	{ { {0.0f, 0.0f, -1.0f}	, {0.0f, 0.0f, -1.0f}	, {0.0f, 0.0f, -1.0f}	} // Right	- first
	, { {0.0f, 0.0f, -1.0f}	, {0.0f, 0.0f, -1.0f}	, {0.0f, 0.0f, -1.0f}	} // Right	- second

	, { {-1.0f, 0.0f, 0.0f}	, {-1.0f, 0.0f, 0.0f}	, {-1.0f, 0.0f, 0.0f}	} // Back	- first
	, { {-1.0f, 0.0f, 0.0f}	, {-1.0f, 0.0f, 0.0f}	, {-1.0f, 0.0f, 0.0f}	} // Back	- second

	, { {0.0f, -1.0f, 0.0f}	, {0.0f, -1.0f, 0.0f}	, {0.0f, -1.0f, 0.0f}	} // Bottom	- first
	, { {0.0f, -1.0f, 0.0f}	, {0.0f, -1.0f, 0.0f}	, {0.0f, -1.0f, 0.0f}	} // Bottom	- second

	, { {0.0f, 0.0f, 1.0f}	, {0.0f, 0.0f, 1.0f}	, {0.0f, 0.0f, 1.0f}	} // Left	- first
	, { {0.0f, 0.0f, 1.0f}	, {0.0f, 0.0f, 1.0f}	, {0.0f, 0.0f, 1.0f}	} // Left	- second

	, { {1.0f, 0.0f, 0.0f}	, {1.0f, 0.0f, 0.0f}	, {1.0f, 0.0f, 0.0f}	} // Front	- first
	, { {1.0f, 0.0f, 0.0f}	, {1.0f, 0.0f, 0.0f}	, {1.0f, 0.0f, 0.0f}	} // Front	- second

	, { {0.0f, 1.0f, 0.0f}	, {0.0f, 1.0f, 0.0f}	, {0.0f, 1.0f, 0.0f}	} // Top	- first
	, { {0.0f, 1.0f, 0.0f}	, {0.0f, 1.0f, 0.0f}	, {0.0f, 1.0f, 0.0f}	} // Top	- second
	};

// Vertex coordinates for cube faces
stacxpr	const ::gpk::tri2<float>					geometryCubeUV					[12]						=
	{ {{1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f}}	// Right	- first			?? I have no idea if this is correct lol
	, {{1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}}	// Right	- second

	, {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}}	// Back		- first
	, {{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}}	// Back		- second

	, {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}}	// Bottom	- first
	, {{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}}	// Bottom	- second

	, {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}}	// Left		- first
	, {{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}}	// Left		- second

	, {{1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}}	// Front	- first
	, {{1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f}}	// Front	- second

	, {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}}	// Top		- first
	, {{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}}	// Top		- second
	};

::gpk::error_t				gpk::generateCubePositions							(::gpk::apod<::gpk::tri3f32>& out_Positions	) { out_Positions	= geometryCubePositions			; return 0; }
::gpk::error_t				gpk::generateCubeNormalsTriangle					(::gpk::apod<::gpk::n3f32  >& out_Normals	) { out_Normals		= geometryCubeNormalsTriangle	; return 0; }
::gpk::error_t				gpk::generateCubeNormalsVertex						(::gpk::apod<::gpk::tri3f32>& out_Normals	) { out_Normals		= geometryCubeNormalsVertex		; return 0; }
::gpk::error_t				gpk::generateCubeUV									(::gpk::apod<::gpk::tri2f32>& out_UV		) { out_UV			= geometryCubeUV				; return 0; }
::gpk::error_t				gpk::generateCubeGeometry
	( ::gpk::apod<::gpk::tri3f32> & out_Positions
	, ::gpk::apod<::gpk::n3f32  > & out_Normals
	, ::gpk::apod<::gpk::tri3f32> & out_NormalsVertex
	, ::gpk::apod<::gpk::tri2f32> & out_UV
	)
{
	gpk_necall(::gpk::generateCubePositions			(out_Positions		), "%s", "Some nasty problem caused this to fail.");
	gpk_necall(::gpk::generateCubeNormalsTriangle	(out_Normals		), "%s", "Some nasty problem caused this to fail.");
	gpk_necall(::gpk::generateCubeNormalsVertex		(out_NormalsVertex	), "%s", "Some nasty problem caused this to fail.");
	gpk_necall(::gpk::generateCubeUV				(out_UV				), "%s", "Some nasty problem caused this to fail.");
	return 0;
}


::gpk::error_t									gpk::generateGridPositions						(const ::gpk::n2u16 & gridMetrics, ::gpk::apod<::gpk::tri3	<float>>& out_Positions	) {
	for(uint32_t z = 0; z < gridMetrics.y; ++z)
		for(uint32_t x = 0; x < gridMetrics.x; ++x) {
			out_Positions.push_back({{(float)x, 0, (float)z}, {(float)x		, 0, (float)z + 1}, {(float)x + 1, 0, (float)z + 1}});
			out_Positions.push_back({{(float)x, 0, (float)z}, {(float)x + 1	, 0, (float)z + 1}, {(float)x + 1, 0, (float)z}});
		}
	return 0;
}
::gpk::error_t									gpk::generateGridNormalsTriangle				(const ::gpk::n2u16 & gridMetrics, ::gpk::apod<::gpk::n3		<float>>& out_Normals	) {
	for(uint32_t z = 0; z < gridMetrics.y; ++z)
		for(uint32_t x = 0; x < gridMetrics.x; ++x) {
			out_Normals.push_back({0, 1, 0});
			out_Normals.push_back({0, 1, 0});
		}
	return 0;
}
::gpk::error_t									gpk::generateGridNormalsVertex					(const ::gpk::n2u16 & gridMetrics, ::gpk::apod<::gpk::tri3	<float>>& out_Normals	) {
	for(uint32_t z = 0; z < gridMetrics.y; ++z)
		for(uint32_t x = 0; x < gridMetrics.x; ++x) {
			out_Normals.push_back({{0, 1, 0}, {0, 1, 0}, {0, 1, 0}});
			out_Normals.push_back({{0, 1, 0}, {0, 1, 0}, {0, 1, 0}});
		}
	return 0;
}
::gpk::error_t				gpk::generateGridUV					(const ::gpk::n2u16 & gridMetrics, ::gpk::apod<::gpk::tri2	<float>>& out_UV		) {
	const ::gpk::n2f64				gridUnit							= {1.0 / gridMetrics.x, 1.0 / gridMetrics.y};
	const ::gpk::n2f64				gridMetricsF						= gridMetrics.f64();
	for(uint32_t z = 0; z < gridMetrics.y; ++z)
	for(uint32_t x = 0; x < gridMetrics.x; ++x) {
		const ::gpk::n2f64				gridCell							= {x / gridMetricsF.x, z / gridMetricsF.y};
		const ::gpk::n2f64				gridCellFar							= gridCell + gridUnit;
		out_UV.push_back({{(float)gridCell.x, (float)gridCell.y}, {(float)gridCell.x, (float)gridCellFar.y}, gridCellFar.f32()});
		out_UV.push_back({{(float)gridCell.x, (float)gridCell.y}, gridCellFar.f32()	, {(float)gridCellFar.x, (float)gridCell.y}});
	}
	return 0;
}
::gpk::error_t				gpk::generateGridGeometry
	( const ::gpk::n2u16			& gridMetrics
	, ::gpk::apod<::gpk::tri3f32>	& out_Positions
	, ::gpk::apod<::gpk::n3f32  >	& out_Normals
	, ::gpk::apod<::gpk::tri3f32>	& out_NormalsVertex
	, ::gpk::apod<::gpk::tri2f32>	& out_UV
	)
{
	gpk_necall(::gpk::generateGridPositions			(gridMetrics, out_Positions		), "%s", "Some nasty problem caused this to fail.");
	gpk_necall(::gpk::generateGridNormalsTriangle	(gridMetrics, out_Normals		), "%s", "Some nasty problem caused this to fail.");
	gpk_necall(::gpk::generateGridNormalsVertex		(gridMetrics, out_NormalsVertex	), "%s", "Some nasty problem caused this to fail.");
	gpk_necall(::gpk::generateGridUV				(gridMetrics, out_UV			), "%s", "Some nasty problem caused this to fail.");
	return 0;
}
