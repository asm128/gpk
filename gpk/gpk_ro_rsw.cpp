#include "gpk_ro_rsw.h"
#include "gpk_view_stream.h"
#include "gpk_io.h"
#include "gpk_file.h"

GPK_USING_TYPEINT();

#pragma pack(push, 1)
	struct SRSWHeader {
		sc_t										Filecode[4];
		uint8_t										VersionMajor;
		uint8_t										VersionMinor;
	};

	struct SRSWWorldInfo {
		//::gpk::SRange;
		int32_t										Top				;
		int32_t										Bottom			;
		int32_t										Left			;
		int32_t										Right			;
		uint32_t									ObjectCount;
		//uc_t								Unknown2	[8];
		//uc_t								UnknownStr	[40];
		//uint32_t									UnknownInts	[2];
	};
#pragma pack(pop)

::gpk::error_t			gpk::rswFileLoad	(::gpk::SRSWFileContents & loaded, const ::gpk::vcu8 & input) {
	::gpk::view_stream<const uint8_t>	rsw_stream			= {input.begin(), input.size()};
	SRSWHeader							header				= {};//*(SRSWHeader*)input.begin();
	gpk_necs(rsw_stream.read_pod(header));//sizeof(SRSWHeader);
	info_printf("RSW magic number: %.4s.", header.Filecode);
	info_printf("RSW version: %u.%u.", (uint32_t)header.VersionMajor, (uint32_t)header.VersionMinor);
	info_printf("RSW version major: 0x%x.", (uint32_t)header.VersionMajor);
	info_printf("RSW version minor: 0x%x.", (uint32_t)header.VersionMinor);
	const uint32_t						promoted			= *(uint16_t*)&input[4];	// I had to put this in a variable because the static analyzer believed I may have been confused with this statement.
	info_printf("RSW version number: 0x%x.", promoted);
	(void)promoted;
	//info_printf("RSW version: 0x%x.", header.Version);
	rsw_stream.read_pod(loaded.INIFilename);
	rsw_stream.read_pod(loaded.GNDFilename);
	if(header.VersionMajor > 1 || (header.VersionMajor == 1 && header.VersionMinor >= 4))
		rsw_stream.read_pod(loaded.GATFilename);
	rsw_stream.read_pod(loaded.SOMFilename);
	info_printf("RSW INI: %s.", &loaded.INIFilename[0]);
	info_printf("RSW GND: %s.", &loaded.GNDFilename[0]);
	info_printf("RSW GAT: %s.", &loaded.GATFilename[0]);
	info_printf("RSW SOM: %s.", &loaded.SOMFilename[0]);
	SRSWWorldInfo												worldInfo;
	if(header.VersionMajor > 1 || (header.VersionMajor == 1 && header.VersionMinor >= 3))
		rsw_stream.read_pod(loaded.Water.Height);
	if(header.VersionMajor > 1 || (header.VersionMajor == 1 && header.VersionMinor >= 8)) {
		rsw_stream.read_pod(loaded.Water.Type		);
		rsw_stream.read_pod(loaded.Water.Amplitude	);
		rsw_stream.read_pod(loaded.Water.Speed		);
		rsw_stream.read_pod(loaded.Water.Pitch		);
	}
	if(header.VersionMajor > 1 || (header.VersionMajor == 1 && header.VersionMinor >= 9))
		rsw_stream.read_pod(loaded.Water.TexCycling);
	//else
	//	throw "todo";
	if(header.VersionMajor > 1 || (header.VersionMajor == 1 && header.VersionMinor >= 5))  {
		rsw_stream.read_pod(loaded.Light.Latitude	);
		rsw_stream.read_pod(loaded.Light.Longitude	);
		rsw_stream.read_pod(loaded.Light.Diffuse	);
		rsw_stream.read_pod(loaded.Light.Ambient	);
	}
	if(header.VersionMajor > 1 || (header.VersionMajor == 1 && header.VersionMinor >= 7))
		rsw_stream.read_pod(loaded.Light.Intensity	);

	if(header.VersionMajor > 1 || (header.VersionMajor == 1 && header.VersionMinor >= 6)) {
		rsw_stream.read_pod(worldInfo.Top		);
		rsw_stream.read_pod(worldInfo.Bottom	);
		rsw_stream.read_pod(worldInfo.Left		);
		rsw_stream.read_pod(worldInfo.Right		);
	}
	else {
		worldInfo.Top											= -500;
		worldInfo.Bottom										= 500;
		worldInfo.Left											= -500;
		worldInfo.Right											= 500;
	}

	rsw_stream.read_pod(worldInfo.ObjectCount);

	SModelInfoRSW												modelInfo;
	SLightInfoRSW												lightInfo;
	SEffectInfoRSW												effectInfo;
	SSoundInfoRSW												soundInfo;
	while(rsw_stream.CursorPosition < input.size()) {
		int32_t														objectType											= -1;
		rsw_stream.read_pod(objectType);
		if(-1 == objectType)
			break;
		switch(objectType) {
		default		: break;
		case	1	: // RSM Model
			if(header.VersionMajor > 1 || (header.VersionMajor == 1 && header.VersionMinor >= 3)) {
				rsw_stream.read_pod(modelInfo.Name);
				rsw_stream.read_pod(modelInfo.AnimType	);
				rsw_stream.read_pod(modelInfo.AnimSpeed	);
				rsw_stream.read_pod(modelInfo.BlockType	);
			}
			rsw_stream.read_pod(modelInfo.Filename		);
			rsw_stream.read_pod(modelInfo.Str2			);
			rsw_stream.read_pod(modelInfo.RootRSMNode	);
			rsw_stream.read_pod(modelInfo.Str4			);
			rsw_stream.read_pod(modelInfo.Position		);
			rsw_stream.read_pod(modelInfo.Rotation		);
			rsw_stream.read_pod(modelInfo.Scale			);
			info_printf(" ---------------------------------------------------------------------------------- RSW Model: %u ---------------------------------------------------------------------------------- ", loaded.RSWModels.size());
			info_printf("RSW model object found     : %s."			, &modelInfo.Name[0]);
			info_printf("RSW model object filename  : %s."			, &modelInfo.Filename[0]);
			info_printf("RSW model object str2      : %s."			, &modelInfo.Str2[0]);
			info_printf("RSW model object str3      : %s."			, &modelInfo.RootRSMNode[0]);
			info_printf("RSW model object str4      : %s."			, &modelInfo.Str4[0]);
			info_printf("RSW model object position  : {%f, %f, %f}.", modelInfo.Position	.x, modelInfo.Position	.y, modelInfo.Position	.z);
			info_printf("RSW model object rotation  : {%f, %f, %f}.", modelInfo.Rotation	.x, modelInfo.Rotation	.y, modelInfo.Rotation	.z);
			info_printf("RSW model object scale     : {%f, %f, %f}.", modelInfo.Scale		.x, modelInfo.Scale		.y, modelInfo.Scale		.z);
			info_printf("RSW model object animType  : %i."			, modelInfo.AnimType );
			info_printf("RSW model object animSpeed : %f."			, modelInfo.AnimSpeed);
			info_printf("RSW model object blockType : %i."			, modelInfo.BlockType);
			if(0 == modelInfo.Filename[0])
				return 0;
			loaded.RSWModels.push_back(modelInfo);
			break;
		case	2	: // Light
			rsw_stream.read_pod(lightInfo);
			info_printf(" ---------------------------------------------------------------------------------- RSW Light: %u ---------------------------------------------------------------------------------- ", loaded.RSWLights.size());
			info_printf("RSW light object found     : %s."			, &lightInfo.Name[0]);
			info_printf("RSW light object todo      : %s."			, lightInfo.ToDo);
			info_printf("RSW light object position  : {%f, %f, %f}.", lightInfo.Position	.x, lightInfo.Position	.y, lightInfo.Position	.z);
			info_printf("RSW light object color     : {%f, %f, %f}.", lightInfo.Color		.x, lightInfo.Color		.y, lightInfo.Color		.z);
			info_printf("RSW light object todo 2    : %f."			, lightInfo.ToDo2);
			loaded.RSWLights.push_back(lightInfo);
			break;
		case	3	: // Sound
			memcpy(&soundInfo, &input[rsw_stream.CursorPosition], sizeof(SSoundInfoRSW) - sizeof(float));
			rsw_stream.CursorPosition								+= sizeof(SSoundInfoRSW) - sizeof(float);
			if (header.VersionMajor >= 2)
				rsw_stream.read_pod(soundInfo.cycle);
			loaded.RSWSounds.push_back(soundInfo);
			info_printf(" ---------------------------------------------------------------------------------- RSW Sound: %u ---------------------------------------------------------------------------------- ", loaded.RSWSounds.size());
			info_printf("RSW sound object name      : %s."			, &soundInfo.name[0]	);
			info_printf("RSW sound object strUnk0   : %s."			, &soundInfo.strUnk0[0]   );
			info_printf("RSW sound object fileName  : %s."			, &soundInfo.fileName[0]	);
			info_printf("RSW sound object strUnk1   : %s."			, &soundInfo.strUnk1[0]	);
			info_printf("RSW sound object position  : {%f, %f, %f}.", soundInfo.position	.x, soundInfo.position	.y, soundInfo.position	.z);
			info_printf("RSW sound object vol       : %f."			, soundInfo.vol);
			info_printf("RSW sound object width     : %i."			, soundInfo.width);
			info_printf("RSW sound object height    : %i."			, soundInfo.height);
			info_printf("RSW sound object range     : %f."			, soundInfo.range);
			info_printf("RSW sound object cycle     : %f."			, soundInfo.cycle);
			break;
		case	4	: // Effect
			rsw_stream.read_pod(effectInfo);
			loaded.RSWEffects.push_back(effectInfo);
			info_printf(" ---------------------------------------------------------------------------------- RSW Effect: %u ---------------------------------------------------------------------------------- ", loaded.RSWEffects.size());
			info_printf("RSW effect object name     : %s."			, &effectInfo.name[0]	);
			info_printf("RSW effect object nameUnk  : %s."			, &effectInfo.nameUnk[0]   );
			info_printf("RSW effect object position : {%f, %f, %f}.", effectInfo.position	.x, effectInfo.position	.y, effectInfo.position	.z);
			info_printf("RSW effect object id       : %i."			, effectInfo.id		);
			info_printf("RSW effect object loop     : %f."			, effectInfo.loop	);
			info_printf("RSW effect object param1   : %f."			, effectInfo.param1	);
			info_printf("RSW effect object param2   : %f."			, effectInfo.param2	);
			info_printf("RSW effect object param3   : %f."			, effectInfo.param3	);
			info_printf("RSW effect object param4   : %f."			, effectInfo.param4	);
			break;
		}
	}
	info_printf("RSW model  objects loaded: %u.", loaded.RSWModels.size());
	info_printf("RSW light  objects loaded: %u.", loaded.RSWLights.size());
	info_printf("RSW sound  objects loaded: %u.", loaded.RSWSounds.size());
	info_printf("RSW effect objects loaded: %u.", loaded.RSWEffects.size());
	return rsw_stream.CursorPosition;
}

::gpk::error_t			gpk::rswFileLoad			(::gpk::SRSWFileContents& loaded, FILE * input)					{
	(void)loaded, (void)input;
	return 0;
}

::gpk::error_t			gpk::rswFileLoad			(::gpk::SRSWFileContents& loaded, const ::gpk::vcs	& input)	{
	::gpk::au8					fileInMemory				= {};
	gpk_necall(gpk::fileToMemory(input, fileInMemory), "Failed to load .rsw file: %s", input.begin());
	uint64_t					unk							= *(uint64_t*)&fileInMemory[fileInMemory.size() - 8];
	(void)unk;
	info_printf("Unk64: 0x%llX.", unk);
	info_printf("Parsing RSW file: %s.", input.begin());
	return rswFileLoad(loaded, fileInMemory);
}
