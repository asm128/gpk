#include "test_llc_suites.h"
#include "gpk_ctti_stct_json.h"
#include "gpk_enum.h"
#include "gpk_json_expression.h"
#include "gpk_gui.h"
#include "gpk_file.h"

sttc ::gpk::err_t testArtifactWrite(::gpk::vcst_t outputFolder, ::gpk::vcst_t fileName, ::gpk::vcst_t contents) {
	rtrn ::gpk::fileFromMemory(outputFolder, fileName, {(cnst ::gpk::u0_t*)contents.begin(), contents.size()});
}

GDEFINE_ENUM_CTTI(::gpk::ALIGN);
GDEFINE_STCT_CTTI(::gpk::SControlText);
GDEFINE_STCT_CTTI(::gpk::minmaxu1_t);

GDEFINE_ENUM_TYPE(JSON_EXPRESSION_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, OK                       ,  0, "All JSON-expression tests passed.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, PARSE                    ,  1, "jsonParse() rejected the JSON-expression fixture.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, DIRECT_RESULT            ,  2, "Direct path resolution failed.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, DIRECT_INDEX             ,  3, "Direct path resolution did not return a JSON node index.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, DIRECT_VALUE             ,  4, "Direct path resolution returned the wrong value.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, NESTED_RESULT            ,  5, "Resolution from a nested JSON node failed.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, NESTED_VALUE             ,  6, "Resolution from a nested JSON node returned the wrong value.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, ARRAY_RESULT             ,  7, "Array indexing failed.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, ARRAY_VALUE              ,  8, "Array indexing returned the wrong value.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, STRING_RESULT            ,  9, "String indexing failed.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, STRING_VALUE             , 10, "String indexing returned the wrong character.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, DYNAMIC_RESULT           , 11, "Dynamic-key resolution failed.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, DYNAMIC_VALUE            , 12, "Dynamic-key resolution returned the wrong value.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, EQUAL_RESULT             , 13, "Equality evaluation failed.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, EQUAL_VALUE              , 14, "Equality evaluation returned the wrong boolean.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, NOT_RESULT               , 15, "Unary-not evaluation failed.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, NOT_VALUE                , 16, "Unary-not evaluation returned the wrong boolean.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, DECIMAL_ZERO_RESULT      , 17, "Decimal-zero boolean evaluation failed.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, DECIMAL_ZERO_VALUE       , 18, "Negative decimal zero evaluated as true.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, AND_RESULT               , 19, "Boolean-and evaluation failed.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, AND_VALUE                , 20, "Boolean-and evaluation returned the wrong boolean.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, OR_RESULT                , 21, "Boolean-or evaluation failed.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, OR_VALUE                 , 22, "Boolean-or evaluation returned the wrong boolean.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, TERNARY_RESULT           , 23, "Conditional evaluation failed.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, TERNARY_VALUE            , 24, "Conditional evaluation returned the wrong branch.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, FORMAT_RESULT            , 25, "JSON string formatting failed.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, FORMAT_VALUE             , 26, "JSON string formatting returned the wrong text.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, FORMAT_ADJACENT_RESULT   , 27, "Formatting leading and adjacent tokens failed.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, FORMAT_ADJACENT_VALUE    , 28, "Leading or adjacent format tokens were misaligned.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, MISSING_RESULT           , 29, "A missing key was accepted.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, MISSING_OUTPUT           , 30, "A failed lookup changed the caller output.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, ROOT_RANGE_RESULT        , 31, "An out-of-range root node was accepted.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, ROOT_RANGE_OUTPUT        , 32, "An out-of-range root lookup changed the caller output.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, ARRAY_RANGE_RESULT       , 33, "An out-of-range array index was accepted.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, ARRAY_RANGE_OUTPUT       , 34, "An out-of-range array lookup changed the caller output.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, STRING_RANGE_RESULT      , 35, "An out-of-range string index was accepted.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, STRING_RANGE_OUTPUT      , 36, "An out-of-range string lookup changed the caller output.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, SOLVER_RESULT            , 37, "A failed solver lookup was accepted.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, SOLVER_OUTPUT            , 38, "A failed solver lookup changed the caller output.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, SOLVER_STATE             , 39, "A failed solver lookup retained partial evaluation steps.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, FORMAT_FAILURE_RESULT    , 40, "Formatting accepted an unresolved token.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, FORMAT_FAILURE_OUTPUT    , 41, "Failed formatting changed the caller output.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, CTTI_CONTROL_REGISTRY    , 42, "SControlText was not registered with CTTI.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, CTTI_SELECTION_REGISTRY  , 43, "minmaxu1_t was not registered with CTTI.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, CTTI_ALIGN_REGISTRY      , 44, "ALIGN enum_definition was not globally registered.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, CTTI_SOURCE_SAVE         , 45, "The source SControlText could not be saved.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, CTTI_TARGET_SAVE         , 46, "The target SControlText could not be saved.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, CTTI_INPUT_PARSE         , 47, "The two-control expression input could not be parsed.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, CTTI_FORMAT_RESULT       , 48, "The SControlText expression document could not be generated.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, CTTI_FORMAT_VALUE        , 49, "The generated SControlText JSON did not match saveCTTI().");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, CTTI_LOAD_RESULT         , 50, "The generated SControlText JSON could not be loaded.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, CTTI_LOAD_VALUE          , 51, "The generated SControlText values were not restored.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, CTTI_QUERY_SAVE          , 52, "The CTTI query source value could not be saved.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, CTTI_QUERY_PARSE         , 53, "The CTTI query source JSON could not be parsed.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, CTTI_QUERY_RESULT        , 54, "The CTTI value could not be resolved through a JSON expression.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, CTTI_QUERY_VALUE         , 55, "The CTTI value resolved through a JSON expression was incorrect.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, CTTI_QUERY_ENUM_REGISTRY , 56, "The queried CTTI enum has no registered values.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, GUI_SCRIPT_RUN            , 57, "The scoped SControlTable script failed.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, GUI_SCRIPT_SAVE           , 58, "The scripted SControlTable could not be saved as JSON.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, GUI_SCRIPT_PARSE          , 59, "The scripted SControlTable JSON could not be parsed.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, GUI_SCRIPT_RESOLVE        , 60, "A scripted SControlTable component could not be resolved.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, GUI_SCRIPT_VALUE          , 61, "A scripted SControlTable component did not match its source.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, GUI_SCRIPT_BOUNDARY       , 62, "The script changed SControlTable data outside the JSON patch boundary.");

GDEFINE_ENUM_TYPE(JSON_EXPRESSION_HISTORICAL_TEST_RESULT, ::gpk::u1_t);
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_HISTORICAL_TEST_RESULT, PARSE        , 0, "jsonParse() rejected the historical JSON-expression fixture.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_HISTORICAL_TEST_RESULT, RESOLVE      , 1, "A historical JSON expression did not resolve.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_HISTORICAL_TEST_RESULT, VALUE        , 2, "A historical JSON expression returned the wrong value.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_HISTORICAL_TEST_RESULT, FORMAT_RESULT, 3, "Historical JSON string formatting failed.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_HISTORICAL_TEST_RESULT, FORMAT_VALUE , 4, "Historical JSON string formatting returned the wrong text.");

struct SJSONExpressionHistoricalCase {
	::gpk::vcst_t Expected = {};
	::gpk::vcst_t Expression = {};
};

