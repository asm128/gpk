#include "gpk_json.h"

#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(JSON_READER_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, OK					, 0, "All JSON reader tests passed.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, PARSE					, 1, "jsonParse() rejected a valid JSON document.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, ROOT_TYPE				, 6, "The semantic JSON root had an unexpected type.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, WRITE					, 7, "jsonWrite() failed to serialize a parsed document.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, ROUND_TRIP				, 8, "A parsed JSON document did not serialize to its expected canonical text.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, COUNTED_INPUT			, 9, "jsonParse() depended on a null terminator beyond its counted input.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, OPTION_PRESETS			, 14, "A JSON parse-option preset did not contain its documented flags.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, STORAGE_VIEW_COUNT		, 17, "The JSON token and view counts differ.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, STORAGE_TREE_COUNT		, 18, "The JSON token and tree counts differ.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, TOKEN_SPAN_ORDER		, 19, "A JSON token span ends before it begins.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, TOKEN_SPAN_BOUNDS		, 20, "A JSON token span exceeds the input.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, TOKEN_VIEW_SIZE		, 21, "A JSON token view has the wrong length.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, TOKEN_VIEW_BEGIN		, 22, "A JSON token view points outside its source span.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, TREE_NODE_PRESENT		, 23, "A JSON token has no tree node.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, TREE_NODE_TOKEN		, 24, "A JSON tree node references the wrong token.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, TREE_NODE_INDEX		, 25, "A JSON tree node has the wrong object index.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, TREE_NODE_PARENT		, 26, "A JSON tree node has the wrong parent.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, READER_DONE			, 27, "The JSON reader did not finish reading.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, READER_NEST_LEVEL		, 28, "The JSON reader retained a nonzero nesting level.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, READER_ELEMENT_INDEX	, 29, "The JSON reader retained a current element index.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, READER_CURRENT_ELEMENT	, 30, "The JSON reader retained a current element pointer.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, ROOT_PRESENT			, 31, "The JSON parse tree has no root node.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, ROOT_REFERENCE		, 32, "The JSON parse tree root reference is null.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, ROOT_VALUE_REFERENCE	, 33, "The semantic JSON root reference is null.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, INTEGER_ROOT_PRESENT	, 34, "The signed-integer parse tree has no root node.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, INTEGER_ROOT_REFERENCE, 35, "The signed-integer parse tree root reference is null.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, INTEGER_GET_RESULT		, 36, "jsonObjectGetInteger() rejected a parsed signed integer.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, INTEGER_GET_VALUE		, 37, "jsonObjectGetInteger() returned the wrong signed value.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, OPTION_NONZERO		, 38, "A JSON parse-option flag has no bit.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, OPTION_SINGLE_BIT		, 39, "A JSON parse-option flag occupies more than one bit.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, OPTION_DISTINCT_BIT	, 40, "A JSON parse-option flag overlaps an earlier flag.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, OPTION_COMBINED		, 41, "The combined JSON parse-option mask is incomplete.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, OPTION_NONE			, 42, "JSON_PARSE_OPTION_NONE is not zero.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, OPTION_OUTPUT_PARSE		, 43, "Parsing with no output-building flags failed.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, OPTION_OUTPUT_VIEW		, 44, "Parsing with no output-building flags produced the wrong view count.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, OPTION_OUTPUT_TREE		, 45, "Parsing with no output-building flags built a tree.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, OPTION_BUILD_TREE_CLEAR	, 46, "Clearing BUILD_TREE left the JSON parse option set.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, OPTION_FINAL_INPUT_SET	, 47, "Setting FINAL_INPUT did not persist in JSON parse options.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, RESET_ROOT_PRESENT		, 48, "The cached JSON tree has no root before reset.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, RESET_ROOT_REFERENCE	, 49, "The cached JSON root reference is null before reset.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, RESET_RESULT			, 50, "SJSONReader::Reset() failed.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, RESET_TOKENS			, 51, "SJSONReader::Reset() retained tokens.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, RESET_VIEWS			, 52, "SJSONReader::Reset() retained views.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, RESET_TREE_COUNT		, 53, "SJSONReader::Reset() changed the cached tree count.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, RESET_TREE_STORAGE		, 54, "SJSONReader::Reset() changed the cached tree storage.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, RESET_ROOT_STORAGE		, 55, "SJSONReader::Reset() changed the cached root reference.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, RESET_CHILD_STORAGE	, 56, "SJSONReader::Reset() changed the cached child storage.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, RESET_CHAR_INDEX		, 57, "SJSONReader::Reset() retained the character index.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, RESET_ELEMENT_INDEX	, 58, "SJSONReader::Reset() retained the element index.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, RESET_ELEMENT			, 59, "SJSONReader::Reset() retained the current element.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, RESET_NEST_LEVEL		, 60, "SJSONReader::Reset() retained the nesting level.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, RESET_CHARACTER		, 61, "SJSONReader::Reset() retained the current character.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, RESET_ESCAPING		, 62, "SJSONReader::Reset() retained the escaping flag.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, RESET_INSIDE_STRING	, 63, "SJSONReader::Reset() retained the inside-string flag.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, RESET_SEPARATOR		, 64, "SJSONReader::Reset() retained the expected-separator flag.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, RESET_DONE			, 65, "SJSONReader::Reset() retained the done-reading flag.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, REUSE_TREE_STORAGE		, 66, "Reparsing changed the cached JSON tree storage.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, REUSE_ROOT_STORAGE		, 67, "Reparsing changed the cached JSON root reference.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, REUSE_CHILD_STORAGE	, 68, "Reparsing changed the cached JSON child storage.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, RESET_ROOT_REFERENCE_AFTER, 69, "The cached JSON root reference is null after reset.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, CODEPOINT_RESULT		, 70, "jsonToCodePoint() returned the wrong status.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, CODEPOINT_VALUE		, 71, "jsonToCodePoint() returned the wrong Unicode code point.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, TREE_INVALIDATE_EMPTY	, 72, "jsonTreeInvalidate() accepted an empty reader.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, TREE_ENSURE_EMPTY		, 73, "jsonTreeEnsure() accepted an empty reader.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, TREE_ENSURE_CLEAN		, 74, "jsonTreeEnsure() rebuilt a current tree.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, TREE_INVALIDATE_RESULT	, 75, "jsonTreeInvalidate() rejected a parsed tree.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, TREE_INVALIDATE_STATE	, 76, "jsonTreeInvalidate() left the root token current.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, TREE_ENSURE_RESULT		, 77, "jsonTreeEnsure() rejected an invalidated tree.");
GDEFINE_ENUM_VALUED(JSON_READER_TEST_RESULT, TREE_ENSURE_STATE		, 78, "jsonTreeEnsure() did not restore the invalidated tree.");

