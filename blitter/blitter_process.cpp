#include "blitter_process.h"
#include "blitter_output.h"

#include "llc_parse.h"
#include "llc_view_bit.h"
#include "llc_bit.h"
#include "llc_chrono.h"

::llc::err_t			blt::blockWrite							(::blt::SWriteCache & writeCache, const ::llc::vcst_t & dbFolderName, const ::llc::vcst_t & dbName, const ::llc::vcst_t & encryptionKey, ::blt::DATABASE_HOST hostType, const ::llc::vcu0_t & partBytes, u2_t  iPart)		{
	::llc::string							partFileName							= writeCache.PartFileName;
	::llc::string							pathToWriteTo							= writeCache.PathToWriteTo;
	::llc::clear(partFileName, pathToWriteTo, writeCache.LoadCache.Deflated, writeCache.LoadCache.Encrypted);
	pathToWriteTo									= dbFolderName;
	if(pathToWriteTo.size() && pathToWriteTo[pathToWriteTo.size() - 1] != '/')
		pathToWriteTo.push_back('/');
	if_fail_fef(::blt::blockFileName(partFileName, dbName, encryptionKey.size() > 0, hostType, iPart), "%s", "??");
	if_fail_fef(pathToWriteTo.append(partFileName), "%s", "Out of memory?");
	return ::llc::fileFromMemorySecure(writeCache.LoadCache, pathToWriteTo, encryptionKey.cu8(), ::llc::bit_true(hostType, ::blt::DATABASE_HOST_DEFLATE), partBytes.cu8());
}

::llc::err_t			databaseFlush							(::blt::SWriteCache & cache, ::blt::TNamedBlitterDB & db, ::llc::vcst_t folder) {
	::llc::view_bit<const u2_t >						dirty									= {db.Val.BlockDirty.begin(), db.Val.Blocks.size()};
	for(u2_t  iBlock = 0; iBlock < db.Val.Blocks.size(); ++iBlock) {
		if(dirty[iBlock]) {
			::llc::string						folderName								= {};
			if_fail_fef(::blt::tableFolderInit(folderName, folder, db.Key, db.Val.BlockSize), "Failed to initialize folder for table '%.*s'. Not enough permissions?", LLC_VIEW_ARGS(db.Key));
			if_fail_fe(::blt::blockWrite(cache, folderName, db.Key, db.Val.EncryptionKey.cc(), db.Val.HostType, db.Val.Blocks[iBlock]->Bytes.cu8(), db.Val.BlockIndices[iBlock]));
		}
	}
	memset(db.Val.BlockDirty.begin(), 0, sizeof(u2_t ) * db.Val.BlockDirty.size());
	return 0;
}

::llc::err_t			blt::blitterFlush						(::blt::SBlitter & appState) {
	::blt::SWriteCache									cache									= {appState.LoadCache};
	for(u2_t  iDatabase = 0; iDatabase < appState.Databases.size(); ++iDatabase) {
		::blt::TNamedBlitterDB								& db									= appState.Databases[iDatabase];
		if_fail_fef(::databaseFlush(cache, db, appState.Folder), "Failed to flush database '%.*s'.", LLC_VIEW_ARGS(db.Key));
	}
	return 0;
}