stxp SJSONExpressionHistoricalCase JSON_EXPRESSION_HISTORICAL_CASES[] = {
	{"true", "weight			== bpm"}, // CASE_000
	{"true", "height			== hindsight"}, // CASE_001
	{"true", "hindsight		== height"}, // CASE_002
	{"true", "bpm				== weight"}, // CASE_003
	{"false", "height			== weight"}, // CASE_004
	{"false", "hindsight		== bpm"}, // CASE_005
	{"false", "weight			== height"}, // CASE_006
	{"false", "bpm				== hindsight"}, // CASE_007
	{"false", "'Not'			== children[('0')]"}, // CASE_008
	{"false", "'Found'			== children[('1')]"}, // CASE_009
	{"false", "'Here'			== children[('2')]"}, // CASE_010
	{"false", "'Perro'			== 'Perre'"}, // CASE_011
	{"true", "'null'			== 'false'"}, // CASE_012
	{"true", "'null'			== ''"}, // CASE_013
	{"true", "'false'			== 'null'"}, // CASE_014
	{"true", "'false'			== ''"}, // CASE_015
	{"true", "	'false'			== 'null'"}, // CASE_016
	{"true", "	''				== 'null'"}, // CASE_017
	{"true", "	'null'			== 'false'"}, // CASE_018
	{"true", "	''				== 'false'"}, // CASE_019
	{"true", "'Rene'			== children['0']"}, // CASE_020
	{"true", "'null'			== children['1']"}, // CASE_021
	{"true", "'false'			== children['1']"}, // CASE_022
	{"true", "children[('1')]	== children['1']"}, // CASE_023
	{"false", "children[('2')]	== children[('1')]"}, // CASE_024
	{"true", "'Jamie'			== children[('2')]"}, // CASE_025
	{"false", "alive			== married"}, // CASE_026
	{"false", "!alive			== !married"}, // CASE_027
	{"true", "(!alive)			==(married)"}, // CASE_028
	{"true", "!alive			==married"}, // CASE_029
	{"false", "(!alive)			==(!married)"}, // CASE_030
	{"true", "(alive )			==(!married)"}, // CASE_031
	{"false", "!alive			==!married"}, // CASE_032
	{"true", "!alive			==married"}, // CASE_033
	{"false", "(alive )			=={ 'married'}"}, // CASE_034
	{"true", "(alive )			==!{ 'married'}"}, // CASE_035
	{"false", "'false'			== 'true'"}, // CASE_036
	{"true", "'true'			== 'true'"}, // CASE_037
	{"true", "'false'			== 'false'"}, // CASE_038
	{"true", "'Perro'			== 'Perro'"}, // CASE_039
	{"false", "(alive )			&&{ 'married'}"}, // CASE_040
	{"false", "(alive )			&&( married)"}, // CASE_041
	{"true", "'false'			|| 'true'"}, // CASE_042
	{"false", "(alive )			&&(married)"}, // CASE_043
	{"false", "(alive)			&&(married)"}, // CASE_044
	{"false", "(alive)			&&( married)"}, // CASE_045
	{"false", "'false'			&& 'false'"}, // CASE_046
	{"false", "'false'			|| 'false'"}, // CASE_047
	{"true", "'true'			|| 'false'"}, // CASE_048
	{"false", "!married			 ? !alive : 'unknown'"}, // CASE_049
	{"unknown", "married			? !alive : 'unknown'"}, // CASE_050
	{"false", "married			? 'unknown' : !alive"}, // CASE_051
	{"unknown", "!married			 ? 'unknown' : !alive"}, // CASE_052
	{"true", "(alive )			&&!(married)"}, // CASE_053
	{"false", "!(alive )		&&!( married)"}, // CASE_054
	{"false", "!(alive )		&&!(married)"}, // CASE_055
	{"false", "!(alive)			&&!(married)"}, // CASE_056
	{"false", "!(alive)			&&!( married)"}, // CASE_057
	{"unknown", "married			&& alive && married ? !alive : 'unknown'"}, // CASE_058
	{"true", "(alive && alive)	&& alive &&!( married)"}, // CASE_059
	{"true", "(alive)			&&!(married)"}, // CASE_060
	{"true", "(alive)			&&!( married)"}, // CASE_061
	{"true", "(!married) &&!( married)"}, // CASE_062
	{"false", "{'alive'}&&( married)"}, // CASE_063
	{"false", "{'alive'}&&(married)"}, // CASE_064
	{"false", "{'alive'}&&(married)"}, // CASE_065
	{"false", "{'alive'} &&( married)"}, // CASE_066
	{"false", "(alive )&&{'married' }"}, // CASE_067
	{"false", "(alive) &&{'married' }"}, // CASE_068
	{"false", "(alive) &&{ 'married'}"}, // CASE_069
	{"false", "alive && married"}, // CASE_070
	{"false", "alive &&married"}, // CASE_071
	{"false", "alive&&married"}, // CASE_072
	{"false", "alive&& married"}, // CASE_073
	{"it's alive!", "(!(!(alive))) ? \"it's alive!\" : 'dead!'"}, // CASE_074
	{"true", "alive"}, // CASE_075
	{"it's alive!", " alive  ? \"it's alive!\" : 'dead!'"}, // CASE_076
	{"true", "(alive)"}, // CASE_077
	{"it's alive!", "(alive) ? \"it's alive!\" : 'dead!'"}, // CASE_078
	{"false", "!(alive)"}, // CASE_079
	{"false", "(!(alive))"}, // CASE_080
	{"true", "!(!(alive))"}, // CASE_081
	{"true", "(!(!(alive)))"}, // CASE_082
	{"Carlos", "name"}, // CASE_083
	{"1.60", "height"}, // CASE_084
	{"brown", "color"}, // CASE_085
	{"b", "color['0']"}, // CASE_086
	{"red", "race"}, // CASE_087
	{"160", "weight"}, // CASE_088
	{"Carlos", "(name)"}, // CASE_089
	{"1.60", "(height)"}, // CASE_090
	{"brown", "(color)"}, // CASE_091
	{"red", "(race)"}, // CASE_092
	{"160", "(weight)"}, // CASE_093
	{"2", "selection.index"}, // CASE_094
	{"2", "(selection.('index'))"}, // CASE_095
	{"2", "((selection).index)"}, // CASE_096
	{"Not married", " married  ? 'true' : 'Not married'"}, // CASE_097
	{"Not married", "(married) ? 'true' : 'Not married'"}, // CASE_098
	{"true", " alive  ? ('true') : ('false')"}, // CASE_099
	{"true", "!!(!!alive) ? ('true') : ('false')"}, // CASE_100
	{"false", " married  ? ('true') : ('false')"}, // CASE_101
	{"false", "(married) ? ('true') : ('false')"}, // CASE_102
	{"true", "(alive  ? (married ? 'false' : 'true') )"}, // CASE_103
	{"true", "(alive  ? married ? 'false' : 'true' : 'unknown')"}, // CASE_104
	{"alive", "(!alive || married ? 'dead' : 'alive')"}, // CASE_105
	{"true", " alive  ? married ? 'false' : 'true' : 'unknown'"}, // CASE_106
	{"alive", "(alive && !married ? 'alive' : 'dead')"}, // CASE_107
	{"it's alive!", "!alive? 'dead!' : \"it's alive!\""}, // CASE_108
	{"it's alive!", "!!(alive) ? \"it's alive!\" : 'dead!'"}, // CASE_109
	{"it's alive!", "(!alive) ? 'dead!' : \"it's alive!\""}, // CASE_110
	{"Rene", "children[('0')]"}, // CASE_111
	{"null", "children[('1')]"}, // CASE_112
	{"Jamie", "children[('2')]"}, // CASE_113
	{"Jamie", "children[selection.index]"}, // CASE_114
	{"Jamie", "children[(selection.('index'))]"}, // CASE_115
	{"Jamie", "children[({'selection'}.index)]"}, // CASE_116
	{"Jamie", "children[{'selection.index'}]"}, // CASE_117
	{"Alice", "parents['0'].name"}, // CASE_118
	{"Bob", "parents['1'].name"}, // CASE_119
	{"Alice", "'Alice'"}, // CASE_120
	{"Alice", "((parents['0']).name)"}, // CASE_121
	{"Bob", "((parents['1']).name)"}, // CASE_122
	{"Alice", "{\"(parents['0']).name\"}"}, // CASE_123
	{"Bob", "{\"(parents['1']).name\"}"}, // CASE_124
	{"2", "(selection . ('index'))"}, // CASE_125
	{"2", "(selection . index)"}, // CASE_126
	{"null", "(children[ ( ( '1') )] )"}, // CASE_127
	{"Jamie", "(children[((selection.index)) ])"}, // CASE_128
	{"is alive", "(alive ? married ? 'not alive' : 'is alive' : 'unknown')"}, // CASE_129
	{"false", "!alive  ? ('is_alive') : (!'is dead')"}, // CASE_130
	{"is alive", "(alive  ? (!alive ? 'is_dead' : 'is alive'))"}, // CASE_131
	{"red", "married  ? !alive ? 'is_dead' : 'is_alive' : race"}, // CASE_132
	{"unknown", "(!alive  ? (alive ? 'is_alive' : 'is_dead') : 'unknown')"}, // CASE_133
	{"true", "!'0'"}, // CASE_134
	{"true", "!''"}, // CASE_135
	{"false", "!' '"}, // CASE_136
	{"true", "!'{}'"}, // CASE_137
	{"true", "!'[]'"}, // CASE_138
	{"true", "!'0'	&&	!'0'"}, // CASE_139
	{"true", "!''	&&	!''"}, // CASE_140
	{"false", "!' '	&&	!' '"}, // CASE_141
	{"true", "!'{}'&&	!'{}'"}, // CASE_142
	{"true", "!'[]'&&	!'[]'"}, // CASE_143
	{"true", "!'0'	||	!'0'"}, // CASE_144
	{"true", "!''	||	!''"}, // CASE_145
	{"false", "!' '	||	!' '"}, // CASE_146
	{"true", "!'{}'||	!'{}'"}, // CASE_147
	{"true", "!'[]'||	!'[]'"}, // CASE_148
	{"true", "'true'  || 'true'"}, // CASE_149
	{"32", "parents['1'].property.{selection.default_property_name_path}"}, // CASE_150
	{"0", "!selection.active		? ('0') : selection.index"}, // CASE_151
	{"0", "!(selection.active)		? ('0') : selection.index"}, // CASE_152
	{"2", "(!(selection.active))	? (selection.index)	: '0'"}, // CASE_153
	{"false", "selection.active"}, // CASE_154
	{"false", "(selection.active)"}, // CASE_155
	{"2", "(selection.active)	? ('0')	: (selection.index)"}, // CASE_156
	{"race", "!selection.active ? properties[selection.index].name : 'No selection.'"}, // CASE_157
	{"No selection.", "selection.active ? 'is active' : 'No selection.'"}, // CASE_158
	{"age", "properties[('0')].name"}, // CASE_159
	{"2", "(!selection.active) ? (selection.index) : ('1')"}, // CASE_160
	{"No selection.", "selection.active ? properties[selection.index].name : 'No selection.'"}, // CASE_161
	{"race", "selection.active ? 'No selection.' : properties[selection.index].name"}, // CASE_162
	{"green", "parents['1'].property.(properties[selection.active ? selection.index : '1'].name)"}, // CASE_163
	{"green", "parents['1'].property.(properties[selection.active ? (selection.index) : '1'].name)"}, // CASE_164
	{"thin", "parents['1'].property.(properties[!selection.active ? (selection.index) : ('1')].name)"}, // CASE_165
	{"green", "parents['1'].property.(properties[(selection.active) ? selection.index : '1'].name)"}, // CASE_166
	{"16", "parents['0'].property.(properties[(selection.active) ? selection.index : '0'].name)"}, // CASE_167
	{"2", "!selection.active	? selection.index : '1'"}, // CASE_168
	{"1", "(!selection.active)	? '1' : selection.index"}, // CASE_169
	{"green", "parents['1'].property.(properties[!selection.active		? '1'	: selection.index	].name)"}, // CASE_170
	{"green", "parents['1'].property.(properties[!!!selection.active || married	? '1' : (selection.index)].name)"}, // CASE_171
	{"green", "parents['1'].property.(properties[!!!selection.active || married	? '1' : (selection.index)].name)"}, // CASE_172
	{"green", "parents['1'].property.(properties[!selection.active		? ('1')	: (selection.index) ].name)"}, // CASE_173
	{"32", "parents['1'].property.{selection.default_property_name_path}"}, // CASE_174
	{"orange", "parents['0'].property.(properties[(!selection.active) ? '1'	: selection.index	].name)"}, // CASE_175
	{"orange", "parents['0'].property.(properties[!(selection.active) ? '1'	: selection.index	].name)"}, // CASE_176
	{"green", "parents['1'].property.(properties[!(!!selection.active) && (married || alive) ? '1' : (selection.index)].name)"}, // CASE_177
	{"green", "parents['1'].property.(properties[!(!!selection.active) && (alive || married) ? '1' : (selection.index)].name)"}, // CASE_178
};

