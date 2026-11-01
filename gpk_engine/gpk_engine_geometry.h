#include "gpk_engine_renderbuffer.h"
#include "gpk_geometry.h"

#ifndef GPK_ENGINE_GEOMETRY_H
#define GPK_ENGINE_GEOMETRY_H

namespace gpk 
{
	::gpk::err_t			createBuffers
		( const ::gpk::SGeometryBuffers	& geometry
		, ::gpk::pobj<SRenderBuffer>	& pIndicesVertex
		, ::gpk::pobj<SRenderBuffer>	& pVertices
		, ::gpk::pobj<SRenderBuffer>	& pNormals
		, ::gpk::pobj<SRenderBuffer>	& pUV
		);

	::gpk::err_t			createBuffers
		( SRenderBufferManager			& bufferManager 
		, const ::gpk::SGeometryBuffers	& geometry
		, u2_t						& iIndicesVertex
		, u2_t						& iVertices
		, u2_t						& iNormals
		, u2_t						& iUV
		);

} // namespace

#endif // GPK_ENGINE_GEOMETRY_H
