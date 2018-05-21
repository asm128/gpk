#include "gpk_color.h"
#include "gpk_texture.h"

#include <string>

#ifndef TERRAIN_H_289374982374
#define TERRAIN_H_289374982374

namespace gpk
{
#pragma pack( push, 1 )
	// U and V TexCoords follow the same order from GAT format heights
	struct STileSkinGND {
				float												u[4]							= { 0, 1, 0, 1 };				//< west->east, north->south ordering; 0=left 1=right
				float												v[4]							= { 0, 0, 1, 1 };				//< west->east, north->south ordering; 0=up 1=down
				int16_t												TextureIndex					= -1			;		//< -1 for none
				int16_t												LightmapIndex					= -1			;		//< -1 for none?
				uint32_t											dwColor							= 0xFFFFFFFFU	;			// a default color to set to the tile for other kind of representations
	};

	struct STileFaceSkinMapping {
				int32_t												SkinIndexTop					= -1;	// <= -1 for none
				int32_t												SkinIndexRight					= -1;	// <= -1 for none
				int32_t												SkinIndexFront					= -1;	// <= -1 for none
	};

	struct STileGeometryGND {
				float												fHeight[4]						= {0, 0, 0, 0};				// west->east, north->south ordering
				::gpk::STileFaceSkinMapping							SkinMapping;	// <= -1 for none
				//int16_t												Flags							= 0;					// GND v <= 1.5 // maybe a color key? a terrain property? We're going to use it to tell if the triangle is inverted.
	};

	// 0 - No walkable - No Snipable
	// 1 - Walkable
	// 2 - Snipable
	// 3 - Walkable - Snipable
	struct STileNavigabilityGND {
				float												fHeight[4]						= {}; // west->east, south->north ordering
				uint8_t												Type							= 0;
				uint8_t												FlagsTop						= 0;
				uint8_t												FlagsRight						= 0;
				uint8_t												FlagsFront						= 0;
	};

	struct STileBrightnessGND 	{
				uint8_t												Brightness	[8][8]				= {};
				::gpk::SColorRGB									ColorRGB	[8][8]				= {};
	};

	struct STiledTerrainMetricsGND {
				::gpk::SCoord2<uint32_t>							Size							;	// Tile count 
				float												TileScale						;	// The size to expand the tiles
	};

	struct STerrainWaterGND {
				float												fSeaLevel						;	//
				int32_t												nWaterType						;	//
				float												fWaveHeight						;	//
				float												fWaveSpeed						;	//
				float												fWavePitch						;	//
				int32_t												nAnimationSpeed					;	//
	};

	struct SGNDFileContents {
				STiledTerrainMetricsGND								Metrics;
				::gpk::array_obj<::std::string>						TextureNames;
				uint32_t											LightmapTiles;		// ?? 
				::gpk::SCoord2<uint32_t>							LightmapSize;	
				::gpk::array_pod<STileBrightnessGND	>				lstTileBrightnessData;
				::gpk::array_pod<STileSkinGND		>				lstTileTextureData;
				::gpk::array_pod<STileGeometryGND	>				lstTileGeometryData;
	};



	//709655609
			::gpk::error_t										gndFileLoad						(SGNDFileContents& loaded, const ::gpk::array_view<ubyte_t>	& input);
			::gpk::error_t										gndFileLoad						(SGNDFileContents& loaded, FILE								* input);
			::gpk::error_t										gndFileLoad						(SGNDFileContents& loaded, const ::gpk::view_const_string	& input);

	struct SModelNodeGND {
				::gpk::array_pod<::gpk::SCoord3<float>>				Normals			;
				::gpk::array_pod<::gpk::SCoord3<float>>				Vertices		;
				::gpk::array_pod<::gpk::SCoord2<float>>				UVs				;
				::gpk::array_pod<::gpk::STriangleWeights<uint32_t>>	VertexIndices	; 
				//::gpk::array_pod<int32_t>							SkinIndices		; // one per triangle (VertexIndices.size() / 3)
	};

	struct STileMapping {
				int32_t												nChunkIndex;
				int32_t												SubsetIndexTop;
				int32_t												SubsetIndexFront;
				int32_t												SubsetIndexRight;
				int32_t												VerticesTop[4];
				int32_t												VerticesFront[4];
				int32_t												VerticesRight[4];
	};

	struct SModelGND {
				::gpk::array_obj<::gpk::SModelNodeGND>				Nodes;
				::gpk::STexture<::gpk::STileMapping>				TileMapping;
	
//																	SModelGND						()							= default;
//																	SModelGND						(const SModelGND& other)	= default;
	};

	enum TILE_FACE_FACING 
		{ TILE_FACE_FACING_TOP
		, TILE_FACE_FACING_FRONT
		, TILE_FACE_FACING_RIGHT
		, TILE_FACE_FACING_BOTTOM
		, TILE_FACE_FACING_BACK
		, TILE_FACE_FACING_LEFT
		};

			::gpk::error_t										gndGenerateFaceGeometry
	( const ::gpk::array_pod<STileSkinGND		>	lstTileTextureData
	, const ::gpk::array_pod<STileGeometryGND	>	lstTileGeometryData
	, const ::gpk::STiledTerrainMetricsGND			tileMapMetrics
	, TILE_FACE_FACING								facing_direction
	, int32_t										textureIndex
	, ::gpk::SModelNodeGND							& generated
	, ::gpk::grid_view<::gpk::STileMapping>			& out_mapping
	);
#pragma pack(pop)
} // namespace

#endif // TERRAIN_H_289374982374