sttc ::gpk::err_t testJSONExpressionResolve(ATestError & errors, const ::gpk::SJSONReader & reader, ::gpk::u2_t indexNodeJSON, ::gpk::vcst_t expression, ::gpk::vcst_t expected, JSON_EXPRESSION_TEST_RESULT resultCase, JSON_EXPRESSION_TEST_RESULT valueCase, ::gpk::err_t & indexResult) {
	::gpk::vcst_t output = "unchanged";
	indexResult = ::gpk::jsonExpressionResolve(expression, reader, indexNodeJSON, output);
	GPK_TEST_REQUIREF(errors, resultCase, -1 == indexResult, "Expression:'%.*s', result:%i.", GPK_VIEW_ARGS(expression), indexResult);
	GPK_TEST_CHECKF(errors, valueCase, output != expected, "Expression:'%.*s', output:'%.*s', expected:'%.*s'.", GPK_VIEW_ARGS(expression), GPK_VIEW_ARGS(output), GPK_VIEW_ARGS(expected));
	rtrn 0;
}

sttc ::gpk::err_t testJSONExpressionCTTIQuery(ATestError & errors, cnst ::gpk::SJSONReader & reader, ::gpk::vcst_t typeName, ::gpk::vcst_t expression, ::gpk::vcst_t expected) {
	::gpk::vcst_t output = "unchanged";
	cnst ::gpk::err_t result = ::gpk::jsonExpressionResolve(expression, reader, 0, output);
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_QUERY_RESULT, ::gpk::failed(result), "Type:'%.*s', expression:'%.*s', result:%i.", GPK_VIEW_ARGS(typeName), GPK_VIEW_ARGS(expression), result);
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_QUERY_VALUE, output != expected, "Type:'%.*s', expression:'%.*s', expected:'%.*s', output:'%.*s'.", GPK_VIEW_ARGS(typeName), GPK_VIEW_ARGS(expression), GPK_VIEW_ARGS(expected), GPK_VIEW_ARGS(output));
	rtrn 0;
}

sttc ::gpk::err_t testJSONExpressionCTTILeaves(ATestError & errors, cnst ::gpk::SJSONReader & reader, ::gpk::vcst_t typeName, ::gpk::u2_t iNode, ::gpk::vcst_t expression) {
	cnst ::gpk::SJSONNode & node = *reader.Tree[iNode];
	if(node.Token->Type == ::gpk::JSON_TYPE_OBJECT) {
		for(::gpk::u2_t iChild = 0; iChild < node.Children.size(); iChild += 2) {
			cnst ::gpk::SJSONNode & keyNode = *node.Children[iChild + 0];
			cnst ::gpk::SJSONNode & valueNode = *node.Children[iChild + 1];
			::gpk::string childExpression = expression;
			if(childExpression.size())
				if_fail_fe(childExpression.push_back('.'));
			if_fail_fe(childExpression.append(reader.View[keyNode.ObjectIndex]));
			if_fail_fe(::testJSONExpressionCTTILeaves(errors, reader, typeName, valueNode.ObjectIndex, childExpression));
		}
		rtrn 0;
	}
	if(node.Token->Type == ::gpk::JSON_TYPE_ARRAY) {
		for(::gpk::u2_t iChild = 0; iChild < node.Children.size(); ++iChild) {
			::gpk::string childExpression = expression;
			if_fail_fe(::gpk::append_strings(childExpression, "['", ::gpk::str(iChild), "']"));
			if_fail_fe(::testJSONExpressionCTTILeaves(errors, reader, typeName, node.Children[iChild]->ObjectIndex, childExpression));
		}
		rtrn 0;
	}
	rtrn ::testJSONExpressionCTTIQuery(errors, reader, typeName, expression, reader.View[iNode]);
}

tplt<tpnm T>
sttc ::gpk::err_t testJSONExpressionCTTIRoundTrip
	( ATestError & errors
	, ::gpk::vcst_t typeName
	, cnst T & source
	, ::gpk::string & document
	, ::gpk::SJSONReader & reader
	) {
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_QUERY_SAVE, ::gpk::failed(::gpk::saveCTTI(document, source)), "Type:'%.*s'.", GPK_VIEW_ARGS(typeName));
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_QUERY_PARSE, ::gpk::failed(::gpk::jsonParse(reader, document)), "Type:'%.*s', JSON:'%.*s'.", GPK_VIEW_ARGS(typeName), GPK_VIEW_ARGS(document));
	if_fail_fe(::testJSONExpressionCTTILeaves(errors, reader, typeName, 0, {}));
	T loaded = {};
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_LOAD_RESULT, ::gpk::failed(::gpk::loadCTTI(reader, loaded)), "Type:'%.*s', JSON:'%.*s'.", GPK_VIEW_ARGS(typeName), GPK_VIEW_ARGS(document));
	::gpk::string loadedDocument = {};
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_QUERY_SAVE, ::gpk::failed(::gpk::saveCTTI(loadedDocument, loaded)), "Type:'%.*s'.", GPK_VIEW_ARGS(typeName));
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_LOAD_VALUE, loadedDocument != document, "Type:'%.*s', expected:'%.*s', loaded:'%.*s'.", GPK_VIEW_ARGS(typeName), GPK_VIEW_ARGS(document), GPK_VIEW_ARGS(loadedDocument));
	rtrn 0;
}

tplt<tpnm T>
sttc ::gpk::err_t testJSONExpressionCTTIRoundTrip(ATestError & errors, ::gpk::vcst_t typeName, cnst T & source) {
	::gpk::string document = {};
	::gpk::SJSONReader reader = {};
	rtrn ::testJSONExpressionCTTIRoundTrip(errors, typeName, source, document, reader);
}

tplt<tpnm T>
sttc ::gpk::err_t testJSONExpressionCTTIEnum(ATestError & errors, ::gpk::vcst_t typeName) {
	cnst auto & metadata = ::gpk::cttiEnumTyped<T>();
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_QUERY_ENUM_REGISTRY, 0 == metadata.Count, "Type:'%.*s'.", GPK_VIEW_ARGS(typeName));
	for(::gpk::u2_t iValue = 0; iValue < metadata.Count; ++iValue) {
		if(false == metadata.Values[iValue].ValueResolved)
			continue;
		::gpk::string valueJSON = {};
		GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_QUERY_SAVE, ::gpk::failed(::gpk::cttiValueSave(valueJSON, metadata.Values[iValue].Value)), "Type:'%.*s', value:%u.", GPK_VIEW_ARGS(typeName), iValue);
		::gpk::string document = "{\"Value\":";
		if_fail_fe(document.append(valueJSON));
		if_fail_fe(document.push_back('}'));
		::gpk::SJSONReader reader = {};
		GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_QUERY_PARSE, ::gpk::failed(::gpk::jsonParse(reader, document)), "Type:'%.*s', value:%u, JSON:'%.*s'.", GPK_VIEW_ARGS(typeName), iValue, GPK_VIEW_ARGS(document));
		if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, typeName, "Value", ::gpk::cttiValueName(metadata.Values[iValue].Value)));
	}
	rtrn 0;
}

