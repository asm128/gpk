#define GPK_STDOUT_LOG_ENABLED
#include "gpk_json_expression.h"
#include "gpk_parse.h"

static constexpr char exampleJSON[] = R"({"name":"Ada","age":37,"active":true,"ratio":1.5,"items":[10,20]})";

static void printSection(const char* title) {
	printf("\n  %s\n  ------------------------------------------------------------\n", title);
}

static gpk::err_t demoJSONSteps(const gpk::vcsc_t json) {
	gpk::SJSONReader reader = {};
	for(reader.StateRead.IndexCurrentChar = 0; reader.StateRead.IndexCurrentChar < json.size(); ++reader.StateRead.IndexCurrentChar)
		if_fail_fe(gpk::jsonParseStep(reader, json));
	if_true_fef(reader.StateRead.NestLevel, "JSON nesting did not close.");
	printf("  jsonParseStep       : %u tokens\n", reader.Token.size());
	return 0;
}

static gpk::err_t demoJSONAccess(const gpk::SJSONReader& reader) {
	gpk::vcsc_t name = {};
	gpk::b8_t active = {};
	gpk::u2_t age = {};
	if_fail_fe(gpk::jsonObjectGetAsString(reader, 0, "name", name));
	if_fail_fe(gpk::jsonObjectGetAsBoolean(reader, 0, "active", active));
	if_fail_fe(gpk::jsonObjectGetAsInteger(reader, 0, "age", age));
	printf("  accessors           : name=%.*s  age=%u  active=%u\n", GPK_VIEW(name), age, (unsigned)active);
	gpk::err_t items = {};
	if_fail_fe(items = gpk::jsonObjectValueGet(reader, 0, "items"));
	gpk::err_t count = {};
	if_fail_fe(count = gpk::jsonArraySize(reader, items));
	printf("  array size          : %i\n", count);
	gpk::err_t second = {};
	if_fail_fe(second = gpk::jsonArrayValueGet(reader, items, 1));
	printf("  array[1]            : %.*s\n", GPK_VIEW(reader.View[second]));
	gpk::asc_t written = {};
	if_fail_fe(gpk::jsonWrite(reader.Tree[0], reader.View, written));
	printf("  jsonWrite           : %.*s\n", GPK_VIEW(written));
	return 0;
}

