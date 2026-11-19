#include "blitter.h"

GPK_USING_TYPEINT();

#include "gpk_stdstring.h"
#include "gpk_file.h"
#include "gpk_path.h"

#include "gpk_json_expression.h"
#include "gpk_noise.h"
#include "gpk_parse.h"
#include "gpk_deflate.h"
#include "gpk_aes.h"
//#include "gpk_cgi.h"
#include "gpk_chrono.h"
#include "gpk_view_bit.h"
#include "gpk_module.h"
#include "gpk_bit.h"

::gpk::err_t			blt::loadConfig				(::blt::SBlitter & appState, const ::gpk::vcst_t & jsonFileName)	{
	if_fail_fef(::gpk::jsonFileRead(appState.Config, jsonFileName), "Failed to load configuration file: %.*s.", GPK_VIEW_ARGS(jsonFileName));
	const ::gpk::err_t							indexApp					= ::gpk::jsonExpressionResolve(::gpk::vcst_t{"application.blitter"	}, appState.Config.Reader, (u2_t )0, appState.Folder);
	const ::gpk::err_t							indexDB						= ::gpk::jsonExpressionResolve(::gpk::vcst_t{"database"				}, appState.Config.Reader, (u2_t )indexApp, appState.Folder);
	if_fail_fef(indexDB, "'database' not found in '%s'.", "application.blitter");
	const ::gpk::err_t							indexPath					= ::gpk::jsonExpressionResolve(::gpk::vcst_t{"path.database"			}, appState.Config.Reader, (u2_t )indexApp, appState.Folder);
	if(::gpk::failed(indexPath)) {
		warning_printf("'path.database' not found in '%s'. Using current path as database root.", "application.blitter");
		appState.Folder									= "./";
	}
	if_fail_fef(::blt::configDatabases(appState.Databases, appState.Config.Reader, indexDB, {}, appState.Folder), "%s", "Failed to load query.");
	return 0;
}

static ::gpk::err_t						dbFileExtension				(const ::blt::DATABASE_HOST & hostType, bool bEncrypted, ::gpk::vcst_t & extension) {
	extension										= bEncrypted
		? ((::blt::DATABASE_HOST_DEFLATE & hostType) ? ::gpk::vcst_t{"czon"} : ::gpk::vcst_t{"cson"})
		: ((::blt::DATABASE_HOST_DEFLATE & hostType) ? ::gpk::vcst_t{"zson"} : ::gpk::vcst_t{"sson"})
		;
	return 0;
}

::gpk::err_t			blt::blockFileName			(::gpk::string & filename, const ::gpk::vcst_t & dbName, bool bEncrypted, const ::blt::DATABASE_HOST hostType, const u2_t  block) {
	if_fail_fef(filename.append(dbName), "%s", "Out of memory?");
	char												temp[64]					= {};
	::gpk::vcst_t							extension					= {};
	::dbFileExtension(hostType, bEncrypted, extension);
	sprintf_s(temp, ".%u.%.*s", block, GPK_VIEW_ARGS(extension));
	if_fail_fef(filename.append(::gpk::vcst_t{temp}), "%s", "Out of memory?");
	return 0;
}

::gpk::err_t			blt::tableFolderName		(::gpk::string & foldername, const ::gpk::vcst_t & dbName, const u2_t  blockSize) {
	if_fail_fef(foldername.append(dbName), "%s", "Out of memory?");
	char												temp[128]					= {};
	sprintf_s(temp, ".%u.db", blockSize);
	if_fail_fef(foldername.append(::gpk::vcst_t{temp}), "%s", "Out of memory?");
	return 0;
}

::gpk::err_t			blt::tableFileName			(::gpk::string & filename, const ::blt::DATABASE_HOST & hostType, bool bEncrypted, const ::gpk::vcst_t & jsonDBKey) {
	if_fail_fef(filename.append(jsonDBKey), "%s", "Out of memory?");
	char												temp[64]					= {};
	::gpk::vcst_t							extension					= {};
	::dbFileExtension(hostType, bEncrypted, extension);
	sprintf_s(temp, ".%.*s", GPK_VIEW_ARGS(extension));
	if_fail_fef(filename.append(::gpk::vcst_t{temp}), "%s", "Out of memory?");
	return 0;
}