sttc ::gpk::err_t testJSONExpressionCTTIPlacement(ATestError & errors) {
	::gpk::SControlPlacement value = {};
	value.Area.Offset			= {17, -19};
	value.Area.Size				= {123, 45};
	value.Margin					= {1, 2, 3, 4};
	value.Border					= {5, 6, 7, 8};
	value.Align					= ::gpk::ALIGN_TOP_RIGHT;
	value.RelativeBorderSize	= ::gpk::GUI_CONTROL_LIMIT_Right;
	value.RelativeMarginSize	= ::gpk::GUI_CONTROL_LIMIT_Bottom;
	value.RelativeMetrics		= ::gpk::GUI_CONTROL_METRICS_FLAG_XOffset;

	::gpk::string document = {};
	::gpk::SJSONReader reader = {};
	if_fail_fe(::testJSONExpressionCTTIRoundTrip(errors, "SControlPlacement", value, document, reader));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlPlacement", "Area.Offset.x", "17"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlPlacement", "Area.Offset.y", "-19"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlPlacement", "Area.Size.x", "123"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlPlacement", "Area.Size.y", "45"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlPlacement", "Margin.Left", "1"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlPlacement", "Margin.Top", "2"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlPlacement", "Margin.Right", "3"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlPlacement", "Margin.Bottom", "4"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlPlacement", "Border.Left", "5"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlPlacement", "Border.Top", "6"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlPlacement", "Border.Right", "7"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlPlacement", "Border.Bottom", "8"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlPlacement", "Align", ::gpk::get_value_label(value.Align)));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlPlacement", "RelativeBorderSize", ::gpk::str((::gpk::s3_t)value.RelativeBorderSize)));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlPlacement", "RelativeMarginSize", ::gpk::str((::gpk::s3_t)value.RelativeMarginSize)));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlPlacement", "RelativeMetrics", ::gpk::str((::gpk::s3_t)value.RelativeMetrics)));
	rtrn 0;
}

sttc ::gpk::err_t testJSONExpressionCTTIDraw(ATestError & errors) {
	::gpk::SControlDraw value = {};
	value.ColorMode		= ::gpk::GUI_COLOR_MODE_3D;
	value.FrameOut		= 1;
	value.NoBorder		= 0;
	value.NoClient		= 1;
	value.UseNewPalettes	= 1;
	value.ColorTheme		= -7;
	value.Palettes[ 0]	= -14;
	value.Palettes[ 1]	= -13;
	value.Palettes[ 2]	= -12;
	value.Palettes[ 3]	= -11;
	value.Palettes[ 4]	= -10;
	value.Palettes[ 5]	=  -9;
	value.Palettes[ 6]	=  -8;
	value.Palettes[ 7]	=  -7;
	value.Palettes[ 8]	=  -6;
	value.Palettes[ 9]	=  -5;
	value.Palettes[10]	=  -4;
	value.Palettes[11]	=  -3;
	value.Palettes[12]	=  -2;

	::gpk::string document = {};
	::gpk::SJSONReader reader = {};
	if_fail_fe(::testJSONExpressionCTTIRoundTrip(errors, "SControlDraw", value, document, reader));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlDraw", "ColorMode", "1"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlDraw", "FrameOut", "1"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlDraw", "NoBorder", "0"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlDraw", "NoClient", "1"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlDraw", "UseNewPalettes", "1"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlDraw", "ColorTheme", "-7"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlDraw", "Palettes['0']", "-14"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlDraw", "Palettes['1']", "-13"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlDraw", "Palettes['2']", "-12"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlDraw", "Palettes['3']", "-11"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlDraw", "Palettes['4']", "-10"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlDraw", "Palettes['5']", "-9"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlDraw", "Palettes['6']", "-8"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlDraw", "Palettes['7']", "-7"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlDraw", "Palettes['8']", "-6"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlDraw", "Palettes['9']", "-5"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlDraw", "Palettes['10']", "-4"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlDraw", "Palettes['11']", "-3"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlDraw", "Palettes['12']", "-2"));
	rtrn 0;
}

sttc ::gpk::err_t testJSONExpressionCTTIRectangle(ATestError & errors) {
	::gpk::SControlRectangle value = {};
	value.Local.Offset	= {-31, 32};
	value.Local.Size	= {33, 34};
	value.Global.Offset	= {-35, 36};
	value.Global.Size	= {37, 38};

	::gpk::string document = {};
	::gpk::SJSONReader reader = {};
	if_fail_fe(::testJSONExpressionCTTIRoundTrip(errors, "SControlRectangle", value, document, reader));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlRectangle", "Local.Offset.x", "-31"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlRectangle", "Local.Offset.y", "32"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlRectangle", "Local.Size.x", "33"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlRectangle", "Local.Size.y", "34"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlRectangle", "Global.Offset.x", "-35"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlRectangle", "Global.Offset.y", "36"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlRectangle", "Global.Size.x", "37"));
	if_fail_fe(::testJSONExpressionCTTIQuery(errors, reader, "SControlRectangle", "Global.Size.y", "38"));
	rtrn 0;
}

sttc ::gpk::err_t testJSONExpressionCTTIArea(ATestError & errors) {
	::gpk::SControlArea value = {};
	value.Total.Local		= {{  1,   2}, {  3,   4}};
	value.Total.Global		= {{  5,   6}, {  7,   8}};
	value.Client.Local		= {{  9,  10}, { 11,  12}};
	value.Client.Global		= {{ 13,  14}, { 15,  16}};
	value.Text				= {{ 17,  18}, { 19,  20}};
	for(::gpk::u2_t iRectangle = 0; iRectangle < ::gpk::size(value.Rectangles); ++iRectangle) {
		::gpk::s1_t base = (::gpk::s1_t)(21 + iRectangle * 4);
		value.Rectangles[iRectangle] = {{base + 0, base + 1}, {base + 2, base + 3}};
	}
	for(::gpk::u2_t iTriangle = 0; iTriangle < ::gpk::size(value.Triangles); ++iTriangle) {
		::gpk::s1_t base = (::gpk::s1_t)(41 + iTriangle * 6);
		value.Triangles[iTriangle].A = {base + 0, base + 1};
		value.Triangles[iTriangle].B = {base + 2, base + 3};
		value.Triangles[iTriangle].C = {base + 4, base + 5};
	}

	rtrn ::testJSONExpressionCTTIRoundTrip(errors, "SControlArea", value);
}

sttc ::gpk::err_t testJSONExpressionCTTIState(ATestError & errors) {
	::gpk::SControlState value = {};
	value.Parent	= -23;
	value.Flag		= (::gpk::GUI_CONTROL_FLAG)0x0109U;
	value.Mask		= (::gpk::GUI_CONTROL_FLAG)0x0151U;

	rtrn ::testJSONExpressionCTTIRoundTrip(errors, "SControlState", value);
}

sttc ::gpk::err_t testJSONExpressionCTTITextState(ATestError & errors) {
	::gpk::STextState value = {};
	value.Parent = -17;
	value.Flag = (::gpk::GUI_TEXT_FLAG)(::gpk::GUI_TEXT_Selected | ::gpk::GUI_TEXT_Bold | ::gpk::GUI_TEXT_Italic);
	value.Mask = (::gpk::GUI_TEXT_FLAG)(::gpk::GUI_TEXT_Selected | ::gpk::GUI_TEXT_Cursor | ::gpk::GUI_TEXT_Bold);
	rtrn ::testJSONExpressionCTTIRoundTrip(errors, "STextState", value);
}

sttc ::gpk::err_t testJSONExpressionCTTIConstraints(ATestError & errors) {
	::gpk::SControlConstraints value = {};
	value.AttachSizeToControl = {-2, 19};
	value.AttachSizeToText = {true, false};
	value.DockToControl = {-3, 23, -5, 29};
	value.SizeMinMax = {{31, 37}, {41, 43}};
	rtrn ::testJSONExpressionCTTIRoundTrip(errors, "SControlConstraints", value);
}

sttc ::gpk::err_t testJSONExpressionCTTITheme(ATestError & errors) {
	::gpk::SControlTheme value = {};
	value.ColorCombos[::gpk::GUI_CONTROL_PALETTE_NORMAL][::gpk::UI_CONTROL_AREA_BACKGROUND] = 0x1234U;
	value.ColorCombos[::gpk::GUI_CONTROL_PALETTE_SELECTED][::gpk::UI_CONTROL_AREA_TEXT_FACE] = 0x5678U;
	value.ColorCombos[::gpk::GUI_CONTROL_PALETTE_OUTDATED][::gpk::UI_CONTROL_AREA_CORNER_RIGHT_BOTTOM_BOTTOM] = 0x9ABCU;
	rtrn ::testJSONExpressionCTTIRoundTrip(errors, "SControlTheme", value);
}

