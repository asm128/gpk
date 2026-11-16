#include "gpk_ctti_parse.h"
#include "gpk_append_json.h"
#include "gpk_array_obj.h"
#include "gpk_cstring.h"
#include "gpk_json.h"
#include "gpk_string.h"

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

	stct SCTTIStructDeclaration;
	err_t cttiStructDeclarationRegister(cnst SCTTIStructDeclaration & declaration) nxpt;

	stct SCTTIStructDeclaration {
		vcst_t				File		= {};
		u2_t				Line		= {};
		vcst_t				SourceText	= {};
		vcst_t				Name		= {};
		view<cnst SCTTIMember>
							Members		= {};
		uP_t				Size		= {};
		b8_t				IsPOD		= {};

		SCTTIStructDeclaration() nxpt = dflt;

		tplt<size_t NDeclaration>
		SCTTIStructDeclaration
			( vcst_c file
			, u2_c line
			, cnst SCTTIParseResult<NDeclaration> & metadata
			, uP_c size
			, b8_c isPOD
			) nxpt
			: File		(file)
			, Line		(line)
			, SourceText	{metadata.SourceText, NDeclaration - 1}
			, Name		{metadata.View(metadata.StructName)}
			, Members	{metadata.Members, metadata.Count}
			, Size		(size)
			, IsPOD		(isPOD)
			{}

		inxp vcst_t View(cnst SCTTISpan span) cnst nxpt { rtrn {SourceText.begin() + span.Offset, span.Count}; }
	};

	inln aobj<SCTTIStructDeclaration> & cttiStructDeclarations() nxpt {
		sttc aobj<SCTTIStructDeclaration> declarations = {};
		rtrn declarations;
	}

	inln err_t cttiStructDeclarationRegister(cnst SCTTIStructDeclaration & declaration) nxpt {
		aobj<SCTTIStructDeclaration> & declarations = cttiStructDeclarations();
		for(u2_t iDeclaration = 0; iDeclaration < declarations.size(); ++iDeclaration)
			if(declarations[iDeclaration].File == declaration.File && declarations[iDeclaration].Line == declaration.Line && declarations[iDeclaration].Name == declaration.Name)
				rtrn 0;
		rtrn declarations.push_back(declaration);
	}

	inln cnst SCTTIStructDeclaration & cttiStruct(vcst_c name) nxpt {
		cnst aobj<SCTTIStructDeclaration> & declarations = cttiStructDeclarations();
		for(u2_t iDeclaration = 0; iDeclaration < declarations.size(); ++iDeclaration)
			if(declarations[iDeclaration].Name == name)
				rtrn declarations[iDeclaration];
		sttc cnst SCTTIStructDeclaration empty = {};
		rtrn empty;
	}

	tplt<tpnm T>
	stct SCTTIStructRegistry;

	tplt<tpnm T>
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
	tplt<tpnm TStruct, u2_t Index>
	stct SCTTIFieldProbe {
		tplt<tpnm TMember, size_t = sizeof(SCTTIFieldTypeRegister<TStruct, TMember, Index>)>
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

	tplt<tpnm TStruct, tpnm TFunc, tpnm... TMembers>
	err_t cttiVisitMembers(TStruct & instance, u2_c index, TFunc func, type_pack<TMembers...>) {
		u0_t (&bytes)[sizeof(TStruct)] = reinterpret_cast<u0_t(&)[sizeof(TStruct)]>(instance);
		uP_t offset = {};
		u2_t current = {};
		err_t result = -1;
		( [&]() {
			stxp uP_t alignment = cttiMemberAlignment<TStruct, TMembers>();
			offset += (alignment - offset % alignment) % alignment;
			if(current++ == index)
				result = func(reinterpret_cast<TMembers&>(bytes[offset]));
			offset += sizeof(TMembers);
		}(), ... );
		rtrn result;
	}
	tplt<tpnm TStruct, tpnm TFunc, tpnm... TMembers>
	err_t cttiVisitMembers(cnst TStruct & instance, u2_c index, TFunc func, type_pack<TMembers...>) {
		cnst u0_t (&bytes)[sizeof(TStruct)] = reinterpret_cast<cnst u0_t(&)[sizeof(TStruct)]>(instance);
		uP_t offset = {};
		u2_t current = {};
		err_t result = -1;
		( [&]() {
			stxp uP_t alignment = cttiMemberAlignment<TStruct, TMembers>();
			offset += (alignment - offset % alignment) % alignment;
			if(current++ == index)
				result = func(reinterpret_cast<cnst TMembers&>(bytes[offset]));
			offset += sizeof(TMembers);
		}(), ... );
		rtrn result;
	}

	tplt<tpnm T, tpnm TFunc>
	err_t cttiVisit(T & instance, u2_c index, TFunc func) {
		usng TStruct = rm_csvl_t<T>;
		usng TIndices = tpnm ctti_index_pack_make<SCTTIStructRegistry<TStruct>::Value.Count>::Type;
		usng TMembers = tpnm SCTTIFieldTypes<TStruct, TIndices>::Type;
		rtrn cttiVisitMembers(instance, index, func, TMembers{});
	}

	tplt<tpnm T>
	concept ctti_struct = requires { SCTTIStructRegistry<rm_csvl_t<T>>::Value; };
	tplt<tpnm T>
	concept ctti_integer = ctti_type_in_pack<rm_csvl_t<T>, type_pack_integers>::Value;
	tplt<tpnm T>
	concept ctti_decimal = ctti_type_in_pack<rm_csvl_t<T>, type_pack<f2_t, f3_t>>::Value;

	tplt<tpnm T> requires ctti_struct<T>
	err_t saveCTTI(string & output, cnst T & input);
	tplt<tpnm T> requires ctti_struct<T>
	err_t loadCTTI(cnst SJSONReader & input, u2_c iNode, T & output);

	inln err_t cttiValueSave(string & output, b8_c input) {
		rtrn output.append(str(input));
	}
	tplt<tpnm T> requires ctti_integer<T>
	err_t cttiValueSave(string & output, cnst T & input) {
		sc_t text[64] = {};
		err_t length = {};
		if constexpr(ctti_same<rm_csvl_t<T>, s0_t>::Value || ctti_same<rm_csvl_t<T>, s1_t>::Value || ctti_same<rm_csvl_t<T>, s2_t>::Value || ctti_same<rm_csvl_t<T>, s3_t>::Value)
			length = sprintf_s(text, "%" GPK_FMT_S3, (s3_t)input);
		else
			length = sprintf_s(text, "%" GPK_FMT_U3, (u3_t)input);
		if_fail_fe(length);
		rtrn output.append({text, (u2_t)length});
	}
	tplt<tpnm T> requires ctti_decimal<T>
	err_t cttiValueSave(string & output, cnst T & input) {
		sc_t text[64] = {};
		cnst err_t length = sprintf_s(text, "%.17g", (f3_t)input);
		if_fail_fe(length);
		rtrn output.append({text, (u2_t)length});
	}
	tplt<tpnm T> requires(__is_enum(T))
	err_t cttiValueSave(string & output, cnst T & input) {
		vcst_t name = {};
		if_fail_fe(get_enum<T>().get_value_label(input, name));
		if_fail_fe(output.push_back('"'));
		if_fail_fe(output.append(name));
		rtrn output.push_back('"');
	}
	tplt<tpnm T> requires ctti_struct<T>
	err_t cttiValueSave(string & output, cnst T & input) {
		rtrn saveCTTI(output, input);
	}

	inln err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, b8_t & output) {
		if_true_fe(input.Token[iNode].Type != JSON_TYPE_BOOLEAN);
		rtrn jsonObjectGetBoolean(input, iNode, output);
	}
	tplt<tpnm T> requires ctti_integer<T>
	err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, T & output) {
		if_true_fe(input.Token[iNode].Type != JSON_TYPE_INTEGER);
		rtrn jsonObjectGetInteger(input, iNode, output);
	}
	tplt<tpnm T> requires ctti_decimal<T>
	err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, T & output) {
		if(JSON_TYPE_INTEGER == input.Token[iNode].Type) {
			s3_t integer = {};
			if_fail_fe(jsonObjectGetInteger(input, iNode, integer));
			output = (T)integer;
			rtrn iNode;
		}
		if_true_fe(input.Token[iNode].Type != JSON_TYPE_DECIMAL);
		rtrn jsonObjectGetDecimal(input, iNode, output);
	}
	tplt<tpnm T> requires(__is_enum(T))
	err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, T & output) {
		if_true_fe(input.Token[iNode].Type != JSON_TYPE_STRING);
		vcst_t name = {};
		if_fail_fe(jsonObjectGetString(input, iNode, name));
		rtrn get_enum<T>().get_value(name, output);
	}
	tplt<tpnm T> requires ctti_struct<T>
	err_t cttiValueLoad(cnst SJSONReader & input, u2_c iNode, T & output) {
		rtrn loadCTTI(input, iNode, output);
	}

	tplt<tpnm T> requires ctti_struct<T>
	err_t saveCTTI(string & output, cnst T & input) {
		u2_c offset = output.size();
		cnst auto & metadata = cttiStruct<rm_csvl_t<T>>();
		if_fail_fe(output.push_back('{'));
		for(u2_t iMember = 0; iMember < metadata.Count; ++iMember) {
			if_fail_fe(appendOpenKey(output, metadata.View(metadata.Members[iMember].Name), 0 != iMember));
			if_fail_fe(cttiVisit(input, iMember, [&](cnst auto & member) { rtrn cttiValueSave(output, member); }));
		}
		if_fail_fe(output.push_back('}'));
		rtrn output.size() - offset;
	}

	tplt<tpnm T> requires ctti_struct<T>
	err_t loadCTTI(cnst SJSONReader & input, u2_c iNode, T & output) {
		cnst auto & metadata = cttiStruct<rm_csvl_t<T>>();
		if_true_fe(iNode >= input.Token.size() || input.Token[iNode].Type != JSON_TYPE_OBJECT);
		for(u2_t iMember = 0; iMember < metadata.Count; ++iMember) {
			cnst err_t iValue = jsonObjectValueGet(input, iNode, metadata.View(metadata.Members[iMember].Name));
			if_fail_fe(iValue);
			if_fail_fe(cttiVisit(output, iMember, [&](auto & member) { rtrn cttiValueLoad(input, (u2_t)iValue, member); }));
		}
		rtrn 0;
	}

	tplt<tpnm T> requires ctti_struct<T>
	err_t loadCTTI(cnst SJSONReader & input, T & output) {
		rtrn loadCTTI(input, 0, output);
	}

	tplt<tpnm T> requires ctti_struct<T>
	err_t loadCTTI(vcst_t & input, T & output) {
		SJSONReader reader = {};
		if_fail_fe(jsonParse(reader, input));
		if_fail_fe(loadCTTI(reader, output));
		rtrn input.slice(input, reader.StateRead.IndexCurrentChar + 1);
	}

	inln err_t cttiMemberJSON
		( cnst SCTTIStructDeclaration & metadata
		, string & output
		) {
		output.clear();
		if_fail_fe(output.push_back('{'));
		for(u2_t iMember = 0; iMember < metadata.Members.size(); ++iMember) {
			cnst SCTTIMember & member = metadata.Members[iMember];
			vcst_c memberName = metadata.View(member.Name);
			vcst_c typeName = metadata.View(member.TypeName);
			if(iMember)
				if_fail_fe(output.push_back(','));
			if_fail_fe(output.push_back('"'));
			if_fail_fe(output.append(memberName));
			if_fail_fe(output.push_back('"'));
			if_fail_fe(output.push_back(':'));
			if_fail_fe(output.push_back('{'));
			if_fail_fe(output.append(vcst_t{"\"type\":\""}));
			if_fail_fe(output.append(typeName));
			if_fail_fe(output.append(vcst_t{"\",\"value\":"}));
			if(member.Value.empty()) {
				if_fail_fe(output.append(vcst_t{"null"}));
			}
			else {
				vcst_c value = metadata.View(member.Value);
				b8_c stringLiteral = value.size() > 1 && '"' == value[0] && '"' == value[value.size() - 1];
				b8_c keywordLiteral = value == "true" || value == "false" || value == "null" || value == "{}" || value == "[]";
				b8_c numberLiteral = value.size() && (('-' == value[0]) || ('+' == value[0]) || (value[0] >= '0' && value[0] <= '9'));
				b8_c floatSuffix = numberLiteral && value.size() && ('f' == value[value.size() - 1] || 'F' == value[value.size() - 1]);
				if(stringLiteral || keywordLiteral || numberLiteral) {
					if_fail_fe(output.append({value.begin(), value.size() - floatSuffix}));
				}
				else {
					if_fail_fe(output.push_back('"'));
					if_fail_fe(output.append(value));
					if_fail_fe(output.push_back('"'));
				}
			}
			if_fail_fe(output.append(vcst_t{",\"modifiers\":["}));
			b8_t firstModifier = true;
			for(u2_t iModifier = member.Modifiers.Offset, modifierStop = member.Modifiers.Offset + member.Modifiers.Count; iModifier < modifierStop;) {
				while(iModifier < modifierStop && cttiSpace(metadata.SourceText[iModifier]))
					++iModifier;
				cnst u2_t modifierOffset = iModifier;
				while(iModifier < modifierStop && false == cttiSpace(metadata.SourceText[iModifier]))
					++iModifier;
				if(modifierOffset == iModifier)
					break;
				if(false == firstModifier)
					if_fail_fe(output.push_back(','));
				if_fail_fe(output.push_back('"'));
				if_fail_fe(output.append(vcst_t{&metadata.SourceText[modifierOffset], iModifier - modifierOffset}));
				if_fail_fe(output.push_back('"'));
				firstModifier = false;
			}
			if_fail_fe(output.push_back(']'));
			if_fail_fe(output.push_back('}'));
		}
		if_fail_fe(output.push_back('}'));
		rtrn output.size();
	}

	tplt<size_t NDeclaration>
	err_t cttiMemberJSON
		( cnst SCTTIParseResult<NDeclaration> & metadata
		, string & output
		) {
		cnst SCTTIStructDeclaration declaration = {vcst_t{}, 0, metadata, 0, false};
		rtrn cttiMemberJSON(declaration, output);
	}


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
			while(typeOffset > beforeName.Offset && false == cttiSpace(result.SourceText[typeOffset - 1]))
				--typeOffset;
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

#define CTTI_STCT(type, ...) \
	__VA_ARGS__ \
	tplt<> stct gpk::SCTTIStructRegistry<type> { \
		sinx auto Value = ::gpk::cttiParseStruct(#__VA_ARGS__); \
		sinx ::gpk::vcst_t File = __FILE__; \
		sinx ::gpk::u2_t Line = __LINE__; \
		sinx ::gpk::uP_t Size = sizeof(type); \
		sinx b8_t IsPOD = __is_pod(type); \
	}; \
	sttc ::gpk::SCTTIStructDeclaration type##_gpk_ctti_declaration = \
		{::gpk::SCTTIStructRegistry<type>::File, ::gpk::SCTTIStructRegistry<type>::Line, ::gpk::SCTTIStructRegistry<type>::Value, ::gpk::SCTTIStructRegistry<type>::Size, ::gpk::SCTTIStructRegistry<type>::IsPOD}; \
	sttc cnst ::gpk::err_t type##_gpk_ctti_registration = \
		::gpk::cttiStructDeclarationRegister(type##_gpk_ctti_declaration)

#endif // GPK_CTTI_STCT_H