static_assert(szof(::gpk::JSON_PARSE_OPTION) == szof(::gpk::u1_t), "JSON parse options must use one uniform 16-bit field.");
static_assert(szof(::gpk::SJSONParseOptions) == szof(::gpk::JSON_PARSE_OPTION), "bit_field<> must not add storage overhead.");

tydf ::gpk::view<const ::gpk::vcst_t> TJSONConstViews;
tydf ::gpk::err_t (*TJSONObjectValueGet)(cnst ::gpk::SJSONNode &, TJSONConstViews, ::gpk::vcst_t);
tydf ::gpk::err_t (*TJSONObjectKeyList)(cnst ::gpk::SJSONNode &, TJSONConstViews, ::gpk::aobj<::gpk::vcst_t> &);
tydf ::gpk::err_t (*TJSONObjectIndexedKeyList)(cnst ::gpk::SJSONNode &, TJSONConstViews, ::gpk::as2_t &, ::gpk::aobj<::gpk::vcst_t> &);
tydf ::gpk::err_t (*TJSONCompare)(cnst ::gpk::SJSONNode &, TJSONConstViews, cnst ::gpk::SJSONNode &, TJSONConstViews);
tydf ::gpk::err_t (*TJSONCompareSharedViews)(cnst ::gpk::SJSONNode &, cnst ::gpk::SJSONNode &, TJSONConstViews);
tydf ::gpk::err_t (*TJSONWrite)(cnst ::gpk::SJSONNode *, TJSONConstViews, ::gpk::string &);
tydf ::gpk::err_t (*TJSONArraySplit)(cnst ::gpk::SJSONNode &, TJSONConstViews, ::gpk::u2_t, ::gpk::aobj<::gpk::string> &);

static_assert(::gpk::is_cnst<TJSONConstViews::T>::Value, "JSON view collections must expose const view descriptors.");
static_assert(::gpk::is_cnst<::gpk::vcst_t::T>::Value, "String views must expose const characters.");
static_assert(requires {
	static_cast<TJSONObjectValueGet>(&::gpk::jsonObjectValueGet);
	static_cast<TJSONObjectKeyList>(&::gpk::jsonObjectKeyList);
	static_cast<TJSONObjectIndexedKeyList>(&::gpk::jsonObjectKeyList);
	}, "JSON object access must preserve const view collections.");
