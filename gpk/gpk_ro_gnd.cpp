#include "gpk_ro_gnd.h"
#include "gpk_grid_view.h"
#include "gpk_stream_view.h"

namespace gpk
{
#pragma pack(push, 1)
	struct SGNDHeader {
					uint32_t									nMagicHeader									;
					uint8_t										nVersionMajor									;
					uint8_t										nVersionMinor									;
	};
#pragma pack(pop)
}// namespace

				::gpk::error_t								gpk::gndFileLoad										(::gpk::SGNDFileContents& loaded, const ::gpk::array_view<ubyte_t>	& input)							{
	::gpk::stream_view<const ubyte_t>								gnd_stream										= {input.begin(), input.size()};
	::gpk::SGNDHeader												gndHeader										= {};
	gpk_necall(gnd_stream.read_pod(gndHeader), "Cannot read GND header. Corrupt file?");
#if defined (GPK_ANDROID) || defined(GPK_LINUX)
	ree_if(gndHeader.nMagicHeader != 0x4E475247UL , "Invalid GND file header.");
#elif defined(GPK_WINDOWS)
	ree_if(gndHeader.nMagicHeader != 'NGRG', "Invalid GND file header.");
#endif
	ree_if((gndHeader.nVersionMajor < 1) || (gndHeader.nVersionMajor == 1 && gndHeader.nVersionMinor < 5), "Invalid GND file version. Major version: %u, Minor version: %u", (int)gndHeader.nVersionMajor, (int)gndHeader.nVersionMinor);
	uint32_t														nTextureCount									= 0;
	uint32_t														nTextureStringSize								= 0;
	gnd_stream.read_pod(loaded.Metrics		);
	gnd_stream.read_pod(nTextureCount		);
	gnd_stream.read_pod(nTextureStringSize	);
	loaded.TextureNames.resize(nTextureCount);
	for(uint32_t iTexture = 0; iTexture < nTextureCount; ++iTexture) 
		loaded.TextureNames[iTexture].resize(nTextureStringSize);

	for(uint32_t iTexture = 0; iTexture < nTextureCount; ++iTexture) 
		gnd_stream.read_pod(&loaded.TextureNames[iTexture][0], nTextureStringSize); 

	uint32_t														tileCountBrightness								= 0; 
	gnd_stream.read_pod(tileCountBrightness		);
	gnd_stream.read_pod(loaded.LightmapSize.x	);
	gnd_stream.read_pod(loaded.LightmapSize.y	);
	gnd_stream.read_pod(loaded.LightmapTiles		);
	loaded.lstTileBrightnessData.resize(tileCountBrightness);	
	gnd_stream.read_pod(loaded.lstTileBrightnessData.begin(), loaded.lstTileBrightnessData.size()); 

	uint32_t														tileCountSkin									= 0; 
	gnd_stream.read_pod(tileCountSkin);
	loaded.lstTileTextureData	.resize(tileCountSkin);			
	gnd_stream.read_pod(loaded.lstTileTextureData.begin(), loaded.lstTileTextureData.size()); 
	
	uint32_t														tileCountGeometry								= loaded.Metrics.Size.x * loaded.Metrics.Size.y;
	loaded.lstTileGeometryData.resize(tileCountGeometry); 
	if( gndHeader.nVersionMajor > 1 || ( gndHeader.nVersionMajor == 1 && gndHeader.nVersionMinor >= 7 ) ) 
		gnd_stream.read_pod(loaded.lstTileGeometryData.begin(), loaded.lstTileGeometryData.size()); 
	else if( gndHeader.nVersionMajor < 1 || ( gndHeader.nVersionMajor == 1 && gndHeader.nVersionMinor <= 5 ) ) {// it seems old 1.5 format used 16 bit integers
		for(uint32_t iTile = 0; iTile < tileCountGeometry; ++iTile) {
			int16_t															top												= -1;
			int16_t															right											= -1;
			int16_t															front											= -1;
			int16_t															flags											= -1;
			::gpk::STileGeometryGND											& tileGeometry									= loaded.lstTileGeometryData[iTile];
			gnd_stream.read_pod(tileGeometry.fHeight, 4); 
			gnd_stream.read_pod(top		); 
			gnd_stream.read_pod(right	); 
			gnd_stream.read_pod(front	); 
			gnd_stream.read_pod(flags	); 
			tileGeometry.SkinMapping.SkinIndexTop						= top	;
			tileGeometry.SkinMapping.SkinIndexRight						= right	;
			tileGeometry.SkinMapping.SkinIndexFront						= front	;
			//tileGeometry.Flags											= flags	;
		}
	}
	return gnd_stream.CursorPosition;
}

			::gpk::error_t								gpk::gndFileLoad											(::gpk::SGNDFileContents& loaded, FILE								* input)							{ 
	loaded, input;
	return 0; 
}

			::gpk::error_t								gpk::gndFileLoad											(::gpk::SGNDFileContents& loaded, const ::gpk::view_const_string	& input)							{ 
	FILE														* fp												= 0;
	ree_if(0 != fopen_s(&fp, input.begin(), "rb") || 0 == fp, "Cannot open .gnd file: %s.", input.begin());

	fseek(fp, 0, SEEK_END);
	int32_t														fileSize											= (int32_t)ftell(fp);
	fseek(fp, 0, SEEK_SET);
	::gpk::array_pod<ubyte_t>									fileInMemory										= 0;
	gpk_necall(fileInMemory.resize(fileSize), "File too large? : %llu.", (uint64_t)fileSize);
	if(fileSize != (int32_t)fread(fileInMemory.begin(), sizeof(ubyte_t), fileSize, fp)) {
		fclose(fp);
		error_printf("fread() failed! file: '%s'.", input.begin());
		return -1;
	}
	fclose(fp);

	return gndFileLoad(loaded, fileInMemory);
}

