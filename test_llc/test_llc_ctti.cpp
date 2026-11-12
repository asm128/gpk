#include "gpk_ctti.h"
#include "gpk_json.h"

#include "test_llc_suites.h"

usns gpk;

GDEFINE_ENUM_TYPE(CTTI_JSON_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, PARSE_RESULT	, 0, "The CTTI declaration could not be parsed.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, MEMBER_COUNT	, 1, "The CTTI declaration exposed an unexpected member count.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, JSON_BUILD		, 2, "The CTTI member JSON could not be generated.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, JSON_TEXT		, 3, "The generated CTTI member JSON is incorrect.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, JSON_PARSE		, 4, "The generated CTTI member-type JSON is invalid.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, JSON_KEY_COUNT	, 5, "The generated JSON exposed an unexpected key count.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, MODIFIERS		, 6, "The CTTI declaration exposed incorrect modifiers.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, TYPE_NAME		, 7, "The CTTI declaration exposed an incorrect type name.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, MEMBER_NAME		, 8, "The CTTI declaration exposed an incorrect member name.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, MEMBER_VALUE		, 9, "The CTTI declaration exposed an incorrect member value.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, BRACE_VALUE		, 10, "Brace initialization was unexpectedly accepted.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, NUMERIC_MODIFIER	, 11, "A numeric modifier was unexpectedly accepted.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, ENUM_PARSE_RESULT	, 12, "The CTTI enum declaration could not be parsed.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, ENUM_COUNT		, 13, "The CTTI enum exposed an unexpected value count.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, ENUM_TYPE			, 14, "The CTTI enum exposed an incorrect underlying type.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, ENUM_NAME			, 15, "The CTTI enum exposed an incorrect value name.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, ENUM_INITIALIZER	, 16, "The CTTI enum exposed an incorrect initializer.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, ENUM_VALUE		, 17, "The CTTI enum exposed an incorrect compiled value.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, ENUM_FIND			, 18, "The CTTI enum registry could not find a value name.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, STRUCT_NAME		, 19, "The CTTI struct exposed an incorrect name.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, ENUM_DECLARATION_NAME, 20, "The complete CTTI enum declaration exposed an incorrect name.");
GDEFINE_ENUM_VALUED(CTTI_JSON_TEST_RESULT, ENUM_COMPILED_VALUES, 21, "The CTTI enum declaration lost its compiler-evaluated values.");

CTTI_STCT(
	SCTTIJSONTest0 {
	inline static const float		Weight		= 234.25f;
	stxp					f3_t	Height		= 234.25;
	volatile gpk::view<gpk::sc_c>	Capacity	= "Factory::Build<Value>(7)";
	static const bool				Enabled		= true;
	gpk::u2_t						Count;
	}
	);

CTTI_ENUM(SCTTIEnumTest, gpk::u1_t,
	CTTIEnumNone	= 3,
	CTTIEnumRead,
	CTTIEnumWrite	= CTTIEnumRead + 4,
	CTTIEnumAlias	= CTTIEnumWrite
	);

CTTI_ENUM(SCTTIEnumSingle, gpk::u0_t, SCTTIEnumSingleValue = 9);
CTTI_ENUM(SCTTIEnumBoundary, gpk::u0_t,
	SCTTIEnumBoundary00, SCTTIEnumBoundary01, SCTTIEnumBoundary02, SCTTIEnumBoundary03,
	SCTTIEnumBoundary04, SCTTIEnumBoundary05, SCTTIEnumBoundary06, SCTTIEnumBoundary07,
	SCTTIEnumBoundary08, SCTTIEnumBoundary09, SCTTIEnumBoundary10, SCTTIEnumBoundary11,
	SCTTIEnumBoundary12, SCTTIEnumBoundary13, SCTTIEnumBoundary14, SCTTIEnumBoundary15,
	SCTTIEnumBoundary16
	);

CTTI_STCT(
	SCTTIJSONTestBraceValue {
	gpk::u2_t Count{0};
	}
	);

#define const2 const

CTTI_STCT(
	SCTTIJSONTestNumericModifier {
	const2 	float			Weight		= 234.3f;
	vltl	gpk::refcount_t Capacity	= 1;
	sttc	b8_c			Enabled		= true;
			view<u2_c>		Count		= {};
	}
	);