static gpk::err_t demoJSONNavigation(const gpk::SJSONReader& reader) {
	gpk::err_t keys = {};
	if_fail_fe(keys = gpk::jsonObjectKeyCount(reader, 0));
	gpk::avcsc_t names = {};
	if_fail_fe(gpk::jsonObjectKeyList(reader, 0, names));
	printf("  object keys         : %i (%u listed)\n", keys, names.size());
	for(gpk::u2_t i = 0; i < names.size(); ++i)
		printf("    key[%u]            : %.*s\n", i, GPK_VIEW(names[i]));
	gpk::as2_t keyIndices = {};
	if_fail_fe(gpk::jsonObjectKeyList(reader, 0, keyIndices));
	gpk::as2_t bothIndices = {};
	gpk::avcsc_t bothNames = {};
	if_fail_fe(gpk::jsonObjectKeyList(reader, 0, bothIndices, bothNames));
	printf("  key index overloads : %u, %u\n", keyIndices.size(), bothIndices.size());
	gpk::as2_t nodeIndices = {};
	gpk::avcsc_t nodeNames = {};
	if_fail_fe(gpk::jsonObjectKeyList(*reader.Tree[0], nodeIndices));
	if_fail_fe(gpk::jsonObjectKeyList(*reader.Tree[0], reader.View, nodeNames));
	gpk::as2_t nodeBothIndices = {};
	gpk::avcsc_t nodeBothNames = {};
	if_fail_fe(gpk::jsonObjectKeyList(*reader.Tree[0], reader.View, nodeBothIndices, nodeBothNames));
	printf("  node key overloads  : %u, %u, %u\n", nodeIndices.size(), nodeNames.size(), nodeBothNames.size());
	gpk::err_t nameIndex = {};
	if_fail_fe(nameIndex = gpk::jsonObjectValueGet(reader, 0, "name"));
	gpk::vcsc_t name = {}, cName = {};
	if_fail_fe(gpk::jsonObjectGetString(reader, nameIndex, name));
	if_fail_fe(gpk::jsonObjectGetCString(reader, nameIndex, cName));
	printf("  node string         : %.*s (C string: %.*s)\n", GPK_VIEW(name), GPK_VIEW(cName));
	gpk::err_t ageIndex = {}, ratioIndex = {}, activeIndex = {}, itemsIndex = {};
	if_fail_fe(ageIndex = gpk::jsonObjectValueGet(reader, 0, "age"));
	if_fail_fe(ratioIndex = gpk::jsonObjectValueGet(reader, 0, "ratio"));
	if_fail_fe(activeIndex = gpk::jsonObjectValueGet(reader, 0, "active"));
	if_fail_fe(itemsIndex = gpk::jsonObjectValueGet(reader, 0, "items"));
	if_fail_fe(gpk::jsonObjectKeyCount(*reader.Tree[0]));
	if_fail_fe(gpk::jsonArraySize(*reader.Tree[itemsIndex]));
	if_fail_fe(gpk::jsonArrayValueGet(*reader.Tree[itemsIndex], 1));
	if_fail_fe(gpk::jsonObjectValueGet(*reader.Tree[0], reader.View, "name"));
	gpk::f3_t decimal = {};
	gpk::b8_t truth = {};
	gpk::s3_t integer = {};
	if_fail_fe(gpk::jsonObjectGetDecimal(reader, ratioIndex, decimal));
	if_fail_fe(gpk::jsonObjectGetInteger(reader, ageIndex, integer));
	if_fail_fe(gpk::jsonObjectGetBoolean(reader, activeIndex, truth));
	printf("  scalar nodes        : decimal=%g  integer=%lld  boolean=%u\n", decimal, (long long)integer, (unsigned)truth);
	gpk::aobj<gpk::asc_t> parts = {};
	if_fail_fe(gpk::jsonArraySplit(*reader.Tree[itemsIndex], reader.View, 1, parts));
	printf("  jsonArraySplit      : %u parts\n", parts.size());
	for(gpk::u2_t i = 0; i < parts.size(); ++i)
		printf("    part[%u]           : %.*s\n", i, GPK_VIEW(parts[i]));
	printSection("JSON comparisons (library diagnostics follow)");
	fflush(stdout);
	if_true_fef(1 != gpk::jsonCompareNumber(*reader.Tree[ageIndex], *reader.Tree[ageIndex], reader.View), "Number comparison failed.");
	if_true_fef(1 != gpk::jsonCompareArray(*reader.Tree[itemsIndex], *reader.Tree[itemsIndex], reader.View), "Array comparison failed.");
	if_true_fef(1 != gpk::jsonCompareObject(*reader.Tree[0], *reader.Tree[0], reader.View), "Object comparison failed.");
	if_true_fef(1 != gpk::jsonCompareNumber(*reader.Tree[ageIndex], reader.View, *reader.Tree[ageIndex], reader.View), "Number comparison failed.");
	if_true_fef(1 != gpk::jsonCompareArray(*reader.Tree[itemsIndex], reader.View, *reader.Tree[itemsIndex], reader.View), "Array comparison failed.");
	if_true_fef(1 != gpk::jsonCompareObject(*reader.Tree[0], reader.View, *reader.Tree[0], reader.View), "Object comparison failed.");
	printf("  comparisons        : 6 passed\n");
	return 0;
}

