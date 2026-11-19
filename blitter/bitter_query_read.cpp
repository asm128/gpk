#include "blitter_process.h"

GPK_USING_TYPEINT();

::gpk::err_t			blt::recordLoad
	( ::gpk::SLoadCache					& loadCache
	, ::blt::TNamedBlitterDB			& database
	, const u3_t 					absoluteIndex
	, u2_t 							& relativeIndex
	, u2_t 							& blockIndex
	, const ::gpk::vcst_t		& folder
	) {
	const u2_t 										indexBlock								= (0 == database.Val.BlockSize) ? (u2_t )-1	: (u2_t )(absoluteIndex / database.Val.BlockSize);
	const s2_t										iBlockElem								= (0 == database.Val.BlockSize)
		? ::blt::tableFileLoad(loadCache, database, folder)
		: ::blt::blockFileLoad(loadCache, database, folder, indexBlock)
		;
	if_fail_fef(iBlockElem, "Failed to load database file: %s.", "??");
	const u3_t 										offsetRecord							= database.Val.Offsets[iBlockElem];
	relativeIndex									= ::gpk::max(0U, (u2_t )(absoluteIndex - offsetRecord));
	blockIndex										= iBlockElem;
	return 0;
}

::gpk::err_t			blt::recordGet
	( ::gpk::SLoadCache					& loadCache
	, ::blt::TNamedBlitterDB			& database
	, const u3_t 					absoluteIndex
	, ::gpk::vcst_t			& output_record
	, u2_t 							& relativeIndex
	, u2_t 							& blockIndex
	, const ::gpk::vcst_t		& folder
	) {
	if_fail_fef(::blt::recordLoad(loadCache, database, absoluteIndex, relativeIndex, blockIndex, folder), "%s", "Failed to load record indices.");

	const ::gpk::SJSONReader							& readerBlock							= database.Val.Blocks[blockIndex]->Reader;
	if_true_fef(0 == readerBlock.Tree.size(), "%s", "Invalid block data.");
	const ::gpk::SJSONNode								& jsonRoot								= *readerBlock.Tree[0];
	if_true_fef(::gpk::JSON_TYPE_ARRAY != jsonRoot.Token->Type, "Invalid json type: %s", ::gpk::get_value_label(jsonRoot.Token->Type).begin());
	if_true_fef(relativeIndex >= (u2_t )::gpk::jsonArraySize(*readerBlock[0]), "Index out of range: %i. Array size: %u.", relativeIndex, ::gpk::jsonArraySize(*readerBlock[0]));
	output_record									= readerBlock.View[::gpk::jsonArrayValueGet(*readerBlock[0], relativeIndex)];
	return 0;
}

::gpk::err_t			recordRangeBlock
	( const ::blt::TNamedBlitterDB				& database
	, const ::gpk::rangeu3_t						& range
	, const u2_t 							idBlock
	, const u2_t 							iNewBlock
	, ::gpk::aobj<::blt::SRangeBlockInfo>	& output_records
	) {
	const ::gpk::SJSONReader	& readerBlock		= database.Val.Blocks[iNewBlock]->Reader;
	if_true_fef(0 == readerBlock.Tree.size(), "%s", "Invalid block data.");
	const ::gpk::SJSONNode		& jsonRoot			= *readerBlock.Tree[0];
	if_true_fef(::gpk::JSON_TYPE_ARRAY != jsonRoot.Token->Type, "Invalid json type: %s", ::gpk::get_value_label(jsonRoot.Token->Type).begin());

	const u3_t 				offsetRecord		= database.Val.Offsets[iNewBlock];
	::blt::SRangeBlockInfo		rangeInfo			= {};
	// Actuallly I believe we should handle this differently than for single-file database access, but it is possible that I already thought about this before.
	rangeInfo.RelativeIndices.Min	= ::gpk::max(0, (s2_t)(range.Offset - offsetRecord));
	rangeInfo.RelativeIndices.Max	= ::gpk::min(::gpk::jsonArraySize(jsonRoot) - 1U, ::gpk::min(database.Val.BlockSize - 1, (u2_t )((range.Offset + range.Count) - offsetRecord - 1)));
	if(rangeInfo.RelativeIndices.Max < rangeInfo.RelativeIndices.Min) {
		rangeInfo.OutputRecords	= {};
		warning_printf("Invalid range: {Offset: %i, Count: %i}", range.Offset, range.Count);
		return 0;
	}

	rangeInfo.BlockIndex							= iNewBlock;
	rangeInfo.BlockId								= idBlock;
	const ::gpk::minmaxs2_t					blockNodeIndices	=
		{ ::gpk::jsonArrayValueGet(jsonRoot, rangeInfo.RelativeIndices.Min)
		, ::gpk::jsonArrayValueGet(jsonRoot, rangeInfo.RelativeIndices.Max)
		};
	const ::gpk::vcst_t						viewRecordFirst				= readerBlock.View[(((u2_t )blockNodeIndices.Min) < readerBlock.View.size()) ? blockNodeIndices.Min : 0];
	const ::gpk::vcst_t						viewRecordLast				= readerBlock.View[blockNodeIndices.Max];
	const u2_t 							charCount	= (u2_t )
		( viewRecordLast.end()
		- viewRecordFirst.begin()
		);
	rangeInfo.OutputRecords							=
		{ viewRecordFirst.begin()
		, charCount
		};
	if_fail_fef(output_records.push_back(rangeInfo), "%s", "Out of memory?");
	return 0;
}