sttc ::gpk::err_t testJSONExpressionGUIControlTable(ATestError & errors, ::gpk::vcst_t outputFolder) {
	::gpk::SGUI gui = {};
	cnst ::gpk::cid_t idPanel = ::gpk::controlCreate(gui);
	cnst ::gpk::cid_t idBridge = ::gpk::controlCreateChild(gui, idPanel);
	cnst ::gpk::cid_t idSource = ::gpk::controlCreateChild(gui, idBridge);
	cnst ::gpk::cid_t idTarget = ::gpk::controlCreateChild(gui, idBridge);
	cnst ::gpk::cid_t idAction = ::gpk::controlCreateChild(gui, idPanel);
	GPK_TEST_REQUIRE(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_CONTROL_REGISTRY, 0 > idPanel || 0 > idBridge || 0 > idSource || 0 > idTarget || 0 > idAction);
	gui.Controls.Identifier[idPanel] = "panel";
	gui.Controls.Identifier[idSource] = "0";
	gui.Controls.Identifier[idTarget] = "field";
	gui.Controls.Identifier[idAction] = "action";

	gui.Controls.States[idSource].Flag = ::gpk::GUI_CONTROL_FLAG_Selected;
	::gpk::STextState sourceTextState = {};
	sourceTextState.Parent = idSource;
	sourceTextState.Flag = (::gpk::GUI_TEXT_FLAG)(::gpk::GUI_TEXT_Selected | ::gpk::GUI_TEXT_Bold | ::gpk::GUI_TEXT_Italic);
	if_fail_fe(gui.Controls.TextState[idSource].push_back(sourceTextState));
	gui.Controls.Constraints[idSource].AttachSizeToControl = {idPanel, idBridge};
	gui.Controls.Constraints[idSource].AttachSizeToText = {true, false};
	gui.Controls.Constraints[idSource].DockToControl = {-1, idAction, idTarget, -1};
	gui.Controls.Constraints[idSource].SizeMinMax = {{11, 13}, {101, 103}};
	gui.Controls.Placement[idSource].Area = {{17, 19}, {107, 109}};
	gui.Controls.Placement[idSource].Margin = {2, 3, 5, 7};
	gui.Controls.Placement[idSource].Border = {11, 13, 17, 19};
	gui.Controls.Placement[idSource].Align = ::gpk::ALIGN_BOTTOM_RIGHT;
	gui.Controls.Draw[idSource].ColorMode = ::gpk::GUI_COLOR_MODE_Flat;
	gui.Controls.Draw[idSource].FrameOut = 1;
	gui.Controls.Draw[idSource].NoBorder = 1;
	gui.Controls.Draw[idSource].UseNewPalettes = 1;
	gui.Controls.Draw[idSource].ColorTheme = 5;
	gui.Controls.Draw[idSource].Palettes[::gpk::GUI_CONTROL_PALETTE_SELECTED] = 9;
	gui.Controls.Text[idSource].Text = "Source from numeric identifier";
	gui.Controls.Text[idSource].Align = ::gpk::ALIGN_CENTER_RIGHT;
	gui.Controls.Text[idSource].Selection = {3, 11};
	gui.Controls.Text[idSource].FontSelected = 7;
	gui.Controls.Script[idSource] = "source script text";

	gui.Controls.Area[idTarget].Total.Local = {{23, 29}, {31, 37}};
	gui.Controls.Images[idTarget].ImageOffset = {41, 43};
	gui.Controls.Script[idAction] =
		"{\"panel\":{\"field\":{"
			"\"State\":\"{panel.('0').State}\","
			"\"TextState\":\"{panel.('0').TextState}\","
			"\"Constraints\":\"{panel.('0').Constraints}\","
			"\"Placement\":\"{panel.('0').Placement}\","
			"\"Draw\":\"{panel.('0').Draw}\","
			"\"Text\":\"{panel.('0').Text}\","
			"\"Script\":\"{panel.('0').Script}\""
		"}}}";
	cnst ::gpk::err_t scriptResult = ::gpk::guiScriptRun(gui, idAction);
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_GUI_SCRIPT_RUN, ::gpk::failed(scriptResult), "Result:%i.", scriptResult);

	::gpk::string guiJSON = {};
	cnst ::gpk::err_t saveResult = ::gpk::guiJSONSave(guiJSON, gui);
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_GUI_SCRIPT_SAVE, ::gpk::failed(saveResult), "Result:%i.", saveResult);
	::gpk::SJSONReader reader = {};
	cnst ::gpk::err_t parseResult = ::gpk::jsonParse(reader, guiJSON);
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_GUI_SCRIPT_PARSE, ::gpk::failed(parseResult), "Result:%i, JSON:'%.*s'.", parseResult, GPK_VIEW_ARGS(guiJSON));
	stxp ::gpk::vcst_c componentNames[] = {"State", "TextState", "Constraints", "Placement", "Draw", "Text", "Script"};
	for(::gpk::u2_t iComponent = 0; iComponent < ::gpk::size(componentNames); ++iComponent) {
		::gpk::string sourceExpression = {};
		::gpk::string targetExpression = {};
		if_fail_fe(::gpk::append_strings(sourceExpression, "panel.('0').", componentNames[iComponent]));
		if_fail_fe(::gpk::append_strings(targetExpression, "panel.field.", componentNames[iComponent]));
		::gpk::vcst_t sourceValue = {};
		::gpk::vcst_t targetValue = {};
		cnst ::gpk::err_t sourceResolve = ::gpk::jsonExpressionResolve(sourceExpression, reader, 0, sourceValue);
		cnst ::gpk::err_t targetResolve = ::gpk::jsonExpressionResolve(targetExpression, reader, 0, targetValue);
		GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_GUI_SCRIPT_RESOLVE, ::gpk::failed(sourceResolve) || ::gpk::failed(targetResolve), "Component:'%.*s', source:%i, target:%i.", GPK_VIEW_ARGS(componentNames[iComponent]), sourceResolve, targetResolve);
		GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_GUI_SCRIPT_VALUE, sourceValue != targetValue, "Component:'%.*s', source:'%.*s', target:'%.*s'.", GPK_VIEW_ARGS(componentNames[iComponent]), GPK_VIEW_ARGS(sourceValue), GPK_VIEW_ARGS(targetValue));
	}
	GPK_TEST_CHECK(errors, JSON_EXPRESSION_TEST_RESULT_GUI_SCRIPT_BOUNDARY
		, gui.Controls.Identifier[idTarget] != "field"
		|| gui.Controls.Area[idTarget].Total.Local.Offset.x != 23
		|| gui.Controls.Area[idTarget].Total.Local.Offset.y != 29
		|| gui.Controls.Area[idTarget].Total.Local.Size.x != 31
		|| gui.Controls.Area[idTarget].Total.Local.Size.y != 37
		|| gui.Controls.Images[idTarget].ImageOffset.x != 41
		|| gui.Controls.Images[idTarget].ImageOffset.y != 43
		|| gui.Controls.Children[idBridge].size() != 2
		|| gui.Controls.EventQueue.size()
		);
	if_fail_fe(::testArtifactWrite(outputFolder, "gui_control_table_script.json", gui.Controls.Script[idAction]));
	if_fail_fe(::testArtifactWrite(outputFolder, "gui_control_table.json", guiJSON));
	rtrn 0;
}

