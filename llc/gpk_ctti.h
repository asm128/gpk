#include "gpk_enum.h"

#ifndef GPK_CTTI_H
#define GPK_CTTI_H

namespace gpk
{

	inxp bool cttiSpace		(cnst sc_t character) nxpt { rtrn ' ' == character || '\t' == character || '\r' == character || '\n' == character; }
	inxp bool cttiNameStart	(cnst sc_t character) nxpt { rtrn '_' == character || (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z'); }
	inxp bool cttiNamePart	(cnst sc_t character) nxpt { rtrn cttiNameStart(character) || (character >= '0' && character <= '9'); }
	tplt<tpnm T>
	inxp auto cttiFirstMember(cnst T & source) nxpt {
		cnst auto & [result] = source;
		rtrn result;
	}

	GDEFINE_ENUM_TYPE (CTTI_PARSE_STATE, u0_t);
	GDEFINE_ENUM_VALUE(CTTI_PARSE_STATE, STRUCT_NAME	, 0);
	GDEFINE_ENUM_VALUE(CTTI_PARSE_STATE, MODIFIERS		, 1);
	GDEFINE_ENUM_VALUE(CTTI_PARSE_STATE, TYPE			, 2);
	GDEFINE_ENUM_VALUE(CTTI_PARSE_STATE, MEMBER_NAME	, 3);
	GDEFINE_ENUM_VALUE(CTTI_PARSE_STATE, MEMBER_VALUE	, 4);
	GDEFINE_ENUM_VALUE(CTTI_PARSE_STATE, COMPLETE		, 5);

	GDEFINE_ENUM_TYPE (CTTI_PARSE_ERROR,  u0_t);
	GDEFINE_ENUM_VALUE(CTTI_PARSE_ERROR, NONE						, 0);	
	GDEFINE_ENUM_VALUE(CTTI_PARSE_ERROR, INVALID_STRUCT_NAME		, 1);					
	GDEFINE_ENUM_VALUE(CTTI_PARSE_ERROR, EXPECTED_OPEN_BRACE		, 2);					
	GDEFINE_ENUM_VALUE(CTTI_PARSE_ERROR, EXPECTED_CLOSE_BRACE		, 3);					
	GDEFINE_ENUM_VALUE(CTTI_PARSE_ERROR, INVALID_MODIFIER			, 4);				
	GDEFINE_ENUM_VALUE(CTTI_PARSE_ERROR, EXPECTED_TYPE				, 5);			
	GDEFINE_ENUM_VALUE(CTTI_PARSE_ERROR, EXPECTED_MEMBER_NAME		, 6);					
	GDEFINE_ENUM_VALUE(CTTI_PARSE_ERROR, INVALID_MEMBER_NAME		, 7);					
	GDEFINE_ENUM_VALUE(CTTI_PARSE_ERROR, EXPECTED_SEMICOLON			, 8);				

	GDEFINE_ENUM_TYPE(CTTI_ENUM_PARSE_STATE,  u0_t);
	GDEFINE_ENUM_VALUE(CTTI_ENUM_PARSE_STATE, ENUM_NAME				, 0);		
	GDEFINE_ENUM_VALUE(CTTI_ENUM_PARSE_STATE, UNDERLYING_TYPE		, 1);				
	GDEFINE_ENUM_VALUE(CTTI_ENUM_PARSE_STATE, VALUE_NAME			, 2);			
	GDEFINE_ENUM_VALUE(CTTI_ENUM_PARSE_STATE, VALUE_INITIALIZER		, 3);				
	GDEFINE_ENUM_VALUE(CTTI_ENUM_PARSE_STATE, COMPLETE				, 4);

	GDEFINE_ENUM_TYPE(CTTI_ENUM_PARSE_ERROR, u0_t);
	GDEFINE_ENUM_VALUE(CTTI_ENUM_PARSE_ERROR, NONE							, 0);
	GDEFINE_ENUM_VALUE(CTTI_ENUM_PARSE_ERROR, INVALID_ENUM_NAME				, 1);
	GDEFINE_ENUM_VALUE(CTTI_ENUM_PARSE_ERROR, EXPECTED_OPEN_BRACE			, 2);
	GDEFINE_ENUM_VALUE(CTTI_ENUM_PARSE_ERROR, EXPECTED_CLOSE_BRACE			, 3);
	GDEFINE_ENUM_VALUE(CTTI_ENUM_PARSE_ERROR, EXPECTED_UNDERLYING_TYPE		, 4);
	GDEFINE_ENUM_VALUE(CTTI_ENUM_PARSE_ERROR, INVALID_VALUE_NAME			, 5);
	GDEFINE_ENUM_VALUE(CTTI_ENUM_PARSE_ERROR, EXPECTED_VALUE_INITIALIZER	, 6);
	GDEFINE_ENUM_VALUE(CTTI_ENUM_PARSE_ERROR, VALUE_COUNT					, 7);

	stct SCTTISpan {
		u2_t		Offset	= {};
		u2_t		Count	= {};
		inxp bool	empty	() cnst nxpt { rtrn 0 == Count; }
	};

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

	stct SCTTIMember {
		SCTTISpan	Modifiers	= {};
		SCTTISpan	TypeName	= {};
		SCTTISpan	Name		= {};
		SCTTISpan	Value		= {};
	};

	stct SCTTIValue {
		vu0_t		Data		= {};
		vcst_t		Type		= {};
		vcst_t		Name		= {};
	};

	tplt<tpnm TEnum>
	stct SCTTIEnumValue {
		SCTTISpan	Name		= {};
		SCTTISpan	Initializer	= {};
		TEnum		Value		= {};
		bool		ValueResolved	= false;
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
		inxp vcst_t	View		(cnst SCTTISpan span) cnst nxpt { rtrn {&SourceText[span.Offset], span.Count}; }
		inxp err_t	Find		(cnst vcst_t name) cnst nxpt {
			for(u2_t iMember = 0; iMember < Count; ++iMember)
				if(View(Members[iMember].Name) == name)
					rtrn iMember;
			rtrn -1;
		}
	};

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

	tplt<tpnm T, size_t N>
	stct SCTTIMemberAccess;

	tplt<tpnm T>
	stct SCTTIMemberAccess<T, 1> {
		tplt<tpnm TVisitor>
		sttc err_t Visit(T & instance, cnst u2_t index, TVisitor & visitor) {
			auto & [member0] = instance;
			rtrn 0 == index ? visitor(member0) : -1;
		}
	};

	tplt<tpnm T>
	stct SCTTIMemberAccess<T, 2> {
		tplt<tpnm TVisitor>
		sttc err_t Visit(T & instance, cnst u2_t index, TVisitor & visitor) {
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
		sttc err_t Visit(T & instance, cnst u2_t index, TVisitor & visitor) {
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
		sttc err_t Visit(T & instance, cnst u2_t index, TVisitor & visitor) {
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
	stct SCTTIEnumViews {
		vcst_t	Names			[NDeclaration]	= {};
		vcst_t	Initializers	[NDeclaration]	= {};
		u2_t	Count							= {};
	};

	tplt<tpnm TEnum, size_t NDeclaration>
	stct SCTTIEnumParseResult {
		sc_t					SourceText		[NDeclaration]	= {};
		SCTTISpan				EnumName						= {};
		SCTTISpan				UnderlyingType					= {};
		SCTTIEnumValue<TEnum>	Values			[NDeclaration]	= {};
		u2_t					Count							= {};
		u2_t					ErrorOffset						= {};
		bool					ValuesCompiled					= false;
		CTTI_ENUM_PARSE_STATE	State							= CTTI_ENUM_PARSE_STATE_ENUM_NAME;
		CTTI_ENUM_PARSE_ERROR	Error							= CTTI_ENUM_PARSE_ERROR_NONE;

		inxp bool	Success() cnst nxpt { rtrn CTTI_ENUM_PARSE_ERROR_NONE == Error && CTTI_ENUM_PARSE_STATE_COMPLETE == State; }
		tplt<size_t N>
		inxp s2_t	Find(cnst sc_t (&name)[N]) cnst nxpt {
			for(u2_t iValue = 0; iValue < Count; ++iValue)
				if(cttiSpanEquals(SourceText, Values[iValue].Name, name))
					rtrn (s2_t)iValue;
			rtrn -1;
		}
	};

	tplt<tpnm TEnum, size_t NDeclaration>
	inxp SCTTIEnumParseResult<TEnum, NDeclaration> cttiParseEnum(cnst sc_t (&declaration)[NDeclaration]) nxpt {
		SCTTIEnumParseResult<TEnum, NDeclaration> result = {};
		for(u2_t iChar = 0; iChar < NDeclaration; ++iChar)
			result.SourceText[iChar] = declaration[iChar];

		SCTTISpan sourceSpan = cttiTrim(result.SourceText, 0, (u2_t)NDeclaration - 1);
		if(sourceSpan.Count && ';' == result.SourceText[sourceSpan.Offset + sourceSpan.Count - 1])
			sourceSpan = cttiTrim(result.SourceText, sourceSpan.Offset, sourceSpan.Count - 1);
		cnst u2_t openBrace = cttiFind(result.SourceText, sourceSpan, '{');
		if(openBrace == sourceSpan.Offset + sourceSpan.Count) {
			result.Error		= CTTI_ENUM_PARSE_ERROR_EXPECTED_OPEN_BRACE;
			result.ErrorOffset	= openBrace;
			rtrn result;
		}
		if(0 == sourceSpan.Count || '}' != result.SourceText[sourceSpan.Offset + sourceSpan.Count - 1]) {
			result.Error		= CTTI_ENUM_PARSE_ERROR_EXPECTED_CLOSE_BRACE;
			result.State		= CTTI_ENUM_PARSE_STATE_VALUE_NAME;
			result.ErrorOffset	= sourceSpan.Offset + sourceSpan.Count;
			rtrn result;
		}

		cnst SCTTISpan header = cttiTrim(result.SourceText, sourceSpan.Offset, openBrace - sourceSpan.Offset);
		cnst u2_t typeOffset = cttiFind(result.SourceText, header, ':');
		result.EnumName = cttiTrim(result.SourceText, header.Offset, typeOffset - header.Offset);
		if(false == cttiIdentifier(result.SourceText, result.EnumName)) {
			result.Error		= CTTI_ENUM_PARSE_ERROR_INVALID_ENUM_NAME;
			result.State		= CTTI_ENUM_PARSE_STATE_ENUM_NAME;
			result.ErrorOffset	= cttiIdentifierError(result.SourceText, result.EnumName);
			rtrn result;
		}
		result.UnderlyingType = typeOffset < header.Offset + header.Count
			? cttiTrim(result.SourceText, typeOffset + 1, header.Offset + header.Count - typeOffset - 1)
			: SCTTISpan{};
		if(result.UnderlyingType.empty()) {
			result.Error		= CTTI_ENUM_PARSE_ERROR_EXPECTED_UNDERLYING_TYPE;
			result.State		= CTTI_ENUM_PARSE_STATE_UNDERLYING_TYPE;
			result.ErrorOffset	= typeOffset;
			rtrn result;
		}

		result.State = CTTI_ENUM_PARSE_STATE_VALUE_NAME;
		u2_t cursor = openBrace + 1;
		cnst u2_t textLength = sourceSpan.Offset + sourceSpan.Count - 1;
		while(true) {
			while(cursor < textLength && cttiSpace(result.SourceText[cursor]))
				++cursor;
			if(cursor == textLength) {
				result.State = CTTI_ENUM_PARSE_STATE_COMPLETE;
				rtrn result;
			}
			cnst u2_t commaOffset = cttiFind(result.SourceText, {cursor, textLength - cursor}, ',');
			cnst SCTTISpan segment = cttiTrim(result.SourceText, cursor, commaOffset - cursor);
			cnst u2_t equalOffset = cttiFind(result.SourceText, segment, '=');
			cnst SCTTISpan valueName = cttiTrim(result.SourceText, segment.Offset, equalOffset - segment.Offset);
			if(false == cttiIdentifier(result.SourceText, valueName)) {
				result.Error		= CTTI_ENUM_PARSE_ERROR_INVALID_VALUE_NAME;
				result.ErrorOffset	= cttiIdentifierError(result.SourceText, valueName);
				rtrn result;
			}

			cnst bool hasInitializer = equalOffset < segment.Offset + segment.Count;
			cnst SCTTISpan initializer = hasInitializer
				? cttiTrim(result.SourceText, equalOffset + 1, segment.Offset + segment.Count - equalOffset - 1)
				: SCTTISpan{};
			if(hasInitializer && initializer.empty()) {
				result.Error		= CTTI_ENUM_PARSE_ERROR_EXPECTED_VALUE_INITIALIZER;
				result.State		= CTTI_ENUM_PARSE_STATE_VALUE_INITIALIZER;
				result.ErrorOffset	= equalOffset + 1;
				rtrn result;
			}

			cnst bool valueResolved = initializer.empty() && (0 == result.Count || result.Values[result.Count - 1].ValueResolved);
			cnst TEnum value = valueResolved
				? result.Count ? (TEnum)(result.Values[result.Count - 1].Value + 1) : (TEnum)0
				: TEnum{};
			result.Values[result.Count++] = {valueName, initializer, value, valueResolved};
			cursor = commaOffset < textLength ? commaOffset + 1 : textLength;
		}
	}

	tplt<tpnm TEnum, tpnm TUnderlying, size_t NDeclaration, size_t NValues>
	inxp SCTTIEnumParseResult<TEnum, NDeclaration> cttiParseEnum
		( cnst sc_t (&declaration)[NDeclaration]
		, cnst TUnderlying (&explicitValues)[NValues]
		) nxpt {
		SCTTIEnumParseResult<TEnum, NDeclaration> result = cttiParseEnum<TEnum>(declaration);
		if(false == result.Success())
			rtrn result;
		if(result.Count != NValues) {
			result.Error		= CTTI_ENUM_PARSE_ERROR_VALUE_COUNT;
			result.ErrorOffset	= result.EnumName.Offset;
			rtrn result;
		}
		for(u2_t iValue = 0; iValue < result.Count; ++iValue) {
			cnst TUnderlying value = result.Values[iValue].Initializer.empty()
				? iValue ? TUnderlying(TUnderlying(result.Values[iValue - 1].Value) + 1) : 0
				: explicitValues[iValue];
			result.Values[iValue].Value = (TEnum)value;
			result.Values[iValue].ValueResolved = true;
		}
		result.ValuesCompiled = true;
		rtrn result;
	}

	tplt<tpnm TEnum, size_t NDeclaration>
	err_t enumDefinitionFromCTTI(cnst SCTTIEnumParseResult<TEnum, NDeclaration> & metadata) {
		if(false == metadata.Success())
			rtrn -1;
		for(u2_t iValue = 0; iValue < metadata.Count; ++iValue)
			if(false == metadata.Values[iValue].ValueResolved)
				rtrn -1;

		enum_definition<TEnum> & definition = get_enum<TEnum>();
		definition.Name = {&metadata.SourceText[metadata.EnumName.Offset], metadata.EnumName.Count};
		for(u2_t iValue = 0; iValue < metadata.Count; ++iValue) {
			cnst SCTTIEnumValue<TEnum> & value = metadata.Values[iValue];
			cnst vcst_t name = {&metadata.SourceText[value.Name.Offset], value.Name.Count};
			if_fail_fe(definition.add_value(value.Value, name, name, name));
		}
		rtrn definition.Values.size();
	}

	tplt<tpnm TEnum, size_t NDeclaration>
	inxp SCTTIEnumViews<NDeclaration> cttiEnumViewsMake(cnst SCTTIEnumParseResult<TEnum, NDeclaration> & metadata) nxpt {
		SCTTIEnumViews<NDeclaration> result = {};
		result.Count = metadata.Count;
		for(u2_t iValue = 0; iValue < metadata.Count; ++iValue) {
			cnst SCTTIEnumValue<TEnum> & value = metadata.Values[iValue];
			result.Names[iValue] = {&metadata.SourceText[value.Name.Offset], value.Name.Count};
			result.Initializers[iValue] = {&metadata.SourceText[value.Initializer.Offset], value.Initializer.Count};
		}
		rtrn result;
	}

	tplt<tpnm T>
	stct SCTTIEnumRegistry;

	tplt<tpnm T>
	inxp cnst auto & cttiEnum() nxpt { rtrn SCTTIEnumRegistry<T>::Value; }

	tplt<tpnm T>
	inxp cnst auto & cttiEnumViews() nxpt { rtrn SCTTIEnumRegistry<T>::Views; }
}

#define GPK_CTTI_JOIN_2(left, right)	left##right
#define GPK_CTTI_JOIN(left, right)	GPK_CTTI_JOIN_2(left, right)
#define GDEFINE_ENUM_CTTI(cttiObject)																\
	static cnst ::gpk::err_t GPK_CTTI_JOIN(gpk_ctti_enum_definition_, __LINE__) =						\
		::gpk::enumDefinitionFromCTTI(cttiObject)
#define CTTI_STCT(...)																				\
	extern stct __VA_ARGS__ GPK_CTTI_JOIN(gpk_ctti_struct_tag_, __LINE__);							\
	tplt<> stct gpk::SCTTIStructRegistry<decltype(GPK_CTTI_JOIN(gpk_ctti_struct_tag_, __LINE__))> {	\
		sinx auto Value = ::gpk::cttiParseStruct(#__VA_ARGS__);							\
		sinx ::gpk::vcst_t File = __FILE__;										\
		sinx ::gpk::u2_t Line = __LINE__;										\
		sinx ::gpk::uP_t Size = sizeof(decltype(GPK_CTTI_JOIN(gpk_ctti_struct_tag_, __LINE__)));	\
		sinx bool IsPOD = __is_pod(decltype(GPK_CTTI_JOIN(gpk_ctti_struct_tag_, __LINE__)));	\
	}

#define CTTI_ENUM(...)																				\
	extern enum __VA_ARGS__ GPK_CTTI_JOIN(gpk_ctti_enum_tag_, __LINE__);								\
	tplt<> stct gpk::SCTTIEnumRegistry<decltype(GPK_CTTI_JOIN(gpk_ctti_enum_tag_, __LINE__))> {	\
		sinx auto Value = ::gpk::cttiParseEnum<decltype(GPK_CTTI_JOIN(gpk_ctti_enum_tag_, __LINE__))>(#__VA_ARGS__);\
		sinx auto Views = ::gpk::cttiEnumViewsMake(Value);								\
		sinx ::gpk::vcst_t File = __FILE__;										\
		sinx ::gpk::u2_t Line = __LINE__;										\
		sinx ::gpk::uP_t Size = sizeof(decltype(GPK_CTTI_JOIN(gpk_ctti_enum_tag_, __LINE__)));	\
		sinx bool IsPOD = __is_pod(decltype(GPK_CTTI_JOIN(gpk_ctti_enum_tag_, __LINE__)));	\
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
#define CTTI_ENUM_COMPILED(EnumName, IntType, ...)												\
	enum EnumName : IntType { __VA_ARGS__ };														\
	stxp IntType EnumName##_gpk_ctti_values[] = {GPK_CTTI_ENUM_VALUES(GPK_CTTI_ENUM_VALUE, IntType, __VA_ARGS__)};	\
	stxp auto EnumName##_gpk_ctti = ::gpk::cttiParseEnum<EnumName>(#EnumName " : " #IntType " { " #__VA_ARGS__ " }", EnumName##_gpk_ctti_values);\
	stxp auto EnumName##_gpk_ctti_views = ::gpk::cttiEnumViewsMake(EnumName##_gpk_ctti);				\
	stxp ::gpk::vcst_t EnumName##_gpk_ctti_file = __FILE__;									\
	stxp ::gpk::u2_t EnumName##_gpk_ctti_line = __LINE__;									\
	stxp ::gpk::uP_t EnumName##_gpk_ctti_size = sizeof(EnumName);							\
	stxp bool EnumName##_gpk_ctti_is_pod = __is_pod(EnumName)

#endif // GPK_CTTI_H