static gpk::err_t demoNumericHelpers() {
	const gpk::vcst_t decimalText = "123rest";
	const gpk::vcst_t hexText = "2a!";
	gpk::u2_t decimal = {}, hex = {};
	gpk::err_t digits = {};
	if_fail_fe(digits = gpk::parseIntegerDecimal(decimalText, decimal));
	printf("  decimal             : %u from %i digits\n", decimal, digits);
	if_fail_fe(digits = gpk::parseIntegerHexadecimal(hexText, hex));
	printf("  hexadecimal         : %u from %i digits\n", hex, digits);
	gpk::u3_t binary = {};
	if_fail_fe(digits = gpk::parseArbitraryBaseInteger(2, gpk::vcst_t{"01"}, gpk::vcst_t{"101"}, &binary));
	printf("  binary              : %llu from %i digits\n", (unsigned long long)binary, digits);
	const gpk::vcst_t spaces = " \tname";
	gpk::u2_t cursor = 0;
	if_fail_fe(gpk::skipToNextCharacter(cursor, spaces));
	printf("  first nonspace      : offset=%u  space classified=%u\n", cursor, (unsigned)gpk::isSpaceCharacter(' '));
	return 0;
}

static gpk::err_t demoScalarOverloads(const gpk::SJSONReader& reader) {
	gpk::err_t age = {}, ratio = {}, active = {};
	if_fail_fe(age = gpk::jsonObjectValueGet(reader, 0, "age"));
	if_fail_fe(ratio = gpk::jsonObjectValueGet(reader, 0, "ratio"));
	if_fail_fe(active = gpk::jsonObjectValueGet(reader, 0, "active"));
	gpk::u0_t u0 = {}; gpk::u1_t u1 = {}; gpk::u2_t u2 = {}; gpk::u3_t u3 = {};
	gpk::s0_t s0 = {}; gpk::s1_t s1 = {}; gpk::s2_t s2 = {}; gpk::s3_t s3 = {};
	if_fail_fe(gpk::jsonObjectGetInteger(reader, age, u0));
	if_fail_fe(gpk::jsonObjectGetInteger(reader, age, u1));
	if_fail_fe(gpk::jsonObjectGetInteger(reader, age, u2));
	if_fail_fe(gpk::jsonObjectGetInteger(reader, age, u3));
	if_fail_fe(gpk::jsonObjectGetInteger(reader, age, s0));
	if_fail_fe(gpk::jsonObjectGetInteger(reader, age, s1));
	if_fail_fe(gpk::jsonObjectGetInteger(reader, age, s2));
	if_fail_fe(gpk::jsonObjectGetInteger(reader, age, s3));
	if_fail_fe(gpk::jsonObjectGetAsInteger(reader, 0, "age", u0));
	if_fail_fe(gpk::jsonObjectGetAsInteger(reader, 0, "age", u1));
	if_fail_fe(gpk::jsonObjectGetAsInteger(reader, 0, "age", u2));
	if_fail_fe(gpk::jsonObjectGetAsInteger(reader, 0, "age", u3));
	if_fail_fe(gpk::jsonObjectGetAsInteger(reader, 0, "age", s0));
	if_fail_fe(gpk::jsonObjectGetAsInteger(reader, 0, "age", s1));
	if_fail_fe(gpk::jsonObjectGetAsInteger(reader, 0, "age", s2));
	if_fail_fe(gpk::jsonObjectGetAsInteger(reader, 0, "age", s3));
	printf("  integer widths      : %u %u %u %llu; %i %i %i %lld\n", (unsigned)u0, (unsigned)u1, u2, (unsigned long long)u3, (int)s0, (int)s1, s2, (long long)s3);
	gpk::f2_t f2 = {}; gpk::f3_t f3 = {}; gpk::b8_t truth = {};
	if_fail_fe(gpk::jsonObjectGetDecimal(reader, ratio, f2));
	if_fail_fe(gpk::jsonObjectGetDecimal(reader, ratio, f3));
	if_fail_fe(gpk::jsonObjectGetAsDecimal(reader, 0, "ratio", f2));
	if_fail_fe(gpk::jsonObjectGetAsDecimal(reader, 0, "ratio", f3));
	printf("  decimal widths      : %g %g\n", (double)f2, (double)f3);
	if_fail_fe(gpk::jsonObjectGetIntegerAsBoolean(reader, age, truth));
	printf("  integer as Boolean  : %u\n", (unsigned)truth);
	if_fail_fe(gpk::jsonObjectGetIntegerAsDecimal(reader, age, f3));
	printf("  integer as decimal  : %g\n", (double)f3);
	if_fail_fe(gpk::jsonObjectGetDecimalAsInteger(reader, ratio, s3));
	printf("  decimal as integer  : %lld\n", (long long)s3);
	if_fail_fe(gpk::jsonObjectGetDecimalAsBoolean(reader, ratio, truth));
	printf("  decimal as Boolean  : %u\n", (unsigned)truth);
	if_fail_fe(gpk::jsonObjectGetBooleanAsInteger(reader, active, s3));
	printf("  Boolean as integer  : %lld\n", (long long)s3);
	if_fail_fe(gpk::jsonObjectGetBooleanAsDecimal(reader, active, f3));
	printf("  Boolean as decimal  : %g\n", (double)f3);
	printf("  scalar overloads    : eight integer widths, two decimal widths, six conversions\n");
	return 0;
}

