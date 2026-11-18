#include "gpk_ctti_parse.h"

#ifndef GPK_CTTI_STCT_H
#define GPK_CTTI_STCT_H

nmsp gpk
{
	enum CTTI_PARSE_STATE : u0_t
		{ CTTI_PARSE_STATE_STRUCT_NAME	= 0
		, CTTI_PARSE_STATE_MODIFIERS		= 1
		, CTTI_PARSE_STATE_TYPE			= 2
		, CTTI_PARSE_STATE_MEMBER_NAME	= 3
		, CTTI_PARSE_STATE_MEMBER_VALUE	= 4
		, CTTI_PARSE_STATE_COMPLETE		= 5
		};

	enum CTTI_PARSE_ERROR : u0_t
		{ CTTI_PARSE_ERROR_NONE						= 0
		, CTTI_PARSE_ERROR_INVALID_STRUCT_NAME		= 1
		, CTTI_PARSE_ERROR_EXPECTED_OPEN_BRACE		= 2
		, CTTI_PARSE_ERROR_EXPECTED_CLOSE_BRACE		= 3
		, CTTI_PARSE_ERROR_INVALID_MODIFIER			= 4
		, CTTI_PARSE_ERROR_EXPECTED_TYPE			= 5
		, CTTI_PARSE_ERROR_EXPECTED_MEMBER_NAME		= 6
		, CTTI_PARSE_ERROR_INVALID_MEMBER_NAME		= 7
		, CTTI_PARSE_ERROR_EXPECTED_SEMICOLON		= 8
		};

	stct SCTTIMember {
		SCTTISpan	Modifiers	= {};
		SCTTISpan	TypeName	= {};
		SCTTISpan	Name		= {};
		SCTTISpan	Value		= {};
	};

	tplt<size_t NDeclaration>
	stct SCTTIParseResult {
		sc_t				SourceText	[NDeclaration]	= {};
		SCTTISpan			StructName					= {};
		SCTTIMember			Members		[NDeclaration]	= {};
		u2_t				Count						= {};
		u2_t				ErrorOffset					= {};
		CTTI_PARSE_STATE	State						= CTTI_PARSE_STATE_STRUCT_NAME;
		CTTI_PARSE_ERROR	Error						= CTTI_PARSE_ERROR_NONE;

		inxp b8_t			Success		() cnst nxpt { rtrn CTTI_PARSE_ERROR_NONE == Error && CTTI_PARSE_STATE_COMPLETE == State; }
		inxp vcst_t			View		(cnst SCTTISpan span) cnst nxpt { rtrn {&SourceText[span.Offset], span.Count}; }
		inxp err_t			Find		(vcst_c name) cnst nxpt {
			for(u2_t iMember = 0; iMember < Count; ++iMember)
				if(View(Members[iMember].Name) == name)
					rtrn iMember;
			rtrn -1;
		}
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
	stct ctti_type_in_pack;
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
	stct SCTTIFieldTypes;
	tplt<tpnm TStruct, u2_t... Indices>
	stct SCTTIFieldTypes
		< TStruct
		, ctti_index_pack<Indices...>
		, decltype(void(TStruct{SCTTIFieldProbe<TStruct, Indices>{}...}))
		> {
		usng Type = type_pack<tpnm decltype(cttiFieldType(SCTTIFieldTag<TStruct, Indices>{}))::Type...>;
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
		for(u2_t iChar = 0; iChar < NDeclaration; ++iChar)
			result.SourceText[iChar] = declaration[iChar];

		SCTTISpan sourceSpan = cttiTrim(result.SourceText, 0, (u2_t)NDeclaration - 1);
		if(sourceSpan.Count && ';' == result.SourceText[sourceSpan.Offset + sourceSpan.Count - 1])
			sourceSpan = cttiTrim(result.SourceText, sourceSpan.Offset, sourceSpan.Count - 1);
		u2_c openBrace = cttiFind(result.SourceText, sourceSpan, '{');
		if(openBrace == sourceSpan.Offset + sourceSpan.Count) {
			result.Error		= CTTI_PARSE_ERROR_EXPECTED_OPEN_BRACE;
			result.State		= CTTI_PARSE_STATE_STRUCT_NAME;
			result.ErrorOffset	= openBrace;
			rtrn result;
		}
		if(0 == sourceSpan.Count || '}' != result.SourceText[sourceSpan.Offset + sourceSpan.Count - 1]) {
			result.Error		= CTTI_PARSE_ERROR_EXPECTED_CLOSE_BRACE;
			result.State		= CTTI_PARSE_STATE_MEMBER_NAME;
			result.ErrorOffset	= sourceSpan.Offset + sourceSpan.Count;
			rtrn result;
		}

		result.StructName = cttiTrim(result.SourceText, sourceSpan.Offset, openBrace - sourceSpan.Offset);
		if(result.StructName.Count > 6
		&& cttiSpanEquals(result.SourceText, {result.StructName.Offset, 6}, "struct")
		&& cttiSpace(result.SourceText[result.StructName.Offset + 6])
		)
			result.StructName = cttiTrim(result.SourceText, result.StructName.Offset + 6, result.StructName.Count - 6);
		if(false == cttiIdentifier(result.SourceText, result.StructName)) {
			result.Error		= CTTI_PARSE_ERROR_INVALID_STRUCT_NAME;
			result.State		= CTTI_PARSE_STATE_STRUCT_NAME;
			result.ErrorOffset	= cttiIdentifierError(result.SourceText, result.StructName);
			rtrn result;
		}

		result.State = CTTI_PARSE_STATE_MODIFIERS;
		u2_t		cursor			= openBrace + 1;
		u2_c	textLength		= sourceSpan.Offset + sourceSpan.Count - 1;
		while(true) {
			while(cursor < textLength && cttiSpace(result.SourceText[cursor]))
				++cursor;
			if(cursor == textLength) {
				result.State = CTTI_PARSE_STATE_COMPLETE;
				rtrn result;
			}

			u2_c declarationOffset = cursor;
			cursor = cttiFind(result.SourceText, {cursor, textLength - cursor}, ';');
			if(cursor == textLength) {
				result.Error		= CTTI_PARSE_ERROR_EXPECTED_SEMICOLON;
				result.State		= CTTI_PARSE_STATE_MEMBER_NAME;
				result.ErrorOffset	= cursor;
				rtrn result;
			}

			cnst SCTTISpan segment = cttiTrim(result.SourceText, declarationOffset, cursor - declarationOffset);
			u2_c equalOffset = cttiFind(result.SourceText, segment, '=');
			cnst SCTTISpan declarationSpan = cttiTrim(result.SourceText, segment.Offset, equalOffset - segment.Offset);
			cnst SCTTISpan memberValue = (equalOffset < segment.Offset + segment.Count)
				? cttiTrim(result.SourceText, equalOffset + 1, segment.Offset + segment.Count - equalOffset - 1)
				: SCTTISpan{};

			u2_t declaratorEnd = declarationSpan.Offset + declarationSpan.Count;
			while(declaratorEnd > declarationSpan.Offset && cttiSpace(result.SourceText[declaratorEnd - 1]))
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
				while(declaratorEnd > declarationSpan.Offset && cttiSpace(result.SourceText[declaratorEnd - 1]))
					--declaratorEnd;
			}
			for(u2_t iColon = declaratorEnd; iColon-- > declarationSpan.Offset; )
				if(':' == result.SourceText[iColon]
				&& (iColon == declarationSpan.Offset || ':' != result.SourceText[iColon - 1])
				&& (iColon + 1 == declaratorEnd || ':' != result.SourceText[iColon + 1])
				) {
					declaratorEnd = iColon;
					while(declaratorEnd > declarationSpan.Offset && cttiSpace(result.SourceText[declaratorEnd - 1]))
						--declaratorEnd;
					break;
				}

			u2_t nameOffset = declaratorEnd;
			while(nameOffset > declarationSpan.Offset && cttiNamePart(result.SourceText[nameOffset - 1]))
				--nameOffset;
			cnst SCTTISpan memberName = {nameOffset, declaratorEnd - nameOffset};
			if(false == cttiIdentifier(result.SourceText, memberName)) {
				result.Error		= CTTI_PARSE_ERROR_INVALID_MEMBER_NAME;
				result.State		= CTTI_PARSE_STATE_MEMBER_NAME;
				result.ErrorOffset	= cttiIdentifierError(result.SourceText, memberName);
				rtrn result;
			}

			cnst SCTTISpan beforeName = cttiTrim(result.SourceText, declarationSpan.Offset, nameOffset - declarationSpan.Offset);
			if(beforeName.empty()) {
				result.Error		= CTTI_PARSE_ERROR_EXPECTED_TYPE;
				result.State		= CTTI_PARSE_STATE_TYPE;
				result.ErrorOffset	= declarationSpan.Offset;
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
				else if(cttiSpace(character) && 0 == templateDepth)
					break;
				--typeOffset;
			}
			cnst SCTTISpan typeName = {typeOffset, beforeName.Offset + beforeName.Count - typeOffset};
			cnst SCTTISpan modifiers = cttiTrim(result.SourceText, beforeName.Offset, typeOffset - beforeName.Offset);
			for(u2_t iChar = 0; iChar < modifiers.Count; ++iChar) {
				sc_c character = result.SourceText[modifiers.Offset + iChar];
				if(false == cttiSpace(character) && false == cttiNameStart(character)) {
					result.Error		= CTTI_PARSE_ERROR_INVALID_MODIFIER;
					result.State		= CTTI_PARSE_STATE_MODIFIERS;
					result.ErrorOffset	= modifiers.Offset + iChar;
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

#endif // GPK_CTTI_STCT_H
