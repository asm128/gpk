#include "gpk_coord.h"
#include "gpk_enum.h"
#include "gpk_ptr.h"
#include "gpk_color.h"

#ifndef GPK_VOXEL_H_098273498237423
#define GPK_VOXEL_H_098273498237423

namespace gpk
{
#pragma pack(push, 1)
	static constexpr ::gpk::SCoord3<float>				VOXEL_VERTICES		[]			= 
			{ {0, 1, 0}, {1, 1, 0}, {0, 1, 1}, {1, 1, 1} // Top
			, {0, 0, 0}, {1, 0, 0}, {0, 0, 1}, {1, 0, 1} // Bottom
			}; 
														
	GDEFINE_ENUM_TYPE(VOXEL_FACE, uint8_t);
	GDEFINE_ENUM_VALUE(VOXEL_FACE, Top		, 0);
	GDEFINE_ENUM_VALUE(VOXEL_FACE, Front	, 1);
	GDEFINE_ENUM_VALUE(VOXEL_FACE, Right	, 2);
	GDEFINE_ENUM_VALUE(VOXEL_FACE, Bottom	, 3);
	GDEFINE_ENUM_VALUE(VOXEL_FACE, Back		, 4);
	GDEFINE_ENUM_VALUE(VOXEL_FACE, Left		, 5);

	struct SVoxelFaceBits {
		bool		 Top			: 1;
		bool		 Front			: 1;
		bool		 Right			: 1;
		bool		 Bottom			: 1;
		bool		 Back			: 1;
		bool		 Left			: 1;
	};