static gpk::err_t demoJSONParse(gpk::SJSONReader& reader, const gpk::vcsc_t json) {
	if_fail_fe(gpk::jsonParse(reader, json));
	printf("  jsonParse           : %u tokens, %u nodes\n", reader.Token.size(), reader.Tree.size());
	if_fail_fe(demoJSONAccess(reader));
	return 0;
}

static gpk::err_t demoTreeRebuild(const gpk::vcsc_t jsonText) {
	gpk::SJSONReader reader = {};
	if_fail_fe(gpk::jsonParse(reader, jsonText, false, true));
	if_fail_fe(gpk::jsonTreeRebuild(reader.Token, reader.Tree));
	printf("  jsonTreeRebuild     : %u nodes\n", reader.Tree.size());
	if_fail_fe(reader.Reset());
	printf("  Reset               : %u tokens, %u views\n", reader.Token.size(), reader.View.size());
	return 0;
}

static gpk::err_t demoFieldMapping() {
	const gpk::SJSONFieldBinding fields[] = {{gpk::vcst_t{"name"}, gpk::JSON_TYPE_STRING}, {gpk::vcst_t{"age"}, gpk::JSON_TYPE_INTEGER}};
	const gpk::kv<gpk::vcsc_t, gpk::vcsc_t> maps[] = {{gpk::vcst_t{"age"}, gpk::vcst_t{"37"}}, {gpk::vcst_t{"name"}, gpk::vcst_t{"Ada"}}};
	gpk::as2_t fieldIndices = {}, mapIndices = {};
	if_fail_fe(gpk::jsonMapToFields(fieldIndices, fields, maps));
	if_fail_fe(gpk::jsonFieldsToMap(mapIndices, fields, maps));
	printf("  field maps          : %u forward, %u reverse\n", fieldIndices.size(), mapIndices.size());
	for(gpk::u2_t i = 0; i < fieldIndices.size(); ++i)
		printf("    map[%u] -> field[%i]\n", i, (int)fieldIndices[i]);
	for(gpk::u2_t i = 0; i < mapIndices.size(); ++i)
		printf("    field[%u] -> map[%i]\n", i, (int)mapIndices[i]);
	return 0;
}

static gpk::err_t demoExpressionSteps(const gpk::vcst_t expression) {
	gpk::SExpressionReader reader = {};
	for(reader.StateRead.IndexCurrentChar = 0; reader.StateRead.IndexCurrentChar < expression.size(); ++reader.StateRead.IndexCurrentChar)
		if_fail_fe(gpk::expressionReaderParseStep(reader, expression));
	if_true_fef(reader.StateRead.NestLevel, "Expression nesting did not close.");
	printf("  expressionReaderParseStep : %u tokens\n", reader.Token.size());
	return 0;
}

static gpk::err_t demoResolve(const gpk::SJSONReader& json, const gpk::vcst_t expression) {
	gpk::SExpressionReader reader = {};
	if_fail_fe(gpk::expressionReaderParse(reader, expression));
	gpk::SJSONExpressionSolver solver = {};
	gpk::vcsc_t result = {};
	if_true_fef(-1 == gpk::jsonExpressionResolve(reader, json, 0, solver, result), "Expression resolution failed.");
	printf("  %-38.*s => %.*s\n", GPK_VIEW(expression), GPK_VIEW(result));
	return 0;
}

