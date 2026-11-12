#include "gpk_typeint.h"

#ifndef GPK_CTTI_H
#define GPK_CTTI_H

namespace gpk
{
	enum CTTI_PARSE_STATE : u0_t
		{ CTTI_PARSE_STATE_STRUCT_NAME
		, CTTI_PARSE_STATE_MODIFIERS
		, CTTI_PARSE_STATE_TYPE
		, CTTI_PARSE_STATE_MEMBER_NAME
		, CTTI_PARSE_STATE_MEMBER_VALUE
		, CTTI_PARSE_STATE_COMPLETE
		};

	enum CTTI_PARSE_ERROR : u0_t
		{ CTTI_PARSE_ERROR_NONE
		, CTTI_PARSE_ERROR_INVALID_STRUCT_NAME
		, CTTI_PARSE_ERROR_INVALID_MODIFIER
		, CTTI_PARSE_ERROR_EXPECTED_TYPE
		, CTTI_PARSE_ERROR_EXPECTED_MEMBER_NAME
		, CTTI_PARSE_ERROR_INVALID_MEMBER_NAME
		, CTTI_PARSE_ERROR_EXPECTED_SEMICOLON
		};

	stct SCTTISpan {
		u2_t		Offset	= {};
		u2_t		Count	= {};
		inxp bool	empty	() cnst nxpt { rtrn 0 == Count; }
	};

	stct SCTTIMember {
		SCTTISpan	Modifiers	= {};
		SCTTISpan	TypeName	= {};
		SCTTISpan	Name		= {};
		SCTTISpan	Value		= {};
	};

	tplt<size_t NStructName, size_t NMemberText>
	stct SCTTIParseResult {
		sc_t				StructName	[NStructName]	= {};
		sc_t				MemberText	[NMemberText]	= {};
		SCTTIMember		Members		[NMemberText]	= {};
		u2_t				Count					= {};
		u2_t				ErrorOffset				= {};
		CTTI_PARSE_STATE	State					= CTTI_PARSE_STATE_STRUCT_NAME;
		CTTI_PARSE_ERROR	Error					= CTTI_PARSE_ERROR_NONE;

		inxp bool		Success		() cnst nxpt { rtrn CTTI_PARSE_ERROR_NONE == Error && CTTI_PARSE_STATE_COMPLETE == State; }
	};