::gpk::err_t			blt::tableFolderInit		(::gpk::string & finalFolderName, const ::gpk::vcst_t & dbPath, const ::gpk::vcst_t & dbName, const u2_t  blockSize) {
	finalFolderName										= dbPath;
	if(finalFolderName.size()) {
		if(finalFolderName[finalFolderName.size()-1] != '/' && finalFolderName[finalFolderName.size()-1] != '\\')
			if_fail_fef(finalFolderName.push_back('/'), "%s", "Out of memory?");
		if_fail_fef(::blt::tableFolderName(finalFolderName, dbName, blockSize), "%s", "??");
		if_fail_fef(::gpk::pathCreate({finalFolderName.begin(), finalFolderName.size()}), "Failed to create database folder: %.*s.", GPK_VIEW_ARGS(finalFolderName));
		info_printf("Output folder: %.*s.", GPK_VIEW_ARGS(finalFolderName));
		if(finalFolderName[finalFolderName.size()-1] != '/' && finalFolderName[finalFolderName.size()-1] != '\\')
			if_fail_fef(finalFolderName.push_back('/'), "%s", "Out of memory?");
	}
	else if(0 == finalFolderName.size())
		if_fail_fef(finalFolderName.append(::gpk::vcst_t{"./"}), "%s", "Out of memory?");
	return 0;
}

static	::gpk::err_t		dbFileLoad					(::gpk::SLoadCache & loadCache, ::blt::TNamedBlitterDB & jsonDB, const ::gpk::vcst_t & folder, const ::gpk::vcst_t & fileName, u2_t  idBlock)	{
	if(jsonDB.Val.Blocks.size() < ::blt::MAX_TABLE_BLOCKS_IN_MEMORY) {
		const s2_t										idxBlock					= jsonDB.Val.Blocks.push_back({});
		if_fail_fef(idxBlock																					, "%s", "Out of memory?");
		if_fail_fef(jsonDB.Val.BlockIndices.push_back(idBlock)												, "%s", "Out of memory?");
		if_fail_fef(jsonDB.Val.Offsets.push_back(idBlock * (s3_t)jsonDB.Val.BlockSize)					, "%s", "Out of memory?");
		if_fail_fef(jsonDB.Val.BlockDirty.resize(jsonDB.Val.BlockIndices.size() / sizeof(u2_t ) + 1, 0U)	, "%s", "Out of memory?");
		if_fail_fef(jsonDB.Val.BlockTimes.push_back(::gpk::timeCurrentInUs())								, "%s", "Out of memory?");
		jsonDB.Val.Blocks[idxBlock].create();
		::gpk::SJSONFile									& dbBlock					= *jsonDB.Val.Blocks[idxBlock];
		if_fail_fef(::gpk::fileToMemorySecure(loadCache, fileName, jsonDB.Val.EncryptionKey.bytes(), gpk::bit_true(jsonDB.Val.HostType, ::blt::DATABASE_HOST_DEFLATE), dbBlock.Bytes), "Failed to open block file: %.*s.", GPK_VIEW_ARGS(fileName));
		if_fail_fef(::gpk::jsonParse(dbBlock.Reader, dbBlock.Bytes), "Failed to parse database block file: %.*s.", GPK_VIEW_ARGS(fileName));
		return idxBlock;
	}
	else {
		const u3_t 										* time						= 0;
		const u2_t 										idxBlock					= ::gpk::min(::gpk::view<const u3_t >{jsonDB.Val.BlockTimes}, &time);
		if_fail_fef(idxBlock, "%s", "Out of memory?");
		::gpk::view_bit<u2_t >							dirty						= {jsonDB.Val.BlockDirty.begin(), jsonDB.Val.Blocks.size()};
		if(dirty[idxBlock]) {
			::gpk::string						folderName						= {};
			::blt::SWriteCache								writeCache						= {loadCache};
			if_fail_fef(::blt::tableFolderInit(folderName, folder, jsonDB.Key, jsonDB.Val.BlockSize), "Failed to initialize folder for table '%.*s'. Not enough permissions?", GPK_VIEW_ARGS(jsonDB.Key));
			if_fail_fef(::blt::blockWrite(writeCache, folderName, jsonDB.Key, jsonDB.Val.EncryptionKey, jsonDB.Val.HostType, jsonDB.Val.Blocks[idxBlock]->Bytes.bytes(), jsonDB.Val.BlockIndices[idxBlock]), "%s", "Unknown error!");
			dirty[idxBlock]									= false;
		}

		jsonDB.Val.BlockIndices	[idxBlock]				= idBlock;
		jsonDB.Val.Offsets		[idxBlock]				= (idBlock * (s3_t)jsonDB.Val.BlockSize);
		jsonDB.Val.BlockTimes	[idxBlock]				= ::gpk::timeCurrentInUs();
		::gpk::SJSONFile									& dbBlock					= *jsonDB.Val.Blocks[idxBlock];
		dbBlock.Bytes	.clear();
		dbBlock.Reader	.Reset();
		if_fail_fef(::gpk::fileToMemorySecure(loadCache, fileName, jsonDB.Val.EncryptionKey.bytes(), gpk::bit_true(jsonDB.Val.HostType, ::blt::DATABASE_HOST_DEFLATE), dbBlock.Bytes), "Failed to open block file: %.*s.", GPK_VIEW_ARGS(fileName));
		if_fail_fef(::gpk::jsonParse(dbBlock.Reader, dbBlock.Bytes), "Failed to parse database block file: %.*s.", GPK_VIEW_ARGS(fileName));
		dirty[idxBlock]									= true;
		return idxBlock;
	}
}

