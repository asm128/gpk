#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_dialog.h"
#include "gpk_scene.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

template <typename _tHeight>
struct STileHeights {
						::gpk::array_static<_tHeight, 4>									Heights;
};

struct STiledTerrainCounters {
						uint32_t															nBaseTileCount					;	// Base tile count is equal to (tile map width*tile map depth)
						uint32_t															nTileColumnCount				;	// Stores the amount of tile columns that contain a single map chunk
						uint32_t															nTileRowCount					;	// Stores the amount of tile rows that contain a single map chunk
						uint32_t															nTopTileFaceCount				;	// Stores the number of tiles facing upwards that contain valid attributes and geometry

						uint32_t															nFrontTileFaceCount				;	// Stores the number of tiles facing front that contain valid attributes and geometry
						uint32_t															nRightTileFaceCount				;	// Stores the number of tiles facing right that contain valid attributes and geometry
						uint32_t															nBottomTileFaceCount			;	// Stores the number of tiles facing upwards that contain valid attributes and geometry
						uint32_t															nBackTileFaceCount				;	// Stores the number of tiles facing front that contain valid attributes and geometry

						uint32_t															nLeftTileFaceCount				;	// Stores the number of tiles facing right that contain valid attributes and geometry
						uint32_t															nTotalTileFaceCount				;	// Stores the total number of valid tiles, which should be equal to the addition of top, front and right tiles.
						uint32_t															nChunkColumnTileCount			;	// Stores the amount of tile columns that contain a single map chunk
						uint32_t															nChunkRowTileCount				;	// Stores the amount of tile rows that contain a single map chunk

						uint32_t															nChunkTotalTileCount			;	// Stores the total number of tiles contained in a single chunk
						uint32_t															nColumnChunkCount				;	// Stores the amount of column chunks
						uint32_t															nRowChunkCount					;	// Stores the amount of row chunks
						uint32_t															nTotalChunkCount				;	// Stores the total chunks contained in a map
};

struct SRenderCache {
						::gpk::array_pod<::gpk::SCoord2<int32_t>>							TrianglePixelCoords							= {};
						::gpk::array_pod<::gpk::STriangleWeights<double>>					TrianglePixelWeights						= {};
						::gpk::array_pod<::gpk::SCoord2<int32_t>>							WireframePixelCoords						= {};

						::gpk::array_pod<::gpk::SColorBGRA>									Triangle3dColorList							= {};
						::gpk::array_pod<::gpk::SCoord3<float>>								TransformedNormalsTriangle					= {};
						::gpk::array_pod<::gpk::STriangle3D<float>>							TransformedNormalsVertex					= {};

						::gpk::array_pod<int32_t>											Triangle3dIndices							= {};
						::gpk::array_pod<::gpk::STriangle3D<float>>							Triangle3dToDraw							= {};
						::gpk::array_pod<::gpk::STriangle3D<float>>							Triangle3dWorld								= {};

						int32_t																TrianglesDrawn								= 0;
						int32_t																PixelsDrawn									= 0;
						int32_t																PixelsSkipped								= 0;
};

namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{
	struct SApplication {
							::gpk::SFramework													Framework;
							::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	Offscreen						= {};
							::gpk::array_obj<::gpk::SImage<::gpk::SColorBGRA>>					PNGImages						= {};

							int32_t																IdExit							= -1;

							::std::mutex														LockRender;

							::gpk::SDialog														DialogMain;
							int32_t																Slider;
							int32_t																NumericTuner;
							int32_t																CheckBox;
							int32_t																Viewport;

							::gpk::SImage<::gpk::SColorBGRA>									TextureGrid									= {};
							::gpk::SImage<::STileHeights<float>>								TileHeights									= {};
							::gpk::SModelGeometry	<float>										Grid										= {};
							::gpk::SModelPivot		<float>										GridPivot									= {};
							::gpk::SScene														Scene;
							::gpk::SCoord3<float>												LightDirection								= {10, 5, 0};

							::SRenderCache														RenderCache									= {};

							int32_t																IdFrameRateUpdate				= -1;
							int32_t																IdFrameRateRender				= -1;
							char																StringFrameRateUpdate	[256]	= {};
							char																StringFrameRateRender	[256]	= {};

																								SApplication				(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace


#endif // APPLICATION_H_2078934982734
