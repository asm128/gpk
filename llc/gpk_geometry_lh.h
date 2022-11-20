#include "gpk_image.h"
#include "gpk_color.h"
#include "gpk_enum.h"
#include "gpk_ptr.h"
#include "gpk_png.h"

#ifndef CED_GEOMETRY_H_29234234
#define CED_GEOMETRY_H_29234234

namespace gpk
{
#pragma pack(push, 1)
	template<typename _tAxis, typename _tColor> struct SLightPoint2{ ::gpk::SCoord2<float> Position; _tColor Color; };
	template<typename _tAxis, typename _tColor> struct SLightPoint3{ ::gpk::SCoord3<float> Position; _tColor Color; };
#pragma pack(pop)

	template<typename _tElement>
	struct SKeyedContainer {
		::gpk::array_obj<_tElement>					Elements	= {};
		::gpk::array_pod<::gpk::view_const_char>	Keys		= {};

		const ::gpk::error_t						SetElement				(const ::gpk::view_const_char & keyName, const _tElement & elementValue) {
			const ::gpk::error_t							elementIndex			= GetElementIndex(); 
			if(elementIndex >= 0 && elementIndex < Keys.size())
				Elements[elementIndex]						= elementValue;
			else {
				Keys		.push_back(keyName);
				Elements	.push_back(elementValue);
			}
			return 0;
		}

		const ::gpk::error_t						GetElementIndex			(const ::gpk::view_const_char & keyName) const { return ::gpk::find(keyName, ::gpk::view_array<const ::gpk::vcc>{Keys}); }
		const _tElement								GetElementValue			(const ::gpk::view_const_char & keyName) const { 
			const ::gpk::error_t							elementIndex			= GetElementIndex(); 
			if(elementIndex >= 0 && elementIndex < Keys.size()) 
				return Keys[elementIndex];
			return {};
		}
	};


#pragma pack(push, 1)
	struct SImageTag {
		 COLOR_TYPE																ColorType;
		 int32_t																Index;
		 int32_t																Palette;
	};
#pragma pack(pop)
	struct SImageManager {
		::gpk::SKeyedContainer<::gpk::SImageTag>								ImageRegistry			= {};

		::gpk::array_obj<::gpk::ptr_obj<::gpk::SImage<::gpk::SColorBGR>		>>	BGR						= {};
		::gpk::array_obj<::gpk::ptr_obj<::gpk::SImage<::gpk::SColorBGRA>	>>	BGRA					= {};
		::gpk::array_obj<::gpk::ptr_obj<::gpk::SImage<uint8_t>				>>	Grayscale8				= {};
		::gpk::array_obj<::gpk::ptr_obj<::gpk::SImage<uint16_t>				>>	Grayscale16				= {};
		::gpk::array_obj<::gpk::ptr_obj<::gpk::SImageMonochrome<uint64_t>	>>	Monochrome				= {};

		::gpk::error_t															CreateImage				(const ::gpk::vcs & filename, const ::gpk::vcs & keyName, COLOR_TYPE colorType)	{
			::gpk::error_t	found =	ImageRegistry.GetElementIndex(keyName);
			if(found >= 0) // Image already created or loaded
				return found;

			::gpk::SPNGData																pngData;
			::gpk::pngFileLoad(pngData, filename);
			if(colorType == COLOR_TYPE_FILE) {
				switch(pngData.Header.ColorType) {
				case COLOR_TYPE_RGB				: case COLOR_TYPE_BGR : colorType = COLOR_TYPE_BGR ; break;
				case COLOR_TYPE_RGBA			: case COLOR_TYPE_BGRA: colorType = COLOR_TYPE_BGRA; break;
				case COLOR_TYPE_GRAYSCALE		: colorType = COLOR_TYPE_GRAYSCALE; break;
				case COLOR_TYPE_GRAYSCALE_ALPHA	: colorType = COLOR_TYPE_BGRA; break;
				case COLOR_TYPE_PALETTE			: colorType = COLOR_TYPE_PALETTE; break;
				}
			}
			return 0;
		}

		::gpk::error_t															CreateImage				(const ::gpk::vcs & filename, COLOR_TYPE colorType)	{
			return CreateImage(filename, filename, colorType);
		}

