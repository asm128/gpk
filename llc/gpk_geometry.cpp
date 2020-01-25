#include "gpk_geometry.h"

// Vertex coordinates for cube faces
static constexpr const ::gpk::STriangle3<float>					geometryCubePositions			[12]						=
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

static constexpr	const ::gpk::SCoord3<float>						geometryCubeNormalsTriangle		[12]						=
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
static constexpr	const ::gpk::STriangle3<float>					geometryCubeNormalsVertex		[12]						=
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
static constexpr	const ::gpk::STriangle2<float>					geometryCubeUV					[12]						=
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

					::gpk::error_t									gpk::generateCubePositions							(::gpk::array_pod<::gpk::STriangle3<float>>& out_Positions	) { out_Positions	= geometryCubePositions			; return 0; }
					::gpk::error_t									gpk::generateCubeNormalsTriangle					(::gpk::array_pod<::gpk::SCoord3	<float>>& out_Normals	) { out_Normals		= geometryCubeNormalsTriangle	; return 0; }
					::gpk::error_t									gpk::generateCubeNormalsVertex						(::gpk::array_pod<::gpk::STriangle3<float>>& out_Normals	) { out_Normals		= geometryCubeNormalsVertex		; return 0; }
					::gpk::error_t									gpk::generateCubeUV									(::gpk::array_pod<::gpk::STriangle2<float>>& out_UV		) { out_UV			= geometryCubeUV				; return 0; }
					::gpk::error_t									gpk::generateCubeGeometry
	( ::gpk::array_pod<::gpk::STriangle3	<float>> & out_Positions
	, ::gpk::array_pod<::gpk::SCoord3		<float>> & out_Normals
	, ::gpk::array_pod<::gpk::STriangle3	<float>> & out_NormalsVertex
	, ::gpk::array_pod<::gpk::STriangle2	<float>> & out_UV
	)
{
	gpk_necall(::gpk::generateCubePositions			(out_Positions		), "%s", "Some nasty problem caused this to fail.");
	gpk_necall(::gpk::generateCubeNormalsTriangle	(out_Normals		), "%s", "Some nasty problem caused this to fail.");
	gpk_necall(::gpk::generateCubeNormalsVertex		(out_NormalsVertex	), "%s", "Some nasty problem caused this to fail.");
	gpk_necall(::gpk::generateCubeUV				(out_UV				), "%s", "Some nasty problem caused this to fail.");
	return 0;
}


					::gpk::error_t									gpk::generateGridPositions						(const ::gpk::SCoord2<uint32_t> & gridMetrics, ::gpk::array_pod<::gpk::STriangle3	<float>>& out_Positions	) {
	for(uint32_t z = 0; z < gridMetrics.y; ++z)
		for(uint32_t x = 0; x < gridMetrics.x; ++x) {
			out_Positions.push_back({{(float)x, 0, (float)z}, {(float)x		, 0, (float)z + 1}, {(float)x + 1, 0, (float)z + 1}});
			out_Positions.push_back({{(float)x, 0, (float)z}, {(float)x + 1	, 0, (float)z + 1}, {(float)x + 1, 0, (float)z}});
		}
	return 0;
}
					::gpk::error_t									gpk::generateGridNormalsTriangle				(const ::gpk::SCoord2<uint32_t> & gridMetrics, ::gpk::array_pod<::gpk::SCoord3		<float>>& out_Normals	) {
	for(uint32_t z = 0; z < gridMetrics.y; ++z)
		for(uint32_t x = 0; x < gridMetrics.x; ++x) {
			out_Normals.push_back({0, 1, 0});
			out_Normals.push_back({0, 1, 0});
		}
	return 0;
}
					::gpk::error_t									gpk::generateGridNormalsVertex					(const ::gpk::SCoord2<uint32_t> & gridMetrics, ::gpk::array_pod<::gpk::STriangle3	<float>>& out_Normals	) {
	for(uint32_t z = 0; z < gridMetrics.y; ++z)
		for(uint32_t x = 0; x < gridMetrics.x; ++x) {
			out_Normals.push_back({{0, 1, 0}, {0, 1, 0}, {0, 1, 0}});
			out_Normals.push_back({{0, 1, 0}, {0, 1, 0}, {0, 1, 0}});
		}
	return 0;
}
					::gpk::error_t									gpk::generateGridUV								(const ::gpk::SCoord2<uint32_t> & gridMetrics, ::gpk::array_pod<::gpk::STriangle2	<float>>& out_UV		) {
	const ::gpk::SCoord2<double>											gridUnit										= {1.0 / gridMetrics.x, 1.0 / gridMetrics.y};
	const ::gpk::SCoord2<double>											gridMetricsF									= gridMetrics.Cast<double>();
	for(uint32_t z = 0; z < gridMetrics.y; ++z)
		for(uint32_t x = 0; x < gridMetrics.x; ++x) {
			const ::gpk::SCoord2<double>											gridCell										= {x / gridMetricsF.x, z / gridMetricsF.y};
			const ::gpk::SCoord2<double>											gridCellFar										= gridCell + gridUnit;
			out_UV.push_back({{(float)gridCell.x, (float)gridCell.y}, {(float)gridCell.x		, (float)gridCellFar.y}	, gridCellFar.Cast<float>()					});
			out_UV.push_back({{(float)gridCell.x, (float)gridCell.y}, gridCellFar.Cast<float>()							, {(float)gridCellFar.x, (float)gridCell.y}	});
		}
	return 0;
}
					::gpk::error_t									gpk::generateGridGeometry
	( const ::gpk::SCoord2<uint32_t>					& gridMetrics
	, ::gpk::array_pod<::gpk::STriangle3	<float>>	& out_Positions
	, ::gpk::array_pod<::gpk::SCoord3		<float>>	& out_Normals
	, ::gpk::array_pod<::gpk::STriangle3	<float>>	& out_NormalsVertex
	, ::gpk::array_pod<::gpk::STriangle2	<float>>	& out_UV
	)
{
	gpk_necall(::gpk::generateGridPositions			(gridMetrics, out_Positions		), "%s", "Some nasty problem caused this to fail.");
	gpk_necall(::gpk::generateGridNormalsTriangle	(gridMetrics, out_Normals		), "%s", "Some nasty problem caused this to fail.");
	gpk_necall(::gpk::generateGridNormalsVertex		(gridMetrics, out_NormalsVertex	), "%s", "Some nasty problem caused this to fail.");
	gpk_necall(::gpk::generateGridUV				(gridMetrics, out_UV			), "%s", "Some nasty problem caused this to fail.");
	return 0;
}