stxp auto cttiBraceValue		= ::gpk::cttiStruct<SCTTIJSONTestBraceValue>();
stxp auto cttiNumericModifier	= ::gpk::cttiStruct<SCTTIJSONTestNumericModifier>();
stxp auto cttiJSONMetadata		= ::gpk::cttiStruct<SCTTIJSONTest0>();
static_assert(cttiJSONMetadata.Success(), "The CTTI JSON test declaration must be valid at compile time.");
static_assert(::gpk::cttiSpanEquals(cttiJSONMetadata.SourceText, cttiJSONMetadata.StructName, "SCTTIJSONTest0"), "The CTTI struct name must come from the complete declaration.");
static_assert(::gpk::CTTI_PARSE_ERROR_INVALID_MEMBER_NAME == cttiBraceValue.Error, "Brace initialization is outside the positional CTTI grammar.");
static_assert(::gpk::CTTI_PARSE_ERROR_INVALID_MODIFIER == cttiNumericModifier.Error, "Modifiers cannot contain numbers.");
static_assert(SCTTIEnumTest_gpk_ctti.Success(), "The CTTI enum declaration must be valid at compile time.");
static_assert(SCTTIEnumTest_gpk_ctti.ValuesCompiled, "The legacy enum macro must retain compiler-evaluated values.");
static_assert(::gpk::cttiSpanEquals(SCTTIEnumTest_gpk_ctti.SourceText, SCTTIEnumTest_gpk_ctti.EnumName, "SCTTIEnumTest"), "The CTTI enum name must come from the complete declaration.");
static_assert(2 == SCTTIEnumTest_gpk_ctti.Find("CTTIEnumWrite"), "The CTTI enum registry must find names at compile time.");
static_assert(SCTTIEnumTest_gpk_ctti.Values[1].Value == CTTIEnumRead, "Implicit enum values must use the compiled enum sequence.");
static_assert(SCTTIEnumTest_gpk_ctti.Values[2].Value == CTTIEnumWrite, "Explicit enum values must use the compiler-evaluated expression.");
static_assert(1 == SCTTIEnumSingle_gpk_ctti.Count && SCTTIEnumSingleValue == SCTTIEnumSingle_gpk_ctti.Values[0].Value, "The single-value enum route must preserve its compiled value.");
static_assert(17 == SCTTIEnumBoundary_gpk_ctti.Count && SCTTIEnumBoundary16 == SCTTIEnumBoundary_gpk_ctti.Values[16].Value, "The enum registry must not inherit the former sixteen-value implementation ceiling.");

tplt<size_t NDeclaration>
sttc ::gpk::vcst_t cttiSpanView
	( cnst ::gpk::SCTTIParseResult<NDeclaration> & metadata
	, cnst ::gpk::SCTTISpan span
	) {
	rtrn {&metadata.SourceText[span.Offset], span.Count};
}

tplt<tpnm TEnum, size_t NDeclaration>
sttc ::gpk::vcst_t cttiEnumSpanView
	( cnst ::gpk::SCTTIEnumParseResult<TEnum, NDeclaration> & metadata
	, cnst ::gpk::SCTTISpan span
	) {
	rtrn {&metadata.SourceText[span.Offset], span.Count};
}

tplt<size_t NDeclaration>
sttc ::gpk::err_t cttiMemberJSON
	( cnst ::gpk::SCTTIParseResult<NDeclaration> & metadata
	, ::gpk::string & output
	) {
	output.clear();
	if_fail_fe(output.push_back('{'));
	for(::gpk::u2_t iMember = 0; iMember < metadata.Count; ++iMember) {
		cnst ::gpk::SCTTIMember & member = metadata.Members[iMember];
		cnst ::gpk::vcst_t memberName = {&metadata.SourceText[member.Name.Offset], member.Name.Count};
		cnst ::gpk::vcst_t typeName = {&metadata.SourceText[member.TypeName.Offset], member.TypeName.Count};
		if(iMember)
			if_fail_fe(output.push_back(','));
		if_fail_fe(output.push_back('"'));
		if_fail_fe(output.append(memberName));
		if_fail_fe(output.push_back('"'));
		if_fail_fe(output.push_back(':'));
		if_fail_fe(output.push_back('{'));
		if_fail_fe(output.append(::gpk::vcst_t{"\"type\":\""}));
		if_fail_fe(output.append(typeName));
		if_fail_fe(output.append(::gpk::vcst_t{"\",\"value\":"}));
		if(member.Value.empty()) {
			if_fail_fe(output.append(::gpk::vcst_t{"null"}));
		}
		else {
			if_fail_fe(output.append(::cttiSpanView(metadata, member.Value)));
		}
		if_fail_fe(output.append(::gpk::vcst_t{",\"modifiers\":["}));
		bool firstModifier = true;
		for(::gpk::u2_t iModifier = member.Modifiers.Offset, modifierStop = member.Modifiers.Offset + member.Modifiers.Count; iModifier < modifierStop;) {
			while(iModifier < modifierStop && ::gpk::cttiSpace(metadata.SourceText[iModifier]))
				++iModifier;
			cnst ::gpk::u2_t modifierOffset = iModifier;
			while(iModifier < modifierStop && false == ::gpk::cttiSpace(metadata.SourceText[iModifier]))
				++iModifier;
			if(modifierOffset == iModifier)
				break;
			if(false == firstModifier)
				if_fail_fe(output.push_back(','));
			if_fail_fe(output.push_back('"'));
			if_fail_fe(output.append(::gpk::vcst_t{&metadata.SourceText[modifierOffset], iModifier - modifierOffset}));
			if_fail_fe(output.push_back('"'));
			firstModifier = false;
		}
		if_fail_fe(output.push_back(']'));
		if_fail_fe(output.push_back('}'));
	}
	if_fail_fe(output.push_back('}'));
	rtrn output.size();
}

