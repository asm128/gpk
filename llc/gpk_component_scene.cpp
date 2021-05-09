#include "gpk_component_scene.h"
#include "gpk_storage.h"
#include "gpk_stdstring.h"
#include "gpk_json.h"

template<typename _tNumber>
static ::gpk::error_t					jsonNumberLoad			(::gpk::SJSONReader & readerCache, ::gpk::vcc in_string, _tNumber & out_value){
	readerCache								= {};
	gpk_necall(::gpk::jsonParse(readerCache, in_string), "%s", "");
	const ::gpk::SJSONToken						& jsonValue				= *readerCache[1]->Token;
	if(readerCache[1]->Token->Type == ::gpk::JSON_TYPE_INTEGER) {
		out_value								= (_tNumber)jsonValue.Value; }
	else {
		double										temp					= 0;
		memcpy(&temp, (const double*)&jsonValue.Value, sizeof(double));
		out_value								= (_tNumber)temp;
	}
	return 0;
}

static inline	::gpk::error_t			floatRead				(::gpk::SJSONReader & readerCache, ::gpk::vcc in_string, float	 & out_value) { return ::jsonNumberLoad(readerCache, in_string, out_value); }
static inline	::gpk::error_t			integerRead				(::gpk::SJSONReader & readerCache, ::gpk::vcc in_string, int32_t & out_value) { return ::jsonNumberLoad(readerCache, in_string, out_value); }