::gpk::err_t			blt::tableFileLoad			(::gpk::SLoadCache & loadCache, ::blt::TNamedBlitterDB & jsonDB, const ::gpk::vcst_t & folder)	{
	if_true_vif(0, jsonDB.Val.Blocks.size() && jsonDB.Val.Blocks[0]->Bytes.size(), "Table already loaded: '%.*s'.", GPK_VIEW_ARGS(jsonDB.Key));
	::gpk::string							fileName					= folder;
	if_fail_fef(fileName.push_back('/')		, "%s", "Out of memory?");
	if_fail_fef(::blt::tableFileName(fileName, jsonDB.Val.HostType, jsonDB.Val.EncryptionKey.size() > 0,jsonDB.Key), "%s", "Out of memory?");
	info_printf("Loading database file: %.*s.", GPK_VIEW_ARGS(fileName));
	if_fail_fef(::dbFileLoad(loadCache, jsonDB, folder, fileName, 0), "Failed to load database from file: %.*s.", GPK_VIEW_ARGS(fileName));
	jsonDB.Val.BlocksOnDisk.clear();
	jsonDB.Val.MaxBlockOnDisk						= 0;
	return jsonDB.Val.BlocksOnDisk.push_back(0);
}

::gpk::err_t			blt::blockFileLoad				(::gpk::SLoadCache & loadCache, ::blt::TNamedBlitterDB & jsonDB, const ::gpk::vcst_t & folder, u2_t  block)	{
	{
		::gpk::err_t			blockIndex						= ::gpk::rfind(block, {jsonDB.Val.BlockIndices.begin(), jsonDB.Val.BlockIndices.size()});
		if(0 <= blockIndex) {
			jsonDB.Val.BlockTimes[blockIndex]				= ::gpk::timeCurrentInUs();
			info_printf("Block already loaded for database '%.*s': %u.", GPK_VIEW_ARGS(jsonDB.Key), block);
			return blockIndex;
		}
	}
	if_fail_fef(::gpk::find(block, {jsonDB.Val.BlocksOnDisk}), "Block %u doesn't exist.", block);
	::gpk::string							fileName						= folder;
	if_fail_fef(fileName.push_back('/'), "%s", "Out of memory?");
	if_fail_fef(::blt::tableFolderName(fileName, jsonDB.Key, jsonDB.Val.BlockSize), "%s", "Out of memory?");
	if_fail_fef(fileName.push_back('/'), "%s", "Out of memory?");
	if_fail_fef(::blt::blockFileName(fileName, jsonDB.Key, jsonDB.Val.EncryptionKey.size() > 0, jsonDB.Val.HostType, block), "%s", "Out of memory?");
	info_printf("Loading block file: %.*s.", GPK_VIEW_ARGS(fileName));
	return ::dbFileLoad(loadCache, jsonDB, folder, fileName, block);
}
static	::gpk::err_t		table_description_load			(::blt::STableDescription & tableDesc, const ::gpk::SJSONReader & configReader, const u2_t  iNode, const ::gpk::vcst_t & tableName)	{
	s2_t												indexOfTable					= iNode;//::gpk::jsonExpressionResolve(tableName, configReader, iNode);
	if_fail_fef(indexOfTable, "API configuration (%.*s) not found in JSON file.", GPK_VIEW_ARGS(tableName));
	::gpk::vcst_t							strValue						= {};
	::gpk::jsonExpressionResolve(::gpk::vcst_t{"public"}, configReader, indexOfTable, strValue);
	tableDesc.Public								= strValue == ::gpk::vcst_t{"true"};

	s2_t												indexOfFields					= ::gpk::jsonExpressionResolve(::gpk::vcst_t{"fields"}, configReader, indexOfTable);
	if_true_vwf(0, ::gpk::failed(indexOfFields), "Fields for table (%.*s) not found in JSON file.", GPK_VIEW_ARGS(tableName));
	::gpk::aobj<::gpk::vcst_t>			namesOfFields;
	if_fail_fef(::gpk::jsonObjectKeyList(configReader, indexOfFields, namesOfFields), "Failed to list fields: %s.", "Unknown reason");
	tableDesc.Fields.resize(namesOfFields.size());
	for(u2_t  iKey = 0; iKey < namesOfFields.size(); ++iKey) {
		::gpk::SJSONFieldBinding							& fieldDesc						= tableDesc.Fields[iKey];
		fieldDesc.Field									= namesOfFields[iKey];
		::gpk::vcst_t							fieldName						= {fieldDesc.Field.begin(), fieldDesc.Field.size()};
		const ::gpk::err_t							indexOfValue					= ::gpk::jsonObjectValueGet(configReader, indexOfFields, {fieldDesc.Field.begin(), fieldDesc.Field.size()});
		fieldDesc.Type									= configReader.Token[indexOfValue].Type;
		::gpk::vcst_t								viewOfValue						= configReader.View[indexOfValue];
		if(::gpk::JSON_TYPE_OBJECT == fieldDesc.Type) {
			[[maybe_unused]] const ::gpk::err_t							indexOfMode					= ::gpk::jsonObjectValueGet(configReader, indexOfValue, ::gpk::vcst_t{"resolve"});
			const ::gpk::err_t							indexOfDisplay				= ::gpk::jsonObjectValueGet(configReader, indexOfValue, ::gpk::vcst_t{"display"});
			::blt::TKeyValBindDescription						& bindDescription			= tableDesc.FieldMapDescription[tableDesc.FieldMapDescription.push_back({fieldName, {}})];
			//bindDescription.Val.Resolve						= (0 > indexOfMode		) ? ::blt::FIELD_RESOLVE_api	: ::gpk::get_value<::blt::FIELD_RESOLVE>(configReader.View[indexOfMode]		);
			bindDescription.Val.Display						= (0 > indexOfDisplay	) ? ::blt::FIELD_DISPLAY_select	: ::gpk::get_value<::blt::FIELD_DISPLAY>(configReader.View[indexOfDisplay]	);
			const ::gpk::err_t							indexOfBind					= ::gpk::jsonObjectValueGet(configReader, indexOfValue, ::gpk::vcst_t{"table"});
			::gpk::vcst_t								otherTableName				= configReader.View[indexOfBind];
			::gpk::string							moduleName						= ::gpk::vcst_t("blt_");
			moduleName.append(otherTableName);
			bindDescription.Val.HandleModule				= GPK_LOAD_MODULE(moduleName.begin());
			if(bindDescription.Val.HandleModule)
				bindDescription.Val.FuncModule					= GPK_LOAD_MODULE_FUNCTION(bindDescription.Val.HandleModule, "blt_output_generate");;
			tableDesc.FieldMaps.push_back({fieldName, otherTableName});
		}
	}
	if_fail_fef(::gpk::jsonMapToFields(tableDesc.IndicesMapToField, tableDesc.Fields, tableDesc.FieldMaps), "Failed to generate field indices for table '%.*s'.", GPK_VIEW_ARGS(tableName));
	if_fail_fef(::gpk::jsonFieldsToMap(tableDesc.IndicesFieldToMap, tableDesc.Fields, tableDesc.FieldMaps), "Failed to generate field indices for table '%.*s'.", GPK_VIEW_ARGS(tableName));
	tableDesc.TableName								= tableName;
	return 0;
}

