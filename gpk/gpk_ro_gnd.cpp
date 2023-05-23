#include "gpk_ro_gnd.h"
#include "gpk_view_grid.h"
#include "gpk_view_stream.h"
#include "gpk_io.h"
#include "gpk_storage.h"

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

::gpk::error_t								gpk::gndFileLoad										(::gpk::SGNDFileContents & loaded, const ::gpk::vu8	& input)							{
	::gpk::view_stream<const uint8_t>								gnd_stream										= {input.begin(), input.size()};
	::gpk::SGNDHeader												gndHeader										= {};
	gpk_necall(gnd_stream.read_pod(gndHeader), "%s", "Cannot read GND header. Corrupt file?");
#if defined (GPK_ANDROID) || defined(GPK_LINUX)
	ree_if(gndHeader.nMagicHeader != 0x4E475247UL , "%s", "Invalid GND file header.");
#elif defined(GPK_WINDOWS)
	ree_if(gndHeader.nMagicHeader != 'NGRG', "%s", "Invalid GND file header.");
#endif
	ree_if((gndHeader.nVersionMajor < 1) || (gndHeader.nVersionMajor == 1 && gndHeader.nVersionMinor < 5), "Invalid GND file version. Major version: %u, Minor version: %u", (int)gndHeader.nVersionMajor, (int)gndHeader.nVersionMinor);
	uint32_t														nTextureCount									= 0;
	uint32_t														nTextureStringSize								= 0;
	gnd_stream.read_pod(loaded.Metrics		);
	gnd_stream.read_pod(nTextureCount		);
	gnd_stream.read_pod(nTextureStringSize	);
	loaded.TextureNames.resize(nTextureCount);
	for(uint32_t iTexture = 0; iTexture < nTextureCount; ++iTexture) {
		loaded.TextureNames[iTexture].resize(nTextureStringSize);
		gnd_stream.read_pod(&loaded.TextureNames[iTexture][0], nTextureStringSize);
	}
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
	(void)loaded, (void)input;
	return 0;
}

			::gpk::error_t								gpk::gndFileLoad											(::gpk::SGNDFileContents& loaded, const ::gpk::vcs	& input)							{
	::gpk::apod<int8_t>									fileInMemory												= {};
	gpk_necall(::gpk::fileToMemory(input, fileInMemory), "Failed to load .gnd file: %s", input.begin());
	return ::gpk::gndFileLoad(loaded, ::gpk::vu8((uint8_t*)fileInMemory.begin(), fileInMemory.size()));
}

