#include "gpk_ctti_enum_json.h"

#include "test_llc_suites.h"

usns gpk;

GDEFINE_ENUM_TYPE(CTTI_ENUM_JSON_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(CTTI_ENUM_JSON_TEST_RESULT, SAVE		, 0, "The static CTTI enum could not be saved to JSON.");
GDEFINE_ENUM_VALUED(CTTI_ENUM_JSON_TEST_RESULT, TEXT		, 1, "The static CTTI enum produced incorrect JSON text.");
GDEFINE_ENUM_VALUED(CTTI_ENUM_JSON_TEST_RESULT, PARSE		, 2, "The CTTI enum JSON input could not be parsed.");
GDEFINE_ENUM_VALUED(CTTI_ENUM_JSON_TEST_RESULT, LOAD		, 3, "The CTTI enum could not be loaded from JSON.");
GDEFINE_ENUM_VALUED(CTTI_ENUM_JSON_TEST_RESULT, VALUE		, 4, "The CTTI enum JSON value is incorrect.");
GDEFINE_ENUM_VALUED(CTTI_ENUM_JSON_TEST_RESULT, REGISTRY	, 5, "The JSON extension did not use the static CTTI enum registry.");

CTTI_ENUM(SCTTIEnumJSON,
	enum SCTTIEnumJSON : gpk::u1_t {
		SCTTIEnumJSON_None	= 3,
		SCTTIEnumJSON_Read,
		SCTTIEnumJSON_Write	= 8
	};
	);

::gpk::err_t test_ctti_enum_json(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	cnst auto & metadata = ::gpk::cttiEnumTyped<SCTTIEnumJSON>();
	GPK_TEST_REQUIRE(errors, CTTI_ENUM_JSON_TEST_RESULT_REGISTRY, false == metadata.Success());
	GPK_TEST_CHECK(errors, CTTI_ENUM_JSON_TEST_RESULT_REGISTRY, metadata.Count != 3 || metadata.Values[2].Value != SCTTIEnumJSON_Write);

	::gpk::string output = {};
	GPK_TEST_REQUIRE(errors, CTTI_ENUM_JSON_TEST_RESULT_SAVE, ::gpk::failed(::gpk::cttiValueSave(output, SCTTIEnumJSON_Write)));
	GPK_TEST_CHECKF(errors, CTTI_ENUM_JSON_TEST_RESULT_TEXT, output != "\"Write\"", "Actual:'%.*s'; expected:'\"Write\"'.", GPK_VIEW_ARGS(output));

	::gpk::SJSONReader reader = {};
	GPK_TEST_REQUIRE(errors, CTTI_ENUM_JSON_TEST_RESULT_PARSE, ::gpk::failed(::gpk::jsonParse(reader, ::gpk::vcst_t{"\"Read\""})));
	SCTTIEnumJSON value = SCTTIEnumJSON_None;
	GPK_TEST_REQUIRE(errors, CTTI_ENUM_JSON_TEST_RESULT_LOAD, ::gpk::failed(::gpk::cttiValueLoad(reader, 0, value)));
	GPK_TEST_CHECK(errors, CTTI_ENUM_JSON_TEST_RESULT_VALUE, value != SCTTIEnumJSON_Read);

	reader = {};
	GPK_TEST_REQUIRE(errors, CTTI_ENUM_JSON_TEST_RESULT_PARSE, ::gpk::failed(::gpk::jsonParse(reader, ::gpk::vcst_t{"\"SCTTIEnumJSON_Write\""})));
	value = SCTTIEnumJSON_None;
	GPK_TEST_REQUIRE(errors, CTTI_ENUM_JSON_TEST_RESULT_LOAD, ::gpk::failed(::gpk::cttiValueLoad(reader, 0, value)));
	GPK_TEST_CHECK(errors, CTTI_ENUM_JSON_TEST_RESULT_VALUE, value != SCTTIEnumJSON_Write);

	reader = {};
	GPK_TEST_REQUIRE(errors, CTTI_ENUM_JSON_TEST_RESULT_PARSE, ::gpk::failed(::gpk::jsonParse(reader, ::gpk::vcst_t{"19"})));
	GPK_TEST_REQUIRE(errors, CTTI_ENUM_JSON_TEST_RESULT_LOAD, ::gpk::failed(::gpk::cttiValueLoad(reader, 0, value)));
	GPK_TEST_CHECK(errors, CTTI_ENUM_JSON_TEST_RESULT_VALUE, value != (SCTTIEnumJSON)19);
	rtrn 0;
}
