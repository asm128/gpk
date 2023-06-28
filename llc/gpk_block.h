#include "gpk_json.h"
#include "gpk_file.h"
#include "gpk_deflate.h"
#include "gpk_aes.h"
#include "gpk_parse.h"
#include "gpk_view_manager.h"
#include "gpk_path.h"

#ifndef GPK_BLOCK_H_23627
#define GPK_BLOCK_H_23627

namespace gpk
{
	stacxpr	::gpk::vcc		EXTENSION_BLOCK_FILE		= {4, ".ubk"};
#pragma pack(push, 1)
	//struct SInt24 {
	//	uint16_t					Low;
	//	uint8_t						High;
	//
	//	constexpr					SInt24						()							noexcept	: Low(0), High(0)		{}
	//	constexpr					SInt24						(int32_t value)				noexcept
	//		: Low((uint16_t)(value & 0xFFFF))
	//		, High((uint8_t)(((value & 0x7F0000) >> 16) | ((value & 0x80000000) >> 24)))
	//		{ }
	//	constexpr	operator		int32_t						()					const	noexcept	{ return (((uint32_t)Low) | ((High & 0x7F) << 16)) | ((High & 0x80) ? 0xFF800000 : 0); }
	//};
	struct SBlockConfig {
		::gpk::vcc					Key							= {};
		uint32_t					BlockSize					= 0;
		bool						Deflate						= 0;
	};
#pragma pack(pop)

	::gpk::error_t				blockConfigLoad				(::gpk::SBlockConfig & out_config, const ::gpk::SJSONReader & reader, int32_t iNode, const ::gpk::SBlockConfig & configDefault = {{}, 65535, 0});

	template<typename _tBlock>
	struct SMapTable {
		::gpk::au32		Id;
		::gpk::apobj<_tBlock>		Block;

		::gpk::SBlockConfig			BlockConfig					= {::gpk::vcc{32, "01234567890123456789012345678901"}, 65535, true};
		::gpk::vcc					DBName						= {};
		int32_t						MaxBlockOnDisk				= -1;
	};

#pragma pack(push, 1)
	struct SRecordMap {
		int32_t						IdBlock						= -1;
		int32_t						IndexRecord					= -1;
	};
#pragma pack(pop)

	::gpk::error_t				blockRecordIndices			(const uint64_t idRecord, uint32_t blockSize, ::gpk::SRecordMap & indices);
	::gpk::error_t				blockRecordId				(const ::gpk::SRecordMap & indices, uint32_t blockSize, uint64_t & idRecord);
	::gpk::error_t				blockRecordPath				(::gpk::ac & fileName, const ::gpk::SRecordMap & indices, const ::gpk::vcc & dbName, const ::gpk::vcc & dbPath);

	stainli	::gpk::error_t		blockRecordPath				(::gpk::ac & fileName, ::gpk::SRecordMap & indices, const uint64_t idRecord, uint32_t blockSize, const ::gpk::vcc & dbName, const ::gpk::vcc & dbPath) {
		::gpk::blockRecordIndices(idRecord, blockSize, indices);
		return ::gpk::blockRecordPath(fileName, indices, dbName, dbPath);
	}

	::gpk::error_t				blockFileName				(::gpk::ac & fileName, const uint32_t idBlock, const ::gpk::vcc & dbName, const ::gpk::vcc & folderName);
	::gpk::error_t				blockFilePath				(::gpk::ac & finalPath, const ::gpk::vcc & dbName, const ::gpk::vcc & dbPath);