	static constexpr ::gpk::SQuad3<float>				VOXEL_FACE_VERTICES		[]	= 
		{ {{0, 1, 0}, {1, 1, 0}, {0, 1, 1}, {1, 1, 1}}  // Top		(y = 1)
		, {{1, 0, 0}, {1, 1, 0}, {1, 0, 1}, {1, 1, 1}}	// Front	(x = 1)
		, {{0, 0, 1}, {1, 0, 1}, {0, 1, 1}, {1, 1, 1}}	// Right	(z = 1)
		, {{0, 0, 0}, {1, 0, 0}, {0, 0, 1}, {1, 0, 1}}  // Bottom	(y = 0)
		, {{0, 0, 0}, {0, 1, 0}, {0, 0, 1}, {0, 1, 1}}	// Back		(x = 0)
		, {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}}	// Left		(z = 0)
		}; 

	constexpr ::gpk::SCoord3<float>						VOXEL_FACE_NORMALS			[6]	= {{0, 1, 0}, {1, 0, 0}, {0, 0, 1}, {0, -1, 0}, {-1, 0, 0}, {0, 0, -1}};	// top, front, right, bottom, back, left
	constexpr ::gpk::SCoord3<int8_t>					VOXEL_DELTAS				[6]	= 
		{ { 0, 1, 0} // Top		(y = 1)
		, { 1, 0, 0} // Front	(x = 1)
		, { 0, 0, 1} // Right	(z = 1)
		, { 0,-1, 0} // Bottom	(y = -1)
		, {-1, 0, 0} // Back	(x = -1)
		, { 0, 0,-1} // Left	(z = -1)
		};
	constexpr uint8_t									VOXEL_FACE_INDICES			[6][6]	= 
		{ {0, 2, 1, 1, 2, 3} // Top
		, {0, 1, 2, 1, 3, 2}
		, {0, 1, 2, 1, 3, 2}
		, {0, 1, 2, 1, 3, 2} // Bottom
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

	template <typename _uIndex>
	struct SVoxel {
		::gpk::SCoord3<_uIndex>							Position;
		_uIndex											ColorIndex;

		bool											Limits						(::gpk::VOXEL_FACE face)	const	{
			switch(face) {
			case ::gpk::VOXEL_FACE_Top		: return Position.y == 255;
			case ::gpk::VOXEL_FACE_Front	: return Position.x == 255;
			case ::gpk::VOXEL_FACE_Right	: return Position.z == 255;
			case ::gpk::VOXEL_FACE_Bottom	: return Position.y ==   0;
			case ::gpk::VOXEL_FACE_Back		: return Position.x ==   0;
			case ::gpk::VOXEL_FACE_Left		: return Position.z ==   0;
			}
			return false;
		}
	};

#pragma pack(pop)

	template<typename _tCell>
	struct SVoxelChunk {
		::gpk::array_pobj<::gpk::SVoxelLayer<_tCell>>	Layers						= {};
	};

	template<typename _tCell>
	struct SVoxelMap {
		::gpk::SCoord3<uint8_t>							Dimensions;
		::gpk::array_pod<::gpk::SColorBGRA>				Palette;
		::gpk::array_pod<::gpk::SVoxel<uint8_t>>		Voxels;
		::gpk::array_obj<::gpk::SVoxelChunk<_tCell>>	Chunks;
		::gpk::array_pod<::gpk::SCoord2<uint8_t>>		ChunkPositions;

		int32_t											GetChunk					(const ::gpk::SCoord2<uint8_t> & chunkCoords) const	{ 
			return ::gpk::find(chunkCoords, ::gpk::view_array<const ::gpk::SCoord2<uint8_t>>{ChunkPositions}); 
		}

		int32_t											GetChunkForVoxel			(const ::gpk::SCoord3<uint8_t> & voxelPosition) const	{ 
			const ::gpk::SCoord3<uint8_t>						floor						= voxelPosition / SVoxelLayer<_tCell>::WIDTH; 
			return GetChunk({floor.x, floor.z}); 
		}

		int32_t											SetValue					(const ::gpk::SCoord3<uint8_t> & voxelPosition, const _tCell & cellValue) 	{ 
			const ::gpk::SCoord3<uint8_t>						floor						= voxelPosition / SVoxelLayer<_tCell>::WIDTH; 
			int32_t												indexChunk					= GetChunk({floor.x, floor.z});
			if(0 > indexChunk) {
				indexChunk										= Chunks.push_back({});
				ChunkPositions.push_back({floor.x, floor.z});
			}

			::gpk::SVoxelChunk<_tCell>							& chunk						= Chunks[indexChunk];
			if(chunk.Layers.size() < uint32_t(voxelPosition.y) + 1)
				chunk.Layers.resize(voxelPosition.y + 1);

			_tCell												& cell						= chunk.Layers[voxelPosition.y]->Cells[voxelPosition.z % SVoxelLayer<_tCell>::DEPTH][voxelPosition.x % SVoxelLayer<_tCell>::WIDTH];
			if(cell != cellValue) {
				if(0 == cell) 
					Voxels.push_back({voxelPosition.x, voxelPosition.y, voxelPosition.z, cellValue});
				else {
					uint32_t											foundVoxel					= (uint32_t)-1;
					for(uint32_t iVoxel = 0; iVoxel < Voxels.size(); ++iVoxel) {
						::gpk::SVoxel<uint8_t>								& voxel						= Voxels[iVoxel];
						if(voxel.Position == voxelPosition) {
							if(0 == cellValue)
								Voxels.remove_unordered(foundVoxel);
							else {
								foundVoxel										= iVoxel;
								Voxels[foundVoxel].ColorIndex = cellValue;
							}
							break;
						}

					}
					if(foundVoxel >= Voxels.size()) {
						warning_printf("Voxel not found! Voxel value: %i", (int32_t)cellValue);
						Voxels.push_back({{voxelPosition.x, voxelPosition.y, voxelPosition.z}, cellValue});
					}
				}
				cell		= cellValue;
			}
			return indexChunk;
		}

		int32_t											GetValue					(const ::gpk::SCoord3<uint8_t> & voxelPosition, _tCell & cellValue) const	{ 
			const ::gpk::SCoord3<uint8_t>						floor						= voxelPosition / SVoxelLayer<_tCell>::WIDTH; 
			int32_t												indexChunk					= GetChunk({floor.x, floor.z});
			if(0 > indexChunk) 
				return -1;

			const ::gpk::SVoxelChunk<_tCell>				 	& chunk						= Chunks[indexChunk];
			if(voxelPosition.y >= chunk.Layers.size()) 
				return -1;

			const ::gpk::ptr_obj<::gpk::SVoxelLayer<_tCell>>	& layer						= chunk.Layers[voxelPosition.y];
			if(0 == layer.get_ref())
				return -1;

			cellValue										= layer->Cells[voxelPosition.z % SVoxelLayer<_tCell>::DEPTH][voxelPosition.x % SVoxelLayer<_tCell>::WIDTH];
			return 0;
		}
	};
} // namespace

#endif // GPK_VOXEL_H_098273498237423