static	::llc::err_t	queryGetDetail
	( ::llc::SLoadCache								& loadCache
	, const ::llc::SExpressionReader				& expressionReader
	, ::llc::array_obj<::blt::TNamedBlitterDB>		& databases
	, const u2_t 								idxDatabase
	, const ::blt::SBlitterQuery					& query
	, ::llc::string						& output
	, const ::llc::vcst_t					& folder
	, const u2_t 								idxExpand
	) {
	::llc::vcst_t							outputRecord						= {};
	u2_t 											nodeIndex							= (u2_t )-1;
	u2_t 											blockIndex							= (u2_t )-1;
	::blt::TNamedBlitterDB								& databaseToRead					= databases[idxDatabase];
	if(::llc::failed(::blt::recordGet(loadCache, databaseToRead, query.Detail, outputRecord, nodeIndex, blockIndex, folder))) {
		error_printf("Failed to load record detail. Offset: %llu. Length: %llu.", query.Range.Offset, query.Range.Count);
		if_fail_fef(output.append(::llc::vcst_t{"{}"}), "%s", "Out of memory?");
		return 1;
	}
	if(idxExpand >= query.ExpansionKeys.size())
		if_fail_fef(output.append(outputRecord), "%s", "Out of memory?");
	else {
		const ::llc::SJSONFile								& currentDBBlock					= *databaseToRead.Val.Blocks[blockIndex];
		const s2_t										indexRecordNode						= ::llc::jsonArrayValueGet(*currentDBBlock.Reader.Tree[0], nodeIndex);
		const ::llc::vcst_t						fieldToExpand						= query.ExpansionKeys[idxExpand];
		const s2_t										indexValueNode						= ::llc::jsonObjectValueGet(currentDBBlock.Reader, (u2_t )indexRecordNode, {fieldToExpand.begin(), fieldToExpand.size()});
		const ::llc::vcst_t						currentRecordView					= currentDBBlock.Reader.View[indexRecordNode];
		if(0 > indexValueNode) {
			info_printf("Cannot expand field. Field not found: %.*s.", LLC_VIEW_ARGS(fieldToExpand));
			if_fail_fef(output.append(currentRecordView), "%s", "Out of memory?");
		}
		else {
			const ::llc::JSON_TYPE							refNodeTYpe								= currentDBBlock.Reader.Token[indexValueNode].Type;
			if(::llc::JSON_TYPE_INTEGER != refNodeTYpe && ::llc::JSON_TYPE_ARRAY != refNodeTYpe) {
				info_printf("Invalid value type: %s.", ::llc::get_value_label(refNodeTYpe).begin());
				if_fail_fef(output.append(currentRecordView), "%s", "Out of memory?");
			}
			else if(::llc::JSON_TYPE_INTEGER == refNodeTYpe) {
				s3_t											nextTableRecordIndex				= -1LL;
				const ::llc::vcst_t					digitsToDetailView					= currentDBBlock.Reader.View[indexValueNode];
				if_fail_fef(::llc::parseIntegerDecimal(digitsToDetailView, nextTableRecordIndex), "%s", "Out of memory?");

				const char										* appendStart						= currentRecordView.begin();
				const char										* appendStop						= digitsToDetailView.begin();
				if_fail_fef(output.append(appendStart, (u2_t )(appendStop - appendStart)), "%s", "Out of memory?");
				bool											bFound								= false;
				for(u2_t  iDB = 0; iDB < databases.size(); ++iDB) {
					::blt::TNamedBlitterDB							& nextTable							= databases[iDB];
					if(nextTable.Key == fieldToExpand || 0 <= ::llc::find(fieldToExpand, ::llc::view_array<const ::llc::vcst_t>{nextTable.Val.Bindings})) {
						bFound										= true;
						::blt::SBlitterQuery							nextQuery							= query;
						nextQuery.Database							= nextTable.Key;
						nextQuery.Detail							= nextTableRecordIndex;
						if_true_cef(::queryGetDetail(loadCache, expressionReader, databases, iDB, nextQuery, output, folder, idxExpand + 1), "%s", "Failed to unroll detail.");
						break;
					}
				}
				if(false == bFound)
					if_fail_fef(output.append(::llc::vcst_t{"{}"}), "%s", "?¿??");
				appendStart									= digitsToDetailView.end();
				appendStop									= currentRecordView.end();
				if_fail_fef(output.append(appendStart, (u2_t )(appendStop - appendStart)), "%s", "Out of memory?");
			}
			else {
				const char					* appendStart						= currentRecordView.begin();
				const ::llc::vcst_t			arrayToDetailView					= currentDBBlock.Reader.View[indexValueNode];
				const char					* appendStop						= arrayToDetailView.begin();
				if_fail_fef(output.append(appendStart, (u2_t )(appendStop - appendStart)), "%s", "Out of memory?");
				if_fail_fef(output.push_back('['), "%s", "Out of memory?");
				const ::llc::SJSONNode		& arrayNode							= *currentDBBlock.Reader.Tree[indexValueNode];
				bool						bFound								= false;
				for(u2_t  iRef = 0, refCount = ::llc::jsonArraySize(*currentDBBlock.Reader.Tree[indexValueNode]); iRef < refCount; ++iRef) {
					u3_t 					nextTableRecordIndex					= (u3_t )-1LL;
					const ::llc::err_t		indexArrayElement						= ::llc::jsonArrayValueGet(arrayNode, iRef);
					const ::llc::vcst_t			digitsToDetailView						= currentDBBlock.Reader.View[indexArrayElement];
					if_fail_fef(::llc::parseIntegerDecimal(digitsToDetailView, nextTableRecordIndex), "%s", "Out of memory?");
					for(u2_t  iDB = 0; iDB < databases.size(); ++iDB) {
						::blt::TNamedBlitterDB							& nextTable							= databases[iDB];
						if(nextTable.Key == fieldToExpand || 0 <= ::llc::find(fieldToExpand, ::llc::view_array<const ::llc::vcst_t>{nextTable.Val.Bindings})) {
							bFound										= true;
							::blt::SBlitterQuery							nextQuery							= query;
							nextQuery.Database							= nextTable.Key;
							nextQuery.Detail							= nextTableRecordIndex;
							if_true_cef(::queryGetDetail(loadCache, expressionReader, databases, iDB, nextQuery, output, folder, idxExpand + 1), "%s", "Failed to unroll detail.");
							break;
						}
					}
					if(false == bFound)
						if_fail_fef(output.append(::llc::vcst_t{"{}"}), "%s", "?¿??");
					if(refCount - 1 != iRef)
						if_fail_fef(output.push_back(','), "%s", "Out of memory?");
				}
				appendStart									= arrayToDetailView.end();
				appendStop									= currentRecordView.end();
				if_fail_fef(output.push_back(']'), "%s", "Out of memory?");
				if_fail_fef(output.append(appendStart, (u2_t )(appendStop - appendStart)), "%s", "Out of memory?");
			}
		}
	}
	return 0;
}