	template<typename _tElement>
	::gpk::error_t				blockMapLoad				(::gpk::ac & loadedBytes, ::gpk::SMapTable<_tElement> & mapTable, const ::gpk::vcc & fileName, const ::gpk::SRecordMap & indexMap, uint8_t maxBlocksInMemory = 16)								{
		(void)maxBlocksInMemory;
		loadedBytes.clear();
		for(uint32_t iBlock = 0; iBlock < mapTable.Id.size(); ++iBlock)
			rvi_if(iBlock, (uint32_t)indexMap.IdBlock == mapTable.Id[iBlock], "Block already loaded: %u.", iBlock);

		::gpk::pobj<_tElement>			newBlock;
		newBlock.create();
		::gpk::error_t					indexBlock					= mapTable.Block.push_back(newBlock);
		gpk_necs(mapTable.Id.push_back(indexMap.IdBlock));
		gpk_necall(::gpk::fileToMemorySecure(loadedBytes, fileName, mapTable.BlockConfig.Key, mapTable.BlockConfig.Deflate), "Failed to load block file: %s.", ::gpk::toString(fileName).begin());
		return indexBlock;
	}

	template<typename _tElement>
	::gpk::error_t				blockMapLoad				(::gpk::ac & loadedBytes, ::gpk::SRecordMap & indexMap, ::gpk::SMapTable<_tElement> & mapBlock, const ::gpk::vcc & dbName, const ::gpk::vcc & dbPath, uint64_t idRecord)								{
		::gpk::apod<char>				fileName					= {};
		gpk_necs(::gpk::blockRecordPath(fileName, indexMap, idRecord, mapBlock.BlockConfig.BlockSize, dbName, dbPath));
		return ::gpk::blockMapLoad(loadedBytes, mapBlock, fileName, indexMap);
	}

	template<typename _tElement>
	::gpk::error_t				blockMapSave				(const ::gpk::vcu8 & blockBytes, const ::gpk::SRecordMap & indexMap, const ::gpk::SMapTable<_tElement> & mapBlock, const ::gpk::vcc & dbName, const ::gpk::vcc & dbPath)								{
		::gpk::ac						finalPath					= {};
		gpk_necs(::gpk::blockFilePath(finalPath, dbName, dbPath));
		::gpk::ac						fileName					= {};
		gpk_necs(::gpk::blockFileName(fileName, indexMap.IdBlock, dbName, finalPath));
		return ::gpk::fileFromMemorySecure(blockBytes, fileName, mapBlock.BlockConfig.Key, mapBlock.BlockConfig.Deflate);
	}

	template<typename _tMapBlock>
	::gpk::error_t				mapTableMapGet				(::gpk::SMapTable<_tMapBlock> & mapTable, const uint64_t idRecord, const ::gpk::vcc & dbPath, ::gpk::ac & outputData) {
		::gpk::SRecordMap				recordMap;
		::gpk::apod<char>				fileBytes;
		::gpk::error_t					indexBlock					= ::gpk::blockMapLoad(fileBytes, recordMap, mapTable, mapTable.DBName, dbPath, idRecord);
		gpk_necall(indexBlock, "Invalid record id: %llu.", idRecord);
		_tMapBlock						& loadedBlock				= *mapTable.Block[indexBlock];
		if(fileBytes.size())
			gpk_necall(loadedBlock.Load(fileBytes), "Failed to load block data for record %llu.", idRecord);

		return loadedBlock.MapGet(recordMap.IndexRecord, outputData);
	}

	//stainli	int16_t	mapTableContainerFromData	(const ::gpk::vcc & sequenceToFind, const uint32_t countContainers)	{
	//	uint64_t					container					= 0;
	//	::gpk::crcGenerate(sequenceToFind, container);
	//	return (int16_t)(container % countContainers);
	//}

	//stainli	uint32_t				mapTableContainerFromId		(const uint32_t blockId, const uint32_t countContainers)	{ return blockId % countContainers; }