		::gpk::error_t															IsMonochrome			(const ::gpk::vcc & keyName)	{
			::gpk::error_t																index					= ImageRegistry.GetElementIndex(keyName);
			gpk_necall(index, "Image %s not found.", ::gpk::toString(keyName).begin());
			return ImageRegistry.Elements[index].ColorType;
		}
	};

	struct SGeometryQuads {
		::gpk::array_pod<::gpk::STriangle3	<float>>		Triangles;
		::gpk::array_pod<::gpk::SCoord3		<float>>		Normals;
		::gpk::array_pod<::gpk::STriangle2	<float>>		TextureCoords;
	};

	struct SGeometryTriangles {
		::gpk::array_pod<::gpk::STriangle3<float>>			Triangles;
		::gpk::array_pod<::gpk::STriangle3<float>>			Normals;
		::gpk::array_pod<::gpk::STriangle2<float>>			TextureCoords;
	};

	struct SGeometryIndexedTriangles {
		::gpk::array_pod<::gpk::SCoord3<float>>				Positions;
		::gpk::array_pod<::gpk::SCoord3<float>>				Normals;
		::gpk::array_pod<::gpk::SCoord2<float>>				TextureCoords;
		//::gpk::array_pod<::gpk::STriangle<uint16_t>>		PositionIndices;
		::gpk::array_pod<uint32_t>							PositionIndices;
	};

	struct SBufferManager {
		::gpk::SKeyedContainer<::gpk::ptr_obj<::gpk::SGeometryQuads		>>	Quads				= {};
		::gpk::SKeyedContainer<::gpk::ptr_obj<::gpk::SGeometryTriangles	>>	Triangles			= {};
	};

