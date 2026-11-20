#include "gpk_ctti_parse.h"
#include "gpk_enum_operators.h"
#include "gpk_array_static.h"

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

    stct SCTTIEnumReaderState {
        u2_t                   IndexCurrentChar = {};
        CTTI_ENUM_PARSE_STATE  State            = CTTI_ENUM_PARSE_STATE_ENUM_NAME;
    };

    tplt<tpnm TEnum>
    stct SCTTIEnumValue {
        rangeu2_t	Name		= {};
        rangeu2_t	Initializer	= {};
        TEnum		Value		= {};
        b8_t		ValueResolved	= false;
    };
    tplt<size_t NDeclaration>
    stct SCTTIEnumViews {
        asttc<vcst_t, NDeclaration>	Names			= {};
        asttc<vcst_t, NDeclaration>	Initializers	= {};
        u2_t						Count			= {};
    };

    tplt<tpnm TEnum, size_t NDeclaration>
    stct SCTTIEnumParseResult {
        sstring<NDeclaration>	SourceText					= {};
        rangeu2_t				EnumName					= {};
        rangeu2_t				UnderlyingType				= {};
        asttc<SCTTIEnumValue<TEnum>, NDeclaration>	Values	= {};
        u2_t					Count						= {};
        b8_t					ValuesCompiled				= false;
        SCTTIEnumReaderState   StateRead                   = {};
        inxp b8_t	            Success() cnst nxpt { rtrn CTTI_ENUM_PARSE_STATE_COMPLETE == StateRead.State; }
    };

    tplt<tpnm TEnum, size_t NDeclaration>
    inxp SCTTIEnumParseResult<TEnum, NDeclaration> cttiParseEnum(sc_c (&declaration)[NDeclaration]) nxpt {
        SCTTIEnumParseResult<TEnum, NDeclaration> result = {};
        SCTTIEnumReaderState & state = result.StateRead;
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
            state.State = CTTI_ENUM_PARSE_STATE_VALUE_NAME;
            state.IndexCurrentChar = sourceSpan.Offset + sourceSpan.Count;
            rtrn result;
        }

        rangeu2_t header = trim(sourceText, {sourceSpan.Offset, openBrace - sourceSpan.Offset}, TRIM_CHARACTERS);
        if(header.Count > 4
        && 'e' == result.SourceText[header.Offset + 0]
        && 'n' == result.SourceText[header.Offset + 1]
        && 'u' == result.SourceText[header.Offset + 2]
        && 'm' == result.SourceText[header.Offset + 3]
        && is_space_character(result.SourceText[header.Offset + 4]))
            header = trim(sourceText, {header.Offset + 4, header.Count - 4}, TRIM_CHARACTERS);
        sourceText.slice(sourceView, header);
        cnst err_t iType = ::gpk::find(':', sourceView);
        u2_c typeOffset = 0 > iType ? header.Offset + header.Count : header.Offset + iType;
        result.EnumName = trim(sourceText, {header.Offset, typeOffset - header.Offset}, TRIM_CHARACTERS);
        cnst err_t enumNameError = cttiIdentifierError(result.SourceText, result.EnumName);
        if(0 <= enumNameError) {
            state.IndexCurrentChar = (u2_t)enumNameError;
            rtrn result;
        }
        state.State = CTTI_ENUM_PARSE_STATE_UNDERLYING_TYPE;
        result.UnderlyingType = typeOffset < header.Offset + header.Count
            ? trim(sourceText, {typeOffset + 1, header.Offset + header.Count - typeOffset - 1}, TRIM_CHARACTERS)
            : rangeu2_t{};
        state.IndexCurrentChar = typeOffset;
        if(result.UnderlyingType.empty())
            rtrn result;

        state.State = CTTI_ENUM_PARSE_STATE_VALUE_NAME;
        state.IndexCurrentChar = openBrace + 1;
        u2_t & cursor = state.IndexCurrentChar;
        u2_c textLength = sourceSpan.Offset + sourceSpan.Count - 1;
        while(true) {
            while(cursor < textLength && is_space_character(result.SourceText[cursor]))
                ++cursor;
            if(cursor == textLength) {
                state.State = CTTI_ENUM_PARSE_STATE_COMPLETE;
                rtrn result;
            }
            sourceText.slice(sourceView, rangeu2_t{cursor, textLength - cursor});
            cnst err_t iComma = ::gpk::find(',', sourceView);
            u2_c commaOffset = 0 > iComma ? textLength : cursor + iComma;
            cnst rangeu2_t segment = trim(sourceText, {cursor, commaOffset - cursor}, TRIM_CHARACTERS);
            sourceText.slice(sourceView, segment);
            cnst err_t iEqual = ::gpk::find('=', sourceView);
            u2_c equalOffset = 0 > iEqual ? segment.Offset + segment.Count : segment.Offset + iEqual;
            cnst rangeu2_t valueName = trim(sourceText, {segment.Offset, equalOffset - segment.Offset}, TRIM_CHARACTERS);
            cnst err_t valueNameError = cttiIdentifierError(result.SourceText, valueName);
            if(0 <= valueNameError) {
                cursor = (u2_t)valueNameError;
                rtrn result;
            }

            b8_c hasInitializer = equalOffset < segment.Offset + segment.Count;
            state.State = CTTI_ENUM_PARSE_STATE_VALUE_INITIALIZER;
            cnst rangeu2_t initializer = hasInitializer
                ? trim(sourceText, {equalOffset + 1, segment.Offset + segment.Count - equalOffset - 1}, TRIM_CHARACTERS)
                : rangeu2_t{};
            if(hasInitializer && initializer.empty()) {
                cursor = equalOffset + 1;
                rtrn result;
            }

            s3_t initializerValue = {};
            b8_t valueExplicit = false;
            if(false == initializer.empty()) {
                rangeu2_t numberSpan = initializer;
                b8_t negative = false;
                if('-' == sourceText[numberSpan.Offset] || '+' == sourceText[numberSpan.Offset]) {
                    negative = '-' == sourceText[numberSpan.Offset];
                    ++numberSpan.Offset;
                    --numberSpan.Count;
                }
                u2_t base = 10;
                vcst_t symbols = DIGITS_DECIMAL;
                if(2 <= numberSpan.Count && '0' == sourceText[numberSpan.Offset] && ('x' == sourceText[numberSpan.Offset + 1] || 'X' == sourceText[numberSpan.Offset + 1])) {
                    base = 16;
                    symbols = DIGITS_HEX;
                    numberSpan.Offset += 2;
                    numberSpan.Count -= 2;
                }
                u3_t magnitude = 0;
                cnst err_t digitCount = parseArbitraryBaseInteger(base, symbols, {&sourceText[numberSpan.Offset], numberSpan.Count}, &magnitude);
                if(0 < digitCount) {
                    numberSpan.Offset += digitCount;
                    numberSpan.Count -= digitCount;
                    while(numberSpan.Count && is_any_of(sourceText[numberSpan.Offset], vcsc_t{"uUlL"}))
                        ++numberSpan.Offset, --numberSpan.Count;
                    numberSpan = trim(sourceText, numberSpan, TRIM_CHARACTERS);
                    valueExplicit = numberSpan.empty() || (2 <= numberSpan.Count && '/' == sourceText[numberSpan.Offset] && '/' == sourceText[numberSpan.Offset + 1]);
                    if(valueExplicit)
                        initializerValue = negative ? -(s3_t)magnitude : (s3_t)magnitude;
                }
            }
            b8_c valueResolved = valueExplicit || (initializer.empty() && (0 == result.Count || result.Values[result.Count - 1].ValueResolved));
            cnst TEnum value = valueExplicit ? (TEnum)initializerValue
                : valueResolved ? result.Count ? (TEnum)((s3_t)result.Values[result.Count - 1].Value + 1) : (TEnum)0
                : TEnum{};
            result.Values[result.Count++] = {valueName, initializer, value, valueResolved};
            state.State = CTTI_ENUM_PARSE_STATE_VALUE_NAME;
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
            result.StateRead.State = CTTI_ENUM_PARSE_STATE_VALUE_NAME;
            result.StateRead.IndexCurrentChar = result.EnumName.Offset;
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

	tplt<tpnm T, tpnm... TArgs>
	inxp asttc<T, sizeof...(TArgs)> cttiValues(TArgs... values) nxpt { rtrn {{(T)values...}}; }

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

	tplt<tpnm TEnum, size_t NDeclaration>
	stct SCTTIEnumRegistration {
		SCTTIEnumParseResult<s3_t, NDeclaration>	Value		= {};
		SCTTIEnumParseResult<TEnum, NDeclaration>	TypedValue	= {};
	};

	tplt<tpnm TEnum, size_t NDeclaration>
	inxp SCTTIEnumRegistration<TEnum, NDeclaration> cttiEnumRegistrationMake(sc_c (&declaration)[NDeclaration]) nxpt {
		rtrn {cttiParseEnum<s3_t>(declaration), cttiParseEnum<TEnum>(declaration)};
	}
	tplt<tpnm TEnum, size_t NDeclaration, size_t NValues>
	inxp SCTTIEnumRegistration<TEnum, NDeclaration> cttiEnumRegistrationMake(sc_c (&declaration)[NDeclaration], cnst asttc<TEnum, NValues> & values) nxpt {
		rtrn {cttiParseEnum<s3_t>(declaration), cttiParseEnum<TEnum>(declaration, values.Storage)};
	}

    tplt<tpnm T>
    stct SCTTIEnumTag {};
    tplt<tpnm T>
    void cttiEnumRegistry(SCTTIEnumTag<T>) = delete;

    tplt<tpnm T>
    concept ctti_enum = requires { cttiEnumRegistry(SCTTIEnumTag<rm_csvl_t<T>>{}); };
    tplt<tpnm T> requires ctti_enum<T>
    usng SCTTIEnumRegistry = decltype(cttiEnumRegistry(SCTTIEnumTag<rm_csvl_t<T>>{}));
	tplt<tpnm T> requires ctti_enum<T>
	sinx auto cttiEnumRegistration = cttiEnumRegistry(SCTTIEnumTag<rm_csvl_t<T>>{});
	tplt<tpnm T> requires ctti_enum<T>
	sinx auto cttiEnumViewsRegistration = cttiEnumViewsMake(cttiEnumRegistration<T>.TypedValue);

    tplt<tpnm T> requires ctti_enum<T>
	sinx cnst auto & cttiEnum() nxpt { rtrn cttiEnumRegistration<T>.Value; }

    tplt<tpnm T> requires ctti_enum<T>
	sinx cnst auto & cttiEnumTyped() nxpt { rtrn cttiEnumRegistration<T>.TypedValue; }

    tplt<tpnm T> requires ctti_enum<T>
	sinx cnst auto & cttiEnumViews() nxpt { rtrn cttiEnumViewsRegistration<T>; }
    tplt<tpnm T> requires ctti_enum<T>
    sinx view<vcst_c> cttiValueNames() nxpt { cnst auto & views = cttiEnumViews<T>(); rtrn {views.Names.begin(), views.Count}; }

    tplt<tpnm T> requires ctti_enum<T>
    inxp vcst_t cttiEnumName() nxpt {
        usng TEnum = rm_csvl_t<T>;
        cnst auto & metadata = cttiEnumTyped<TEnum>();
        rtrn {&metadata.SourceText[metadata.EnumName.Offset], metadata.EnumName.Count};
    }
    tplt<tpnm T> requires ctti_enum<T>
    inxp vcst_t cttiEnumName(cnst T &) nxpt { rtrn cttiEnumName<T>(); }

    tplt<tpnm T> requires ctti_enum<T>
    inxp vcst_t cttiValueNameByIndex(u2_c iValue, b8_c scoped = true) nxpt {
        usng TEnum = rm_csvl_t<T>;
        cnst auto & metadata = cttiEnumTyped<TEnum>();
        if(iValue >= metadata.Count)
            rtrn {};
        vcst_t name = cttiEnumViews<TEnum>().Names[iValue];
        if(false == scoped
        && name.size() > metadata.EnumName.Count + 1
        && name[metadata.EnumName.Count] == '_'
        )
            name = {name.begin() + metadata.EnumName.Count + 1, name.size() - metadata.EnumName.Count - 1};
        rtrn name;
    }

    tplt<tpnm T> requires ctti_enum<T>
    inxp vcst_t cttiValueName(cnst T input, b8_c scoped = true) nxpt {
        usng TEnum = rm_csvl_t<T>;
        cnst auto & metadata = cttiEnumTyped<TEnum>();
        for(u2_t iValue = 0; iValue < metadata.Count; ++iValue)
            if(metadata.Values[iValue].ValueResolved && input == metadata.Values[iValue].Value)
                rtrn cttiValueNameByIndex<TEnum>(iValue, scoped);
        rtrn {};
    }

} // nmsp