static	::llc::err_t	fillEmptyBlocks				(const ::llc::vcst_t & emptyBlockData, const u2_t  emptyBlocks, bool appendComma, ::llc::string & output)	{
	for(u2_t  iEmpty = 0; iEmpty < emptyBlocks; ++iEmpty) {
		if_fail_fef(output.append(emptyBlockData), "%s", "Out of memory?");
		if(emptyBlocks - 1 != iEmpty)
			if_fail_fef(output.push_back(','), "%s", "Out of memory?");
	}
	if(appendComma && emptyBlocks)
		if_fail_fef(output.push_back(','), "%s", "Out of memory?");
	return 0;
}

static	::llc::err_t	nextBlock					(const u2_t  iRangeInfo, const u2_t  lastRangeInfo, const ::llc::view_array<const ::blt::SRangeBlockInfo>	& rangeInfo, const ::llc::vcst_t & emptyBlockData, ::llc::string & output) {
	if(rangeInfo.size() -1 != iRangeInfo)
		if_fail_fef(output.push_back(','), "%s", "Out of memory?");

	if(iRangeInfo < lastRangeInfo) {	// Fill inner empty blocks
		const u2_t 										emptyBlocks							= rangeInfo[iRangeInfo + 1].BlockId - rangeInfo[iRangeInfo].BlockId;
		if(emptyBlocks > 1) {
			info_printf("Empty blocks between: %u and %u.", rangeInfo[iRangeInfo].BlockId, rangeInfo[iRangeInfo + 1].BlockId);
			if_fail_fef(::fillEmptyBlocks(emptyBlockData, emptyBlocks - 1, lastRangeInfo != iRangeInfo, output), "%s", "Out of memory?");
		}
	}
	return 0;
}

