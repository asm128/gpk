#include "gpk_block.h"
#include "gpk_json_expression.h"
#include "gpk_noise.h"

::gpk::error_t			gpk::blockRecordId			(const ::gpk::SRecordMap & indices, uint32_t blockSize, uint64_t & idRecord)	{
	idRecord						= ((uint64_t)indices.IdBlock * blockSize + (uint64_t)indices.IndexRecord);
	return 0;
}

::gpk::error_t			gpk::blockRecordIndices		(const uint64_t idRecord, uint32_t blockSize, ::gpk::SRecordMap & indices)	{
	const uint64_t						recordCoord					= (uint64_t)(idRecord & 0x000FFFFFFFFFFFFFULL);
	indices.IdBlock					= (uint32_t)(recordCoord / blockSize);
	indices.IndexRecord				= (uint32_t)(recordCoord % blockSize);
	return 0;
}

::gpk::error_t			gpk::blockRecordPath		(::gpk::apod<char> & fileName, const ::gpk::SRecordMap & indices, const ::gpk::vcc & dbName, const ::gpk::vcc & dbPath)	{
	::gpk::apod<char>					finalPath					= dbPath;
	gpk_necs(gpk::blockFilePath(finalPath, dbName, dbPath));
	gpk_necs(gpk::blockFileName(fileName, indices.IdBlock, dbName, finalPath));
	return 0;
}

::gpk::error_t			gpk::blockFilePath			(::gpk::apod<char> & finalPath, const ::gpk::vcc & dbName, const ::gpk::vcc & dbPath) {
	finalPath						= dbPath;
	if(finalPath.size())
		gpk_necs(finalPath.push_back('/'));
	gpk_necs(finalPath.append(dbName));
	return 0;
}

::gpk::error_t			gpk::blockFileName			(::gpk::apod<char> & fileName, const uint32_t idBlock, const ::gpk::vcc & dbName, const ::gpk::vcc & folderName)	{
	if(folderName.size()) {
		gpk_necs(fileName.append(folderName));
		gpk_necs(fileName.push_back('/'));
	}
	gpk_necs(fileName.append_string("db."));
	gpk_necs(fileName.append(dbName));

	char								temp	[32]				= {};
	snprintf(temp, ::gpk::size(temp) - 2, ".%u%s", idBlock, EXTENSION_BLOCK_FILE.begin());
	gpk_necs(fileName.append_string(temp));
	return 0;
}


::gpk::error_t			gpk::blockConfigLoad		(::gpk::SBlockConfig& loaded, const ::gpk::SJSONReader & reader, int32_t iNode, const ::gpk::SBlockConfig& configDefault)	{
	if(0 > ::gpk::jsonExpressionResolve(::gpk::vcs{"key"}, reader, iNode, loaded.Key))
		loaded.Key						= configDefault.Key;

	::gpk::error_t						indexJSONBlockSize			= ::gpk::jsonExpressionResolve(::gpk::vcs{"blocksize"}, reader, (uint32_t)iNode);
	::gpk::error_t						indexJSONDeflate			= ::gpk::jsonExpressionResolve(::gpk::vcs{"deflate"  }, reader, (uint32_t)iNode);
	loaded.BlockSize				= (0 <= indexJSONBlockSize	) ? (uint32_t)	reader.Token[indexJSONBlockSize	].Value : configDefault.BlockSize;
	loaded.Deflate					= (0 <= indexJSONDeflate	) ?				reader.Token[indexJSONDeflate	].Value : configDefault.Deflate;
	return 0;
}
