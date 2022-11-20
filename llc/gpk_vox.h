#include "gpk_array_static.h"
#include "gpk_storage.h"
#include "gpk_parse.h"
#include "gpk_color.h"

#ifndef GPK_VOX_H_9832347982
#define GPK_VOX_H_9832347982

namespace gpk 
{
#define gpk_vox_info_printf info_printf
	::gpk::array_static<::gpk::SColorBGRA, 256> VOX_PALETTE_DEFAULT = {
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

	GDEFINE_ENUM_TYPE(VOX_TAG, uint8_t);
	GDEFINE_ENUM_VALUE(VOX_TAG, MAIN,  0);
	GDEFINE_ENUM_VALUE(VOX_TAG, SIZE,  1);
	GDEFINE_ENUM_VALUE(VOX_TAG, PACK,  2);
	GDEFINE_ENUM_VALUE(VOX_TAG, XYZI,  3);
	GDEFINE_ENUM_VALUE(VOX_TAG, RGBA,  4);
	GDEFINE_ENUM_VALUE(VOX_TAG, nTRN,  5);
	GDEFINE_ENUM_VALUE(VOX_TAG, nGRP,  6);
	GDEFINE_ENUM_VALUE(VOX_TAG, nSHP,  7);
	GDEFINE_ENUM_VALUE(VOX_TAG, MATL,  8);
	GDEFINE_ENUM_VALUE(VOX_TAG, LAYR,  9);
	GDEFINE_ENUM_VALUE(VOX_TAG, rOBJ, 10);
	GDEFINE_ENUM_VALUE(VOX_TAG, rCAM, 11);
	GDEFINE_ENUM_VALUE(VOX_TAG, NOTE, 12);
	GDEFINE_ENUM_VALUE(VOX_TAG, IMAP, 13);

	// 2. Chunk Structure
	// -------------------------------------------------------------------------------
	// # Bytes  | Type       | Value
	// -------------------------------------------------------------------------------
	// 1x4      | char       | chunk id
	// 4        | int        | num bytes of chunk content (N)
	// 4        | int        | num bytes of children chunks (M)
	// N        |            | chunk content
	// M        |            | children chunk

#pragma pack(push, 1)
	struct SVOXFileHeader {
		::gpk::array_static<char, 4>						MagicNumber				= {};
		uint32_t											Version					= {};
	};

	struct SVOXChunkHeader {
		::gpk::array_static<char, 4>						Type					= {};
		uint32_t											ChunkSize				= 0;	// | num bytes of chunk content (N)
		uint32_t											ChildChunks				= 0;	// | num bytes of children chunks (M)

		inline	bool										IsType					(::gpk::vcs riff)	const	{
			return riff == Type;
		} 
	};

	struct SVOXTransformFrame { // DICT	: frame attributes
		uint8_t												Rotation				= {};
		::gpk::SCoord3<int32_t>								Translation				= {};
		uint32_t											IndexFrame				= {};
	}; 

	struct SVOXVoxel {
		uint8_t												x;
		uint8_t												y;
		uint8_t												z;
		uint8_t												i;
	};

#pragma pack(pop)

	struct SVOXChunk {
		SVOXChunkHeader										Header					= {};
		::gpk::array_pod<byte_t>							Data					= {};
	};

	struct SVOXChunkXYZI {
		SVOXChunkHeader										Header					= {};
		::gpk::array_pod<SVOXVoxel>							Data					= {};
	};

	typedef ::gpk::array_obj<::gpk::SKeyVal<::gpk::array_pod<char>, ::gpk::array_pod<char>>>	SVOXDict;

	struct SVOXChunkTransform {
		SVOXChunkHeader										Header					= {};
		int32_t												Id						= 0;
		SVOXDict											Attributes				= {};
		int32_t 											IdChild					= 0;
		int32_t 											IdReserved				= -1; // (must be -1)
		int32_t												IdLayer					= -1;
		::gpk::array_pod<::gpk::SVOXTransformFrame>			Frames					= {};
	};

	struct SVOXChunkNode {
		SVOXChunkHeader										Header					= {};
		int32_t												Id						= 0;
		SVOXDict											Attributes				= {};
		::gpk::array_pod<int32_t>							IdChildren				= {};
	};

	struct SVOXChunkMaterial {
		SVOXChunkHeader										Header					= {};
		int32_t												Id						= 0;
		SVOXDict											Attributes				= {};
	};

	struct SVOXChunkRenderAttributes {
		SVOXChunkHeader										Header					= {};
		SVOXDict											Attributes				= {};
	};

	struct SVOXChunkCamera {
		SVOXChunkHeader										Header					= {};
		int32_t												Id						= 0;
		SVOXDict											Attributes				= {};
	};

	struct SVOXShapeModel {
		int32_t												Id						= 0;
		SVOXDict											Attributes				= {};
	};

	struct SVOXChunkShape {
		SVOXChunkHeader										Header					= {};
		int32_t												Id						= 0;
		SVOXDict											Attributes				= {};
		::gpk::array_obj<::gpk::SVOXShapeModel>				Models					= {};
	};

	struct SVOXChunkLayer {
		SVOXChunkHeader										Header					= {};
		int32_t												Id						= 0;
		SVOXDict											Attributes				= {};
		int32_t												IdReserved				= 0;
	};

	struct SVOXChunkNote {
		SVOXChunkHeader										Header					= {};
		::gpk::array_obj<::gpk::array_pod<char>>			ColorNames				= {};
	};

	::gpk::error_t										voxDictLoad				(::gpk::SVOXDict & attributes, ::gpk::view_array<const byte_t> & input) 						{
		uint32_t												bytesRead				= sizeof(uint32_t); 
		const uint32_t											dictLength				= *(uint32_t*)input.begin(); 
		input												= {input.begin() + bytesRead, input.size() - bytesRead}; 
		for(uint32_t iLength = 0; iLength < dictLength; ++iLength) {
			::gpk::view_array<const char>							readKey					= {};
			::gpk::view_array<const char>							readVal					= {};
			bytesRead = ::gpk::viewRead(readKey, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
			bytesRead = ::gpk::viewRead(readVal, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
			attributes.push_back({readKey, readVal});
		}
		return 0;
	}

	
	struct SVOXData {
		SVOXFileHeader										Header					= {};
		::gpk::array_obj<::gpk::SVOXChunk>					Chunks					= {};
		::gpk::array_obj<::gpk::SVOXChunkXYZI>				ChunksCoord				= {};
		::gpk::array_obj<::gpk::SVOXChunkTransform>			ChunksTransform			= {};
		::gpk::array_obj<::gpk::SVOXChunkNode>				ChunksNode				= {};
		::gpk::array_obj<::gpk::SVOXChunkMaterial>			ChunksMaterial			= {};
		::gpk::array_obj<::gpk::SVOXChunkRenderAttributes>	ChunksRenderAttributes	= {};
		::gpk::array_obj<::gpk::SVOXChunkCamera>			ChunksCamera			= {};
		::gpk::array_obj<::gpk::SVOXChunkLayer>				ChunksLayer				= {};
		::gpk::array_obj<::gpk::SVOXChunkShape>				ChunksShape				= {};
		::gpk::array_obj<::gpk::SVOXChunkNote>				ChunksNote				= {};

		::gpk::SCoord3<uint8_t>								GetDimensions			()	const	{
			for(uint32_t iChunk = 0; iChunk < Chunks.size(); ++iChunk)
				if(0 == memcmp(Chunks[iChunk].Header.Type.Storage, "SIZE", 4)) {
					const ::gpk::SCoord3<uint32_t> coord = *(::gpk::SCoord3<uint32_t>*)Chunks[iChunk].Data.begin();
					return ::gpk::SCoord3<uint32_t>{coord.y, coord.z, coord.x}.Cast<uint8_t>();
				}

			return {};
		}

		::gpk::view_array<const ::gpk::SVOXVoxel>					GetXYZI					()	const	{
			for(uint32_t iChunk = 0; iChunk < ChunksCoord.size(); ++iChunk)
				if(0 == memcmp(ChunksCoord[iChunk].Header.Type.Storage, "XYZI", 4)) {
					return {ChunksCoord[iChunk].Data.begin(), ChunksCoord[iChunk].Data.size()};
				}

			return {};
		}
		
		::gpk::view_array<const ::gpk::SColorBGRA>					GetBGRA					()	const	{
			for(uint32_t iChunk = 0; iChunk < Chunks.size(); ++iChunk)
				if(0 == memcmp(Chunks[iChunk].Header.Type.Storage, "RGBA", 4)) {
					return {(const ::gpk::SColorBGRA*)Chunks[iChunk].Data.begin(), Chunks[iChunk].Data.size() / 4};
				}

			return {};
		}
		
		::gpk::error_t										Load					(::gpk::view_array<const byte_t> & input) { 
			uint32_t												bytesRead				= sizeof(SVOXFileHeader); 
			Header												= *(const SVOXFileHeader*)input.begin(); 
			input												= {input.begin() + bytesRead, input.size() - bytesRead}; 

			while(input.size() >= 4) {
				bytesRead											= sizeof(::gpk::SVOXChunkHeader); 
				const ::gpk::SVOXChunkHeader							& readChunkHeader	= *(const ::gpk::SVOXChunkHeader*)input.begin(); 
				input												= {input.begin() + bytesRead, input.size() - bytesRead};

				const ::gpk::vcs										CHUNK_TAGS_STATIC[]	= {"MAIN", "SIZE", "PACK", "RGBA"};
				gpk_vox_info_printf("Chunk type: %s.", gpk::toString(readChunkHeader.Type).begin());
				if(-1 != ::gpk::find(::gpk::vcs{readChunkHeader.Type}, ::gpk::view_array<const ::gpk::vcs>{CHUNK_TAGS_STATIC})) {
					::gpk::SVOXChunk										newChunk				= {};
					newChunk.Header										= readChunkHeader;

						 if(0 == strncmp(newChunk.Header.Type.Storage, "MAIN", 4)) { }
					else if(0 == strncmp(newChunk.Header.Type.Storage, "SIZE", 4)) { bytesRead = sizeof(::gpk::SCoord3<uint32_t>)	; newChunk.Data.append({input.begin(), bytesRead}); input = {input.begin() + bytesRead, input.size() - bytesRead}; }
					else if(0 == strncmp(newChunk.Header.Type.Storage, "PACK", 4)) { bytesRead = sizeof(uint32_t)					; newChunk.Data.append({input.begin(), bytesRead}); input = {input.begin() + bytesRead, input.size() - bytesRead}; }
					else if(0 == strncmp(newChunk.Header.Type.Storage, "RGBA", 4)) { 
						bytesRead = sizeof(uint32_t) * 256; 
						newChunk.Data.append({input.begin(), bytesRead}); 
						input = {input.begin() + bytesRead, input.size() - bytesRead}; 
						::gpk::view_array<::gpk::SColorBGRA>	bgraView = {(::gpk::SColorBGRA*)newChunk.Data.begin(), 256};
						for(uint32_t iColor = 0; iColor < bgraView.size(); ++iColor) {
							::gpk::SColorBGRA									& bgra				= bgraView[iColor];
							std::swap(bgra.r, bgra.b);
						}

					}

					Chunks.push_back(newChunk);
				}
				else {
					if(0 == strncmp(readChunkHeader.Type.Storage, "rOBJ", 4)) { 
						::gpk::SVOXChunkRenderAttributes			newChunk			= {}; 
		 				newChunk.Header							= readChunkHeader;
						::gpk::voxDictLoad(newChunk.Attributes, input);
						ChunksRenderAttributes.push_back(newChunk);
					}
					else if(0 == strncmp(readChunkHeader.Type.Storage, "MATL", 4)) { 
						::gpk::SVOXChunkMaterial					newChunk			= {}; 
		 				newChunk.Header							= readChunkHeader;
						bytesRead = sizeof(int32_t	); newChunk.Id = *(int32_t*)input.begin(); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
						::gpk::voxDictLoad(newChunk.Attributes, input);
						ChunksMaterial.push_back(newChunk);
					}
					else if(0 == strncmp(readChunkHeader.Type.Storage, "rCAM", 4)) { 
						::gpk::SVOXChunkCamera						newChunk			= {}; 
		 				newChunk.Header							= readChunkHeader;
						bytesRead = sizeof(int32_t	); newChunk.Id = *(int32_t*)input.begin(); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
						::gpk::voxDictLoad(newChunk.Attributes, input);
						ChunksCamera.push_back(newChunk);
					}
					else if(0 == strncmp(readChunkHeader.Type.Storage, "LAYR", 4)) { 
						::gpk::SVOXChunkLayer						newChunk			= {}; 
		 				newChunk.Header							= readChunkHeader;
						bytesRead = sizeof(int32_t	); newChunk.Id = *(int32_t*)input.begin(); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
						::gpk::voxDictLoad(newChunk.Attributes, input);
						bytesRead = sizeof(int32_t	); newChunk.IdReserved = *(int32_t*)input.begin(); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
						ChunksLayer.push_back(newChunk);
					}
					else if(0 == strncmp(readChunkHeader.Type.Storage, "XYZI", 4)) { 
						::gpk::SVOXChunkXYZI								newChunk			= {}; 
		 				newChunk.Header									= readChunkHeader;
						::gpk::view_array<const SVOXVoxel>					readChunkData		= {};
						bytesRead										= ::gpk::viewRead(readChunkData, input); 
						input											= {input.begin() + bytesRead, input.size() - bytesRead}; 
						newChunk.Data									= readChunkData;
						for(uint32_t iVoxel = 0; iVoxel < newChunk.Data.size(); ++iVoxel) {
							::gpk::SVOXVoxel									& voxel				= newChunk.Data[iVoxel];
							voxel											= {voxel.y, voxel.z, voxel.x, voxel.i};
						}
						ChunksCoord.push_back(newChunk);
					}
					else if(0 == strncmp(readChunkHeader.Type.Storage, "NOTE", 4)) { 
						::gpk::SVOXChunkNote							newChunk			= {}; 
		 				newChunk.Header								= readChunkHeader;
						bytesRead = sizeof(uint32_t	); const uint32_t countFrames = *(uint32_t*)input.begin(); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
						for(uint32_t iFrame = 0; iFrame < countFrames; ++iFrame) {
							::gpk::view_array<const char>					readVal		= {};
							bytesRead = ::gpk::viewRead(readVal, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
							newChunk.ColorNames.push_back(readVal);
						}
						ChunksNote.push_back(newChunk);
					}
					else if(0 == strncmp(readChunkHeader.Type.Storage, "nGRP", 4)) { 
						::gpk::SVOXChunkNode						newChunk			= {}; 
		 				newChunk.Header							= readChunkHeader;

						bytesRead = sizeof(int32_t	); newChunk.Id = *(int32_t*)input.begin(); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
						::gpk::voxDictLoad(newChunk.Attributes, input);

						::gpk::view_array<const int32_t>			readChildren		= {};
						bytesRead								= ::gpk::viewRead(readChildren, input); 
						input									= {input.begin() + bytesRead, input.size() - bytesRead}; 
						newChunk.IdChildren						= readChildren;
						ChunksNode.push_back(newChunk);
					}
					else if(0 == strncmp(readChunkHeader.Type.Storage, "nSHP", 4)) { 
						::gpk::SVOXChunkShape						newChunk			= {}; 
		 				newChunk.Header							= readChunkHeader;
						bytesRead = sizeof(int32_t	); newChunk.Id = *(int32_t*)input.begin(); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
						::gpk::voxDictLoad(newChunk.Attributes, input);
						bytesRead = sizeof(uint32_t	); const uint32_t countModels = *(uint32_t*)input.begin(); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
						for(uint32_t iFrame = 0; iFrame < countModels; ++iFrame) {
							::gpk::SVOXShapeModel						newModel	= {};
							bytesRead = sizeof(int32_t	); newModel.Id = *(int32_t*)input.begin(); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
							::gpk::voxDictLoad(newModel.Attributes, input);
							newChunk.Models.push_back(newModel);
						}
						ChunksShape.push_back(newChunk);
					}
					else if(0 == strncmp(readChunkHeader.Type.Storage, "nTRN", 4)) { 
						::gpk::SVOXChunkTransform							newChunk			= {}; 
		 				newChunk.Header									= readChunkHeader;

						bytesRead = sizeof(int32_t	); newChunk.Id = *(int32_t*)input.begin(); input = {input.begin() + bytesRead, input.size() - bytesRead}; 

						::gpk::voxDictLoad(newChunk.Attributes, input);
						bytesRead = sizeof(int32_t 	); newChunk.IdChild		= *(int32_t *)input.begin(); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
						bytesRead = sizeof(int32_t 	); newChunk.IdReserved	= *(int32_t *)input.begin(); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
						bytesRead = sizeof(int32_t	); newChunk.IdLayer		= *(int32_t	*)input.begin(); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
						bytesRead = sizeof(uint32_t	); const uint32_t countFrames = *(uint32_t*)input.begin(); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
						for(uint32_t iFrame = 0; iFrame < countFrames; ++iFrame) {
							::gpk::SVOXTransformFrame							newFrame	= {};
							bytesRead = sizeof(uint32_t	); const uint32_t dictLength = *(uint32_t*)input.begin(); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
							for(uint32_t iLength = 0; iLength < dictLength; ++iLength) {
								::gpk::view_array<const char>						readKey		= {};
								::gpk::view_array<const char>						readVal		= {};
								bytesRead = ::gpk::viewRead(readKey, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; 
								bytesRead = ::gpk::viewRead(readVal, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; 

								// Welcome to the amazing fucked up way to store a transform in a binary file
									 if(readKey == ::gpk::vcs{"_r"}) { newFrame.Rotation = readVal[0]; }
								else if(readKey == ::gpk::vcs{"_t"}) {
									{ int32_t value = 0; uint32_t charsProcessed = ::gpk::parseIntegerDecimal(readVal, &value) + 1; readVal = {readVal.begin() + charsProcessed, readVal.size() - charsProcessed}; newFrame.Translation.x = value; }
									{ int32_t value = 0; uint32_t charsProcessed = ::gpk::parseIntegerDecimal(readVal, &value) + 1; readVal = {readVal.begin() + charsProcessed, readVal.size() - charsProcessed}; newFrame.Translation.z = value; }
									{ int32_t value = 0; uint32_t charsProcessed = ::gpk::parseIntegerDecimal(readVal, &value) + 1; readVal = {readVal.begin() + charsProcessed, readVal.size() - charsProcessed}; newFrame.Translation.y = value; }
								}
								else if(readKey == ::gpk::vcs{"_f"}) { newFrame.IndexFrame	= *(const int32_t*)readVal.begin(); }; 
							}
							newChunk.Frames.push_back(newFrame);
						}
						ChunksTransform.push_back(newChunk);

					}
					//else if(0 == strncmp(readChunkHeader.Type.Storage, "IMAP", 4)) { bytesRead = 4; newChunk.Data.append({input.begin(), bytesRead}); input = {input.begin() + bytesRead, input.size() - bytesRead}; }
					else {
						warning_printf("Unknown chunk type: %s.", gpk::toString(readChunkHeader.Type).begin());
						input									= {input.begin() + readChunkHeader.ChunkSize, input.size() - readChunkHeader.ChunkSize}; 
					}
				}
			}

			return 0;
		}
	};

} // namespace

#endif // GPK_VOX_H_9832347982