#define GPK_CTTI_ENUM_STATIC_REGISTER(type, ...) \
    GDEFINE_ENUM_OPERATORS(type); \
	sinx auto cttiEnumRegistry(::gpk::SCTTIEnumTag<type>) nxpt { rtrn ::gpk::cttiEnumRegistrationMake<type>(#__VA_ARGS__); }

#define GPK_CTTI_ENUM_REGISTER(type, ...) \
    GPK_CTTI_ENUM_STATIC_REGISTER(type, __VA_ARGS__)

#define CTTI_ENUM(type, ...) \
    __VA_ARGS__ \
    GPK_CTTI_ENUM_REGISTER(type, __VA_ARGS__)

#define CTTI_ENUM_COMPILED(EnumName, IntType, CompiledValues, ...) \
    enum EnumName : IntType { __VA_ARGS__ }; \
    GDEFINE_ENUM_OPERATORS(EnumName); \
	sinx auto cttiEnumRegistry(::gpk::SCTTIEnumTag<EnumName>) nxpt { rtrn ::gpk::cttiEnumRegistrationMake<EnumName>(#EnumName " : " #IntType " { " #__VA_ARGS__ " }", ::gpk::cttiValues<EnumName> CompiledValues); }

#endif // GPK_CTTI_ENUM_H
