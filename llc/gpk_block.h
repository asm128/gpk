#include "gpk_json.h"
#include "gpk_storage.h"

#ifndef GPK_BLOCK_H_9823749283749823
#define GPK_BLOCK_H_9823749283749823

namespace gpk
{
#pragma pack(push, 1)
	struct SBlockConfig {
		::gpk::view_const_string					Key;
		uint32_t									BlockSize;
		uint16_t									Containers;
		bool										Deflate;
	};
#pragma pack(pop)

	template<typename _tBlock>
	struct SMapBlock {
		::gpk::array_pod<uint32_t>					Id;
		::gpk::array_obj<_tBlock>					Block;

		::gpk::SBlockConfig							BlockConfig;
		::gpk::view_const_string					DBName;
	};

	struct SRecordMap {
		int32_t										IdBlock						= -1;
		int32_t										IndexRecord					= -1;
	};

	::gpk::error_t								blockConfigLoad				(::gpk::SBlockConfig& out_config, const ::gpk::SJSONReader & reader, int32_t iNode, const ::gpk::SBlockConfig& configDefault = {{}, 65535, 0});
	::gpk::error_t								blockFileName				(const uint32_t idBlock, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & folderName, ::gpk::array_pod<char_t> & fileName);

	template<typename _tElement>
				::gpk::error_t					blockMapLoad				(::gpk::array_pod<char_t> & loadedBytes, ::gpk::SRecordMap & indexMap, ::gpk::SMapBlock<_tElement> & mapBlock, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & dbPath, uint64_t idRecord)								{
		indexMap.IdBlock							= (uint32_t)(idRecord / mapBlock.BlockConfig.BlockSize);
		indexMap.IndexRecord						= (uint32_t)(idRecord % mapBlock.BlockConfig.BlockSize);
		for(uint32_t iBlock = 0; iBlock < mapBlock.Id.size(); ++iBlock) {
			if((uint32_t)indexMap.IdBlock == mapBlock.Id[iBlock])
				return iBlock;
		}
		::gpk::error_t									indexBlock					= mapBlock.Block.push_back({});
		gpk_necall(mapBlock.Id.push_back(indexMap.IdBlock), "%s", "");
		::gpk::array_pod<char_t>						fileName					= {};
		gpk_necall(::gpk::blockFileName(indexMap.IdBlock, dbName, dbPath, fileName), "%s", "Out of memory?");
		gpk_necall(::gpk::fileFromMemory({fileName.begin(), fileName.size()}, loadedBytes), "Invalid record id: %llu. Block doesn't exist.", idRecord);
		return indexBlock;
	}
} // namespace

#endif // GPK_BLOCK_H_9823749283749823
