#include "gpk_block.h"
#include "gpk_json_expression.h"
#include "gpk_noise.h"

::gpk::error_t								gpk::blockRecordId			(const ::gpk::SRecordMap & indices, uint32_t blockSize, uint64_t & idRecord)	{
	idRecord									= ((uint64_t)indices.IndexContainer) << 32;
	idRecord									|= ((uint64_t)indices.IdBlock * blockSize + (int32_t)indices.IndexRecord);
	return 0;
}

::gpk::error_t								gpk::blockRecordIndices		(const uint64_t idRecord, uint32_t blockSize, ::gpk::SRecordMap & indices)	{
	const uint32_t									recordCoord					= (uint32_t)(idRecord & 0xFFFFFFFFUL);
	indices.IndexContainer						= (uint8_t)((idRecord & 0xFFFFFFFF00000000ULL) >> 32);
	indices.IdBlock								= (uint32_t)(recordCoord / blockSize);
	indices.IndexRecord							= (uint32_t)(recordCoord % blockSize);
	return 0;
}

::gpk::error_t								gpk::blockRecordPath		(::gpk::array_pod<char_t> & fileName, const ::gpk::SRecordMap & indices, uint32_t containers, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & dbPath)	{
	::gpk::array_pod<char_t>						finalPath					= dbPath;
	::gpk::blockFilePath(finalPath, dbName, dbPath, containers, indices.IndexContainer);
	gpk_necall(::gpk::blockFileName(indices.IdBlock, dbName, finalPath, fileName), "%s", "Out of memory?");
	return 0;
}

::gpk::error_t								gpk::blockConfigLoad		(::gpk::SBlockConfig& loaded, const ::gpk::SJSONReader & reader, int32_t iNode, const ::gpk::SBlockConfig& configDefault)	{
	if(0 > ::gpk::jsonExpressionResolve("key", reader, iNode, loaded.Key))
		loaded.Key									= configDefault.Key;
	::gpk::view_const_string						dummy						= {};
	::gpk::error_t									indexJSONBlockSize			= ::gpk::jsonExpressionResolve("blocksize"	, reader, iNode, dummy);
	::gpk::error_t									indexJSONDeflate			= ::gpk::jsonExpressionResolve("deflate"	, reader, iNode, dummy);
	::gpk::error_t									indexJSONContainers			= ::gpk::jsonExpressionResolve("containers"	, reader, iNode, dummy);
	loaded.BlockSize							= (0 <= indexJSONBlockSize	) ? (uint32_t)	reader.Token[indexJSONBlockSize	].Value : configDefault.BlockSize;
	loaded.Deflate								= (0 <= indexJSONDeflate	) ?				reader.Token[indexJSONDeflate	].Value : configDefault.Deflate;
	loaded.Containers							= (0 <= indexJSONContainers	) ?	(uint8_t)	reader.Token[indexJSONDeflate	].Value : configDefault.Containers;
	return 0;
}

::gpk::error_t								gpk::blockFileName			(const uint32_t idBlock, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & folderName, ::gpk::array_pod<char_t> & fileName)	{
	if(folderName.size()) {
		fileName.append(folderName);
		fileName.push_back('/');
	}
	::gpk::array_pod<char_t>						tablePath;
	fileName.append(::gpk::view_const_string{"db."});
	fileName.append(dbName);
	char											temp	[32]				= {};
	sprintf_s(temp, ".%u.ubk", idBlock);
	fileName.append(::gpk::view_const_string{temp});
	return 0;
}

::gpk::error_t								gpk::blockFilePath			(::gpk::array_pod<char_t> & finalPath, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & dbPath, const uint32_t containers, const uint32_t indexContainer) {
	finalPath									= dbPath;
	if(finalPath.size())
		finalPath.push_back('/');
	finalPath.append(dbName);
	if(containers) {
		finalPath.push_back('/');
		char											temp[32]					= {};
		sprintf_s(temp, "%u", indexContainer);
		finalPath.append_string(temp);
	}
	return 0;
}
