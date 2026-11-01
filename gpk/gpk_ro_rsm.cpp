#include "gpk_ro_rsm.h"
#include "gpk_matrix.h"
#include "gpk_view_stream.h"
#include "gpk_io.h"
#include "gpk_file.h"

GPK_USING_TYPEINT();

#pragma pack(push, 1)
struct SRSMHeader {	// Represents the byte layout fou
	sc_t										filecode[4];
	u0_t										versionMajor;
	u0_t										versionMinor;
};
#pragma pack(pop)


			gpk::err_t 							analyzeArray								(const ::gpk::view<u0_t>& input) {
	info_printf("%s", "---- Analyzing bytes     :");			for(u2_t iChar = 0; iChar < input.size() / 1; ++iChar)			info_printf("'%c' : %.4u : %.4i : 0x%x"	, input[iChar] ? input[iChar] : ' ', (u2_t)input[iChar], (s2_t)((s0_t*)input.begin())[iChar], input[iChar]);

	info_printf("%s", "---- Analyzing shorts    :");			for(u2_t iChar = 0; iChar < input.size() / 2; ++iChar)			info_printf(  "%.6u : %.6i : 0x%.4x"	, ((u1_t	*)&input[0])[iChar], (s2_t)((s1_t	*)input.begin())[iChar], ((u1_t	*)input.begin())[iChar]);
	info_printf("%s", "---- Analyzing ints      :");			for(u2_t iChar = 0; iChar < input.size() / 4; ++iChar)			info_printf("%.12u : %.12i : 0x%.8x"	, ((u2_t	*)&input[0])[iChar], ((s2_t			*)input.begin())[iChar], ((u2_t	*)input.begin())[iChar]);
	info_printf("%s", "---- Analyzing floats    :");			for(u2_t iChar = 0; iChar < input.size() / 4; ++iChar)			info_printf("%.12f"						, ((f2_t		*)&input[0])[iChar]);

	info_printf("%s", "---- Analyzing shifted shorts (1):");	for(u2_t iChar = 0; iChar < (input.size() - 1) / 2; ++iChar)	info_printf(  "%.6u : %.6i : 0x%.4x"	, ((u1_t	*)&input[1])[iChar], (s2_t)((s1_t	*)&input[1])[iChar], ((u1_t	*)&input[1])[iChar]);
	info_printf("%s", "---- Analyzing shifted ints   (1):");	for(u2_t iChar = 0; iChar < (input.size() - 1) / 4; ++iChar)	info_printf("%.12u : %.12i : 0x%.8x"	, ((u2_t	*)&input[1])[iChar], ((s2_t			*)&input[1])[iChar], ((u2_t	*)&input[1])[iChar]);
	info_printf("%s", "---- Analyzing shifted floats (1):");	for(u2_t iChar = 0; iChar < (input.size() - 1) / 4; ++iChar)	info_printf("%.12f : %.12f"				, ((f2_t		*)&input[1])[iChar], ((f2_t			*)&input[1])[iChar]);

	info_printf("%s", "---- Analyzing shifted ints   (2):");	for(u2_t iChar = 0; iChar < (input.size() - 2) / 4; ++iChar)	info_printf("%.12u : %.12i : 0x%.8x"	, ((u2_t	*)&input[2])[iChar], ((s2_t			*)&input[2])[iChar], ((u2_t	*)&input[2])[iChar]);
	info_printf("%s", "---- Analyzing shifted floats (2):");	for(u2_t iChar = 0; iChar < (input.size() - 2) / 4; ++iChar)	info_printf("%.12f : %.12f"				, ((f2_t		*)&input[2])[iChar], ((f2_t			*)&input[2])[iChar]);

	info_printf("%s", "---- Analyzing shifted ints   (3):");	for(u2_t iChar = 0; iChar < (input.size() - 3) / 4; ++iChar)	info_printf("%.12u : %.12i : 0x%.8x"	, ((u2_t	*)&input[3])[iChar], ((s2_t			*)&input[3])[iChar], ((u2_t	*)&input[3])[iChar]);
	info_printf("%s", "---- Analyzing shifted floats (3):");	for(u2_t iChar = 0; iChar < (input.size() - 3) / 4; ++iChar)	info_printf("%.12f : %.12f"				, ((f2_t		*)&input[3])[iChar], ((f2_t			*)&input[3])[iChar]);
	return 0;
}