#pragma warning(disable : 4100)
static		::gpk::error_t									gndGenerateFaceGeometryRight							(uint32_t baseX, uint32_t baseZ, float tileScale, const ::gpk::STileGeometryGND & geometryTile, const ::gpk::STileGeometryGND * geometryTileRight, const ::gpk::array_view<::gpk::STileSkinGND>& lstTileSkinData, ::gpk::SModelNodeGND& generated, ::gpk::STileMapping & out_mapping)	{ 
	const uint32_t													baseVertexIndex											= generated.Vertices.size();
	const ::gpk::SCoord3<float>										faceVerts	[4]											= 
		{ {baseX + 0.0f, (geometryTile.fHeight[2] / tileScale), baseZ + 1.0f}
		, {baseX + 1.0f, (geometryTile.fHeight[3] / tileScale), baseZ + 1.0f}
		, {baseX + 0.0f, geometryTileRight ? (geometryTileRight->fHeight[0] / tileScale) : 0.0f, baseZ + 1.0f}
		, {baseX + 1.0f, geometryTileRight ? (geometryTileRight->fHeight[1] / tileScale) : 0.0f, baseZ + 1.0f}
		};
	const ::gpk::SCoord3<float>										faceNormals	[4]											=
		{ (faceVerts[1] - faceVerts[0]).Cross(faceVerts[2] - faceVerts[0]).Normalize()
		, (faceVerts[3] - faceVerts[1]).Cross(faceVerts[0] - faceVerts[1]).Normalize()
		, (faceVerts[0] - faceVerts[2]).Cross(faceVerts[3] - faceVerts[2]).Normalize()
		, (faceVerts[2] - faceVerts[3]).Cross(faceVerts[1] - faceVerts[3]).Normalize()
		};

	generated.Vertices		.append(faceVerts	);
	generated.Normals		.append(faceNormals	);
	for(uint32_t i = 0; i < 4; ++i)
		out_mapping.VerticesRight[i]								= baseVertexIndex + i;

	{
		const int32_t													faceSkins	[4]										= 
			{ geometryTile.SkinMapping.SkinIndexRight
			, geometryTile.SkinMapping.SkinIndexRight
			, geometryTile.SkinMapping.SkinIndexRight
			, geometryTile.SkinMapping.SkinIndexRight
			};
		//generated.SkinIndices.append(faceSkins);
		const ::gpk::STileSkinGND										& skinTile											= lstTileSkinData[faceSkins[0]];
		::gpk::SCoord2<float>											faceUVs	[4]											;
		for(uint32_t i = 0; i < 4; ++i)
			faceUVs[i]													= {skinTile.u[i], skinTile.v[i]};
		generated.UVs.append(faceUVs);
	}
	{
		const ::gpk::STriangleWeights<uint32_t>							faceIndices[6]										= 
			{	{ baseVertexIndex + 0 // + 0//+ 0 // 0
				, baseVertexIndex + 1 // + 1//+ 2 // 1
				, baseVertexIndex + 2 // + 2//+ 1 // 2
				} //			 //// 	+	 //// 
			,	{ baseVertexIndex + 1 // + 1//+ 1 // 1
				, baseVertexIndex + 3 // + 3//+ 2 // 3
				, baseVertexIndex + 2 // + 2//+ 3 // 2
			}
			};
		generated.VertexIndices	.append(faceIndices);
	}	
	return 0; 
}