sttc ::gpk::err_t testJSONExpressionHistorical(ATestError & errors, ::gpk::vcst_t outputFolder) {
	cnst ::gpk::vcst_t document =
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
		  "\n\t    , \"property\" : { \"color\" : \"orange\", \"age\" : 16, \"race\" : \"fat\" }"
		  "\n\t    },"
		  "\n\t    { \"name\"     : \"Bob\" // this is another comment"
		  "\n\t    , \"property\" : { \"color\" : \"green\", \"age\" : 32, \"race\" : \"thin\" }"
		  "\n\t    }]"
		  "\n\t, \"properties\" : [{ \"name\" : \"age\", \"type\" : \"int\"}, { \"name\" : \"color\", \"type\" : \"string\"}, { \"name\" : \"race\", \"type\" : \"string\"} ]"
		  "\n}";
	::gpk::SJSONReader reader = {};
	cnst ::gpk::err_t parseResult = ::gpk::jsonParse(reader, document);
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_HISTORICAL_TEST_RESULT_PARSE, ::gpk::failed(parseResult), "Result:%i, document:'%.*s'.", parseResult, GPK_VIEW_ARGS(document));

	for(::gpk::u2_t iCase = 0; iCase < ::gpk::size(JSON_EXPRESSION_HISTORICAL_CASES); ++iCase) {
		cnst SJSONExpressionHistoricalCase & expressionCase = JSON_EXPRESSION_HISTORICAL_CASES[iCase];
		::gpk::vcst_t output = "unchanged";
		cnst ::gpk::err_t result = ::gpk::jsonExpressionResolve(expressionCase.Expression, reader, 0, output);
		GPK_TEST_CHECKF(errors, JSON_EXPRESSION_HISTORICAL_TEST_RESULT_RESOLVE, -1 == result, "Case:%u, expression:'%.*s', result:%i.", iCase, GPK_VIEW_ARGS(expressionCase.Expression), result);
		GPK_TEST_CHECKF(errors, JSON_EXPRESSION_HISTORICAL_TEST_RESULT_VALUE, output != expressionCase.Expected, "Case:%u, expression:'%.*s', output:'%.*s', expected:'%.*s'.", iCase, GPK_VIEW_ARGS(expressionCase.Expression), GPK_VIEW_ARGS(output), GPK_VIEW_ARGS(expressionCase.Expected));
	}

	cnst ::gpk::vcst_t format =
		  "I want to replace this (but not \\{this}): \n{parents['1'].name}'s {properties[selection.active ? selection.index : '1'].name}: // this comment should appear here"
		  "\n{parents['1'].property.(properties[selection.active ? selection.index : '1'].name // this comment should not appear here"
		  "\n)}.";
	::gpk::asc_t formatted = {};
	cnst ::gpk::err_t formatResult = ::gpk::jsonStringFormat(format, reader, 0, formatted);
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_HISTORICAL_TEST_RESULT_FORMAT_RESULT, ::gpk::failed(formatResult), "Result:%i, format:'%.*s'.", formatResult, GPK_VIEW_ARGS(format));
	cnst ::gpk::vcst_t formattedView = {formatted.begin(), formatted.size()};
	cnst ::gpk::vcst_t expected = "I want to replace this (but not \\{this}): \nBob's color: // this comment should appear here\ngreen.";
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_HISTORICAL_TEST_RESULT_FORMAT_VALUE, formattedView != expected, "Output:'%.*s', expected:'%.*s'.", GPK_VIEW_ARGS(formattedView), GPK_VIEW_ARGS(expected));
	if_fail_fe(::testArtifactWrite(outputFolder, "historical_input.json", document));
	if_fail_fe(::testArtifactWrite(outputFolder, "historical_output.txt", formattedView));
	rtrn 0;
}

