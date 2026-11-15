#include "gpk_ctti_parse.h"
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
		inxp err_t			Find		(cnst vcst_t name) cnst nxpt {
			for(u2_t iMember = 0; iMember < Count; ++iMember)
				if(View(Members[iMember].Name) == name)
					rtrn iMember;
			rtrn -1;
		}
	};

	stct SCTTIValue {
		vu0_t		Data		= {};
		vcst_t		Type		= {};
		vcst_t		Name		= {};
	};

	tplt<tpnm T>
	stct SCTTIStructRegistry;

	tplt<tpnm T>
	inxp cnst auto & cttiStruct() nxpt { rtrn SCTTIStructRegistry<T>::Value; }

	tplt<tpnm T, size_t N>
	stct SCTTIMemberAccess;

	tplt<tpnm T>
	stct SCTTIMemberAccess<T, 1> {
		tplt<tpnm TVisitor>
		sttc err_t Visit(T & instance, u2_c index, TVisitor & visitor) {
			auto & [member0] = instance;
			rtrn 0 == index ? visitor(member0) : -1;
		}
	};

	tplt<tpnm T>
	stct SCTTIMemberAccess<T, 2> {
		tplt<tpnm TVisitor>
		sttc err_t Visit(T & instance, u2_c index, TVisitor & visitor) {
			auto & [member0, member1] = instance;
			switch(index) {
			case 0: rtrn visitor(member0);
			case 1: rtrn visitor(member1);
			default: rtrn -1;
			}
		}
	};

	tplt<tpnm T>
	stct SCTTIMemberAccess<T, 3> {
		tplt<tpnm TVisitor>
		sttc err_t Visit(T & instance, u2_c index, TVisitor & visitor) {
			auto & [member0, member1, member2] = instance;
			switch(index) {
			case 0: rtrn visitor(member0);
			case 1: rtrn visitor(member1);
			case 2: rtrn visitor(member2);
			default: rtrn -1;
			}
		}
	};

	tplt<tpnm T>
	stct SCTTIMemberAccess<T, 4> {
		tplt<tpnm TVisitor>
		sttc err_t Visit(T & instance, u2_c index, TVisitor & visitor) {
			auto & [member0, member1, member2, member3] = instance;
			switch(index) {
			case 0: rtrn visitor(member0);
			case 1: rtrn visitor(member1);
			case 2: rtrn visitor(member2);
			case 3: rtrn visitor(member3);
			default: rtrn -1;
			}
		}
	};

	tplt<tpnm T>
	inxp err_t cttiMemberIndex(cnst vcst_t name) nxpt { rtrn cttiStruct<T>().Find(name); }

	tplt<tpnm T, tpnm TVisitor>
	err_t cttiVisit(T & instance, cnst vcst_t name, TVisitor && visitor) {
		cnst err_t iMember = cttiMemberIndex<T>(name);
		if(0 > iMember)
			rtrn iMember;
		rtrn SCTTIMemberAccess<T, SCTTIStructRegistry<T>::Value.Count>::Visit(instance, (u2_t)iMember, visitor);
	}

	tplt<tpnm T>
	err_t cttiValue(T & instance, cnst vcst_t name, SCTTIValue & output) {
		output = {};
		cnst err_t iMember = cttiMemberIndex<T>(name);
		if(0 > iMember)
			rtrn iMember;
		auto memberView = [&output](auto & member) -> err_t {
			output.Data = {(u0_t*)&member, sizeof(member)};
			rtrn 0;
		};
		cnst err_t visitResult = SCTTIMemberAccess<T, SCTTIStructRegistry<T>::Value.Count>::Visit(instance, (u2_t)iMember, memberView);
		if(0 > visitResult)
			rtrn visitResult;
		cnst auto & metadata = cttiStruct<T>();
		cnst SCTTIMember & member = metadata.Members[iMember];
		output.Type = metadata.View(member.TypeName);
		output.Name = metadata.View(member.Name);
		rtrn iMember;
	}

	tplt<size_t NDeclaration>
	err_t cttiMemberJSON
		( cnst SCTTIParseResult<NDeclaration> & metadata
		, string & output
		) {
		output.clear();
		if_fail_fe(output.push_back('{'));
		for(u2_t iMember = 0; iMember < metadata.Count; ++iMember) {
			cnst SCTTIMember & member = metadata.Members[iMember];
			cnst vcst_t memberName = metadata.View(member.Name);
			cnst vcst_t typeName = metadata.View(member.TypeName);
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
				cnst vcst_t value = metadata.View(member.Value);
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

			u2_t nameOffset = declarationSpan.Offset + declarationSpan.Count;
			while(nameOffset > declarationSpan.Offset && cttiNamePart(result.SourceText[nameOffset - 1]))
				--nameOffset;
			cnst SCTTISpan memberName = {nameOffset, declarationSpan.Offset + declarationSpan.Count - nameOffset};
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
	}

#endif // GPK_CTTI_STCT_H
