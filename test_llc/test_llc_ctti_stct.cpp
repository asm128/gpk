//#include "gpk_ctti_stct.h"
//
//#include "test_llc_suites.h"
//
//usns gpk;
//
//GDEFINE_ENUM_TYPE(CTTI_STCT_TEST_RESULT, ::gpk::u0_t);
//GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, PARSE_RESULT		, 0, "The static CTTI struct declaration could not be parsed.");
//GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, STRUCT_NAME		, 1, "The static CTTI struct declaration exposed an incorrect name.");
//GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, MEMBER_COUNT		, 2, "The static CTTI struct declaration exposed an unexpected member count.");
//GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, MODIFIERS		, 3, "The static CTTI struct declaration exposed incorrect modifiers.");
//GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, TYPE_NAME		, 4, "The static CTTI struct declaration exposed an incorrect type name.");
//GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, MEMBER_NAME		, 5, "The static CTTI struct declaration exposed an incorrect member name.");
//GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, MEMBER_VALUE		, 6, "The static CTTI struct declaration exposed an incorrect member value.");
//GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, MEMBER_FIND		, 7, "The static CTTI struct declaration could not find a member name.");
//GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, BRACE_VALUE		, 8, "Brace initialization was unexpectedly accepted.");
//GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, ALIAS_MODIFIER	, 9, "A valid macro alias modifier was rejected.");
//GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, TYPE_FACTS		, 10, "The static CTTI struct compiler facts are incorrect.");
//GDEFINE_ENUM_VALUED(CTTI_STCT_TEST_RESULT, FIELD_TYPES		, 11, "The static CTTI struct field types were not inferred.");
//
//CTTI_STCT(SCTTIStructStaticMetadata,
//	struct SCTTIStructStaticMetadata {
//	inln sttc cnst float		Weight		= 234.25f;
//	stxp					f3_t	Height		= 234.25;
//	vltl gpk::view<gpk::sc_c>	Capacity	= "Factory::Build<Value>(7)";
//	sttc b8_c				Enabled		= true;
//	gpk::u2_t						Count;
//	};
//	);
//
//CTTI_STCT(SCTTIStructStaticBraceValue,
//	struct SCTTIStructStaticBraceValue {
//		gpk::u2_t Count{0};
//	};
//	);
//
//#define const2 cnst
//CTTI_STCT(SCTTIStructStaticNumericModifier,
//	struct SCTTIStructStaticNumericModifier {
//		const2 	float			Weight		= 234.3f;
//		vltl	gpk::refcount_t Capacity	= 1;
//		sttc	b8_c			Enabled		= true;
//				view<u2_c>		Count		= {};
//	};
//	);
//
//CTTI_STCT(SCTTIStructStaticPOD,
//	struct SCTTIStructStaticPOD {
//		gpk::u2_t Value;
//	};
//	);
//
//CTTI_STCT(SCTTIStructStaticFields,
//	struct SCTTIStructStaticFields {
//		gpk::u0_t First	= {};
//		gpk::s2_t Second	= {};
//		gpk::u1_t Fixed[3]	= {};
//	};
//	);
//
//tplt<tpnm T, ::gpk::u2_t N>
//stct SCTTIStructStaticArray {
//	T Storage[N] = {};
//};
//
//CTTI_STCT(SCTTIStructStaticNestedTemplate,
//	struct SCTTIStructStaticNestedTemplate {
//		SCTTIStructStaticArray<SCTTIStructStaticArray<gpk::u1_t, 2>, 3>
//			Values = {};
//	};
//	);
//
//nmsp ctti_registration_namespace {
//	CTTI_STCT(SCTTIStructNamespaced,
//		struct SCTTIStructNamespaced {
//			gpk::u0_t Value = 7;
//		};
//		);
//}
//
//stxp auto cttiStructStaticMetadata		= ::gpk::cttiStruct<SCTTIStructStaticMetadata>();
//stxp auto cttiStructStaticBraceValue	= ::gpk::cttiStruct<SCTTIStructStaticBraceValue>();
//stxp auto cttiStructStaticModifier		= ::gpk::cttiStruct<SCTTIStructStaticNumericModifier>();
//usng SCTTIStructStaticFieldIndices = tpnm ::gpk::ctti_index_pack_make<3>::Type;
//usng SCTTIStructStaticFieldTypes = tpnm ::gpk::SCTTIFieldTypes<SCTTIStructStaticFields, SCTTIStructStaticFieldIndices>::Type;
//stxp auto cttiStructStaticNestedTemplate = ::gpk::cttiStruct<SCTTIStructStaticNestedTemplate>();
//
//static_assert(cttiStructStaticMetadata.Success(), "The static CTTI struct declaration must be valid at compile time.");
//static_assert(::gpk::vcst_t{&cttiStructStaticMetadata.SourceText[cttiStructStaticMetadata.StructName.Offset], cttiStructStaticMetadata.StructName.Count} == "SCTTIStructStaticMetadata", "The static CTTI struct name must come from the complete declaration.");
//static_assert(false == cttiStructStaticBraceValue.Success(), "Brace initialization is outside the positional CTTI grammar.");
//static_assert(cttiStructStaticModifier.Success(), "A compiler-valid macro alias must remain valid CTTI input.");
//static_assert(::gpk::SCTTIStructRegistry<SCTTIStructStaticPOD>::File.size() && ::gpk::SCTTIStructRegistry<SCTTIStructStaticPOD>::Line, "Static CTTI struct source provenance must be retained.");
//static_assert(sizeof(SCTTIStructStaticPOD) == ::gpk::SCTTIStructRegistry<SCTTIStructStaticPOD>::Size, "Static CTTI struct size must come from the completed compiler type.");
//static_assert(::gpk::SCTTIStructRegistry<SCTTIStructStaticPOD>::IsPOD, "The POD struct must be identified without dynamic traits.");
//static_assert(::gpk::ctti_type_in_pack<::gpk::u0_t, SCTTIStructStaticFieldTypes>::Value && ::gpk::ctti_type_in_pack<::gpk::s2_t, SCTTIStructStaticFieldTypes>::Value && ::gpk::ctti_type_in_pack<::gpk::u1_t[3], SCTTIStructStaticFieldTypes>::Value, "Static CTTI field types must include fixed arrays.");
//static_assert(cttiStructStaticNestedTemplate.Success(), "Nested template types containing whitespace must remain one CTTI type token.");
//static_assert(::gpk::vcst_t{&cttiStructStaticNestedTemplate.SourceText[cttiStructStaticNestedTemplate.Members[0].TypeName.Offset], cttiStructStaticNestedTemplate.Members[0].TypeName.Count} == "SCTTIStructStaticArray<SCTTIStructStaticArray<gpk::u1_t, 2>, 3>", "Nested template type text must be preserved.");
//static_assert(::gpk::ctti_struct<::ctti_registration_namespace::SCTTIStructNamespaced>, "Static CTTI struct registration must work from consumer namespaces.");
//static_assert(::gpk::cttiStruct<::ctti_registration_namespace::SCTTIStructNamespaced>().Success(), "Namespaced struct registration must retain its declaration.");
//
//::gpk::err_t test_ctti_stct(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
//	GPK_TEST_REQUIRE(errors, CTTI_STCT_TEST_RESULT_PARSE_RESULT, false == cttiStructStaticMetadata.Success());
//	cnst ::gpk::vcst_t structName = {&cttiStructStaticMetadata.SourceText[cttiStructStaticMetadata.StructName.Offset], cttiStructStaticMetadata.StructName.Count};
//	GPK_TEST_CHECKF(errors, CTTI_STCT_TEST_RESULT_STRUCT_NAME, structName != "SCTTIStructStaticMetadata", "Unexpected struct name:'%.*s'.", GPK_VIEW_ARGS(structName));
//	GPK_TEST_CHECKF(errors, CTTI_STCT_TEST_RESULT_MEMBER_COUNT, cttiStructStaticMetadata.Count != 5, "Member count:%u, expected:5.", cttiStructStaticMetadata.Count);
//	cnst ::gpk::vcst_t modifiers = {&cttiStructStaticMetadata.SourceText[cttiStructStaticMetadata.Members[0].Modifiers.Offset], cttiStructStaticMetadata.Members[0].Modifiers.Count};
//	cnst ::gpk::vcst_t typeName = {&cttiStructStaticMetadata.SourceText[cttiStructStaticMetadata.Members[1].TypeName.Offset], cttiStructStaticMetadata.Members[1].TypeName.Count};
//	cnst ::gpk::vcst_t memberName = {&cttiStructStaticMetadata.SourceText[cttiStructStaticMetadata.Members[1].Name.Offset], cttiStructStaticMetadata.Members[1].Name.Count};
//	cnst ::gpk::vcst_t memberValue = {&cttiStructStaticMetadata.SourceText[cttiStructStaticMetadata.Members[2].Value.Offset], cttiStructStaticMetadata.Members[2].Value.Count};
//	GPK_TEST_CHECKF(errors, CTTI_STCT_TEST_RESULT_MODIFIERS, modifiers != "inln sttc cnst", "Unexpected modifiers:'%.*s'.", GPK_VIEW_ARGS(modifiers));
//	GPK_TEST_CHECKF(errors, CTTI_STCT_TEST_RESULT_TYPE_NAME, typeName != "f3_t", "Unexpected type:'%.*s'.", GPK_VIEW_ARGS(typeName));
//	GPK_TEST_CHECKF(errors, CTTI_STCT_TEST_RESULT_MEMBER_NAME, memberName != "Height", "Unexpected name:'%.*s'.", GPK_VIEW_ARGS(memberName));
//	GPK_TEST_CHECKF(errors, CTTI_STCT_TEST_RESULT_MEMBER_VALUE, memberValue != "\"Factory::Build<Value>(7)\"", "Unexpected value:'%.*s'.", GPK_VIEW_ARGS(memberValue));
//	GPK_TEST_CHECK(errors, CTTI_STCT_TEST_RESULT_MEMBER_VALUE, false == cttiStructStaticMetadata.Members[4].Value.empty());
//	GPK_TEST_CHECK(errors, CTTI_STCT_TEST_RESULT_MEMBER_FIND, (::gpk::view<cnst ::gpk::SCTTIMember>{cttiStructStaticMetadata.Members.begin(), cttiStructStaticMetadata.Count}.find([&](cnst ::gpk::SCTTIMember & member) { rtrn ::gpk::vcst_t{&cttiStructStaticMetadata.SourceText[member.Name.Offset], member.Name.Count} == "Enabled"; }) != 3));
//	GPK_TEST_CHECK(errors, CTTI_STCT_TEST_RESULT_BRACE_VALUE, cttiStructStaticBraceValue.Success());
//	GPK_TEST_CHECK(errors, CTTI_STCT_TEST_RESULT_ALIAS_MODIFIER, false == cttiStructStaticModifier.Success());
//	GPK_TEST_CHECK(errors, CTTI_STCT_TEST_RESULT_TYPE_FACTS, sizeof(SCTTIStructStaticPOD) != ::gpk::SCTTIStructRegistry<SCTTIStructStaticPOD>::Size || false == ::gpk::SCTTIStructRegistry<SCTTIStructStaticPOD>::IsPOD);
//	GPK_TEST_CHECK(errors, CTTI_STCT_TEST_RESULT_FIELD_TYPES, (false == ::gpk::ctti_type_in_pack<::gpk::u0_t, SCTTIStructStaticFieldTypes>::Value || false == ::gpk::ctti_type_in_pack<::gpk::s2_t, SCTTIStructStaticFieldTypes>::Value || false == ::gpk::ctti_type_in_pack<::gpk::u1_t[3], SCTTIStructStaticFieldTypes>::Value));
//	GPK_TEST_CHECK(errors, CTTI_STCT_TEST_RESULT_PARSE_RESULT, false == cttiStructStaticNestedTemplate.Success());
//	cnst ::gpk::vcst_t nestedTypeName = {&cttiStructStaticNestedTemplate.SourceText[cttiStructStaticNestedTemplate.Members[0].TypeName.Offset], cttiStructStaticNestedTemplate.Members[0].TypeName.Count};
//	GPK_TEST_CHECKF(errors, CTTI_STCT_TEST_RESULT_TYPE_NAME, nestedTypeName != "SCTTIStructStaticArray<SCTTIStructStaticArray<gpk::u1_t, 2>, 3>", "Unexpected nested template type:'%.*s'.", GPK_VIEW_ARGS(nestedTypeName));
//	rtrn 0;
//}