::gpk::err_t			blt::configDatabases		(::gpk::array_obj<::blt::TNamedBlitterDB> & databases, const ::gpk::SJSONReader & configReader, const s2_t indexConfigNode, const ::gpk::view<const ::gpk::vcst_t> & databasesToLoad, const ::gpk::vcst_t & folder)	{
	::gpk::vcst_t							jsonResult					= {};
	const s2_t										indexObjectDatabases		= (-1 == indexConfigNode)
		? ::gpk::jsonExpressionResolve(::gpk::vcst_t{"application.blitter.database"}, configReader, 0, jsonResult)
		: indexConfigNode
		;
	if_fail_fef(indexObjectDatabases, "%s", "Failed to get database config from JSON file.");
	jsonResult										= "";
	const ::gpk::err_t							databaseArraySize			= ::gpk::jsonArraySize(*configReader[indexObjectDatabases]);
	if_fail_fef(databaseArraySize, "%s", "Failed to get database count from config file.");
	char												temp[64]					= {};
	if_fail_fef(databases.resize(databaseArraySize), "%s", "Out of memory?");
	::gpk::SLoadCache									loadCache;
	for(u2_t  iDatabase = 0, countDatabases = (u2_t )databaseArraySize; iDatabase < countDatabases; ++iDatabase) {
		sprintf_s(temp, "['%u']", iDatabase);
		const s2_t										iJSONTable					=::gpk::jsonExpressionResolve(::gpk::vcst_t{temp}, configReader, indexObjectDatabases, jsonResult);
		if_fail_fef(iJSONTable, "Failed to load config from json! Last contents found: %.*s.", GPK_VIEW_ARGS(jsonResult));
		if_fail_fef(::gpk::jsonExpressionResolve(::gpk::vcst_t{"name"}, configReader, iJSONTable, jsonResult), "Failed to load config from json! Last contents found: %.*s.", GPK_VIEW_ARGS(jsonResult));
		::blt::TNamedBlitterDB								& jsonDB					= databases[iDatabase];
		jsonDB.Key										= jsonResult;
		if_fail_fef(::table_description_load(jsonDB.Val.Description, configReader, iJSONTable, jsonDB.Key), "%s", "Unknown error");
		{	// -- Load database block size
			s2_t												indexBlockNode				= ::gpk::jsonExpressionResolve(::gpk::vcst_t{"block"}, configReader, iJSONTable, jsonResult);
			if_true_wf(::gpk::failed(indexBlockNode), "Failed to load config from json! Last contents found: %.*s.", GPK_VIEW_ARGS(jsonResult))
			else
				::gpk::parseIntegerDecimal(jsonResult, (jsonDB.Val.BlockSize = 0));
		}
		{	// -- Load database block size
			s2_t												indexKeyNode				= ::gpk::jsonExpressionResolve(::gpk::vcst_t{"key"}, configReader, iJSONTable, jsonResult);
			if_true_wf(::gpk::failed(indexKeyNode), "Failed to load config from json! Last contents found: %.*s.", GPK_VIEW_ARGS(jsonResult))
			else
				jsonDB.Val.EncryptionKey = jsonResult;
		}
		{	// -- Load database modes (remote, deflate)
			info_printf("Loading database info for '%.*s'.", GPK_VIEW_ARGS(jsonDB.Key));
			jsonResult										= {};
			s2_t												typeFound					= ::gpk::jsonExpressionResolve(::gpk::vcst_t{"source"}, configReader, iJSONTable, jsonResult);
			if_true_wf(::gpk::failed(typeFound), "Failed to load database type for database: %.*s. Defaulting to local.", GPK_VIEW_ARGS(jsonDB.Key));
			jsonDB.Val.HostType								= (::gpk::vcst_t{"local"} == jsonResult || ::gpk::failed(typeFound)) ? ::blt::DATABASE_HOST_LOCAL : ::blt::DATABASE_HOST_REMOTE;
			jsonResult										= {};
			typeFound										= ::gpk::jsonExpressionResolve(::gpk::vcst_t{"deflate"}, configReader, iJSONTable, jsonResult);
			if_true_wf(::gpk::failed(typeFound), "Failed to load database compression for database: %.*s. Defaulting to uncompressed.", GPK_VIEW_ARGS(jsonDB.Key));
			if(::gpk::vcst_t{"true"} == jsonResult)
				jsonDB.Val.HostType								|= ::blt::DATABASE_HOST_DEFLATE;
		}
		{	// -- Load field bindings
			::gpk::err_t			indexBindArray				= ::gpk::jsonExpressionResolve(::gpk::vcst_t{"bind"}, configReader, iJSONTable, jsonResult);
			if_true_wf(::gpk::failed(indexBindArray), "No bindings found for database file: %.*s.", GPK_VIEW_ARGS(jsonDB.Key))
			else {
				::gpk::err_t			sizeBindArray				= ::gpk::jsonArraySize(*configReader[indexBindArray]);
				if_fail_fef(sizeBindArray, "Cannot get size of array: %s.", "??");
				if_fail_fef(jsonDB.Val.Bindings.resize(sizeBindArray), "%s", "Out of memory?");;
				for(u2_t  iBind = 0; iBind < jsonDB.Val.Bindings.size(); ++iBind) {
					sprintf_s(temp, "['%u']", iBind);
					if_fail_fef(::gpk::jsonExpressionResolve(::gpk::vcst_t{temp}, configReader, indexBindArray, jsonResult), "Failed to load config from json! Last contents found: %.*s.", GPK_VIEW_ARGS(jsonResult));
					jsonDB.Val.Bindings[iBind]						= jsonResult;
				}
			}
		}
		::gpk::string							dbfilename					= {};
		if_fail_fef(::blt::tableFileName(dbfilename, jsonDB.Val.HostType, jsonDB.Val.EncryptionKey.size() > 0, jsonDB.Key), "%s", "??");
		if(gpk::bit_true(jsonDB.Val.HostType, ::blt::DATABASE_HOST_REMOTE))
			continue;

		if(jsonDB.Val.BlockSize) {	// block databases get loaded on-demand. However, we need to know which blocks are available on disk in order to fill missing blocks with empty records.
			::gpk::vcst_t								extension;
			::dbFileExtension(jsonDB.Val.HostType, jsonDB.Val.EncryptionKey.size() > 0, extension);
			::gpk::string								folderName			= folder;
			if_fail_fef(folderName.push_back('/'), "Failed to load database: %.*s. Out of memory?", GPK_VIEW_ARGS(dbfilename));
			if_fail_fef(::blt::tableFolderName(folderName, jsonDB.Key, jsonDB.Val.BlockSize), "Failed to load database: %.*s. Out of memory?", GPK_VIEW_ARGS(dbfilename));
			::gpk::array_obj<::gpk::string>				blockFiles;
			if(::gpk::failed(::gpk::pathList({folderName.begin(), folderName.size()}, blockFiles)))
				if_fail_fef(::gpk::pathCreate(folderName), "Failed to load database: %.*s. Out of memory?", GPK_VIEW_ARGS(dbfilename));

			for(u2_t  iFile = 0; iFile < blockFiles.size(); ++iFile) {
				const ::gpk::vcst_t							pathBlock			= blockFiles[iFile];
				if(pathBlock.size() >= extension.size()) {
					if(extension != ::gpk::vcst_t{&pathBlock[pathBlock.size() - extension.size()], extension.size()})
						continue;
				}
				const ::gpk::vcst_t							blockDigits			= {&pathBlock[folderName.size() + jsonDB.Key.size() + 2], pathBlock.size() - folderName.size() - extension.size() - jsonDB.Key.size() - 3};
				u3_t 												blockIndex			= 0;
				if_fail_fef(::gpk::parseIntegerDecimal(blockDigits, blockIndex), "Failed to load database: %.*s. Out of memory?", GPK_VIEW_ARGS(dbfilename));
				bool													bAdd				= true;
				for(u2_t  i = 0; i < jsonDB.Val.BlocksOnDisk.size(); ++i)
					if(((u2_t )blockIndex) < jsonDB.Val.BlocksOnDisk[i]) {
						if_fail_fef(jsonDB.Val.BlocksOnDisk.insert(i, (u2_t )blockIndex), "%s.", "Out of memory?");
						jsonDB.Val.MaxBlockOnDisk							= ::gpk::max(jsonDB.Val.MaxBlockOnDisk, (s2_t)blockIndex);
						bAdd												= false;
						break;
					}

				if(bAdd) {
					jsonDB.Val.MaxBlockOnDisk							= ::gpk::max(jsonDB.Val.MaxBlockOnDisk, (s2_t)blockIndex);
					if_fail_fef(jsonDB.Val.BlocksOnDisk.push_back((u2_t )blockIndex), "Failed to load database: %.*s. Out of memory?", GPK_VIEW_ARGS(dbfilename));
				}
			}
			for(u2_t  i = 0; i < jsonDB.Val.BlocksOnDisk.size(); ++i)
				info_printf("Database block found for '%.*s': %u.", GPK_VIEW_ARGS(dbfilename), jsonDB.Val.BlocksOnDisk[i]);
			continue;
		}
		// -- Load json database file.
		if(0 == databasesToLoad.size()) {
			if_fail_fef(::blt::tableFileLoad(loadCache, jsonDB, folder), "Failed to load database: %.*s.", GPK_VIEW_ARGS(dbfilename));
		}
		else {
			for(u2_t  iDB = 0; iDB = databasesToLoad.size(); ++iDB)
				if(databasesToLoad[iDB] == jsonDB.Key) {
					if_fail_fef(::blt::tableFileLoad(loadCache, jsonDB, folder), "Failed to load database: %.*s.", GPK_VIEW_ARGS(dbfilename));
					break;
				}
		}
	}
	return 0;
}

