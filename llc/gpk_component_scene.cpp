#include "gpk_component_scene.h"
#include "gpk_file.h"
#include "gpk_stdstring.h"
#include "gpk_json.h"
#include "gpk_label.h"


GPK_USING_TYPEINT();
GPK_USING_VIEW();
GPK_USING_APOD();
GPK_USING_N2();
GPK_USING_N3();

tplt<tpnm _tNumber>
static	gpk::err_t jsonNumberLoad			(::gpk::SJSONReader & readerCache, ::gpk::vcst_t in_string, _tNumber & out_value){
	readerCache								= {};
	if_fail_fe(gpk::jsonParse(readerCache, in_string));
	const ::gpk::SJSONToken						& jsonValue				= *readerCache[1]->Token;
	if(readerCache[1]->Token->Type == ::gpk::JSON_TYPE_INTEGER) {
		out_value								= (_tNumber)jsonValue.Value; }
	else {
		f3_t										temp					= 0;
		memcpy(&temp, (const f3_t*)&jsonValue.Value, sizeof(f3_t));
		out_value								= (_tNumber)temp;
	}
	return 0;
}

stin	gpk::err_t 		floatRead				(::gpk::SJSONReader & readerCache, ::gpk::vcst_t in_string, f2_t	 & out_value) { return ::jsonNumberLoad(readerCache, in_string, out_value); }
stin	gpk::err_t 		integerRead				(::gpk::SJSONReader & readerCache, ::gpk::vcst_t in_string, s2_t & out_value) { return ::jsonNumberLoad(readerCache, in_string, out_value); }

static	gpk::err_t 		createFromSTL			(::gpk::SComponentScene & scene, ::gpk::SSTLFile & stlFile, ::gpk::vcst_t componentName)  {
	::gpk::SNodeRenderer						& renderer				= scene.Renderer;
	s2_t										indexNewNode			= renderer.Nodes.push_back(componentName, {});
	::gpk::SRenderNode							& newNode				= renderer.Nodes[indexNewNode];

	::gpk::SNodeRenderer::TVertexBuffer			& bufferVertices		= renderer.Vertices			[newNode.Vertices		= renderer.Vertices			.push_back(componentName, {})];
	::gpk::SNodeRenderer::TNormalBuffer			& bufferNormals			= renderer.Normals			[newNode.Normals		= renderer.Normals			.push_back(componentName, {})];
	::gpk::SNodeRenderer::TVertexColorBuffer	& bufferVertexColors	= renderer.VertexColors		[newNode.VertexColor	= renderer.VertexColors		.push_back(componentName, {})];
	::gpk::SMaterial							& material				= renderer.Materials		[newNode.Material		= renderer.Materials		.push_back(componentName, {})];
	::gpk::SRenderNodeTransform					& transform				= renderer.Transforms		[newNode.Transform		= renderer.Transforms		.push_back(componentName, {})];
	::gpk::SBodyVolumeOld						& boundingVolume		= renderer.BoundingVolumes	[newNode.BoundingVolume	= renderer.BoundingVolumes	.push_back(componentName, {})];

	transform.SetIdentity();

	material.Ambient						= ::gpk::DARKGRAY * .5	;
	material.Diffuse						= ::gpk::WHITE			;
	material.Specular						= ::gpk::GRAY			;
	material.Emissive						= ::gpk::BLACK			;
	material.SpecularFactor					= 10.0f					;

	bufferVertices		.resize(stlFile.Triangles.size() * 3);
	bufferNormals		.resize(stlFile.Triangles.size());
	bufferVertexColors	.resize(stlFile.Triangles.size());
	newNode.PerFaceColor					= 1;
	newNode.PerFaceNormal					= 1;
	for(u2_t iTriangle = 0; iTriangle < stlFile.Triangles.size(); ++iTriangle) {
		const ::gpk::SSTLTriangle					triangle				= stlFile.Triangles[iTriangle];
		bufferVertices		[iTriangle * 3 + 0]	= {triangle.Triangle.A.x, triangle.Triangle.A.z, triangle.Triangle.A.y};
		bufferVertices		[iTriangle * 3 + 2]	= {triangle.Triangle.B.x, triangle.Triangle.B.z, triangle.Triangle.B.y};
		bufferVertices		[iTriangle * 3 + 1]	= {triangle.Triangle.C.x, triangle.Triangle.C.z, triangle.Triangle.C.y};
		bufferNormals		[iTriangle]			= {triangle.Normal.x, triangle.Normal.z, triangle.Normal.y};
		bufferVertexColors	[iTriangle].FromBGR16(triangle.Attribute);
		boundingVolume.Limits.Min.x				= ::gpk::min(triangle.Triangle.A.x, ::gpk::min(triangle.Triangle.B.x, ::gpk::min(triangle.Triangle.C.x, boundingVolume.Limits.Min.x)));
		boundingVolume.Limits.Min.y				= ::gpk::min(triangle.Triangle.A.z, ::gpk::min(triangle.Triangle.B.z, ::gpk::min(triangle.Triangle.C.z, boundingVolume.Limits.Min.y)));
		boundingVolume.Limits.Min.z				= ::gpk::min(triangle.Triangle.A.y, ::gpk::min(triangle.Triangle.B.y, ::gpk::min(triangle.Triangle.C.y, boundingVolume.Limits.Min.z)));
		boundingVolume.Limits.Max.x				= ::gpk::max(triangle.Triangle.A.x, ::gpk::max(triangle.Triangle.B.x, ::gpk::max(triangle.Triangle.C.x, boundingVolume.Limits.Max.x)));
		boundingVolume.Limits.Max.y				= ::gpk::max(triangle.Triangle.A.z, ::gpk::max(triangle.Triangle.B.z, ::gpk::max(triangle.Triangle.C.z, boundingVolume.Limits.Max.y)));
		boundingVolume.Limits.Max.z				= ::gpk::max(triangle.Triangle.A.y, ::gpk::max(triangle.Triangle.B.y, ::gpk::max(triangle.Triangle.C.y, boundingVolume.Limits.Max.z)));
	}
	boundingVolume.Center					= boundingVolume.Limits.Min + (boundingVolume.Limits.Max - boundingVolume.Limits.Min) / 2.0;
	boundingVolume.Radius					= ::gpk::max(boundingVolume.Limits.Min.Length(), boundingVolume.Limits.Max.Length()) / 2;
	boundingVolume.Type						= ::gpk::BOUNDINGPRIMITIVE_TYPE_SPHERE;
	for(u2_t iVertex = 0; iVertex < bufferVertices.size(); ++iVertex)
		bufferVertices[iVertex]					-= boundingVolume.Center;

	::gpk::SComponentData						newComponentGroup		= {};
	newComponentGroup.Parent			= -1;
	newComponentGroup.RenderNodes.push_back(indexNewNode);
	return scene.Components.push_back(componentName, newComponentGroup);
}

