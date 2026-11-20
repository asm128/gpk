#include "gpk_ctti_enum.h"

#include "test_llc_suites.h"

usns gpk;

GDEFINE_ENUM_TYPE(CTTI_ENUM_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(CTTI_ENUM_TEST_RESULT, PARSE_RESULT		, 0, "The static CTTI enum declaration could not be parsed.");
GDEFINE_ENUM_VALUED(CTTI_ENUM_TEST_RESULT, VALUE_COUNT		, 1, "The static CTTI enum declaration exposed an unexpected value count.");
GDEFINE_ENUM_VALUED(CTTI_ENUM_TEST_RESULT, UNDERLYING_TYPE	, 2, "The static CTTI enum declaration exposed an incorrect underlying type.");
GDEFINE_ENUM_VALUED(CTTI_ENUM_TEST_RESULT, VALUE_NAME		, 3, "The static CTTI enum declaration exposed an incorrect value name.");
GDEFINE_ENUM_VALUED(CTTI_ENUM_TEST_RESULT, INITIALIZER		, 4, "The static CTTI enum declaration exposed an incorrect initializer.");
GDEFINE_ENUM_VALUED(CTTI_ENUM_TEST_RESULT, VALUE			, 5, "The static CTTI enum declaration exposed an incorrect compiled value.");
GDEFINE_ENUM_VALUED(CTTI_ENUM_TEST_RESULT, FIND			, 6, "The static CTTI enum declaration could not find a value name.");
GDEFINE_ENUM_VALUED(CTTI_ENUM_TEST_RESULT, SOURCE_ONLY		, 7, "The source-only CTTI enum value resolution is incorrect.");
GDEFINE_ENUM_VALUED(CTTI_ENUM_TEST_RESULT, VIEWS			, 8, "The static CTTI enum view projection is incorrect.");
GDEFINE_ENUM_VALUED(CTTI_ENUM_TEST_RESULT, TYPE_FACTS		, 9, "The static CTTI enum compiler facts are incorrect.");
GDEFINE_ENUM_VALUED(CTTI_ENUM_TEST_RESULT, VALUE_BOUNDARY	, 10, "The static CTTI enum declaration retained the former sixteen-value ceiling.");

CTTI_ENUM_COMPILED(SCTTIEnumStatic, gpk::u1_t,
	(SCTTIEnumStaticNone, SCTTIEnumStaticRead, SCTTIEnumStaticWrite, SCTTIEnumStaticAlias),
	SCTTIEnumStaticNone	= 3,
	SCTTIEnumStaticRead,
	SCTTIEnumStaticWrite	= SCTTIEnumStaticRead + 4,
	SCTTIEnumStaticAlias	= SCTTIEnumStaticWrite
	);

CTTI_ENUM_COMPILED(SCTTIEnumStaticSingle, gpk::u0_t, (SCTTIEnumStaticSingleValue), SCTTIEnumStaticSingleValue = 9);
CTTI_ENUM_COMPILED(SCTTIEnumStaticBoundary, gpk::u0_t,
	(SCTTIEnumStaticBoundary00, SCTTIEnumStaticBoundary01, SCTTIEnumStaticBoundary02, SCTTIEnumStaticBoundary03, SCTTIEnumStaticBoundary04, SCTTIEnumStaticBoundary05, SCTTIEnumStaticBoundary06, SCTTIEnumStaticBoundary07, SCTTIEnumStaticBoundary08, SCTTIEnumStaticBoundary09, SCTTIEnumStaticBoundary10, SCTTIEnumStaticBoundary11, SCTTIEnumStaticBoundary12, SCTTIEnumStaticBoundary13, SCTTIEnumStaticBoundary14, SCTTIEnumStaticBoundary15, SCTTIEnumStaticBoundary16),
	SCTTIEnumStaticBoundary00, SCTTIEnumStaticBoundary01, SCTTIEnumStaticBoundary02, SCTTIEnumStaticBoundary03,
	SCTTIEnumStaticBoundary04, SCTTIEnumStaticBoundary05, SCTTIEnumStaticBoundary06, SCTTIEnumStaticBoundary07,
	SCTTIEnumStaticBoundary08, SCTTIEnumStaticBoundary09, SCTTIEnumStaticBoundary10, SCTTIEnumStaticBoundary11,
	SCTTIEnumStaticBoundary12, SCTTIEnumStaticBoundary13, SCTTIEnumStaticBoundary14, SCTTIEnumStaticBoundary15,
	SCTTIEnumStaticBoundary16
	);

CTTI_ENUM(SCTTIEnumStaticFull,
	enum SCTTIEnumStaticFull : gpk::u1_t {
		SCTTIEnumStaticFull_None = 0,
		SCTTIEnumStaticFull_Read,
		SCTTIEnumStaticFull_Write
	};
	);

nmsp ctti_registration_namespace {
	CTTI_ENUM_COMPILED(SCTTIEnumNamespaced, gpk::u0_t, (SCTTIEnumNamespaced_Value), SCTTIEnumNamespaced_Value = 7);
}

static_assert(::gpk::cttiEnumTyped<SCTTIEnumStatic>().Success(), "The static CTTI enum declaration must be valid at compile time.");
static_assert(::gpk::cttiEnumTyped<SCTTIEnumStatic>().ValuesCompiled, "The compiled CTTI enum route must retain compiler-evaluated values.");
static_assert(2 == ::gpk::find(::gpk::vcst_t{"SCTTIEnumStaticWrite"}, ::gpk::cttiEnumViews<SCTTIEnumStatic>().Names), "The static CTTI enum declaration must find names at compile time.");
static_assert(::gpk::cttiEnumTyped<SCTTIEnumStatic>().Values[1].Value == SCTTIEnumStaticRead, "Implicit enum values must use the compiled enum sequence.");
static_assert(::gpk::cttiEnumTyped<SCTTIEnumStatic>().Values[2].Value == SCTTIEnumStaticWrite, "Explicit enum values must use the compiler-evaluated expression.");
static_assert(1 == ::gpk::cttiEnumTyped<SCTTIEnumStaticSingle>().Count && SCTTIEnumStaticSingleValue == ::gpk::cttiEnumTyped<SCTTIEnumStaticSingle>().Values[0].Value, "The single-value enum route must preserve its compiled value.");
static_assert(17 == ::gpk::cttiEnumTyped<SCTTIEnumStaticBoundary>().Count && SCTTIEnumStaticBoundary16 == ::gpk::cttiEnumTyped<SCTTIEnumStaticBoundary>().Values[16].Value, "Static CTTI enums must not inherit the former sixteen-value ceiling.");
static_assert(SCTTIEnumStaticFull_None == 0 && SCTTIEnumStaticFull_Read == 1 && SCTTIEnumStaticFull_Write == 2, "The wrapped enum declaration must retain its compiler values.");
static_assert(sizeof(SCTTIEnumStatic) == sizeof(gpk::u1_t) && __is_pod(SCTTIEnumStatic), "Static CTTI enum type facts must be retained.");
static_assert(::gpk::ctti_enum<::ctti_registration_namespace::SCTTIEnumNamespaced>, "Static CTTI enum registration must work from consumer namespaces.");
static_assert(::gpk::cttiEnumTyped<::ctti_registration_namespace::SCTTIEnumNamespaced>().Values[0].Value == ::ctti_registration_namespace::SCTTIEnumNamespaced_Value, "Namespaced enum registration must retain compiler values.");

::gpk::err_t test_ctti_enum(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	cnst auto & metadata = ::gpk::cttiEnumTyped<SCTTIEnumStatic>();
	cnst auto & views = ::gpk::cttiEnumViews<SCTTIEnumStatic>();
	GPK_TEST_REQUIRE(errors, CTTI_ENUM_TEST_RESULT_PARSE_RESULT, false == metadata.Success());
	GPK_TEST_CHECKF(errors, CTTI_ENUM_TEST_RESULT_VALUE_COUNT, metadata.Count != 4, "Value count:%u, expected:4.", metadata.Count);
	cnst ::gpk::vcst_t enumType = {&metadata.SourceText[metadata.UnderlyingType.Offset], metadata.UnderlyingType.Count};
	GPK_TEST_CHECKF(errors, CTTI_ENUM_TEST_RESULT_UNDERLYING_TYPE, enumType != "gpk::u1_t", "Unexpected type:'%.*s'.", GPK_VIEW_ARGS(enumType));
	cnst ::gpk::vcst_t valueName = {&metadata.SourceText[metadata.Values[2].Name.Offset], metadata.Values[2].Name.Count};
	cnst ::gpk::vcst_t initializer = {&metadata.SourceText[metadata.Values[2].Initializer.Offset], metadata.Values[2].Initializer.Count};
	GPK_TEST_CHECKF(errors, CTTI_ENUM_TEST_RESULT_VALUE_NAME, valueName != "SCTTIEnumStaticWrite", "Unexpected name:'%.*s'.", GPK_VIEW_ARGS(valueName));
	GPK_TEST_CHECKF(errors, CTTI_ENUM_TEST_RESULT_INITIALIZER, initializer != "SCTTIEnumStaticRead + 4", "Unexpected initializer:'%.*s'.", GPK_VIEW_ARGS(initializer));
	GPK_TEST_CHECK(errors, CTTI_ENUM_TEST_RESULT_VALUE, metadata.Values[0].Value != SCTTIEnumStaticNone);
	GPK_TEST_CHECK(errors, CTTI_ENUM_TEST_RESULT_VALUE, metadata.Values[1].Value != SCTTIEnumStaticRead);
	GPK_TEST_CHECK(errors, CTTI_ENUM_TEST_RESULT_VALUE, metadata.Values[2].Value != SCTTIEnumStaticWrite);
	GPK_TEST_CHECK(errors, CTTI_ENUM_TEST_RESULT_VALUE, metadata.Values[3].Value != SCTTIEnumStaticAlias);
	GPK_TEST_CHECK(errors, CTTI_ENUM_TEST_RESULT_FIND, ::gpk::find(::gpk::vcst_t{"SCTTIEnumStaticAlias"}, views.Names) != 3);

	cnst auto & sourceOnly = ::gpk::cttiEnum<SCTTIEnumStaticFull>();
	cnst auto & sourceOnlyViews = ::gpk::cttiEnumViews<SCTTIEnumStaticFull>();
	GPK_TEST_REQUIRE(errors, CTTI_ENUM_TEST_RESULT_PARSE_RESULT, false == sourceOnly.Success());
	GPK_TEST_CHECK(errors, CTTI_ENUM_TEST_RESULT_SOURCE_ONLY, sourceOnly.Count != 3 || false == sourceOnly.Values[0].ValueResolved || 0 != sourceOnly.Values[0].Value || 1 != sourceOnly.Values[1].Value || 2 != sourceOnly.Values[2].Value);
	GPK_TEST_CHECKF(errors, CTTI_ENUM_TEST_RESULT_VIEWS, sourceOnlyViews.Names[0] != "SCTTIEnumStaticFull_None", "Unexpected first view:'%.*s'.", GPK_VIEW_ARGS(sourceOnlyViews.Names[0]));
	GPK_TEST_CHECKF(errors, CTTI_ENUM_TEST_RESULT_VIEWS, sourceOnlyViews.Names[2] != "SCTTIEnumStaticFull_Write", "Unexpected last view:'%.*s'.", GPK_VIEW_ARGS(sourceOnlyViews.Names[2]));
	GPK_TEST_CHECK(errors, CTTI_ENUM_TEST_RESULT_TYPE_FACTS, sizeof(SCTTIEnumStatic) != sizeof(::gpk::u1_t) || false == __is_pod(SCTTIEnumStatic));
	GPK_TEST_CHECK(errors, CTTI_ENUM_TEST_RESULT_VALUE_BOUNDARY, ::gpk::cttiEnumTyped<SCTTIEnumStaticBoundary>().Count != 17 || ::gpk::cttiEnumTyped<SCTTIEnumStaticBoundary>().Values[16].Value != SCTTIEnumStaticBoundary16);
	rtrn 0;
}