	inxp bool cttiSpace		(cnst sc_t character) nxpt { rtrn ' ' == character || '\t' == character || '\r' == character || '\n' == character; }
	inxp bool cttiNameStart	(cnst sc_t character) nxpt { rtrn '_' == character || (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z'); }
	inxp bool cttiNamePart	(cnst sc_t character) nxpt { rtrn cttiNameStart(character) || (character >= '0' && character <= '9'); }

	tplt<size_t N>
	inxp SCTTISpan cttiTrim(cnst sc_t (&text)[N], u2_t offset, u2_t count) nxpt {
		while(count && cttiSpace(text[offset]))
			++offset, --count;
		while(count && cttiSpace(text[offset + count - 1]))
			--count;
		rtrn {offset, count};
	}
	tplt<size_t N>
	inxp u2_t cttiFind(cnst sc_t (&text)[N], cnst SCTTISpan span, cnst sc_t delimiter) nxpt {
		for(u2_t iChar = 0; iChar < span.Count; ++iChar)
			if(delimiter == text[span.Offset + iChar])
				rtrn span.Offset + iChar;
		rtrn span.Offset + span.Count;
	}

	tplt<size_t N>
	inxp bool cttiIdentifier(cnst sc_t (&text)[N], cnst SCTTISpan span) nxpt {
		if(span.empty() || false == cttiNameStart(text[span.Offset]))
			rtrn false;
		for(u2_t iChar = 1; iChar < span.Count; ++iChar)
			if(false == cttiNamePart(text[span.Offset + iChar]))
				rtrn false;
		rtrn true;
	}
	tplt<size_t N>
	inxp u2_t cttiIdentifierError(cnst sc_t (&text)[N], cnst SCTTISpan span) nxpt {
		if(span.empty() || false == cttiNameStart(text[span.Offset]))
			rtrn span.Offset;
		for(u2_t iChar = 1; iChar < span.Count; ++iChar)
			if(false == cttiNamePart(text[span.Offset + iChar]))
				rtrn span.Offset + iChar;
		rtrn span.Offset + span.Count;
	}
	tplt<size_t NText, size_t NValue>
	inxp bool cttiSpanEquals(cnst sc_t (&text)[NText], cnst SCTTISpan span, cnst sc_t (&value)[NValue]) nxpt {
		if(span.Count != NValue - 1)
			rtrn false;
		for(u2_t iChar = 0; iChar < span.Count; ++iChar)
			if(text[span.Offset + iChar] != value[iChar])
				rtrn false;
		rtrn true;
	}
	tplt<tpnm T>
	inxp auto cttiFirstMember(cnst T & source) nxpt {
		cnst auto & [result] = source;
		rtrn result;
	}
	tplt<size_t NStructName, size_t NMemberText>
	inxp SCTTIParseResult<NStructName, NMemberText> cttiParseStruct(cnst sc_t (&structName)[NStructName], cnst sc_t (&memberText)[NMemberText]) nxpt {
		static_assert(NStructName <= 0xFFFFFFFFULL && NMemberText <= 0xFFFFFFFFULL, "CTTI source text exceeds the supported size.");
		SCTTIParseResult<NStructName, NMemberText> result = {};
		for(u2_t iChar = 0; iChar < NStructName; ++iChar)
			result.StructName[iChar] = structName[iChar];
		for(u2_t iChar = 0; iChar < NMemberText; ++iChar)
			result.MemberText[iChar] = memberText[iChar];

		cnst SCTTISpan structNameSpan = cttiTrim(result.StructName, 0, (u2_t)NStructName - 1);
		if(false == cttiIdentifier(result.StructName, structNameSpan)) {
			result.Error		= CTTI_PARSE_ERROR_INVALID_STRUCT_NAME;
			result.State		= CTTI_PARSE_STATE_STRUCT_NAME;
			result.ErrorOffset	= cttiIdentifierError(result.StructName, structNameSpan);
			rtrn result;
		}

		result.State = CTTI_PARSE_STATE_MODIFIERS;
		u2_t		cursor			= 0;
		cnst u2_t	textLength		= (u2_t)NMemberText - 1;
		while(true) {
			while(cursor < textLength && cttiSpace(result.MemberText[cursor]))
				++cursor;
			if(cursor == textLength) {
				result.State = CTTI_PARSE_STATE_COMPLETE;
				rtrn result;
			}

			cnst u2_t declarationOffset = cursor;
			cursor = cttiFind(result.MemberText, {cursor, textLength - cursor}, ';');
			if(cursor == textLength) {
				result.Error		= CTTI_PARSE_ERROR_EXPECTED_SEMICOLON;
				result.State		= CTTI_PARSE_STATE_MEMBER_NAME;
				result.ErrorOffset	= cursor;
				rtrn result;
			}

			cnst SCTTISpan segment = cttiTrim(result.MemberText, declarationOffset, cursor - declarationOffset);
			cnst u2_t equalOffset = cttiFind(result.MemberText, segment, '=');
			cnst SCTTISpan declaration = cttiTrim(result.MemberText, segment.Offset, equalOffset - segment.Offset);
			cnst SCTTISpan memberValue = (equalOffset < segment.Offset + segment.Count)
				? cttiTrim(result.MemberText, equalOffset + 1, segment.Offset + segment.Count - equalOffset - 1)
				: SCTTISpan{};

			u2_t nameOffset = declaration.Offset + declaration.Count;
			while(nameOffset > declaration.Offset && cttiNamePart(result.MemberText[nameOffset - 1]))
				--nameOffset;
			cnst SCTTISpan memberName = {nameOffset, declaration.Offset + declaration.Count - nameOffset};
			if(false == cttiIdentifier(result.MemberText, memberName)) {
				result.Error		= CTTI_PARSE_ERROR_INVALID_MEMBER_NAME;
				result.State		= CTTI_PARSE_STATE_MEMBER_NAME;
				result.ErrorOffset	= cttiIdentifierError(result.MemberText, memberName);
				rtrn result;
			}

			cnst SCTTISpan beforeName = cttiTrim(result.MemberText, declaration.Offset, nameOffset - declaration.Offset);
			if(beforeName.empty()) {
				result.Error		= CTTI_PARSE_ERROR_EXPECTED_TYPE;
				result.State		= CTTI_PARSE_STATE_TYPE;
				result.ErrorOffset	= declaration.Offset;
				rtrn result;
			}
			u2_t typeOffset = beforeName.Offset + beforeName.Count;
			while(typeOffset > beforeName.Offset && false == cttiSpace(result.MemberText[typeOffset - 1]))
				--typeOffset;
			cnst SCTTISpan typeName = {typeOffset, beforeName.Offset + beforeName.Count - typeOffset};
			cnst SCTTISpan modifiers = cttiTrim(result.MemberText, beforeName.Offset, typeOffset - beforeName.Offset);
			for(u2_t iChar = 0; iChar < modifiers.Count; ++iChar) {
				cnst sc_t character = result.MemberText[modifiers.Offset + iChar];
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

	enum CTTI_ENUM_PARSE_STATE : u0_t
		{ CTTI_ENUM_PARSE_STATE_ENUM_NAME
		, CTTI_ENUM_PARSE_STATE_UNDERLYING_TYPE
		, CTTI_ENUM_PARSE_STATE_VALUE_NAME
		, CTTI_ENUM_PARSE_STATE_VALUE_INITIALIZER
		, CTTI_ENUM_PARSE_STATE_COMPLETE
		};

	enum CTTI_ENUM_PARSE_ERROR : u0_t
		{ CTTI_ENUM_PARSE_ERROR_NONE
		, CTTI_ENUM_PARSE_ERROR_INVALID_ENUM_NAME
		, CTTI_ENUM_PARSE_ERROR_EXPECTED_UNDERLYING_TYPE
		, CTTI_ENUM_PARSE_ERROR_INVALID_VALUE_NAME
		, CTTI_ENUM_PARSE_ERROR_EXPECTED_VALUE_INITIALIZER
		, CTTI_ENUM_PARSE_ERROR_VALUE_COUNT
		};

	tplt<tpnm TEnum>
	stct SCTTIEnumValue {
		SCTTISpan	Name		= {};
		SCTTISpan	Initializer	= {};
		TEnum		Value		= {};
	};

	tplt<tpnm TEnum, size_t NEnumName, size_t NUnderlyingType, size_t NValueText>
	stct SCTTIEnumParseResult {
		sc_t				EnumName		[NEnumName]		= {};
		sc_t				UnderlyingType	[NUnderlyingType]	= {};
		sc_t				ValueText		[NValueText]		= {};
		SCTTIEnumValue<TEnum>	Values			[NValueText]		= {};
		u2_t				Count						= {};
		u2_t				ErrorOffset					= {};
		CTTI_ENUM_PARSE_STATE	State						= CTTI_ENUM_PARSE_STATE_ENUM_NAME;
		CTTI_ENUM_PARSE_ERROR	Error						= CTTI_ENUM_PARSE_ERROR_NONE;

		inxp bool	Success() cnst nxpt { rtrn CTTI_ENUM_PARSE_ERROR_NONE == Error && CTTI_ENUM_PARSE_STATE_COMPLETE == State; }
		tplt<size_t N>
		inxp s2_t	Find(cnst sc_t (&name)[N]) cnst nxpt {
			for(u2_t iValue = 0; iValue < Count; ++iValue)
				if(cttiSpanEquals(ValueText, Values[iValue].Name, name))
					rtrn (s2_t)iValue;
			rtrn -1;
		}
	};

	tplt<tpnm TEnum, tpnm TUnderlying, size_t NEnumName, size_t NUnderlyingType, size_t NValueText, size_t NValues>
	inxp SCTTIEnumParseResult<TEnum, NEnumName, NUnderlyingType, NValueText> cttiParseEnum
		( cnst sc_t (&enumName)[NEnumName]
		, cnst sc_t (&underlyingType)[NUnderlyingType]
		, cnst sc_t (&valueText)[NValueText]
		, cnst TUnderlying (&explicitValues)[NValues]
		) nxpt {
		SCTTIEnumParseResult<TEnum, NEnumName, NUnderlyingType, NValueText> result = {};
		for(u2_t iChar = 0; iChar < NEnumName; ++iChar)
			result.EnumName[iChar] = enumName[iChar];
		for(u2_t iChar = 0; iChar < NUnderlyingType; ++iChar)
			result.UnderlyingType[iChar] = underlyingType[iChar];
		for(u2_t iChar = 0; iChar < NValueText; ++iChar)
			result.ValueText[iChar] = valueText[iChar];

		cnst SCTTISpan enumNameSpan = cttiTrim(result.EnumName, 0, (u2_t)NEnumName - 1);
		if(false == cttiIdentifier(result.EnumName, enumNameSpan)) {
			result.Error		= CTTI_ENUM_PARSE_ERROR_INVALID_ENUM_NAME;
			result.State		= CTTI_ENUM_PARSE_STATE_ENUM_NAME;
			result.ErrorOffset	= cttiIdentifierError(result.EnumName, enumNameSpan);
			rtrn result;
		}
		cnst SCTTISpan underlyingTypeSpan = cttiTrim(result.UnderlyingType, 0, (u2_t)NUnderlyingType - 1);
		if(underlyingTypeSpan.empty()) {
			result.Error		= CTTI_ENUM_PARSE_ERROR_EXPECTED_UNDERLYING_TYPE;
			result.State		= CTTI_ENUM_PARSE_STATE_UNDERLYING_TYPE;
			rtrn result;
		}

		result.State = CTTI_ENUM_PARSE_STATE_VALUE_NAME;
		u2_t cursor = 0;
		cnst u2_t textLength = (u2_t)NValueText - 1;
		while(true) {
			while(cursor < textLength && cttiSpace(result.ValueText[cursor]))
				++cursor;
			if(cursor == textLength) {
				result.State = CTTI_ENUM_PARSE_STATE_COMPLETE;
				rtrn result;
			}
			if(result.Count >= NValues) {
				result.Error		= CTTI_ENUM_PARSE_ERROR_VALUE_COUNT;
				result.ErrorOffset	= cursor;
				rtrn result;
			}

			cnst u2_t commaOffset = cttiFind(result.ValueText, {cursor, textLength - cursor}, ',');
			cnst SCTTISpan segment = cttiTrim(result.ValueText, cursor, commaOffset - cursor);
			cnst u2_t equalOffset = cttiFind(result.ValueText, segment, '=');
			cnst SCTTISpan valueName = cttiTrim(result.ValueText, segment.Offset, equalOffset - segment.Offset);
			if(false == cttiIdentifier(result.ValueText, valueName)) {
				result.Error		= CTTI_ENUM_PARSE_ERROR_INVALID_VALUE_NAME;
				result.ErrorOffset	= cttiIdentifierError(result.ValueText, valueName);
				rtrn result;
			}

			cnst bool hasInitializer = equalOffset < segment.Offset + segment.Count;
			cnst SCTTISpan initializer = hasInitializer
				? cttiTrim(result.ValueText, equalOffset + 1, segment.Offset + segment.Count - equalOffset - 1)
				: SCTTISpan{};
			if(hasInitializer && initializer.empty()) {
				result.Error		= CTTI_ENUM_PARSE_ERROR_EXPECTED_VALUE_INITIALIZER;
				result.State		= CTTI_ENUM_PARSE_STATE_VALUE_INITIALIZER;
				result.ErrorOffset	= equalOffset + 1;
				rtrn result;
			}

			cnst TUnderlying value = hasInitializer
				? explicitValues[result.Count]
				: result.Count ? TUnderlying(TUnderlying(result.Values[result.Count - 1].Value) + 1) : 0;
			result.Values[result.Count++] = {valueName, initializer, (TEnum)value};
			cursor = commaOffset < textLength ? commaOffset + 1 : textLength;
		}
	}
}

#define CTTI_STCT(structName, members)											\
	stct structName {															\
		members;																\
		stxp SCTTIParseResult<::gpk::size(#structName), ::gpk::size(#members)>	\
			gpk_ctti = ::gpk::cttiParseStruct(#structName, #members);			\
	}

#define GPK_CTTI_CAT_(a, b) a##b
#define GPK_CTTI_CAT(a, b) GPK_CTTI_CAT_(a, b)
#define GPK_CTTI_ARG_COUNT_(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,count,...) count
#define GPK_CTTI_ARG_COUNT(...) GPK_CTTI_ARG_COUNT_I((__VA_ARGS__,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1))
#define GPK_CTTI_ARG_COUNT_I(arguments) GPK_CTTI_ARG_COUNT_ arguments
#define GPK_CTTI_MAP_1(f,t,e0) f(t,e0)
#define GPK_CTTI_MAP_2(f,t,e0,e1) f(t,e0),f(t,e1)
#define GPK_CTTI_MAP_3(f,t,e0,e1,e2) f(t,e0),f(t,e1),f(t,e2)
#define GPK_CTTI_MAP_4(f,t,e0,e1,e2,e3) f(t,e0),f(t,e1),f(t,e2),f(t,e3)
#define GPK_CTTI_MAP_5(f,t,e0,e1,e2,e3,e4) f(t,e0),f(t,e1),f(t,e2),f(t,e3),f(t,e4)
#define GPK_CTTI_MAP_6(f,t,e0,e1,e2,e3,e4,e5) f(t,e0),f(t,e1),f(t,e2),f(t,e3),f(t,e4),f(t,e5)
#define GPK_CTTI_MAP_7(f,t,e0,e1,e2,e3,e4,e5,e6) f(t,e0),f(t,e1),f(t,e2),f(t,e3),f(t,e4),f(t,e5),f(t,e6)
#define GPK_CTTI_MAP_8(f,t,e0,e1,e2,e3,e4,e5,e6,e7) f(t,e0),f(t,e1),f(t,e2),f(t,e3),f(t,e4),f(t,e5),f(t,e6),f(t,e7)
#define GPK_CTTI_MAP_9(f,t,e0,e1,e2,e3,e4,e5,e6,e7,e8) f(t,e0),f(t,e1),f(t,e2),f(t,e3),f(t,e4),f(t,e5),f(t,e6),f(t,e7),f(t,e8)
#define GPK_CTTI_MAP_10(f,t,e0,e1,e2,e3,e4,e5,e6,e7,e8,e9) f(t,e0),f(t,e1),f(t,e2),f(t,e3),f(t,e4),f(t,e5),f(t,e6),f(t,e7),f(t,e8),f(t,e9)
#define GPK_CTTI_MAP_11(f,t,e0,e1,e2,e3,e4,e5,e6,e7,e8,e9,e10) f(t,e0),f(t,e1),f(t,e2),f(t,e3),f(t,e4),f(t,e5),f(t,e6),f(t,e7),f(t,e8),f(t,e9),f(t,e10)
#define GPK_CTTI_MAP_12(f,t,e0,e1,e2,e3,e4,e5,e6,e7,e8,e9,e10,e11) f(t,e0),f(t,e1),f(t,e2),f(t,e3),f(t,e4),f(t,e5),f(t,e6),f(t,e7),f(t,e8),f(t,e9),f(t,e10),f(t,e11)
#define GPK_CTTI_MAP_13(f,t,e0,e1,e2,e3,e4,e5,e6,e7,e8,e9,e10,e11,e12) f(t,e0),f(t,e1),f(t,e2),f(t,e3),f(t,e4),f(t,e5),f(t,e6),f(t,e7),f(t,e8),f(t,e9),f(t,e10),f(t,e11),f(t,e12)
#define GPK_CTTI_MAP_14(f,t,e0,e1,e2,e3,e4,e5,e6,e7,e8,e9,e10,e11,e12,e13) f(t,e0),f(t,e1),f(t,e2),f(t,e3),f(t,e4),f(t,e5),f(t,e6),f(t,e7),f(t,e8),f(t,e9),f(t,e10),f(t,e11),f(t,e12),f(t,e13)
#define GPK_CTTI_MAP_15(f,t,e0,e1,e2,e3,e4,e5,e6,e7,e8,e9,e10,e11,e12,e13,e14) f(t,e0),f(t,e1),f(t,e2),f(t,e3),f(t,e4),f(t,e5),f(t,e6),f(t,e7),f(t,e8),f(t,e9),f(t,e10),f(t,e11),f(t,e12),f(t,e13),f(t,e14)
#define GPK_CTTI_MAP_16(f,t,e0,e1,e2,e3,e4,e5,e6,e7,e8,e9,e10,e11,e12,e13,e14,e15) f(t,e0),f(t,e1),f(t,e2),f(t,e3),f(t,e4),f(t,e5),f(t,e6),f(t,e7),f(t,e8),f(t,e9),f(t,e10),f(t,e11),f(t,e12),f(t,e13),f(t,e14),f(t,e15)
#define GPK_CTTI_MAP_CALL_1(arguments) GPK_CTTI_MAP_1 arguments
#define GPK_CTTI_MAP_CALL_2(arguments) GPK_CTTI_MAP_2 arguments
#define GPK_CTTI_MAP_CALL_3(arguments) GPK_CTTI_MAP_3 arguments
#define GPK_CTTI_MAP_CALL_4(arguments) GPK_CTTI_MAP_4 arguments
#define GPK_CTTI_MAP_CALL_5(arguments) GPK_CTTI_MAP_5 arguments
#define GPK_CTTI_MAP_CALL_6(arguments) GPK_CTTI_MAP_6 arguments
#define GPK_CTTI_MAP_CALL_7(arguments) GPK_CTTI_MAP_7 arguments
#define GPK_CTTI_MAP_CALL_8(arguments) GPK_CTTI_MAP_8 arguments
#define GPK_CTTI_MAP_CALL_9(arguments) GPK_CTTI_MAP_9 arguments
#define GPK_CTTI_MAP_CALL_10(arguments) GPK_CTTI_MAP_10 arguments
#define GPK_CTTI_MAP_CALL_11(arguments) GPK_CTTI_MAP_11 arguments
#define GPK_CTTI_MAP_CALL_12(arguments) GPK_CTTI_MAP_12 arguments
#define GPK_CTTI_MAP_CALL_13(arguments) GPK_CTTI_MAP_13 arguments
#define GPK_CTTI_MAP_CALL_14(arguments) GPK_CTTI_MAP_14 arguments
#define GPK_CTTI_MAP_CALL_15(arguments) GPK_CTTI_MAP_15 arguments
#define GPK_CTTI_MAP_CALL_16(arguments) GPK_CTTI_MAP_16 arguments
#define GPK_CTTI_MAP_DISPATCH(count, arguments) GPK_CTTI_MAP_DISPATCH_I(count, arguments)
#define GPK_CTTI_MAP_DISPATCH_I(count, arguments) GPK_CTTI_CAT(GPK_CTTI_MAP_CALL_, count)(arguments)

#define GPK_CTTI_ENUM_VALUE(IntType, entry) []() cxpr { stct SValue { IntType entry; }; rtrn ::gpk::cttiFirstMember(SValue{}); }()
#define CTTI_ENUM(EnumName, IntType, ...)															\
	enum EnumName : IntType { __VA_ARGS__ };														\
	stxp IntType EnumName##_gpk_ctti_values[] = {GPK_CTTI_MAP_DISPATCH(GPK_CTTI_ARG_COUNT(__VA_ARGS__), (GPK_CTTI_ENUM_VALUE, IntType, __VA_ARGS__))};	\
	stxp auto EnumName##_gpk_ctti = ::gpk::cttiParseEnum<EnumName>(#EnumName, #IntType, #__VA_ARGS__, EnumName##_gpk_ctti_values)

#endif // GPK_CTTI_H
