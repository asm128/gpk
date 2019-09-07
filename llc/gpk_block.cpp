#include "gpk_block.h"
#include "gpk_json_expression.h"
#include "gpk_noise.h"

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

static constexpr const uint32_t				GPK_BLOCK_CRC_SEED			= 18973;

::gpk::error_t								gpk::crcVerifyAndRemove		(::gpk::array_pod<byte_t> & bytes)	{
	ree_if(bytes.size() < 8, "Invalid input. No CRC can be found in an array of %u bytes.", bytes.size());
	uint64_t										check						= 0;
	const uint32_t									startOfCRC					= bytes.size() - 8;
	const uint64_t									found						= (bytes.size() >= 8) ? *(uint64_t*)&bytes[startOfCRC] : (uint64_t)-1LL;
	for(uint32_t iByte = 0; iByte < startOfCRC; ++iByte)
		check									+= ::gpk::noise1DBase(bytes[iByte], ::GPK_BLOCK_CRC_SEED);
	ree_if(check != found, "CRC Check failed: Stored: %llu. Calculated: %llu.", found, check);
	gpk_necall(bytes.resize(bytes.size() - 8), "%s", "Out of memory?");
	return 0;
}

::gpk::error_t								gpk::crcGenerateAndAppend	(::gpk::array_pod<byte_t> & bytes)	{
	uint64_t										crcToStore					= 0;
	for(uint32_t i=0; i < bytes.size(); ++i)
		crcToStore								+= ::gpk::noise1DBase(bytes[i], ::GPK_BLOCK_CRC_SEED);

	gpk_necall(bytes.append((char*)&crcToStore, sizeof(uint64_t)), "%s", "Out of memory?");;
	return 0;
}


