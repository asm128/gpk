#include "gpk_enum.h"
#include "gpk_bit.h"
#include "gpk_array_ptr.h"
#include "gpk_slice.h"
#include "gpk_string.h"

#ifndef GPK_JSON_H_23627
#define GPK_JSON_H_23627

namespace gpk
{
#pragma pack(push, 1)
#ifdef GPK_ARDUINO
	tydf  s1_t json_id_t;
	GDEFINE_ENUM_TYPE(JSON_TYPE, s0_t);
#else
	tydf  s2_t json_id_t;
	GDEFINE_ENUM_TYPE(JSON_TYPE, s2_t);
#endif
	GDEFINE_ENUM_VALUE(JSON_TYPE, NULL		,  0);
	GDEFINE_ENUM_VALUE(JSON_TYPE, INTEGER	,  1);
	GDEFINE_ENUM_VALUE(JSON_TYPE, DECIMAL	,  2);
	GDEFINE_ENUM_VALUE(JSON_TYPE, BOOLEAN	,  3);
	GDEFINE_ENUM_VALUE(JSON_TYPE, STRING	,  4);
	GDEFINE_ENUM_VALUE(JSON_TYPE, KEY		,  5);
	GDEFINE_ENUM_VALUE(JSON_TYPE, VALUE		,  6);
	GDEFINE_ENUM_VALUE(JSON_TYPE, OBJECT	,  7);
	GDEFINE_ENUM_VALUE(JSON_TYPE, ARRAY		,  8);
	GDEFINE_ENUM_VALUE(JSON_TYPE, CODEPOINT	,  9);
	GDEFINE_ENUM_VALUE(JSON_TYPE, COUNT		, 10);
	GDEFINE_ENUM_VALUE(JSON_TYPE, UNKNOWN	, -1);
	GDEFINE_FLAG_TYPE(JSON_PARSE_OPTION, u1_t);
	GDEFINE_FLAG_VALUE(JSON_PARSE_OPTION, NONE						, 0x0000U);
	GDEFINE_FLAG_VALUE(JSON_PARSE_OPTION, BUILD_TREE					, 0x0001U);
	GDEFINE_FLAG_VALUE(JSON_PARSE_OPTION, BUILD_VIEWS				, 0x0002U);
	GDEFINE_FLAG_VALUE(JSON_PARSE_OPTION, FINAL_INPUT				, 0x0004U);
	GDEFINE_FLAG_VALUE(JSON_PARSE_OPTION, ALLOW_LINE_COMMENTS		, 0x0008U);
	GDEFINE_FLAG_VALUE(JSON_PARSE_OPTION, ALLOW_TRAILING_COMMAS		, 0x0010U);
	GDEFINE_FLAG_VALUE(JSON_PARSE_OPTION, ALLOW_LEADING_PLUS			, 0x0020U);
	GDEFINE_FLAG_VALUE(JSON_PARSE_OPTION, ALLOW_LEADING_DECIMAL_POINT	, 0x0040U);
	GDEFINE_FLAG_VALUE(JSON_PARSE_OPTION, ALLOW_TRAILING_DECIMAL_POINT	, 0x0080U);
	GDEFINE_FLAG_VALUE(JSON_PARSE_OPTION, ALLOW_LEADING_ZEROES			, 0x0100U);
	GDEFINE_FLAG_VALUE(JSON_PARSE_OPTION, ALLOW_TRAILING_CONTENT		, 0x0200U);
	GDEFINE_FLAG_VALUE(JSON_PARSE_OPTION, ALLOW_UNESCAPED_CONTROLS		, 0x0400U);
	GDEFINE_FLAG_VALUE(JSON_PARSE_OPTION, CONTAINER_ROOT_ONLY			, 0x0800U);
	tydf bit_field<JSON_PARSE_OPTION> SJSONParseOptions;
	stxp SJSONParseOptions JSON_PARSE_STRICT		= {JSON_PARSE_OPTION_BUILD_TREE | JSON_PARSE_OPTION_BUILD_VIEWS | JSON_PARSE_OPTION_FINAL_INPUT};
	stxp SJSONParseOptions JSON_PARSE_LEGACY_ROOT	= {JSON_PARSE_OPTION(JSON_PARSE_STRICT) | JSON_PARSE_OPTION_CONTAINER_ROOT_ONLY};
	stxp SJSONParseOptions JSON_PARSE_GPK			= {JSON_PARSE_OPTION(JSON_PARSE_STRICT)
		| JSON_PARSE_OPTION_ALLOW_LINE_COMMENTS | JSON_PARSE_OPTION_ALLOW_TRAILING_COMMAS | JSON_PARSE_OPTION_ALLOW_LEADING_PLUS
		| JSON_PARSE_OPTION_ALLOW_LEADING_DECIMAL_POINT | JSON_PARSE_OPTION_ALLOW_TRAILING_DECIMAL_POINT | JSON_PARSE_OPTION_ALLOW_LEADING_ZEROES
		| JSON_PARSE_OPTION_ALLOW_TRAILING_CONTENT | JSON_PARSE_OPTION_ALLOW_UNESCAPED_CONTROLS};
	stct SJSONToken {
		json_id_t				ParentIndex;
		JSON_TYPE				Type;
		sliceu2_t				Span;
		uint64_t				Value;
	};
	stct SJSONNode {
		cnst SJSONToken			* Token				= 0;
		SJSONNode				* Parent			= 0;
		apobj<SJSONNode>		Children			= {};
		json_id_t				ObjectIndex			= -1;
	};
	stct SJSONReaderState {
		u2_t					IndexCurrentChar	= 0;
		json_id_t				IndexCurrentElement	= -1;
		SJSONToken				* CurrentElement	= 0;
		s1_t					NestLevel			= 0;
		sc_t					CharCurrent			= 0;
		b8_t					Escaping			: 1;
		b8_t					InsideString		: 1;
		b8_t					ExpectingSeparator	: 1;
		b8_t					DoneReading			: 1;
								SJSONReaderState	()
		: Escaping			(false)
		, InsideString		(false)
		, ExpectingSeparator(false)
		, DoneReading		(false)
		{}
	};
#pragma pack(pop)

