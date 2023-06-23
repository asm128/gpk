#include "gpk_geometry_buffers.h"

#ifndef	GPK_GLTF_H_2023623
#define	GPK_GLTF_H_2023623

namespace gpk
{
	struct SGLTFHeader {
		uint32_t				Magic			= {};
		uint32_t				Version			= {};
		uint32_t				Length			= {};
	};

	struct SGLTF {
		SGLTFHeader				Header			= {};
		::gpk::SGeometryBuffers	Geometry;

	};

	::gpk::error_t		gltfLoad		(::gpk::SGeometryBuffers & geometry);
} // namespace

#endif	// GPK_GLTF_H_2023623