	template<typename _tMapBlock>
	int64_t						mapTableMapId				(::gpk::SMapTable<_tMapBlock> & mapTable, const ::gpk::vcc & dbPath, const ::gpk::vcc & sequenceToFind)	{
		::gpk::au32			idBlocks					= mapTable.Id;
		::gpk::au32			blocksToSkip;
		for(uint32_t iBlock = 0; iBlock < mapTable.Block.size(); ++iBlock) {
			const _tMapBlock				& block						= *mapTable.Block[iBlock];
			const int32_t					idEmail						= block.MapId(sequenceToFind);
			const uint32_t					idBlock						= mapTable.Id[iBlock];
			if(0 <= idEmail) {
				::gpk::SRecordMap				indices;
				indices.IdBlock				= idBlock;
				indices.IndexRecord			= idEmail;
				uint64_t						result						= (uint64_t)-1LL;
				::gpk::blockRecordId(indices, mapTable.BlockConfig.BlockSize, result);
				return result;
			}
			blocksToSkip.push_back(idBlock);
			(void)block;
		}
		::gpk::apod<char>				containerPath				= {};
		::gpk::blockFilePath(containerPath, mapTable.DBName, dbPath);
		::gpk::aapod<char>				paths;
		::gpk::pathCreate(containerPath);
		::gpk::pathList(containerPath, paths, false, {});
		constexpr ::gpk::vcs				extension					= ::gpk::EXTENSION_BLOCK_FILE;
		::gpk::apod<char>					loadedBytes;
		for(uint32_t iFile = 0; iFile < paths.size(); ++iFile) {
			const ::gpk::vcc				fileNameCurrent				= paths[iFile];
			if(extension.size() + mapTable.DBName.size() >= fileNameCurrent.size())
				continue;

			::gpk::vcc					filePart;//					= {&fileNameCurrent[fileNameCurrent.size() - extension.size()], extension.size()};
			gpk_necs(fileNameCurrent.slice(filePart, fileNameCurrent.size() - extension.size(), extension.size()));
			if(filePart != extension)
				continue;

			//filePart				= {fileNameCurrent.begin(), fileNameCurrent.size() - extension.size()};
			gpk_necs(fileNameCurrent.slice(filePart, 0, fileNameCurrent.size() - extension.size()));
			::gpk::error_t				indexOfPrevDot				= ::gpk::rfind('.', filePart);
			if(-1 == indexOfPrevDot)
				continue;

			filePart				= {&filePart[indexOfPrevDot], filePart.size() - indexOfPrevDot};
			uint32_t					idBlock						= (uint32_t)-1;
			::gpk::parseIntegerDecimal(filePart, idBlock);
			mapTable.MaxBlockOnDisk	= ::gpk::max(mapTable.MaxBlockOnDisk, (int32_t)idBlock);
			if(0 <= ::gpk::find(idBlock, ::gpk::vcu32{blocksToSkip.begin(), blocksToSkip.size()}))
				continue;

			::gpk::SRecordMap			indexMap					= {};
			indexMap.IdBlock		= idBlock;
			indexMap.IndexRecord	= -1;
			loadedBytes.clear();
			const ::gpk::error_t		indexBlock					= ::gpk::blockMapLoad(loadedBytes, mapTable, fileNameCurrent, indexMap);
			_tMapBlock					& block						= *mapTable.Block[indexBlock];
			gpk_necall(block.Load(loadedBytes), "Failed to load block file: %s.", ::gpk::toString(fileNameCurrent).begin());
			const int32_t				indexRecord					= block.MapId(sequenceToFind);
			if(0 <= indexRecord) {
				indexMap.IndexRecord	= indexRecord;
				uint64_t					result						= (uint64_t)-1LL;
				::gpk::blockRecordId(indexMap, mapTable.BlockConfig.BlockSize, result);
				return result;
			}
		}
		return -1;
	}

