#include "gpk_ctti_stct.h"

#include "test_llc_suites.h"

usns gpk;

GDEFINE_ENUM_TYPE(CTTI_STCT_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, PARSE_RESULT		, 0, "The static CTTI struct declaration could not be parsed.");
GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, STRUCT_NAME		, 1, "The static CTTI struct declaration exposed an incorrect name.");
GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, MEMBER_COUNT		, 2, "The static CTTI struct declaration exposed an unexpected member count.");
GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, MODIFIERS		, 3, "The static CTTI struct declaration exposed incorrect modifiers.");
GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, TYPE_NAME		, 4, "The static CTTI struct declaration exposed an incorrect type name.");
GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, MEMBER_NAME		, 5, "The static CTTI struct declaration exposed an incorrect member name.");
GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, MEMBER_VALUE		, 6, "The static CTTI struct declaration exposed an incorrect member value.");
GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, MEMBER_FIND		, 7, "The static CTTI struct declaration could not find a member name.");
GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, BRACE_VALUE		, 8, "Brace initialization was unexpectedly accepted.");
GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, NUMERIC_MODIFIER	, 9, "A numeric modifier was unexpectedly accepted.");
GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, TYPE_FACTS		, 10, "The static CTTI struct compiler facts are incorrect.");
GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, FIELD_TYPES		, 11, "The static CTTI struct field types were not inferred.");

CTTI_STCT(SCTTIStructStaticMetadata,
	struct SCTTIStructStaticMetadata {
	inln sttc cnst float		Weight		= 234.25f;
	stxp					f3_t	Height		= 234.25;
	vltl gpk::view<gpk::sc_c>	Capacity	= "Factory::Build<Value>(7)";
	sttc b8_c				Enabled		= true;
	gpk::u2_t						Count;
	};
	);

CTTI_STCT(SCTTIStructStaticBraceValue,
	struct SCTTIStructStaticBraceValue {
		gpk::u2_t Count{0};
	};
	);

#define const2 cnst
CTTI_STCT(SCTTIStructStaticNumericModifier,
	struct SCTTIStructStaticNumericModifier {
		const2 	float			Weight		= 234.3f;
		vltl	gpk::refcount_t Capacity	= 1;
		sttc	b8_c			Enabled		= true;
				view<u2_c>		Count		= {};
	};
	);

CTTI_STCT(SCTTIStructStaticPOD,
	struct SCTTIStructStaticPOD {
		gpk::u2_t Value;
	};
	);

CTTI_STCT(SCTTIStructStaticFields,
	struct SCTTIStructStaticFields {
		gpk::u0_t First	= {};
		gpk::s2_t Second	= {};
	};
	);

nmsp ctti_registration_namespace {
	CTTI_STCT(SCTTIStructNamespaced,
		struct SCTTIStructNamespaced {
			gpk::u0_t Value = 7;
		};
		);
}

stxp auto cttiStructStaticMetadata		= ::gpk::cttiStruct<SCTTIStructStaticMetadata>();
stxp auto cttiStructStaticBraceValue	= ::gpk::cttiStruct<SCTTIStructStaticBraceValue>();
stxp auto cttiStructStaticModifier		= ::gpk::cttiStruct<SCTTIStructStaticNumericModifier>();
usng SCTTIStructStaticFieldIndices = tpnm ::gpk::ctti_index_pack_make<2>::Type;
usng SCTTIStructStaticFieldTypes = tpnm ::gpk::SCTTIFieldTypes<SCTTIStructStaticFields, SCTTIStructStaticFieldIndices>::Type;

static_assert(cttiStructStaticMetadata.Success(), "The static CTTI struct declaration must be valid at compile time.");
static_assert(::gpk::cttiSpanEquals(cttiStructStaticMetadata.SourceText, cttiStructStaticMetadata.StructName, "SCTTIStructStaticMetadata"), "The static CTTI struct name must come from the complete declaration.");
static_assert(::gpk::CTTI_PARSE_ERROR_INVALID_MEMBER_NAME == cttiStructStaticBraceValue.Error, "Brace initialization is outside the positional CTTI grammar.");
static_assert(::gpk::CTTI_PARSE_ERROR_INVALID_MODIFIER == cttiStructStaticModifier.Error, "Modifiers cannot contain numbers.");
static_assert(::gpk::SCTTIStructRegistry<SCTTIStructStaticPOD>::File.size() && ::gpk::SCTTIStructRegistry<SCTTIStructStaticPOD>::Line, "Static CTTI struct source provenance must be retained.");
static_assert(sizeof(SCTTIStructStaticPOD) == ::gpk::SCTTIStructRegistry<SCTTIStructStaticPOD>::Size, "Static CTTI struct size must come from the completed compiler type.");
static_assert(::gpk::SCTTIStructRegistry<SCTTIStructStaticPOD>::IsPOD, "The POD struct must be identified without dynamic traits.");
static_assert(::gpk::ctti_type_in_pack<::gpk::u0_t, SCTTIStructStaticFieldTypes>::Value && ::gpk::ctti_type_in_pack<::gpk::s2_t, SCTTIStructStaticFieldTypes>::Value, "Static CTTI field types must be inferred.");
static_assert(::gpk::ctti_struct<::ctti_registration_namespace::SCTTIStructNamespaced>, "Static CTTI struct registration must work from consumer namespaces.");
static_assert(::gpk::cttiStruct<::ctti_registration_namespace::SCTTIStructNamespaced>().Success(), "Namespaced struct registration must retain its declaration.");

