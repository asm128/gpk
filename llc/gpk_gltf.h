#include "gpk_geometry_buffers.h"

#ifndef	GPK_GLTF_H_23627
#define	GPK_GLTF_H_23627

nmsp gpk
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
} // nmsp

#endif	// GPK_GLTF_H_23627
