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
	typedef	u3_t				tileid_t;
	typedef	::gpk::img<tileid_t>	tilegrid;

	GDEFINE_ENUM_TYPE (TILE_FLAG, u0_t);
	GDEFINE_ENUM_VALUE(TILE_FLAG, Walk, 1);
	GDEFINE_ENUM_VALUE(TILE_FLAG, Shot, 2);

	struct STileBlock {
		::gpk::apod<TILE_FLAG>	Flags;
		::gpk::tilegrid			Tiles;

		::gpk::err_t			Save				(::gpk::au0_t & output)	const	{ 
			if_fail_fe(gpk::saveView (output, Flags)); 
			if_fail_fe(gpk::saveImage(output, Tiles.View)); 
			return 0;
		}

		::gpk::err_t			Load				(::gpk::vcu0_t & input)	{ 
			if_fail_fe(gpk::loadView (input, Flags )); 
			if_fail_fe(gpk::loadImage(input, Tiles)); 
			return 0;
		}
	};

#pragma pack(push, 1)
	struct STileMapSetup {
		u3_t				Seed				= ::gpk::noise1DBase(::gpk::timeCurrentInUs());
		::gpk::n2u0_t				BlockSize			= {255, 255};
		::gpk::n2u1_t			WorldSize			= {255, 255};	// in blocks
	};
#pragma pack(pop)

	struct STileMap {
		::gpk::STileMapSetup		Config;	// Map constants
		::gpk::an3u32				Coords;	// Block coords
		::gpk::apod<STimestamps>	Stamps;	// 
		::gpk::apobj<STileBlock>	Blocks;	// 
	
		::gpk::err_t			Save				(::gpk::au0_t & output)	const	{ 
			if_fail_fe(gpk::savePOD (output, Config)); 
			if_fail_fe(gpk::saveView(output, Coords)); 
			if_fail_fe(gpk::saveView(output, Stamps)); 
			if_fail_fe(gpk::savePOD (output, Blocks.size())); 
			for(u2_t iMap = 0; iMap < Blocks.size(); ++iMap) {
				if_fail_fe(Blocks[iMap] ? Blocks[iMap]->Save(output) : STileBlock{}.Save(output));
			}
			return 0; 
		}
		::gpk::err_t			Load				(::gpk::vcu0_t & input) { 
			if_fail_fe(gpk::loadPOD (input, Config)); 
			if_fail_fe(gpk::loadView(input, Coords)); 
			if_fail_fe(gpk::loadView(input, Stamps)); 
			u2_t					blockCount			= {};
			if_fail_fe(gpk::loadPOD (input, blockCount)); 
			if_fail_fe(Blocks.reserve(blockCount));
			for(u2_t iMap = 0; iMap < blockCount; ++iMap) {
				::gpk::pobj<STileBlock>		block;
				if_fail_fe(block->Load(input));
				if(0 == block->Tiles.size())
					block.clear();

				if_fail_fe(Blocks.push_back(block));
			}
			return 0; 
		}
	};

} // namespace ssg

#endif // GPK_TILE_MAP
