#include "gpk_coord.h"
#include "gpk_enum.h"
#include "gpk_ptr.h"
#include "gpk_color.h"
#include "gpk_array_static.h"

#ifndef GPK_VOXEL_H_098273498237423
#define GPK_VOXEL_H_098273498237423

namespace gpk
{
#pragma pack(push, 1)
	constexpr ::gpk::array_static<::gpk::bgra, 256>	VOXEL_PALETTE = {
		0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff, 0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff,
		0xff6699ff, 0xff3399ff, 0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff, 0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff,
		0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff, 0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
		0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc, 0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc,
		0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc, 0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99,
		0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999, 0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
		0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099, 0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66,
		0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66, 0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966, 0xff009966, 0xffff6666,
		0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
		0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33, 0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933,
		0xff669933, 0xff339933, 0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633, 0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033,
		0xffcc0033, 0xff990033, 0xff660033, 0xff330033, 0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
		0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900, 0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300, 0xff993300,
		0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000, 0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044,
		0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400, 0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
		0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777, 0xff555555, 0xff444444, 0xff222222, 0xff111111
	};

	stacxpr	::gpk::n3<float>				VOXEL_VERTICES		[8]		= 
			{ {0, 1, 0}, {1, 1, 0}, {0, 1, 1}, {1, 1, 1} // Top
			, {0, 0, 0}, {1, 0, 0}, {0, 0, 1}, {1, 0, 1} // Bottom
			}; 
														
	GDEFINE_ENUM_TYPE(VOXEL_FACE, uint8_t);
	GDEFINE_ENUM_VALUE(VOXEL_FACE, Bottom	, 0);
	GDEFINE_ENUM_VALUE(VOXEL_FACE, Back		, 1);
	GDEFINE_ENUM_VALUE(VOXEL_FACE, Left		, 2);
	GDEFINE_ENUM_VALUE(VOXEL_FACE, Top		, 3);
	GDEFINE_ENUM_VALUE(VOXEL_FACE, Front	, 4);
	GDEFINE_ENUM_VALUE(VOXEL_FACE, Right	, 5);

	struct SVoxelFaceBits {
		bool		 Bottom			: 1;
		bool		 Back			: 1;
		bool		 Left			: 1;
		bool		 Top			: 1;
		bool		 Front			: 1;
		bool		 Right			: 1;
	};