static	::llc::err_t	queryGetRange
	( ::llc::SLoadCache						& loadCache
	, const ::llc::SExpressionReader		& expressionReader
	, ::llc::aobj<::blt::TNamedBlitterDB>	& databases
	, const u2_t 						idxDatabase
	, const ::blt::SBlitterQuery			& query
	, ::llc::string							& output
	, const ::llc::vcst_t						& folder
	, const u2_t 						idxExpand
	) {
	if(0 == query.Range.Count) {
		if_fail_fef(output.append(::llc::vcst_t{"[]"}), "%s", "Out of memory?");
		return 0;
	}
	::llc::apod<::llc::minmaxu2_t>				relativeIndices						= {};
	::llc::rangeu2_t								blockRange							= {};
	::llc::aobj<::blt::SRangeBlockInfo>				rangeInfo							= {};
	::blt::TNamedBlitterDB									& databaseToRead					= databases[idxDatabase];
	::llc::string								emptyBlockData						= {};
	const ::llc::vcst_t							empty_record						= "{},";
	for(u2_t  iRecord = 0; iRecord < databaseToRead.Val.BlockSize; ++iRecord)
		if_fail_fef(emptyBlockData.append(empty_record), "%s", "Out of memory?");
	if(emptyBlockData.size() > 0) // Remove last comma.
		if_fail_fef(emptyBlockData.resize(emptyBlockData.size() - 1), "%s", "Out of memory?");

	if_fail_fef(::blt::recordRange(loadCache, databaseToRead, query.Range, folder, rangeInfo, blockRange), "Failed to load record range. Offset: %llu. Length: %llu.", query.Range.Offset, query.Range.Count);
	const u2_t 											lastRangeInfo						= rangeInfo.size() - 1;
	if_fail_fef(output.push_back('['), "%s", "Out of memory?");
	if((0 < databaseToRead.Val.BlockSize) && rangeInfo.size()) {	// Fill leading records if the blocks don't exist.
		const u2_t 											recordsToAvoid						= (query.Range.Offset % databaseToRead.Val.BlockSize);
		const u2_t 											emptyBlocks							= rangeInfo[0].BlockId - blockRange.Offset;
		if(0 == recordsToAvoid)
			if_fail_fef(::fillEmptyBlocks(emptyBlockData, emptyBlocks, lastRangeInfo != 0, output), "%s", "Out of memory?");
		else {
			if(emptyBlocks > 0) {
				for(u2_t  iElem = recordsToAvoid; iElem < databaseToRead.Val.BlockSize; ++iElem)
					if_fail_fef(output.append(::llc::vcst_t{"{},"}), "%s", "Out of memory?");
				if_fail_fef(::fillEmptyBlocks(emptyBlockData, emptyBlocks - 1, lastRangeInfo != 0, output), "%s", "Out of memory?");
			}
		}
	}

	if(idxExpand >= query.ExpansionKeys.size()) {
		for(u2_t  iView = 0; iView < rangeInfo.size(); ++iView) {
			const ::llc::vcst_t							rangeView							= rangeInfo[iView].OutputRecords;
			if_fail_fef(output.append(rangeView), "%s", "Out of memory?");
			if_fail_fef(::nextBlock(iView, lastRangeInfo, rangeInfo, emptyBlockData, output), "%s", "??");
		}
	}
	else {
		::blt::SBlitterQuery			elemQuery							= query;
		for(u2_t  iView = 0; iView < rangeInfo.size(); ++iView) {
			const ::blt::SRangeBlockInfo	& blockInfo							= rangeInfo[iView];
			const ::llc::minmaxu2_t			rangeToExpand						= blockInfo.RelativeIndices;
			for(u2_t  iRecord = rangeToExpand.Min; iRecord < rangeToExpand.Max + 1; ++iRecord) {
				elemQuery.Detail								= iRecord + blockInfo.BlockId * (s3_t)databaseToRead.Val.BlockSize;
				if_fail_fef(::queryGetDetail(loadCache, expressionReader, databases, idxDatabase, elemQuery, output, folder, idxExpand), "%s", "??");
				if(rangeToExpand.Max != iRecord)
					if_fail_fef(output.push_back(','), "%s", "Out of memory?");
			}
			if_fail_fef(::nextBlock(iView, lastRangeInfo, rangeInfo, emptyBlockData, output), "%s", "??");
		}
	}

	if((0 < databaseToRead.Val.BlockSize && databaseToRead.Val.BlocksOnDisk.size())) {	// Fill trailing records if the blocks don't exist.
		const u2_t 										recordsToAvoid						= (u2_t )((query.Range.Offset + query.Range.Count) % databaseToRead.Val.BlockSize);
		const u2_t 										lastBlockId							= rangeInfo.size() ? rangeInfo[rangeInfo.size() - 1].BlockId : 0;
		const u2_t 										emptyBlocks							= (blockRange.Offset + blockRange.Count - 1) - lastBlockId;
		if(emptyBlocks > 0) {
			if(0 == recordsToAvoid)
				if_fail_fef(::fillEmptyBlocks(emptyBlockData, emptyBlocks - 1, false, output), "%s", "Out of memory?");
			else {
				if_fail_fef(output.push_back(','), "%s", "Out of memory?");
				if(emptyBlocks > 1)
					if_fail_fef(::fillEmptyBlocks(emptyBlockData, emptyBlocks - 1, true, output), "%s", "Out of memory?");
				for(u2_t  iElem = 0; iElem < recordsToAvoid; ++iElem)
					if_fail_fef(output.append(::llc::vcst_t{"{},"}), "%s", "Out of memory?");
				if_fail_fef(output.resize(output.size()-1), "%s", "Out of memory?");
			}
		}
	}

	if_fail_fef(output.push_back(']'), "%s", "Out of memory?");
	return 0;
}

