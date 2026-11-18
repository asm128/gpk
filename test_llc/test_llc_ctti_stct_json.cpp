#include "gpk_ctti_stct_json.h"

#include "test_llc_suites.h"

usns gpk;

GDEFINE_ENUM_TYPE(CTTI_STCT_JSON_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(CTTI_STCT_JSON_TEST_RESULT, METADATA		, 0, "The JSON extension did not use the static CTTI struct registry.");
GDEFINE_ENUM_VALUED(CTTI_STCT_JSON_TEST_RESULT, MEMBER_COUNT	, 1, "The dynamic CTTI struct declaration exposed an unexpected member count.");
GDEFINE_ENUM_VALUED(CTTI_STCT_JSON_TEST_RESULT, MEMBER_NAME	, 2, "The dynamic CTTI struct declaration exposed an incorrect member name.");
GDEFINE_ENUM_VALUED(CTTI_STCT_JSON_TEST_RESULT, MEMBER_VISIT	, 3, "The dynamic CTTI visitor could not reach a live member.");
GDEFINE_ENUM_VALUED(CTTI_STCT_JSON_TEST_RESULT, INSTANCE_SAVE	, 4, "The CTTI instance could not be saved.");
GDEFINE_ENUM_VALUED(CTTI_STCT_JSON_TEST_RESULT, INSTANCE_LOAD	, 5, "The CTTI instance could not be loaded.");
GDEFINE_ENUM_VALUED(CTTI_STCT_JSON_TEST_RESULT, INPUT_CURSOR	, 6, "The CTTI input cursor was advanced incorrectly.");
GDEFINE_ENUM_VALUED(CTTI_STCT_JSON_TEST_RESULT, JSON_BUILD		, 7, "The CTTI member JSON could not be generated.");
GDEFINE_ENUM_VALUED(CTTI_STCT_JSON_TEST_RESULT, JSON_TEXT		, 8, "The generated CTTI member JSON is incorrect.");
GDEFINE_ENUM_VALUED(CTTI_STCT_JSON_TEST_RESULT, JSON_PARSE		, 9, "The generated CTTI member JSON is invalid.");
GDEFINE_ENUM_VALUED(CTTI_STCT_JSON_TEST_RESULT, JSON_KEY_COUNT	, 10, "The generated CTTI member JSON exposed an unexpected key count.");

CTTI_ENUM(SCTTIStructJSONMode,
	enum SCTTIStructJSONMode : gpk::u1_t {
		SCTTIStructJSONModeNone		= 3,
		SCTTIStructJSONModeRead,
		SCTTIStructJSONModeWrite	= 8,
		SCTTIStructJSONModeAlias	= 8
	};
	);

CTTI_STCT(SCTTIStructJSONMetadata,
	struct SCTTIStructJSONMetadata {
	inln sttc cnst float		Weight		= 234.25f;
	stxp					f3_t	Height		= 234.25;
	vltl gpk::view<gpk::sc_c>	Capacity	= "Factory::Build<Value>(7)";
	sttc b8_c				Enabled		= true;
	gpk::u2_t						Count;
	};
	);

CTTI_STCT(SCTTIStructJSONInstance,
	struct SCTTIStructJSONInstance {
		gpk::u2_t			Count	= {};
		gpk::f3_t			Ratio	= {};
		SCTTIStructJSONMode	Mode	= SCTTIStructJSONModeNone;
		gpk::b8_t			Enabled	= {};
	};
	);

CTTI_STCT(SCTTIStructJSONNested,
	struct SCTTIStructJSONNested {
		gpk::u0_t	Code	= {};
		gpk::s2_t	Offset	= {};
	};
	);

CTTI_STCT(SCTTIStructJSONMany,
	struct SCTTIStructJSONMany {
		gpk::u0_t			Unsigned8	= {};
		gpk::s0_t			Signed8		= {};
		gpk::u1_t			Unsigned16	= {};
		gpk::s1_t			Signed16	= {};
		gpk::u2_t			Unsigned32	= {};
		gpk::s2_t			Signed32	= {};
		gpk::u3_t			Unsigned64	= {};
		gpk::f2_t			Ratio32		= {};
		gpk::f3_t			Ratio64		= {};
		SCTTIStructJSONMode	Mode		= SCTTIStructJSONModeNone;
		gpk::b8_t			Enabled		= {};
		SCTTIStructJSONNested
							Nested		= {};
	};
	);

#pragma pack(push, 1)
CTTI_STCT(SCTTIStructJSONPacked,
	struct SCTTIStructJSONPacked {
		gpk::u0_t			Unsigned8	= {};
		gpk::u3_t			Unsigned64	= {};
		gpk::u1_t			Unsigned16	= {};
		gpk::f3_t			Ratio64		= {};
		SCTTIStructJSONMode	Mode		= SCTTIStructJSONModeNone;
		gpk::b8_t			Enabled		= {};
		gpk::s2_t			Signed32	= {};
	};
	);
#pragma pack(pop)

CTTI_STCT(SCTTIStructJSONText,
	struct SCTTIStructJSONText {
		gpk::string	Text	= {};
		gpk::vcst_t	Label	= {};
	};
	);

usng SCTTIStructJSONFixedValues	= ::gpk::astatic<::gpk::u1_t, 3>;
usng SCTTIStructJSONPodValues	= ::gpk::apod<::gpk::s1_t>;
usng SCTTIStructJSONObjectValues	= ::gpk::aobj<SCTTIStructJSONNested>;
CTTI_STCT(SCTTIStructJSONArrays,
	struct SCTTIStructJSONArrays {
		SCTTIStructJSONFixedValues	Fixed	= {};
		SCTTIStructJSONPodValues		Pods	= {};
		SCTTIStructJSONObjectValues	Objects	= {};
		gpk::view<gpk::s1_t>			Window	= {};
	};
	);

CTTI_STCT(SCTTIStructJSONCArray,
	struct SCTTIStructJSONCArray {
		gpk::u1_t Values[3] = {};
	};
	);

CTTI_STCT(SCTTIStructJSONWindow,
	struct SCTTIStructJSONWindow {
		gpk::apod<gpk::u1_t>	Storage	= {};
		gpk::view<gpk::u1_t>	Window	= {};
	};
	);

CTTI_STCT(SCTTIStructJSONPOD,
	struct SCTTIStructJSONPOD {
		gpk::u2_t Value;
	};
	);

stxp auto cttiStructJSONMetadata = ::gpk::cttiStruct<SCTTIStructJSONMetadata>();

sttc ::gpk::err_t cttiStructJSONWindowSave(::gpk::string & output, cnst void * instance) {
	cnst SCTTIStructJSONWindow & input = *(cnst SCTTIStructJSONWindow*)instance;
	rtrn ::gpk::cttiValueSave(output, input.Storage);
}

sttc ::gpk::err_t cttiStructJSONWindowLoad(cnst ::gpk::SJSONReader & input, ::gpk::u2_t iNode, void * instance) {
	SCTTIStructJSONWindow & output = *(SCTTIStructJSONWindow*)instance;
	cnst ::gpk::err_t count = ::gpk::jsonArraySize(input, iNode);
	if_fail_fe(count);
	if_fail_fe(output.Storage.resize((::gpk::u2_t)count));
	output.Window = {output.Storage.begin(), output.Storage.size()};
	rtrn ::gpk::cttiValueLoad(input, iNode, output.Window);
}

::gpk::err_t test_ctti_stct_json(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	cnst auto & metadata = ::gpk::cttiStruct<SCTTIStructJSONPOD>();
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_METADATA, false == metadata.Success());
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_METADATA, metadata.View(metadata.StructName) != "SCTTIStructJSONPOD");
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_COUNT, 1 != metadata.Count);
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_NAME, metadata.View(metadata.Members[0].Name) != "Value");

	SCTTIStructJSONInstance sourceA = {7, 1.25, SCTTIStructJSONModeWrite, true};
	SCTTIStructJSONInstance sourceB = {9, -2.5, SCTTIStructJSONModeRead, false};
	::gpk::string serialized = {};
	::gpk::err_t savedA = {};
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_INSTANCE_SAVE, ::gpk::failed(savedA = ::gpk::saveCTTI(serialized, sourceA)));
	::gpk::err_t savedB = {};
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_INSTANCE_SAVE, ::gpk::failed(savedB = ::gpk::saveCTTI(serialized, sourceB)));
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_INSTANCE_SAVE, (::gpk::u2_t)(savedA + savedB) != serialized.size());

	::gpk::vcst_t serializedInput = serialized;
	SCTTIStructJSONInstance loadedA = {};
	SCTTIStructJSONInstance loadedB = {};
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_INSTANCE_LOAD, ::gpk::failed(::gpk::loadCTTI(serializedInput, loadedA)));
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_INPUT_CURSOR, serializedInput.size() != serialized.size() - (u2_t)savedA);
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_INSTANCE_LOAD, ::gpk::failed(::gpk::loadCTTI(serializedInput, loadedB)));
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_INPUT_CURSOR, serializedInput.size());
	GPK_TEST_CHECKF(errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_VISIT, loadedA.Count != sourceA.Count || loadedA.Ratio != sourceA.Ratio || loadedA.Mode != sourceA.Mode || loadedA.Enabled != sourceA.Enabled, "Loaded A:{%u,%.17g,%u,%u}; expected:{%u,%.17g,%u,%u}.", loadedA.Count, loadedA.Ratio, (u2_t)loadedA.Mode, loadedA.Enabled, sourceA.Count, sourceA.Ratio, (u2_t)sourceA.Mode, sourceA.Enabled);
	GPK_TEST_CHECKF(errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_VISIT, loadedB.Count != sourceB.Count || loadedB.Ratio != sourceB.Ratio || loadedB.Mode != sourceB.Mode || loadedB.Enabled != sourceB.Enabled, "Loaded B:{%u,%.17g,%u,%u}; expected:{%u,%.17g,%u,%u}.", loadedB.Count, loadedB.Ratio, (u2_t)loadedB.Mode, loadedB.Enabled, sourceB.Count, sourceB.Ratio, (u2_t)sourceB.Mode, sourceB.Enabled);

	SCTTIStructJSONMany sourceMany = {11, -12, 1300, -1400, 150000, -160000, 17000000, 18.25f, -19.5, SCTTIStructJSONModeAlias, true, {20, -210000}};
	cnst ::gpk::err_t lastVisit = ::gpk::cttiVisit(sourceMany, 11, [](auto & member) {
		if constexpr(requires { member.Code; member.Offset; }) {
			member.Offset = -220000;
			rtrn 0;
		}
		else
			rtrn -1;
	});
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_VISIT, ::gpk::failed(lastVisit) || sourceMany.Nested.Offset != -220000);
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_VISIT, false == ::gpk::failed(::gpk::cttiVisit(sourceMany, 12, [](auto &) { rtrn 0; })));
	::gpk::string serializedMany = {};
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_INSTANCE_SAVE, ::gpk::failed(::gpk::saveCTTI(serializedMany, sourceMany)));
	::gpk::vcst_t serializedManyInput = serializedMany;
	SCTTIStructJSONMany loadedMany = {};
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_INSTANCE_LOAD, ::gpk::failed(::gpk::loadCTTI(serializedManyInput, loadedMany)));
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_INPUT_CURSOR, serializedManyInput.size());
	GPK_TEST_CHECK
		( errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_VISIT
		, loadedMany.Unsigned8 != sourceMany.Unsigned8
		|| loadedMany.Signed8 != sourceMany.Signed8
		|| loadedMany.Unsigned16 != sourceMany.Unsigned16
		|| loadedMany.Signed16 != sourceMany.Signed16
		|| loadedMany.Unsigned32 != sourceMany.Unsigned32
		|| loadedMany.Signed32 != sourceMany.Signed32
		|| loadedMany.Unsigned64 != sourceMany.Unsigned64
		|| loadedMany.Ratio32 != sourceMany.Ratio32
		|| loadedMany.Ratio64 != sourceMany.Ratio64
		|| loadedMany.Mode != sourceMany.Mode
		|| loadedMany.Enabled != sourceMany.Enabled
		|| loadedMany.Nested.Code != sourceMany.Nested.Code
		|| loadedMany.Nested.Offset != sourceMany.Nested.Offset
		);

	SCTTIStructJSONPacked sourcePacked = {23, 24000000, 2500, -26.75, SCTTIStructJSONModeRead, true, -270000};
	::gpk::string serializedPacked = {};
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_INSTANCE_SAVE, ::gpk::failed(::gpk::saveCTTI(serializedPacked, sourcePacked)));
	::gpk::vcst_t serializedPackedInput = serializedPacked;
	SCTTIStructJSONPacked loadedPacked = {};
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_INSTANCE_LOAD, ::gpk::failed(::gpk::loadCTTI(serializedPackedInput, loadedPacked)));
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_INPUT_CURSOR, serializedPackedInput.size());
	GPK_TEST_CHECK
		( errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_VISIT
		, loadedPacked.Unsigned8 != sourcePacked.Unsigned8
		|| loadedPacked.Unsigned64 != sourcePacked.Unsigned64
		|| loadedPacked.Unsigned16 != sourcePacked.Unsigned16
		|| loadedPacked.Ratio64 != sourcePacked.Ratio64
		|| loadedPacked.Mode != sourcePacked.Mode
		|| loadedPacked.Enabled != sourcePacked.Enabled
		|| loadedPacked.Signed32 != sourcePacked.Signed32
		);

	SCTTIStructJSONText sourceText = {};
	sourceText.Text = ::gpk::string{"quoted \"text\" \\ path\nnext"};
	sourceText.Label = ::gpk::vcst_t{"Static GUI label"};
	::gpk::string serializedText = {};
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_INSTANCE_SAVE, ::gpk::failed(::gpk::saveCTTI(serializedText, sourceText)));
	::gpk::vcst_t serializedTextInput = serializedText;
	SCTTIStructJSONText loadedText = {};
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_INSTANCE_LOAD, ::gpk::failed(::gpk::loadCTTI(serializedTextInput, loadedText)));
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_INPUT_CURSOR, serializedTextInput.size());
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_VISIT, loadedText.Text != sourceText.Text);
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_VISIT, loadedText.Label != sourceText.Label);
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_VISIT, loadedText.Label.begin() < serializedText.begin() || loadedText.Label.end() > serializedText.end());

	SCTTIStructJSONArrays sourceArrays = {};
	sourceArrays.Fixed[0] = 31;
	sourceArrays.Fixed[1] = 32;
	sourceArrays.Fixed[2] = 33;
	sourceArrays.Pods = ::gpk::apod<::gpk::s1_t>{-41, 42, -43, 44};
	if_fail_fe(sourceArrays.Objects.resize(2));
	sourceArrays.Objects[0] = {51, -520000};
	sourceArrays.Objects[1] = {53, 540000};
	::gpk::s1_t sourceWindow[] = {-61, 62};
	sourceArrays.Window = {sourceWindow};
	::gpk::string serializedArrays = {};
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_INSTANCE_SAVE, ::gpk::failed(::gpk::saveCTTI(serializedArrays, sourceArrays)));
	::gpk::vcst_t serializedArraysInput = serializedArrays;
	::gpk::s1_t loadedWindow[2] = {};
	SCTTIStructJSONArrays loadedArrays = {};
	loadedArrays.Window = {loadedWindow};
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_INSTANCE_LOAD, ::gpk::failed(::gpk::loadCTTI(serializedArraysInput, loadedArrays)));
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_INPUT_CURSOR, serializedArraysInput.size());
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_VISIT, loadedArrays.Fixed != sourceArrays.Fixed);
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_VISIT, loadedArrays.Pods != sourceArrays.Pods);
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_VISIT, loadedArrays.Objects.size() != sourceArrays.Objects.size());
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_VISIT, loadedArrays.Objects[0].Code != sourceArrays.Objects[0].Code || loadedArrays.Objects[0].Offset != sourceArrays.Objects[0].Offset);
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_VISIT, loadedArrays.Objects[1].Code != sourceArrays.Objects[1].Code || loadedArrays.Objects[1].Offset != sourceArrays.Objects[1].Offset);
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_VISIT, loadedWindow[0] != sourceWindow[0] || loadedWindow[1] != sourceWindow[1]);

	SCTTIStructJSONCArray sourceCArray = {{71, 72, 73}};
	::gpk::string serializedCArray = {};
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_INSTANCE_SAVE, ::gpk::failed(::gpk::saveCTTI(serializedCArray, sourceCArray)));
	::gpk::vcst_t serializedCArrayInput = serializedCArray;
	SCTTIStructJSONCArray loadedCArray = {};
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_INSTANCE_LOAD, ::gpk::failed(::gpk::loadCTTI(serializedCArrayInput, loadedCArray)));
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_INPUT_CURSOR, serializedCArrayInput.size());
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_VISIT, loadedCArray.Values[0] != sourceCArray.Values[0] || loadedCArray.Values[1] != sourceCArray.Values[1] || loadedCArray.Values[2] != sourceCArray.Values[2]);

	SCTTIStructJSONWindow sourceWindowCallback = {};
	sourceWindowCallback.Storage = ::gpk::apod<::gpk::u1_t>{81, 82, 83, 84};
	::gpk::u1_t sourceWindowOverride[] = {91, 92};
	sourceWindowCallback.Window = {sourceWindowOverride};
	cnst ::gpk::SCTTIFieldJSONOverride windowOverrides[] = {{"Window", ::cttiStructJSONWindowSave, ::cttiStructJSONWindowLoad}};
	::gpk::string serializedWindowCallback = {};
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_INSTANCE_SAVE, ::gpk::failed(::gpk::saveCTTI(serializedWindowCallback, sourceWindowCallback, {windowOverrides})));
	GPK_TEST_CHECKF(errors, CTTI_STCT_JSON_TEST_RESULT_INSTANCE_SAVE, serializedWindowCallback != "{\"Storage\":[81,82,83,84],\"Window\":[81,82,83,84]}", "Serialized:'%.*s'.", GPK_VIEW_ARGS(serializedWindowCallback));
	::gpk::SJSONReader windowReader = {};
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_JSON_PARSE, ::gpk::failed(::gpk::jsonParse(windowReader, serializedWindowCallback)));
	SCTTIStructJSONWindow loadedWindowCallback = {};
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_INSTANCE_LOAD, ::gpk::failed(::gpk::loadCTTI(windowReader, loadedWindowCallback, {windowOverrides})));
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_VISIT, loadedWindowCallback.Window.begin() != loadedWindowCallback.Storage.begin() || loadedWindowCallback.Window.size() != loadedWindowCallback.Storage.size());
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_VISIT, loadedWindowCallback.Storage != sourceWindowCallback.Storage);

	::gpk::vcst_t partialInput = "{\"Count\":1}";
	SCTTIStructJSONInstance partialOutput = sourceA;
	cnst ::gpk::err_t partialResult = ::gpk::loadCTTI(partialInput, partialOutput);
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_INSTANCE_LOAD, ::gpk::failed(partialResult));
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_INPUT_CURSOR, partialInput.size());
	GPK_TEST_CHECK(errors, CTTI_STCT_JSON_TEST_RESULT_MEMBER_VISIT
		, partialOutput.Count != 1
		|| partialOutput.Ratio != sourceA.Ratio
		|| partialOutput.Mode != sourceA.Mode
		|| partialOutput.Enabled != sourceA.Enabled
		);

	::gpk::string output = {};
	cnst ::gpk::err_t buildResult = ::gpk::cttiMemberJSON(cttiStructJSONMetadata, output);
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_JSON_BUILD, ::gpk::failed(buildResult));
	stxp ::gpk::vcst_t expected = "{\"Weight\":{\"type\":\"float\",\"value\":234.25,\"modifiers\":[\"inln\",\"sttc\",\"cnst\"]},\"Height\":{\"type\":\"f3_t\",\"value\":234.25,\"modifiers\":[\"stxp\"]},\"Capacity\":{\"type\":\"gpk::view<gpk::sc_c>\",\"value\":\"Factory::Build<Value>(7)\",\"modifiers\":[\"vltl\"]},\"Enabled\":{\"type\":\"b8_c\",\"value\":true,\"modifiers\":[\"sttc\"]},\"Count\":{\"type\":\"gpk::u2_t\",\"value\":null,\"modifiers\":[]}}";
	GPK_TEST_CHECKF(errors, CTTI_STCT_JSON_TEST_RESULT_JSON_TEXT, output != expected, "Actual:'%.*s'; expected:'%.*s'.", GPK_VIEW_ARGS(output), GPK_VIEW_ARGS(expected));
	::gpk::string registeredOutput = {};
	if_fail_fe(::gpk::cttiMemberJSON<SCTTIStructJSONMetadata>(registeredOutput));
	GPK_TEST_CHECKF(errors, CTTI_STCT_JSON_TEST_RESULT_JSON_TEXT, registeredOutput != expected, "Registered:'%.*s'; expected:'%.*s'.", GPK_VIEW_ARGS(registeredOutput), GPK_VIEW_ARGS(expected));

	::gpk::SJSONReader reader = {};
	cnst ::gpk::err_t parseResult = ::gpk::jsonParse(reader, output);
	GPK_TEST_REQUIRE(errors, CTTI_STCT_JSON_TEST_RESULT_JSON_PARSE, ::gpk::failed(parseResult));
	if(::gpk::failed(parseResult))
		rtrn 0;
	cnst ::gpk::err_t keyCount = ::gpk::jsonObjectKeyCount(reader, 0);
	GPK_TEST_CHECKF(errors, CTTI_STCT_JSON_TEST_RESULT_JSON_KEY_COUNT, keyCount != 5, "Key count:%i, expected:5.", keyCount);
	rtrn 0;
}