// Read rotation keyframes. Not sure how they're expressed though.
static	gpk::err_t rsmReadRotationKeyframes									(::gpk::view_stream<const u0_t>& rsm_stream, ::gpk::apod<::gpk::SRSMFrameRotation> & modelKeyframes)	{
	u2_t					keyframeCount												= 0;			// Get the number of keyframe
	rsm_stream.read_pod(keyframeCount);
	info_printf("Rotation keyframe count: %u.", keyframeCount);
	if(keyframeCount) {
		modelKeyframes.resize(keyframeCount);
		if_fail_fe(rsm_stream.read_pod(modelKeyframes));
	}
	return 0;
}

// >= v1.5 Read position keyframes
static	gpk::err_t rsmReadPositionKeyframes									(::gpk::view_stream<const u0_t>& rsm_stream, ::gpk::apod<::gpk::SRSMFramePosition> & modelKeyframes)	{
	u2_t					positionFrameCount											= 0;
	if_fail_fe(rsm_stream.read_pod(positionFrameCount));
	if(positionFrameCount) {
		modelKeyframes.resize(positionFrameCount);
		if_fail_fe(rsm_stream.read_pod(modelKeyframes));
	}
	return 0;
}

gpk::err_t 		gpk::rsmFileLoad											(::gpk::SRSMFileContents & loaded, const ::gpk::view<u0_t>	& input)							{
	::gpk::view_stream<const u0_t>							rsm_stream													= {input.begin(), input.size()};
	SRSMHeader													header														= {};
	rsm_stream.read_pod(header);
	rsm_stream.read_pod(loaded.AnimLength);
	rsm_stream.read_pod(loaded.ShadeType);
	if(header.versionMajor > 1 || (header.versionMajor == 1 && header.versionMinor >= 4))
		rsm_stream.read_pod(loaded.Alpha);

	rsm_stream.read_pod(loaded.Unknown);

	u2_t					textureCount												= 0;			// Get the number of textures
	rsm_stream.read_pod(textureCount);

	info_printf("RSM magic number   : %.4s."	, header.filecode);
	info_printf("RSM version        : %u.%u."	, header.versionMajor, header.versionMinor);
	info_printf("RSM animation time : %u."		, (u2_t)loaded.AnimLength);
	info_printf("RSM shade type     : %u."		, (u2_t)loaded.ShadeType);
	info_printf("RSM alpha          : %u."		, (u2_t)loaded.Alpha);
\
	loaded.TextureNames.resize(textureCount);
	for(u2_t iTex = 0; iTex < textureCount; ++iTex) {
		rsm_stream.read_pod(loaded.TextureNames[iTex].Storage);
		info_printf("Texture %i name: %s.", (s2_t)iTex, loaded.TextureNames[iTex].Storage);
	}

	rsm_stream.read_pod(loaded.RootNodeName);

	s2_t														meshCountIBelieve											= 1;
	rsm_stream.read_pod(meshCountIBelieve);

	u2_t					totalVertices												= 0;
	u2_t					totalUVs													= 0;
	u2_t					totalFaces													= 0;
	u2_t					byteOffsetStartModel										= rsm_stream.CursorPosition;

	loaded.Nodes.resize(meshCountIBelieve);
	for(s2_t iMesh = 0; iMesh < meshCountIBelieve; ++iMesh) {
		::gpk::SRSMNode												& newNode													= loaded.Nodes[iMesh];
		rsm_stream.read_pod(newNode.Name		);
		rsm_stream.read_pod(newNode.ParentName	);
		info_printf("---------------------------------------------- Reading mesh node: %u ----------------------------------------------", (u2_t)iMesh);
		info_printf("Mesh node name: %s."	, newNode.Name			);
		info_printf("Parent node name: %s."	, newNode.ParentName	);
		{
			u2_t					texMappingCount												= 0;			// Get the number of texture indices for this model
			rsm_stream.read_pod(texMappingCount);
			info_printf("Texture index count: %u.", texMappingCount);
			if(texMappingCount) {
				::gpk::apod<s2_t>									& modelTextures												= newNode.TextureIndices;
				modelTextures.resize(texMappingCount);
				rsm_stream.read_pod(modelTextures);
			}
		}

		rsm_stream.read_pod(newNode.Transform);
		info_printf("Node transform (Row0	): {%f, %f, %f}."		, newNode.Transform.Row0		.x, newNode.Transform.Row0			.y, newNode.Transform.Row0			.z);
		info_printf("Node transform (Row1	): {%f, %f, %f}."		, newNode.Transform.Row1		.x, newNode.Transform.Row1			.y, newNode.Transform.Row1			.z);
		info_printf("Node transform (Row2	): {%f, %f, %f}."		, newNode.Transform.Row2		.x, newNode.Transform.Row2			.y, newNode.Transform.Row2			.z);
		info_printf("Node offset             : {%f, %f, %f}."		, newNode.Transform.Offset		.x, newNode.Transform.Offset		.y, newNode.Transform.Offset		.z);
		info_printf("Node translation        : {%f, %f, %f}."		, newNode.Transform.Translation	.x, newNode.Transform.Translation	.y, newNode.Transform.Translation	.z);
		info_printf("Node rotation value     : %f."					, newNode.Transform.Rotation);
		info_printf("Node rotation axis      : {%f, %f, %f}."		, newNode.Transform.RotAxis		.x, newNode.Transform.RotAxis		.y, newNode.Transform.RotAxis		.z);
		info_printf("Node scale              : {%f, %f, %f}."		, newNode.Transform.Scale		.x, newNode.Transform.Scale			.y, newNode.Transform.Scale			.z);
		{ // Read vertices (positions)
			u2_t					vertexCount													= 0;			// Get the number of vertex
			rsm_stream.read_pod(vertexCount);
			info_printf("Vertex count: %u.", vertexCount);
			if(vertexCount) {
				::gpk::apod<::gpk::n3f2_t>						& modelVertices												= newNode.Vertices;
				modelVertices.resize(vertexCount);
				rsm_stream.read_pod(modelVertices);
			}
		}
		{ // Read vertex attributes (texCoord & ...color?)
			u2_t					texVtxCount													= 0;			// Get the number of unk
			rsm_stream.read_pod(texVtxCount);
			info_printf("UV coord count: %u.", texVtxCount);
			if(texVtxCount) {
				::gpk::apod<::gpk::SRSMTexCoord>						& modelUNKs													= newNode.UVs;
				modelUNKs.resize(texVtxCount);
				if((header.versionMajor == 1 && header.versionMinor >= 2) || header.versionMajor > 1) //{ >= v1.2
					rsm_stream.read_pod(modelUNKs);
				else {
					for( u2_t iVertex = 0; iVertex < texVtxCount; ++iVertex ) {
						::gpk::SRSMTexCoord											& curTexCoord												= modelUNKs[iVertex];
						curTexCoord.Unknown										= (u2_t)-1;
						rsm_stream.read_pod(curTexCoord.UV);
					}
				}
			}
		}
		//1C1SQJL_esAR795AR795
		{ // Read Triangle descriptions (vertex/UV indices, texture and other properties)
			u2_t					faceCount													= 0;			// Get the number of face
			rsm_stream.read_pod(faceCount);
			info_printf("Face count: %u.", faceCount);
			if(faceCount) {
				::gpk::apod<SRSMFace>									& modelFaces												= newNode.Faces;
				modelFaces.resize(faceCount);
				if((header.versionMajor == 1 && header.versionMinor >= 2) || (header.versionMajor > 1))
					rsm_stream.read_pod(modelFaces);
				else {
					for(u2_t iFace = 0; iFace < faceCount; ++iFace) {
						::gpk::SRSMFace												& currentFace												= modelFaces[iFace];
						rsm_stream.read_pod(currentFace.Vertices			);
						rsm_stream.read_pod(currentFace.UVs					);
						rsm_stream.read_pod(currentFace.IndexTextureIndex	);
						rsm_stream.read_pod(currentFace.todo1				);
						rsm_stream.read_pod(currentFace.TwoSided			);
						currentFace.SmoothGroup									= -1;	// This isn't defined on version < 1.2
					}
				}
			}
		}

		if((header.versionMajor == 1 && header.versionMinor >= 5) || (header.versionMajor > 1))
			rsmReadPositionKeyframes(rsm_stream, newNode.PositionKeyframes);

		rsmReadRotationKeyframes(rsm_stream, newNode.RotationKeyframes);

		totalVertices											+= newNode.Vertices	.size();
		totalUVs												+= newNode.UVs		.size();
		totalFaces												+= newNode.Faces	.size();
	}
	info_printf("Total Vertices        : %u.", totalVertices	);
	info_printf("Total UVs             : %u.", totalUVs			);
	info_printf("Total Faces           : %u.", totalFaces		);
	(void)byteOffsetStartModel;
	info_printf("Total Node bytes read : %u.", (rsm_stream.CursorPosition - byteOffsetStartModel));
	return rsm_stream.CursorPosition;
}

