#include "gpk_scene.h"

GPK_USING_TYPEINT();
GPK_USING_VIEW();
GPK_USING_APOD();

// Vertex coordinates for cube faces
stxp	const ::gpk::tri3<f2_t>					geometryCubePositions			[12]						=
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

stxp	const ::gpk::n3f2_t						geometryCubeNormalsTriangle		[12]						=
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
stxp	const ::gpk::tri3<f2_t>					geometryCubeNormalsVertex		[12]						=
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
stxp	const ::gpk::tri2<f2_t>					geometryCubeUV					[12]						=
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

gpk::err_t 		gpk::generateCubePositions							(::gpk::apod<::gpk::tri3f2_t>& out_Positions	) { out_Positions	= geometryCubePositions			; return 0; }
gpk::err_t 		gpk::generateCubeNormalsTriangle					(::gpk::apod<::gpk::n3f2_t  >& out_Normals	) { out_Normals		= geometryCubeNormalsTriangle	; return 0; }
gpk::err_t 		gpk::generateCubeNormalsVertex						(::gpk::apod<::gpk::tri3f2_t>& out_Normals	) { out_Normals		= geometryCubeNormalsVertex		; return 0; }
gpk::err_t 		gpk::generateCubeUV									(::gpk::apod<::gpk::tri2f2_t>& out_UV		) { out_UV			= geometryCubeUV				; return 0; }
gpk::err_t 		gpk::generateCubeGeometry
	( ::gpk::apod<::gpk::tri3f2_t> & out_Positions
	, ::gpk::apod<::gpk::n3f2_t  > & out_Normals
	, ::gpk::apod<::gpk::tri3f2_t> & out_NormalsVertex
	, ::gpk::apod<::gpk::tri2f2_t> & out_UV
	)
{
	if_fail_fef(gpk::generateCubePositions			(out_Positions		), "%s", "Some nasty problem caused this to fail.");
	if_fail_fef(gpk::generateCubeNormalsTriangle	(out_Normals		), "%s", "Some nasty problem caused this to fail.");
	if_fail_fef(gpk::generateCubeNormalsVertex		(out_NormalsVertex	), "%s", "Some nasty problem caused this to fail.");
	if_fail_fef(gpk::generateCubeUV				(out_UV				), "%s", "Some nasty problem caused this to fail.");
	return 0;
}


gpk::err_t 		gpk::generateGridPositions		(const ::gpk::n2u1_t & gridMetrics, ::gpk::apod<::gpk::tri3	<f2_t>>& out_Positions	) {
	for(u2_t z = 0; z < gridMetrics.y; ++z)
		for(u2_t x = 0; x < gridMetrics.x; ++x) {
			out_Positions.push_back({{(f2_t)x, 0, (f2_t)z}, {(f2_t)x		, 0, (f2_t)z + 1}, {(f2_t)x + 1, 0, (f2_t)z + 1}});
			out_Positions.push_back({{(f2_t)x, 0, (f2_t)z}, {(f2_t)x + 1	, 0, (f2_t)z + 1}, {(f2_t)x + 1, 0, (f2_t)z}});
		}
	return 0;
}
gpk::err_t 		gpk::generateGridNormalsTriangle				(const ::gpk::n2u1_t & gridMetrics, ::gpk::apod<::gpk::n3		<f2_t>>& out_Normals	) {
	for(u2_t z = 0; z < gridMetrics.y; ++z)
		for(u2_t x = 0; x < gridMetrics.x; ++x) {
			out_Normals.push_back({0, 1, 0});
			out_Normals.push_back({0, 1, 0});
		}
	return 0;
}
gpk::err_t 		gpk::generateGridNormalsVertex					(const ::gpk::n2u1_t & gridMetrics, ::gpk::apod<::gpk::tri3	<f2_t>>& out_Normals	) {
	for(u2_t z = 0; z < gridMetrics.y; ++z)
		for(u2_t x = 0; x < gridMetrics.x; ++x) {
			out_Normals.push_back({{0, 1, 0}, {0, 1, 0}, {0, 1, 0}});
			out_Normals.push_back({{0, 1, 0}, {0, 1, 0}, {0, 1, 0}});
		}
	return 0;
}
gpk::err_t 		gpk::generateGridUV					(const ::gpk::n2u1_t & gridMetrics, ::gpk::apod<::gpk::tri2	<f2_t>>& out_UV		) {
	const ::gpk::n2f3_t				gridUnit							= {1.0 / gridMetrics.x, 1.0 / gridMetrics.y};
	const ::gpk::n2f3_t				gridMetricsF						= gridMetrics.f3();
	for(u2_t z = 0; z < gridMetrics.y; ++z)
	for(u2_t x = 0; x < gridMetrics.x; ++x) {
		const ::gpk::n2f3_t				gridCell							= {x / gridMetricsF.x, z / gridMetricsF.y};
		const ::gpk::n2f3_t				gridCellFar							= gridCell + gridUnit;
		out_UV.push_back({{(f2_t)gridCell.x, (f2_t)gridCell.y}, {(f2_t)gridCell.x, (f2_t)gridCellFar.y}, gridCellFar.f2()});
		out_UV.push_back({{(f2_t)gridCell.x, (f2_t)gridCell.y}, gridCellFar.f2()	, {(f2_t)gridCellFar.x, (f2_t)gridCell.y}});
	}
	return 0;
}
gpk::err_t 		gpk::generateGridGeometry
	( const ::gpk::n2u1_t			& gridMetrics
	, ::gpk::apod<::gpk::tri3f2_t>	& out_Positions
	, ::gpk::apod<::gpk::n3f2_t  >	& out_Normals
	, ::gpk::apod<::gpk::tri3f2_t>	& out_NormalsVertex
	, ::gpk::apod<::gpk::tri2f2_t>	& out_UV
	)
{
	if_fail_fef(gpk::generateGridPositions			(gridMetrics, out_Positions		), "%s", "Some nasty problem caused this to fail.");
	if_fail_fef(gpk::generateGridNormalsTriangle	(gridMetrics, out_Normals		), "%s", "Some nasty problem caused this to fail.");
	if_fail_fef(gpk::generateGridNormalsVertex		(gridMetrics, out_NormalsVertex	), "%s", "Some nasty problem caused this to fail.");
	if_fail_fef(gpk::generateGridUV				(gridMetrics, out_UV			), "%s", "Some nasty problem caused this to fail.");
	return 0;
}