static	s3_t			pushNewBlock
	( ::llc::SLoadCache			& loadCache
	, ::blt::TNamedBlitterDB	& database
	, const ::llc::vcst_t			& folder
	, const ::llc::SJSONReader	& recordToAdd
	, const s2_t				idMaxBlockOnDisk
	) {
	const u2_t 					idNewBlock			= idMaxBlockOnDisk + 1;
	if(database.Val.Blocks.size() < ::blt::MAX_TABLE_BLOCKS_IN_MEMORY) {
		::llc::pobj<::llc::SJSONFile>	newBlock;
		newBlock->Bytes.push_back('[');
		loadCache.Deflated.clear();
		if_true_fef(0 == recordToAdd.Tree.size(), "Invalid json record! %s.", "Forgot to build the reader from the record?")
		if_fail_fef(::llc::jsonWrite(recordToAdd[0], recordToAdd.View, *(::llc::string*)&loadCache.Deflated), "Failed to write json record! %s", "Invalid format?");
		newBlock->Bytes.append(loadCache.Deflated.cc());
		newBlock->Bytes.push_back(']');
		if_fail_fef(::llc::jsonParse(newBlock->Reader, newBlock->Bytes), "%s", "Failed to parse new block.");
		const u2_t 										indexBlock			= database.Val.Blocks.push_back(newBlock);
		database.Val.BlockIndices.push_back(idNewBlock);
		database.Val.BlocksOnDisk.push_back(idNewBlock);
		database.Val.MaxBlockOnDisk						= ::llc::max(database.Val.MaxBlockOnDisk, (s2_t)idNewBlock);
		database.Val.Offsets.push_back(idNewBlock * database.Val.BlockSize);
		database.Val.BlockDirty.resize((database.Val.Blocks.size() / 32) + 1, 0);
		database.Val.BlockTimes.push_back(::llc::timeCurrentInUs());
		::llc::view_bit<u2_t >{database.Val.BlockDirty.begin(), database.Val.Blocks.size()}[indexBlock]	= true;
	}
	else {
		const u3_t 										* time						= 0;
		const u2_t 										idxBlock					= ::llc::min(::llc::view_array<const u3_t >{database.Val.BlockTimes}, &time);
		::llc::view_bit<u2_t >							dirty						= {database.Val.BlockDirty.begin(), database.Val.Blocks.size()};
		if(dirty[idxBlock]) {
			::blt::SWriteCache								writeCache						= {loadCache};
			if_fail_fef(::databaseFlush(writeCache, database, folder), "Failed to flush table '%.*s'. Not enough permissions?", LLC_VIEW_ARGS(database.Key));
		}
		if_fail_fef(idxBlock, "%s", "Out of memory?");
		::llc::pobj<::llc::SJSONFile>	& pBlock					= database.Val.Blocks[idxBlock];
		::llc::SJSONFile				& newBlock					= *pBlock;
		newBlock.Bytes.clear();
		newBlock.Reader.Reset();
		newBlock.Bytes.push_back('[');
		loadCache.Deflated.clear();
		if_true_fef(0 == recordToAdd.Tree.size(), "Invalid json record! %s.", "Forgot to build the reader from the record?")
		if_fail_fef(::llc::jsonWrite(recordToAdd[0], recordToAdd.View, *(::llc::string*)&loadCache.Deflated), "Failed to write json record! %s", "Invalid format?");
		newBlock.Bytes.append(loadCache.Deflated.cc());
		newBlock.Bytes.push_back(']');
		if_fail_fef(::llc::jsonParse(newBlock.Reader, newBlock.Bytes), "%s", "Failed to parse new block.");
		database.Val.BlockIndices	[idxBlock]			= idNewBlock;
		database.Val.BlocksOnDisk.push_back(idNewBlock);
		database.Val.Offsets		[idxBlock]			= idNewBlock * database.Val.BlockSize;
		database.Val.BlockTimes		[idxBlock]			= ::llc::timeCurrentInUs();
		database.Val.MaxBlockOnDisk						= ::llc::max(database.Val.MaxBlockOnDisk, (s2_t)idNewBlock);
		::llc::view_bit<u2_t >{database.Val.BlockDirty.begin(), database.Val.Blocks.size()}[idxBlock]	= true;
	}
	return idNewBlock * (u3_t )database.Val.BlockSize;
}

