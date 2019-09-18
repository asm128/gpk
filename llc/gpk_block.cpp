#include "gpk_block.h"
#include "gpk_json_expression.h"
#include "gpk_noise.h"

::gpk::error_t								gpk::blockRecordId			(const ::gpk::SRecordMap & indices, uint32_t blockSize, uint64_t & idRecord)	{
	idRecord									= ((uint64_t)indices.IndexContainer) << 52;
	idRecord									|= ((uint64_t)indices.IdBlock * blockSize + (uint64_t)indices.IndexRecord);
	return 0;
}

::gpk::error_t								gpk::blockRecordIndices		(const uint64_t idRecord, uint32_t blockSize, ::gpk::SRecordMap & indices)	{
	const uint64_t									recordCoord					= (uint64_t)(idRecord & 0x000FFFFFFFFFFFFFULL);
	indices.IndexContainer						= (uint8_t)((idRecord & 0xFFF0000000000000ULL) >> 52);
	indices.IdBlock								= (uint32_t)(recordCoord / blockSize);
	indices.IndexRecord							= (uint32_t)(recordCoord % blockSize);
	return 0;
}

::gpk::error_t								gpk::blockRecordPath		(::gpk::array_pod<char_t> & fileName, const ::gpk::SRecordMap & indices, uint32_t containers, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & dbPath)	{
	::gpk::array_pod<char_t>						finalPath					= dbPath;
	gpk_necall(::gpk::blockFilePath(finalPath, dbName, dbPath, containers, indices.IndexContainer), "%s", "Out of memory?");
	gpk_necall(::gpk::blockFileName(indices.IdBlock, dbName, finalPath, fileName), "%s", "Out of memory?");
	return 0;
}

::gpk::error_t								gpk::blockConfigLoad		(::gpk::SBlockConfig& loaded, const ::gpk::SJSONReader & reader, int32_t iNode, const ::gpk::SBlockConfig& configDefault)	{
	if(0 > ::gpk::jsonExpressionResolve("key", reader, iNode, loaded.Key))
		loaded.Key									= configDefault.Key;
	::gpk::error_t									indexJSONBlockSize			= ::gpk::jsonExpressionResolve("blocksize"	, reader, (uint32_t)iNode);
	::gpk::error_t									indexJSONDeflate			= ::gpk::jsonExpressionResolve("deflate"	, reader, (uint32_t)iNode);
	::gpk::error_t									indexJSONContainers			= ::gpk::jsonExpressionResolve("containers"	, reader, (uint32_t)iNode);
	loaded.BlockSize							= (0 <= indexJSONBlockSize	) ? (uint32_t)	reader.Token[indexJSONBlockSize	].Value : configDefault.BlockSize;
	loaded.Deflate								= (0 <= indexJSONDeflate	) ?				reader.Token[indexJSONDeflate	].Value : configDefault.Deflate;
	loaded.Containers							= (0 <= indexJSONContainers	) ?	(uint8_t)	reader.Token[indexJSONDeflate	].Value : configDefault.Containers;
	return 0;
}

::gpk::error_t								gpk::blockFileName			(const uint32_t idBlock, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & folderName, ::gpk::array_pod<char_t> & fileName)	{
	if(folderName.size()) {
		gpk_necall(fileName.append(folderName), "%s", "Out of memory?");
		gpk_necall(fileName.push_back('/'), "%s", "Out of memory?");
	}
	::gpk::array_pod<char_t>						tablePath;
	gpk_necall(fileName.append(::gpk::view_const_string{"db."}), "%s", "Out of memory?");
	gpk_necall(fileName.append(dbName), "%s", "Out of memory?");
	char											temp	[32]				= {};
	sprintf_s(temp, ".%u.ubk", idBlock);
	gpk_necall(fileName.append(::gpk::view_const_string{temp}), "%s", "Out of memory?");
	return 0;
}

::gpk::error_t								gpk::blockFilePath			(::gpk::array_pod<char_t> & finalPath, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & dbPath, const uint32_t containers, const uint32_t indexContainer) {
	finalPath									= dbPath;
	if(finalPath.size())
		gpk_necall(finalPath.push_back('/'), "%s", "Out of memory?");
	gpk_necall(finalPath.append(dbName), "%s", "Out of memory?");
	if(containers) {
		gpk_necall(finalPath.push_back('/'), "%s", "Out of memory?");
		char											temp[32]					= {};
		sprintf_s(temp, "%u", indexContainer);
		gpk_necall(finalPath.append_string(temp), "%s", "Out of memory?");
	}
	return 0;
}
