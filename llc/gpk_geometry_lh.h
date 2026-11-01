#include "gpk_apod_color.h"
#include "gpk_tri2.h"
#include "gpk_tri3.h"
#include "gpk_range.h"
#include "gpk_color_type.h"
#include "gpk_grid_color.h"

#ifndef CED_GEOMETRY_H_23627
#define CED_GEOMETRY_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm _tAxis, tpnm _tColor> struct SLightPoint2 { ::gpk::n2f2_t Position; _tColor Color; };
	tplt<tpnm _tAxis, tpnm _tColor> struct SLightPoint3 { ::gpk::n3f2_t Position; _tColor Color; };

	struct SImageTag {
		 COLOR_TYPE	ColorType;
		 s2_t	Index;
		 s2_t	Palette;
	};
#pragma pack(pop)

	struct SGeometryQuads {
		::gpk::apod<::gpk::tri3f2_t>	Triangles;
		::gpk::apod<::gpk::n3f2_t>	Normals;
		::gpk::apod<::gpk::tri2f2_t>	TextureCoords;
	};

	struct SGeometryTriangles {
		::gpk::apod<::gpk::tri3f2_t>	Triangles;
		::gpk::apod<::gpk::tri3f2_t>	Normals;
		::gpk::apod<::gpk::tri2f2_t>	TextureCoords;
	};


	GDEFINE_ENUM_TYPE (MODEL_NODE_TYPE, u0_t);
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

	GDEFINE_ENUM_TYPE (PRIMITIVE_TYPE, u0_t);
	GDEFINE_ENUM_VALUE(PRIMITIVE_TYPE, POINT		, 0);
	GDEFINE_ENUM_VALUE(PRIMITIVE_TYPE, SEGMENT		, 1);
	GDEFINE_ENUM_VALUE(PRIMITIVE_TYPE, TRIANGLE		, 2);
	GDEFINE_ENUM_VALUE(PRIMITIVE_TYPE, RECTANGLE	, 3);

	GDEFINE_ENUM_TYPE (INDEX_MODE, u0_t);
	GDEFINE_ENUM_VALUE(INDEX_MODE, LIST			, 0);
	GDEFINE_ENUM_VALUE(INDEX_MODE, STRIP		, 1);
	GDEFINE_ENUM_VALUE(INDEX_MODE, FAN			, 2);

	GDEFINE_ENUM_TYPE (BUFFER_TYPE, u0_t);
	GDEFINE_ENUM_VALUE(BUFFER_TYPE, VERTEX		, 0);
	GDEFINE_ENUM_VALUE(BUFFER_TYPE, POLYGON		, 1);
#pragma pack(push, 1)

	struct SGeometryGroupModes {
		PRIMITIVE_TYPE						PrimitiveType	;
		INDEX_MODE							IndexMode		;

		u1_t							Transparent		: 1;
		u1_t							SkipSpecular	: 1;
		u1_t							SkipAmbient		: 1;
		u1_t							SkipDiffuse		: 1;
	};

	struct SGeometryGroup {
		u2_t							Geometry		= (u2_t)-1;
		u2_t							Image			= (u2_t)-1;
		u2_t							Material		= (u2_t)-1;
		::gpk::SGeometryGroupModes			Modes			= {};
		::gpk::rangeu2_t					Slice			= {};
	};
#pragma pack(pop)

	struct SModelNode {
		::gpk::as2_t						VertexBuffers	= {};
		::gpk::as2_t						Images			= {};
		s2_t								IndexBuffer		= -1;
		::gpk::SGeometryGroupModes			Modes			= {};
		::gpk::rangeu1_t					RangeIndex		= {};
		::gpk::rangeu1_t					RangeVertex		= {};
	};

	struct SModelQuads {
		::gpk::apod<::gpk::tri3f2_t>			Triangles;
		::gpk::apod<::gpk::n3f2_t>			Normals;
		::gpk::apod<::gpk::tri2f2_t>			TextureCoords;
		::gpk::apod<::gpk::SGeometryGroup>	GeometryGroups;
	};

	struct SModelTriangles {
		::gpk::apod<::gpk::tri3f2_t>			Triangles;
		::gpk::apod<::gpk::tri3f2_t>			Normals;
		::gpk::apod<::gpk::tri2f2_t>			TextureCoords;
		::gpk::apod<::gpk::SGeometryGroup>	GeometryGroups;
	};

