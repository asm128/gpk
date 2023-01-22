#include "gpk_engine_container.h"
#include "gpk_matrix.h"
#include "gpk_enum.h"

#ifndef GPK_ENGINE_RENDERMESH_H
#define GPK_ENGINE_RENDERMESH_H

namespace gpk 
{
	// ------------------ Geometry Mesh
#pragma pack(push, 1)
	struct SGeometrySlice {
		::gpk::SRange<uint32_t>				Slice;
	};
#pragma pack(pop)

	GDEFINE_ENUM_TYPE(NORMAL_MODE, uint8_t);
	GDEFINE_ENUM_VALUE(NORMAL_MODE, Point			, 0);
	GDEFINE_ENUM_VALUE(NORMAL_MODE, Line			, 1);
	GDEFINE_ENUM_VALUE(NORMAL_MODE, Triangle		, 2);
	GDEFINE_ENUM_VALUE(NORMAL_MODE, Quad			, 3);

	GDEFINE_ENUM_TYPE(MESH_MODE, uint8_t);
	GDEFINE_ENUM_VALUE(MESH_MODE, List				, 0);
	GDEFINE_ENUM_VALUE(MESH_MODE, Strip				, 1);
	GDEFINE_ENUM_VALUE(MESH_MODE, Fan				, 2);

	GDEFINE_ENUM_TYPE(GEOMETRY_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(GEOMETRY_TYPE, Point			, 0);
	GDEFINE_ENUM_VALUE(GEOMETRY_TYPE, Line			, 1);
	GDEFINE_ENUM_VALUE(GEOMETRY_TYPE, Triangle		, 2);
	GDEFINE_ENUM_VALUE(GEOMETRY_TYPE, Quad			, 3);

#pragma pack(push, 1)
	struct SMeshDescription {
		::gpk::GEOMETRY_TYPE				Type;
		::gpk::MESH_MODE					Mode;
		::gpk::NORMAL_MODE					NormalMode;
	};
#pragma pack(pop)

	struct SGeometryMesh {
		::gpk::SMeshDescription				Desc;
		::gpk::apod<uint32_t>				GeometryBuffers;
		::gpk::apod<uint32_t>				ConstantBuffers;
		::gpk::apod<::gpk::SGeometrySlice>	GeometrySlices;

		::gpk::error_t						Save			(::gpk::apod<ubyte_t> & output) const { 
			gpk_necs(::gpk::viewSave(output, ::gpk::v1<const ::gpk::SMeshDescription>{&Desc, 1}));
			gpk_necs(::gpk::viewSave(output, GeometryBuffers));
			gpk_necs(::gpk::viewSave(output, ConstantBuffers));
			gpk_necs(::gpk::viewSave(output, GeometrySlices));
			return 0;
		}
		::gpk::error_t						Load			(::gpk::vcub & input) {
			gpk_necs(::gpk::loadPOD(input, Desc));
			gpk_necs(::gpk::loadView(input, GeometryBuffers));
			gpk_necs(::gpk::loadView(input, ConstantBuffers));
			gpk_necs(::gpk::loadView(input, GeometrySlices));
			return 0;
		}
	};	

	typedef	::gpk::SLinearMap<::gpk::SGeometryMesh>			SMeshManager;
} // namespace

#endif