::gpk::err_t			blt::recordRange
	( ::gpk::SLoadCache						& loadCache
	, ::blt::TNamedBlitterDB				& database
	, const ::gpk::rangeu3_t					& range
	, const ::gpk::vcst_t						& folder
	, ::gpk::aobj<::blt::SRangeBlockInfo>	& output_records
	, ::gpk::rangeu2_t						& blockRange
	) {
	const u3_t 										maxRecord			= ((range.Count == ::blt::MAX_TABLE_RECORD_COUNT) ? range.Count : range.Offset + range.Count);
	u2_t 											blockStart			= (0 == database.Val.BlockSize) ? 0				: (u2_t )range.Offset / database.Val.BlockSize;
	u2_t 											blockStop			= (0 == database.Val.BlockSize) ? (u2_t )-1	: (u2_t )(maxRecord / database.Val.BlockSize);
	if(0 < database.Val.BlockSize) {
		if(0 == database.Val.BlocksOnDisk.size()) {
			blockStart										= 0;
			blockStop										= 0;
		}
		else {
			if(blockStop > database.Val.BlocksOnDisk[database.Val.BlocksOnDisk.size() - 1])
				blockStop									= database.Val.BlocksOnDisk[database.Val.BlocksOnDisk.size() - 1];
			if(blockStart > database.Val.BlocksOnDisk[database.Val.BlocksOnDisk.size() - 1])
				blockStart									= database.Val.BlocksOnDisk[database.Val.BlocksOnDisk.size() - 1];
		}
	}
	blockRange										= {blockStart, blockStop - blockStart + 1};

	if(0 == database.Val.BlockSize) {
		s2_t												iNewBlock			= ::blt::tableFileLoad(loadCache, database, folder);
		if_fail_fef(iNewBlock, "%s", "Missing table found.");	// We need to improve this in order to support missing blocks.
		if_fail_fef(::recordRangeBlock(database, range, 0, iNewBlock, output_records), "Error accessing range for block: %u.", 0);
	}
	else {
		::gpk::array_pod<u2_t >							blocksToProcess		= {};
		for(u2_t  iBlockOnDisk = 0; iBlockOnDisk < database.Val.BlocksOnDisk.size(); ++iBlockOnDisk) {
			const u2_t 										blockOnDisk			= database.Val.BlocksOnDisk[iBlockOnDisk];
			if(::gpk::in_range(blockOnDisk, blockRange.Offset, blockRange.Offset + blockRange.Count))
				if_fail_fef(blocksToProcess.push_back(blockOnDisk), "%s.", "Out of memory?");
		}
		for(u2_t  iBlock = 0; iBlock < blocksToProcess.size(); ++iBlock) {
			const u2_t 										idBlockToLoad		= blocksToProcess[iBlock];
			s2_t												iNewBlock			= ::blt::blockFileLoad(loadCache, database, folder, idBlockToLoad);
			if_fail_fef(iNewBlock, "Missing block found: %u. This shouldn't happen because we already filtered which blocks are on disk, so either the block is corrupted and it has to be restored before the query executes.", idBlockToLoad);	// We need to improve this in order to support missing blocks.
			if_fail_fef(::recordRangeBlock(database, range, idBlockToLoad, iNewBlock, output_records), "Error accessing range for block: %u.", idBlockToLoad);
		}
	}
	return 0;
}