	GDEFINE_ENUM_TYPE (MODEL_NODE_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(MODEL_NODE_TYPE, FILE		, 0);
	GDEFINE_ENUM_VALUE(MODEL_NODE_TYPE, FUNCTION	, 1);
	GDEFINE_ENUM_VALUE(MODEL_NODE_TYPE, COMMAND		, 2);
	GDEFINE_ENUM_VALUE(MODEL_NODE_TYPE, POINT		, 3);
	GDEFINE_ENUM_VALUE(MODEL_NODE_TYPE, SEGMENT		, 4);
	GDEFINE_ENUM_VALUE(MODEL_NODE_TYPE, RECTANGLE	, 5);
	GDEFINE_ENUM_VALUE(MODEL_NODE_TYPE, TRIANGLE	, 6);
	GDEFINE_ENUM_VALUE(MODEL_NODE_TYPE, CIRCLE		, 7);
	GDEFINE_ENUM_VALUE(MODEL_NODE_TYPE, BOX			, 8);
	GDEFINE_ENUM_VALUE(MODEL_NODE_TYPE, SPHERE		, 9);
	GDEFINE_ENUM_VALUE(MODEL_NODE_TYPE, CYLINDER	, 10);

	GDEFINE_ENUM_TYPE (PRIMITIVE_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(PRIMITIVE_TYPE, POINT		, 0);
	GDEFINE_ENUM_VALUE(PRIMITIVE_TYPE, SEGMENT		, 1);
	GDEFINE_ENUM_VALUE(PRIMITIVE_TYPE, TRIANGLE		, 2);
	GDEFINE_ENUM_VALUE(PRIMITIVE_TYPE, RECTANGLE	, 3);

	GDEFINE_ENUM_TYPE (INDEX_MODE, uint8_t);
	GDEFINE_ENUM_VALUE(INDEX_MODE, LIST			, 0);
	GDEFINE_ENUM_VALUE(INDEX_MODE, STRIP		, 1);
	GDEFINE_ENUM_VALUE(INDEX_MODE, FAN			, 2);

	GDEFINE_ENUM_TYPE (BUFFER_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(BUFFER_TYPE, VERTEX		, 0);
	GDEFINE_ENUM_VALUE(BUFFER_TYPE, POLYGON		, 1);
#pragma pack(push, 1)
	struct SGeometryGroupModes {
		PRIMITIVE_TYPE										PrimitiveType		;
		INDEX_MODE											IndexMode			;

		uint16_t											Transparent			: 1;
		uint16_t											SkipSpecular		: 1;
		uint16_t											SkipAmbient			: 1;
		uint16_t											SkipDiffuse			: 1;
	};

	struct SGeometryGroup {
		uint32_t											Geometry			= (uint32_t)-1;
		uint32_t											Image				= (uint32_t)-1;
		uint32_t											Material			= (uint32_t)-1;
		::gpk::SGeometryGroupModes							Modes				= {};
		::gpk::SRange<uint32_t>								Slice				= {};
	};
#pragma pack(pop)

	struct SModelNode {
		::gpk::array_pod<int32_t>							VertexBuffers	= {};
		::gpk::array_pod<int32_t>							Images			= {};
		int32_t												IndexBuffer		= -1;
		::gpk::SGeometryGroupModes							Modes			= {};
		::gpk::SRange<uint16_t>								RangeIndex		= {};
		::gpk::SRange<uint16_t>								RangeVertex		= {};
	};

	struct SModelQuads {
		::gpk::array_pod<::gpk::STriangle3	<float>>		Triangles;
		::gpk::array_pod<::gpk::SCoord3		<float>>		Normals;
		::gpk::array_pod<::gpk::STriangle2	<float>>		TextureCoords;
		::gpk::array_pod<::gpk::SGeometryGroup>				GeometryGroups;
	};

	struct SModelTriangles {
		::gpk::array_pod<::gpk::STriangle3<float>>			Triangles;
		::gpk::array_pod<::gpk::STriangle3<float>>			Normals;
		::gpk::array_pod<::gpk::STriangle2<float>>			TextureCoords;
		::gpk::array_pod<::gpk::SGeometryGroup>				GeometryGroups;
	};


	int													geometryBuildCube				(::gpk::SGeometryQuads & geometry, const ::gpk::SCoord3<float> & scale);
	int													geometryBuildGrid				(::gpk::SGeometryQuads & geometry, ::gpk::SCoord2<uint32_t> gridSize, ::gpk::SCoord2<float> gridCenter, const ::gpk::SCoord3<float> & scale);
	int													geometryBuildSphere				(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::SCoord3<float> & gridCenter);
	int													geometryBuildCylinder			(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radiusYMin, float radiusYMax, const ::gpk::SCoord3<float> & gridCenter, const ::gpk::SCoord3<float> & scale);
	int													geometryBuildHalfHelix			(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::SCoord3<float> & gridCenter, const ::gpk::SCoord3<float> & scale);
	int													geometryBuildHelix				(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::SCoord3<float> & gridCenter, const ::gpk::SCoord3<float> & scale);
	int													geometryBuildTender				(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::SCoord3<float> & gridCenter, const ::gpk::SCoord3<float> & scale);
	int													geometryBuildFigure0			(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::SCoord3<float> & gridCenter);
	int													geometryBuildFigure1			(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::SCoord3<float> & gridCenter);

	int													geometryBuildSphere				(::gpk::SGeometryTriangles & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::SCoord3<float> & gridCenter);
	int													geometryBuildSphere				(::gpk::SGeometryIndexedTriangles & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::SCoord3<float> & gridCenter);

	int													geometryBuildCube				(::gpk::SGeometryQuads & geometry, const ::gpk::SCoord3<float> & scale, const ::gpk::SCoord3<float> & translation);
#pragma pack(push, 1)
	struct STile {
		float												Height[4]	;
		int16_t												Top			;
		int16_t												Front		;
		int16_t												Right		;
		int16_t												Flags		;
	};
#pragma pack(pop)
	int													geometryBuildTileListFromImage	(::gpk::view_grid<const ::gpk::SColorBGRA> image, ::gpk::array_pod<STile> & out_tiles, uint32_t imagePitch = 0);
	int													geometryBuildGridFromTileList	(::gpk::SGeometryQuads & geometry, ::gpk::view_grid<const ::gpk::STile> image, ::gpk::SCoord2<float> gridCenter, const ::gpk::SCoord3<float> & scale);

#pragma pack(push, 1)
	struct SRenderMaterial {
		::gpk::SColorFloat								Diffuse		;
		uint16_t										Color		;
		uint16_t										Palette		;
	};

	struct SAABBGeometry {
		::gpk::SCoord3<float>							Vertices	[8]			= {};
	};
#pragma pack(pop)

} // namespace

#endif // CED_GEOMETRY_H_29234234
