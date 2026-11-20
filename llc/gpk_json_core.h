#include "gpk_ctti_enum.h"
#include "gpk_bit.h"
#include "gpk_reader_core.h"
#include "gpk_slice.h"

#ifndef GPK_JSON_CORE_H
#define GPK_JSON_CORE_H

nmsp gpk
{
#pragma pack(push, 1)
#ifdef GPK_ARDUINO
	tydf  s1_t json_id_t;
	tydf  s0_t json_type_t;
#else
	tydf  s2_t json_id_t;
	tydf  s2_t json_type_t;
#endif
	CTTI_ENUM(JSON_TYPE,
	enum JSON_TYPE : json_type_t
		{ JSON_TYPE_NULL		=  0
		, JSON_TYPE_INTEGER		=  1
		, JSON_TYPE_DECIMAL		=  2
		, JSON_TYPE_BOOLEAN		=  3
		, JSON_TYPE_STRING		=  4
		, JSON_TYPE_KEY			=  5
		, JSON_TYPE_VALUE		=  6
		, JSON_TYPE_OBJECT		=  7
		, JSON_TYPE_ARRAY		=  8
		, JSON_TYPE_CODEPOINT	=  9
		, JSON_TYPE_COUNT		= 10
		, JSON_TYPE_UNKNOWN		= -1
		};
	);
	CTTI_ENUM(JSON_PARSE_OPTION,
	enum JSON_PARSE_OPTION : u1_t
		{ JSON_PARSE_OPTION_NONE						= 0x0000U
		, JSON_PARSE_OPTION_BUILD_TREE					= 0x0001U
		, JSON_PARSE_OPTION_BUILD_VIEWS				= 0x0002U
		, JSON_PARSE_OPTION_FINAL_INPUT				= 0x0004U
		, JSON_PARSE_OPTION_ALLOW_LINE_COMMENTS		= 0x0008U
		, JSON_PARSE_OPTION_ALLOW_TRAILING_COMMAS		= 0x0010U
		, JSON_PARSE_OPTION_ALLOW_LEADING_PLUS			= 0x0020U
		, JSON_PARSE_OPTION_ALLOW_LEADING_DECIMAL_POINT	= 0x0040U
		, JSON_PARSE_OPTION_ALLOW_TRAILING_DECIMAL_POINT	= 0x0080U
		, JSON_PARSE_OPTION_ALLOW_LEADING_ZEROES			= 0x0100U
		, JSON_PARSE_OPTION_ALLOW_TRAILING_CONTENT		= 0x0200U
		, JSON_PARSE_OPTION_ALLOW_UNESCAPED_CONTROLS		= 0x0400U
		, JSON_PARSE_OPTION_CONTAINER_ROOT_ONLY			= 0x0800U
		};
	);
	tydf bit_field<JSON_PARSE_OPTION> SJSONParseOptions;
	stxp SJSONParseOptions JSON_PARSE_STRICT		= {JSON_PARSE_OPTION_BUILD_TREE | JSON_PARSE_OPTION_BUILD_VIEWS | JSON_PARSE_OPTION_FINAL_INPUT};
	stxp SJSONParseOptions JSON_PARSE_LEGACY_ROOT	= {JSON_PARSE_OPTION(JSON_PARSE_STRICT) | JSON_PARSE_OPTION_CONTAINER_ROOT_ONLY};
	stxp SJSONParseOptions JSON_PARSE_GPK			= {JSON_PARSE_OPTION(JSON_PARSE_STRICT)
		| JSON_PARSE_OPTION_ALLOW_LINE_COMMENTS | JSON_PARSE_OPTION_ALLOW_TRAILING_COMMAS | JSON_PARSE_OPTION_ALLOW_LEADING_PLUS
		| JSON_PARSE_OPTION_ALLOW_LEADING_DECIMAL_POINT | JSON_PARSE_OPTION_ALLOW_TRAILING_DECIMAL_POINT | JSON_PARSE_OPTION_ALLOW_LEADING_ZEROES
		| JSON_PARSE_OPTION_ALLOW_TRAILING_CONTENT | JSON_PARSE_OPTION_ALLOW_UNESCAPED_CONTROLS};
	stct SJSONToken {
		json_id_t	ParentIndex;
		JSON_TYPE	Type;
		sliceu2_t	Span;
		u3_t		Value;
	};
	stct SJSONReaderState : SReaderState<SJSONToken, json_id_t, s1_t> {
		b8_t					Escaping			: 1;
		b8_t					InsideString		: 1;
		b8_t					ExpectingSeparator	: 1;
		b8_t					DoneReading			: 1;
						inxp	SJSONReaderState	()
		: Escaping			(false)
		, InsideString		(false)
		, ExpectingSeparator(false)
		, DoneReading		(false)
		{}
	};
#pragma pack(pop)
	tydf SReaderView<SJSONToken> SJSONReaderView;

	inxp vcst_t jsonTokenView(vcst_t source, cnst SJSONToken & token) nxpt {
		if(token.Span.Begin > token.Span.End || token.Span.End > source.size())
			rtrn {};
		rtrn {&source[token.Span.Begin], token.Span.End - token.Span.Begin};
	}

	inxp err_t jsonTokenNext(view<cnst SJSONToken> tokens, u2_t iToken) nxpt {
		if(iToken >= tokens.size() || tokens[iToken].Span.Begin > tokens[iToken].Span.End)
			rtrn -1;
		cnst u2_t spanEnd = tokens[iToken].Span.End;
		for(++iToken; iToken < tokens.size(); ++iToken)
			if(tokens[iToken].Span.Begin >= spanEnd)
				rtrn iToken;
		rtrn tokens.size();
	}

	inxp err_t jsonTokenChildGet(view<cnst SJSONToken> tokens, u2_t iToken, u2_t index) nxpt {
		if(iToken >= tokens.size() || tokens[iToken].Span.Begin > tokens[iToken].Span.End)
			rtrn -1;
		cnst u2_t spanEnd = tokens[iToken].Span.End;
		for(u2_t iChild = iToken + 1; iChild < tokens.size() && tokens[iChild].Span.Begin < spanEnd;) {
			if(0 == index--)
				rtrn iChild;
			cnst err_t iNext = jsonTokenNext(tokens, iChild);
			if(0 > iNext || (u2_t)iNext <= iChild)
				rtrn -1;
			iChild = (u2_t)iNext;
		}
		rtrn -1;
	}

	inxp err_t jsonTokenValueGet(view<cnst SJSONToken> tokens, u2_t iToken) nxpt {
		if(iToken >= tokens.size())
			rtrn -1;
		if(JSON_TYPE_KEY == tokens[iToken].Type || JSON_TYPE_VALUE == tokens[iToken].Type)
			rtrn jsonTokenChildGet(tokens, iToken, 0);
		rtrn iToken;
	}

	inxp err_t jsonArraySize(view<cnst SJSONToken> tokens, u2_t iToken) nxpt {
		cnst err_t iArray = jsonTokenValueGet(tokens, iToken);
		if(0 > iArray || JSON_TYPE_ARRAY != tokens[iArray].Type)
			rtrn -1;
		u2_t count = 0;
		cnst u2_t spanEnd = tokens[iArray].Span.End;
		for(u2_t iChild = (u2_t)iArray + 1; iChild < tokens.size() && tokens[iChild].Span.Begin < spanEnd;) {
			if(JSON_TYPE_VALUE != tokens[iChild].Type)
				rtrn -1;
			++count;
			cnst err_t iNext = jsonTokenNext(tokens, iChild);
			if(0 > iNext || (u2_t)iNext <= iChild)
				rtrn -1;
			iChild = (u2_t)iNext;
		}
		rtrn count;
	}

	inxp err_t jsonObjectKeyCount(view<cnst SJSONToken> tokens, u2_t iToken) nxpt {
		cnst err_t iObject = jsonTokenValueGet(tokens, iToken);
		if(0 > iObject || JSON_TYPE_OBJECT != tokens[iObject].Type)
			rtrn -1;
		u2_t count = 0;
		cnst u2_t spanEnd = tokens[iObject].Span.End;
		for(u2_t iChild = (u2_t)iObject + 1; iChild < tokens.size() && tokens[iChild].Span.Begin < spanEnd;) {
			if(JSON_TYPE_KEY != tokens[iChild].Type)
				rtrn -1;
			cnst err_t iValue = jsonTokenNext(tokens, iChild);
			if(0 > iValue || (u2_t)iValue >= tokens.size() || tokens[iValue].Span.Begin >= spanEnd || JSON_TYPE_VALUE != tokens[iValue].Type)
				rtrn -1;
			++count;
			cnst err_t iNext = jsonTokenNext(tokens, (u2_t)iValue);
			if(0 > iNext || (u2_t)iNext <= (u2_t)iValue)
				rtrn -1;
			iChild = (u2_t)iNext;
		}
		rtrn count;
	}

	inxp err_t jsonArrayValueGet(view<cnst SJSONToken> tokens, u2_t iToken, u2_t index) nxpt {
		cnst err_t iArray = jsonTokenValueGet(tokens, iToken);
		if(0 > iArray || JSON_TYPE_ARRAY != tokens[iArray].Type)
			rtrn -1;
		cnst u2_t spanEnd = tokens[iArray].Span.End;
		for(u2_t iChild = (u2_t)iArray + 1; iChild < tokens.size() && tokens[iChild].Span.Begin < spanEnd;) {
			if(JSON_TYPE_VALUE != tokens[iChild].Type)
				rtrn -1;
			if(0 == index--)
				rtrn jsonTokenValueGet(tokens, iChild);
			cnst err_t iNext = jsonTokenNext(tokens, iChild);
			if(0 > iNext || (u2_t)iNext <= iChild)
				rtrn -1;
			iChild = (u2_t)iNext;
		}
		rtrn -1;
	}

	inxp err_t jsonObjectValueGet(view<cnst SJSONToken> tokens, vcst_t source, u2_t iToken, vcst_t key) nxpt {
		cnst err_t iObject = jsonTokenValueGet(tokens, iToken);
		if(0 > iObject || JSON_TYPE_OBJECT != tokens[iObject].Type)
			rtrn -1;
		cnst u2_t spanEnd = tokens[iObject].Span.End;
		for(u2_t iKeyWrapper = (u2_t)iObject + 1; iKeyWrapper < tokens.size() && tokens[iKeyWrapper].Span.Begin < spanEnd;) {
			if(JSON_TYPE_KEY != tokens[iKeyWrapper].Type)
				rtrn -1;
			cnst err_t iKey = jsonTokenValueGet(tokens, iKeyWrapper);
			cnst err_t iValueWrapper = jsonTokenNext(tokens, iKeyWrapper);
			if(0 > iKey || 0 > iValueWrapper || (u2_t)iValueWrapper >= tokens.size() || tokens[iValueWrapper].Span.Begin >= spanEnd || JSON_TYPE_VALUE != tokens[iValueWrapper].Type)
				rtrn -1;
			if(jsonTokenView(source, tokens[iKey]) == key)
				rtrn jsonTokenValueGet(tokens, (u2_t)iValueWrapper);
			cnst err_t iNext = jsonTokenNext(tokens, (u2_t)iValueWrapper);
			if(0 > iNext || (u2_t)iNext <= (u2_t)iValueWrapper)
				rtrn -1;
			iKeyWrapper = (u2_t)iNext;
		}
		rtrn -1;
	}

	tplt<typename _tFunc>
	inxp err_t jsonArrayForEach(view<cnst SJSONToken> tokens, u2_t iToken, _tFunc funcForEach) {
		cnst err_t iArray = jsonTokenValueGet(tokens, iToken);
		if(0 > iArray || JSON_TYPE_ARRAY != tokens[iArray].Type)
			rtrn -1;
		u2_t iElement = 0;
		cnst u2_t spanEnd = tokens[iArray].Span.End;
		for(u2_t iChild = (u2_t)iArray + 1; iChild < tokens.size() && tokens[iChild].Span.Begin < spanEnd; ++iElement) {
			if(JSON_TYPE_VALUE != tokens[iChild].Type)
				rtrn -1;
			cnst err_t iValue = jsonTokenValueGet(tokens, iChild);
			if(0 > iValue)
				rtrn -1;
			cnst err_t result = funcForEach((u2_t)iValue);
			if(0 > result)
				rtrn result;
			if(result)
				rtrn iElement + 1;
			cnst err_t iNext = jsonTokenNext(tokens, iChild);
			if(0 > iNext || (u2_t)iNext <= iChild)
				rtrn -1;
			iChild = (u2_t)iNext;
		}
		rtrn iElement;
	}

	tplt<typename _tFunc>
	inxp err_t jsonObjectForEach(view<cnst SJSONToken> tokens, u2_t iToken, _tFunc funcForEach) {
		cnst err_t iObject = jsonTokenValueGet(tokens, iToken);
		if(0 > iObject || JSON_TYPE_OBJECT != tokens[iObject].Type)
			rtrn -1;
		u2_t iPair = 0;
		cnst u2_t spanEnd = tokens[iObject].Span.End;
		for(u2_t iKeyWrapper = (u2_t)iObject + 1; iKeyWrapper < tokens.size() && tokens[iKeyWrapper].Span.Begin < spanEnd; ++iPair) {
			if(JSON_TYPE_KEY != tokens[iKeyWrapper].Type)
				rtrn -1;
			cnst err_t iKey = jsonTokenValueGet(tokens, iKeyWrapper);
			cnst err_t iValueWrapper = jsonTokenNext(tokens, iKeyWrapper);
			if(0 > iKey || 0 > iValueWrapper || (u2_t)iValueWrapper >= tokens.size() || tokens[iValueWrapper].Span.Begin >= spanEnd || JSON_TYPE_VALUE != tokens[iValueWrapper].Type)
				rtrn -1;
			cnst err_t iValue = jsonTokenValueGet(tokens, (u2_t)iValueWrapper);
			if(0 > iValue)
				rtrn -1;
			cnst err_t result = funcForEach((u2_t)iKey, (u2_t)iValue);
			if(0 > result)
				rtrn result;
			if(result)
				rtrn iPair + 1;
			cnst err_t iNext = jsonTokenNext(tokens, (u2_t)iValueWrapper);
			if(0 > iNext || (u2_t)iNext <= (u2_t)iValueWrapper)
				rtrn -1;
			iKeyWrapper = (u2_t)iNext;
		}
		rtrn iPair;
	}

	inxp err_t jsonObjectValueGet(view<cnst SJSONToken> tokens, view<vcst_c> views, u2_t iToken, vcst_t key) {
		err_t iValueFound = -1;
		cnst err_t result = jsonObjectForEach(tokens, iToken, [views, key, &iValueFound](u2_t iKey, u2_t iValue) constexpr -> err_t {
			if(iKey >= views.size())
				rtrn -1;
			if(views[iKey] == key) {
				iValueFound = iValue;
				rtrn 1;
			}
			rtrn 0;
			});
		if(0 > result)
			rtrn result;
		rtrn iValueFound;
	}

	inxp err_t jsonCompare(view<cnst SJSONToken> tokens, vcst_t source, u2_t iToken, view<cnst SJSONToken> otherTokens, vcst_t otherSource, u2_t iOther) nxpt;

	inxp err_t jsonCompareNumber(view<cnst SJSONToken> tokens, vcst_t source, u2_t iToken, view<cnst SJSONToken> otherTokens, vcst_t otherSource, u2_t iOther) nxpt {
		iToken = (u2_t)jsonTokenValueGet(tokens, iToken);
		iOther = (u2_t)jsonTokenValueGet(otherTokens, iOther);
		if(iToken >= tokens.size() || iOther >= otherTokens.size())
			rtrn -1;
		if(tokens[iToken].Type != otherTokens[iOther].Type || (JSON_TYPE_INTEGER != tokens[iToken].Type && JSON_TYPE_DECIMAL != tokens[iToken].Type))
			rtrn 0;
		rtrn jsonTokenView(source, tokens[iToken]) == jsonTokenView(otherSource, otherTokens[iOther]);
	}

	inxp err_t jsonCompareArray(view<cnst SJSONToken> tokens, vcst_t source, u2_t iToken, view<cnst SJSONToken> otherTokens, vcst_t otherSource, u2_t iOther) nxpt {
		cnst err_t count = jsonArraySize(tokens, iToken);
		cnst err_t otherCount = jsonArraySize(otherTokens, iOther);
		if(0 > count || 0 > otherCount)
			rtrn -1;
		if(count != otherCount)
			rtrn 0;
		for(u2_t iElement = 0; iElement < (u2_t)count; ++iElement) {
			cnst err_t iValue = jsonArrayValueGet(tokens, iToken, iElement);
			cnst err_t iOtherValue = jsonArrayValueGet(otherTokens, iOther, iElement);
			if(0 > iValue || 0 > iOtherValue)
				rtrn -1;
			cnst err_t equal = jsonCompare(tokens, source, (u2_t)iValue, otherTokens, otherSource, (u2_t)iOtherValue);
			if(0 >= equal)
				rtrn equal;
		}
		rtrn 1;
	}

	inxp err_t jsonCompareObject(view<cnst SJSONToken> tokens, vcst_t source, u2_t iToken, view<cnst SJSONToken> otherTokens, vcst_t otherSource, u2_t iOther) nxpt {
		cnst err_t count = jsonObjectKeyCount(tokens, iToken);
		cnst err_t otherCount = jsonObjectKeyCount(otherTokens, iOther);
		if(0 > count || 0 > otherCount)
			rtrn -1;
		if(count != otherCount)
			rtrn 0;
		iToken = (u2_t)jsonTokenValueGet(tokens, iToken);
		iOther = (u2_t)jsonTokenValueGet(otherTokens, iOther);
		u2_t iKeyWrapper = iToken + 1;
		u2_t iOtherKeyWrapper = iOther + 1;
		for(u2_t iPair = 0; iPair < (u2_t)count; ++iPair) {
			cnst err_t iKey = jsonTokenValueGet(tokens, iKeyWrapper);
			cnst err_t iOtherKey = jsonTokenValueGet(otherTokens, iOtherKeyWrapper);
			cnst err_t iValueWrapper = jsonTokenNext(tokens, iKeyWrapper);
			cnst err_t iOtherValueWrapper = jsonTokenNext(otherTokens, iOtherKeyWrapper);
			if(0 > iKey || 0 > iOtherKey || 0 > iValueWrapper || 0 > iOtherValueWrapper)
				rtrn -1;
			if(jsonTokenView(source, tokens[iKey]) != jsonTokenView(otherSource, otherTokens[iOtherKey]))
				rtrn 0;
			cnst err_t equal = jsonCompare(tokens, source, (u2_t)iValueWrapper, otherTokens, otherSource, (u2_t)iOtherValueWrapper);
			if(0 >= equal)
				rtrn equal;
			iKeyWrapper = (u2_t)jsonTokenNext(tokens, (u2_t)iValueWrapper);
			iOtherKeyWrapper = (u2_t)jsonTokenNext(otherTokens, (u2_t)iOtherValueWrapper);
		}
		rtrn 1;
	}

	inxp err_t jsonCompare(view<cnst SJSONToken> tokens, vcst_t source, u2_t iToken, view<cnst SJSONToken> otherTokens, vcst_t otherSource, u2_t iOther) nxpt {
		cnst err_t iValue = jsonTokenValueGet(tokens, iToken);
		cnst err_t iOtherValue = jsonTokenValueGet(otherTokens, iOther);
		if(0 > iValue || 0 > iOtherValue)
			rtrn -1;
		cnst JSON_TYPE type = tokens[iValue].Type;
		if(type != otherTokens[iOtherValue].Type)
			rtrn 0;
		switch(type) {
		case JSON_TYPE_OBJECT: rtrn jsonCompareObject(tokens, source, (u2_t)iValue, otherTokens, otherSource, (u2_t)iOtherValue);
		case JSON_TYPE_ARRAY : rtrn jsonCompareArray (tokens, source, (u2_t)iValue, otherTokens, otherSource, (u2_t)iOtherValue);
		case JSON_TYPE_INTEGER:
		case JSON_TYPE_DECIMAL: rtrn jsonCompareNumber(tokens, source, (u2_t)iValue, otherTokens, otherSource, (u2_t)iOtherValue);
		default: rtrn jsonTokenView(source, tokens[iValue]) == jsonTokenView(otherSource, otherTokens[iOtherValue]);
		}
	}
} // nmsp

#endif // GPK_JSON_CORE_H