static_assert(requires {
	static_cast<TJSONCompare>(&::gpk::jsonCompareNumber);
	static_cast<TJSONCompare>(&::gpk::jsonCompareArray);
	static_cast<TJSONCompare>(&::gpk::jsonCompareObject);
	static_cast<TJSONCompareSharedViews>(&::gpk::jsonCompareNumber);
	static_cast<TJSONCompareSharedViews>(&::gpk::jsonCompareArray);
	static_cast<TJSONCompareSharedViews>(&::gpk::jsonCompareObject);
	}, "JSON comparison must preserve const view collections.");
static_assert(requires {
	static_cast<TJSONWrite>(&::gpk::jsonWrite);
	static_cast<TJSONArraySplit>(&::gpk::jsonArraySplit);
	}, "JSON serialization must preserve const view collections.");

sttc bool jsonTextMismatch(::gpk::vcst_t actual, ::gpk::vcst_t expected) {
	rtrn actual.size() != expected.size() || (actual.size() && 0 != memcmp(actual.begin(), expected.begin(), actual.size()));
}

sttc ::gpk::err_t testJSONStructure(::gpk::ATestError & errors, cnst ::gpk::SJSONReader & reader, ::gpk::vcst_t input) {
	GPK_TEST_REQUIREF(errors, JSON_READER_TEST_RESULT_STORAGE_VIEW_COUNT, reader.Token.size() != reader.View.size() , "Tokens:%u, views:%u." , reader.Token.size(), reader.View.size() );
	GPK_TEST_REQUIREF(errors, JSON_READER_TEST_RESULT_STORAGE_TREE_COUNT, reader.Token.size() != reader.Tree.size() , "Tokens:%u, tree nodes:%u." , reader.Token.size(), reader.Tree.size() );
	for(::gpk::u2_t iToken = 0; iToken < reader.Token.size(); ++iToken) {
		cnst ::gpk::SJSONToken & token = reader.Token[iToken];
		cnst ::gpk::vcst_t & tokenView = reader.View[iToken];
		always_printf("JSON token:%u type:%s parent:%i span:%u..%u text:'%.*s'.", iToken, ::gpk::get_value_namep(token.Type), token.ParentIndex, token.Span.Begin, token.Span.End, GPK_VIEW_ARGS(tokenView));
		cnst ::gpk::pobj<::gpk::SJSONNode> & node = reader.Tree[iToken];
		cnst bool hasParent = token.ParentIndex >= 0 && (::gpk::u2_t)token.ParentIndex < reader.Tree.size();
		GPK_TEST_REQUIREF(errors, JSON_READER_TEST_RESULT_TOKEN_SPAN_ORDER, token.Span.Begin > token.Span.End , "Token:%u span:%u..%u." , iToken, token.Span.Begin, token.Span.End );
		GPK_TEST_REQUIREF(errors, JSON_READER_TEST_RESULT_TOKEN_SPAN_BOUNDS, token.Span.End > input.size() , "Token:%u end:%u, input size:%u." , iToken, token.Span.End, input.size() );
		GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_TOKEN_VIEW_SIZE, tokenView.size() != token.Span.End - token.Span.Begin , "Token:%u view size:%u, span size:%u." , iToken, tokenView.size(), token.Span.End - token.Span.Begin );
		GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_TOKEN_VIEW_BEGIN, tokenView.begin() != (token.Span.Begin < input.size() ? &input[token.Span.Begin] : input.end()) , "Token:%u view begin:%p, expected:%p." , iToken, tokenView.begin(), token.Span.Begin < input.size() ? &input[token.Span.Begin] : input.end() );
		GPK_TEST_REQUIREF(errors, JSON_READER_TEST_RESULT_TREE_NODE_PRESENT, 0 == node.get_ref() , "Token:%u node:%p, expected non-null." , iToken, node.get_ref() );
		GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_TREE_NODE_TOKEN, node->Token != &reader.Token[iToken] , "Node:%u token:%p, expected:%p." , iToken, node->Token, &reader.Token[iToken] );
		GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_TREE_NODE_INDEX, node->ObjectIndex != (::gpk::json_id_t)iToken , "Node:%u object index:%i." , iToken, node->ObjectIndex );
		GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_TREE_NODE_PARENT, node->Parent != (hasParent ? reader.Tree[token.ParentIndex].operator->() : 0) , "Node:%u parent:%p, expected:%p." , iToken, node->Parent, hasParent ? reader.Tree[token.ParentIndex].operator->() : 0 );
	}
	rtrn 0;
}