tplt<size_t NDeclaration>
sttc ::gpk::vcst_t cttiStructStaticSpanView
	( cnst ::gpk::SCTTIParseResult<NDeclaration> & metadata
	, cnst ::gpk::SCTTISpan span
	) {
	rtrn {&metadata.SourceText[span.Offset], span.Count};
}

::gpk::err_t test_ctti_stct(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	GPK_TEST_REQUIRE(errors, CTTI_STCT_TEST_RESULT_PARSE_RESULT, false == cttiStructStaticMetadata.Success());
	GPK_TEST_CHECKF(errors, CTTI_STCT_TEST_RESULT_STRUCT_NAME, ::cttiStructStaticSpanView(cttiStructStaticMetadata, cttiStructStaticMetadata.StructName) != "SCTTIStructStaticMetadata", "Unexpected struct name:'%.*s'.", GPK_VIEW_ARGS(::cttiStructStaticSpanView(cttiStructStaticMetadata, cttiStructStaticMetadata.StructName)));
	GPK_TEST_CHECKF(errors, CTTI_STCT_TEST_RESULT_MEMBER_COUNT, cttiStructStaticMetadata.Count != 5, "Member count:%u, expected:5.", cttiStructStaticMetadata.Count);
	GPK_TEST_CHECKF(errors, CTTI_STCT_TEST_RESULT_MODIFIERS, ::cttiStructStaticSpanView(cttiStructStaticMetadata, cttiStructStaticMetadata.Members[0].Modifiers) != "inln sttc cnst", "Unexpected modifiers:'%.*s'.", GPK_VIEW_ARGS(::cttiStructStaticSpanView(cttiStructStaticMetadata, cttiStructStaticMetadata.Members[0].Modifiers)));
	GPK_TEST_CHECKF(errors, CTTI_STCT_TEST_RESULT_TYPE_NAME, ::cttiStructStaticSpanView(cttiStructStaticMetadata, cttiStructStaticMetadata.Members[1].TypeName) != "f3_t", "Unexpected type:'%.*s'.", GPK_VIEW_ARGS(::cttiStructStaticSpanView(cttiStructStaticMetadata, cttiStructStaticMetadata.Members[1].TypeName)));
	GPK_TEST_CHECKF(errors, CTTI_STCT_TEST_RESULT_MEMBER_NAME, ::cttiStructStaticSpanView(cttiStructStaticMetadata, cttiStructStaticMetadata.Members[1].Name) != "Height", "Unexpected name:'%.*s'.", GPK_VIEW_ARGS(::cttiStructStaticSpanView(cttiStructStaticMetadata, cttiStructStaticMetadata.Members[1].Name)));
	GPK_TEST_CHECKF(errors, CTTI_STCT_TEST_RESULT_MEMBER_VALUE, ::cttiStructStaticSpanView(cttiStructStaticMetadata, cttiStructStaticMetadata.Members[2].Value) != "\"Factory::Build<Value>(7)\"", "Unexpected value:'%.*s'.", GPK_VIEW_ARGS(::cttiStructStaticSpanView(cttiStructStaticMetadata, cttiStructStaticMetadata.Members[2].Value)));
	GPK_TEST_CHECK(errors, CTTI_STCT_TEST_RESULT_MEMBER_VALUE, false == cttiStructStaticMetadata.Members[4].Value.empty());
	GPK_TEST_CHECK(errors, CTTI_STCT_TEST_RESULT_MEMBER_FIND, cttiStructStaticMetadata.Find("Enabled") != 3);
	GPK_TEST_CHECK(errors, CTTI_STCT_TEST_RESULT_BRACE_VALUE, ::gpk::CTTI_PARSE_ERROR_INVALID_MEMBER_NAME != cttiStructStaticBraceValue.Error);
	GPK_TEST_CHECK(errors, CTTI_STCT_TEST_RESULT_NUMERIC_MODIFIER, ::gpk::CTTI_PARSE_ERROR_INVALID_MODIFIER != cttiStructStaticModifier.Error);
	GPK_TEST_CHECK(errors, CTTI_STCT_TEST_RESULT_TYPE_FACTS, sizeof(SCTTIStructStaticPOD) != ::gpk::SCTTIStructRegistry<SCTTIStructStaticPOD>::Size || false == ::gpk::SCTTIStructRegistry<SCTTIStructStaticPOD>::IsPOD);
	GPK_TEST_CHECK(errors, CTTI_STCT_TEST_RESULT_FIELD_TYPES, (false == ::gpk::ctti_type_in_pack<::gpk::u0_t, SCTTIStructStaticFieldTypes>::Value || false == ::gpk::ctti_type_in_pack<::gpk::s2_t, SCTTIStructStaticFieldTypes>::Value));
	rtrn 0;
}