static	gpk::err_t createFromMTL			(::gpk::SComponentScene & scene, ::gpk::vcst_t filename, ::gpk::SKeyedArrayPOD<s1_t> & indices)  {
	::gpk::string				rawMat					= {};
	::gpk::fileToMemory(filename, rawMat);

	::gpk::SJSONReader			numberReader			= {};
	::gpk::aobj<::gpk::vcst_t>	matFileLines			= {};
	::gpk::split(::gpk::vcst_t{rawMat}, '\n', matFileLines);

	s2_t						countMaterials			= 0;
	::gpk::string				materialPath			= {};
	::gpk::label				materialName			= {};
	::gpk::SMaterial			newMaterial				= {};
	for(u2_t iLine = 0; iLine < matFileLines.size(); ++iLine) {
		::gpk::vcst_t & line = matFileLines[iLine];
		::gpk::trim(line, line);
		if(0 == line.size() || line[0] == '#')
			continue;
		info_printf("Line %u: '%.*s'.", iLine, GPK_VIEW_ARGS(line));

		::gpk::aobj<::gpk::vcst_t>	lineValues				= {};
		::gpk::split(line, vcst_t{" \t"}, lineValues);
		for(u2_t iValue = 0; iValue < lineValues.size(); ++iValue) {
			::gpk::vcst_t						& value					= lineValues[iValue];
			::gpk::trim(value, value);
			if(0 == value.size())
				lineValues.remove(iValue--);
		}

		const ::gpk::vcst_t							command					= lineValues[0];
		if(command == "newmtl") {
				 if(countMaterials) {
					materialPath = filename;
					materialPath.push_back('/');
					materialPath.append(materialName);
					if_fail_fe(indices.push_back(materialName, (s1_t)scene.Renderer.Materials.push_back(::gpk::label(materialPath), newMaterial)));
				 }
				 materialName = lineValues[1];
				 ++countMaterials;
			 }
		else if(command == "Ka") {
			for(u2_t iAxis = 0; iAxis < 3; ++iAxis)
				if_fail_e(::floatRead(numberReader, lineValues[1 + iAxis], (&newMaterial.Ambient.r)[iAxis]));
			info_printf("Material Ambient: [%f, %f, %f].", newMaterial.Ambient.r, newMaterial.Ambient.g, newMaterial.Ambient.b);
		}
		else if(command == "Kd") {
			for(u2_t iAxis = 0; iAxis < 3; ++iAxis)
				if_fail_e(::floatRead(numberReader, lineValues[1 + iAxis], (&newMaterial.Diffuse.r)[iAxis]));
			info_printf("Material Diffuse: [%f, %f, %f].", newMaterial.Diffuse.r, newMaterial.Diffuse.g, newMaterial.Diffuse.b);
		}
		else if(command == "Ks") {
			for(u2_t iAxis = 0; iAxis < 3; ++iAxis)
				if_fail_e(::floatRead(numberReader, lineValues[1 + iAxis], (&newMaterial.Diffuse.r)[iAxis]));
			info_printf("Material Diffuse: [%f, %f, %f].", newMaterial.Diffuse.r, newMaterial.Diffuse.g, newMaterial.Diffuse.b);
		}
		else if(command == "d") {
			if_fail_e(::floatRead(numberReader, lineValues[1], newMaterial.Transparency));
			info_printf("Material Transparency: %f.", newMaterial.Transparency);
		}
		else if(command == "Ns") {
			if_fail_e(::floatRead(numberReader, lineValues[1], newMaterial.Transparency));
			info_printf("Material Specular factor: %f.", newMaterial.SpecularFactor);
		}
		else if(command == "illum") { }
	}
	if(countMaterials) {
		materialPath = filename;
		materialPath.push_back('/');
		materialPath.append(materialName);
		indices.push_back(::gpk::label(materialName), (s1_t)scene.Renderer.Materials.push_back(::gpk::label(::gpk::vcst_t{materialPath}), newMaterial));
	}
	return 0;
}