static gpk::err_t demoResolveOverloads(const gpk::SJSONReader& json) {
	const gpk::vcst_t expression = "name";
	gpk::SExpressionReader reader = {};
	if_fail_fe(gpk::expressionReaderParse(reader, expression));
	gpk::vcsc_t result = {};
	if_fail_fe(gpk::jsonExpressionResolve(reader, json, 0, result));
	if_fail_fe(gpk::jsonExpressionResolve(reader, json, 0));
	if_fail_fe(gpk::jsonExpressionResolve(expression, json, 0, result));
	if_fail_fe(gpk::jsonExpressionResolve(expression, json, 0));
	gpk::SEvaluationStepResult step = {};
	if_fail_fe(step.SetBoolCarry(true, result));
	printf("  resolve overloads   : name and Boolean carry %.*s\n", GPK_VIEW(result));
	return 0;
}

static gpk::err_t demoTypes(const gpk::SJSONReader& json) {
	const gpk::SJSONToken& jsonToken = json.Token[0];
	const gpk::SJSONNode& jsonNode = *json[0];
	const gpk::SJSONReaderState& jsonState = json.StateRead;
	const gpk::JSON_TYPE jsonType = jsonToken.Type;
	const gpk::json_id_t jsonId = jsonNode.ObjectIndex;
	printf("  reader range        : %td nodes\n", json.end() - json.begin());
	gpk::SJSONFile file = {};
	gpk::TKeyValJSONFile keyedFile = {};
	(void)file; (void)keyedFile;
	printf("  JSON types          : id=%i  type=%i  cursor=%u\n", (int)jsonId, (int)jsonType, jsonState.IndexCurrentChar);
	gpk::SExpressionReader expression = {};
	if_fail_fe(gpk::expressionReaderParse(expression, gpk::vcst_t{"name"}));
	const gpk::SExpressionToken& expressionToken = expression.Token[0];
	const gpk::SExpressionNode& expressionNode = *expression.Tree[0];
	const gpk::SExpressionReaderState& expressionState = expression.StateRead;
	const gpk::EXPRESSION_READER_TYPE expressionType = expressionToken.Type;
	printf("  expression types    : type=%i  children=%u  cursor=%u\n", (int)expressionType, expressionNode.Children.size(), expressionState.IndexCurrentChar);
	gpk::SJSONExpressionSolver solver = {};
	gpk::vcsc_t result = {};
	if_true_fef(-1 == gpk::jsonExpressionResolve(expression, json, 0, solver, result), "Expression resolution failed.");
	if_true_fef(!solver.Results.size(), "No evaluation steps were recorded.");
	const gpk::SEvaluationStepResult& evaluation = solver.Results[0];
	printf("  evaluation step     : root=%u  result=%i\n", evaluation.IndexRootJSONNode, evaluation.IndexJSONResult);
	gpk::SStripLiteralState stripState = {};
	gpk::apod<gpk::SStripLiteralToken> strips = {};
	if_fail_fe(gpk::stripLiteralParse(stripState, strips, gpk::vcst_t{"Hello, {name}!"}));
	const gpk::STRIP_LITERAL_TYPE stripType = strips[0].Type;
	printf("  strip types         : type=%i  spans=%u  cursor=%u\n", (int)stripType, strips.size(), stripState.IndexCurrentChar);
	return 0;
}

static gpk::err_t demoLiteralSplit(const gpk::vcst_t format) {
	gpk::SStripLiteralState state = {};
	gpk::apod<gpk::SStripLiteralToken> types = {};
	if_fail_fe(gpk::stripLiteralParse(state, types, format));
	gpk::aobj<gpk::vcsc_t> views = {};
	if_fail_fe(gpk::stripLiteralGetViews(views, types, format));
	for(gpk::u2_t i = 0; i < views.size(); ++i)
		printf("    span[%u] type=%i  : %.*s\n", i, (int)types[i].Type, GPK_VIEW(views[i]));
	return 0;
}

