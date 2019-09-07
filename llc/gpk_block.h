#include "gpk_json.h"
#include "gpk_storage.h"
#include "gpk_deflate.h"
#include "gpk_aes.h"

#ifndef GPK_BLOCK_H_9823749283749823
#define GPK_BLOCK_H_9823749283749823

namespace gpk
{
#pragma pack(push, 1)
	struct SInt24 {
					uint16_t						Low;
					uint8_t							High;

		constexpr									SInt24						(int32_t value)				noexcept
			: Low((uint16_t)(value & 0xFFFF))
			, High((uint8_t)(((value & 0x7F0000) >> 16) | ((value & 0x80000000) >> 24)))
			{ }
		constexpr	operator						int32_t						()					const	noexcept		{ return (((uint32_t)Low) | ((High & 0x7F) << 16)) | ((High & 0x80) ? 0xFF800000 : 0); }
	};
#pragma pack(pop)

#pragma pack(push, 1)
	struct SBlockConfig {
					::gpk::view_const_string		Key;
					uint32_t						BlockSize;
					uint8_t							Containers;
					bool							Deflate;
	};
#pragma pack(pop)

	template<typename _tBlock>
	struct SMapBlock {
					::gpk::array_pod<uint32_t>		Id;
					::gpk::array_obj<_tBlock>		Block;

					::gpk::SBlockConfig				BlockConfig;
					::gpk::view_const_string		DBName;
	};

	struct SRecordMap {
					int32_t							IdBlock						= -1;
					int32_t							IndexRecord					= -1;
					int32_t							IndexContainer				= -1;
	};

	inline			::gpk::error_t					blockRecordIndices			(const uint64_t idRecord, uint32_t blockSize, ::gpk::SRecordMap & indices)	{
		const uint32_t									recordCoord					= (uint32_t)(idRecord & 0xFFFFFFFFUL);
		indices.IndexContainer						= (uint32_t)(idRecord & 0xFFFFFFFF00000000ULL);
		indices.IdBlock								= (uint32_t)(recordCoord / blockSize);
		indices.IndexRecord							= (uint32_t)(recordCoord % blockSize);
		return 0;
	}

				::gpk::error_t					blockConfigLoad				(::gpk::SBlockConfig& out_config, const ::gpk::SJSONReader & reader, int32_t iNode, const ::gpk::SBlockConfig& configDefault = {{}, 65535, 0});
				::gpk::error_t					blockFileName				(const uint32_t idBlock, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & folderName, ::gpk::array_pod<char_t> & fileName);

				::gpk::error_t					crcVerifyAndRemove		(::gpk::array_pod<byte_t> & bytes);
				::gpk::error_t					crcGenerateAndAppend	(::gpk::array_pod<byte_t> & bytes);

	template<typename _tElement>
				::gpk::error_t					blockMapLoad				(::gpk::array_pod<char_t> & loadedBytes, ::gpk::SRecordMap & indexMap, ::gpk::SMapBlock<_tElement> & mapBlock, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & dbPath, uint64_t idRecord)								{
		loadedBytes.clear();
		::gpk::blockRecordIndices(idRecord, mapBlock.BlockConfig.BlockSize, indexMap);
		for(uint32_t iBlock = 0; iBlock < mapBlock.Id.size(); ++iBlock) {
			if((uint32_t)indexMap.IdBlock == mapBlock.Id[iBlock])
				return iBlock;
		}
		::gpk::error_t									indexBlock					= mapBlock.Block.push_back({});
		gpk_necall(mapBlock.Id.push_back(indexMap.IdBlock), "%s", "Out of memory?");
		::gpk::array_pod<char_t>						finalPath					= dbPath;
		if(finalPath.size())
			finalPath.push_back('/');
		finalPath.append(dbName);
		if(mapBlock.BlockConfig.Containers) {
			finalPath.push_back('/');
			char											temp[32]					= {};
			sprintf_s(temp, "%u", indexMap.IndexContainer);
			finalPath.append_string(temp);
		}
		::gpk::array_pod<char_t>						fileName					= {};
		gpk_necall(::gpk::blockFileName(indexMap.IdBlock, dbName, finalPath, fileName), "%s", "Out of memory?");
		::gpk::array_pod<char_t>						encrypted;
		::gpk::array_pod<char_t>						deflated;
		if(false == mapBlock.BlockConfig.Deflate && 0 == mapBlock.BlockConfig.Key.size()) {
			gpk_necall(::gpk::fileToMemory({fileName.begin(), fileName.size()}, loadedBytes), "Invalid record id: %llu. Block doesn't exist.", idRecord);
			gpk_necall(::gpk::crcVerifyAndRemove(loadedBytes), "%s", "CRC Check failed!");
		}
		else if(false == mapBlock.BlockConfig.Deflate) {
			gpk_necall(::gpk::fileToMemory({fileName.begin(), fileName.size()}, encrypted), "Invalid record id: %llu. Block doesn't exist.", idRecord);
			gpk_necall(::gpk::crcVerifyAndRemove(encrypted), "%s", "CRC Check failed!");
			gpk_necall(::gpk::aesDecode(encrypted, mapBlock.BlockConfig.Key, ::gpk::AES_LEVEL_256, loadedBytes), "Failed to decompress file! Corrupt file?");
		}
		else if(0 == mapBlock.BlockConfig.Key.size()) {
			gpk_necall(::gpk::fileToMemory({fileName.begin(), fileName.size()}, deflated), "Invalid record id: %llu. Block doesn't exist.", idRecord);
			gpk_necall(::gpk::crcVerifyAndRemove(deflated), "%s", "CRC Check failed!");
			gpk_necall(::gpk::arrayInflate(deflated, loadedBytes), "Failed to decompress file! Corrupt file?");
		}
		else {
			gpk_necall(::gpk::fileToMemory({fileName.begin(), fileName.size()}, encrypted), "Invalid record id: %llu. Block doesn't exist.", idRecord);
			gpk_necall(::gpk::crcVerifyAndRemove(encrypted), "%s", "CRC Check failed!");
			gpk_necall(::gpk::aesDecode(encrypted, mapBlock.BlockConfig.Key, ::gpk::AES_LEVEL_256, deflated), "Failed to decompress file! Corrupt file?");
			gpk_necall(::gpk::arrayInflate(deflated, loadedBytes), "Failed to decompress file! Corrupt file?");
		}
		return indexBlock;
	}