sttc ::gpk::err_t testJSONDocument
	(::gpk::ATestError & errors, ::gpk::vcst_t input, ::gpk::vcst_t expected, ::gpk::JSON_TYPE expectedType, JSON_READER_TEST_RESULT parseResult = JSON_READER_TEST_RESULT_PARSE) {
	::gpk::SJSONReader reader;
	cnst ::gpk::err_t result = ::gpk::jsonParse(reader, input);
	always_printf("JSON parse: expected type:%s; return:%i; input (%u bytes):\n%.*s", ::gpk::get_value_namep(expectedType), result, input.size(), GPK_VIEW_ARGS(input));
	GPK_TEST_REQUIREF(errors, parseResult, ::gpk::failed(result) , "Valid JSON parse failed. result:%i, input:'%.*s'." , result, (int)input.size(), input.begin() );
	cnst ::gpk::u2_t structureFailures = testErrorCount(errors);
	if_fail_fe(testJSONStructure(errors, reader, input));
	if(structureFailures != testErrorCount(errors))
		rtrn 0;
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_READER_DONE, false == reader.StateRead.DoneReading , "Done reading:%u, expected:1." , (::gpk::u2_t)reader.StateRead.DoneReading );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_READER_NEST_LEVEL, reader.StateRead.NestLevel , "Nest level:%i, expected:0." , reader.StateRead.NestLevel );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_READER_ELEMENT_INDEX, reader.StateRead.IndexCurrentElement != -1 , "Current element index:%i, expected:-1." , reader.StateRead.IndexCurrentElement );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_READER_CURRENT_ELEMENT, reader.StateRead.CurrentElement , "Current element:%p, expected:null." , reader.StateRead.CurrentElement );
	GPK_TEST_REQUIREF(errors, JSON_READER_TEST_RESULT_ROOT_PRESENT, 0 == reader.Tree.size() , "Missing JSON root. tree nodes:%u." , reader.Tree.size() );
	GPK_TEST_REQUIREF(errors, JSON_READER_TEST_RESULT_ROOT_REFERENCE, 0 == reader.Tree[0].get_ref() , "Root reference:%p, expected non-null." , reader.Tree[0].get_ref() );
	cnst ::gpk::pobj<::gpk::SJSONNode> & treeRoot = reader.Tree[0];
	cnst ::gpk::pobj<::gpk::SJSONNode> & root = treeRoot->Token->Type == ::gpk::JSON_TYPE_VALUE && treeRoot->Children.size() ? treeRoot->Children[0] : treeRoot;
	GPK_TEST_REQUIREF(errors, JSON_READER_TEST_RESULT_ROOT_VALUE_REFERENCE, 0 == root.get_ref() , "Semantic root reference:%p, expected non-null." , root.get_ref() );
	GPK_TEST_REQUIREF(errors, JSON_READER_TEST_RESULT_ROOT_TYPE, root->Token->Type != expectedType , "Root type mismatch. actual:%i, expected:%i, tree nodes:%u." , (::gpk::s2_t)root->Token->Type, (::gpk::s2_t)expectedType, reader.Tree.size() );
	::gpk::string output;
	cnst ::gpk::err_t writeResult = ::gpk::jsonWrite(reader.Tree[0], reader.View, output);
	always_printf("JSON write result:%i\nExpected:\n%.*s\nActual:\n%.*s\nText comparison:%.*s.", writeResult, GPK_VIEW_ARGS(expected), (int)output.size(), GPK_VIEW_ARGS(output), jsonTextMismatch(output, expected) ? "FAIL" : "PASS");
	GPK_TEST_REQUIREF(errors, JSON_READER_TEST_RESULT_WRITE, ::gpk::failed(writeResult) , "JSON write failed. result:%i, root type:%i." , writeResult, (::gpk::s2_t)root->Token->Type );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_ROUND_TRIP, jsonTextMismatch(output, expected) , "Round-trip mismatch. input:'%.*s', actual:'%.*s', expected:'%.*s'." , (int)input.size(), input.begin(), (int)output.size(), output.begin(), (int)expected.size(), expected.begin() );
	rtrn 0;
}