static		::gpk::error_t									gndGenerateFaceGeometryFront							(uint32_t baseX, uint32_t baseZ, float tileScale, const ::gpk::STileGeometryGND & geometryTile, const ::gpk::STileGeometryGND * geometryTileFront, const ::gpk::array_view<::gpk::STileSkinGND>& lstTileSkinData, ::gpk::SModelNodeGND& generated, ::gpk::STileMapping & out_mapping)	{ 
	const uint32_t													baseVertexIndex											= generated.Vertices.size();
	const ::gpk::SCoord3<float>										faceVerts	[4]											= 
		{ {baseX + 1.0f, (geometryTile.fHeight[3] / tileScale), baseZ + 1.0f}
		, {baseX + 1.0f, (geometryTile.fHeight[1] / tileScale), baseZ + 0.0f}
		, {baseX + 1.0f, geometryTileFront ? (geometryTileFront->fHeight[2] / tileScale) : 0.0f, baseZ + 1.0f}
		, {baseX + 1.0f, geometryTileFront ? (geometryTileFront->fHeight[0] / tileScale) : 0.0f, baseZ + 0.0f}
		};
	const ::gpk::SCoord3<float>										faceNormals	[4]											=
		{ (faceVerts[1] - faceVerts[0]).Cross(faceVerts[2] - faceVerts[0]).Normalize()
		, (faceVerts[3] - faceVerts[1]).Cross(faceVerts[0] - faceVerts[1]).Normalize()
		, (faceVerts[0] - faceVerts[2]).Cross(faceVerts[3] - faceVerts[2]).Normalize()
		, (faceVerts[2] - faceVerts[3]).Cross(faceVerts[1] - faceVerts[3]).Normalize()
		};
	generated.Vertices		.append(faceVerts	);
	generated.Normals		.append(faceNormals	);
	for(uint32_t i = 0; i < 4; ++i)
		out_mapping.VerticesFront[i]								= baseVertexIndex + i;
	{
		const int32_t													faceSkins	[4]										= 
			{ geometryTile.SkinMapping.SkinIndexFront
			, geometryTile.SkinMapping.SkinIndexFront
			, geometryTile.SkinMapping.SkinIndexFront
			, geometryTile.SkinMapping.SkinIndexFront
			};
		//generated.SkinIndices.append(faceSkins);
		const ::gpk::STileSkinGND										& skinTile											= lstTileSkinData[faceSkins[0]];
		::gpk::SCoord2<float>											faceUVs	[4]											;
		for(uint32_t i = 0; i < 4; ++i)
			faceUVs[i]													= {skinTile.u[i], skinTile.v[i]};
		generated.UVs.append(faceUVs);
		const ::gpk::STriangleWeights<uint32_t>							faceIndices[6]										= 
			{	{ baseVertexIndex + 0 // + 0//+ 0 // 0
				, baseVertexIndex + 1 // + 1//+ 2 // 1
				, baseVertexIndex + 2 // + 2//+ 1 // 2
				} //			 //// 	+	 //// 
			,	{ baseVertexIndex + 1 // + 1//+ 1 // 1
				, baseVertexIndex + 3 // + 3//+ 2 // 3
				, baseVertexIndex + 2 // + 2//+ 3 // 2
			}
			};
		generated.VertexIndices	.append(faceIndices);
	}	
	return 0; 
}
#pragma warning(default : 4100)