static ::gpk::error_t					createFromSTL			(::gpk::SComponentScene & scene, ::gpk::SSTLFile & stlFile, ::gpk::view_const_string componentName)  {
	::gpk::SNodeRenderer						& renderer				= scene.Renderer;
	int32_t										indexNewNode			= renderer.Nodes.push_back(componentName, {});
	::gpk::SRenderNode							& newNode				= renderer.Nodes[indexNewNode];

	::gpk::SNodeRenderer::TVertexBuffer			& bufferVertices		= renderer.Vertices			[newNode.Vertices		= renderer.Vertices			.push_back(componentName, {})];
	::gpk::SNodeRenderer::TNormalBuffer			& bufferNormals			= renderer.Normals			[newNode.Normals		= renderer.Normals			.push_back(componentName, {})];
	::gpk::SNodeRenderer::TVertexColorBuffer	& bufferVertexColors	= renderer.VertexColors		[newNode.VertexColor	= renderer.VertexColors		.push_back(componentName, {})];
	::gpk::SMaterial							& material				= renderer.Materials		[newNode.Material		= renderer.Materials		.push_back(componentName, {})];
	::gpk::SRenderNodeTransform					& transform				= renderer.Transforms		[newNode.Transform		= renderer.Transforms		.push_back(componentName, {})];
	::gpk::SBoundingVolume						& boundingVolume		= renderer.BoundingVolumes	[newNode.BoundingVolume	= renderer.BoundingVolumes	.push_back(componentName, {})];

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
	for(uint32_t iTriangle = 0; iTriangle < stlFile.Triangles.size(); ++iTriangle) {
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
	boundingVolume.Center					= boundingVolume.Limits.Min + (boundingVolume.Limits.Max - boundingVolume.Limits.Min) / 2;
	boundingVolume.Radius					= ::gpk::max(boundingVolume.Limits.Min.Length(), boundingVolume.Limits.Max.Length()) / 2;
	boundingVolume.Type						= ::gpk::BOUNDINGPRIMITIVE_TYPE_SPHERE;
	for(uint32_t iVertex = 0; iVertex < bufferVertices.size(); ++iVertex)
		bufferVertices[iVertex]					-= boundingVolume.Center;

	::gpk::SComponentData						newComponentGroup		= {};
	newComponentGroup.IndexParent			= -1;
	newComponentGroup.RenderNodes.push_back(indexNewNode);
	return scene.Components.push_back(componentName, newComponentGroup);
}

static ::gpk::error_t					createFromMTL			(::gpk::SComponentScene & scene, ::gpk::view_const_char filename, ::gpk::SKeyedArrayPOD<int16_t> & indices)  {
	::gpk::array_pod<byte_t>					rawMat					= {};
	::gpk::fileToMemory(filename, rawMat);

	::gpk::SJSONReader							numberReader			= {};
	::gpk::array_obj<::gpk::view_const_char>	matFileLines			= {};
	::gpk::split(::gpk::vcc{rawMat}, '\n', matFileLines);

	int32_t										countMaterials			= 0;
	::gpk::array_pod<char_t>					materialPath			= {};
	::gpk::label								materialName			= {};
	::gpk::SMaterial							newMaterial				= {};
	for(uint32_t iLine = 0; iLine < matFileLines.size(); ++iLine) {
		::gpk::view_const_char & line = matFileLines[iLine];
		::gpk::trim(line, line);
		if(0 == line.size() || line[0] == '#')
			continue;
		info_printf("Line %u: '%s'.", iLine, ::gpk::toString(line).begin());

		::gpk::array_obj<::gpk::view_const_char>	lineValues				= {};
		::gpk::split(line, ::gpk::vcs{" \t"}, lineValues);
		for(uint32_t iValue = 0; iValue < lineValues.size(); ++iValue) {
			::gpk::view_const_char						& value					= lineValues[iValue];
			::gpk::trim(value, value);
			if(0 == value.size())
				lineValues.remove(iValue--);
		}

		const ::gpk::vcc							command					= lineValues[0];
		if(command == ::gpk::vcs{"newmtl"}) {
				 if(countMaterials) {
					materialPath = filename;
					materialPath.push_back('/');
					materialPath.append(materialName);
					indices.push_back(materialName, (int16_t)scene.Renderer.Materials.push_back(::gpk::label(::gpk::vcc{materialPath}), newMaterial));
				 }
				 materialName = {lineValues[1].begin(), lineValues[1].size()};
				 ++countMaterials;
			 }
		else if(command == ::gpk::vcs{"Ka"		}) {
			for(uint32_t iAxis = 0; iAxis < 3; ++iAxis)
				e_if(errored(::floatRead(numberReader, lineValues[1 + iAxis], (&newMaterial.Ambient.r)[iAxis])), "%s", "");
			info_printf("Material Ambient: [%f, %f, %f].", newMaterial.Ambient.r, newMaterial.Ambient.g, newMaterial.Ambient.b);
		}
		else if(command == ::gpk::vcs{"Kd"		}) {
			for(uint32_t iAxis = 0; iAxis < 3; ++iAxis)
				e_if(errored(::floatRead(numberReader, lineValues[1 + iAxis], (&newMaterial.Diffuse.r)[iAxis])), "%s", "");
			info_printf("Material Diffuse: [%f, %f, %f].", newMaterial.Diffuse.r, newMaterial.Diffuse.g, newMaterial.Diffuse.b);
		}
		else if(command == ::gpk::vcs{"Ks"		}) {
			for(uint32_t iAxis = 0; iAxis < 3; ++iAxis)
				e_if(errored(::floatRead(numberReader, lineValues[1 + iAxis], (&newMaterial.Diffuse.r)[iAxis])), "%s", "");
			info_printf("Material Diffuse: [%f, %f, %f].", newMaterial.Diffuse.r, newMaterial.Diffuse.g, newMaterial.Diffuse.b);
		}
		else if(command == ::gpk::vcs{"d"		}) {
			e_if(errored(::floatRead(numberReader, lineValues[1], newMaterial.Transparency)), "%s", "");
			info_printf("Material Transparency: %f.", newMaterial.Transparency);
		}
		else if(command == ::gpk::vcs{"Ns"		}) {
			e_if(errored(::floatRead(numberReader, lineValues[1], newMaterial.Transparency)), "%s", "");
			info_printf("Material Specular factor: %f.", newMaterial.SpecularFactor);
		}
		else if(command == ::gpk::vcs{"illum"	}) { }
	}
	if(countMaterials) {
		materialPath = filename;
		materialPath.push_back('/');
		materialPath.append(materialName);
		indices.push_back(::gpk::label(materialName), (int16_t)scene.Renderer.Materials.push_back(::gpk::label(::gpk::vcc{materialPath}), newMaterial));
	}
	return 0;
}

static ::gpk::error_t					createFromOBJ			(::gpk::SComponentScene & scene, ::gpk::view_const_char filename)  {
	::gpk::array_pod<byte_t>					rawObj					= {};
	::gpk::fileToMemory(filename, rawObj);

	::gpk::SKeyedArrayPOD<int16_t>				materialIndices			= {};

	::gpk::SComponentData						newObject				= {};
	::gpk::SComponentData						newGroup				= {};
	uint32_t									countObjects			= 0;
	uint32_t									countGroups				= 0;
	::gpk::array_pod<int16_t>					objectIndices			= {};
	::gpk::array_pod<char_t>					objectPath				= {};
	::gpk::label								objectName				= {};
	::gpk::label								groupName				= {};
	::gpk::array_pod<char_t>					groupPath				= {};


	::gpk::array_obj<::gpk::view_const_char>	objFileLines			= {};
	::gpk::split(::gpk::vcc{rawObj}, '\n', objFileLines);
	::gpk::SJSONReader							numberReader			= {};
	info_printf("%s", "Preloading materials...");
	for(uint32_t iLine = 0; iLine < objFileLines.size(); ++iLine) {
		::gpk::view_const_char						& line					= objFileLines[iLine];
		::gpk::trim(line, line);
		if(0 == line.size() || line[0] == '#')
			continue;
		info_printf("Line %u: '%s'.", iLine, ::gpk::toString(line).begin());
		::gpk::array_obj<::gpk::view_const_char>	lineValues				= {};
		::gpk::split(line, ::gpk::vcs{" \t"}, lineValues);

		for(uint32_t iValue = 0; iValue < lineValues.size(); ++iValue) {
			::gpk::view_const_char						& value					= lineValues[iValue];
			::gpk::ltrim(value, value);
			::gpk::rtrim(value, value);
			if(0 == value.size())
				lineValues.remove(iValue--);
		}
		const ::gpk::vcc							command					= lineValues[0];
		if(command == ::gpk::vcs{"mtllib"}) {
			::gpk::array_pod<char_t>					matFilename				= {};
			int32_t										pathStop				= ::gpk::rfind('/', filename);
			if(pathStop == -1) {
				matFilename.append_string("./");
				matFilename.append_string(lineValues[1]);
			}
			else {
				::gpk::view_const_char						filepath				= {};
				gpk_necall(filename.slice(filepath, 0, pathStop), "%s", "");
				matFilename.append(filepath);
				matFilename.push_back('/');
				matFilename.append(lineValues[1]);
			}
			e_if(::createFromMTL(scene, matFilename, materialIndices), "Failed to load material file '%s'.", ::gpk::toString(matFilename).begin());
		}
	}

	for(uint32_t iLine = 0; iLine < objFileLines.size(); ++iLine) {
		::gpk::view_const_char						& line					= objFileLines[iLine];
		::gpk::trim(line, line);
		if(0 == line.size() || line[0] == '#')
			continue;
		info_printf("Line %u: '%s'.", iLine, ::gpk::toString(line).begin());

		::gpk::array_obj<::gpk::view_const_char>	lineValues				= {};
		::gpk::split(line, ::gpk::vcs{" \t"}, lineValues);
		for(uint32_t iValue = 0; iValue < lineValues.size(); ++iValue) {
			::gpk::view_const_char						& value					= lineValues[iValue];
			::gpk::ltrim(value, value);
			::gpk::rtrim(value, value);
			if(0 == value.size())
				lineValues.remove(iValue--);
		}
		const ::gpk::vcc							command					= lineValues[0];
		if(command == ::gpk::vcs{"o"}) {
			if(countObjects) {
				objectPath								= filename;
				objectPath.push_back('/');
				objectPath.append(objectName);
				objectIndices.push_back((int16_t)scene.Components.push_back(::gpk::label(::gpk::vcc{objectPath}), newObject));
			}
			objectName								= {lineValues[1].begin(), lineValues[1].size()};
			++countObjects;
			info_printf("Object #%i found: '%s'.", countObjects, ::gpk::toString(objectName).begin());
		}
		else if(command == ::gpk::vcs{"g"}) {
			if(countGroups) {
				groupPath								= filename;
				groupPath.push_back('/');
				groupPath.append(groupName);
				objectIndices.push_back((int16_t)scene.Components.push_back(::gpk::label(::gpk::vcc{groupPath}), newGroup));
			}
			groupName								= {lineValues[1].begin(), lineValues[1].size()};
			newGroup.RenderNodes.push_back(scene.Renderer.Nodes.push_back(groupName, {}));
			++countGroups;
			info_printf("Group #%i found: '%s'.", countGroups, ::gpk::toString(groupName).begin());
		}
		else if(command == ::gpk::vcs{"usemtl"}) {
			::gpk::vcc									materialName	= lineValues[1];
			for(uint32_t iMaterial = 0; iMaterial < materialIndices.size(); ++iMaterial)
				if(materialIndices.Names[iMaterial] == materialName) {
					scene.Renderer.Nodes[newGroup.RenderNodes[0]].Material = materialIndices.Values[iMaterial];
					break;
				}
		}
		else if(command == ::gpk::vcs{"v"}) {
			::gpk::SCoord3<float>						vertex			= {};
			for(uint32_t iAxis = 0; iAxis < 3; ++iAxis)
				e_if(errored(::floatRead(numberReader, lineValues[1 + iAxis], (&vertex.x)[iAxis])), "%s", "");
			info_printf("Vertex found: [%f, %f, %f].", vertex.x, vertex.y, vertex.z);
			//scene.Renderer.Vertices[scene.Renderer.Nodes[newGroup.RenderNodes[0]].Vertices].push_back(vertex);
		}
		else if(command == ::gpk::vcs{"vt"}) {
			::gpk::SCoord2<float>						texCoord		= {};
			for(uint32_t iAxis = 0; iAxis < 2; ++iAxis)
				e_if(errored(::floatRead(numberReader, lineValues[1 + iAxis], (&texCoord.x)[iAxis])), "%s", "");
			info_printf("TexCoord found: [%f, %f, %f].", texCoord.x, texCoord.y);
		}
		else if(command == ::gpk::vcs{"vn"}) {
			::gpk::SCoord3<float>						vertex			= {};
			for(uint32_t iAxis = 0; iAxis < 3; ++iAxis)
				e_if(errored(::floatRead(numberReader, lineValues[1 + iAxis], (&vertex.x)[iAxis])), "%s", "");
			info_printf("Normal found: [%f, %f, %f].", vertex.x, vertex.y, vertex.z);
		}
		else if(command == ::gpk::vcs{"f"}) {
			::gpk::SCoord3<int32_t>						indices			= {};
			for(uint32_t iAxis = 0; iAxis < 3; ++iAxis)
				e_if(errored(::integerRead(numberReader, lineValues[1 + iAxis], (&indices.x)[iAxis])), "%s", "");
			info_printf("Face found: [%i, %i, %i].", indices.x, indices.y, indices.z);
		}
		else if(command == ::gpk::vcs{"vp"}) {

		}
	}
	return 0;
}

::gpk::error_t	gpk::SComponentScene::CreateFromFile		(::gpk::view_const_string filename)		{
	::gpk::view_const_char					extension				= {};
	::gpk::array_pod<char_t>				ext_lwr					= {};
	gpk_necall(filename.slice(extension, filename.size() - 4, 4), "File extension not supported for file '%s'", filename.begin());
	gpk_necall(ext_lwr.append(extension), "%s", "");
	::gpk::tolower(ext_lwr);
	static const ::gpk::vcs					str_stl					= ".stl";
	static const ::gpk::vcs					str_obj					= ".obj";
	if(ext_lwr == str_stl) {
		::gpk::SSTLFile							stlFile					= {};
		gpk_necall(::gpk::stlFileLoad(filename, stlFile), "Failed to load file '%s'.", ::gpk::toString(filename).begin());
		return ::createFromSTL(*this, stlFile, filename);
	}
	else if(ext_lwr == str_obj) {
		return ::createFromOBJ(*this, filename);
	}
	else {
		error_printf("File format not supported for file '%s'", ::gpk::toString(filename).begin());
		return -1;
	}
	return 0;
}
