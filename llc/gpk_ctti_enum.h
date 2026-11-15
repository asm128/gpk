#include "gpk_ctti_parse.h"
#include "gpk_enum.h"

#ifndef GPK_CTTI_ENUM_H
#define GPK_CTTI_ENUM_H

nmsp gpk
{
	enum CTTI_ENUM_PARSE_STATE : u0_t
		{ CTTI_ENUM_PARSE_STATE_ENUM_NAME			= 0
		, CTTI_ENUM_PARSE_STATE_UNDERLYING_TYPE		= 1
		, CTTI_ENUM_PARSE_STATE_VALUE_NAME			= 2
		, CTTI_ENUM_PARSE_STATE_VALUE_INITIALIZER	= 3
		, CTTI_ENUM_PARSE_STATE_COMPLETE				= 4
		};

	enum CTTI_ENUM_PARSE_ERROR : u0_t
		{ CTTI_ENUM_PARSE_ERROR_NONE						= 0
		, CTTI_ENUM_PARSE_ERROR_INVALID_ENUM_NAME			= 1
		, CTTI_ENUM_PARSE_ERROR_EXPECTED_OPEN_BRACE			= 2
		, CTTI_ENUM_PARSE_ERROR_EXPECTED_CLOSE_BRACE			= 3
		, CTTI_ENUM_PARSE_ERROR_EXPECTED_UNDERLYING_TYPE		= 4
		, CTTI_ENUM_PARSE_ERROR_INVALID_VALUE_NAME			= 5
		, CTTI_ENUM_PARSE_ERROR_EXPECTED_VALUE_INITIALIZER	= 6
		, CTTI_ENUM_PARSE_ERROR_VALUE_COUNT					= 7
		};

	tplt<tpnm TEnum>
	stct SCTTIEnumValue {
		SCTTISpan	Name		= {};
		SCTTISpan	Initializer	= {};
		TEnum		Value		= {};
		b8_t		ValueResolved	= false;
	};
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
		b8_t					ValuesCompiled					= false;
		CTTI_ENUM_PARSE_STATE	State							= CTTI_ENUM_PARSE_STATE_ENUM_NAME;
		CTTI_ENUM_PARSE_ERROR	Error							= CTTI_ENUM_PARSE_ERROR_NONE;

		inxp b8_t	Success() cnst nxpt { rtrn CTTI_ENUM_PARSE_ERROR_NONE == Error && CTTI_ENUM_PARSE_STATE_COMPLETE == State; }
		tplt<size_t N>
		inxp s2_t	Find(sc_c (&name)[N]) cnst nxpt {
			for(u2_t iValue = 0; iValue < Count; ++iValue)
				if(cttiSpanEquals(SourceText, Values[iValue].Name, name))
					rtrn (s2_t)iValue;
			rtrn -1;
		}
	};

	tplt<tpnm TEnum, size_t NDeclaration>
	inxp SCTTIEnumParseResult<TEnum, NDeclaration> cttiParseEnum(sc_c (&declaration)[NDeclaration]) nxpt {
		SCTTIEnumParseResult<TEnum, NDeclaration> result = {};
		for(u2_t iChar = 0; iChar < NDeclaration; ++iChar)
			result.SourceText[iChar] = declaration[iChar];

		SCTTISpan sourceSpan = cttiTrim(result.SourceText, 0, (u2_t)NDeclaration - 1);
		if(sourceSpan.Count && ';' == result.SourceText[sourceSpan.Offset + sourceSpan.Count - 1])
			sourceSpan = cttiTrim(result.SourceText, sourceSpan.Offset, sourceSpan.Count - 1);
		u2_c openBrace = cttiFind(result.SourceText, sourceSpan, '{');
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

		SCTTISpan header = cttiTrim(result.SourceText, sourceSpan.Offset, openBrace - sourceSpan.Offset);
		if(header.Count > 4
		&& 'e' == result.SourceText[header.Offset + 0]
		&& 'n' == result.SourceText[header.Offset + 1]
		&& 'u' == result.SourceText[header.Offset + 2]
		&& 'm' == result.SourceText[header.Offset + 3]
		&& cttiSpace(result.SourceText[header.Offset + 4]))
			header = cttiTrim(result.SourceText, header.Offset + 4, header.Count - 4);
		u2_c typeOffset = cttiFind(result.SourceText, header, ':');
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
		u2_c textLength = sourceSpan.Offset + sourceSpan.Count - 1;
		while(true) {
			while(cursor < textLength && cttiSpace(result.SourceText[cursor]))
				++cursor;
			if(cursor == textLength) {
				result.State = CTTI_ENUM_PARSE_STATE_COMPLETE;
				rtrn result;
			}
			u2_c commaOffset = cttiFind(result.SourceText, {cursor, textLength - cursor}, ',');
			cnst SCTTISpan segment = cttiTrim(result.SourceText, cursor, commaOffset - cursor);
			u2_c equalOffset = cttiFind(result.SourceText, segment, '=');
			cnst SCTTISpan valueName = cttiTrim(result.SourceText, segment.Offset, equalOffset - segment.Offset);
			if(false == cttiIdentifier(result.SourceText, valueName)) {
				result.Error		= CTTI_ENUM_PARSE_ERROR_INVALID_VALUE_NAME;
				result.ErrorOffset	= cttiIdentifierError(result.SourceText, valueName);
				rtrn result;
			}

			b8_c hasInitializer = equalOffset < segment.Offset + segment.Count;
			cnst SCTTISpan initializer = hasInitializer
				? cttiTrim(result.SourceText, equalOffset + 1, segment.Offset + segment.Count - equalOffset - 1)
				: SCTTISpan{};
			if(hasInitializer && initializer.empty()) {
				result.Error		= CTTI_ENUM_PARSE_ERROR_EXPECTED_VALUE_INITIALIZER;
				result.State		= CTTI_ENUM_PARSE_STATE_VALUE_INITIALIZER;
				result.ErrorOffset	= equalOffset + 1;
				rtrn result;
			}

			s3_t initializerValue = {};
			b8_c valueExplicit = false == initializer.empty() && cttiInteger(result.SourceText, initializer, initializerValue);
			b8_c valueResolved = valueExplicit || (initializer.empty() && (0 == result.Count || result.Values[result.Count - 1].ValueResolved));
			cnst TEnum value = valueExplicit ? (TEnum)initializerValue
				: valueResolved ? result.Count ? (TEnum)((s3_t)result.Values[result.Count - 1].Value + 1) : (TEnum)0
				: TEnum{};
			result.Values[result.Count++] = {valueName, initializer, value, valueResolved};
			cursor = commaOffset < textLength ? commaOffset + 1 : textLength;
		}
	}

	tplt<tpnm TEnum, tpnm TUnderlying, size_t NDeclaration, size_t NValues>
	inxp SCTTIEnumParseResult<TEnum, NDeclaration> cttiParseEnum
		( sc_c (&declaration)[NDeclaration]
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

	stct SCTTIEnumDeclaration;
	err_t cttiEnumDeclarationRegister(SCTTIEnumDeclaration & declaration) nxpt;

	stct SCTTIEnumDeclaration {
		vcst_t					File			= {};
		u2_t					Line			= {};
		vcst_t					SourceText		= {};
		vcst_t					Name			= {};
		vcst_t					UnderlyingType	= {};
		cnst SCTTIEnumValue<s3_t> *	Values			= {};
		cnst vcst_t *				Names			= {};
		cnst vcst_t *				Initializers	= {};
		u2_t					Count			= {};
		SCTTIEnumDeclaration *		Next			= {};

		tplt<size_t NDeclaration>
		SCTTIEnumDeclaration
			( cnst vcst_t file
			, u2_c line
			, cnst SCTTIEnumParseResult<s3_t, NDeclaration> & metadata
			, cnst SCTTIEnumViews<NDeclaration> & views
			) nxpt
			: File			(file)
			, Line			(line)
			, SourceText		{metadata.SourceText, NDeclaration - 1}
			, Name			{&metadata.SourceText[metadata.EnumName.Offset], metadata.EnumName.Count}
			, UnderlyingType	{&metadata.SourceText[metadata.UnderlyingType.Offset], metadata.UnderlyingType.Count}
			, Values			(metadata.Values)
			, Names			(views.Names)
			, Initializers	(views.Initializers)
			, Count			(metadata.Count)
			{ cttiEnumDeclarationRegister(*this); }
	};

	inln SCTTIEnumDeclaration * & cttiEnumDeclarationRoot() nxpt {
		sttc SCTTIEnumDeclaration * root = {};
		rtrn root;
	}

	inln err_t cttiEnumDeclarationRegister(SCTTIEnumDeclaration & declaration) nxpt {
		SCTTIEnumDeclaration * & root = cttiEnumDeclarationRoot();
		for(SCTTIEnumDeclaration * current = root; current; current = current->Next)
			if(current->File == declaration.File && current->Line == declaration.Line && current->Name == declaration.Name)
				rtrn 0;
		declaration.Next = root;
		root = &declaration;
		rtrn 0;
	}

	inln cnst SCTTIEnumDeclaration * cttiEnum(cnst vcst_t name) nxpt {
		for(cnst SCTTIEnumDeclaration * current = cttiEnumDeclarationRoot(); current; current = current->Next)
			if(current->Name == name)
				rtrn current;
		rtrn {};
	}

	tplt<tpnm T>
	stct SCTTIEnumRegistry;

	tplt<tpnm T>
	inxp cnst auto & cttiEnum() nxpt { rtrn SCTTIEnumRegistry<T>::Value; }

	tplt<tpnm T>
	inxp cnst auto & cttiEnumViews() nxpt { rtrn SCTTIEnumRegistry<T>::Views; }

	tplt<tpnm TEnum, size_t NDeclaration>
	err_t enumDefinitionFromCTTI(cnst SCTTIEnumParseResult<TEnum, NDeclaration> & metadata) {
		if(false == metadata.Success())
			rtrn -1;
		for(u2_t iValue = 0; iValue < metadata.Count; ++iValue)
			if(false == metadata.Values[iValue].ValueResolved)
				rtrn -1;

		enum_definition<TEnum> & definition = get_enum<TEnum>();
		if(definition.Names.size()) {
			if(definition.Names.size() != metadata.Count || definition.Values.size() != metadata.Count)
				rtrn -1;
			for(u2_t iValue = 0; iValue < metadata.Count; ++iValue) {
				cnst SCTTIEnumValue<TEnum> & value = metadata.Values[iValue];
				vcst_t name = {&metadata.SourceText[value.Name.Offset], value.Name.Count};
				if(name.size() > metadata.EnumName.Count + 1
				&& name[metadata.EnumName.Count] == '_')
					name = {name.begin() + metadata.EnumName.Count + 1, name.size() - metadata.EnumName.Count - 1};
				if(definition.Values[iValue] != value.Value || definition.Names[iValue] != name)
					rtrn -1;
			}
			rtrn definition.Values.size();
		}
		definition.Name = {&metadata.SourceText[metadata.EnumName.Offset], metadata.EnumName.Count};
		for(u2_t iValue = 0; iValue < metadata.Count; ++iValue) {
			cnst SCTTIEnumValue<TEnum> & value = metadata.Values[iValue];
			vcst_t name = {&metadata.SourceText[value.Name.Offset], value.Name.Count};
			if(name.size() > metadata.EnumName.Count + 1
			&& name[metadata.EnumName.Count] == '_')
				name = {name.begin() + metadata.EnumName.Count + 1, name.size() - metadata.EnumName.Count - 1};
			if_fail_fe(definition.add_value(value.Value, name, name, name));
		}
		rtrn definition.Values.size();
	}
} // nmsp

