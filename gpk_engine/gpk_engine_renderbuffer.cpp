#include "gpk_engine_renderbuffer.h"

::gpk::error_t			gpk::createBuffers	
	( uint32_t							vertexCount
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pIndicesVertex
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pVertices
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pNormals
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pUV
	) {
	::gpk::clear(pIndicesVertex, pVertices, pNormals, pUV);
	pVertices	->Desc.Format	= ::gpk::DATA_TYPE_FLOAT32_3;
	pVertices	->Desc.Usage	= ::gpk::BUFFER_USAGE_Position;

	pNormals	->Desc.Format	= ::gpk::DATA_TYPE_FLOAT32_3;
	pNormals	->Desc.Usage	= ::gpk::BUFFER_USAGE_Normal;

	pUV			->Desc.Format	= ::gpk::DATA_TYPE_FLOAT32_2;
	pUV			->Desc.Usage	= ::gpk::BUFFER_USAGE_UV;

	pIndicesVertex->Desc.Usage	= ::gpk::BUFFER_USAGE_Index;
	pIndicesVertex->Desc.Format	
		= (vertexCount > 0xFFFF) ? ::gpk::DATA_TYPE_UINT32
		: (vertexCount > 0x00FF) ? ::gpk::DATA_TYPE_UINT16
		: ::gpk::DATA_TYPE_UINT8
		;
	return 0;
}
