#include "gpk_img_serialize.h"
#include "gpk_array_ptr.h"
#include "gpk_apod_n2.h"
#include "gpk_apod_n3.h"

#include "gpk_noise.h"
#include "gpk_enum.h"

#ifndef GPK_TILE_MAP
#define GPK_TILE_MAP

namespace gpk
{
	typedef	uint64_t				tileid_t;
	typedef	::gpk::img<tileid_t>	tilegrid;

	GDEFINE_ENUM_TYPE (TILE_FLAG, uint8_t);
	GDEFINE_ENUM_VALUE(TILE_FLAG, Walk, 1);
	GDEFINE_ENUM_VALUE(TILE_FLAG, Shot, 2);

	struct STileBlock {
		::gpk::apod<TILE_FLAG>	Flags;
		::gpk::tilegrid			Tiles;

		::gpk::error_t			Save				(::gpk::au8 & output)	const	{ 
			gpk_necs(::gpk::saveView (output, Flags)); 
			gpk_necs(::gpk::saveImage(output, Tiles.View)); 
			return 0;
		}

		::gpk::error_t			Load				(::gpk::vcu8 & input)	{ 
			gpk_necs(::gpk::loadView (input, Flags )); 
			gpk_necs(::gpk::loadImage(input, Tiles)); 
			return 0;
		}
	};

#pragma pack(push, 1)
	struct STileMapSetup {
		uint64_t				Seed				= ::gpk::noise1DBase(::gpk::timeCurrentInUs());
		::gpk::n2u8				BlockSize			= {255, 255};
		::gpk::n2u16			WorldSize			= {255, 255};	// in blocks
	};
#pragma pack(pop)

	struct STileMap {
		::gpk::STileMapSetup		Config;	// Map constants
		::gpk::an3u32				Coords;	// Block coords
		::gpk::apod<STimestamps>	Stamps;	// 
		::gpk::apobj<STileBlock>	Blocks;	// 
	
		::gpk::error_t			Save				(::gpk::au8 & output)	const	{ 
			gpk_necs(::gpk::savePOD (output, Config)); 
			gpk_necs(::gpk::saveView(output, Coords)); 
			gpk_necs(::gpk::saveView(output, Stamps)); 
			gpk_necs(::gpk::savePOD (output, Blocks.size())); 
			for(uint32_t iMap = 0; iMap < Blocks.size(); ++iMap) {
				gpk_necs(Blocks[iMap] ? Blocks[iMap]->Save(output) : STileBlock{}.Save(output));
			}
			return 0; 
		}
		::gpk::error_t			Load				(::gpk::vcu8 & input) { 
			gpk_necs(::gpk::loadPOD (input, Config)); 
			gpk_necs(::gpk::loadView(input, Coords)); 
			gpk_necs(::gpk::loadView(input, Stamps)); 
			uint32_t					blockCount			= {};
			gpk_necs(::gpk::loadPOD (input, blockCount)); 
			gpk_necs(Blocks.reserve(blockCount));
			for(uint32_t iMap = 0; iMap < blockCount; ++iMap) {
				::gpk::pobj<STileBlock>		block;
				gpk_necs(block->Load(input));
				if(0 == block->Tiles.size())
					block.clear();

				gpk_necs(Blocks.push_back(block));
			}
			return 0; 
		}
	};

} // namespace ssiege

#endif // GPK_TILE_MAP