static	::gpk::err_t		queryLoad						(::blt::SBlitterQuery& query, const ::gpk::view<const ::gpk::keyval<::gpk::vcst_t>> keyvals, ::gpk::array_obj<::gpk::vcst_t> & expansionKeyStorage) {
	::gpk::keyvalNumeric("offset", keyvals, query.Range.Offset);
	if(0 > ::gpk::keyvalNumeric("limit", keyvals, query.Range.Count))
		::gpk::keyvalNumeric("count", keyvals, query.Range.Count);

	const ::gpk::err_t							indexExpand						= ::gpk::find(::gpk::vcst_t{"expand"}, keyvals);
	if(0 <= indexExpand) {
		if_fail_fef(::gpk::split(::gpk::vcst_t{keyvals[indexExpand].Val}, '.', expansionKeyStorage), "%s", "Out of memory?");
		query.ExpansionKeys								= expansionKeyStorage;
	}
	return 0;
}

::gpk::err_t			blt::requestProcess				(::gpk::SExpressionReader & expressionReader, ::blt::SBlitterQuery & query, const ::gpk::SHTTPAPIRequest & request, ::gpk::array_obj<::gpk::vcst_t> & expansionKeyStorage)						{
	// --- Generate response
	{
		::gpk::array_obj<::gpk::vcst_t>			pathSplit						= {};
		::gpk::split(request.Path, '/', pathSplit);
		query.Database									= (0 < pathSplit.size()) ? pathSplit[0] : request.Path;
		query.Command									= (1 < pathSplit.size()) ? pathSplit[1] : ::gpk::vcst_t{};
		if(2 < pathSplit.size()) {
			// --- Retrieve detail part
			u3_t 											detail							= (u3_t )-1LL;
			::gpk::vcst_t								strDetail						= {};
			strDetail										= pathSplit[pathSplit.size() - 1];
			if(strDetail.size()) {
				::gpk::parseIntegerDecimal(strDetail, detail);
				query.Detail									= (s3_t)detail;
			}
		}
		info_printf("Parameters: \nDatabase: %.*s.\nCommand: %.*s.\nDetail: %llu.", GPK_VIEW_ARGS(query.Database), GPK_VIEW_ARGS(query.Command), query.Detail);
	}
	{ // retrieve path and database and read expression if any
		const ::gpk::err_t							indexPath						= ::gpk::find('.', query.Database);
		if(((u2_t )indexPath + 1) < query.Database.size())
			query.Path										= {&query.Database[indexPath + 1]	, query.Database.size() - (indexPath + 1)};

		if(0 <= indexPath)
			query.Database									= {query.Database.begin()			, (u2_t )indexPath};

		if(query.Path.size())
			if_fail_fef(::gpk::expressionReaderParse(expressionReader, {query.Path.begin(), query.Path.size()}), "Error: %s", "Invalid path syntax?");
	}
	query.Record									= request.ContentBody;
	if_fail_fef(::gpk::jsonParse(query.RecordReader, query.Record), "Failed to parse JSON record '%.*s'", GPK_VIEW_ARGS(query.Record));
	// --- Retrieve query data from querystring.
	if_fail_fef(::queryLoad(query, request.QueryStringKeyVals, expansionKeyStorage), "%s", "Out of memory?");
	return 0;
}