sttc ::gpk::err_t testJSONValidDocuments(::gpk::ATestError & errors) {
	if_fail_fe(testJSONDocument(errors, "true", "true", ::gpk::JSON_TYPE_BOOLEAN));
	if_fail_fe(testJSONDocument(errors, "-42", "-42", ::gpk::JSON_TYPE_INTEGER));
	if_fail_fe(testJSONDocument(errors, "12.5", "12.5", ::gpk::JSON_TYPE_DECIMAL));
	if_fail_fe(testJSONDocument(errors, "\"text\"", "\"text\"", ::gpk::JSON_TYPE_STRING));
	if_fail_fe(testJSONDocument(errors, "[]", "[]", ::gpk::JSON_TYPE_ARRAY));
	if_fail_fe(testJSONDocument(errors, "{}", "{}", ::gpk::JSON_TYPE_OBJECT));
	if_fail_fe(testJSONDocument(errors, "[1,true,null,\"x\"]", "[1,true,null,\"x\"]", ::gpk::JSON_TYPE_ARRAY));
	if_fail_fe(testJSONDocument(errors, "{\"name\":\"gpk\",\"count\":3,\"enabled\":true}", "{\"name\":\"gpk\",\"count\":3,\"enabled\":true}", ::gpk::JSON_TYPE_OBJECT));
	if_fail_fe(testJSONDocument(errors, " \n { \"a\" : [ 1, 2 ] } \t", "{\"a\":[1,2]}", ::gpk::JSON_TYPE_OBJECT));

	cnst ::gpk::sc_t countedInput[] = {'t', 'r', 'u', 'e'};
	rtrn testJSONDocument(errors, {countedInput}, "true", ::gpk::JSON_TYPE_BOOLEAN, JSON_READER_TEST_RESULT_COUNTED_INPUT);
}

sttc ::gpk::err_t testJSONReset(::gpk::ATestError & errors) {
	::gpk::SJSONReader reader;
	if_fail_fe(::gpk::jsonParse(reader, "{\"values\":[1,2,3],\"flag\":true}"));
	GPK_TEST_REQUIREF(errors, JSON_READER_TEST_RESULT_RESET_ROOT_PRESENT, 0 == reader.Tree.size() , "Cached tree nodes:%u, expected at least one." , reader.Tree.size() );
	GPK_TEST_REQUIREF(errors, JSON_READER_TEST_RESULT_RESET_ROOT_REFERENCE, 0 == reader.Tree[0].get_ref() , "Cached root reference:%p, expected non-null." , reader.Tree[0].get_ref() );
	::gpk::u2_c treeSize = reader.Tree.size();
	cnst ::gpk::view<const ::gpk::pobj<::gpk::SJSONNode>> treeStorage = {reader.Tree.begin(), reader.Tree.size()};
	cnst ::gpk::view<const ::gpk::gref<::gpk::SJSONNode>> rootReference = {reader.Tree[0].get_ref(), 1};
	cnst ::gpk::view<const ::gpk::pobj<::gpk::SJSONNode>> childStorage = {reader.Tree[0]->Children.begin(), reader.Tree[0]->Children.size()};
	cnst ::gpk::err_t result = reader.Reset();
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_RESET_RESULT, ::gpk::failed(result), "Reset result:%i.", result);
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_RESET_TOKENS, reader.Token.size(), "Tokens after reset:%u.", reader.Token.size());
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_RESET_VIEWS, reader.View.size(), "Views after reset:%u.", reader.View.size());
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_RESET_TREE_COUNT, reader.Tree.size() != treeSize , "Tree nodes after reset:%u, expected:%u." , reader.Tree.size(), treeSize );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_RESET_TREE_STORAGE, reader.Tree.begin() != treeStorage.begin() , "Tree storage after reset:%p, expected:%p." , reader.Tree.begin(), treeStorage.begin() );
	if(reader.Tree.size()) {
		GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_RESET_ROOT_REFERENCE_AFTER, 0 == reader.Tree[0].get_ref() , "Root reference after reset:%p, expected non-null." , reader.Tree[0].get_ref() );
		GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_RESET_ROOT_STORAGE, reader.Tree[0].get_ref() != rootReference.begin() , "Root reference after reset:%p, expected:%p." , reader.Tree[0].get_ref(), rootReference.begin() );
		if(reader.Tree[0].get_ref()) {
			GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_RESET_CHILD_STORAGE, reader.Tree[0]->Children.begin() != childStorage.begin() , "Child storage after reset:%p, expected:%p." , reader.Tree[0]->Children.begin(), childStorage.begin() );
		}
	}
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_RESET_CHAR_INDEX, reader.StateRead.IndexCurrentChar , "Character index after reset:%u." , reader.StateRead.IndexCurrentChar );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_RESET_ELEMENT_INDEX, reader.StateRead.IndexCurrentElement != -1 , "Element index after reset:%i." , reader.StateRead.IndexCurrentElement );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_RESET_ELEMENT, reader.StateRead.CurrentElement , "Current element after reset:%p." , reader.StateRead.CurrentElement );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_RESET_NEST_LEVEL, reader.StateRead.NestLevel , "Nest level after reset:%i." , reader.StateRead.NestLevel );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_RESET_CHARACTER, reader.StateRead.CharCurrent , "Current character after reset:%i." , reader.StateRead.CharCurrent );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_RESET_ESCAPING, reader.StateRead.Escaping , "Escaping after reset:%u." , (::gpk::u2_t)reader.StateRead.Escaping );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_RESET_INSIDE_STRING, reader.StateRead.InsideString , "Inside string after reset:%u." , (::gpk::u2_t)reader.StateRead.InsideString );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_RESET_SEPARATOR, reader.StateRead.ExpectingSeparator , "Expecting separator after reset:%u." , (::gpk::u2_t)reader.StateRead.ExpectingSeparator );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_RESET_DONE, reader.StateRead.DoneReading , "Done reading after reset:%u." , (::gpk::u2_t)reader.StateRead.DoneReading );

	::gpk::vcst_t input = "{\"value\":-2}";
	if_fail_fe(::gpk::jsonParse(reader, input));
	cnst ::gpk::u2_t structureFailures = testErrorCount(errors);
	if_fail_fe(testJSONStructure(errors, reader, input));
	if(structureFailures != testErrorCount(errors))
		rtrn 0;
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_REUSE_TREE_STORAGE, reader.Tree.begin() != treeStorage.begin() , "Tree storage after rebuild:%p, expected:%p." , reader.Tree.begin(), treeStorage.begin() );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_REUSE_ROOT_STORAGE, reader.Tree[0].get_ref() != rootReference.begin() , "Root reference after rebuild:%p, expected:%p." , reader.Tree[0].get_ref(), rootReference.begin() );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_REUSE_CHILD_STORAGE, reader.Tree[0]->Children.begin() != childStorage.begin() , "Child storage after rebuild:%p, expected:%p." , reader.Tree[0]->Children.begin(), childStorage.begin() );
	rtrn 0;
}

