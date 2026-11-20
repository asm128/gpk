#define GPK_CONSOLE_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_json.h"
#include "gpk_json_expression.h"

::gpk::err_t								printNode						(::gpk::SJSONNode* node, const ::gpk::vcst_t& testJson)			{
	const ::gpk::asc_t						viewPrintable					= ::gpk::toString({&testJson[node->Token->Span.Begin], node->Token->Span.End - node->Token->Span.Begin});
	const ::gpk::vcst_t						viewTokenType					= ::gpk::get_value_label(node->Token->Type);
	//info_printf("Entering JSON node type: %u (%s). Node Span: {%u, %u}. Parent index: %u. Object index: %u. Text: %s", node->Token->Type, viewTokenType.begin(), node->Token->Span.Begin, node->Token->Span.End, node->Token->ParentIndex, node->ObjectIndex, viewPrintable.begin());
	for(uint32_t iChildren = 0; iChildren < node->Children.size(); ++iChildren)
		::printNode(node->Children[iChildren], testJson);
	//info_printf("Exiting JSON node type: %u (%s). Node Span: {%u, %u}. Parent index: %u. Object index: %u. Text: %s", node->Token->Type, viewTokenType.begin(), node->Token->Span.Begin, node->Token->Span.End, node->Token->ParentIndex, node->ObjectIndex, viewPrintable.begin());
	return 0;
}