#if defined(GPK_WINDOWS)
#	pragma warning(disable : 4100)
#endif
static		::gpk::error_t									gndGenerateFaceGeometryRight							(uint32_t baseX, uint32_t baseZ, float tileScale, const ::gpk::STileGeometryGND & geometryTile, const ::gpk::STileGeometryGND * geometryTileRight, const ::gpk::view_array<::gpk::STileSkinGND>& lstTileSkinData, ::gpk::SModelNodeGND& generated, ::gpk::STileMapping & out_mapping)	{
	const uint32_t													baseVertexIndex											= generated.Vertices.size();
	const ::gpk::n3<float>										faceVerts	[4]											=
		{ {baseX + 0.0f, (geometryTile.fHeight[2] / tileScale), baseZ + 1.0f}
		, {baseX + 1.0f, (geometryTile.fHeight[3] / tileScale), baseZ + 1.0f}
		, {baseX + 0.0f, geometryTileRight ? (geometryTileRight->fHeight[0] / tileScale) : 0.0f, baseZ + 1.0f}
		, {baseX + 1.0f, geometryTileRight ? (geometryTileRight->fHeight[1] / tileScale) : 0.0f, baseZ + 1.0f}
		};
	const ::gpk::n3<float>										faceNormals	[4]											=
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
		::gpk::n2<float>											faceUVs	[4]											;
		for(uint32_t i = 0; i < 4; ++i)
			faceUVs[i]													= {skinTile.u[i], skinTile.v[i]};
		generated.UVs.append(faceUVs);
	}
	{
		const ::gpk::tri<uint32_t>							faceIndices[6]										=
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

static		::gpk::error_t									gndGenerateFaceGeometryFront							(uint32_t baseX, uint32_t baseZ, float tileScale, const ::gpk::STileGeometryGND & geometryTile, const ::gpk::STileGeometryGND * geometryTileFront, const ::gpk::view_array<::gpk::STileSkinGND>& lstTileSkinData, ::gpk::SModelNodeGND& generated, ::gpk::STileMapping & out_mapping)	{
	const uint32_t													baseVertexIndex											= generated.Vertices.size();
	const ::gpk::n3<float>										faceVerts	[4]											=
		{ {baseX + 1.0f, (geometryTile.fHeight[3] / tileScale), baseZ + 1.0f}
		, {baseX + 1.0f, (geometryTile.fHeight[1] / tileScale), baseZ + 0.0f}
		, {baseX + 1.0f, geometryTileFront ? (geometryTileFront->fHeight[2] / tileScale) : 0.0f, baseZ + 1.0f}
		, {baseX + 1.0f, geometryTileFront ? (geometryTileFront->fHeight[0] / tileScale) : 0.0f, baseZ + 0.0f}
		};
	const ::gpk::n3<float>										faceNormals	[4]											=
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
		::gpk::n2<float>											faceUVs	[4]											;
		for(uint32_t i = 0; i < 4; ++i)
			faceUVs[i]													= {skinTile.u[i], skinTile.v[i]};
		generated.UVs.append(faceUVs);
		const ::gpk::tri<uint32_t>							faceIndices[6]										=
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
#if defined(GPK_WINDOWS)
#	pragma warning(default : 4100)
#endif

static		::gpk::error_t									gndGenerateFaceGeometryTop								(uint32_t baseX, uint32_t baseZ, float tileScale, const ::gpk::STileGeometryGND & geometryTile, const ::gpk::view_array<::gpk::STileSkinGND>& lstTileSkinData, ::gpk::SModelNodeGND& generated, ::gpk::STileMapping & out_mapping)	{
	const uint32_t													baseVertexIndex											= generated.Vertices.size();
	const ::gpk::n3<float>										faceVerts	[4]											=
		{ {baseX + 0.0f, (geometryTile.fHeight[0] / tileScale), baseZ + 0.0f}
		, {baseX + 1.0f, (geometryTile.fHeight[1] / tileScale), baseZ + 0.0f}
		, {baseX + 0.0f, (geometryTile.fHeight[2] / tileScale), baseZ + 1.0f}
		, {baseX + 1.0f, (geometryTile.fHeight[3] / tileScale), baseZ + 1.0f}
		};
	const ::gpk::n3<float>										faceNormals	[4]											=
		{ (faceVerts[1] - faceVerts[0]).Cross(faceVerts[2] - faceVerts[0]).Normalize()
		, (faceVerts[3] - faceVerts[1]).Cross(faceVerts[0] - faceVerts[1]).Normalize()
		, (faceVerts[0] - faceVerts[2]).Cross(faceVerts[3] - faceVerts[2]).Normalize()
		, (faceVerts[2] - faceVerts[3]).Cross(faceVerts[1] - faceVerts[3]).Normalize()
		};
	//::gpk::n3<float>											normal
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
		::gpk::n2<float>											faceUVs	[4]											;
		for(uint32_t i = 0; i < 4; ++i)
			faceUVs[i]													= {skinTile.u[i], skinTile.v[i]};
		generated.UVs.append(faceUVs);
	}
	{
		const ::gpk::tri<uint32_t>							faceIndices[6]										=
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
	( const ::gpk::apod<STileSkinGND		>	lstTileTextureData
	, const ::gpk::apod<STileGeometryGND	>	lstTileGeometryData
	, const ::gpk::STiledTerrainMetricsGND			tileMapMetrics
	, TILE_FACE_FACING								facing_direction
	, int32_t										textureIndex
	, SModelNodeGND									& generated
	, ::gpk::view2d<::gpk::STileMapping>			& out_mapping
	) {
	::gpk::view2d<const ::gpk::STileGeometryGND>						geometryView										= {lstTileGeometryData.begin(), tileMapMetrics.Size};
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

			::gpk::error_t										gpk::blendGNDNormals								(const ::gpk::view2d<::gpk::STileGeometryGND> &tileGeometryView, const ::gpk::view_array<::gpk::STileSkinGND>& lstTileSkinData, const ::gpk::view2d<::gpk::STileMapping>& tileMappingView, ::gpk::view_array<::gpk::SModelNodeGND> & gndModelNodes)																						{
	for(uint32_t y = 0; y < tileGeometryView.metrics().y - 1; ++y) {
		const ::gpk::view_array<const ::gpk::STileGeometryGND	>			rowTileGeometry										= tileGeometryView	[y];
		const ::gpk::view_array<const ::gpk::STileMapping		>			rowTileMapping										= tileMappingView	[y];
		const ::gpk::view_array<const ::gpk::STileGeometryGND	>			rowNextTileGeometry									= tileGeometryView	[y + 1];
		const ::gpk::view_array<const ::gpk::STileMapping		>			rowNextTileMapping									= tileMappingView	[y + 1];
		for(uint32_t x = 0; x < tileGeometryView.metrics().x - 1; ++x) {
			const ::gpk::STileGeometryGND										& tileGeometry0										= rowTileGeometry		[x];
			const ::gpk::STileGeometryGND										& tileGeometry1										= rowTileGeometry		[x + 1];
			const ::gpk::STileGeometryGND										& tileGeometry2										= rowNextTileGeometry	[x];
			const ::gpk::STileGeometryGND										& tileGeometry3										= rowNextTileGeometry	[x + 1];

			const ::gpk::STileMapping											& tileMapping0										= rowTileMapping		[x + 0];
			const ::gpk::STileMapping											& tileMapping1										= rowTileMapping		[x + 1];
			const ::gpk::STileMapping											& tileMapping2										= rowNextTileMapping	[x + 0];
			const ::gpk::STileMapping											& tileMapping3										= rowNextTileMapping	[x + 1];

			const bool															processTile0										= (tileGeometry0.SkinMapping.SkinIndexTop != -1);// && (app.GNDData.lstTileTextureData[tileGeometry0.SkinMapping.SkinIndexTop].TextureIndex != -1);
			const bool															processTile1										= (tileGeometry1.SkinMapping.SkinIndexTop != -1) && tileGeometry0.SkinMapping.SkinIndexFront == -1;// && (app.GNDData.lstTileTextureData[tileGeometry1.SkinMapping.SkinIndexTop].TextureIndex != -1);
			const bool															processTile2										= (tileGeometry2.SkinMapping.SkinIndexTop != -1) && tileGeometry0.SkinMapping.SkinIndexRight == -1;// && (app.GNDData.lstTileTextureData[tileGeometry2.SkinMapping.SkinIndexTop].TextureIndex != -1);
			const bool															processTile3										= (tileGeometry3.SkinMapping.SkinIndexTop != -1) && (tileGeometry0.SkinMapping.SkinIndexFront == -1 && tileGeometry0.SkinMapping.SkinIndexRight == -1);// && (app.GNDData.lstTileTextureData[tileGeometry3.SkinMapping.SkinIndexTop].TextureIndex != -1);
			const int32_t														texIndex0											= processTile0 ? lstTileSkinData[tileGeometry0.SkinMapping.SkinIndexTop].TextureIndex : -1;
			const int32_t														texIndex1											= processTile1 ? lstTileSkinData[tileGeometry1.SkinMapping.SkinIndexTop].TextureIndex : -1;
			const int32_t														texIndex2											= processTile2 ? lstTileSkinData[tileGeometry2.SkinMapping.SkinIndexTop].TextureIndex : -1;
			const int32_t														texIndex3											= processTile3 ? lstTileSkinData[tileGeometry3.SkinMapping.SkinIndexTop].TextureIndex : -1;
			::gpk::n3<float>												normal												= {};
			uint32_t															divisor												= 0;
			if(processTile0) { ++divisor; ::gpk::SModelNodeGND & gndNode0 = gndModelNodes[texIndex0]; normal += gndNode0.Normals[tileMapping0.VerticesTop[3]]; }
			if(processTile1) { ++divisor; ::gpk::SModelNodeGND & gndNode1 = gndModelNodes[texIndex1]; normal += gndNode1.Normals[tileMapping1.VerticesTop[2]]; }
			if(processTile2) { ++divisor; ::gpk::SModelNodeGND & gndNode2 = gndModelNodes[texIndex2]; normal += gndNode2.Normals[tileMapping2.VerticesTop[1]]; }
			if(processTile3) { ++divisor; ::gpk::SModelNodeGND & gndNode3 = gndModelNodes[texIndex3]; normal += gndNode3.Normals[tileMapping3.VerticesTop[0]]; }
			if(divisor) {
				(normal /= divisor).Normalize();
				if(processTile0) { ::gpk::SModelNodeGND	& gndNode0 = gndModelNodes[texIndex0]; gndNode0.Normals[tileMapping0.VerticesTop[3]] = normal; }
				if(processTile1) { ::gpk::SModelNodeGND	& gndNode1 = gndModelNodes[texIndex1]; gndNode1.Normals[tileMapping1.VerticesTop[2]] = normal; }
				if(processTile2) { ::gpk::SModelNodeGND	& gndNode2 = gndModelNodes[texIndex2]; gndNode2.Normals[tileMapping2.VerticesTop[1]] = normal; }
				if(processTile3) { ::gpk::SModelNodeGND	& gndNode3 = gndModelNodes[texIndex3]; gndNode3.Normals[tileMapping3.VerticesTop[0]] = normal; }
			}
		}
	}
	return 0;
}
