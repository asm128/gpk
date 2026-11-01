#include "gpk_geometry_buffers.h"

#ifndef	GPK_GLTF_H_23627
#define	GPK_GLTF_H_23627

namespace gpk
{
	struct SGLTFHeader {
		u2_t				Magic			= {};
		u2_t				Version			= {};
		u2_t				Length			= {};
	};

	struct SGLTF {
		SGLTFHeader				Header			= {};
		::gpk::SGeometryBuffers	Geometry;

	};

	gpk::err_t 	gltfLoad		(::gpk::SGeometryBuffers & geometry);
} // namespace

#endif	// GPK_GLTF_H_23627