::gpk::err_t								testJSONReader				()			{
	static const ::gpk::vcst_t			testJson					= //"[{\"a\":[123 3,32,1156]}]";
		"\n            "
		"\n[\t {\"Bleh\": \"test\" }, {}, {   }, [  ]  , [], {\"a\":{}, \"b\":[123,]}"
		"\n,\t {\"NameId\" : \"ASD\", \"Bleh\" :[234124,123,243234, 321   , 65198], \"Else\": [{\"Object\": false}, {}],\"Something\" : \"out\",}"
		"\n,\t {\"NameId\" : \"654\", \"Bleh\":21354, \"Else\\u1954\": \"in\"} "
		"\n,\t {\"NameId\" : true, \"B\\\"leh\": null, \"Else\": false}		"
		"\n,\t {\"NameId\" : \"654\", \"Bleh\":21354, \"Else\\u1954\": \"in\"} "
		"\n,\t {\"NameId\" : true, \"B\\\"leh\": null, \"Else\": false} "
		"\n,\t {\"NameId\" : \"true\", \"Bleh\": \"null\", \"Else\": []} "
		//"\n, { \"NameId\" : 12344, \"Bleh\": \"\" \"\", \"Else\": \"false\", \"not\" : tres} "
		"\n,\t { \"NameId\" : .123, \"Bleh\": -456 , \"Else\": -.759 } "
		"\n]"
		"\n           "
		;
	info_printf("JSON string (%u characters): %s", testJson.size(), testJson.begin());

	const char										bleh[]						= "";
	info_printf("%s", bleh);

	::gpk::SJSONReader								jsonReader;
	gpk_necall(::gpk::jsonParse(jsonReader, testJson), "Failed to parse json: '%s'.", testJson.begin());

	info_printf("---------------------------- Linear iteration: %u objects.", jsonReader.Token.size());
	::gpk::asc_t						outputJson;
	::gpk::pobj<::gpk::SJSONNode>				root							= jsonReader.Tree[0];
	::printNode(root, testJson);

	info_printf("---------------------------- Linear iteration: %u objects.", jsonReader.Token.size());
	for(uint32_t iNode = 0; iNode < jsonReader.Token.size(); ++iNode) {
		const gpk::SJSONToken							& node							= jsonReader.Token[iNode];
		if( ::gpk::JSON_TYPE_VALUE	== node.Type
		 || ::gpk::JSON_TYPE_KEY	== node.Type
		 )
			continue;
		::gpk::vcst_t						view							= jsonReader.View[iNode];
		char											bufferFormat [8192]				= {};
		uint32_t										lenString						= view.size();
		sprintf_s(bufferFormat, "Node type: %%u (%%s). Node Span: {%%u, %%u}. Parent index: %%u. Object index: %%u. Text: %%%u.%us", lenString, lenString);
		info_printf(bufferFormat, node.Type, ::gpk::get_value_label(node.Type).begin(), node.Span.Begin, node.Span.End, node.ParentIndex, iNode, view.begin());
	}

	const ::gpk::vcst_t					test_key						= "Bleh";
	info_printf("---------------------------- Access test. Test key: %s", test_key.begin());
	::gpk::err_t									indexOfFirstObject				= ::gpk::jsonArrayValueGet(*root, 0);
	::gpk::pobj<::gpk::SJSONNode>				object							= jsonReader.Tree[indexOfFirstObject];
	::gpk::err_t									indexOfElement					= ::gpk::jsonObjectValueGet(*object, jsonReader.View, test_key);
	gpk_necall((uint32_t)indexOfElement >= jsonReader.View.size(), "Test key '%s' not found: %i.", test_key.begin(), indexOfElement);
	const gpk::SJSONToken							& node							= jsonReader.Token	[indexOfElement];
	::gpk::vcst_t						view							= jsonReader.View	[indexOfElement - 1];	// get the parent in order to retrieve the view with the surrounding in the case of strings "".
	char											bufferFormat [8192]				= {};
	uint32_t										lenString						= view.size();
	sprintf_s(bufferFormat, "Found test value {'%%s':%%%u.%us}. Node type: %%u (%%s). Node Span: {%%u, %%u}. Parent index: %%u. Object index: %%u.", lenString, lenString);
	info_printf(bufferFormat, test_key.begin(), view.begin(), node.Type, ::gpk::get_value_label(node.Type).begin(), node.Span.Begin, node.Span.End, node.ParentIndex, indexOfElement, view.begin());
	return 0;
}

	::gpk::err_t						testJSONFormatter			(::gpk::SJSONReader & jsonReader, const ::gpk::vcst_t format, const ::gpk::vcst_t inputJson)			{
	::gpk::pobj<::gpk::SJSONNode>				root						= jsonReader.Tree[0];
	::printNode(root, inputJson);
	const ::gpk::err_t							indexOfFirstObject			= 0;
	info_printf("Test format:\n%s", format.begin());
	::gpk::asc_t						formatted;
	gpk_necall(::gpk::jsonStringFormat(format, jsonReader, indexOfFirstObject, formatted), "%s", "Error formatting string from JSON object.");
	info_printf("Formatted string after jsonStringFormat():\n%s", formatted.begin());
	return 0;
}

	::gpk::err_t						testJSONExpression
	(	const ::gpk::SJSONReader						& jsonReader
	,	const ::gpk::vcst_t					expression
	,	const ::gpk::vcst_t					expected
	) {
	info_printf("Testing expression: %s", ::gpk::toString(expression).begin());
	::gpk::vcsc_t						result;
	::gpk::asc_t						output;
	::gpk::SExpressionReader				reader;
	::gpk::vcsc_t						trimmedExpression = expression;
	if_fail_fe(::gpk::trim(trimmedExpression));
	if_fail_fe(::gpk::expressionReaderParse(reader, trimmedExpression));
	::gpk::SJSONExpressionSolver		results;
	const ::gpk::err_t resolvedIndex = gpk::jsonExpressionResolve(reader, jsonReader, 0U, results, result);
	if(-1 == resolvedIndex || result != expected) {
		error_printf("Failed to resolve expression. \nExpression: %s \nExpected: %s \nResult: %.*s. Index: %i", expression.begin(), expected.begin(), GPK_VIEW(result), resolvedIndex);
		output										= result;
		output.append("\n");
		OutputDebugStringA(output.begin());
		return -1;
	}
	info_printf("Test succeeded. \nExpression: %s \nExpected: %s \nResult: ", expression.begin(), expected.begin());
	output										= result;
	output.append("\n");
	OutputDebugStringA(output.begin());
	return 0;
}