static	s3_t								appendRecord
	( ::llc::SLoadCache								& loadCache
	, ::blt::SDatabase								& database
	, const u2_t 								indexBlock
	, const ::llc::SJSONReader						& recordToAdd
	) {
	loadCache.Deflated.clear();
	loadCache.Deflated.push_back(',');
	if_fail_fef(::llc::jsonWrite(recordToAdd.Tree[0], recordToAdd.View, *(::llc::string*)&loadCache.Deflated), "Failed to write json record! %s", "Invalid format?");
	::llc::SJSONFile									& block				= *database.Blocks[indexBlock];
	const u2_t 										insertPos			= block.Bytes.size() - 1;
	const u2_t 										indexOfToken		= block.Reader.Token.size();
	if_fail_fe(block.Bytes.insert(insertPos, loadCache.Deflated.cc()));
	::llc::SJSONReaderState								& stateReader										= block.Reader.StateRead;
	block.Reader.Token[0].Span.End					= block.Reader.Token[0].Span.Begin;
	stateReader.Escaping							= false;
	stateReader.ExpectingSeparator					= true;
	stateReader.InsideString						= false;
	stateReader.NestLevel							= 1;
	stateReader.IndexCurrentElement					= 0;
	stateReader.CurrentElement						= &block.Reader.Token[stateReader.IndexCurrentElement];
	stateReader.DoneReading							= false;
	for(stateReader.IndexCurrentChar = insertPos; stateReader.IndexCurrentChar < (s2_t)block.Bytes.size(); ++stateReader.IndexCurrentChar) {
		if_fail_fef(::llc::jsonParseStep(block.Reader, block.Bytes), "%s", "Parse step failed.");
		if_true_bif(stateReader.DoneReading, "%i json characters read.", stateReader.IndexCurrentChar + 1);
	}
	if_true_fef(stateReader.NestLevel, "Nest level: %i (Needs to be zero).", stateReader.NestLevel);
	block.Reader.View.resize(block.Reader.Token.size());
	for(u2_t  iView = 0; iView < block.Reader.Token.size(); ++iView) {
		::llc::SJSONToken												& currentElement									= block.Reader.Token[iView];
		block.Reader.View[iView]									= ::llc::vcst_t{&block.Bytes[currentElement.Span.Begin], currentElement.Span.End - currentElement.Span.Begin};
	}
	block.Reader.Tree.resize(block.Reader.Token.size());
	for(u2_t  iToken = indexOfToken; iToken < block.Reader.Token.size(); ++iToken) {
		::llc::pobj<::llc::SJSONNode>	& pcurrentNode				= block.Reader.Tree[iToken];
		pcurrentNode->Token			= &block.Reader.Token[iToken];
		::llc::SJSONNode				& currentNode				= *pcurrentNode;
		currentNode.ObjectIndex		= iToken;
		//if(-1 != currentNode.Token->ParentIndex)
		//	currentNode.Parent							= block.Reader.Tree[currentNode.Token->ParentIndex];
	}
	//const s2_t										idxRecordNode			= ::llc::jsonArrayValueGet(block.Reader, 0, ::llc::jsonArraySize(block.Reader, 0) - 1);
	::llc::pobj<::llc::SJSONNode>	& recordNode			= block.Reader.Tree[indexOfToken];
	if(-1 != recordNode->Token->ParentIndex)
		recordNode->Parent			= block.Reader.Tree[recordNode->Token->ParentIndex];
	::llc::SJSONNode				& arrayNode				= *block.Reader.Tree[0];
	arrayNode.Children.push_back(recordNode);
	//::llc::jsonTreeRebuild(block.Reader.Token, block.Reader.Tree);
	//block.Reader.Reset();
	//if_fail_fef(::llc::jsonParse(block.Reader, block.Bytes), "%s", "Failed to read JSON!");
	database.BlockDirty.resize((database.Blocks.size() / 32) + 1, 0);
	::llc::view_bit<u2_t >{database.BlockDirty.begin(), database.Blocks.size()}[indexBlock]	= true;
	return database.Offsets[indexBlock] + arrayNode.Children.size() - 1;
}