sttc ::gpk::err_t testJSONTreeFreshness(::gpk::ATestError & errors) {
	::gpk::SJSONReader empty;
	GPK_TEST_CHECK(errors, JSON_READER_TEST_RESULT_TREE_INVALIDATE_EMPTY, false == ::gpk::failed(::gpk::jsonTreeInvalidate(empty)));
	GPK_TEST_CHECK(errors, JSON_READER_TEST_RESULT_TREE_ENSURE_EMPTY, false == ::gpk::failed(::gpk::jsonTreeEnsure(empty)));

	cnst ::gpk::vcst_t input = "{\"values\":[1,2,3],\"flag\":true}";
	::gpk::SJSONReader reader;
	if_fail_fe(::gpk::jsonParse(reader, input));
	reader.Tree[0]->ObjectIndex = -2;
	GPK_TEST_CHECK(errors, JSON_READER_TEST_RESULT_TREE_ENSURE_CLEAN, ::gpk::failed(::gpk::jsonTreeEnsure(reader)) || reader.Tree[0]->ObjectIndex != -2);
	reader.Tree[0]->ObjectIndex = 0;
	GPK_TEST_REQUIRE(errors, JSON_READER_TEST_RESULT_TREE_INVALIDATE_RESULT, ::gpk::failed(::gpk::jsonTreeInvalidate(reader)));
	GPK_TEST_CHECK(errors, JSON_READER_TEST_RESULT_TREE_INVALIDATE_STATE, reader.Tree[0]->Token);
	GPK_TEST_REQUIRE(errors, JSON_READER_TEST_RESULT_TREE_ENSURE_RESULT, ::gpk::failed(::gpk::jsonTreeEnsure(reader)));
	GPK_TEST_CHECK(errors, JSON_READER_TEST_RESULT_TREE_ENSURE_STATE, reader.Tree[0]->Token != &reader.Token[0] || reader.Tree[0]->ObjectIndex);
	rtrn testJSONStructure(errors, reader, input);
}

