#include "gpk_block.h"
#include "gpk_json_expression.h"
#include "gpk_noise.h"

::gpk::error_t								gpk::blockRecordId			(const ::gpk::SRecordMap & indices, uint32_t blockSize, uint64_t & idRecord)	{
	idRecord									= ((uint64_t)indices.IdBlock * blockSize + (uint64_t)indices.IndexRecord);
	return 0;
}

::gpk::error_t								gpk::blockRecordIndices		(const uint64_t idRecord, uint32_t blockSize, ::gpk::SRecordMap & indices)	{
	const uint64_t									recordCoord					= (uint64_t)(idRecord & 0x000FFFFFFFFFFFFFULL);
	indices.IdBlock								= (uint32_t)(recordCoord / blockSize);
	indices.IndexRecord							= (uint32_t)(recordCoord % blockSize);
	return 0;
}

::gpk::error_t								gpk::blockRecordPath		(::gpk::array_pod<char_t> & fileName, const ::gpk::SRecordMap & indices, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & dbPath)	{
	::gpk::array_pod<char_t>						finalPath					= dbPath;
	gpk_necall(::gpk::blockFilePath(finalPath, dbName, dbPath), "%s", "Out of memory?");
	gpk_necall(::gpk::blockFileName(indices.IdBlock, dbName, finalPath, fileName), "%s", "Out of memory?");
	return 0;
}

::gpk::error_t								gpk::blockConfigLoad		(::gpk::SBlockConfig& loaded, const ::gpk::SJSONReader & reader, int32_t iNode, const ::gpk::SBlockConfig& configDefault)	{
	if(0 > ::gpk::jsonExpressionResolve(::gpk::vcs{"key"}, reader, iNode, loaded.Key))
		loaded.Key									= configDefault.Key;
	::gpk::error_t									indexJSONBlockSize			= ::gpk::jsonExpressionResolve(::gpk::vcs{"blocksize"}, reader, (uint32_t)iNode);
	::gpk::error_t									indexJSONDeflate			= ::gpk::jsonExpressionResolve(::gpk::vcs{"deflate"  }, reader, (uint32_t)iNode);
	loaded.BlockSize							= (0 <= indexJSONBlockSize	) ? (uint32_t)	reader.Token[indexJSONBlockSize	].Value : configDefault.BlockSize;
	loaded.Deflate								= (0 <= indexJSONDeflate	) ?				reader.Token[indexJSONDeflate	].Value : configDefault.Deflate;
	return 0;
}

::gpk::error_t								gpk::blockFileName			(const uint32_t idBlock, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & folderName, ::gpk::array_pod<char_t> & fileName)	{
	if(folderName.size()) {
		gpk_necall(fileName.append(folderName), "%s", "Out of memory?");
		gpk_necall(fileName.push_back('/'), "%s", "Out of memory?");
	}
	::gpk::array_pod<char_t>						tablePath;
	gpk_necall(fileName.append(::gpk::vcs{"db."}), "%s", "Out of memory?");
	gpk_necall(fileName.append(dbName), "%s", "Out of memory?");
	char											temp	[32]				= {};
	snprintf(temp, ::gpk::size(temp) - 2, ".%u.ubk", idBlock);
	gpk_necall(fileName.append(::gpk::vcs{temp}), "%s", "Out of memory?");
	return 0;
}

::gpk::error_t								gpk::blockFilePath			(::gpk::array_pod<char_t> & finalPath, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & dbPath) {
	finalPath									= dbPath;
	if(finalPath.size())
		gpk_necall(finalPath.push_back('/'), "%s", "Out of memory?");
	gpk_necall(finalPath.append(dbName), "%s", "Out of memory?");
	return 0;
}
