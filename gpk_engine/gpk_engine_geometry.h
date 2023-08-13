#include "gpk_engine_renderbuffer.h"
#include "gpk_geometry.h"

#ifndef GPK_ENGINE_GEOMETRY_H
#define GPK_ENGINE_GEOMETRY_H

namespace gpk 
{
	::gpk::error_t			createBuffers
		( const ::gpk::SGeometryBuffers	& geometry
		, ::gpk::pobj<SRenderBuffer>	& pIndicesVertex
		, ::gpk::pobj<SRenderBuffer>	& pVertices
		, ::gpk::pobj<SRenderBuffer>	& pNormals
		, ::gpk::pobj<SRenderBuffer>	& pUV
		);

	::gpk::error_t			createBuffers
		( SRenderBufferManager			& bufferManager 
		, const ::gpk::SGeometryBuffers	& geometry
		, uint32_t						& iIndicesVertex
		, uint32_t						& iVertices
		, uint32_t						& iNormals
		, uint32_t						& iUV
		);

} // namespace

#endif // GPK_ENGINE_GEOMETRY_H
