#include "gpk_json.h"
#include "gpk_storage.h"
#include "gpk_deflate.h"
#include "gpk_aes.h"
#include "gpk_find.h"
#include "gpk_parse.h"

#ifndef GPK_BLOCK_H_9823749283749823
#define GPK_BLOCK_H_9823749283749823

namespace gpk
{
#pragma pack(push, 1)
	struct SInt24 {
					uint16_t									Low;
					uint8_t										High;

		constexpr												SInt24						()							noexcept	: Low(0), High(0)		{}
		constexpr												SInt24						(int32_t value)				noexcept
			: Low((uint16_t)(value & 0xFFFF))
			, High((uint8_t)(((value & 0x7F0000) >> 16) | ((value & 0x80000000) >> 24)))
			{ }
		constexpr	operator									int32_t						()					const	noexcept	{ return (((uint32_t)Low) | ((High & 0x7F) << 16)) | ((High & 0x80) ? 0xFF800000 : 0); }
	};

	struct SBlockConfig {
					::gpk::view_const_string					Key;
					uint32_t									BlockSize;
					uint8_t										Containers;
					bool										Deflate;
	};
#pragma pack(pop)

	template<typename _tBlock>
	struct SMapBlock {
					::gpk::array_pod<uint8_t>					IdContainer;
					::gpk::array_pod<uint32_t>					Id;
					::gpk::array_obj<::gpk::ptr_obj<_tBlock>>	Block;

					::gpk::SBlockConfig							BlockConfig;
					::gpk::view_const_string					DBName;
					int32_t										MaxBlockOnDisk				= -1;
	};

#pragma pack(push, 1)
	struct SRecordMap {
					int32_t										IdBlock						= -1;
					SInt24										IndexRecord					= -1;
					uint8_t										IndexContainer				= 0;
	};
#pragma pack(pop)

				::gpk::error_t								blockRecordIndices			(const uint64_t idRecord, uint32_t blockSize, ::gpk::SRecordMap & indices);
				::gpk::error_t								blockRecordId				(const ::gpk::SRecordMap & indices, uint32_t blockSize, uint64_t & idRecord);
				::gpk::error_t								blockRecordPath				(::gpk::array_pod<char_t> & fileName, const ::gpk::SRecordMap & indices, uint32_t containers, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & dbPath);
	inline		::gpk::error_t								blockRecordPath				(::gpk::array_pod<char_t> & fileName, ::gpk::SRecordMap & indices, const uint64_t idRecord, uint32_t blockSize, uint32_t containers, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & dbPath) {
					::gpk::blockRecordIndices(idRecord, blockSize, indices);
					return ::gpk::blockRecordPath(fileName, indices, containers, dbName, dbPath);
				}

				::gpk::error_t								blockConfigLoad				(::gpk::SBlockConfig& out_config, const ::gpk::SJSONReader & reader, int32_t iNode, const ::gpk::SBlockConfig& configDefault = {{}, 65535, 0});
				::gpk::error_t								blockFileName				(const uint32_t idBlock, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & folderName, ::gpk::array_pod<char_t> & fileName);
				::gpk::error_t								blockFilePath				(::gpk::array_pod<char_t> & finalPath, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & dbPath, const uint32_t containers, const uint8_t indexContainer);

	template<typename _tElement>
				::gpk::error_t								blockMapLoad				(::gpk::array_pod<char_t> & loadedBytes, ::gpk::SMapBlock<_tElement> & mapBlock, const ::gpk::view_const_char & fileName, const ::gpk::SRecordMap & indexMap)								{
		loadedBytes.clear();
		for(uint32_t iBlock = 0; iBlock < mapBlock.Id.size(); ++iBlock) {
			if((uint32_t)indexMap.IdBlock == mapBlock.Id[iBlock])
				return iBlock;
		}
		::gpk::error_t												indexBlock					= mapBlock.Block.push_back({});
		gpk_necall(mapBlock.Id			.push_back(indexMap.IdBlock			), "%s", "Out of memory?");
		gpk_necall(mapBlock.IdContainer	.push_back(indexMap.IndexContainer	), "%s", "Out of memory?");
		gpk_necall(::gpk::fileToMemorySecure(loadedBytes, fileName, mapBlock.BlockConfig.Key, mapBlock.BlockConfig.Deflate), "Failed to load block file: %s.", ::gpk::toString(fileName).begin());
		return indexBlock;
	}

	template<typename _tElement>
				::gpk::error_t								blockMapLoad				(::gpk::array_pod<char_t> & loadedBytes, ::gpk::SRecordMap & indexMap, ::gpk::SMapBlock<_tElement> & mapBlock, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & dbPath, uint64_t idRecord)								{
		::gpk::array_pod<char_t>									fileName					= {};
		::gpk::blockRecordPath(fileName, indexMap, idRecord, mapBlock.BlockConfig.BlockSize, mapBlock.BlockConfig.Containers, dbName, dbPath);
		return ::gpk::blockMapLoad(loadedBytes, mapBlock, fileName, indexMap);
	}