s3_t											blt::queryProcess
	( ::llc::SLoadCache								& loadCache
	, ::llc::array_obj<::blt::TNamedBlitterDB>		& databases
	, const ::llc::SExpressionReader				& expressionReader
	, const ::blt::SBlitterQuery					& query
	, const ::llc::vcst_t					& folder
	, ::llc::string						& output
	) {
	if(query.Command == ::llc::vcst_t{"get"}) {
		for(u2_t  iDB = 0; iDB < databases.size(); ++iDB) {
			if(query.Database == databases[iDB].Key)
				return (0 <= query.Detail)
					? ::queryGetDetail	(loadCache, expressionReader, databases, iDB, query, output, folder, 0)
					: ::queryGetRange	(loadCache, expressionReader, databases, iDB, query, output, folder, 0)
					;
		}
	}
	else if(query.Command == ::llc::vcst_t{"model"}) {
		for(u2_t  iDB = 0; iDB < databases.size(); ++iDB) {
			if(query.Database == databases[iDB].Key)
				return ::blt::outputModel(output, databases[iDB].Val.Description);
		}
	}
	else if(query.Command == ::llc::vcst_t{"form"}) {
		for(u2_t  iDB = 0; iDB < databases.size(); ++iDB) {
			if(query.Database == databases[iDB].Key)
				return 0;
		}
	}
	else if(query.Command == ::llc::vcst_t{"push_back"}) {
		for(u2_t  iDB = 0; iDB < databases.size(); ++iDB) {
			::blt::TNamedBlitterDB								& database			= databases[iDB];
			if(query.Database != database.Key)
				continue;
			if(0 == database.Val.BlocksOnDisk.size())
				return ::pushNewBlock(loadCache, database, folder, query.RecordReader, -1);
			else {
				const u2_t 										idBlock				= database.Val.MaxBlockOnDisk;//::llc::max(::llc::vcu2_t{database.Val.BlocksOnDisk});
				u2_t 											indexRecord			= (u2_t )-1;
				u2_t 											indexBlock			= (u2_t )-1;
				if_fail_fef(::blt::recordLoad(loadCache, database, idBlock * (u3_t )database.Val.BlockSize, indexRecord, indexBlock, folder), "Failed to load block for record: %lli.", query.Detail);
				::llc::SJSONFile									& block				= *database.Val.Blocks[indexBlock];
				const ::llc::SJSONNode								& tableNode			= *block.Reader[0];
				if(database.Val.BlockSize && tableNode.Children.size() >= database.Val.BlockSize)
					return ::pushNewBlock(loadCache, database, folder, query.RecordReader, idBlock);
				else
					return ::appendRecord(loadCache, database.Val, indexBlock, query.RecordReader);
			}
		}
	}
	else if(query.Command == ::llc::vcst_t{"pop_back"}) {
		for(u2_t  iDB = 0; iDB < databases.size(); ++iDB) {
			if(query.Database == databases[iDB].Key) {
				return -1;	// Not implemented
		//		u2_t 											idBlock				= 0;;
		//		::llc::max(::llc::vcu2_t{databases[iDB].Val.BlocksOnDisk}, &idBlock);
			}
		}
	}
	else if(query.Command == ::llc::vcst_t{"replace"}) {
		for(u2_t  iDB = 0; iDB < databases.size(); ++iDB) {
			if(query.Database == databases[iDB].Key) {
				return -1;	// Not implemented
		//		u2_t 											idBlock				= 0;;
		//		::llc::max(::llc::vcu2_t{databases[iDB].Val.BlocksOnDisk}, &idBlock);
			}
		}
	}

	error_printf("Database not found: %.*s.", LLC_VIEW_ARGS(query.Database));
	return -1;
}
