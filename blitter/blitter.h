
#include "gpk_keyval_view.h"
#include "gpk_range.h"
#include "gpk_json.h"
#include "gpk_http.h"
#include "gpk_expression.h"
#include "gpk_deflate.h"
#include "gpk_file_pack.h"

#ifndef BLITTER_H_20190712
#define BLITTER_H_20190712

namespace blt
{
	GPK_USING_TYPEINT();
	using gpk::err_t;

	GDEFINE_ENUM_TYPE(FIELD_DISPLAY, u0_t );
	GDEFINE_ENUM_VALUE(FIELD_DISPLAY, select, 0);
	GDEFINE_ENUM_VALUE(FIELD_DISPLAY, edit, 1);

	struct SBindDescription {
		void											* HandleModule				= 0;
		void											* FuncModule				= 0;
		::gpk::vcst_t							TableName					= {};
		s2_t											TableIndex					= -1;
		FIELD_DISPLAY									Display						= ::blt::FIELD_DISPLAY_select	;
	};

	typedef ::gpk::kv<::gpk::vcst_t, SBindDescription>	TKeyValBindDescription;

	struct STableDescription {
		::gpk::aobj<::gpk::SJSONFieldBinding	>	Fields						= {};
		::gpk::aobj<::gpk::keyval<::gpk::vcst_t>	>	FieldMaps					= {};
		::gpk::aobj<::blt::TKeyValBindDescription>	FieldMapDescription			= {};
		::gpk::apod<s2_t>						IndicesFieldToMap			= {};
		::gpk::apod<s2_t>						IndicesMapToField			= {};
		::gpk::vcst_t							TableName					= {};
		bool											Public						= false;
	};
} // namespace

namespace blt
{
	GDEFINE_ENUM_TYPE (DATABASE_HOST, s0_t );
	GDEFINE_ENUM_VALUE(DATABASE_HOST, LOCAL				, 0);
	GDEFINE_ENUM_VALUE(DATABASE_HOST, REMOTE			, 1);
	GDEFINE_ENUM_VALUE(DATABASE_HOST, DEFLATE			, 2);
	GDEFINE_ENUM_VALUE(DATABASE_HOST, REMOTE_DEFLATE	, 3);

	err_t					tableFolderName						(::gpk::string & foldername	, const ::gpk::vcst_t & dbName, const u2_t  block);
	err_t					blockFileName						(::gpk::string & filename	, const ::gpk::vcst_t & dbName, bool bEncrypted, const ::blt::DATABASE_HOST hostType, const u2_t  block);
	err_t					tableFileName						(::gpk::string & filename	, const ::blt::DATABASE_HOST & hostType, bool bEncrypted, const ::gpk::vcst_t & jsonDBKey);
	err_t					tableFolderInit						(::gpk::string & finalFolderName, const ::gpk::vcst_t & dbPath, const ::gpk::vcst_t & dbName, const u2_t  blockSize);

	stxp	u3_c					MAX_TABLE_RECORD_COUNT				= 0x7FFFFFFFFFFFFFFF;
	stxp	u3_c					MAX_TABLE_BLOCKS_IN_MEMORY			= 1024;
	struct SDatabase {
		::gpk::aobj<::gpk::vcst_t>		Bindings							= {};
		u2_t							BlockSize							= 0;
		::gpk::au3_t					Offsets								= {};
		::gpk::apobj<::gpk::SJSONFile>	Blocks								= {};
		::gpk::au2_t					BlockDirty							= {};
		::gpk::au2_t					BlockIndices						= {};
		::gpk::au3_t					BlockTimes							= {};
		::gpk::vcst_t					EncryptionKey						= {};
		::blt::DATABASE_HOST			HostType							= ::blt::DATABASE_HOST_LOCAL;
		::gpk::au2_t					BlocksOnDisk						= {};
		s2_t							MaxBlockOnDisk						= -1;
		::blt::STableDescription		Description							= {};
	};

	struct SBlitterQuery {
		::gpk::vcst_t					Database							= "";
		::gpk::vcst_t					Command								= "";
		::gpk::vcst_t					Path								= "";
		::gpk::vcst_t					Record								= "";
		::gpk::rangeu3_t				Range								= {0, MAX_TABLE_RECORD_COUNT};
		::gpk::view<::gpk::vcst_t>		ExpansionKeys						= {};
		s3_t							Detail								= -1;
		::gpk::SJSONReader				RecordReader						= {};
	};

	//struct SBlitterOutput {
	//	::gpk::string									CurrentOutput;
	//	::gpk::aobj<::gpk::pobj<::gpk::apod<byte_t>>>	BlocksToWrite;
	//};

	typedef ::gpk::keyval<::gpk::vcst_t, ::blt::SDatabase>	TNamedBlitterDB;

	s3_t			queryProcess
		( ::gpk::SLoadCache						& loadCache
		, ::gpk::aobj<::blt::TNamedBlitterDB>	& databases
		, const ::gpk::SExpressionReader		& expressionReader
		, const ::blt::SBlitterQuery			& query
		, const ::gpk::vcst_t						& folder
		, ::gpk::string						& output
		);

	struct SWriteCache {
		::gpk::SLoadCache		& LoadCache		;
		::gpk::string		PartFileName	;
		::gpk::string		PathToWriteTo	;
	};

	struct SBlitter {
		::gpk::aobj<::blt::TNamedBlitterDB>	Databases				= {};
		::blt::SBlitterQuery				Query					= {};
		::gpk::SJSONFile					Config					= {};
		::gpk::vcst_t							Folder					= {};
		::gpk::aobj<::gpk::vcst_t>				ExpansionKeyStorage		= {};
		::gpk::SExpressionReader			ExpressionReader		= {};
		::gpk::SLoadCache					LoadCache				= {};
	};

	err_t			blockFileLoad			(::gpk::SLoadCache & loadCache, ::blt::TNamedBlitterDB & jsonDB, const ::gpk::vcst_t & folder, u2_t  block);
	err_t			tableFileLoad			(::gpk::SLoadCache & loadCache, ::blt::TNamedBlitterDB & jsonDB, const ::gpk::vcst_t & folder);
	err_t			configDatabases			(::gpk::aobj<::blt::TNamedBlitterDB> & databases, const ::gpk::SJSONReader & configReader, const s2_t indexConfigNode, ::gpk::view<::gpk::vcst_c> databasesToLoad, const ::gpk::vcst_t & folder);

	err_t			blockWrite				(::blt::SWriteCache & writeCache, const ::gpk::vcst_t & dbFolderName, const ::gpk::vcst_t & dbName, const ::gpk::vcst_t & encryptionKey, ::blt::DATABASE_HOST hostType, const ::gpk::vcu0_t & partBytes, u2_t  iPart);

	err_t			loadConfig				(::blt::SBlitter & appState, const ::gpk::vcst_t & jsonFileName);

	err_t			blitterFlush			(::blt::SBlitter & appState);

	err_t			requestProcess			(::gpk::SExpressionReader & expressionReader, ::blt::SBlitterQuery & query, const ::gpk::SHTTPAPIRequest & request, ::gpk::aobj<::gpk::vcst_t> & expansionKeyStorage);
	stxp	u2_t	CRC_SEED				= 18973;
} // namespace

#endif // BLITTER_H_20190712