	stacxpr	::gpk::SQuad3<float>				VOXEL_FACE_VERTICES	[6]		= 
		{ {{0, 1, 0}, {1, 1, 0}, {0, 1, 1}, {1, 1, 1}}  // Top		(y = 1)
		, {{1, 0, 0}, {1, 1, 0}, {1, 0, 1}, {1, 1, 1}}	// Front	(x = 1)
		, {{0, 0, 1}, {1, 0, 1}, {0, 1, 1}, {1, 1, 1}}	// Right	(z = 1)
		, {{0, 0, 0}, {1, 0, 0}, {0, 0, 1}, {1, 0, 1}}  // Bottom	(y = 0)
		, {{0, 0, 0}, {0, 1, 0}, {0, 0, 1}, {0, 1, 1}}	// Back		(x = 0)
		, {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}}	// Left		(z = 0)
		}; 

	stacxpr	::gpk::SQuad2<float>				VOXEL_FACE_UV	[6]		= 
		{ {{0, 0}, {1, 0}, {0, 1}, {1, 1}}	// Top		
		, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}	// Front	
		, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}	// Right	
		, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}	// Bottom	
		, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}	// Back		
		, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}	// Left		
		};

	stacxpr	::gpk::SQuad3<float>				VOXEL_FACE_NORMALS	[6]		= 
		{ {{0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}}	// Top		
		, {{1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}}	// Front	
		, {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}}	// Right	
		, {{0, -1, 0}, {0, -1, 0}, {0, -1, 0}, {0, -1, 0}}	// Bottom	
		, {{-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}}	// Back		
		, {{0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}}	// Left		
		};

	stacxpr	::gpk::n3f32				VOXEL_NORMALS				[6]		= {{0, 1, 0}, {1, 0, 0}, {0, 0, 1}, {0, -1, 0}, {-1, 0, 0}, {0, 0, -1}};	// top, front, right, bottom, back, left
	stacxpr	::gpk::SQuad2<uint8_t>				VOXEL_FACE_NORMAL_INDICES	[6]		= 
		{ {0, 0, 0, 0}	// Top		
		, {1, 1, 1, 1}	// Front	
		, {2, 2, 2, 2}	// Right	
		, {3, 3, 3, 3}	// Bottom	
		, {4, 4, 4, 4}	// Back		
		, {5, 5, 5, 5}	// Left		
		};

	stacxpr	::gpk::n3i8				VOXEL_DELTAS				[6]		= 
		{ { 0, 1, 0} // Top		(y = 1)
		, { 1, 0, 0} // Front	(x = 1)
		, { 0, 0, 1} // Right	(z = 1)
		, { 0,-1, 0} // Bottom	(y = -1)
		, {-1, 0, 0} // Back	(x = -1)
		, { 0, 0,-1} // Left	(z = -1)
		};

	stacxpr	uint8_t							VOXEL_FACE_INDICES		[6][6]	= 
		{ {0, 2, 1, 1, 2, 3} // Top
		, {0, 1, 2, 1, 3, 2}
		, {0, 1, 2, 1, 3, 2}
		, {0, 1, 2, 1, 3, 2} // Bottom
		, {0, 2, 1, 1, 2, 3}
		, {0, 2, 1, 1, 2, 3}
		}; 

	stacxpr	uint16_t							VOXEL_FACE_INDICES_16	[6][6]	= 
		{ {4 * 3 + 0, 4 * 3 + 1, 4 * 3 + 2, 4 * 3 + 1, 4 * 3 + 3, 4 * 3 + 2} // Bottom
		, {4 * 4 + 0, 4 * 4 + 2, 4 * 4 + 1, 4 * 4 + 1, 4 * 4 + 2, 4 * 4 + 3}
		, {4 * 5 + 0, 4 * 5 + 2, 4 * 5 + 1, 4 * 5 + 1, 4 * 5 + 2, 4 * 5 + 3}
		, {4 * 0 + 0, 4 * 0 + 2, 4 * 0 + 1, 4 * 0 + 1, 4 * 0 + 2, 4 * 0 + 3} // Top
		, {4 * 1 + 0, 4 * 1 + 1, 4 * 1 + 2, 4 * 1 + 1, 4 * 1 + 3, 4 * 1 + 2}
		, {4 * 2 + 0, 4 * 2 + 1, 4 * 2 + 2, 4 * 2 + 1, 4 * 2 + 3, 4 * 2 + 2}
		, 
		}; 

	stacxpr	float								VOXEL_UV_SCALE				= 1.0 / 16; // 1/16 is the standard for Minecraft texture mapping format

	template<typename T, uint16_t _width = 16>
	struct SVoxelLayer {
		stacxpr	uint32_t					WIDTH						= _width;
		stacxpr	uint32_t					DEPTH						= WIDTH;

		uint16_t										Y							= {};
		T											Cells	[DEPTH][WIDTH]		= {};
	};

	template <typename _uIndex>
	struct SVoxel {
		::gpk::n3<_uIndex>							Position;
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

	template<typename T>
	struct SVoxelChunk {
		::gpk::apobj<::gpk::SVoxelLayer<T>>	Layers						= {};
	};

	template<typename T>
	struct SVoxelMap {
		::gpk::n3u8							Dimensions;
		::gpk::apod<::gpk::bgra>			Palette;
		::gpk::apod<::gpk::SVoxel<uint8_t>>	Voxels;
		::gpk::aobj<::gpk::SVoxelChunk<T>>	Chunks;
		::gpk::apod<::gpk::n2u8>			ChunkPositions;

		int32_t										GetChunk					(const ::gpk::n2u8 & chunkCoords) const	{ 
			return ::gpk::find(chunkCoords, ::gpk::view<const ::gpk::n2u8>{ChunkPositions}); 
		}

		int32_t											GetChunkForVoxel			(const ::gpk::n3u8 & voxelPosition) const	{ 
			const ::gpk::n3u8						floor						= voxelPosition / SVoxelLayer<T>::WIDTH; 
			return GetChunk({floor.x, floor.z}); 
		}

		int32_t											SetValue					(const ::gpk::n3u8 & voxelPosition, const T & cellValue) 	{ 
			const ::gpk::n3u8						floor						= voxelPosition / SVoxelLayer<T>::WIDTH; 
			int32_t												indexChunk					= GetChunk({floor.x, floor.z});
			if(0 > indexChunk) {
				indexChunk										= Chunks.push_back({});
				ChunkPositions.push_back({floor.x, floor.z});
			}

			::gpk::SVoxelChunk<T>							& chunk						= Chunks[indexChunk];
			if(chunk.Layers.size() < uint32_t(voxelPosition.y) + 1)
				chunk.Layers.resize(voxelPosition.y + 1);

			T												& cell						= chunk.Layers[voxelPosition.y]
				->Cells[voxelPosition.z % SVoxelLayer<T>::DEPTH][voxelPosition.x % SVoxelLayer<T>::WIDTH];
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

		int32_t											GetValue					(const ::gpk::n3u8 & voxelPosition, T & cellValue) const	{ 
			const ::gpk::n3u8						floor						= voxelPosition / SVoxelLayer<T>::WIDTH; 
			int32_t												indexChunk					= GetChunk({floor.x, floor.z});
			if(0 > indexChunk) 
				return -1;

			const ::gpk::SVoxelChunk<T>				 	& chunk						= Chunks[indexChunk];
			if(voxelPosition.y >= chunk.Layers.size()) 
				return -1;

			const ::gpk::pobj<::gpk::SVoxelLayer<T>>	& layer						= chunk.Layers[voxelPosition.y];
			if(0 == layer.get_ref())
				return -1;

			cellValue										= layer->Cells[voxelPosition.z % SVoxelLayer<T>::DEPTH][voxelPosition.x % SVoxelLayer<T>::WIDTH];
			return 0;
		}
	};
} // namespace

#endif // GPK_VOXEL_H_098273498237423