	stct SJSONReader {
		apod<SJSONToken>		Token;
		aobj<vcst_t>			View;
		apobj<SJSONNode>		Tree;
		SJSONReaderState		StateRead;
		
		cnst pobj<SJSONNode>&	oper[]	(u2_t index)	cnst	{ rtrn Tree[index]; }
		cnst pobj<SJSONNode>*	begin	()				cnst	{ rtrn Tree.begin(); }
		cnst pobj<SJSONNode>*	end		()				cnst	{ rtrn Tree.end(); }
		err_t					Reset	()						{
			::gpk::clear(Token, View);
			StateRead				= {};
			rtrn 0;
		}
	};

	// Reader functions: Populate a SJSONReader structure from an input JSON string.
	err_t			jsonParse			(SJSONReader & reader, vcst_t jsonAsString, SJSONParseOptions options = JSON_PARSE_GPK);
	err_t			jsonParseStep		(SJSONReader & reader, vcst_t jsonAsString, SJSONParseOptions options = JSON_PARSE_GPK);
	stin err_t		jsonParse			(SJSONReader & reader, vcst_t jsonAsString, bool buildTree, bool buildViews = true) {
		SJSONParseOptions options = JSON_PARSE_GPK;
		options.Set(JSON_PARSE_OPTION_BUILD_TREE , buildTree );
		options.Set(JSON_PARSE_OPTION_BUILD_VIEWS, buildViews);
		rtrn jsonParse(reader, jsonAsString, options);
	}
	err_t			jsonTreeRebuild		(view<cnst SJSONToken> in_object, apobj<SJSONNode> & out_nodes);

	nsix	err_t	jsonArraySize		(cnst SJSONNode & node)												nxpt	{ rtrn node.Children.size(); }	// returns the index of the JSON element corresponding to the index provided as parameter.
	nsix	err_t	jsonObjectKeyCount	(cnst SJSONNode & node)												nxpt	{ rtrn node.Children.size() / 2; }
	err_t			jsonArrayValueGet	(cnst SJSONNode & node, u2_t index);	// returns the index of the JSON element corresponding to the index provided as parameter.
	err_t			jsonObjectValueGet	(cnst SJSONNode & node, view<cnst vcst_t> views, vcst_t key);
	err_t			jsonObjectKeyList	(cnst SJSONNode & node, as2_t & indices);
	err_t			jsonObjectKeyList	(cnst SJSONNode & node, view<cnst vcst_t> views, aobj<vcst_t> & keys);
	err_t			jsonObjectKeyList	(cnst SJSONNode & node, view<cnst vcst_t> views, as2_t & indices, aobj<vcst_t> & keys);
	err_t			jsonCompareNumber	(cnst SJSONNode & node, view<cnst vcst_t> views, cnst SJSONNode & other, view<cnst vcst_t> otherViews);
	err_t			jsonCompareArray	(cnst SJSONNode & node, view<cnst vcst_t> views, cnst SJSONNode & other, view<cnst vcst_t> otherViews);
	err_t			jsonCompareObject	(cnst SJSONNode & node, view<cnst vcst_t> views, cnst SJSONNode & other, view<cnst vcst_t> otherViews);