	template<typename _tElement>
				::gpk::error_t								blockMapSave				(const ::gpk::array_pod<char_t> & blockBytes, const ::gpk::SRecordMap & indexMap, const ::gpk::SMapBlock<_tElement> & mapBlock, const ::gpk::view_const_char & dbName, const ::gpk::view_const_char & dbPath, uint64_t idRecord)								{
		::gpk::array_pod<char_t>									finalPath					= {};
		::gpk::blockFilePath(finalPath, dbName, dbPath, mapBlock.BlockConfig.Containers, indexMap.IndexContainer);
		::gpk::array_pod<char_t>									fileName					= {};
		gpk_necall(::gpk::blockFileName(indexMap.IdBlock, dbName, finalPath, fileName), "%s", "Out of memory?");
		return ::gpk::fileFromMemorySecure(blockBytes, fileName, mapBlock.BlockConfig.Key, mapBlock.BlockConfig.Deflate);
	}

	template<typename _tMapBlock>
			int64_t											getMapId							(::gpk::SMapBlock<_tMapBlock> & mapBlocks, const ::gpk::view_const_char & dbPath, const ::gpk::view_const_char & sequenceToFind)	{
		uint64_t													container							= 0;
		::gpk::crcGenerate(sequenceToFind, container);
		container												= container % mapBlocks.BlockConfig.Containers;
		::gpk::array_pod<uint32_t>									idBlocks							= mapBlocks.Id;
		::gpk::array_pod<uint32_t>									blocksToSkip;
		for(uint32_t iBlock = 0; iBlock < mapBlocks.Block.size(); ++iBlock) {
			if(mapBlocks.IdContainer[iBlock] != container)
				continue;
			const _tMapBlock											& block								= *mapBlocks.Block[iBlock];
			const int32_t												idEmail								= block.GetMapId(sequenceToFind);
			const uint32_t												idBlock								= mapBlocks.Id[iBlock];
			if(0 <= idEmail) {
				::gpk::SRecordMap											indices;
				indices.IndexContainer									= mapBlocks.IdContainer[iBlock];
				indices.IdBlock											= idBlock;
				indices.IndexRecord										= idEmail;
				uint64_t													result								= (uint64_t)-1LL;
				::gpk::blockRecordId(indices, mapBlocks.BlockConfig.BlockSize, result);
				return result;
			}
			blocksToSkip.push_back(idBlock);
			(void)block;
		}
		::gpk::array_pod<char_t>									containerPath						= {};
		::gpk::blockFilePath(containerPath, mapBlocks.DBName, dbPath, mapBlocks.BlockConfig.Containers, (uint8_t)container);
		::gpk::array_obj<::gpk::array_pod<char_t>>					paths;
		::gpk::pathList({containerPath.begin(), containerPath.size()}, paths, false);
		const ::gpk::view_const_string								extension							= "ubk";
		::gpk::array_pod<char_t>									loadedBytes;
		for(uint32_t iFile = 0; iFile < paths.size(); ++iFile) {
			const ::gpk::view_const_char								fileNameCurrent						= paths[iFile];
			if(5 >= fileNameCurrent.size())
				continue;
			::gpk::view_const_char										filePart							= {&fileNameCurrent[fileNameCurrent.size() - 3], 3};
			if(filePart != extension)
				continue;
			filePart												= {fileNameCurrent.begin(), fileNameCurrent.size()-4};
			::gpk::error_t												indexOfPrevDot						= ::gpk::rfind('.', filePart);
			if(-1 == indexOfPrevDot)
				continue;
			filePart												= {&filePart[indexOfPrevDot], filePart.size() - indexOfPrevDot};
			uint32_t													idBlock								= (uint32_t)-1;
			::gpk::parseIntegerDecimal(filePart, &idBlock);
			mapBlocks.MaxBlockOnDisk								= ::gpk::max(mapBlocks.MaxBlockOnDisk, (int32_t)idBlock);
			if(0 <= ::gpk::find(idBlock, ::gpk::view_array<const uint32_t>{blocksToSkip.begin(), blocksToSkip.size()}))
				continue;
			::gpk::SRecordMap											indexMap							= {};
			indexMap.IdBlock										= idBlock;
			indexMap.IndexContainer									= (uint8_t)container;
			indexMap.IndexRecord									= -1;
			loadedBytes.clear();
			const ::gpk::error_t										indexBlock							= ::gpk::blockMapLoad(loadedBytes, mapBlocks, fileNameCurrent, indexMap);
			_tMapBlock													& block								= *mapBlocks.Block[indexBlock];
			gpk_necall(block.Load(loadedBytes), "Failed to load block file: %s.", ::gpk::toString(fileNameCurrent).begin());
			const int32_t												idEmail								= block.GetMapId(sequenceToFind);
			if(0 <= idEmail) {
				indexMap.IndexRecord									= idEmail;
				uint64_t													result								= (uint64_t)-1LL;
				::gpk::blockRecordId(indexMap, mapBlocks.BlockConfig.BlockSize, result);
				return result;
			}
		}
		return -1;
	}
} // namespace

#endif // GPK_BLOCK_H_9823749283749823
