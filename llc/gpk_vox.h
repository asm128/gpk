
#include "gpk_swap.h"

#include "gpk_array_static.h"
#include "gpk_file.h"
#include "gpk_parse.h"
#include "gpk_color.h"
#include "gpk_string.h"

#include "gpk_n3.h"

#ifndef GPK_VOX_H_23627
#define GPK_VOX_H_23627

namespace gpk
{
#define gpk_vox_info_printf info_printf
	stxp astatic<bgra, 256> VOX_PALETTE_DEFAULT = {
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

	GDEFINE_ENUM_TYPE(VOX_TAG, u0_t);
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
	// 1x4      | sc_t       | chunk id
	// 4        | int        | num bytes of chunk content (N)
	// 4        | int        | num bytes of children chunks (M)
	// N        |            | chunk content
	// M        |            | children chunk

#pragma pack(push, 1)
	struct SVOXFileHeader {
		astatic<sc_t, 4>			MagicNumber				= {};
		u2_t						Version					= {};
	};

	struct SVOXChunkHeader {
		astatic<sc_t, 4>			Type					= {};
		u2_t						ChunkSize				= 0;	// | num bytes of chunk content (N)
		u2_t						ChildChunks				= 0;	// | num bytes of children chunks (M)

		inline	bool				IsType					(vcst_t riff)	const	{
			return riff == Type;
		}
	};

	struct SVOXTransformFrame { // DICT	: frame attributes
		u0_t						Rotation				= {};
		n3s2_t						Translation				= {};
		u2_t						IndexFrame				= {};
	};

	struct SVOXVoxel {
		u0_t						x;
		u0_t						y;
		u0_t						z;
		u0_t						i;
	};

#pragma pack(pop)

	struct SVOXChunk {
		SVOXChunkHeader				Header					= {};
		au0_t						Data					= {};
	};

	struct SVOXChunkXYZI {
		SVOXChunkHeader				Header					= {};
		apod<SVOXVoxel>				Data					= {};
	};

	typedef aobj<kv<string>>		SVOXDict;

	struct SVOXChunkTransform {
		SVOXChunkHeader				Header					= {};
		s2_t						Id						= 0;
		SVOXDict					Attributes				= {};
		s2_t 						IdChild					= 0;
		s2_t 						IdReserved				= -1; // (must be -1)
		s2_t						IdLayer					= -1;
		apod<SVOXTransformFrame>	Frames	= {};
	};

	struct SVOXChunkNode {
		SVOXChunkHeader				Header					= {};
		s2_t						Id						= 0;
		SVOXDict					Attributes				= {};
		as2_t						IdChildren				= {};
	};

	struct SVOXChunkMaterial {
		SVOXChunkHeader				Header					= {};
		s2_t						Id						= 0;
		SVOXDict					Attributes				= {};
	};

	struct SVOXChunkRenderAttributes {
		SVOXChunkHeader				Header					= {};
		SVOXDict					Attributes				= {};
	};

	struct SVOXChunkCamera {
		SVOXChunkHeader				Header					= {};
		s2_t						Id						= 0;
		SVOXDict					Attributes				= {};
	};

	struct SVOXShapeModel {
		s2_t						Id						= 0;
		SVOXDict					Attributes				= {};
	};

	struct SVOXChunkShape {
		SVOXChunkHeader				Header					= {};
		s2_t						Id						= 0;
		SVOXDict					Attributes				= {};
		aobj<SVOXShapeModel>		Models					= {};
	};

	struct SVOXChunkLayer {
		SVOXChunkHeader				Header					= {};
		s2_t						Id						= 0;
		SVOXDict					Attributes				= {};
		s2_t						IdReserved				= 0;
	};

	struct SVOXChunkNote {
		SVOXChunkHeader				Header					= {};
		aobj<string>				ColorNames				= {};
	};

	tplTstin err_t	voxViewLoadLegacy		(vcu0_t & input, view<cnst T> & output) {
		err_t					bytesRead				= 0;
		if_fail_fe(bytesRead = viewPascal(output, input));
		if_fail_fe(input.slice(input, bytesRead));
		rtrn bytesRead;
	}

	stin err_t		voxUnsignedLoad			(vcst_t input, u2_t & output) {
		if_zero_fe(input.size());
		u3_t					value					= 0;
		err_t					digits					= 0;
		if_fail_fe(digits = parseIntegerDecimal(input, value));
		if_true_fe((u2_t)digits != input.size() || value > 0xFFFFFFFFULL);
		output					= (u2_t)value;
		rtrn digits;
	}

	stin err_t		voxSignedLoad			(vcst_t input, s2_t & output) {
		if_zero_fe(input.size());
		cnst bool				negative				= '-' == input[0];
		vcst_t					magnitude				= {};
		if_fail_fe(input.slice(magnitude, negative ? 1U : 0U));
		u2_t					value					= 0;
		if_fail_fe(voxUnsignedLoad(magnitude, value));
		if_true_fe(value > (negative ? 0x80000000U : 0x7FFFFFFFU));
		output					= negative ? (0x80000000U == value ? (-0x7FFFFFFF - 1) : -(s2_t)value) : (s2_t)value;
		rtrn input.size();
	}

	stin err_t		voxTranslationLoad		(vcst_t input, n3s2_t & output) {
		n3s2_t					loaded					= {};
		u2_t					offset					= 0;
		for(u2_t iValue = 0; iValue < 3; ++iValue) {
			while(offset < input.size() && ' ' == input[offset])
				++offset;
			u2_c					start					= offset;
			while(offset < input.size() && ' ' != input[offset])
				++offset;
			if_true_fe(start == offset);
			vcst_t					token					= {};
			if_fail_fe(input.slice(token, start, offset - start));
			s2_t					value					= 0;
			if_fail_fe(voxSignedLoad(token, value));
			if(0 == iValue) loaded.x = value;
			else if(1 == iValue) loaded.z = value;
			else loaded.y = value;
		}
		while(offset < input.size() && ' ' == input[offset])
			++offset;
		if_true_fe(offset != input.size());
		output					= loaded;
		rtrn offset;
	}

	stin err_t		voxDictLoad				(SVOXDict & attributes, vcu0_t & input) {
		SVOXDict					loaded					= {};
		u2_t					dictLength				= 0;
		if_fail_fe(loadPOD(input, dictLength));
		for(u2_t iLength = 0; iLength < dictLength; ++iLength) {
			vcst_t					readKey					= {};
			vcst_t					readVal					= {};
			if_fail_fe(voxViewLoadLegacy(input, readKey));
			if_fail_fe(voxViewLoadLegacy(input, readVal));
			if_fail_fe(loaded.push_back({string{readKey}, string{readVal}}));
		}
		attributes				= loaded;
		rtrn 0;
	}


	struct SVOXData {
		SVOXFileHeader					Header					= {};
		aobj<SVOXChunk>					Chunks					= {};
		aobj<SVOXChunkXYZI>				ChunksCoord				= {};
		aobj<SVOXChunkTransform>		ChunksTransform			= {};
		aobj<SVOXChunkNode>				ChunksNode				= {};
		aobj<SVOXChunkMaterial>			ChunksMaterial			= {};
		aobj<SVOXChunkRenderAttributes>	ChunksRenderAttributes	= {};
		aobj<SVOXChunkCamera>			ChunksCamera			= {};
		aobj<SVOXChunkLayer>			ChunksLayer				= {};
		aobj<SVOXChunkShape>			ChunksShape				= {};
		aobj<SVOXChunkNote>				ChunksNote				= {};

		n3u0_t				GetDimensions			()	const {
			for(u2_t iChunk = 0; iChunk < Chunks.size(); ++iChunk)
				if(Chunks[iChunk].Header.IsType("SIZE") && Chunks[iChunk].Data.size() == szof(n3u2_t)) {
					n3u2_t					coord					= {};
					memcpy(&coord, Chunks[iChunk].Data.begin(), szof(coord));
					rtrn n3u2_t{coord.y, coord.z, coord.x}.u0();
				}
			rtrn {};
		}

		view<cnst SVOXVoxel>	GetXYZI				()	const {
			for(u2_t iChunk = 0; iChunk < ChunksCoord.size(); ++iChunk)
				if(ChunksCoord[iChunk].Header.IsType("XYZI"))
					rtrn ChunksCoord[iChunk].Data;
			rtrn {};
		}

		view<cnst bgra>			GetBGRA				()	const {
			for(u2_t iChunk = 0; iChunk < Chunks.size(); ++iChunk)
				if(Chunks[iChunk].Header.IsType("RGBA") && 0 == Chunks[iChunk].Data.size() % szof(bgra))
					rtrn {reinterpret_cast<cnst bgra*>(Chunks[iChunk].Data.begin()), Chunks[iChunk].Data.size() / szof(bgra)};
			rtrn VOX_PALETTE_DEFAULT;
		}

		err_t					Load					(vcu0_t & input) {
			vcu0_t					remaining				= input;
			SVOXData					loaded					= {};
			if_fail_fe(loadPOD(remaining, loaded.Header));
			if_true_fe(vcst_t{loaded.Header.MagicNumber} != GPK_CXS("VOX "));
			bool						mainFound				= false;
			while(remaining.size()) {
				SVOXChunkHeader			readChunkHeader			= {};
				if_fail_fe(loadPOD(remaining, readChunkHeader));
				if_true_fe(readChunkHeader.ChunkSize > remaining.size());
				if_true_fe(readChunkHeader.ChildChunks > remaining.size() - readChunkHeader.ChunkSize);
				vcu0_t					content					= {};
				if_fail_fe(remaining.slice(content, 0, readChunkHeader.ChunkSize));
				if_fail_fe(remaining.slice(remaining, readChunkHeader.ChunkSize));

				if(readChunkHeader.IsType("MAIN")) {
					if_true_fe(mainFound || loaded.Chunks.size() || readChunkHeader.ChunkSize || readChunkHeader.ChildChunks != remaining.size());
					SVOXChunk					newChunk				= {readChunkHeader};
					if_fail_fe(loaded.Chunks.push_back(newChunk));
					mainFound				= true;
					continue;
				}
				if_true_fe(false == mainFound);

				if(readChunkHeader.IsType("SIZE") || readChunkHeader.IsType("PACK") || readChunkHeader.IsType("RGBA")) {
					u2_c					expectedSize			= readChunkHeader.IsType("SIZE") ? szof(n3u2_t) : readChunkHeader.IsType("PACK") ? szof(u2_t) : szof(bgra) * 256U;
					if_true_fe(content.size() != expectedSize);
					SVOXChunk					newChunk				= {readChunkHeader};
					if_fail_fe(newChunk.Data.append(content));
					if(readChunkHeader.IsType("RGBA")) {
						view<bgra>					colors					= {reinterpret_cast<bgra*>(newChunk.Data.begin()), 256};
						for(u2_t iColor = 0; iColor < colors.size(); ++iColor)
							swap(colors[iColor].r, colors[iColor].b);
					}
					if_fail_fe(loaded.Chunks.push_back(newChunk));
					continue;
				}

				if(readChunkHeader.IsType("XYZI")) {
					SVOXChunkXYZI				newChunk				= {readChunkHeader};
					view<cnst SVOXVoxel>	readChunkData			= {};
					if_fail_fe(voxViewLoadLegacy(content, readChunkData));
					if_true_fe(content.size());
					if_fail_fe(newChunk.Data.append(readChunkData));
					for(u2_t iVoxel = 0; iVoxel < newChunk.Data.size(); ++iVoxel) {
						SVOXVoxel					& voxel					= newChunk.Data[iVoxel];
						voxel						= {voxel.y, voxel.z, voxel.x, voxel.i};
					}
					if_fail_fe(loaded.ChunksCoord.push_back(newChunk));
					continue;
				}

				if(readChunkHeader.IsType("rOBJ")) {
					SVOXChunkRenderAttributes newChunk = {readChunkHeader};
					if_fail_fe(voxDictLoad(newChunk.Attributes, content));
					if_true_fe(content.size());
					if_fail_fe(loaded.ChunksRenderAttributes.push_back(newChunk));
				}
				else if(readChunkHeader.IsType("MATL")) {
					SVOXChunkMaterial			newChunk				= {readChunkHeader};
					if_fail_fe(loadPOD(content, newChunk.Id));
					if_fail_fe(voxDictLoad(newChunk.Attributes, content));
					if_true_fe(content.size());
					if_fail_fe(loaded.ChunksMaterial.push_back(newChunk));
				}
				else if(readChunkHeader.IsType("rCAM")) {
					SVOXChunkCamera				newChunk				= {readChunkHeader};
					if_fail_fe(loadPOD(content, newChunk.Id));
					if_fail_fe(voxDictLoad(newChunk.Attributes, content));
					if_true_fe(content.size());
					if_fail_fe(loaded.ChunksCamera.push_back(newChunk));
				}
				else if(readChunkHeader.IsType("LAYR")) {
					SVOXChunkLayer				newChunk				= {readChunkHeader};
					if_fail_fe(loadPOD(content, newChunk.Id));
					if_fail_fe(voxDictLoad(newChunk.Attributes, content));
					if_fail_fe(loadPOD(content, newChunk.IdReserved));
					if_true_fe(content.size());
					if_fail_fe(loaded.ChunksLayer.push_back(newChunk));
				}
				else if(readChunkHeader.IsType("NOTE")) {
					SVOXChunkNote				newChunk				= {readChunkHeader};
					u2_t						countNames				= 0;
					if_fail_fe(loadPOD(content, countNames));
					for(u2_t iName = 0; iName < countNames; ++iName) {
						vcst_t						readName				= {};
						if_fail_fe(voxViewLoadLegacy(content, readName));
						if_fail_fe(newChunk.ColorNames.push_back(string{readName}));
					}
					if_true_fe(content.size());
					if_fail_fe(loaded.ChunksNote.push_back(newChunk));
				}
				else if(readChunkHeader.IsType("nGRP")) {
					SVOXChunkNode				newChunk				= {readChunkHeader};
					if_fail_fe(loadPOD(content, newChunk.Id));
					if_fail_fe(voxDictLoad(newChunk.Attributes, content));
					vcs2_t						readChildren			= {};
					if_fail_fe(voxViewLoadLegacy(content, readChildren));
					if_fail_fe(newChunk.IdChildren.append(readChildren));
					if_true_fe(content.size());
					if_fail_fe(loaded.ChunksNode.push_back(newChunk));
				}
				else if(readChunkHeader.IsType("nSHP")) {
					SVOXChunkShape				newChunk				= {readChunkHeader};
					if_fail_fe(loadPOD(content, newChunk.Id));
					if_fail_fe(voxDictLoad(newChunk.Attributes, content));
					u2_t						countModels				= 0;
					if_fail_fe(loadPOD(content, countModels));
					for(u2_t iModel = 0; iModel < countModels; ++iModel) {
						SVOXShapeModel				newModel				= {};
						if_fail_fe(loadPOD(content, newModel.Id));
						if_fail_fe(voxDictLoad(newModel.Attributes, content));
						if_fail_fe(newChunk.Models.push_back(newModel));
					}
					if_true_fe(content.size());
					if_fail_fe(loaded.ChunksShape.push_back(newChunk));
				}
				else if(readChunkHeader.IsType("nTRN")) {
					SVOXChunkTransform			newChunk				= {readChunkHeader};
					if_fail_fe(loadPOD(content, newChunk.Id));
					if_fail_fe(voxDictLoad(newChunk.Attributes, content));
					if_fail_fe(loadPOD(content, newChunk.IdChild));
					if_fail_fe(loadPOD(content, newChunk.IdReserved));
					if_fail_fe(loadPOD(content, newChunk.IdLayer));
					u2_t						countFrames				= 0;
					if_fail_fe(loadPOD(content, countFrames));
					for(u2_t iFrame = 0; iFrame < countFrames; ++iFrame) {
						SVOXDict					frameAttributes			= {};
						if_fail_fe(voxDictLoad(frameAttributes, content));
						SVOXTransformFrame		newFrame				= {};
						for(u2_t iAttribute = 0; iAttribute < frameAttributes.size(); ++iAttribute) {
							vcst_t						key						= frameAttributes[iAttribute].Key;
							vcst_t						value					= frameAttributes[iAttribute].Val;
							if(key == GPK_CXS("_r")) {
								u2_t						rotation					= 0;
								if_fail_fe(voxUnsignedLoad(value, rotation));
								if_true_fe(rotation > 0xFFU);
								newFrame.Rotation			= (u0_t)rotation;
							}
							else if(key == GPK_CXS("_t")) {
								if_fail_fe(voxTranslationLoad(value, newFrame.Translation));
							}
							else if(key == GPK_CXS("_f")) {
								if_fail_fe(voxUnsignedLoad(value, newFrame.IndexFrame));
							}
						}
						if_fail_fe(newChunk.Frames.push_back(newFrame));
					}
					if_true_fe(content.size());
					if_fail_fe(loaded.ChunksTransform.push_back(newChunk));
				}
				else {
					vcst_t					chunkType				= {readChunkHeader.Type};
					warning_printf("Unknown chunk type: %.*s.", GPK_VIEW_ARGS(chunkType));
				}
			}
			if_true_fe(false == mainFound);
			*this						= loaded;
			input						= remaining;
			rtrn 0;
		}
	};

} // namespace

#endif // GPK_VOX_H_23627


