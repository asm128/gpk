#include "gpk_img_serialize.h"
#include "gpk_array_ptr.h"
#include "gpk_apod_n2.h"
#include "gpk_apod_n3.h"
#include "gpk_chrono.h"

#include "gpk_noise.h"
#include "gpk_enum.h"

#ifndef GPK_TILE_MAP
#define GPK_TILE_MAP

nmsp gpk
{
	tydf	u3_t			tileid_t;
	tydf	img<tileid_t>	tilegrid;

	GDEFINE_ENUM_TYPE (TILE_FLAG, u0_t);
	GDEFINE_ENUM_VALUE(TILE_FLAG, Walk, 1);
	GDEFINE_ENUM_VALUE(TILE_FLAG, Shot, 2);

	struct STileBlock {
		apod<TILE_FLAG>	Flags;
		tilegrid		Tiles;

		err_t 			Save				(au0_t & output)	const	{ 
			if_fail_fe(saveView (output, Flags)); 
			if_fail_fe(saveImage(output, Tiles.View)); 
			return 0;
		}

		err_t 			Load				(vcu0_t & input)	{ 
			if_fail_fe(loadView (input, Flags )); 
			if_fail_fe(loadImage(input, Tiles)); 
			return 0;
		}
	};

#pragma pack(push, 1)
	struct STileMapSetup {
		u3_t				Seed				= noise1DBase(timeCurrentInUs());
		n2u0_t				BlockSize			= {255, 255};
		n2u1_t				WorldSize			= {255, 255};	// in blocks
	};
#pragma pack(pop)

	struct STileMap {
		STileMapSetup		Config;	// Map constants
		a3u2_t				Coords;	// Block coords
		apod<STimestamps>	Stamps;	// 
		apobj<STileBlock>	Blocks;	// 
	
		err_t 		Save				(au0_t & output)	const	{ 
			if_fail_fe(savePOD (output, Config)); 
			if_fail_fe(saveView(output, Coords)); 
			if_fail_fe(saveView(output, Stamps)); 
			if_fail_fe(savePOD (output, Blocks.size())); 
			for(u2_t iMap = 0; iMap < Blocks.size(); ++iMap) {
				if_fail_fe(Blocks[iMap] ? Blocks[iMap]->Save(output) : STileBlock{}.Save(output));
			}
			return 0; 
		}
		err_t 		Load				(vcu0_t & input) { 
			if_fail_fe(loadPOD (input, Config)); 
			if_fail_fe(loadView(input, Coords)); 
			if_fail_fe(loadView(input, Stamps)); 
			u2_t					blockCount			= {};
			if_fail_fe(loadPOD (input, blockCount)); 
			if_fail_fe(Blocks.reserve(blockCount));
			for(u2_t iMap = 0; iMap < blockCount; ++iMap) {
				pobj<STileBlock>		block;
				if_fail_fe(block->Load(input));
				if(0 == block->Tiles.size())
					block.clear();

				if_fail_fe(Blocks.push_back(block));
			}
			return 0; 
		}
	};

} // nmsp ssg

#endif // GPK_TILE_MAP
