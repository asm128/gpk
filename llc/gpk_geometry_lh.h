#include "gpk_array.h"
#include "gpk_color.h"
#include "gpk_coord.h"
#include "gpk_view_grid.h"

#ifndef CED_GEOMETRY_H_29234234
#define CED_GEOMETRY_H_29234234

namespace gpk
{
#pragma pack(push, 1)
	struct SGeometryGroupModes {
		uint16_t											Transparent			: 1;
		uint16_t											SkipSpecular		: 1;
		uint16_t											SkipAmbient			: 1;
		uint16_t											SkipDiffuse			: 1;
	};
	struct SGeometryGroup {
		int16_t												Image				;
		::gpk::SGeometryGroupModes							Modes				;
		::gpk::SRange<uint16_t>								Slice				;
		//::gpk::array_pod<uint16_t>	IndicesPointLightCache	;
	};
#pragma pack(pop)

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

	struct SModelManager {

	};

	int													geometryBuildCube				(::gpk::SGeometryQuads & geometry, const ::gpk::SCoord3<float> & scale);
	int													geometryBuildGrid				(::gpk::SGeometryQuads & geometry, ::gpk::SCoord2<uint32_t> gridSize, ::gpk::SCoord2<float> gridCenter, const ::gpk::SCoord3<float> & scale);
	int													geometryBuildSphere				(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::SCoord3<float> & gridCenter);
	int													geometryBuildCylinder			(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::SCoord3<float> & gridCenter, const ::gpk::SCoord3<float> & scale);
	int													geometryBuildHalfHelix			(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::SCoord3<float> & gridCenter, const ::gpk::SCoord3<float> & scale);
	int													geometryBuildHelix				(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::SCoord3<float> & gridCenter, const ::gpk::SCoord3<float> & scale);
	int													geometryBuildTender				(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::SCoord3<float> & gridCenter, const ::gpk::SCoord3<float> & scale);
	int													geometryBuildFigure0			(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::SCoord3<float> & gridCenter);
	int													geometryBuildFigure1			(::gpk::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::SCoord3<float> & gridCenter);

	int													geometryBuildSphere				(::gpk::SGeometryTriangles & geometry, uint32_t stacks, uint32_t slices, float radius, const ::gpk::SCoord3<float> & gridCenter);
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

} // namespace

#endif // CED_GEOMETRY_H_29234234