static		::gpk::error_t									gndGenerateFaceGeometryTop								(uint32_t baseX, uint32_t baseZ, float tileScale, const ::gpk::STileGeometryGND & geometryTile, const ::gpk::array_view<::gpk::STileSkinGND>& lstTileSkinData, ::gpk::SModelNodeGND& generated, ::gpk::STileMapping & out_mapping)	{
	const uint32_t													baseVertexIndex											= generated.Vertices.size();
	const ::gpk::SCoord3<float>										faceVerts	[4]											= 
		{ {baseX + 0.0f, (geometryTile.fHeight[0] / tileScale), baseZ + 0.0f}
		, {baseX + 1.0f, (geometryTile.fHeight[1] / tileScale), baseZ + 0.0f}
		, {baseX + 0.0f, (geometryTile.fHeight[2] / tileScale), baseZ + 1.0f}
		, {baseX + 1.0f, (geometryTile.fHeight[3] / tileScale), baseZ + 1.0f}
		};
	const ::gpk::SCoord3<float>										faceNormals	[4]											=
		{ (faceVerts[1] - faceVerts[0]).Cross(faceVerts[2] - faceVerts[0]).Normalize()
		, (faceVerts[3] - faceVerts[1]).Cross(faceVerts[0] - faceVerts[1]).Normalize()
		, (faceVerts[0] - faceVerts[2]).Cross(faceVerts[3] - faceVerts[2]).Normalize()
		, (faceVerts[2] - faceVerts[3]).Cross(faceVerts[1] - faceVerts[3]).Normalize()
		};
	//::gpk::SCoord3<float>											normal												
	//	= faceNormals	[0]
	//	+ faceNormals	[1]
	//	+ faceNormals	[2]
	//	+ faceNormals	[3]
	//	;
	//(normal /=4).Normalize();
	//faceNormals	[0] = normal;
	//faceNormals	[1] = normal;
	//faceNormals	[2] = normal;
	//faceNormals	[3] = normal;
	generated.Vertices		.append(faceVerts	);
	generated.Normals		.append(faceNormals	);
	for(uint32_t i = 0; i < 4; ++i)
		out_mapping.VerticesTop[i]								= baseVertexIndex + i;

	{
		const int32_t													faceSkins	[4]										= 
			{ geometryTile.SkinMapping.SkinIndexTop
			, geometryTile.SkinMapping.SkinIndexTop
			, geometryTile.SkinMapping.SkinIndexTop
			, geometryTile.SkinMapping.SkinIndexTop
			};
		//generated.SkinIndices.append(faceSkins);
		const ::gpk::STileSkinGND										& skinTile											= lstTileSkinData[faceSkins[0]];
		::gpk::SCoord2<float>											faceUVs	[4]											;
		for(uint32_t i = 0; i < 4; ++i)
			faceUVs[i]													= {skinTile.u[i], skinTile.v[i]};
		generated.UVs.append(faceUVs);
	}
	{
		const ::gpk::STriangleWeights<uint32_t>							faceIndices[6]										= 
			{	{ baseVertexIndex + 0 // + 0//+ 0 // 0
				, baseVertexIndex + 1 // + 1//+ 2 // 1
				, baseVertexIndex + 2 // + 2//+ 1 // 2
				} //			 //// 	+	 //// 
			,	{ baseVertexIndex + 1 // + 1//+ 1 // 1
				, baseVertexIndex + 3 // + 3//+ 2 // 3
				, baseVertexIndex + 2 // + 2//+ 3 // 2
			}
			};
		generated.VertexIndices	.append(faceIndices);
	}
	return 0;
}
			::gpk::error_t										gpk::gndGenerateFaceGeometry
	( const ::gpk::array_pod<STileSkinGND		>	lstTileTextureData
	, const ::gpk::array_pod<STileGeometryGND	>	lstTileGeometryData
	, const ::gpk::STiledTerrainMetricsGND			tileMapMetrics
	, TILE_FACE_FACING								facing_direction
	, int32_t										textureIndex
	, SModelNodeGND									& generated
	, ::gpk::grid_view<::gpk::STileMapping>			& out_mapping
	) {
	::gpk::grid_view<const ::gpk::STileGeometryGND>						geometryView										= {lstTileGeometryData.begin(), tileMapMetrics.Size};
	for(uint32_t y = 0; y < geometryView.metrics().y; ++y)
	for(uint32_t x = 0; x < geometryView.metrics().x; ++x) {
		const ::gpk::STileGeometryGND										& geometryTile										= geometryView[y][x];
		const ::gpk::STileGeometryGND										* geometryTileFront									= ((x + 1) < geometryView.metrics().x) ? &geometryView	[y][x + 1]	: 0;
		const ::gpk::STileGeometryGND										* geometryTileRight									= ((y + 1) < geometryView.metrics().y) ? &geometryView	[y + 1][x]	: 0;
		//const ::gpk::STileGeometryGND										* geometryTileFrontRight							
		//	= ((x + 1) < geometryView.metrics().x 
		//	&& (y + 1) < geometryView.metrics().y) 
		//	? &geometryView	[y + 1][x + 1] : 0;
		::gpk::TILE_FACE_FACING												facingDirection										= facing_direction;
		if(facingDirection == ::gpk::TILE_FACE_FACING_FRONT && geometryTileFront && (geometryTileFront->fHeight[0] > geometryTile.fHeight[1] || geometryTileFront->fHeight[2] > geometryTile.fHeight[3])) facingDirection = ::gpk::TILE_FACE_FACING_BACK;
		if(facingDirection == ::gpk::TILE_FACE_FACING_BACK	&& geometryTileFront && (geometryTileFront->fHeight[0] < geometryTile.fHeight[1] || geometryTileFront->fHeight[2] < geometryTile.fHeight[3])) facingDirection = ::gpk::TILE_FACE_FACING_FRONT;
		if(facingDirection == ::gpk::TILE_FACE_FACING_RIGHT && geometryTileRight && (geometryTileRight->fHeight[0] > geometryTile.fHeight[2] || geometryTileRight->fHeight[1] > geometryTile.fHeight[3])) facingDirection = ::gpk::TILE_FACE_FACING_LEFT;
		if(facingDirection == ::gpk::TILE_FACE_FACING_LEFT	&& geometryTileRight && (geometryTileRight->fHeight[0] < geometryTile.fHeight[2] || geometryTileRight->fHeight[1] < geometryTile.fHeight[3])) facingDirection = ::gpk::TILE_FACE_FACING_RIGHT;
		if(facingDirection == ::gpk::TILE_FACE_FACING_FRONT && 0 == geometryTileFront && (0 > geometryTile.fHeight[1] || 0 > geometryTile.fHeight[3])) facingDirection = ::gpk::TILE_FACE_FACING_BACK;
		if(facingDirection == ::gpk::TILE_FACE_FACING_BACK	&& 0 == geometryTileFront && (0 < geometryTile.fHeight[1] || 0 < geometryTile.fHeight[3])) facingDirection = ::gpk::TILE_FACE_FACING_FRONT;
		if(facingDirection == ::gpk::TILE_FACE_FACING_RIGHT && 0 == geometryTileRight && (0 > geometryTile.fHeight[2] || 0 > geometryTile.fHeight[3])) facingDirection = ::gpk::TILE_FACE_FACING_LEFT;
		if(facingDirection == ::gpk::TILE_FACE_FACING_LEFT	&& 0 == geometryTileRight && (0 < geometryTile.fHeight[2] || 0 < geometryTile.fHeight[3])) facingDirection = ::gpk::TILE_FACE_FACING_RIGHT;
		if(facingDirection != facing_direction)
			continue;
		switch(facing_direction) {
		default:
			break;
		//case TILE_FACE_FACING_BOTTOM: 
		case TILE_FACE_FACING_TOP	: 
			if(-1 == geometryTile.SkinMapping.SkinIndexTop)
				continue;
			if(textureIndex != -1 && textureIndex != lstTileTextureData[geometryTile.SkinMapping.SkinIndexTop].TextureIndex)
				continue;
			::gndGenerateFaceGeometryTop(x, y, tileMapMetrics.TileScale, geometryTile, lstTileTextureData, generated, out_mapping[y][x]);
			break;
		case TILE_FACE_FACING_BACK	: 
		case TILE_FACE_FACING_FRONT	: 
			if(-1 == geometryTile.SkinMapping.SkinIndexFront)
				continue;
			if(textureIndex != -1 && textureIndex != lstTileTextureData[geometryTile.SkinMapping.SkinIndexFront].TextureIndex)
				continue;
			::gndGenerateFaceGeometryFront(x, y, tileMapMetrics.TileScale, geometryTile, geometryTileFront, lstTileTextureData, generated, out_mapping[y][x]);
			break;
		case TILE_FACE_FACING_LEFT	: 
		case TILE_FACE_FACING_RIGHT	: 
			if(-1 == geometryTile.SkinMapping.SkinIndexRight)
				continue;
			if(textureIndex != -1 && textureIndex != lstTileTextureData[geometryTile.SkinMapping.SkinIndexRight].TextureIndex)
				continue;
			::gndGenerateFaceGeometryRight(x, y, tileMapMetrics.TileScale, geometryTile, geometryTileRight, lstTileTextureData, generated, out_mapping[y][x]);
			break;
		}
	}
	return 0;
}