#define GPK_CTTI_JOIN_2(left, right)	left##right
#define GPK_CTTI_JOIN(left, right)	GPK_CTTI_JOIN_2(left, right)
#define GDEFINE_ENUM_CTTI(cttiObject) \
	sttc cnst ::gpk::err_t GPK_CTTI_JOIN(gpk_ctti_enum_definition_, __LINE__) = \
		::gpk::enumDefinitionFromCTTI(cttiObject)

#define CTTI_ENUM(type, ...) \
	__VA_ARGS__ \
	GDEFINE_ENUM_OPERATORS(type); \
	stct type##_gpk_ctti_storage { \
		sinx auto Value = ::gpk::cttiParseEnum<::gpk::s3_t>(#__VA_ARGS__); \
		sinx auto TypedValue = ::gpk::cttiParseEnum<type>(#__VA_ARGS__); \
		sinx auto Views = ::gpk::cttiEnumViewsMake(Value); \
	}; \
	sttc ::gpk::SCTTIEnumDeclaration type##_gpk_ctti_declaration = \
		{__FILE__, __LINE__, type##_gpk_ctti_storage::Value, type##_gpk_ctti_storage::Views}; \
	sttc cnst ::gpk::err_t type##_gpk_enum_definition = \
		::gpk::enumDefinitionFromCTTI(type##_gpk_ctti_storage::TypedValue)

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
#define CTTI_ENUM_COMPILED(EnumName, IntType, ...) \
	enum EnumName : IntType { __VA_ARGS__ }; \
	stxp IntType EnumName##_gpk_ctti_values[] = {GPK_CTTI_ENUM_VALUES(GPK_CTTI_ENUM_VALUE, IntType, __VA_ARGS__)}; \
	stxp auto EnumName##_gpk_ctti = ::gpk::cttiParseEnum<EnumName>(#EnumName " : " #IntType " { " #__VA_ARGS__ " }", EnumName##_gpk_ctti_values); \
	stxp auto EnumName##_gpk_ctti_views = ::gpk::cttiEnumViewsMake(EnumName##_gpk_ctti); \
	stxp ::gpk::vcst_t EnumName##_gpk_ctti_file = __FILE__; \
	stxp ::gpk::u2_t EnumName##_gpk_ctti_line = __LINE__; \
	stxp ::gpk::uP_t EnumName##_gpk_ctti_size = sizeof(EnumName); \
	stxp b8_t EnumName##_gpk_ctti_is_pod = __is_pod(EnumName)

#endif // GPK_CTTI_ENUM_H
