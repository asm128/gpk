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
		, CTTI_PARSE_ERROR_EXPECTED_OPEN_BRACE
		, CTTI_PARSE_ERROR_EXPECTED_CLOSE_BRACE
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

	tplt<size_t NDeclaration>
	stct SCTTIParseResult {
		sc_t				SourceText	[NDeclaration]	= {};
		SCTTISpan			StructName				= {};
		SCTTIMember		Members		[NDeclaration]	= {};
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
	tplt<size_t NDeclaration>
	inxp SCTTIParseResult<NDeclaration> cttiParseStruct(cnst sc_t (&declaration)[NDeclaration]) nxpt {
		static_assert(NDeclaration <= 0xFFFFFFFFULL, "CTTI source text exceeds the supported size.");
		SCTTIParseResult<NDeclaration> result = {};
		for(u2_t iChar = 0; iChar < NDeclaration; ++iChar)
			result.SourceText[iChar] = declaration[iChar];

		SCTTISpan sourceSpan = cttiTrim(result.SourceText, 0, (u2_t)NDeclaration - 1);
		if(sourceSpan.Count && ';' == result.SourceText[sourceSpan.Offset + sourceSpan.Count - 1])
			sourceSpan = cttiTrim(result.SourceText, sourceSpan.Offset, sourceSpan.Count - 1);
		cnst u2_t openBrace = cttiFind(result.SourceText, sourceSpan, '{');
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
		if(false == cttiIdentifier(result.SourceText, result.StructName)) {
			result.Error		= CTTI_PARSE_ERROR_INVALID_STRUCT_NAME;
			result.State		= CTTI_PARSE_STATE_STRUCT_NAME;
			result.ErrorOffset	= cttiIdentifierError(result.SourceText, result.StructName);
			rtrn result;
		}

		result.State = CTTI_PARSE_STATE_MODIFIERS;
		u2_t		cursor			= openBrace + 1;
		cnst u2_t	textLength		= sourceSpan.Offset + sourceSpan.Count - 1;
		while(true) {
			while(cursor < textLength && cttiSpace(result.SourceText[cursor]))
				++cursor;
			if(cursor == textLength) {
				result.State = CTTI_PARSE_STATE_COMPLETE;
				rtrn result;
			}

			cnst u2_t declarationOffset = cursor;
			cursor = cttiFind(result.SourceText, {cursor, textLength - cursor}, ';');
			if(cursor == textLength) {
				result.Error		= CTTI_PARSE_ERROR_EXPECTED_SEMICOLON;
				result.State		= CTTI_PARSE_STATE_MEMBER_NAME;
				result.ErrorOffset	= cursor;
				rtrn result;
			}

			cnst SCTTISpan segment = cttiTrim(result.SourceText, declarationOffset, cursor - declarationOffset);
			cnst u2_t equalOffset = cttiFind(result.SourceText, segment, '=');
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
				cnst sc_t character = result.SourceText[modifiers.Offset + iChar];
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

	tplt<tpnm T>
	stct SCTTIStructRegistry;

	tplt<tpnm T>
	inxp cnst auto & cttiStruct() nxpt { rtrn SCTTIStructRegistry<T>::Value; }

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

#define GPK_CTTI_JOIN_2(left, right)	left##right
#define GPK_CTTI_JOIN(left, right)	GPK_CTTI_JOIN_2(left, right)
#define CTTI_STCT(...)																				\
	extern stct __VA_ARGS__ GPK_CTTI_JOIN(gpk_ctti_struct_tag_, __LINE__);							\
	tplt<> stct gpk::SCTTIStructRegistry<decltype(GPK_CTTI_JOIN(gpk_ctti_struct_tag_, __LINE__))> {	\
		inline static constexpr auto Value = ::gpk::cttiParseStruct(#__VA_ARGS__);							\
	}

#define GPK_CTTI_PARENS ()
#define GPK_CTTI_EXPAND(...) GPK_CTTI_EXPAND_5(GPK_CTTI_EXPAND_5(GPK_CTTI_EXPAND_5(GPK_CTTI_EXPAND_5(__VA_ARGS__))))
#define GPK_CTTI_EXPAND_5(...) GPK_CTTI_EXPAND_4(GPK_CTTI_EXPAND_4(GPK_CTTI_EXPAND_4(GPK_CTTI_EXPAND_4(__VA_ARGS__))))
#define GPK_CTTI_EXPAND_4(...) GPK_CTTI_EXPAND_3(GPK_CTTI_EXPAND_3(GPK_CTTI_EXPAND_3(GPK_CTTI_EXPAND_3(__VA_ARGS__))))
#define GPK_CTTI_EXPAND_3(...) GPK_CTTI_EXPAND_2(GPK_CTTI_EXPAND_2(GPK_CTTI_EXPAND_2(GPK_CTTI_EXPAND_2(__VA_ARGS__))))
#define GPK_CTTI_EXPAND_2(...) GPK_CTTI_EXPAND_1(GPK_CTTI_EXPAND_1(GPK_CTTI_EXPAND_1(GPK_CTTI_EXPAND_1(__VA_ARGS__))))
#define GPK_CTTI_EXPAND_1(...) __VA_ARGS__
#define GPK_CTTI_ENUM_VALUES(function, type, ...) __VA_OPT__(GPK_CTTI_EXPAND(GPK_CTTI_ENUM_VALUES_I(function, type, __VA_ARGS__)))
#define GPK_CTTI_ENUM_VALUES_I(function, type, entry, ...) function(type, entry) __VA_OPT__(, GPK_CTTI_ENUM_VALUES_AGAIN GPK_CTTI_PARENS (function, type, __VA_ARGS__))
#define GPK_CTTI_ENUM_VALUES_AGAIN() GPK_CTTI_ENUM_VALUES_I

#define GPK_CTTI_ENUM_VALUE(IntType, entry) []() cxpr { stct SValue { IntType entry; }; rtrn ::gpk::cttiFirstMember(SValue{}); }()
#define CTTI_ENUM(EnumName, IntType, ...)															\
	enum EnumName : IntType { __VA_ARGS__ };														\
	stxp IntType EnumName##_gpk_ctti_values[] = {GPK_CTTI_ENUM_VALUES(GPK_CTTI_ENUM_VALUE, IntType, __VA_ARGS__)};	\
	stxp auto EnumName##_gpk_ctti = ::gpk::cttiParseEnum<EnumName>(#EnumName, #IntType, #__VA_ARGS__, EnumName##_gpk_ctti_values)

#endif // GPK_CTTI_H