int											main						()	{
	//gpk_necall(::testJSONReader(), "%s", "Failed to read JSON!");
	uint32_t										testsSucceeded			= 0;
	const ::gpk::vcst_t					inputJson				=
		"\n{\t \"name\"           : \"Carlos\""
		"\n\t, \"height\"         : 1.60"
		"\n\t, \"color\"          : \"brown\""
		"\n\t, \"race\"           : \"red\""
		"\n\t, \"weight\"         : 160"
		"\n\t, \"bpm\"            : 160."
		"\n\t, \"hindsight\"      : 1.600000000"
		"\n\t, \"alive\"          : true"
		"\n\t, \"married\"        : false"
		"\n\t, \"children\"       : [\"Rene\", null, \"Jamie\"]"
		"\n\t, \"selection\"      : { \"index\" : 2, \"name\" : \"children\", \"active\" : false , \"default_property_name_path\" : \"properties['0'].name\" }"
		"\n\t, \"parents\"        : "
		"\n\t    [{ \"name\"    : \"Alice\""
		"\n\t    , \"property\" : "
		"\n\t    { \"color\"    : \"orange\", \"age\" : 16, \"race\" : \"fat\" }"
		"\n\t    },"
		"\n\t    { \"name\"     : \"Bob\" // this is another comment"
		"\n\t    , \"property\" : "
		"\n\t    { \"color\"    : \"green\", \"age\" : 32, \"race\" : \"thin\" }"
		"\n\t    }]"
		"\n\t, \"properties\" : [{ \"name\" : \"age\", \"type\" : \"int\"}, { \"name\" : \"color\", \"type\" : \"string\"}, { \"name\" : \"race\", \"type\" : \"string\"} ]"
		"\n}"
		;
	::gpk::SJSONReader								jsonReader;
	::gpk::kv<::gpk::vcst_t>						resultExpression[]		=
		{ {"true"			, "weight			== bpm																						" }	//   0
		, {"true"			, "height			== hindsight																					" }	//   0
		, {"true"			, "hindsight		== height																						" }	//   0
		, {"true"			, "bpm				== weight																						" }	//   0
		, {"false"			, "height			== weight																					" }	//   0
		, {"false"			, "hindsight		== bpm																						" }	//   0
		, {"false"			, "weight			== height																						" }	//   0
		, {"false"			, "bpm				== hindsight																					" }	//   0
		, {"false"			, "'Not'			== children[('0')]																				" }	//   0
		, {"false"			, "'Found'			== children[('1')]																				" }	//   1
		, {"false"			, "'Here'			== children[('2')]																				" }	//   2
		, {"false"			, "'Perro'			== 'Perre'																						" }	//   3
		, {"true"			, "'null'			== 'false'																						" }	//   3
		, {"true"			, "'null'			== ''																							" }	//   3
		, {"true"			, "'false'			== 'null'																						" }	//   3
		, {"true"			, "'false'			== ''																							" }	//   3
		, {"true"			, "	'false'			== 'null'																						" }	//   3
		, {"true"			, "	''				== 'null'																						" }	//   3
		, {"true"			, "	'null'			== 'false'																						" }	//   3
		, {"true"			, "	''				== 'false'																						" }	//   3
		, {"true"			, "'Rene'			== children['0']																				" }	//   4
		, {"true"			, "'null'			== children['1']																				" }	//   5
		, {"true"			, "'false'			== children['1']																				" }	//   5
		, {"true"			, "children[('1')]	== children['1']																				" }	//   5
		, {"false"			, "children[('2')]	== children[('1')]																				" }	//   5
		, {"true"			, "'Jamie'			== children[('2')]																				" }	//   6
		, {"false"			, "alive			== married																						" }	//   7
		, {"false"			, "!alive			== !married																						" }	//   8
		, {"true"			, "(!alive)			==(married)																						" }	//   9
		, {"true"			, "!alive			==married																						" }	//  10
		, {"false"			, "(!alive)			==(!married)																					" }	//  11
		, {"true"			, "(alive )			==(!married)																					" }	//  12
		, {"false"			, "!alive			==!married																						" }	//  13
		, {"true"			, "!alive			==married																						" }	//  14
		, {"false"			, "(alive )			=={ 'married'}																					" }	//  15
		, {"true"			, "(alive )			==!{ 'married'}																					" }	//  15
		, {"false"			, "'false'			== 'true'																						" }	//  16
		, {"true"			, "'true'			== 'true'																						" }	//  17
		, {"true"			, "'false'			== 'false'																						" }	//  18
		, {"true"			, "'Perro'			== 'Perro'																						" }	//  19
		, {"false"			, "(alive )			&&{ 'married'}																					" }	//  20
		, {"false"			, "(alive )			&&( married)																					" }	//  21
		, {"true"			, "'false'			|| 'true'																						" }	//  22
		, {"false"			, "(alive )			&&(married)																						" }	//  23
		, {"false"			, "(alive)			&&(married)																						" }	//  24
		, {"false"			, "(alive)			&&( married)																					" }	//  25
		, {"false"			, "'false'			&& 'false'																						" }	//  26
		, {"false"			, "'false'			|| 'false'																						" }	//  27
		, {"true"			, "'true'			|| 'false'																						" }	//  28
		, {"false"			, "!married			 ? !alive : 'unknown'																			" }	//  29
		, {"unknown"		, "married			? !alive : 'unknown'																			" }	//  30
		, {"false"			, "married			? 'unknown' : !alive 																			" }	//  31
		, {"unknown"		, "!married			 ? 'unknown' : !alive																			" }	//  32
		, {"true"			, "(alive )			&&!(married)																					" }	//  33
		, {"false"			, "!(alive )		&&!( married)																					" }	//  34
		, {"false"			, "!(alive )		&&!(married)																					" }	//  35
		, {"false"			, "!(alive)			&&!(married)																					" }	//  36
		, {"false"			, "!(alive)			&&!( married)																					" }	//  37
		, {"unknown"		, "married			&& alive && married ? !alive : 'unknown'														" }	//  38
		, {"true"			, "(alive && alive)	&& alive &&!( married)																			" }	//  39
		, {"true"			, "(alive)			&&!(married)																					" }	//  40
		, {"true"			, "(alive)			&&!( married)																					" }	//  41
		, {"true"			, "(!married) &&!( married)																							" }	//  42
		, {"false"			, "{'alive'}&&( married)																							" }	//  43
		, {"false"			, "{'alive'}&&(married)																								" }	//  44
		, {"false"			, "{'alive'}&&(married)																								" }	//  45
		, {"false"			, "{'alive'} &&( married)																							" }	//  46
		, {"false"			, "(alive )&&{'married' }																							" }	//  47
		, {"false"			, "(alive) &&{'married' }																							" }	//  48
		, {"false"			, "(alive) &&{ 'married'}																							" }	//  49
		, {"false"			, "alive && married																									" }	//  50
		, {"false"			, "alive &&married																									" }	//  51
		, {"false"			, "alive&&married																									" }	//  52
		, {"false"			, "alive&& married																									" }	//  53
		, {"it's alive!"	, "(!(!(alive))) ? \"it's alive!\" : 'dead!'																		" }	//  54
		, {"true"			, "alive																											" }	//  55
		, {"it's alive!"	, " alive  ? \"it's alive!\" : 'dead!'																				" }	//  56
		, {"true"			, "(alive)																											" }	//  57
		, {"it's alive!"	, "(alive) ? \"it's alive!\" : 'dead!'																				" }	//  58
		, {"false"			, "!(alive)																											" }	//  59
		, {"false"			, "(!(alive))																										" }	//  60
		, {"true"			, "!(!(alive))																										" }	//  61
		, {"true"			, "(!(!(alive)))																									" }	//  62
		, {"Carlos"			, "name																												" }	//  63
		, {"1.60"			, "height																											" }	//  64
		, {"brown"			, "color																											" }	//  65
		, {"b"				, "color['0']																										" }	//  66
		, {"red"			, "race																												" }	//  67
		, {"160"			, "weight																											" }	//  68
		, {"Carlos"			, "(name)																											" }	//  69
		, {"1.60"			, "(height)																											" }	//  70
		, {"brown"			, "(color)																											" }	//  71
		, {"red"			, "(race)																											" }	//  72
		, {"160"			, "(weight)																											" }	//  73
		, {"2"				, "selection.index																									" }	//  74
		, {"2"				, "(selection.('index'))																							" }	//  75
		, {"2"				, "((selection).index)																								" }	//  76
		, {"Not married"	, " married  ? 'true' : 'Not married'																				" }	//  77
		, {"Not married"	, "(married) ? 'true' : 'Not married'																				" }	//  78
		, {"true"			, " alive  ? ('true') : ('false')																					" }	//  79
		, {"true"			, "!!(!!alive) ? ('true') : ('false')																				" }	//  80
		, {"false"			, " married  ? ('true') : ('false')																					" }	//  81
		, {"false"			, "(married) ? ('true') : ('false')																					" }	//  82
		, {"true"			, "(alive  ? (married ? 'false' : 'true') )																			" }	//  83
		, {"true"			, "(alive  ? married ? 'false' : 'true' : 'unknown')																" }	//  84
		, {"alive"			, "(!alive || married ? 'dead' : 'alive')																			" }	//  85
		, {"true"			, " alive  ? married ? 'false' : 'true' : 'unknown'																	" }	//  86
		, {"alive"			, "(alive && !married ? 'alive' : 'dead')																			" }	//  87
		, {"it's alive!"	, "!alive? 'dead!' : \"it's alive!\"																				" }	//  88
		, {"it's alive!"	, "!!(alive) ? \"it's alive!\" : 'dead!'																			" }	//  89
		, {"it's alive!"	, "(!alive) ? 'dead!' : \"it's alive!\"																				" }	//  90
		, {"Rene"			, "children[('0')]																									" }	//  91
		, {"null"			, "children[('1')]																									" }	//  92
		, {"Jamie"			, "children[('2')]																									" }	//  93
		, {"Jamie"			, "children[selection.index]																						" }	//  94
		, {"Jamie"			, "children[(selection.('index'))]																					" }	//  95
		, {"Jamie"			, "children[({'selection'}.index)]																					" }	//  96
		, {"Jamie"			, "children[{'selection.index'}]																					" }	//  97
		, {"Alice"			, "parents['0'].name																								" }	//  98
		, {"Bob"			, "parents['1'].name																								" }	//  99
		, {"Alice"			, "'Alice'																											" }	// 100
		, {"Alice"			, "((parents['0']).name)																							" }	// 101
		, {"Bob"			, "((parents['1']).name)																							" }	// 102
		, {"Alice"			, "{\"(parents['0']).name\"}																						" }	// 103
		, {"Bob"			, "{\"(parents['1']).name\"}																						" }	// 104
		, {"2"				, "(selection . ('index'))																							" }	// 105
		, {"2"				, "(selection . index)																								" }	// 106
		, {"null"			, "(children[ ( ( '1') )] )																							" }	// 107
		, {"Jamie"			, "(children[((selection.index)) ])																					" }	// 108
		, {"is alive"		, "(alive ? married ? 'not alive' : 'is alive' : 'unknown')															" }	// 109
		, {"false"			, "!alive  ? ('is_alive') : (!'is dead')																			" }	// 110
		, {"is alive"		, "(alive  ? (!alive ? 'is_dead' : 'is alive'))																		" }	// 111
		, {"red"			, "married  ? !alive ? 'is_dead' : 'is_alive' : race																" }	// 112
		, {"unknown"		, "(!alive  ? (alive ? 'is_alive' : 'is_dead') : 'unknown')															" }	// 113
		, {"true"			, "!'0'																												" }	// 114
		, {"true"			, "!''																												" }	// 115
		, {"false"			, "!' '																												" }	// 116
		, {"true"			, "!'{}'																											" }	// 117
		, {"true"			, "!'[]'																											" }	// 118
		, {"true"			, "!'0'	&&	!'0'	 																								" }	// 119
		, {"true"			, "!''	&&	!''																										" }	// 120
		, {"false"			, "!' '	&&	!' '																									" }	// 121
		, {"true"			, "!'{}'&&	!'{}'																									" }	// 122
		, {"true"			, "!'[]'&&	!'[]'																									" }	// 123
		, {"true"			, "!'0'	||	!'0'	 																								" }	// 124
		, {"true"			, "!''	||	!''																										" }	// 125
		, {"false"			, "!' '	||	!' '																									" }	// 126
		, {"true"			, "!'{}'||	!'{}'																									" }	// 127
		, {"true"			, "!'[]'||	!'[]'																									" }	// 128
		, {"true"			, "'true'  || 'true'																								" }	// 129
		, {"32"				, "parents['1'].property.{selection.default_property_name_path}														" }	// 130
		, {"0"				, "!selection.active		? ('0') : selection.index																" }	// 131
		, {"0"				, "!(selection.active)		? ('0') : selection.index																" }	// 132
		, {"2"				, "(!(selection.active))	? (selection.index)	: '0'																" }	// 133
		, {"false"			, "selection.active																									" }	// 134
		, {"false"			, "(selection.active)																								" }	// 135
		, {"2"				, "(selection.active)	? ('0')	: (selection.index)																	" }	// 136
		, {"race"			, "!selection.active ? properties[selection.index].name : 'No selection.'											" }	// 137
		, {"No selection."	, "selection.active ? 'is active' : 'No selection.'																	" }	// 138
		, {"age"			, "properties[('0')].name																							" }	// 139
		, {"2"				, "(!selection.active) ? (selection.index) : ('1')																	" }	// 140
		, {"No selection."	, "selection.active ? properties[selection.index].name : 'No selection.'											" }	// 141
		, {"race"			, "selection.active ? 'No selection.' : properties[selection.index].name											" }	// 142
		, {"green"			, "parents['1'].property.(properties[selection.active ? selection.index : '1'].name)								" }	// 143
		, {"green"			, "parents['1'].property.(properties[selection.active ? (selection.index) : '1'].name)								" }	// 144
		, {"thin"			, "parents['1'].property.(properties[!selection.active ? (selection.index) : ('1')].name)							" }	// 145
		, {"green"			, "parents['1'].property.(properties[(selection.active) ? selection.index : '1'].name)								" }	// 146
		, {"16"				, "parents['0'].property.(properties[(selection.active) ? selection.index : '0'].name)								" }	// 147
		, {"2"				, "!selection.active	? selection.index : '1'																		" }	// 148
		, {"1"				, "(!selection.active)	? '1' : selection.index																		" }	// 149
		, {"green"			, "parents['1'].property.(properties[!selection.active		? '1'	: selection.index	].name)						" }	// 150
		, {"green"			, "parents['1'].property.(properties[!!!selection.active || married	? '1' : (selection.index)].name)				" }	// 151
		, {"green"			, "parents['1'].property.(properties[!!!selection.active || married	? '1' : (selection.index)].name)				" }	// 152
		, {"green"			, "parents['1'].property.(properties[!selection.active		? ('1')	: (selection.index) ].name)						" }	// 153
		, {"32"				, "parents['1'].property.{selection.default_property_name_path}														" }	// 154
		, {"orange"			, "parents['0'].property.(properties[(!selection.active) ? '1'	: selection.index	].name)							" }	// 155
		, {"orange"			, "parents['0'].property.(properties[!(selection.active) ? '1'	: selection.index	].name)							" }	// 156
		, {"green"			, "parents['1'].property.(properties[!(!!selection.active) && (married || alive) ? '1' : (selection.index)].name)	" }	// 157
		, {"green"			, "parents['1'].property.(properties[!(!!selection.active) && (alive || married) ? '1' : (selection.index)].name)	" }	// 158
		};

	info_printf("Input JSON:\n%s", inputJson.begin());
	gpk_necall(::gpk::jsonParse(jsonReader, inputJson), "Failed to parse json: '%s'.", inputJson.begin());
	for(uint32_t iTest = 0; iTest < ::gpk::size(resultExpression); ++iTest) {
		if(::gpk::failed(testJSONExpression(jsonReader, resultExpression[iTest].Val, resultExpression[iTest].Key))) error_printf( "Failed to resolve expression: %s", ::gpk::toString(resultExpression[iTest].Val).begin());
		else
			++testsSucceeded;
	}

	//::gpk::SJSONFile	fileCases;
	//::gpk::jsonFileRead(fileCases, "case.json");
	//for(uint32_t iCase = 0; iCase < (uint32_t)::gpk::jsonArraySize(*fileCases.Reader[0]); ++iCase) {
	//	const uint32_t			indexMain		= ::gpk::jsonArrayValueGet(*fileCases.Reader[0], iCase);
	//	const uint32_t			indexResult		= ::gpk::jsonArrayValueGet(*fileCases.Reader[indexMain], 0);
	//	const uint32_t			indexScript		= ::gpk::jsonArrayValueGet(*fileCases.Reader[indexMain], 1);
	//	const ::gpk::SJSONNode	& nodeResult	= *fileCases.Reader[indexResult];
	//	const ::gpk::SJSONNode	& nodeScript	= *fileCases.Reader[indexScript];
	//	gerror_if(::gpk::failed(testJSONExpression(jsonReader, fileCases.Reader.View[nodeScript.ObjectIndex], fileCases.Reader.View[nodeResult.ObjectIndex])), "Failed to resolve expression: %s", ::gpk::toString(fileCases.Reader.View[nodeScript.ObjectIndex]).begin())
	//	else
	//		++testsSucceeded;
	//}

	const ::gpk::vcst_t					format						=
		"I want to replace this (but not \\{this}): \n{parents['1'].name}'s {properties[selection.active ? selection.index : '1'].name}: // this comment should appear here"
		"\n{parents['1'].property.(properties[selection.active ? selection.index : '1'].name // this comment should not appear here"
		"\n)}."
		;
	if(::gpk::failed(::testJSONFormatter(jsonReader, format, inputJson))) error_printf( "Failed to format string!""\nFormat: \n%s""\nInput JSON: \n%s", format.begin(), inputJson.begin());
	else
		++testsSucceeded;
	info_printf("%u tests executed successfully", testsSucceeded);
	return 0;
}