#pragma pack(push, 1)
	struct STile {
		f2_t			Height	[4]	= {};
		s1_t			Top			= -1;
		s1_t			Front		= -1;
		s1_t			Right		= -1;
		s1_t			Flags		= -1;
	};

	tplt<tpnm _tIndex>
	struct STileIndices {
		_tIndex			Top		[4]	= {};
		_tIndex			Front	[4]	= {};
		_tIndex			Right	[4]	= {};
	};

#pragma pack(pop)

#ifndef GPK_ATMEL

	gpk::err_t geometryBuildCube		(::gpk::SGeometryQuads & geometry, ::gpk::n3f2_c & scale);
	gpk::err_t geometryBuildGrid		(::gpk::SGeometryQuads & geometry, ::gpk::n2u1_t gridSize, ::gpk::n2f2_t gridCenter, ::gpk::n3f2_c & scale);

	gpk::err_t geometryBuildSphere		(::gpk::SGeometryQuads & geometry, u2_t stacks, u2_t slices, f2_t radius, ::gpk::n3f2_c & gridCenter);
	gpk::err_t geometryBuildCylinder	(::gpk::SGeometryQuads & geometry, u2_t stacks, u2_t slices, f2_t radiusYMin, f2_t radiusYMax, ::gpk::n3f2_c & gridCenter, ::gpk::n3f2_c & scale);
	gpk::err_t geometryBuildHelixHalf	(::gpk::SGeometryQuads & geometry, u2_t stacks, u2_t slices, f2_t radius, ::gpk::n3f2_c & gridCenter, ::gpk::n3f2_c & scale);
	gpk::err_t geometryBuildHelix		(::gpk::SGeometryQuads & geometry, u2_t stacks, u2_t slices, f2_t radius, ::gpk::n3f2_c & gridCenter, ::gpk::n3f2_c & scale);
	gpk::err_t geometryBuildTender		(::gpk::SGeometryQuads & geometry, u2_t stacks, u2_t slices, f2_t radius, ::gpk::n3f2_c & gridCenter, ::gpk::n3f2_c & scale);
	gpk::err_t geometryBuildFigure0	(::gpk::SGeometryQuads & geometry, u2_t stacks, u2_t slices, f2_t radius, ::gpk::n3f2_c & gridCenter);
	gpk::err_t geometryBuildFigure1	(::gpk::SGeometryQuads & geometry, u2_t stacks, u2_t slices, f2_t radius, ::gpk::n3f2_c & gridCenter);

	gpk::err_t geometryBuildSphere		(::gpk::SGeometryTriangles & geometry, u2_t stacks, u2_t slices, f2_t radius, ::gpk::n3f2_c & gridCenter);
	gpk::err_t geometryBuildCube		(::gpk::SGeometryQuads & geometry, const ::gpk::n3f2_t & scale, const ::gpk::n3f2_t & translation);

	gpk::err_t geometryBuildTileListFromImage	(::gpk::gc8bgra image, ::gpk::apod<STile> & out_tiles, u2_t imagePitch = 0);
	gpk::err_t geometryBuildGridFromTileList	(::gpk::SGeometryQuads & geometry, ::gpk::grid<const ::gpk::STile> image, ::gpk::n2f2_t gridCenter, const ::gpk::n3f2_t & scale);

#endif

#pragma pack(push, 1)
	struct SRenderMaterialPaletted {
		::gpk::rgbaf	Diffuse		;
		u1_t		Color		;
		u1_t		Palette		;
	};

	struct SAABBGeometry {
		::gpk::n3f2_t	Vertices	[8]			= {};
	};
#pragma pack(pop)

} // namespace

#endif // CED_GEOMETRY_H_23627
