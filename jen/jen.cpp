#define GPK_STDOUT_LOG_ENABLED

#include "gpk_args.h"
#include "gpk_file.h"
#include "gpk_json.h"
#include "gpk_json_expression.h"

stxp const char input_json [] = R"jenjson(
[
        { "properties" : [{ "name" : "color", "type" : "string"}, { "name" : "age", "type" : "int"} ]
        , "selection"  : {"index" : 1 , "active" : true }
        , "people"     :
                [{ "name"     : "David"
                 , "property" :
                        { "color" : "red", "age" : 25 }
                },
                { "name"      : "Charles"
                 , "property" :
                        { "color" : "green", "age" : 32 }
                },
                { "name"      : "Charles"
                 , "property" :
                        { "color" : "blue", "age" : 95 }
                },
                ],
        },
]
)jenjson";
stxp const char input_expression [] = "{people['2'].name}'s {properties[selection.index].name}: {people['2'].property.(properties[selection.index].name)}";

int main(int argc, char** argv) {
	::gpk::SCommandLineArgs args;
	::gpk::SJSONFile file;
	if_fail_fe(gpk::argsParse(args, argc, argv));
	if(2 != args.Order.size()) {
		printf("usage: %s [-f] [json] [-f] [expression]\ninput_json:\n%s\ninput_expression:\n%s\n", argv[0], input_json, input_expression);
		const ::gpk::vcst_t demoJson = {input_json, sizeof(input_json) - 1};
		const ::gpk::vcst_t demoExpression = {input_expression, sizeof(input_expression) - 1};
		if_fail_fe(gpk::jsonParse(file.Reader, demoJson));
		::gpk::err_t demoRoot = {};
		if_fail_fe(demoRoot = gpk::jsonArrayValueGet(file.Reader, 0, 0));
		::gpk::SJSONReader demoReader = {};
		if_fail_fe(gpk::jsonParse(demoReader, file.Reader.View[demoRoot]));
		::gpk::asc_t demoResult = {};
		if_fail_fe(gpk::jsonStringFormat(demoExpression, demoReader, 0, demoResult));
		printf("result:\n%.*s\n", GPK_VIEW(demoResult));
		return argc == 1 ? 0 : -1;
	}
	::gpk::vcst_t input[2] = {};
	bool fromFile[2] = {};
	::gpk::u2_t iOption = 0, iPositional = 0;
	for(::gpk::u2_t iInput = 0; iInput < 2; ++iInput) {
		fromFile[iInput] = 1 == args.Order[iInput];
		if(fromFile[iInput]) {
			if_true_fef((args.Options[iOption].Key != ::gpk::vcst_t{"f", 1}) || !args.Options[iOption].Val.size(), "%s", "Expected -f followed by a filename.");
			input[iInput] = args.Options[iOption++].Val;
		}
		else
			input[iInput] = args.Positionals[iPositional++];
	}
	if(fromFile[0]) {
		if_fail_fe(gpk::jsonFileRead(file, input[0]));
	}
	else {
		if_fail_fe(gpk::jsonParse(file.Reader, input[0]));
	}
	::gpk::asc_t expressionBytes = {};
	if(fromFile[1]) {
		if_fail_fe(gpk::fileToMemory(input[1], expressionBytes));
		if_fail_fe(expressionBytes.slice(input[1], 0));
	}
	::gpk::asc_t output = {};
	if_fail_fe(gpk::jsonStringFormat(input[1], file.Reader, 0, output));
	printf("%.*s\n", GPK_VIEW(output));
	return 0;
}
