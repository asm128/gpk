
#include "llc_keyval_old.h"
#include "llc_range.h"
#include "llc_json.h"
#include "llc_http.h"
#include "llc_expression.h"
#include "llc_deflate.h"

#ifndef BLITTER_H_20190712
#define BLITTER_H_20190712

namespace blt
{
	LLC_USING_TYPEINT();
	using llc::err_t;

	GDEFINE_ENUM_TYPE(FIELD_DISPLAY, u0_t );
	GDEFINE_ENUM_VALUE(FIELD_DISPLAY, select, 0);
	GDEFINE_ENUM_VALUE(FIELD_DISPLAY, edit, 1);

	struct SBindDescription {
		void											* HandleModule				= 0;
		void											* FuncModule				= 0;
		::llc::vcst_t							TableName					= {};
		s2_t											TableIndex					= -1;
		FIELD_DISPLAY									Display						= ::blt::FIELD_DISPLAY_select	;
	};

	typedef ::llc::kv<::llc::vcst_t, SBindDescription>	TKeyValBindDescription;

	struct STableDescription {
		::llc::aobj<::llc::SJSONFieldBinding	>	Fields						= {};
		::llc::aobj<::llc::TKeyValConstChar	>	FieldMaps					= {};
		::llc::aobj<::blt::TKeyValBindDescription>	FieldMapDescription			= {};
		::llc::apod<s2_t>						IndicesFieldToMap			= {};
		::llc::apod<s2_t>						IndicesMapToField			= {};
		::llc::vcst_t							TableName					= {};
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

	err_t					tableFolderName						(::llc::string & foldername	, const ::llc::vcst_t & dbName, const u2_t  block);
	err_t					blockFileName						(::llc::string & filename	, const ::llc::vcst_t & dbName, bool bEncrypted, const ::blt::DATABASE_HOST hostType, const u2_t  block);
	err_t					tableFileName						(::llc::string & filename	, const ::blt::DATABASE_HOST & hostType, bool bEncrypted, const ::llc::vcst_t & jsonDBKey);
	err_t					tableFolderInit						(::llc::string & finalFolderName, const ::llc::vcst_t & dbPath, const ::llc::vcst_t & dbName, const u2_t  blockSize);

	stxp	u3_c					MAX_TABLE_RECORD_COUNT				= 0x7FFFFFFFFFFFFFFF;
	stxp	u3_c					MAX_TABLE_BLOCKS_IN_MEMORY			= 1024;
	struct SDatabase {
		::llc::aobj<::llc::vcst_t>		Bindings							= {};
		u2_t							BlockSize							= 0;
		::llc::au3_t					Offsets								= {};
		::llc::apobj<::llc::SJSONFile>	Blocks								= {};
		::llc::au2_t					BlockDirty							= {};
		::llc::au2_t					BlockIndices						= {};
		::llc::au3_t					BlockTimes							= {};
		::llc::vcst_t					EncryptionKey						= {};
		::blt::DATABASE_HOST			HostType							= ::blt::DATABASE_HOST_LOCAL;
		::llc::au2_t					BlocksOnDisk						= {};
		s2_t							MaxBlockOnDisk						= -1;
		::blt::STableDescription		Description							= {};
	};

	struct SBlitterQuery {
		::llc::vcst_t					Database							= "";
		::llc::vcst_t					Command								= "";
		::llc::vcst_t					Path								= "";
		::llc::vcst_t					Record								= "";
		::llc::rangeu3_t				Range								= {0, MAX_TABLE_RECORD_COUNT};
		::llc::view<::llc::vcst_t>		ExpansionKeys						= {};
		s3_t							Detail								= -1;
		::llc::SJSONReader				RecordReader						= {};
	};

	//struct SBlitterOutput {
	//	::llc::string									CurrentOutput;
	//	::llc::aobj<::llc::pobj<::llc::apod<byte_t>>>	BlocksToWrite;
	//};

	typedef ::llc::keyval<::llc::vcst_t, ::blt::SDatabase>	TNamedBlitterDB;

	s3_t			queryProcess
		( ::llc::SLoadCache						& loadCache
		, ::llc::aobj<::blt::TNamedBlitterDB>	& databases
		, const ::llc::SExpressionReader		& expressionReader
		, const ::blt::SBlitterQuery			& query
		, const ::llc::vcst_t						& folder
		, ::llc::string						& output
		);

	struct SWriteCache {
		::llc::SLoadCache		& LoadCache		;
		::llc::string		PartFileName	;
		::llc::string		PathToWriteTo	;
	};

	struct SBlitter {
		::llc::aobj<::blt::TNamedBlitterDB>	Databases				= {};
		::blt::SBlitterQuery				Query					= {};
		::llc::SJSONFile					Config					= {};
		::llc::vcst_t							Folder					= {};
		::llc::aobj<::llc::vcst_t>				ExpansionKeyStorage		= {};
		::llc::SExpressionReader			ExpressionReader		= {};
		::llc::SLoadCache					LoadCache				= {};
	};

	err_t			blockFileLoad			(::llc::SLoadCache & loadCache, ::blt::TNamedBlitterDB & jsonDB, const ::llc::vcst_t & folder, u2_t  block);
	err_t			tableFileLoad			(::llc::SLoadCache & loadCache, ::blt::TNamedBlitterDB & jsonDB, const ::llc::vcst_t & folder);
	err_t			configDatabases			(::llc::aobj<::blt::TNamedBlitterDB> & databases, const ::llc::SJSONReader & configReader, const s2_t indexConfigNode, const ::llc::view_array<const ::llc::vcst_t> & databasesToLoad, const ::llc::vcst_t & folder);

	err_t			blockWrite				(::blt::SWriteCache & writeCache, const ::llc::vcst_t & dbFolderName, const ::llc::vcst_t & dbName, const ::llc::vcst_t & encryptionKey, ::blt::DATABASE_HOST hostType, const ::llc::vcu0_t & partBytes, u2_t  iPart);

	err_t			loadConfig				(::blt::SBlitter & appState, const ::llc::vcst_t & jsonFileName);

	err_t			blitterFlush			(::blt::SBlitter & appState);

	err_t			requestProcess			(::llc::SExpressionReader & expressionReader, ::blt::SBlitterQuery & query, const ::llc::SHTTPAPIRequest & request, ::llc::aobj<::llc::vcst_t> & expansionKeyStorage);
	stxp	u2_t	CRC_SEED				= 18973;
} // namespace

#endif // BLITTER_H_20190712