gpk::err_t 		gpk::rsmFileLoad											(::gpk::SRSMFileContents & loaded, FILE								* input)							{
	(void)loaded, (void)input;
	return 0;
}

gpk::err_t 		gpk::rsmFileLoad											(::gpk::SRSMFileContents & loaded, const ::gpk::vcst_t	& input)							{
	::gpk::au0_t				fileInMemory												= {};
	if_fail_fef(gpk::fileToMemory(input, fileInMemory), "Failed to load .rsw file: %s", input.begin());
	u3_t													unk															= *(u3_t*)&fileInMemory[fileInMemory.size() - 8];
	(void)unk;
	info_printf("%u", unk);
	info_printf("Parsing RSM file: %s.", input.begin());
	return rsmFileLoad(loaded, fileInMemory);
}

gpk::err_t 		gpk::rsmGeometryGenerate					(const ::gpk::SRSMFileContents& rsmData, ::gpk::view<::gpk::SModelNodeRSM>& out_meshes)			{
	for(u2_t iTex = 0, texCount = rsmData.TextureNames.size(); iTex < texCount; ++iTex) {
		for(u2_t iNode = 0, countNodes = rsmData.Nodes.size(); iNode < countNodes; ++iNode) {
			const ::gpk::SRSMNode										& rsmNode													= rsmData.Nodes[iNode];
			::gpk::SModelNodeRSM										& meshNode													= out_meshes[iTex * countNodes + iNode];
			meshNode.RSMNodeIndex									= iNode;
			meshNode.TextureIndex									= iTex;
			u2_t					vertexOffset												= 0;
			for(u2_t iFace = 0, countFaces = rsmNode.Faces.size(); iFace < countFaces; ++iFace) {
				const ::gpk::SRSMFace										& face														= rsmNode.Faces[iFace];
				if(rsmNode.TextureIndices[face.IndexTextureIndex] != (s2_t)iTex)
					continue;
				for(u2_t iVert = 0; iVert < 3; ++iVert) {
					if_fail_fef(meshNode.UVs				.push_back(rsmNode.UVs		[face.UVs		[iVert]].UV				), "%s", "Out of memory. Invalid RSM input?");
					if_fail_fef(meshNode.Vertices		.push_back(rsmNode.Vertices	[face.Vertices	[iVert]]				), "%s", "Out of memory. Invalid RSM input?");
				}
				if_fail_fef(meshNode.Normals			.push_back({ (rsmNode.Vertices	[face.Vertices	[1]] - rsmNode.Vertices	[face.Vertices	[0]]).Cross(rsmNode.Vertices	[face.Vertices	[2]] - rsmNode.Vertices	[face.Vertices	[0]]).Normalize() }), "%s", "Out of memory. Invalid RSM input?");
				if_fail_fef(meshNode.Normals			.push_back({ (rsmNode.Vertices	[face.Vertices	[1]] - rsmNode.Vertices	[face.Vertices	[0]]).Cross(rsmNode.Vertices	[face.Vertices	[2]] - rsmNode.Vertices	[face.Vertices	[0]]).Normalize() }), "%s", "Out of memory. Invalid RSM input?");
				if_fail_fef(meshNode.Normals			.push_back({ (rsmNode.Vertices	[face.Vertices	[1]] - rsmNode.Vertices	[face.Vertices	[0]]).Cross(rsmNode.Vertices	[face.Vertices	[2]] - rsmNode.Vertices	[face.Vertices	[0]]).Normalize() }), "%s", "Out of memory. Invalid RSM input?");
				if_fail_fef(meshNode.VertexIndices	.push_back({vertexOffset, vertexOffset + 1, vertexOffset + 2}), "%s", "Out of memory. Invalid RSM input?");
				vertexOffset											+= 3;
			}
		}
	}
	return 0;
}
