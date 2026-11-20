#include "gpk_ctti_parse.h"
#include "gpk_array_static.h"

#ifndef GPK_CTTI_STCT_H
#define GPK_CTTI_STCT_H

nmsp gpk
{
	enum CTTI_PARSE_STATE : u0_t
		{ CTTI_PARSE_STATE_STRUCT_NAME	= 0
		, CTTI_PARSE_STATE_MODIFIERS		= 1
		, CTTI_PARSE_STATE_TYPE			= 2
		, CTTI_PARSE_STATE_MEMBER_NAME	= 3
		, CTTI_PARSE_STATE_COMPLETE		= 4
		};

	stct SCTTIReaderState {
		u2_t              IndexCurrentChar = {};
		CTTI_PARSE_STATE  State            = CTTI_PARSE_STATE_STRUCT_NAME;
	};

	stct SCTTIMember {
		rangeu2_t	Modifiers	= {};
		rangeu2_t	TypeName	= {};
		rangeu2_t	Name		= {};
		rangeu2_t	Value		= {};
	};

	tplt<size_t NDeclaration>
	stct SCTTIParseResult {
		sstring<NDeclaration>	SourceText					= {};
		rangeu2_t			StructName					= {};
		astatic<SCTTIMember, NDeclaration>	Members		= {};
		u2_t				Count						= {};
		SCTTIReaderState StateRead                   = {};

		inxp b8_t			Success		() cnst nxpt { rtrn CTTI_PARSE_STATE_COMPLETE == StateRead.State; }
	};

	tplt<tpnm T>
	stct SCTTIStructTag {};
	tplt<tpnm T>
	void cttiStructRegistry(SCTTIStructTag<T>) = delete;

	tplt<tpnm T>
	concept ctti_struct = requires { cttiStructRegistry(SCTTIStructTag<rm_csvl_t<T>>{}); };
	tplt<tpnm T> requires ctti_struct<T>
	usng SCTTIStructRegistry = decltype(cttiStructRegistry(SCTTIStructTag<rm_csvl_t<T>>{}));

	tplt<tpnm T> requires ctti_struct<T>
	inxp cnst auto & cttiStruct() nxpt { rtrn SCTTIStructRegistry<T>::Value; }

	tplt<tpnm T, tpnm U>
	stct ctti_same {
		stxp b8_t Value = false;
	};
	tplt<tpnm T>
	stct ctti_same<T, T> {
		stxp b8_t Value = true;
	};
	tplt<tpnm T, tpnm TPack>
	stct ctti_type_in_pack { stxp b8_t Value = false; };
	tplt<tpnm T, tpnm... TArgs>
	stct ctti_type_in_pack<T, type_pack<TArgs...>> {
		stxp b8_t Value = (ctti_same<T, TArgs>::Value || ...);
	};

	tplt<u2_t... Indices>
	stct ctti_index_pack {};
	tplt<u2_t N, u2_t... Indices>
	stct ctti_index_pack_make : ctti_index_pack_make<N - 1, N - 1, Indices...> {};
	tplt<u2_t... Indices>
	stct ctti_index_pack_make<0, Indices...> { usng Type = ctti_index_pack<Indices...>; };

	tplt<tpnm T>
	stct SCTTIType { usng Type = T; };
	tplt<tpnm TStruct, u2_t Index>
	stct SCTTIFieldTag { friend auto cttiFieldType(SCTTIFieldTag<TStruct, Index>); };
	tplt<tpnm TStruct, tpnm TMember, u2_t Index>
	stct SCTTIFieldTypeRegister {
		friend auto cttiFieldType(SCTTIFieldTag<TStruct, Index>) { rtrn SCTTIType<TMember>{}; }
	};
	tplt<tpnm TStruct, tpnm TMember, u2_t Index, tpnm = void>
	stct SCTTIFieldTypeEnsure : SCTTIFieldTypeRegister<TStruct, TMember, Index> {};
	tplt<tpnm TStruct, tpnm TMember, u2_t Index>
	stct SCTTIFieldTypeEnsure
		< TStruct
		, TMember
		, Index
		, decltype(void(sizeof(tpnm decltype(cttiFieldType(SCTTIFieldTag<TStruct, Index>{}))::Type)))
		> {};
	tplt<tpnm TStruct, u2_t Index>
	stct SCTTIFieldProbe {
		tplt<tpnm TMember, size_t = sizeof(SCTTIFieldTypeEnsure<TStruct, TMember, Index>)>
		inxp operator TMember & () cnst nxpt;
	};
	tplt<tpnm TStruct, tpnm TIndices, tpnm = void>
	stct SCTTIFieldTypes {};
	tplt<tpnm TStruct, u2_t... Indices>
	stct SCTTIFieldTypes
		< TStruct
		, ctti_index_pack<Indices...>
		, decltype(void(TStruct{SCTTIFieldProbe<TStruct, Indices>{}...}))
		> {
		usng Type = type_pack<tpnm decltype(cttiFieldType(SCTTIFieldTag<TStruct, Indices>{}))::Type...>;
	};
	tplt<tpnm TStruct>
	concept ctti_struct_member_types = requires { tpnm SCTTIStructRegistry<TStruct>::MemberTypes; };
	tplt<tpnm TStruct, tpnm TIndices, b8_t = ctti_struct_member_types<TStruct>>
	stct SCTTIMemberTypes { usng Type = tpnm SCTTIFieldTypes<TStruct, TIndices>::Type; };
	tplt<tpnm TStruct, tpnm TIndices>
	stct SCTTIMemberTypes<TStruct, TIndices, true> {
		usng Type = tpnm SCTTIStructRegistry<TStruct>::MemberTypes;
	};

	tplt<tpnm TStruct, tpnm TMember>
	inxp uP_t cttiMemberAlignment() nxpt {
		rtrn alignof(TStruct) < alignof(TMember) ? alignof(TStruct) : alignof(TMember);
	}

	tplt<tpnm T>
	concept ctti_integer = ctti_type_in_pack<rm_csvl_t<T>, type_pack_integers>::Value;
	tplt<tpnm T>
	concept ctti_decimal = ctti_type_in_pack<rm_csvl_t<T>, type_pack<f2_t, f3_t>>::Value;

	tplt<size_t NDeclaration>
	inxp SCTTIParseResult<NDeclaration> cttiParseStruct(sc_c (&declaration)[NDeclaration]) nxpt {
		static_assert(NDeclaration <= 0xFFFFFFFFULL, "CTTI source text exceeds the supported size.");
		SCTTIParseResult<NDeclaration> result = {};
		SCTTIReaderState & state = result.StateRead;
		for(u2_t iChar = 0; iChar < NDeclaration; ++iChar)
			result.SourceText[iChar] = declaration[iChar];
		cnst vcst_t sourceText = {result.SourceText.Storage, (u2_t)NDeclaration - 1};

		rangeu2_t sourceSpan = trim(sourceText, {0, (u2_t)NDeclaration - 1}, TRIM_CHARACTERS);
		if(sourceSpan.Count && ';' == result.SourceText[sourceSpan.Offset + sourceSpan.Count - 1])
			sourceSpan = trim(sourceText, {sourceSpan.Offset, sourceSpan.Count - 1}, TRIM_CHARACTERS);
		vcsc_t sourceView = {};
		sourceText.slice(sourceView, sourceSpan);
		cnst err_t iOpenBrace = ::gpk::find('{', sourceView);
		u2_c openBrace = 0 > iOpenBrace ? sourceSpan.Offset + sourceSpan.Count : sourceSpan.Offset + iOpenBrace;
		state.IndexCurrentChar = openBrace;
		if(openBrace == sourceSpan.Offset + sourceSpan.Count)
			rtrn result;
		if(0 == sourceSpan.Count || '}' != result.SourceText[sourceSpan.Offset + sourceSpan.Count - 1]) {
			state.State = CTTI_PARSE_STATE_MEMBER_NAME;
			state.IndexCurrentChar = sourceSpan.Offset + sourceSpan.Count;
			rtrn result;
		}

		result.StructName = trim(sourceText, {sourceSpan.Offset, openBrace - sourceSpan.Offset}, TRIM_CHARACTERS);
		if(result.StructName.Count > 6
		&& vcst_t{&result.SourceText[result.StructName.Offset], 6} == "struct"
		&& is_space_character(result.SourceText[result.StructName.Offset + 6])
		)
			result.StructName = trim(sourceText, {result.StructName.Offset + 6, result.StructName.Count - 6}, TRIM_CHARACTERS);
		cnst err_t structNameError = cttiIdentifierError(result.SourceText, result.StructName);
		if(0 <= structNameError) {
			state.IndexCurrentChar = (u2_t)structNameError;
			rtrn result;
		}

		state.State = CTTI_PARSE_STATE_MODIFIERS;
		state.IndexCurrentChar = openBrace + 1;
		u2_t		& cursor		= state.IndexCurrentChar;
		u2_c	textLength		= sourceSpan.Offset + sourceSpan.Count - 1;
		while(true) {
			while(cursor < textLength && is_space_character(result.SourceText[cursor]))
				++cursor;
			if(cursor == textLength) {
				state.State = CTTI_PARSE_STATE_COMPLETE;
				rtrn result;
			}
			state.State = CTTI_PARSE_STATE_MODIFIERS;

			u2_c declarationOffset = cursor;
			sourceText.slice(sourceView, rangeu2_t{cursor, textLength - cursor});
			cnst err_t iSemicolon = ::gpk::find(';', sourceView);
			cursor = 0 > iSemicolon ? textLength : cursor + iSemicolon;
			if(cursor == textLength) {
				state.State = CTTI_PARSE_STATE_MEMBER_NAME;
				rtrn result;
			}

			cnst rangeu2_t segment = trim(sourceText, {declarationOffset, cursor - declarationOffset}, TRIM_CHARACTERS);
			sourceText.slice(sourceView, segment);
			cnst err_t iEqual = ::gpk::find('=', sourceView);
			u2_c equalOffset = 0 > iEqual ? segment.Offset + segment.Count : segment.Offset + iEqual;
			cnst rangeu2_t declarationSpan = trim(sourceText, {segment.Offset, equalOffset - segment.Offset}, TRIM_CHARACTERS);
			cnst rangeu2_t memberValue = (equalOffset < segment.Offset + segment.Count)
				? trim(sourceText, {equalOffset + 1, segment.Offset + segment.Count - equalOffset - 1}, TRIM_CHARACTERS)
				: rangeu2_t{};

			u2_t declaratorEnd = declarationSpan.Offset + declarationSpan.Count;
			while(declaratorEnd > declarationSpan.Offset && is_space_character(result.SourceText[declaratorEnd - 1]))
				--declaratorEnd;
			while(declaratorEnd > declarationSpan.Offset && ']' == result.SourceText[declaratorEnd - 1]) {
				u2_t depth = 1;
				u2_t iSuffix = declaratorEnd - 1;
				while(iSuffix > declarationSpan.Offset && depth) {
					--iSuffix;
					depth += ']' == result.SourceText[iSuffix];
					depth -= '[' == result.SourceText[iSuffix];
				}
				if(depth)
					break;
				declaratorEnd = iSuffix;
				while(declaratorEnd > declarationSpan.Offset && is_space_character(result.SourceText[declaratorEnd - 1]))
					--declaratorEnd;
			}
			for(u2_t iColon = declaratorEnd; iColon-- > declarationSpan.Offset; )
				if(':' == result.SourceText[iColon]
				&& (iColon == declarationSpan.Offset || ':' != result.SourceText[iColon - 1])
				&& (iColon + 1 == declaratorEnd || ':' != result.SourceText[iColon + 1])
				) {
					declaratorEnd = iColon;
					while(declaratorEnd > declarationSpan.Offset && is_space_character(result.SourceText[declaratorEnd - 1]))
						--declaratorEnd;
					break;
				}

			u2_t nameOffset = declaratorEnd;
			while(nameOffset > declarationSpan.Offset && cttiNamePart(result.SourceText[nameOffset - 1]))
				--nameOffset;
			cnst rangeu2_t memberName = {nameOffset, declaratorEnd - nameOffset};
			cnst err_t memberNameError = cttiIdentifierError(result.SourceText, memberName);
			if(0 <= memberNameError) {
				state.State = CTTI_PARSE_STATE_MEMBER_NAME;
				cursor = (u2_t)memberNameError;
				rtrn result;
			}

			cnst rangeu2_t beforeName = trim(sourceText, {declarationSpan.Offset, nameOffset - declarationSpan.Offset}, TRIM_CHARACTERS);
			if(beforeName.empty()) {
				state.State = CTTI_PARSE_STATE_TYPE;
				cursor = declarationSpan.Offset;
				rtrn result;
			}
			u2_t typeOffset = beforeName.Offset + beforeName.Count;
			u2_t templateDepth = 0;
			while(typeOffset > beforeName.Offset) {
				sc_c character = result.SourceText[typeOffset - 1];
				if('>' == character)
					++templateDepth;
				else if('<' == character && templateDepth)
					--templateDepth;
				else if(is_space_character(character) && 0 == templateDepth)
					break;
				--typeOffset;
			}
			cnst rangeu2_t typeName = {typeOffset, beforeName.Offset + beforeName.Count - typeOffset};
			cnst rangeu2_t modifiers = trim(sourceText, {beforeName.Offset, typeOffset - beforeName.Offset}, TRIM_CHARACTERS);
			for(u2_t iChar = 0; iChar < modifiers.Count; ++iChar) {
				sc_c character = result.SourceText[modifiers.Offset + iChar];
				if(false == is_space_character(character) && false == cttiNameStart(character)) {
					cursor = modifiers.Offset + iChar;
					rtrn result;
				}
			}

			result.Members[result.Count++] = {modifiers, typeName, memberName, memberValue};
			++cursor;
		}
	}
} // nmsp

