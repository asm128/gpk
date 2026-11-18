#include "gpk_json_core.h"
#include "gpk_array_ptr.h"
#include "gpk_string.h"

#ifndef GPK_JSON_H_23627
#define GPK_JSON_H_23627

nmsp gpk
{
#pragma pack(push, 1)
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
	stin err_t		jsonParse			(SJSONReader & reader, vcst_t jsonAsString, b8_t buildTree, b8_t buildViews = true) {
		SJSONParseOptions options = JSON_PARSE_GPK;
		options.Set(JSON_PARSE_OPTION_BUILD_TREE , buildTree );
		options.Set(JSON_PARSE_OPTION_BUILD_VIEWS, buildViews);
		rtrn jsonParse(reader, jsonAsString, options);
	}
	err_t			jsonParseError		(cnst SJSONReaderState & reader, asc_t & errorString);
	err_t			jsonParseError		(cnst SJSONReader & reader, asc_t & errorString);
	err_t			jsonTreeRebuild		(view<cnst SJSONToken> in_object, apobj<SJSONNode> & out_nodes);
	err_t			jsonReaderLoad		(SJSONReader & reader, view<cnst SJSONToken> tokens, vcst_t source);
	err_t			jsonTreeInvalidate	(SJSONReader & reader);
	err_t			jsonTreeEnsure		(SJSONReader & reader);

	err_t			jsonObjectKeyList	(view<cnst SJSONToken> tokens, u2_t iToken, as2_t & indices);
	err_t			jsonObjectKeyList	(view<cnst SJSONToken> tokens, vcst_t source, u2_t iToken, aobj<vcst_t> & keys);
	err_t			jsonObjectKeyList	(view<cnst SJSONToken> tokens, vcst_t source, u2_t iToken, as2_t & indices, aobj<vcst_t> & keys);
	err_t			jsonObjectKeyList	(view<cnst SJSONToken> tokens, view<vcst_c> views, u2_t iToken, aobj<vcst_t> & keys);
	err_t			jsonObjectKeyList	(view<cnst SJSONToken> tokens, view<vcst_c> views, u2_t iToken, as2_t & indices, aobj<vcst_t> & keys);

	err_t			jsonArrayValueGet	(cnst SJSONNode & node, u2_t index);	// returns the index of the JSON element corresponding to the index provided as parameter.
	err_t			jsonObjectValueGet	(cnst SJSONNode & node, view<vcst_c> views, vcst_t key);
	err_t			jsonObjectKeyList	(cnst SJSONNode & node, as2_t & indices);
	err_t			jsonObjectKeyList	(cnst SJSONNode & node, view<vcst_c> views, aobj<vcst_t> & keys);
	err_t			jsonObjectKeyList	(cnst SJSONNode & node, view<vcst_c> views, as2_t & indices, aobj<vcst_t> & keys);
	err_t			jsonCompareNumber	(cnst SJSONNode & node, view<vcst_c> views, cnst SJSONNode & other, view<vcst_c> otherViews);
	err_t			jsonCompareArray	(cnst SJSONNode & node, view<vcst_c> views, cnst SJSONNode & other, view<vcst_c> otherViews);
	err_t			jsonCompareObject	(cnst SJSONNode & node, view<vcst_c> views, cnst SJSONNode & other, view<vcst_c> otherViews);

	nsix	err_t	jsonArraySize		(cnst SJSONNode & node)												nxpt	{ rtrn node.Children.size(); }	// returns the index of the JSON element corresponding to the index provided as parameter.
	nsix	err_t	jsonObjectKeyCount	(cnst SJSONNode & node)												nxpt	{ rtrn node.Children.size() / 2; }
	err_t			jsonArrayForEach	(cnst SJSONNode & node, FError<cnst SJSONNode &> funcForEach);
	//err_t			jsonArrayEnumerate	(cnst SJSONNode & node, FError<cnst SJSONNode &, u2_t index> funcEnumerate);
	// Access functions: Iterate over children looking for keys or values.
	ndsi	err_t	jsonArraySize		(cnst SJSONReader & reader, u2_t iNode)											{ rtrn jsonArraySize(reader.Token, iNode); }
	ndsi	err_t	jsonObjectKeyCount	(cnst SJSONReader & reader, u2_t iNode)											{ rtrn jsonObjectKeyCount(reader.Token, iNode); }
	ndsi	err_t	jsonArrayValueGet	(cnst SJSONReader & reader, u2_t iNode, u2_t indexOfValueToGet)					{ rtrn jsonArrayValueGet(reader.Token, iNode, indexOfValueToGet); }
	ndsi	err_t	jsonObjectValueGet	(cnst SJSONReader & reader, u2_t iNode, vcst_t key)								{ rtrn jsonObjectValueGet(reader.Token, reader.View, iNode, key); }
	stin	err_t	jsonObjectKeyList	(cnst SJSONReader & reader, u2_t iNode, as2_t & indices)						{ rtrn jsonObjectKeyList(reader.Token, iNode, indices); }
	stin	err_t	jsonObjectKeyList	(cnst SJSONReader & reader, u2_t iNode, aobj<vcst_t> & keys)					{ rtrn jsonObjectKeyList(reader.Token, reader.View, iNode, keys); }
	stin	err_t	jsonObjectKeyList	(cnst SJSONReader & reader, u2_t iNode, as2_t & indices, aobj<vcst_t> & keys)	{ rtrn jsonObjectKeyList(reader.Token, reader.View, iNode, indices, keys); }

	ndsi	err_t	jsonCompareArray	(cnst SJSONNode & node, cnst SJSONNode & other, view<vcst_c> views)	{ rtrn jsonCompareArray		(node, views, other, views); }
	ndsi	err_t	jsonCompareObject	(cnst SJSONNode & node, cnst SJSONNode & other, view<vcst_c> views)	{ rtrn jsonCompareObject	(node, views, other, views); }
	ndsi	err_t	jsonCompareNumber	(cnst SJSONNode & node, cnst SJSONNode & other, view<vcst_c> views)	{ rtrn jsonCompareNumber	(node, views, other, views); }

	err_t	jsonObjectGetCString(cnst SJSONReader & reader, u2_t iNode, vcst_t	& value);
	err_t	jsonObjectGetString (cnst SJSONReader & reader, u2_t iNode, vcst_t	& value);
	err_t	jsonObjectGetString (cnst SJSONReader & reader, u2_t iNode, string	& value);
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
	err_t			jsonWrite				(cnst SJSONNode * node, view<vcst_c> jsonViews, string & output);
	err_t			jsonWrite				(view<cnst SJSONToken> tokens, vcst_t source, string & output, u2_t iToken = 0);
	err_t			jsonArraySplit			(cnst SJSONNode & jsonArrayToSplit, view<vcst_c> jsonViews, u2_c blockSize, aobj<string> & outputJsons);

	stct SJSONFieldBinding {
		vcst_t			Field					= {};
		JSON_TYPE		Type					= {};
	};

	err_t			jsonMapToFields			(as2_t & indicesOfFields, view<cnst SJSONFieldBinding> fields, view<cnst keyval<vcst_t>> fieldMaps);
	err_t			jsonFieldsToMap			(as2_t & indicesOfFields, view<cnst SJSONFieldBinding> fields, view<cnst keyval<vcst_t>> fieldMaps);
	err_t			jsonToCodePoint			(vcst_t input, u2_t & unicode);

} // nmsp

#endif // GPK_JSON_H_23627