::gpk::err_t testJSONExpressionCTTI(ATestError & errors, ::gpk::vcst_t outputFolder) {
	GPK_TEST_REQUIRE(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_CONTROL_REGISTRY, 0 == ::gpk::cttiStruct("SControlText").Name.size());
	GPK_TEST_REQUIRE(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_SELECTION_REGISTRY, 0 == ::gpk::cttiStruct("minmaxu1_t").Name.size());
	cnst ::gpk::SEnumDefinition * alignDefinition = ::gpk::enumDefinition("ALIGN");
	GPK_TEST_REQUIRE(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_ALIGN_REGISTRY, 0 == alignDefinition);
	GPK_TEST_REQUIRE(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_ALIGN_REGISTRY, alignDefinition->ValueSize != sizeof(::gpk::ALIGN));
	cnst ::gpk::view<::gpk::vcst_c> alignNames = alignDefinition->Names(alignDefinition->Instance);
	GPK_TEST_REQUIRE(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_ALIGN_REGISTRY, 0 == alignNames.size());
	::gpk::ALIGN registeredAlign = {};
	cnst ::gpk::err_t registeredValueResult = alignDefinition->Value(alignDefinition->Instance, "BOTTOM_RIGHT", &registeredAlign);
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_ALIGN_REGISTRY, ::gpk::failed(registeredValueResult), "Result:%i.", registeredValueResult);
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_ALIGN_REGISTRY, registeredAlign != ::gpk::ALIGN_BOTTOM_RIGHT, "Value:%u.", registeredAlign);
	::gpk::vcst_t registeredLabel = {};
	cnst ::gpk::err_t registeredLabelResult = alignDefinition->Label(alignDefinition->Instance, &registeredAlign, registeredLabel);
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_ALIGN_REGISTRY, ::gpk::failed(registeredLabelResult), "Result:%i.", registeredLabelResult);
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_ALIGN_REGISTRY, registeredLabel != "BOTTOM_RIGHT", "Label:'%.*s'.", GPK_VIEW_ARGS(registeredLabel));
	if_fail_fe(::testJSONExpressionCTTIEnum<::gpk::ALIGN>(errors, "ALIGN"));
	if_fail_fe(::testJSONExpressionCTTIEnum<::gpk::GUI_CONTROL_LIMIT>(errors, "GUI_CONTROL_LIMIT"));
	if_fail_fe(::testJSONExpressionCTTIEnum<::gpk::GUI_CONTROL_METRICS_FLAG>(errors, "GUI_CONTROL_METRICS_FLAG"));
	if_fail_fe(::testJSONExpressionCTTIPlacement(errors));
	if_fail_fe(::testJSONExpressionCTTIEnum<::gpk::GUI_COLOR_MODE>(errors, "GUI_COLOR_MODE"));
	if_fail_fe(::testJSONExpressionCTTIEnum<::gpk::GUI_CONTROL_PALETTE>(errors, "GUI_CONTROL_PALETTE"));
	if_fail_fe(::testJSONExpressionCTTIDraw(errors));
	if_fail_fe(::testJSONExpressionCTTIEnum<::gpk::CONTROL_AREA_BASE>(errors, "CONTROL_AREA_BASE"));
	if_fail_fe(::testJSONExpressionCTTIEnum<::gpk::GUI_CONTROL_AREA_OLD>(errors, "GUI_CONTROL_AREA_OLD"));
	if_fail_fe(::testJSONExpressionCTTIEnum<::gpk::UI_CONTROL_AREA>(errors, "UI_CONTROL_AREA"));
	if_fail_fe(::testJSONExpressionCTTIRectangle(errors));
	if_fail_fe(::testJSONExpressionCTTIArea(errors));
	if_fail_fe(::testJSONExpressionCTTIEnum<::gpk::GUI_CONTROL_FLAG>(errors, "GUI_CONTROL_FLAG"));
	if_fail_fe(::testJSONExpressionCTTIState(errors));
	if_fail_fe(::testJSONExpressionCTTIEnum<::gpk::GUI_TEXT_FLAG>(errors, "GUI_TEXT_FLAG"));
	if_fail_fe(::testJSONExpressionCTTITextState(errors));
	if_fail_fe(::testJSONExpressionCTTIConstraints(errors));
	if_fail_fe(::testJSONExpressionCTTITheme(errors));
	if_fail_fe(::testJSONExpressionGUIControlTable(errors, outputFolder));

	::gpk::SControlText source = {};
	source.Text			= "Source text";
	source.Align			= ::gpk::ALIGN_TOP_LEFT;
	source.Selection	= {2, 7};
	source.FontSelected	= 3;

	::gpk::SControlText target = {};
	target.Text			= "Target text";
	target.Align			= ::gpk::ALIGN_BOTTOM_RIGHT;
	target.Selection	= {4, 9};
	target.FontSelected	= 5;

	::gpk::string sourceJSON = {};
	::gpk::string targetJSON = {};
	cnst ::gpk::err_t sourceSave = ::gpk::saveCTTI(sourceJSON, source);
	cnst ::gpk::err_t targetSave = ::gpk::saveCTTI(targetJSON, target);
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_SOURCE_SAVE, ::gpk::failed(sourceSave), "Result:%i.", sourceSave);
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_TARGET_SAVE, ::gpk::failed(targetSave), "Result:%i.", targetSave);
	::gpk::SJSONReader sourceReader = {};
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_QUERY_PARSE, ::gpk::failed(::gpk::jsonParse(sourceReader, sourceJSON)), "Type:'SControlText', JSON:'%.*s'.", GPK_VIEW_ARGS(sourceJSON));
	if_fail_fe(::testJSONExpressionCTTILeaves(errors, sourceReader, "SControlText", 0, {}));

	::gpk::string inputJSON = {};
	if_fail_fe(inputJSON.push_back('{'));
	if_fail_fe(::gpk::appendOpenKey(inputJSON, "Source"));
	if_fail_fe(inputJSON.append(sourceJSON));
	if_fail_fe(::gpk::appendOpenKey(inputJSON, "Target", true));
	if_fail_fe(inputJSON.append(targetJSON));
	if_fail_fe(inputJSON.push_back('}'));

	::gpk::SJSONReader reader = {};
	cnst ::gpk::err_t inputParse = ::gpk::jsonParse(reader, inputJSON);
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_INPUT_PARSE, ::gpk::failed(inputParse), "Result:%i, input:'%.*s'.", inputParse, GPK_VIEW_ARGS(inputJSON));

	::gpk::SControlText expected = target;
	expected.Text = source.Text;
	::gpk::string expectedJSON = {};
	if_fail_fe(::gpk::saveCTTI(expectedJSON, expected));

	::gpk::asc_t formatted = {};
	if_fail_fe(formatted.push_back('{'));
	cnst ::gpk::err_t formatResult = ::gpk::jsonStringFormat
		( "\"Text\":\"{Source.Text}\",\"Align\":\"{Target.Align}\",\"Selection\":{Target.Selection},\"FontSelected\":{Target.FontSelected}"
		, reader
		, 0
		, formatted
		);
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_FORMAT_RESULT, ::gpk::failed(formatResult), "Result:%i.", formatResult);
	if_fail_fe(formatted.push_back('}'));
	cnst ::gpk::vcst_t formattedView = {formatted.begin(), formatted.size()};
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_FORMAT_VALUE, formattedView != expectedJSON, "Expected:'%.*s'. Output:'%.*s'.", GPK_VIEW_ARGS(expectedJSON), GPK_VIEW_ARGS(formattedView));

	::gpk::SControlText loaded = {};
	::gpk::vcst_t loadInput = formattedView;
	cnst ::gpk::err_t loadResult = ::gpk::loadCTTI(loadInput, loaded);
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_LOAD_RESULT, ::gpk::failed(loadResult), "Result:%i.", loadResult);
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_LOAD_VALUE
		, loaded.Text != expected.Text || loaded.Align != expected.Align || loaded.Selection != expected.Selection || loaded.FontSelected != expected.FontSelected
		, "Loaded Text:'%.*s', Align:%u, Selection:{%u,%u}, Font:%i."
		, GPK_VIEW_ARGS(loaded.Text), loaded.Align, loaded.Selection.Min, loaded.Selection.Max, loaded.FontSelected
		);

	::gpk::SControlText partial = target;
	::gpk::vcst_t partialInput = "{\"FontSelected\":1}";
	cnst ::gpk::err_t partialResult = ::gpk::loadCTTI(partialInput, partial);
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_LOAD_RESULT, ::gpk::failed(partialResult), "Result:%i.", partialResult);
	GPK_TEST_CHECK(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_LOAD_VALUE, partialInput.size());
	GPK_TEST_CHECK(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_LOAD_VALUE
		, partial.FontSelected != 1 || partial.Text != target.Text || partial.Align != target.Align || partial.Selection != target.Selection
		);

	::gpk::SGUI gui = {};
	cnst ::gpk::cid_t idSource = ::gpk::controlCreate(gui);
	cnst ::gpk::cid_t idTarget = ::gpk::controlCreate(gui);
	cnst ::gpk::cid_t idAction = ::gpk::controlCreate(gui);
	cnst ::gpk::cid_t idStructural = ::gpk::controlCreateChild(gui, idTarget);
	GPK_TEST_REQUIRE(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_CONTROL_REGISTRY, 0 > idSource || 0 > idTarget || 0 > idAction || 0 > idStructural);
	gui.Controls.Identifier[idSource] = "source";
	gui.Controls.Identifier[idTarget] = "target";
	gui.Controls.Identifier[idAction] = "action";
	gui.Controls.Text[idSource].FontSelected = 3;
	gui.Controls.Text[idTarget] = target;
	gui.Controls.Placement[idTarget].Area = {{17, 19}, {123, 45}};
	gui.Controls.Text[idStructural].FontSelected = 7;
	gui.Controls.Placement[idStructural].Area = {{31, 37}, {41, 43}};
	gui.Controls.Script[idAction] =
		"{\"target\":{"
			"\"State\":{\"Flag\":1},"
			"\"Text\":{\"FontSelected\":\"{source.Text.FontSelected}\"},"
			"\"Placement\":{\"Area\":{\"Offset\":{\"x\":99}}}"
		"}}";
	cnst ::gpk::err_t scriptResult = ::gpk::guiScriptRun(gui, idAction);
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_FORMAT_RESULT, ::gpk::failed(scriptResult), "Result:%i.", scriptResult);
	GPK_TEST_CHECK(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_LOAD_VALUE, false == gui.Controls.States[idTarget].IsHidden());
	GPK_TEST_CHECK(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_LOAD_VALUE
		, gui.Controls.Text[idTarget].FontSelected != gui.Controls.Text[idSource].FontSelected
		|| gui.Controls.Text[idTarget].Text != target.Text
		|| gui.Controls.Text[idTarget].Align != target.Align
		|| gui.Controls.Text[idTarget].Selection != target.Selection
		);
	GPK_TEST_CHECK(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_LOAD_VALUE
		, gui.Controls.Placement[idTarget].Area.Offset.x != 99
		|| gui.Controls.Placement[idTarget].Area.Offset.y != 19
		|| gui.Controls.Placement[idTarget].Area.Size.x != 123
		|| gui.Controls.Placement[idTarget].Area.Size.y != 45
		);
	GPK_TEST_CHECK(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_LOAD_VALUE
		, gui.Controls.States[idStructural].IsHidden()
		|| gui.Controls.Text[idStructural].FontSelected != 7
		|| gui.Controls.Placement[idStructural].Area.Offset.x != 31
		|| gui.Controls.Placement[idStructural].Area.Offset.y != 37
		|| gui.Controls.Placement[idStructural].Area.Size.x != 41
		|| gui.Controls.Placement[idStructural].Area.Size.y != 43
		);
	gui.Controls.Script[idAction] = "{\"target\":{\"State\":{\"Flag\":\"{target.State.Flag == 'Hidden' ? '0' : '1'}\"}}}";
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_FORMAT_RESULT, ::gpk::failed(::gpk::guiScriptRun(gui, idAction)), "%s", "Failed to show target through ternary script.");
	GPK_TEST_CHECK(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_LOAD_VALUE, gui.Controls.States[idTarget].IsHidden());
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_FORMAT_RESULT, ::gpk::failed(::gpk::guiScriptRun(gui, idAction)), "%s", "Failed to hide target through ternary script.");
	GPK_TEST_CHECK(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_LOAD_VALUE, false == gui.Controls.States[idTarget].IsHidden());
	gui.Controls.Script[idAction] = "{\"target\":{\"Text\":{\"FontSelected\":5}}}";
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_LOAD_RESULT, ::gpk::failed(::gpk::guiScriptRun(gui, idAction)), "%s", "Failed to load static script patch.");
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_CTTI_LOAD_VALUE
		, gui.Controls.Text[idTarget].FontSelected != 5
		|| gui.Controls.Text[idTarget].Text != target.Text
		|| gui.Controls.Text[idTarget].Align != target.Align
		|| gui.Controls.Text[idTarget].Selection != target.Selection
		, "FontSelected:%i, Text:'%.*s', Align:%u, Selection:{%u,%u}; Expected FontSelected:%i, Text:'%.*s', Align:%u, Selection:{%u,%u}."
		, gui.Controls.Text[idTarget].FontSelected, GPK_VIEW_ARGS(gui.Controls.Text[idTarget].Text), gui.Controls.Text[idTarget].Align, gui.Controls.Text[idTarget].Selection.Min, gui.Controls.Text[idTarget].Selection.Max
		, 5, GPK_VIEW_ARGS(target.Text), target.Align, target.Selection.Min, target.Selection.Max
		);
	if_fail_fe(::testArtifactWrite(outputFolder, "ctti_source.json", sourceJSON));
	if_fail_fe(::testArtifactWrite(outputFolder, "ctti_target.json", targetJSON));
	if_fail_fe(::testArtifactWrite(outputFolder, "ctti_expression_input.json", inputJSON));
	if_fail_fe(::testArtifactWrite(outputFolder, "ctti_expression_expected.json", expectedJSON));
	if_fail_fe(::testArtifactWrite(outputFolder, "ctti_expression_output.json", formattedView));
	rtrn 0;
}