	template<typename _tElement>
				::gpk::error_t					blockMapSave				(const ::gpk::array_pod<char_t> & blockBytes, const ::gpk::SRecordMap & indexMap, const ::gpk::SMapBlock<_tElement> & mapBlock, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & dbPath)								{
		::gpk::array_pod<char_t>						finalPath					= dbPath;
		if(finalPath.size())
			finalPath.push_back('/');
		finalPath.append(dbName);
		if(mapBlock.BlockConfig.Containers) {
			finalPath.push_back('/');
			char											temp[32]					= {};
			sprintf_s(temp, "%u", indexMap.IndexContainer);
			finalPath.append_string(temp);
		}
		::gpk::array_pod<char_t>						fileName					= {};
		gpk_necall(::gpk::blockFileName(indexMap.IdBlock, dbName, finalPath, fileName), "%s", "Out of memory?");
		::gpk::array_pod<char_t>						encrypted;
		::gpk::array_pod<char_t>						deflated;
		if(false == mapBlock.BlockConfig.Deflate && 0 == mapBlock.BlockConfig.Key.size()) {
			::gpk::array_pod<char_t>						bytesToWrite				= blockBytes;
			gpk_necall(::gpk::crcGenerateAndAppend(bytesToWrite), "%s", "CRC Check failed!");
			gpk_necall(::gpk::fileFromMemory({fileName.begin(), fileName.size()}, bytesToWrite), "Failed to save block: {container: %u, block: %u}.", indexMap.IndexContainer, indexMap.IdBlock);
		}
		else if(false == mapBlock.BlockConfig.Deflate) {
			gpk_necall(::gpk::aesEncode(blockBytes, mapBlock.BlockConfig.Key, ::gpk::AES_LEVEL_256, encrypted), "%s", "Failed to encrypt file!");
			gpk_necall(::gpk::crcGenerateAndAppend(encrypted), "%s", "CRC Check failed!");
			gpk_necall(::gpk::fileFromMemory({fileName.begin(), fileName.size()}, encrypted), "Failed to save block: {container: %u, block: %u}.", indexMap.IndexContainer, indexMap.IdBlock);
		}
		else if(0 == mapBlock.BlockConfig.Key.size()) {
			gpk_necall(::gpk::arrayDeflate(blockBytes, deflated), "Failed to compress file!");
			gpk_necall(::gpk::crcGenerateAndAppend(deflated), "%s", "CRC Check failed!");
			gpk_necall(::gpk::fileFromMemory({fileName.begin(), fileName.size()}, deflated), "Failed to save block: {container: %u, block: %u}.", indexMap.IndexContainer, indexMap.IdBlock);
		}
		else {
			gpk_necall(::gpk::arrayDeflate(blockBytes, deflated), "Failed to compress file! Corrupt file?");
			gpk_necall(::gpk::aesEncode(deflated, mapBlock.BlockConfig.Key, ::gpk::AES_LEVEL_256, encrypted), "Failed to encrypt file!");
			gpk_necall(::gpk::crcGenerateAndAppend(encrypted), "%s", "CRC Check failed!");
			gpk_necall(::gpk::fileFromMemory({fileName.begin(), fileName.size()}, encrypted), "Failed to save block: {container: %u, block: %u}.", indexMap.IndexContainer, indexMap.IdBlock);
		}
		return 0;
	}
} // namespace

#endif // GPK_BLOCK_H_9823749283749823
