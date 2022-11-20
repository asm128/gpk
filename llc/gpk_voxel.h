#include "gpk_coord.h"
#include "gpk_enum.h"

#ifndef GPK_VOXEL_H_098273498237423
#define GPK_VOXEL_H_098273498237423

namespace gpk
{
#pragma pack(push, 1)
	static constexpr ::gpk::SCoord3<float>				VOXEL_VERTICES		[]			= 
			{ {0, 1, 0}, {1, 1, 0}, {0, 1, 1}, {1, 1, 1}
			, {0, 0, 0}, {1, 0, 0}, {0, 0, 1}, {1, 0, 1}
			}; 

	GDEFINE_ENUM_TYPE(VOXEL_FACE, uint8_t);
	GDEFINE_ENUM_VALUE(VOXEL_FACE, TOP		, 0);
	GDEFINE_ENUM_VALUE(VOXEL_FACE, FRONT	, 1);
	GDEFINE_ENUM_VALUE(VOXEL_FACE, RIGHT	, 2);
	GDEFINE_ENUM_VALUE(VOXEL_FACE, BOTTOM	, 3);
	GDEFINE_ENUM_VALUE(VOXEL_FACE, BACK		, 4);
	GDEFINE_ENUM_VALUE(VOXEL_FACE, LEFT		, 5);

	static constexpr ::gpk::SQuad3<float>				VOXEL_FACE_VERTICES		[]	= 
		{ {{0, 1, 0}, {1, 1, 0}, {0, 1, 1}, {1, 1, 1}}
		, {{1, 0, 0}, {1, 1, 0}, {1, 0, 1}, {1, 1, 1}}
		, {{0, 0, 1}, {1, 0, 1}, {0, 1, 1}, {1, 1, 1}}
		, {{0, 0, 0}, {1, 0, 0}, {0, 0, 1}, {1, 0, 1}}
		, {{0, 0, 0}, {0, 1, 0}, {0, 0, 1}, {0, 1, 1}}
		, {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}}
		}; 

	constexpr ::gpk::SCoord3<float>						VOXEL_FACE_NORMALS			[6]	= {{0, 1, 0}, {1, 0, 0}, {0, 0, 1}, {0, -1, 0}, {-1, 0, 0}, {0, 0, -1}};	// top, front, right, bottom, back, left

	constexpr uint8_t									VOXEL_FACE_INDICES			[6][6]	= 
		{ {0, 2, 1, 1, 2, 3}
		, {0, 1, 2, 1, 3, 2}
		, {0, 1, 2, 1, 3, 2}
		, {0, 1, 2, 1, 3, 2}
		, {0, 2, 1, 1, 2, 3}
		, {0, 2, 1, 1, 2, 3}
		}; 

	template<typename _tCell, uint16_t _width = 16>
	struct SVoxelLayer {
		static constexpr	uint32_t					WIDTH						= _width;
		static constexpr	uint32_t					DEPTH						= WIDTH;

		uint16_t										Y							= {};
		_tCell											Cells	[DEPTH][WIDTH]		= {};
	};

	template<typename _tCell>
	struct SVoxelChunk {
		::gpk::array_pobj<::gpk::SVoxelLayer<_tCell>>	Layers						= {};
	};

	template<typename _tCell>
	struct SVoxelMap {
		::gpk::array_obj<::gpk::SVoxelChunk<_tCell>>	Chunks;
		::gpk::array_pod<::gpk::SCoord2<uint16_t>>		ChunkPositions;

		int32_t											GetChunkForVoxel			(const ::gpk::SCoord3<uint32_t> & voxelPosition) const	{ 
			const ::gpk::SCoord3<uint16_t>						floor						= voxelPosition / SVoxelLayer<_tCell>::WIDTH; 
			return ::gpk::find({floor.x, floor.z}, ::gpk::view_array<const ::gpk::SCoord2<uint16_t>>{ChunkPositions}); 
		}

		int32_t											GetChunk					(const ::gpk::SCoord2<uint16_t> & chunkCoords) const	{ 
			return ::gpk::find(chunkCoords, ::gpk::view_array<const ::gpk::SCoord2<uint16_t>>{ChunkPositions}); 
		}

		int32_t											SetValue					(const ::gpk::SCoord3<uint32_t> & voxelPosition, const _tCell & cellValue) 	{ 
			const ::gpk::SCoord3<uint16_t>						floor						= (voxelPosition / SVoxelLayer<_tCell>::WIDTH).Cast<uint16_t>(); 
			int32_t												indexChunk					= GetChunk({floor.x, floor.z});
			if(0 > indexChunk) {
				indexChunk										= Chunks.push_back({});
				ChunkPositions.push_back({floor.x, floor.z});
			}

			::gpk::SVoxelChunk<_tCell>							& chunk						= Chunks[indexChunk];
			if(chunk.Layers.size() < (voxelPosition.y + 1))
				chunk.Layers.resize(voxelPosition.y + 1);

			chunk.Layers[voxelPosition.y]->Cells[voxelPosition.z % SVoxelLayer<_tCell>::DEPTH][voxelPosition.x % SVoxelLayer<_tCell>::WIDTH] = cellValue;
			return indexChunk;
		}

		int32_t											GetValue					(const ::gpk::SCoord3<uint32_t> & voxelPosition, _tCell & cellValue) const	{ 
			const ::gpk::SCoord3<uint16_t>						floor						= (voxelPosition / SVoxelLayer<_tCell>::WIDTH).Cast<uint16_t>(); 
			int32_t												indexChunk					= GetChunk({floor.x, floor.z});
			if(0 > indexChunk) 
				return -1;
			const ::gpk::SVoxelChunk<_tCell>				 	& chunk						= Chunks[indexChunk];
			if(voxelPosition.y >= chunk.Layers.size() || 0 == chunk.Layers[voxelPosition.y].get_ref())
				return -1;
			cellValue										= chunk.Layers[voxelPosition.y]->Cells[voxelPosition.z % SVoxelLayer<_tCell>::DEPTH][voxelPosition.x % SVoxelLayer<_tCell>::WIDTH];
			return 0;
		}
	};
#pragma pack(pop)
} // namespace

#endif // GPK_VOXEL_H_098273498237423