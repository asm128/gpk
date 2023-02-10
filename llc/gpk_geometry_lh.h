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
	template<typename _tAxis, typename _tColor> struct SLightPoint2{ ::gpk::n2<float> Position; _tColor Color; };
	template<typename _tAxis, typename _tColor> struct SLightPoint3{ ::gpk::n3<float> Position; _tColor Color; };

	struct SImageTag {
		 COLOR_TYPE											ColorType;
		 int32_t											Index;
		 int32_t											Palette;
	};
#pragma pack(pop)

	struct SGeometryQuads {
		::gpk::apod<::gpk::STriangle3	<float>>	Triangles;
		::gpk::apod<::gpk::n3			<float>>	Normals;
		::gpk::apod<::gpk::STriangle2	<float>>	TextureCoords;
	};

	struct SGeometryTriangles {
		::gpk::apod<::gpk::STriangle3<float>>		Triangles;
		::gpk::apod<::gpk::STriangle3<float>>		Normals;
		::gpk::apod<::gpk::STriangle2<float>>		TextureCoords;
	};

	struct SGeometryIndexedTriangles {
		::gpk::apod<::gpk::n3<float>>				Positions;
		::gpk::apod<::gpk::n3<float>>				Normals;
		::gpk::apod<::gpk::n2<float>>				TextureCoords;
		::gpk::apod<uint32_t>						PositionIndices;
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
		PRIMITIVE_TYPE							PrimitiveType		;
		INDEX_MODE								IndexMode			;

		uint16_t								Transparent			: 1;
		uint16_t								SkipSpecular		: 1;
		uint16_t								SkipAmbient			: 1;
		uint16_t								SkipDiffuse			: 1;
	};

	struct SGeometryGroup {
		uint32_t								Geometry			= (uint32_t)-1;
		uint32_t								Image				= (uint32_t)-1;
		uint32_t								Material			= (uint32_t)-1;
		::gpk::SGeometryGroupModes				Modes				= {};
		::gpk::SRange<uint32_t>					Slice				= {};
	};
#pragma pack(pop)

	struct SModelNode {
		::gpk::apod<int32_t>					VertexBuffers	= {};
		::gpk::apod<int32_t>					Images			= {};
		int32_t									IndexBuffer		= -1;
		::gpk::SGeometryGroupModes				Modes			= {};
		::gpk::SRange<uint16_t>					RangeIndex		= {};
		::gpk::SRange<uint16_t>					RangeVertex		= {};
	};

	struct SModelQuads {
		::gpk::apod<::gpk::STriangle3<float>>	Triangles;
		::gpk::apod<::gpk::n3<float>>			Normals;
		::gpk::apod<::gpk::STriangle2<float>>	TextureCoords;
		::gpk::apod<::gpk::SGeometryGroup>		GeometryGroups;
	};

	struct SModelTriangles {
		::gpk::apod<::gpk::STriangle3<float>>	Triangles;
		::gpk::apod<::gpk::STriangle3<float>>	Normals;
		::gpk::apod<::gpk::STriangle2<float>>	TextureCoords;
		::gpk::apod<::gpk::SGeometryGroup>		GeometryGroups;
	};


	int											geometryBuildCube				(::gpk::SGeometryQuads & geometry, const ::gpk::n3<float> & scale);
	int											geometryBuildGrid				(::gpk::SGeometryQuads & geometry, ::gpk::n2<uint32_t> gridSize, ::gpk::n2<float> gridCenter, const ::gpk::n3<float> & scale);
	int											geometryBuildSphere				(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3<float> & gridCenter);
	int											geometryBuildCylinder			(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radiusYMin, float radiusYMax, const ::gpk::n3<float> & gridCenter, const ::gpk::n3<float> & scale);
	int											geometryBuildHalfHelix			(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3<float> & gridCenter, const ::gpk::n3<float> & scale);
	int											geometryBuildHelix				(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3<float> & gridCenter, const ::gpk::n3<float> & scale);
	int											geometryBuildTender				(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3<float> & gridCenter, const ::gpk::n3<float> & scale);
	int											geometryBuildFigure0			(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3<float> & gridCenter);
	int											geometryBuildFigure1			(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3<float> & gridCenter);

	int											geometryBuildSphere				(::gpk::SGeometryTriangles & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3<float> & gridCenter);
	int											geometryBuildSphere				(::gpk::SGeometryIndexedTriangles & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3<float> & gridCenter);
	int											geometryBuildCylinder			(::gpk::SGeometryIndexedTriangles & geometry, uint32_t stacks, uint32_t slices, float radiusYMin, float radiusYMax, const ::gpk::n3<float> & gridCenter, const ::gpk::n3<float> & scale, bool negateNormals, float diameterRatio = 1.0f);

	int											geometryBuildCube				(::gpk::SGeometryQuads & geometry, const ::gpk::n3<float> & scale, const ::gpk::n3<float> & translation);
#pragma pack(push, 1)
	struct STile {
		float										Height[4]	;
		int16_t										Top			;
		int16_t										Front		;
		int16_t										Right		;
		int16_t										Flags		;
	};
#pragma pack(pop)
	int											geometryBuildTileListFromImage	(::gpk::view2d<const ::gpk::SColorBGRA> image, ::gpk::apod<STile> & out_tiles, uint32_t imagePitch = 0);
	int											geometryBuildGridFromTileList	(::gpk::SGeometryQuads & geometry, ::gpk::view2d<const ::gpk::STile> image, ::gpk::n2<float> gridCenter, const ::gpk::n3<float> & scale);

#pragma pack(push, 1)
	struct SRenderMaterialPaletted {
		::gpk::rgbaf								Diffuse		;
		uint16_t									Color		;
		uint16_t									Palette		;
	};

	struct SAABBGeometry {
		::gpk::n3<float>							Vertices	[8]			= {};
	};
#pragma pack(pop)

} // namespace

#endif // CED_GEOMETRY_H_29234234
