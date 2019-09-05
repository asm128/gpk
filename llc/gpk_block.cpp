#include "gpk_block.h"
#include "gpk_json_expression.h"

::gpk::error_t								gpk::blockConfigLoad		(::gpk::SBlockConfig& loaded, const ::gpk::SJSONReader & reader, int32_t iNode, const ::gpk::SBlockConfig& configDefault)	{
	if(0 > ::gpk::jsonExpressionResolve("key", reader, iNode, loaded.Key))
		loaded.Key									= configDefault.Key;
	::gpk::view_const_string						dummy						= {};
	::gpk::error_t									indexJSONBlockSize			= ::gpk::jsonExpressionResolve("blocksize"	, reader, iNode, dummy);
	::gpk::error_t									indexJSONDeflate			= ::gpk::jsonExpressionResolve("deflate"	, reader, iNode, dummy);
	loaded.BlockSize							= (0 <= indexJSONBlockSize	) ? (uint32_t)	reader.Token[indexJSONBlockSize	].Value : configDefault.BlockSize;
	loaded.Deflate								= (0 <= indexJSONDeflate	) ?				reader.Token[indexJSONDeflate	].Value : configDefault.Deflate;
	return 0;
}

::gpk::error_t								gpk::blockFileName			(const uint32_t idBlock, const ::gpk::view_const_string & dbName, ::gpk::array_pod<char_t> & fileName)	{
	fileName.append(::gpk::view_const_string{"db."});
	fileName.append(dbName);
	char											temp	[32]				= {};
	sprintf_s(temp, ".%u.ubk", idBlock);
	fileName.append(::gpk::view_const_string{temp});
	return 0;
}