#define GPK_CTTI_STCT_STATIC_REGISTER(type, declaration) \
	stct type##_gpk_ctti_storage { \
		sinx auto Value = ::gpk::cttiParseStruct(declaration); \
		sinx ::gpk::vcst_t File = __FILE__; \
		sinx ::gpk::u2_t Line = __LINE__; \
		sinx ::gpk::uP_t Size = sizeof(type); \
		sinx b8_t IsPOD = __is_pod(type); \
	}; \
	sinx type##_gpk_ctti_storage cttiStructRegistry(::gpk::SCTTIStructTag<type>) nxpt { rtrn {}; }

#define GPK_CTTI_STCT_REGISTER(type, declaration) \
	GPK_CTTI_STCT_STATIC_REGISTER(type, declaration)

#define CTTI_STCT_EXISTING(type, ...) \
	GPK_CTTI_STCT_REGISTER(type, #__VA_ARGS__)

#define CTTI_STCT(type, ...) \
	__VA_ARGS__ \
	GPK_CTTI_STCT_REGISTER(type, #__VA_ARGS__)

#define GPK_CTTI_STCT_TEMPLATE_REGISTER(type, type_parameter, declaration, ...) \
	tplt<tpnm type_parameter> stct type##_gpk_ctti_storage { \
		usng MemberTypes = ::gpk::type_pack<__VA_ARGS__>; \
		sinx auto Value = ::gpk::cttiParseStruct(#declaration); \
		sinx ::gpk::vcst_t File = __FILE__; \
		sinx ::gpk::u2_t Line = __LINE__; \
		sinx ::gpk::uP_t Size = sizeof(type<type_parameter>); \
		sinx b8_t IsPOD = __is_pod(type<type_parameter>); \
	}; \
	tplt<tpnm type_parameter> sinx type##_gpk_ctti_storage<type_parameter> cttiStructRegistry(::gpk::SCTTIStructTag<type<type_parameter>>) nxpt { rtrn {}; }

#define CTTI_STCT_TEMPLATE_EXISTING(type, type_parameter, declaration, ...) \
	GPK_CTTI_STCT_TEMPLATE_REGISTER(type, type_parameter, declaration, __VA_ARGS__)

#endif // GPK_CTTI_STCT_H
