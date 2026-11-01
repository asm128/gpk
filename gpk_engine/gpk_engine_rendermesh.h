#include "gpk_linear_map_pobj.h"

#include "gpk_enum.h"

#include "gpk_range_n2.h"

#ifndef GPK_ENGINE_RENDERMESH_H_23627
#define GPK_ENGINE_RENDERMESH_H_23627

namespace gpk 
{
	// ------------------ Geometry Mesh
#pragma pack(push, 1)
	struct SGeometrySlice {
		::gpk::rangeu2_t						Slice;
	};

	GDEFINE_ENUM_TYPE(NORMAL_MODE, u0_t);
	GDEFINE_ENUM_VALUE(NORMAL_MODE, Point			, 0);
	GDEFINE_ENUM_VALUE(NORMAL_MODE, Line			, 1);
	GDEFINE_ENUM_VALUE(NORMAL_MODE, Triangle		, 2);
	GDEFINE_ENUM_VALUE(NORMAL_MODE, Quad			, 3);

	GDEFINE_ENUM_TYPE(MESH_MODE, u0_t);
	GDEFINE_ENUM_VALUE(MESH_MODE, List				, 0);
	GDEFINE_ENUM_VALUE(MESH_MODE, Strip				, 1);
	GDEFINE_ENUM_VALUE(MESH_MODE, Fan				, 2);

	GDEFINE_ENUM_TYPE(GEOMETRY_TYPE, u0_t);
	GDEFINE_ENUM_VALUE(GEOMETRY_TYPE, Point			, 0);
	GDEFINE_ENUM_VALUE(GEOMETRY_TYPE, Line			, 1);
	GDEFINE_ENUM_VALUE(GEOMETRY_TYPE, Triangle		, 2);
	GDEFINE_ENUM_VALUE(GEOMETRY_TYPE, Quad			, 3);

	struct SMeshDescription {
		::gpk::GEOMETRY_TYPE				Type;
		::gpk::MESH_MODE					Mode;
		::gpk::NORMAL_MODE					NormalMode;
	};
#pragma pack(pop)

	struct SGeometryMesh {
		::gpk::SMeshDescription				Desc;
		::gpk::au2_t						GeometryBuffers;
		::gpk::au2_t						ConstantBuffers;
		::gpk::apod<::gpk::SGeometrySlice>	GeometrySlices;

		gpk::err_t 					Save			(::gpk::au0_t & output) const { 
			if_fail_fe(gpk::savePOD(output, Desc));
			if_fail_fe(gpk::saveView(output, GeometryBuffers));
			if_fail_fe(gpk::saveView(output, ConstantBuffers));
			if_fail_fe(gpk::saveView(output, GeometrySlices));
			return 0;
		}
		gpk::err_t 					Load			(::gpk::vcu0_t & input) {
			if_fail_fe(gpk::loadPOD(input, Desc));
			if_fail_fe(gpk::loadView(input, GeometryBuffers));
			if_fail_fe(gpk::loadView(input, ConstantBuffers));
			if_fail_fe(gpk::loadView(input, GeometrySlices));
			return 0;
		}
	};	

	typedef	::gpk::gpk_linear_map_pobj<::gpk::SGeometryMesh>	SMeshManager;
} // namespace

#endif // GPK_ENGINE_RENDERMESH_H_23627
