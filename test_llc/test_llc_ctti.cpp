#include "gpk_ctti.h"
#include "gpk_json.h"

#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(CTTI_JSON_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, PARSE_RESULT	, 0, "The CTTI declaration could not be parsed.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, MEMBER_COUNT	, 1, "The CTTI declaration exposed an unexpected member count.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, JSON_BUILD		, 2, "The CTTI member-type JSON could not be generated.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, JSON_TEXT		, 3, "The generated CTTI member-type JSON is incorrect.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, JSON_PARSE		, 4, "The generated CTTI member-type JSON is invalid.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, JSON_KEY_COUNT	, 5, "The generated JSON exposed an unexpected key count.");

stxp auto cttiJSONMetadata = ::gpk::cttiParseStruct
	( "SCTTIJSONTest"
	, "gpk::u2_t Count, Capacity; f3_t Weight; b8_t Enabled;"
	);

static_assert(cttiJSONMetadata.Success(), "The CTTI JSON test declaration must be valid at compile time.");

tplt<size_t NStructName, size_t NMemberText>
sttc ::gpk::err_t cttiMemberTypeJSON
	( cnst ::gpk::SCTTIParseResult<NStructName, NMemberText> & metadata
	, ::gpk::string & output
	) {
	output.clear();
	if_fail_fe(output.push_back('{'));
	for(::gpk::u2_t iMember = 0; iMember < metadata.Count; ++iMember) {
		cnst ::gpk::SCTTIMember & member = metadata.Members[iMember];
		cnst ::gpk::vcst_t memberName = {&metadata.MemberText[member.Name.Offset], member.Name.Count};
		cnst ::gpk::vcst_t typeName = {&metadata.MemberText[member.TypeName.Offset], member.TypeName.Count};
		if(iMember)
			if_fail_fe(output.push_back(','));
		if_fail_fe(output.push_back('"'));
		if_fail_fe(output.append(memberName));
		if_fail_fe(output.push_back('"'));
		if_fail_fe(output.push_back(':'));
		if_fail_fe(output.push_back('"'));
		if_fail_fe(output.append(typeName));
		if_fail_fe(output.push_back('"'));
	}
	if_fail_fe(output.push_back('}'));
	rtrn output.size();
}

::gpk::err_t test_ctti(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	GPK_TEST_REQUIRE(errors, CTTI_JSON_TEST_RESULT_PARSE_RESULT, false == cttiJSONMetadata.Success());
	GPK_TEST_CHECKF(errors, CTTI_JSON_TEST_RESULT_MEMBER_COUNT, cttiJSONMetadata.Count != 4, "Member count:%u, expected:4.", cttiJSONMetadata.Count);

	::gpk::string output;
	cnst ::gpk::err_t buildResult = ::cttiMemberTypeJSON(cttiJSONMetadata, output);
	GPK_TEST_REQUIRE(errors, CTTI_JSON_TEST_RESULT_JSON_BUILD, ::gpk::failed(buildResult));
	stxp ::gpk::vcst_t expected = "{\"Count\":\"gpk::u2_t\",\"Capacity\":\"gpk::u2_t\",\"Weight\":\"f3_t\",\"Enabled\":\"b8_t\"}";
	GPK_TEST_CHECKF(errors, CTTI_JSON_TEST_RESULT_JSON_TEXT, output != expected, "Actual:'%.*s'; expected:'%.*s'.", GPK_VIEW_ARGS(output), GPK_VIEW_ARGS(expected));

	::gpk::SJSONReader reader;
	cnst ::gpk::err_t parseResult = ::gpk::jsonParse(reader, output);
	GPK_TEST_REQUIRE(errors, CTTI_JSON_TEST_RESULT_JSON_PARSE, ::gpk::failed(parseResult));
	if(::gpk::failed(parseResult))
		rtrn 0;
	cnst ::gpk::err_t keyCount = ::gpk::jsonObjectKeyCount(reader, 0);
	GPK_TEST_CHECKF(errors, CTTI_JSON_TEST_RESULT_JSON_KEY_COUNT, keyCount != 4, "Key count:%i, expected:4.", keyCount);
	rtrn 0;
}