sttc ::gpk::err_t testJSONSignedInteger(::gpk::ATestError & errors) {
	::gpk::SJSONReader reader;
	if_fail_fe(::gpk::jsonParse(reader, "-42"));
	GPK_TEST_REQUIREF(errors, JSON_READER_TEST_RESULT_INTEGER_ROOT_PRESENT, 0 == reader.Tree.size() , "Missing signed-integer root. tree nodes:%u." , reader.Tree.size() );
	GPK_TEST_REQUIREF(errors, JSON_READER_TEST_RESULT_INTEGER_ROOT_REFERENCE, 0 == reader.Tree[0].get_ref() , "Signed-integer root reference:%p, expected non-null." , reader.Tree[0].get_ref() );
	cnst ::gpk::pobj<::gpk::SJSONNode> & treeRoot = reader.Tree[0];
	cnst ::gpk::pobj<::gpk::SJSONNode> & root = treeRoot->Token->Type == ::gpk::JSON_TYPE_VALUE && treeRoot->Children.size() ? treeRoot->Children[0] : treeRoot;
	GPK_TEST_REQUIREF(errors, JSON_READER_TEST_RESULT_ROOT_VALUE_REFERENCE, 0 == root.get_ref() , "Signed-integer semantic root reference:%p, expected non-null." , root.get_ref() );
	::gpk::s3_t value = {};
	cnst ::gpk::err_t result = ::gpk::jsonObjectGetInteger(reader, root->ObjectIndex, value);
	GPK_TEST_REQUIREF(errors, JSON_READER_TEST_RESULT_INTEGER_GET_RESULT, ::gpk::failed(result) , "Signed integer getter result:%i, expected success." , result );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_INTEGER_GET_VALUE, value != -42 , "Signed integer value:%" GPK_FMT_S3 ", expected:-42." , value );
	rtrn 0;
}

sttc ::gpk::err_t testJSONCodePoints(::gpk::ATestError & errors) {
	stct SCodePointTest {
		::gpk::vcst_t	Input;
		::gpk::u2_t		Expected;
	};
	cnst SCodePointTest valid[] =
		{ {"0041"		, 0x000041U}
		, {"0041F"		, 0x000041U}
		, {"D7FF"		, 0x00D7FFU}
		, {"E000"		, 0x00E000U}
		, {"D83D\\uDE00"	, 0x01F600U}
		, {"DBFF\\uDFFF"	, 0x10FFFFU}
		};
	for(::gpk::u2_t iTest = 0; iTest < ::gpk::size(valid); ++iTest) {
		::gpk::u2_t codePoint = {};
		cnst ::gpk::err_t result = ::gpk::jsonToCodePoint(valid[iTest].Input, codePoint);
		GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_CODEPOINT_RESULT, ::gpk::failed(result), "Valid input rejected. input:'%.*s', result:%i.", GPK_VIEW_ARGS(valid[iTest].Input), result);
		if(::gpk::failed(result))
			continue;
		GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_CODEPOINT_VALUE, codePoint != valid[iTest].Expected, "Code point mismatch. input:'%.*s', actual:0x%08X, expected:0x%08X.", GPK_VIEW_ARGS(valid[iTest].Input), codePoint, valid[iTest].Expected);
	}

	cnst ::gpk::vcst_t invalid[] =
		{ ""
		, "123"
		, "12G4"
		, "DC00"
		, "D800"
		, "D83D\\xDE00"
		, "D83D\\uDE0"
		, "D83D\\u0041"
		, "D83D\\uE000"
		};
	for(::gpk::u2_t iTest = 0; iTest < ::gpk::size(invalid); ++iTest) {
		::gpk::u2_t codePoint = {};
		cnst ::gpk::err_t result = ::gpk::jsonToCodePoint(invalid[iTest], codePoint);
		GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_CODEPOINT_RESULT, false == ::gpk::failed(result), "Invalid input accepted. input:'%.*s', code point:0x%08X.", GPK_VIEW_ARGS(invalid[iTest]), codePoint);
	}
	rtrn 0;
}