::gpk::err_t testJSONExpression(ATestError & errors, ::gpk::vcst_t outputFolder) {
	if_fail_fe(testJSONExpressionHistorical(errors, outputFolder));
	if_fail_fe(testJSONExpressionCTTI(errors, outputFolder));
	cnst ::gpk::vcst_t document = "{\"application\":{\"blitter\":{\"database\":{\"name\":\"main\",\"enabled\":true,\"disabled\":false,\"negativeZero\":-0.0,\"path\":{\"database\":\"data\"},\"bind\":[\"id\",\"name\"]}}},\"selector\":\"selectorName\",\"selectorName\":\"name\"}";
	::gpk::SJSONReader reader = {};
	cnst ::gpk::err_t parseResult = ::gpk::jsonParse(reader, document);
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_PARSE, ::gpk::failed(parseResult), "Result:%i, document:'%.*s'.", parseResult, GPK_VIEW_ARGS(document));

	::gpk::err_t indexDatabase = -1;
	if_fail_fe(testJSONExpressionResolve(errors, reader, 0, "application.blitter.database", "{\"name\":\"main\",\"enabled\":true,\"disabled\":false,\"negativeZero\":-0.0,\"path\":{\"database\":\"data\"},\"bind\":[\"id\",\"name\"]}", JSON_EXPRESSION_TEST_RESULT_DIRECT_RESULT, JSON_EXPRESSION_TEST_RESULT_DIRECT_VALUE, indexDatabase));
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_DIRECT_INDEX, indexDatabase < 0, "Database index:%i.", indexDatabase);

	::gpk::err_t result = -1;
	if_fail_fe(testJSONExpressionResolve(errors, reader, (::gpk::u2_t)indexDatabase, "path.database", "data", JSON_EXPRESSION_TEST_RESULT_NESTED_RESULT, JSON_EXPRESSION_TEST_RESULT_NESTED_VALUE, result));
	if_fail_fe(testJSONExpressionResolve(errors, reader, 0, "application.blitter.database.bind['1']", "name", JSON_EXPRESSION_TEST_RESULT_ARRAY_RESULT, JSON_EXPRESSION_TEST_RESULT_ARRAY_VALUE, result));
	if_fail_fe(testJSONExpressionResolve(errors, reader, 0, "application.blitter.database.name['2']", "i", JSON_EXPRESSION_TEST_RESULT_STRING_RESULT, JSON_EXPRESSION_TEST_RESULT_STRING_VALUE, result));
	if_fail_fe(testJSONExpressionResolve(errors, reader, 0, "application.blitter.database.{selector}", "main", JSON_EXPRESSION_TEST_RESULT_DYNAMIC_RESULT, JSON_EXPRESSION_TEST_RESULT_DYNAMIC_VALUE, result));
	if_fail_fe(testJSONExpressionResolve(errors, reader, 0, "application.blitter.database.name == 'main'", "true", JSON_EXPRESSION_TEST_RESULT_EQUAL_RESULT, JSON_EXPRESSION_TEST_RESULT_EQUAL_VALUE, result));
	if_fail_fe(testJSONExpressionResolve(errors, reader, 0, "!application.blitter.database.disabled", "true", JSON_EXPRESSION_TEST_RESULT_NOT_RESULT, JSON_EXPRESSION_TEST_RESULT_NOT_VALUE, result));
	if_fail_fe(testJSONExpressionResolve(errors, reader, 0, "!application.blitter.database.negativeZero", "true", JSON_EXPRESSION_TEST_RESULT_DECIMAL_ZERO_RESULT, JSON_EXPRESSION_TEST_RESULT_DECIMAL_ZERO_VALUE, result));
	if_fail_fe(testJSONExpressionResolve(errors, reader, 0, "application.blitter.database.enabled && application.blitter.database.name", "true", JSON_EXPRESSION_TEST_RESULT_AND_RESULT, JSON_EXPRESSION_TEST_RESULT_AND_VALUE, result));
	if_fail_fe(testJSONExpressionResolve(errors, reader, 0, "application.blitter.database.disabled || application.blitter.database.name", "true", JSON_EXPRESSION_TEST_RESULT_OR_RESULT, JSON_EXPRESSION_TEST_RESULT_OR_VALUE, result));
	if_fail_fe(testJSONExpressionResolve(errors, reader, 0, "application.blitter.database.enabled ? 'yes' : 'no'", "yes", JSON_EXPRESSION_TEST_RESULT_TERNARY_RESULT, JSON_EXPRESSION_TEST_RESULT_TERNARY_VALUE, result));

	::gpk::asc_t formatted = {};
	::gpk::err_t formatResult;
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_FORMAT_RESULT, ::gpk::failed(formatResult = ::gpk::jsonStringFormat("db:{application.blitter.database.name};first:{application.blitter.database.bind['0']}", reader, 0, formatted)), "Result:%i.", formatResult);
	cnst ::gpk::vcst_t formattedView = {formatted.begin(), formatted.size()};
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_FORMAT_VALUE, formattedView != "db:main;first:id", "Output:'%.*s'.", GPK_VIEW_ARGS(formattedView));

	::gpk::asc_t adjacent = {};
	cnst ::gpk::err_t adjacentResult = ::gpk::jsonStringFormat("{application.blitter.database.name}{application.blitter.database.bind['1']}", reader, 0, adjacent);
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_TEST_RESULT_FORMAT_ADJACENT_RESULT, ::gpk::failed(adjacentResult), "Result:%i.", adjacentResult);
	cnst ::gpk::vcst_t adjacentView = {adjacent.begin(), adjacent.size()};
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_FORMAT_ADJACENT_VALUE, adjacentView != "mainname", "Output:'%.*s'.", GPK_VIEW_ARGS(adjacentView));

	::gpk::vcst_t preserved = "preserved";
	::gpk::setupLogCallbacks(0, 0);
	cnst ::gpk::err_t missingResult = ::gpk::jsonExpressionResolve("application.blitter.missing.name", reader, 0, preserved);
	::gpk::setupDefaultLogCallbacks();
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_MISSING_RESULT, 0 <= missingResult, "Result:%i.", missingResult);
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_MISSING_OUTPUT, preserved != "preserved", "Output:'%.*s'.", GPK_VIEW_ARGS(preserved));

	preserved = "preserved";
	::gpk::setupLogCallbacks(0, 0);
	cnst ::gpk::err_t rootRangeResult = ::gpk::jsonExpressionResolve("application", reader, reader.Tree.size(), preserved);
	::gpk::setupDefaultLogCallbacks();
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_ROOT_RANGE_RESULT, 0 <= rootRangeResult, "Result:%i, root:%u, nodes:%u.", rootRangeResult, reader.Tree.size(), reader.Tree.size());
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_ROOT_RANGE_OUTPUT, preserved != "preserved", "Output:'%.*s'.", GPK_VIEW_ARGS(preserved));

	preserved = "preserved";
	::gpk::setupLogCallbacks(0, 0);
	cnst ::gpk::err_t arrayRangeResult = ::gpk::jsonExpressionResolve("application.blitter.database.bind['9']", reader, 0, preserved);
	::gpk::setupDefaultLogCallbacks();
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_ARRAY_RANGE_RESULT, 0 <= arrayRangeResult, "Result:%i.", arrayRangeResult);
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_ARRAY_RANGE_OUTPUT, preserved != "preserved", "Output:'%.*s'.", GPK_VIEW_ARGS(preserved));

	preserved = "preserved";
	::gpk::setupLogCallbacks(0, 0);
	cnst ::gpk::err_t stringRangeResult = ::gpk::jsonExpressionResolve("application.blitter.database.name['9']", reader, 0, preserved);
	::gpk::setupDefaultLogCallbacks();
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_STRING_RANGE_RESULT, 0 <= stringRangeResult, "Result:%i.", stringRangeResult);
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_STRING_RANGE_OUTPUT, preserved != "preserved", "Output:'%.*s'.", GPK_VIEW_ARGS(preserved));

	::gpk::SExpressionReader expressionReader = {};
	if_fail_fe(::gpk::expressionReaderParse(expressionReader, "application.blitter.missing.name"));
	::gpk::SJSONExpressionSolver solver = {};
	::gpk::SEvaluationStepResult priorStep = {};
	priorStep.Output = "prior";
	if_fail_fe(solver.Results.push_back(priorStep));
	cnst ::gpk::u2_t resultCountBefore = solver.Results.size();
	preserved = "preserved";
	::gpk::setupLogCallbacks(0, 0);
	cnst ::gpk::err_t solverResult = ::gpk::jsonExpressionResolve(expressionReader, reader, 0, solver, preserved);
	::gpk::setupDefaultLogCallbacks();
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_SOLVER_RESULT, 0 <= solverResult, "Result:%i.", solverResult);
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_SOLVER_OUTPUT, preserved != "preserved", "Output:'%.*s'.", GPK_VIEW_ARGS(preserved));
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_SOLVER_STATE, solver.Results.size() != resultCountBefore, "Steps:%u, expected:%u.", solver.Results.size(), resultCountBefore);

	::gpk::asc_t failedFormat = ::gpk::vcst_t{"preserved"};
	::gpk::setupLogCallbacks(0, 0);
	cnst ::gpk::err_t failedFormatResult = ::gpk::jsonStringFormat("prefix:{application.blitter.missing.name}", reader, 0, failedFormat);
	::gpk::setupDefaultLogCallbacks();
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_FORMAT_FAILURE_RESULT, 0 <= failedFormatResult, "Result:%i.", failedFormatResult);
	cnst ::gpk::vcst_t failedFormatView = {failedFormat.begin(), failedFormat.size()};
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_TEST_RESULT_FORMAT_FAILURE_OUTPUT, failedFormatView != "preserved", "Output:'%.*s'.", GPK_VIEW_ARGS(failedFormatView));
	if_fail_fe(::testArtifactWrite(outputFolder, "expression_input.json", document));
	if_fail_fe(::testArtifactWrite(outputFolder, "expression_output.txt", formattedView));
	if_fail_fe(::testArtifactWrite(outputFolder, "expression_adjacent.txt", adjacentView));
	rtrn 0;
}

::gpk::err_t test_json_expression(STestRunnerState &, ::gpk::ATestError & errors, ::gpk::vcst_t outputFolder) {
	rtrn testJSONExpression(errors, outputFolder);
}
