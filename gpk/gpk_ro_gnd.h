#include "gpk_color.h"
#include "gpk_image.h"
#include "gpk_tri.h"
#include "gpk_array_obj.h"

#ifndef TERRAIN_H_23627
#define TERRAIN_H_23627

nmsp gpk
{
#pragma pack( push, 1 )
	// U and V TexCoords follow the same order from GAT format heights
	struct STileSkinGND {
		f2_t									u[4]							= { 0, 1, 0, 1 };		//< west->east, north->south ordering; 0=left 1=right
		f2_t									v[4]							= { 0, 0, 1, 1 };		//< west->east, north->south ordering; 0=up 1=down
		s1_t									TextureIndex					= -1			;		//< -1 for none
		s1_t									LightmapIndex					= -1			;		//< -1 for none?
		u2_t								dwColor							= 0xFFFFFFFFU	;		// a default color to set to the tile for other kind of representations
	};

	struct STileFaceSkinMapping {
		s2_t									SkinIndexTop					= -1;	// <= -1 for none
		s2_t									SkinIndexRight					= -1;	// <= -1 for none
		s2_t									SkinIndexFront					= -1;	// <= -1 for none
	};

	struct STileGeometryGND {
		f2_t									fHeight[4]						;			// west->east, north->south ordering
		STileFaceSkinMapping				SkinMapping						;	// <= -1 for none
		//s1_t									Flags						= 0;				// GND v <= 1.5 // maybe a color key? a terrain property? We're going to use it to tell if the triangle is inverted.
	};

	// 0 - No walkable - No Snipable
	// 1 - Walkable
	// 2 - Snipable
	// 3 - Walkable - Snipable
	struct STileNavigabilityGND {
		f2_t									fHeight[4]						= {}; // west->east, south->north ordering
		u0_t									Type							= 0;
		u0_t									FlagsTop						= 0;
		u0_t									FlagsRight						= 0;
		u0_t									FlagsFront						= 0;
	};

	struct STileBrightnessGND 	{
		u0_t									Brightness	[8][8]				= {};
		SColorRGB						ColorRGB	[8][8]				= {};
	};

	struct STiledTerrainMetricsGND {
		n2u2_t							Size							;	// Tile count
		f2_t									TileScale						;	// The size to expand the tiles
	};

	struct STerrainWaterGND {
		f2_t									fSeaLevel						;	//
		s2_t									nWaterType						;	//
		f2_t									fWaveHeight						;	//
		f2_t									fWaveSpeed						;	//
		f2_t									fWavePitch						;	//
		s2_t									nAnimationSpeed					;	//
	};

	struct SGNDFileContents {
		STiledTerrainMetricsGND					Metrics;
		aobj<apod<sc_t>>			TextureNames;
		u2_t								LightmapTiles;		// ??
		n2u2_t							LightmapSize;
		apod<STileBrightnessGND	>		lstTileBrightnessData;
		apod<STileSkinGND		>		lstTileTextureData;
		apod<STileGeometryGND	>		lstTileGeometryData;
	};

	//709655609
	gpk::err_t 						gndFileLoad						(SGNDFileContents& loaded, vu0_c		& input);
	gpk::err_t 						gndFileLoad						(SGNDFileContents& loaded, FILE				* input);
	gpk::err_t 						gndFileLoad						(SGNDFileContents& loaded, vcst_c	& input);

	struct SModelNodeGND {
		apod<n3f2_t>				Normals			;
		apod<n3f2_t>				Vertices		;
		apod<n2f2_t>				UVs				;
		apod<tri<u2_t>>		VertexIndices	;
		//apod<s2_t>					SkinIndices		; // one per triangle (VertexIndices.size() / 3)
	};

	struct STileMapping {
		s2_t									nChunkIndex;
		s2_t									SubsetIndexTop;
		s2_t									SubsetIndexFront;
		s2_t									SubsetIndexRight;
		s2_t									VerticesTop[4];
		s2_t									VerticesFront[4];
		s2_t									VerticesRight[4];
	};

	struct SModelGND {
		aobj<SModelNodeGND>		Nodes;
		img<STileMapping>			TileMapping;
	};

	struct STiledTerrainCounters {
		u2_t								nBaseTileCount						;	// Base tile count is equal to (tile map width*tile map depth)
		u2_t								nTileColumnCount					;	// Stores the amount of tile columns that contain a single map chunk
		u2_t								nTileRowCount						;	// Stores the amount of tile rows that contain a single map chunk
		u2_t								nTopTileFaceCount					;	// Stores the number of tiles facing upwards that contain valid attributes and geometry

		u2_t								nFrontTileFaceCount					;	// Stores the number of tiles facing front that contain valid attributes and geometry
		u2_t								nRightTileFaceCount					;	// Stores the number of tiles facing right that contain valid attributes and geometry
		u2_t								nBottomTileFaceCount				;	// Stores the number of tiles facing upwards that contain valid attributes and geometry
		u2_t								nBackTileFaceCount					;	// Stores the number of tiles facing front that contain valid attributes and geometry

		u2_t								nLeftTileFaceCount					;	// Stores the number of tiles facing right that contain valid attributes and geometry
		u2_t								nTotalTileFaceCount					;	// Stores the total number of valid tiles, which should be equal to the addition of top, front and right tiles.
		u2_t								nChunkColumnTileCount				;	// Stores the amount of tile columns that contain a single map chunk
		u2_t								nChunkRowTileCount					;	// Stores the amount of tile rows that contain a single map chunk

		u2_t								nChunkTotalTileCount				;	// Stores the total number of tiles contained in a single chunk
		u2_t								nColumnChunkCount					;	// Stores the amount of column chunks
		u2_t								nRowChunkCount						;	// Stores the amount of row chunks
		u2_t								nTotalChunkCount					;	// Stores the total chunks contained in a map
	};

	enum TILE_FACE_FACING
		{ TILE_FACE_FACING_TOP
		, TILE_FACE_FACING_FRONT
		, TILE_FACE_FACING_RIGHT
		, TILE_FACE_FACING_BOTTOM
		, TILE_FACE_FACING_BACK
		, TILE_FACE_FACING_LEFT
		};

	gpk::err_t 						gndGenerateFaceGeometry
		( const apod<STileSkinGND		>	lstTileTextureData
		, const apod<STileGeometryGND	>	lstTileGeometryData
		, const STiledTerrainMetricsGND		tileMapMetrics
		, TILE_FACE_FACING							facing_direction
		, s2_t									textureIndex
		, SModelNodeGND						& generated
		, grid<STileMapping>			& out_mapping
		);

	gpk::err_t 						blendGNDNormals		(const grid<STileGeometryGND> &tileGeometryView, const view<STileSkinGND>& lstTileSkinData, const grid<STileMapping>& tileMappingView, view<SModelNodeGND> & gndModelNodes);

#pragma pack(pop)
} // nmsp

#endif // TERRAIN_H_23627