sttc ::gpk::err_t testJSONOptions(::gpk::ATestError & errors) {
	cnst ::gpk::JSON_PARSE_OPTION individual[] =
		{ ::gpk::JSON_PARSE_OPTION_BUILD_TREE, ::gpk::JSON_PARSE_OPTION_BUILD_VIEWS, ::gpk::JSON_PARSE_OPTION_FINAL_INPUT
		, ::gpk::JSON_PARSE_OPTION_ALLOW_LINE_COMMENTS, ::gpk::JSON_PARSE_OPTION_ALLOW_TRAILING_COMMAS, ::gpk::JSON_PARSE_OPTION_ALLOW_LEADING_PLUS
		, ::gpk::JSON_PARSE_OPTION_ALLOW_LEADING_DECIMAL_POINT, ::gpk::JSON_PARSE_OPTION_ALLOW_TRAILING_DECIMAL_POINT, ::gpk::JSON_PARSE_OPTION_ALLOW_LEADING_ZEROES
		, ::gpk::JSON_PARSE_OPTION_ALLOW_TRAILING_CONTENT, ::gpk::JSON_PARSE_OPTION_ALLOW_UNESCAPED_CONTROLS, ::gpk::JSON_PARSE_OPTION_CONTAINER_ROOT_ONLY
		};
	::gpk::u1_t combined = {};
	for(::gpk::u2_t iOption = 0; iOption < ::gpk::size(individual); ++iOption) {
		cnst ::gpk::u1_t value = individual[iOption];
		GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_OPTION_NONZERO, 0 == value , "Option:%u value:0x%04X, expected nonzero." , iOption, value );
		GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_OPTION_SINGLE_BIT, value & (value - 1) , "Option:%u value:0x%04X, expected one bit." , iOption, value );
		GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_OPTION_DISTINCT_BIT, combined & value , "Option:%u value:0x%04X overlaps previous:0x%04X." , iOption, value, combined );
		combined |= value;
	}
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_OPTION_COMBINED, combined != 0x0FFFU , "Combined option mask:0x%04X, expected:0x0FFF." , combined );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_OPTION_NONE, ::gpk::JSON_PARSE_OPTION_NONE , "NONE option mask:0x%04X, expected:0." , (::gpk::u1_t)::gpk::JSON_PARSE_OPTION_NONE );

	cnst ::gpk::JSON_PARSE_OPTION strictExpected = ::gpk::JSON_PARSE_OPTION_BUILD_TREE | ::gpk::JSON_PARSE_OPTION_BUILD_VIEWS | ::gpk::JSON_PARSE_OPTION_FINAL_INPUT;
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_OPTION_PRESETS, (::gpk::JSON_PARSE_OPTION)::gpk::JSON_PARSE_STRICT != strictExpected , "Strict preset mismatch. actual:0x%04X, expected:0x%04X." , (::gpk::u1_t)::gpk::JSON_PARSE_STRICT, (::gpk::u1_t)strictExpected );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_OPTION_PRESETS, (::gpk::JSON_PARSE_OPTION)::gpk::JSON_PARSE_LEGACY_ROOT != (strictExpected | ::gpk::JSON_PARSE_OPTION_CONTAINER_ROOT_ONLY) , "Legacy-root preset mismatch. actual:0x%04X." , (::gpk::u1_t)::gpk::JSON_PARSE_LEGACY_ROOT );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_OPTION_PRESETS, (::gpk::u1_t)::gpk::JSON_PARSE_GPK != (::gpk::u1_t)(combined & ~::gpk::JSON_PARSE_OPTION_CONTAINER_ROOT_ONLY) , "GPK preset mismatch. actual:0x%04X, expected:0x%04X." , (::gpk::u1_t)::gpk::JSON_PARSE_GPK, (::gpk::u1_t)(combined & ~::gpk::JSON_PARSE_OPTION_CONTAINER_ROOT_ONLY) );

	::gpk::SJSONParseOptions options = ::gpk::JSON_PARSE_OPTION_NONE;
	options.Set(::gpk::JSON_PARSE_OPTION_BUILD_TREE).Set(::gpk::JSON_PARSE_OPTION_FINAL_INPUT).Clear(::gpk::JSON_PARSE_OPTION_BUILD_TREE);
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_OPTION_BUILD_TREE_CLEAR, options.Any(::gpk::JSON_PARSE_OPTION_BUILD_TREE) , "BUILD_TREE remained set. options:0x%04X." , (::gpk::u1_t)options );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_OPTION_FINAL_INPUT_SET, false == options.All(::gpk::JSON_PARSE_OPTION_FINAL_INPUT) , "FINAL_INPUT was cleared. options:0x%04X." , (::gpk::u1_t)options );

	::gpk::SJSONReader reader;
	cnst ::gpk::SJSONParseOptions noOutputs = ::gpk::JSON_PARSE_OPTION_FINAL_INPUT;
	cnst ::gpk::err_t parseResult = ::gpk::jsonParse(reader, "{}", noOutputs);
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_OPTION_OUTPUT_PARSE, ::gpk::failed(parseResult) , "No-output parse result:%i." , parseResult );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_OPTION_OUTPUT_VIEW, reader.View.size() != 1 , "No-output view count:%u, expected:1." , reader.View.size() );
	GPK_TEST_CHECKF(errors, JSON_READER_TEST_RESULT_OPTION_OUTPUT_TREE, reader.Tree.size() , "No-output tree count:%u, expected:0." , reader.Tree.size() );
	rtrn 0;
}

::gpk::err_t testJSONReader(::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	if_fail_fe(testJSONOptions(errors));
	if_fail_fe(testJSONValidDocuments(errors));
	if_fail_fe(testJSONSignedInteger(errors));
	if_fail_fe(testJSONCodePoints(errors));
	if_fail_fe(testJSONTreeFreshness(errors));
	rtrn testJSONReset(errors);
}