	template<typename _tMapBlock>
	int64_t						mapTableMapAdd				(::gpk::SMapTable<_tMapBlock> & mapTable, const ::gpk::vcc & dbPath, const ::gpk::vcc & sequenceToAdd)	{
		int64_t							idRecord					= ::gpk::mapTableMapId(mapTable, dbPath, sequenceToAdd);
		ree_if(0 <= idRecord, "Sequence already registered: %s.", ::gpk::toString(sequenceToAdd).begin());
		if(-1 == mapTable.MaxBlockOnDisk) {
			::gpk::pobj<_tMapBlock>			newBlock;
			newBlock.create();
			::gpk::SRecordMap				newIndices;
			gpk_necs(newIndices.IndexRecord = newBlock->MapAdd(sequenceToAdd));
			newIndices.IdBlock			= 0;
			mapTable.Block	.push_back(newBlock);
			mapTable.Id		.push_back(newIndices.IdBlock);
			++mapTable.MaxBlockOnDisk;
			uint64_t						newIdRecord					= (uint64_t)-1LL;
			::gpk::blockRecordId(newIndices, mapTable.BlockConfig.BlockSize, newIdRecord);
			::gpk::apod<char>				bytesToWrite;
			gpk_necs(newBlock->Save(bytesToWrite));
			gpk_necall(::gpk::blockMapSave(bytesToWrite, newIndices, mapTable, mapTable.DBName, dbPath), "Failed to add record! Disk full? %s. %s.", ::gpk::toString(mapTable.DBName).begin(), ::gpk::toString(dbPath).begin());
			return newIdRecord;
		}
		for(uint32_t iBlock = 0, countBlocks = mapTable.Block.size(); iBlock < countBlocks; ++iBlock) {
			const uint32_t					idBlockInMemory				= mapTable.Id[iBlock];
			_tMapBlock						& blockInMemory				= *mapTable.Block[iBlock];
			if(idBlockInMemory == ((uint32_t)mapTable.MaxBlockOnDisk) && blockInMemory.Size() < (int32_t)mapTable.BlockConfig.BlockSize) {
				::gpk::SRecordMap				newIndices;
				gpk_necs(newIndices.IndexRecord = blockInMemory.MapAdd(sequenceToAdd));
				newIndices.IdBlock			= idBlockInMemory;
				uint64_t						newIdRecord					= (uint64_t)-1LL;
				::gpk::blockRecordId(newIndices, mapTable.BlockConfig.BlockSize, newIdRecord);
				::gpk::apod<char>				bytesToWrite;
				gpk_necs(blockInMemory.Save(bytesToWrite));
				gpk_necall(::gpk::blockMapSave(bytesToWrite, newIndices, mapTable, mapTable.DBName, dbPath), "Failed to add record! Disk full? %s. %s.", ::gpk::toString(mapTable.DBName).begin(), ::gpk::toString(dbPath).begin());
				return newIdRecord;
			}
		}

		::gpk::SRecordMap				indexMap					= {};
		indexMap.IdBlock			= mapTable.MaxBlockOnDisk;
		::gpk::apod<char>				containerPath				= {};
		::gpk::apod<char>				fileNameCurrent				= {};
		::gpk::aobj<::gpk::apod<char>>paths;
		::gpk::blockFilePath(containerPath, mapTable.DBName, dbPath);
		::gpk::blockFileName(indexMap.IdBlock, mapTable.DBName, containerPath, fileNameCurrent);
		::gpk::apod<char>				loadedBytes;
		const ::gpk::error_t			indexBlock					= ::gpk::blockMapLoad(loadedBytes, mapTable, fileNameCurrent, indexMap);
		if(indexBlock >= 0) {
			_tMapBlock						& block						= *mapTable.Block[indexBlock];
			gpk_necall(block.Load(loadedBytes), "Failed to load block file: %s.", ::gpk::toString(fileNameCurrent).begin());
		}

		::gpk::pathCreate(containerPath);
		return idRecord;
	}

	struct SMapBlock {
		::gpk::CViewManager<uint8_t>			Allocator;

		inline	::gpk::error_t					Size						()										const	{ return Allocator.Counts.size(); }
		inline	::gpk::error_t					Save						(::gpk::apod<uint8_t> & output)			const	{ return Allocator.Save(output); }
		inline	::gpk::error_t					Load						(::gpk::vcu8 & input)							{ return Allocator.Load(input); }
		inline	::gpk::error_t					MapAdd		(const ::gpk::vcu8 & dataToAdd)					{
			return Allocator.View(dataToAdd.begin(), (uint16_t)dataToAdd.size());
		}

		::gpk::error_t							MapId		(const ::gpk::vcu8 & dataToFind)		const;
		::gpk::error_t							MapGet						(uint32_t index, ::gpk::vcu8 & data)	const;
	};
} // namespace

#endif // GPK_BLOCK_H_23627