::gpk::err_t test_ctti(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	GPK_TEST_REQUIRE (errors, CTTI_JSON_TEST_RESULT_PARSE_RESULT          , false == cttiJSONMetadata.Success());
	GPK_TEST_CHECKF  (errors, CTTI_JSON_TEST_RESULT_STRUCT_NAME           , ::cttiSpanView(cttiJSONMetadata, cttiJSONMetadata.StructName) != "SCTTIJSONTest0", "Unexpected struct name:'%.*s'.", GPK_VIEW_ARGS(::cttiSpanView(cttiJSONMetadata, cttiJSONMetadata.StructName)));
	GPK_TEST_CHECKF  (errors, CTTI_JSON_TEST_RESULT_MEMBER_COUNT          , cttiJSONMetadata.Count != 4, "Member count:%u, expected:4.", cttiJSONMetadata.Count);
	GPK_TEST_CHECKF  (errors, CTTI_JSON_TEST_RESULT_MODIFIERS             , ::cttiSpanView(cttiJSONMetadata, cttiJSONMetadata.Members[0].Modifiers) != "inline static const", "Unexpected modifiers:'%.*s'.", GPK_VIEW_ARGS(::cttiSpanView(cttiJSONMetadata, cttiJSONMetadata.Members[0].Modifiers)));
	GPK_TEST_CHECKF  (errors, CTTI_JSON_TEST_RESULT_TYPE_NAME             , ::cttiSpanView(cttiJSONMetadata, cttiJSONMetadata.Members[1].TypeName) != "gpk::view<gpk::sc_c>", "Unexpected type:'%.*s'.", GPK_VIEW_ARGS(::cttiSpanView(cttiJSONMetadata, cttiJSONMetadata.Members[1].TypeName)));
	GPK_TEST_CHECKF  (errors, CTTI_JSON_TEST_RESULT_MEMBER_NAME           , ::cttiSpanView(cttiJSONMetadata, cttiJSONMetadata.Members[1].Name) != "Capacity", "Unexpected name:'%.*s'.", GPK_VIEW_ARGS(::cttiSpanView(cttiJSONMetadata, cttiJSONMetadata.Members[1].Name)));
	GPK_TEST_CHECKF  (errors, CTTI_JSON_TEST_RESULT_MEMBER_VALUE          , ::cttiSpanView(cttiJSONMetadata, cttiJSONMetadata.Members[1].Value) != "\"Factory::Build<Value>(7)\"", "Unexpected value:'%.*s'.", GPK_VIEW_ARGS(::cttiSpanView(cttiJSONMetadata, cttiJSONMetadata.Members[1].Value)));
	GPK_TEST_CHECK   (errors, CTTI_JSON_TEST_RESULT_MEMBER_VALUE          , false == cttiJSONMetadata.Members[3].Value.empty());
	GPK_TEST_CHECK   (errors, CTTI_JSON_TEST_RESULT_BRACE_VALUE           , ::gpk::CTTI_PARSE_ERROR_INVALID_MEMBER_NAME != cttiBraceValue.Error);
	GPK_TEST_CHECK   (errors, CTTI_JSON_TEST_RESULT_NUMERIC_MODIFIER      , ::gpk::CTTI_PARSE_ERROR_INVALID_MODIFIER != cttiNumericModifier.Error);
	GPK_TEST_REQUIRE (errors, CTTI_JSON_TEST_RESULT_ENUM_PARSE_RESULT     , false == SCTTIEnumTest_gpk_ctti.Success());
	GPK_TEST_CHECKF  (errors, CTTI_JSON_TEST_RESULT_ENUM_DECLARATION_NAME , ::cttiEnumSpanView(SCTTIEnumTest_gpk_ctti, SCTTIEnumTest_gpk_ctti.EnumName) != "SCTTIEnumTest", "Unexpected enum declaration name:'%.*s'.", GPK_VIEW_ARGS(::cttiEnumSpanView(SCTTIEnumTest_gpk_ctti, SCTTIEnumTest_gpk_ctti.EnumName)));
	GPK_TEST_CHECK   (errors, CTTI_JSON_TEST_RESULT_ENUM_COMPILED_VALUES  , false == SCTTIEnumTest_gpk_ctti.ValuesCompiled);
	GPK_TEST_CHECKF  (errors, CTTI_JSON_TEST_RESULT_ENUM_COUNT            , SCTTIEnumTest_gpk_ctti.Count != 4, "Enum value count:%u, expected:4.", SCTTIEnumTest_gpk_ctti.Count);
	cnst ::gpk::vcst_t enumType = ::cttiEnumSpanView(SCTTIEnumTest_gpk_ctti, SCTTIEnumTest_gpk_ctti.UnderlyingType);
	GPK_TEST_CHECKF  (errors, CTTI_JSON_TEST_RESULT_ENUM_TYPE             , enumType != "gpk::u1_t", "Unexpected enum type:'%.*s'.", GPK_VIEW_ARGS(enumType));
	GPK_TEST_CHECKF  (errors, CTTI_JSON_TEST_RESULT_ENUM_NAME             , ::cttiEnumSpanView(SCTTIEnumTest_gpk_ctti, SCTTIEnumTest_gpk_ctti.Values[2].Name) != "CTTIEnumWrite", "Unexpected enum name:'%.*s'.", GPK_VIEW_ARGS(::cttiEnumSpanView(SCTTIEnumTest_gpk_ctti, SCTTIEnumTest_gpk_ctti.Values[2].Name)));
	GPK_TEST_CHECKF  (errors, CTTI_JSON_TEST_RESULT_ENUM_INITIALIZER      , ::cttiEnumSpanView(SCTTIEnumTest_gpk_ctti, SCTTIEnumTest_gpk_ctti.Values[2].Initializer) != "CTTIEnumRead + 4", "Unexpected enum initializer:'%.*s'.", GPK_VIEW_ARGS(::cttiEnumSpanView(SCTTIEnumTest_gpk_ctti, SCTTIEnumTest_gpk_ctti.Values[2].Initializer)));
	GPK_TEST_CHECK   (errors, CTTI_JSON_TEST_RESULT_ENUM_VALUE            , SCTTIEnumTest_gpk_ctti.Values[0].Value != CTTIEnumNone);
	GPK_TEST_CHECK   (errors, CTTI_JSON_TEST_RESULT_ENUM_VALUE            , SCTTIEnumTest_gpk_ctti.Values[1].Value != CTTIEnumRead);
	GPK_TEST_CHECK   (errors, CTTI_JSON_TEST_RESULT_ENUM_VALUE            , SCTTIEnumTest_gpk_ctti.Values[2].Value != CTTIEnumWrite);
	GPK_TEST_CHECK   (errors, CTTI_JSON_TEST_RESULT_ENUM_VALUE            , SCTTIEnumTest_gpk_ctti.Values[3].Value != CTTIEnumAlias);
	GPK_TEST_CHECK   (errors, CTTI_JSON_TEST_RESULT_ENUM_FIND             , SCTTIEnumTest_gpk_ctti.Find("CTTIEnumAlias") != 3);

	::gpk::string output;
	cnst ::gpk::err_t buildResult = ::cttiMemberJSON(cttiJSONMetadata, output);
	GPK_TEST_REQUIRE(errors, CTTI_JSON_TEST_RESULT_JSON_BUILD, ::gpk::failed(buildResult));
	stxp ::gpk::vcst_t expected = "{\"Weight\":{\"type\":\"float\",\"value\":234.25,\"modifiers\":[\"inline\",\"static\",\"const\"]},\"Capacity\":{\"type\":\"gpk::view<gpk::sc_c>\",\"value\":\"Factory::Build<Value>(7)\",\"modifiers\":[\"volatile\"]},\"Enabled\":{\"type\":\"bool\",\"value\":true,\"modifiers\":[\"static\",\"const\"]},\"Count\":{\"type\":\"gpk::u2_t\",\"value\":null,\"modifiers\":[]}}";
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
