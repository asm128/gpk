#include "gpk_png.h"
#include "gpk_tri2.h"
#include "gpk_tri3.h"
#include "gpk_range.h"

#ifndef CED_GEOMETRY_H_23627
#define CED_GEOMETRY_H_23627

namespace gpk
{
#pragma pack(push, 1)
	template<typename _tAxis, typename _tColor> struct SLightPoint2 { ::gpk::n2f32 Position; _tColor Color; };
	template<typename _tAxis, typename _tColor> struct SLightPoint3 { ::gpk::n3f32 Position; _tColor Color; };

	struct SImageTag {
		 COLOR_TYPE	ColorType;
		 int32_t	Index;
		 int32_t	Palette;
	};
#pragma pack(pop)

	struct SGeometryQuads {
		::gpk::apod<::gpk::tri3f32>	Triangles;
		::gpk::apod<::gpk::n3f32>	Normals;
		::gpk::apod<::gpk::tri2f32>	TextureCoords;
	};

	struct SGeometryTriangles {
		::gpk::apod<::gpk::tri3f32>	Triangles;
		::gpk::apod<::gpk::tri3f32>	Normals;
		::gpk::apod<::gpk::tri2f32>	TextureCoords;
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
		PRIMITIVE_TYPE						PrimitiveType	;
		INDEX_MODE							IndexMode		;

		uint16_t							Transparent		: 1;
		uint16_t							SkipSpecular	: 1;
		uint16_t							SkipAmbient		: 1;
		uint16_t							SkipDiffuse		: 1;
	};

	struct SGeometryGroup {
		uint32_t							Geometry		= (uint32_t)-1;
		uint32_t							Image			= (uint32_t)-1;
		uint32_t							Material		= (uint32_t)-1;
		::gpk::SGeometryGroupModes			Modes			= {};
		::gpk::rangeu32						Slice			= {};
	};
#pragma pack(pop)

	struct SModelNode {
		::gpk::ai32							VertexBuffers	= {};
		::gpk::ai32							Images			= {};
		int32_t								IndexBuffer		= -1;
		::gpk::SGeometryGroupModes			Modes			= {};
		::gpk::rangeu16						RangeIndex		= {};
		::gpk::rangeu16						RangeVertex		= {};
	};

	struct SModelQuads {
		::gpk::apod<::gpk::tri3f32>			Triangles;
		::gpk::apod<::gpk::n3f32>			Normals;
		::gpk::apod<::gpk::tri2f32>			TextureCoords;
		::gpk::apod<::gpk::SGeometryGroup>	GeometryGroups;
	};

	struct SModelTriangles {
		::gpk::apod<::gpk::tri3f32>			Triangles;
		::gpk::apod<::gpk::tri3f32>			Normals;
		::gpk::apod<::gpk::tri2f32>			TextureCoords;
		::gpk::apod<::gpk::SGeometryGroup>	GeometryGroups;
	};

	::gpk::error_t	geometryBuildCube		(::gpk::SGeometryQuads & geometry, const ::gpk::n3f32 & scale);
	::gpk::error_t	geometryBuildGrid		(::gpk::SGeometryQuads & geometry, ::gpk::n2u16 gridSize, ::gpk::n2f32 gridCenter, const ::gpk::n3f32 & scale);

	::gpk::error_t	geometryBuildSphere		(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3f32 & gridCenter);
	::gpk::error_t	geometryBuildCylinder	(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radiusYMin, float radiusYMax, const ::gpk::n3f32 & gridCenter, const ::gpk::n3f32 & scale);
	::gpk::error_t	geometryBuildHelixHalf	(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3f32 & gridCenter, const ::gpk::n3f32 & scale);
	::gpk::error_t	geometryBuildHelix		(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3f32 & gridCenter, const ::gpk::n3f32 & scale);
	::gpk::error_t	geometryBuildTender		(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3f32 & gridCenter, const ::gpk::n3f32 & scale);
	::gpk::error_t	geometryBuildFigure0	(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3f32 & gridCenter);
	::gpk::error_t	geometryBuildFigure1	(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3f32 & gridCenter);

	::gpk::error_t	geometryBuildSphere		(::gpk::SGeometryTriangles & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::n3f32 & gridCenter);
	::gpk::error_t	geometryBuildCube		(::gpk::SGeometryQuads & geometry, const ::gpk::n3f32 & scale, const ::gpk::n3f32 & translation);
#pragma pack(push, 1)
	struct STile {
		float			Height	[4]	= {};
		int16_t			Top			= -1;
		int16_t			Front		= -1;
		int16_t			Right		= -1;
		int16_t			Flags		= -1;
	};

	template<typename _tIndex>
	struct STileIndices {
		_tIndex			Top		[4]	= {};
		_tIndex			Front	[4]	= {};
		_tIndex			Right	[4]	= {};
	};

#pragma pack(pop)
	::gpk::error_t	geometryBuildTileListFromImage	(::gpk::gc8bgra image, ::gpk::apod<STile> & out_tiles, uint32_t imagePitch = 0);
	::gpk::error_t	geometryBuildGridFromTileList	(::gpk::SGeometryQuads & geometry, ::gpk::grid<const ::gpk::STile> image, ::gpk::n2f32 gridCenter, const ::gpk::n3f32 & scale);

#pragma pack(push, 1)
	struct SRenderMaterialPaletted {
		::gpk::rgbaf	Diffuse		;
		uint16_t		Color		;
		uint16_t		Palette		;
	};

	struct SAABBGeometry {
		::gpk::n3f32	Vertices	[8]			= {};
	};
#pragma pack(pop)

} // namespace

#endif // CED_GEOMETRY_H_23627