static gpk::err_t demoLiteralSteps(const gpk::vcst_t format) {
	gpk::SStripLiteralState state = {};
	gpk::apod<gpk::SStripLiteralToken> spans = {};
	for(state.IndexCurrentChar = 0; state.IndexCurrentChar < format.size(); ++state.IndexCurrentChar) {
		state.CharCurrent = format[state.IndexCurrentChar];
		if_fail_fe(gpk::stripLiteralParseStep(state, spans, format));
	}
	printf("  stripLiteralParseStep : %u spans\n", spans.size());
	return 0;
}

static gpk::err_t demoFormat(const gpk::SJSONReader& json, const gpk::vcst_t format) {
	gpk::asc_t result = {};
	if_fail_fe(gpk::jsonStringFormat(format, json, 0, result));
	printf("  jsonStringFormat    : %.*s\n", GPK_VIEW(result));
	return 0;
}

static gpk::err_t demoExpressionExamples() {
	const gpk::vcst_t jsonText = R"({"name":"Ada","alive":true,"married":false,"selection":{"active":true,"index":1},"children":["Rene",null,"Jamie"],"parents":[{}, {"property":{"color":"green"}}],"properties":[{"name":"age"},{"name":"color"}]})";
	gpk::SJSONReader json = {};
	if_fail_fe(gpk::jsonParse(json, jsonText));
	if_fail_fe(demoResolve(json, "children[('2')]"));
	if_fail_fe(demoResolve(json, "'Ada'"));
	if_fail_fe(demoResolve(json, "selection.active ? 'yes' : 'no'"));
	if_fail_fe(demoResolve(json, "alive && !married"));
	if_fail_fe(demoResolve(json, "alive || married"));
	if_fail_fe(demoResolve(json, "!married"));
	if_fail_fe(demoResolve(json, "selection.index == selection.index"));
	if_fail_fe(demoResolve(json, "parents['1'].property.(properties[selection.index].name)"));
	return 0;
}

int main(int argc, char** argv) {
	const gpk::vcst_t jsonText = exampleJSON;
	printf("GPK JSON reference\n==================\n  Input: %.*s\n", GPK_VIEW(jsonText));
	printSection("JSON parsing and access");
	if_fail_fe(demoJSONSteps(jsonText));
	gpk::SJSONReader json = {};
	if_fail_fe(demoJSONParse(json, jsonText));
	printSection("Tree and object navigation");
	if_fail_fe(demoTreeRebuild(jsonText));
	if_fail_fe(demoJSONNavigation(json));
	printSection("Scalar conversions");
	if_fail_fe(demoScalarOverloads(json));
	printSection("Field mapping and numeric parsing");
	if_fail_fe(demoFieldMapping());
	if_fail_fe(demoNumericHelpers());
	printSection("Expression parsing and resolution");
	if_fail_fe(demoExpressionSteps("name"));
	if_fail_fe(demoResolve(json, "name"));
	if_fail_fe(demoResolve(json, "age"));
	if_fail_fe(demoResolve(json, "active"));
	if_fail_fe(demoResolveOverloads(json));
	printSection("Reader and token types");
	if_fail_fe(demoTypes(json));
	printSection("Literal splitting and formatting");
	if_fail_fe(demoLiteralSplit("Hello, {name}!"));
	if_fail_fe(demoLiteralSteps("Hello, {name}!"));
	if_fail_fe(demoFormat(json, "Hello, {name}!"));
	if_fail_fe(demoFormat(json, "Literal \\{name}; value {name}."));
	printSection("Expression examples");
	if_fail_fe(demoExpressionExamples());
	if(argc > 1) {
		printSection("JSON file input");
		gpk::SJSONFile file = {};
		if_fail_fe(gpk::jsonFileRead(file, gpk::vcst_t{argv[1], (gpk::u2_t)-1}));
		printf("  jsonFileRead        : %u tokens\n", file.Reader.Token.size());
	}
	printf("\n  All examples completed.\n\n");
	return 0;
}