	// Access functions: Iterate over children looking for keys or values.
	ndsi	err_t	jsonArraySize		(cnst SJSONReader & reader, u2_t iNode)											{ if_true_fwf(iNode >= reader.Token.size(), "Invalid node index: %" GPK_FMT_S2 ". Total nodes: %" GPK_FMT_U2, iNode, reader.Token.size()); if_true_fe(!reader.Tree[iNode]); rtrn jsonArraySize(*reader.Tree[iNode]); }
	ndsi	err_t	jsonObjectKeyCount	(cnst SJSONReader & reader, u2_t iNode)											{ if_true_fwf(iNode >= reader.Token.size(), "Invalid node index: %" GPK_FMT_S2 ". Total nodes: %" GPK_FMT_U2, iNode, reader.Token.size()); if_true_fe(!reader.Tree[iNode]); rtrn jsonObjectKeyCount(*reader.Tree[iNode]); }
	ndsi	err_t	jsonArrayValueGet	(cnst SJSONReader & reader, u2_t iNode, u2_t indexOfValueToGet)					{ if_true_fwf(iNode >= reader.Token.size(), "Invalid node index: %" GPK_FMT_S2 ". Total nodes: %" GPK_FMT_U2, iNode, reader.Token.size()); if_true_fe(!reader.Tree[iNode]); rtrn jsonArrayValueGet(*reader.Tree[iNode], indexOfValueToGet); }
	ndsi	err_t	jsonObjectValueGet	(cnst SJSONReader & reader, u2_t iNode, vcst_t key)								{ if_true_fwf(iNode >= reader.Token.size(), "Invalid node index: %" GPK_FMT_S2 ". Total nodes: %" GPK_FMT_U2, iNode, reader.Token.size()); if_true_fe(!reader.Tree[iNode]); rtrn jsonObjectValueGet(*reader.Tree[iNode], reader.View, {key.begin(), key.size()}); }
	stin	err_t	jsonObjectKeyList	(cnst SJSONReader & reader, u2_t iNode, as2_t & indices)						{ if_true_fwf(iNode >= reader.Token.size(), "Invalid node index: %" GPK_FMT_S2 ". Total nodes: %" GPK_FMT_U2, iNode, reader.Token.size()); if_true_fe(!reader.Tree[iNode]); rtrn jsonObjectKeyList(*reader.Tree[iNode], indices); }
	stin	err_t	jsonObjectKeyList	(cnst SJSONReader & reader, u2_t iNode, aobj<vcst_t> & keys)					{ if_true_fwf(iNode >= reader.Token.size(), "Invalid node index: %" GPK_FMT_S2 ". Total nodes: %" GPK_FMT_U2, iNode, reader.Token.size()); if_true_fe(!reader.Tree[iNode]); rtrn jsonObjectKeyList(*reader.Tree[iNode], reader.View, keys); }
	stin	err_t	jsonObjectKeyList	(cnst SJSONReader & reader, u2_t iNode, as2_t & indices, aobj<vcst_t> & keys)	{ if_true_fwf(iNode >= reader.Token.size(), "Invalid node index: %" GPK_FMT_S2 ". Total nodes: %" GPK_FMT_U2, iNode, reader.Token.size()); if_true_fe(!reader.Tree[iNode]); rtrn jsonObjectKeyList(*reader.Tree[iNode], reader.View, indices, keys); }

	ndsi	err_t	jsonCompareArray	(cnst SJSONNode & node, cnst SJSONNode & other, view<cnst vcst_t> views)	{ rtrn jsonCompareArray		(node, views, other, views); }
	ndsi	err_t	jsonCompareObject	(cnst SJSONNode & node, cnst SJSONNode & other, view<cnst vcst_t> views)	{ rtrn jsonCompareObject	(node, views, other, views); }
	ndsi	err_t	jsonCompareNumber	(cnst SJSONNode & node, cnst SJSONNode & other, view<cnst vcst_t> views)	{ rtrn jsonCompareNumber	(node, views, other, views); }

	err_t	jsonObjectGetCString(cnst SJSONReader & reader, u2_t iNode, vcst_t	& value);
	err_t	jsonObjectGetString (cnst SJSONReader & reader, u2_t iNode, vcst_t	& value);
	err_t	jsonObjectGetBoolean(cnst SJSONReader & reader, u2_t iNode, b8_t	& value);
	err_t	jsonObjectGetInteger(cnst SJSONReader & reader, u2_t iNode, u0_t	& value);
	err_t	jsonObjectGetInteger(cnst SJSONReader & reader, u2_t iNode, u1_t	& value);
	err_t	jsonObjectGetInteger(cnst SJSONReader & reader, u2_t iNode, u2_t	& value);
	err_t	jsonObjectGetInteger(cnst SJSONReader & reader, u2_t iNode, u3_t	& value);
	err_t	jsonObjectGetInteger(cnst SJSONReader & reader, u2_t iNode, s0_t	& value);
	err_t	jsonObjectGetInteger(cnst SJSONReader & reader, u2_t iNode, s1_t	& value);
	err_t	jsonObjectGetInteger(cnst SJSONReader & reader, u2_t iNode, s2_t	& value);
	err_t	jsonObjectGetInteger(cnst SJSONReader & reader, u2_t iNode, s3_t	& value);
	err_t	jsonObjectGetDecimal(cnst SJSONReader & reader, u2_t iNode, f2_t	& value);
	err_t	jsonObjectGetDecimal(cnst SJSONReader & reader, u2_t iNode, f3_t	& value);