static	gpk::err_t createFromOBJ			(::gpk::SComponentScene & scene, ::gpk::vcst_t filename)  {
	::gpk::string							rawObj					= {};
	if_fail_fef(gpk::fileToMemory(filename, rawObj), "Failed to load OBJ file: %.*s.", GPK_VIEW_ARGS(filename));

	::gpk::SKeyedArrayPOD<s1_t>				materialIndices			= {};

	::gpk::SComponentData						newObject				= {};
	::gpk::SComponentData						newGroup				= {};
	u2_t									countObjects			= 0;
	u2_t									countGroups				= 0;
	::gpk::apod<s1_t>						objectIndices			= {};
	::gpk::string							objectPath				= {};
	::gpk::label								objectName				= {};
	::gpk::label								groupName				= {};
	::gpk::string							groupPath				= {};


	::gpk::aobj<::gpk::vcst_t>						objFileLines			= {};
	::gpk::split(::gpk::vcst_t{rawObj}, '\n', objFileLines);
	::gpk::SJSONReader							numberReader			= {};
	info_printf("%s", "Preloading materials...");
	for(u2_t iLine = 0; iLine < objFileLines.size(); ++iLine) {
		::gpk::vcst_t						& line					= objFileLines[iLine];
		::gpk::trim(line, line);
		if(0 == line.size() || line[0] == '#')
			continue;
		info_printf("Line %u: '%s'.", iLine, GPK_VIEW_ARGS(line));
		::gpk::aobj<::gpk::vcst_t>	lineValues				= {};
		::gpk::split(line, ::gpk::vcst_t{" \t"}, lineValues);

		for(u2_t iValue = 0; iValue < lineValues.size(); ++iValue) {
			::gpk::vcst_t						& value					= lineValues[iValue];
			::gpk::ltrim(value, value);
			::gpk::rtrim(value, value);
			if(0 == value.size())
				lineValues.remove(iValue--);
		}
		const ::gpk::vcst_t							command					= lineValues[0];
		if(command == "mtllib") {
			::gpk::string					matFilename				= {};
			s2_t										pathStop				= ::gpk::rfind('/', filename);
			if(pathStop == -1) {
				matFilename.append_string("./");
				matFilename.append_string(lineValues[1]);
			}
			else {
				::gpk::vcst_t						filepath				= {};
				if_fail_fe(filename.slice(filepath, 0, pathStop));
				matFilename.append(filepath);
				matFilename.push_back('/');
				matFilename.append(lineValues[1]);
			}
			if_fail_ef(::createFromMTL(scene, matFilename, materialIndices), "Failed to load material file '.*s'.", GPK_VIEW_ARGS(matFilename));
		}
	}

	info_printf("%s", "Loading geometries...");
	for(u2_t iLine = 0; iLine < objFileLines.size(); ++iLine) {
		::gpk::vcst_t						& line					= objFileLines[iLine];
		::gpk::trim(line, line);
		if(0 == line.size() || line[0] == '#')
			continue;
		info_printf("Line %u: '%s'.", iLine, GPK_VIEW_ARGS(line));

		::gpk::aobj<::gpk::vcst_t>	lineValues				= {};
		::gpk::split(line, ::gpk::vcst_t{" \t"}, lineValues);
		for(u2_t iValue = 0; iValue < lineValues.size(); ++iValue) {
			::gpk::vcst_t						& value					= lineValues[iValue];
			::gpk::ltrim(value, value);
			::gpk::rtrim(value, value);
			if(0 == value.size())
				lineValues.remove(iValue--);
		}
		const ::gpk::vcst_t							command					= lineValues[0];
		if(command == "o") {
			if(countObjects) {
				objectPath								= filename;
				objectPath.push_back('/');
				objectPath.append(objectName);
				objectIndices.push_back((s1_t)scene.Components.push_back(::gpk::label(objectPath), newObject));
			}
			objectName								= lineValues[1];
			++countObjects;
			info_printf("Object #%i found: '%.*s'.", countObjects, GPK_VIEW_ARGS(objectName));
		}
		else if(command == "g") {
			if(countGroups) {
				groupPath								= filename;
				groupPath.push_back('/');
				groupPath.append(groupName);
				objectIndices.push_back((s1_t)scene.Components.push_back(::gpk::label(::gpk::vcst_t{groupPath}), newGroup));
			}
			groupName								= lineValues[1];
			newGroup.RenderNodes.push_back(scene.Renderer.Nodes.push_back(groupName, {}));
			++countGroups;
			info_printf("Group #%i found: '%.*s'.", countGroups, GPK_VIEW_ARGS(groupName));
		}
		else if(command == "usemtl") {
			::gpk::vcst_t									materialName	= lineValues[1];
			info_printf("Setting material: %.*s", GPK_VIEW_ARGS(materialName));
			for(u2_t iMaterial = 0; iMaterial < materialIndices.size(); ++iMaterial)
				if(materialIndices.Names[iMaterial] == materialName) {
					info_printf("Setting material: %.*s", GPK_VIEW_ARGS(materialName));
					scene.Renderer.Nodes[newGroup.RenderNodes[0]].Material = materialIndices.Values[iMaterial];
					break;
				}
		}
		else if(command == "v") {
			::gpk::n3f2_t						vertex			= {};
			for(u2_t iAxis = 0; iAxis < 3; ++iAxis)
				if_fail_e(::floatRead(numberReader, lineValues[1 + iAxis], (&vertex.x)[iAxis]));
			info_printf("Vertex found: [%f, %f, %f].", vertex.x, vertex.y, vertex.z);
			//scene.Renderer.Vertices[scene.Renderer.Nodes[newGroup.RenderNodes[0]].Vertices].push_back(vertex);
		}
		else if(command == "vt") {
			::gpk::n2f2_t						texCoord		= {};
			for(u2_t iAxis = 0; iAxis < 2; ++iAxis)
				if_fail_e(::floatRead(numberReader, lineValues[1 + iAxis], (&texCoord.x)[iAxis]));
			info_printf("TexCoord found: [%f, %f, %f].", texCoord.x, texCoord.y);
		}
		else if(command == "vn") {
			::gpk::n3f2_t						vertex			= {};
			for(u2_t iAxis = 0; iAxis < 3; ++iAxis)
				if_fail_e(::floatRead(numberReader, lineValues[1 + iAxis], (&vertex.x)[iAxis]));
			info_printf("Normal found: [%f, %f, %f].", vertex.x, vertex.y, vertex.z);
		}
		else if(command == "f") {
			::gpk::n3s2_t						indices			= {};
			for(u2_t iAxis = 0; iAxis < 3; ++iAxis)
				if_fail_e(::integerRead(numberReader, lineValues[1 + iAxis], (&indices.x)[iAxis]));
			info_printf("Face found: [%i, %i, %i].", indices.x, indices.y, indices.z);
		}
		else if(command == "vp") {

		}
	}
	return 0;
}

gpk::err_t			gpk::SComponentScene::CreateFromFile		(vcst_t filename)		{
	vcst_t				extension				= {};
	string				ext_lwr					= {};
	if_fail_fef(filename.slice(extension, filename.size() - 4, 4), "File extension not supported for file '%.*s'", GPK_VIEW_ARGS(filename));
	if_fail_fe(ext_lwr.append(extension));
	tolower(ext_lwr);
	stxp	vcst_c			str_stl			= ".stl";
	stxp	vcst_c			str_obj			= ".obj";
	if(ext_lwr == str_stl) {
		SSTLFile							stlFile					= {};
		if_fail_fef(stlFileLoad(filename, stlFile), "Failed to load file '%.*s'.", GPK_VIEW_ARGS(filename));
		return ::createFromSTL(*this, stlFile, filename);
	}
	else if(ext_lwr == str_obj) {
		return ::createFromOBJ(*this, filename);
	}
	else {
		error_printf("File format not supported for file '%.*s'", GPK_VIEW_ARGS(filename));
		return -1;
	}
	return 0;
}