	// TODO: Finish
	//err_t	jsonObjectGetIntegerAsString (cnst SJSONReader & reader, u2_t iNode, vcst_t	& value);
	err_t	jsonObjectGetIntegerAsBoolean(cnst SJSONReader & reader, u2_t iNode, b8_t	& value);
	err_t	jsonObjectGetIntegerAsDecimal(cnst SJSONReader & reader, u2_t iNode, f3_t	& value);
	err_t	jsonObjectGetDecimalAsInteger(cnst SJSONReader & reader, u2_t iNode, s3_t	& value);
	//err_t	jsonObjectGetDecimalAsString (cnst SJSONReader & reader, u2_t iNode, vcst_t	& value);
	err_t	jsonObjectGetDecimalAsBoolean(cnst SJSONReader & reader, u2_t iNode, b8_t	& value);
	//err_t	jsonObjectGetStringAsInteger (cnst SJSONReader & reader, u2_t iNode, s3_t	& value);
	//err_t	jsonObjectGetStringAsBoolean (cnst SJSONReader & reader, u2_t iNode, b8_t	& value);
	//err_t	jsonObjectGetStringAsDecimal (cnst SJSONReader & reader, u2_t iNode, f3_t	& value);
	err_t	jsonObjectGetBooleanAsInteger(cnst SJSONReader & reader, u2_t iNode, s3_t	& value);
	//err_t	jsonObjectGetBooleanAsString (cnst SJSONReader & reader, u2_t iNode, vcst_t	& value);
	err_t	jsonObjectGetBooleanAsDecimal(cnst SJSONReader & reader, u2_t iNode, f3_t	& value);
	// 
	//err_t	jsonObjectGetAsString	(cnst SJSONReader & reader, u2_t iNode, vcst_t	& value);
	//err_t	jsonObjectGetAsBoolean	(cnst SJSONReader & reader, u2_t iNode, b8_t	& value);
	//err_t	jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, u0_t	& value);
	//err_t	jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, u1_t	& value);
	//err_t	jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, u2_t	& value);
	//err_t	jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, u3_t	& value);
	//err_t	jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, s0_t	& value);
	//err_t	jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, s1_t	& value);
	//err_t	jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, s2_t	& value);
	//err_t	jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, s3_t	& value);
	//err_t	jsonObjectGetAsDecimal	(cnst SJSONReader & reader, u2_t iNode, f2_t	& value);
	//err_t	jsonObjectGetAsDecimal	(cnst SJSONReader & reader, u2_t iNode, f3_t	& value);

	err_t	jsonObjectGetAsString	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, vcst_t	& value);
	err_t	jsonObjectGetAsBoolean	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, b8_t	& value);
	err_t	jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, u0_t	& value);
	err_t	jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, u1_t	& value);
	err_t	jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, u2_t	& value);
	err_t	jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, u3_t	& value);
	err_t	jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, s0_t	& value);
	err_t	jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, s1_t	& value);
	err_t	jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, s2_t	& value);
	err_t	jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, s3_t	& value);
	err_t	jsonObjectGetAsDecimal	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, f2_t	& value);
	err_t	jsonObjectGetAsDecimal	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, f3_t	& value);

	stct SJSONFile {
		string			Bytes					= {};
		SJSONReader		Reader					= {};
	};

	tydf keyval<vcst_t, SJSONFile>	TKeyValJSONFile;

	err_t			jsonFileRead			(SJSONFile & file, vcst_t filename);
	err_t			jsonWrite				(cnst SJSONNode * node, view<cnst vcst_t> jsonViews, string & output);
	err_t			jsonArraySplit			(cnst SJSONNode & jsonArrayToSplit, view<cnst vcst_t> jsonViews, u2_c blockSize, aobj<string> & outputJsons);

	stct SJSONFieldBinding {
		vcst_t			Field					= {};
		JSON_TYPE		Type					= {};
	};

	err_t			jsonMapToFields			(as2_t & indicesOfFields, view<cnst SJSONFieldBinding> fields, view<cnst TKeyValConstChar> fieldMaps);
	err_t			jsonFieldsToMap			(as2_t & indicesOfFields, view<cnst SJSONFieldBinding> fields, view<cnst TKeyValConstChar> fieldMaps);
} // namespace

#endif // GPK_JSON_H_23627


