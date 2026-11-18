#include "gpk_json_core.h"
#include "gpk_array_static.h"

#ifndef GPK_JSON_CTTI_H
#define GPK_JSON_CTTI_H

nmsp gpk
{
	enum CTTI_JSON_PARSE_ERROR : u0_t
		{ CTTI_JSON_PARSE_ERROR_NONE				= 0
		, CTTI_JSON_PARSE_ERROR_UNEXPECTED_END		= 1
		, CTTI_JSON_PARSE_ERROR_UNEXPECTED_CHARACTER	= 2
		, CTTI_JSON_PARSE_ERROR_EXPECTED_COLON		= 3
		, CTTI_JSON_PARSE_ERROR_EXPECTED_SEPARATOR	= 4
		, CTTI_JSON_PARSE_ERROR_INVALID_LITERAL		= 5
		, CTTI_JSON_PARSE_ERROR_INVALID_NUMBER		= 6
		, CTTI_JSON_PARSE_ERROR_TOKEN_COUNT			= 7
		};

	tplt<size_t N>
	stct SCTTIJSONReader {
		astatic<SJSONToken, N>	Token		= {};
		u2_t				Count		= {};
		u2_t				ErrorOffset	= {};
		CTTI_JSON_PARSE_ERROR	Error		= CTTI_JSON_PARSE_ERROR_NONE;

		inxp b8_t			Success		() cnst nxpt { rtrn CTTI_JSON_PARSE_ERROR_NONE == Error; }
		inxp view<cnst SJSONToken> Tokens	() cnst nxpt { rtrn {Token.begin(), Count}; }
	};

	tplt<size_t N>
	inxp void cttiJSONError(SCTTIJSONReader<N> & result, CTTI_JSON_PARSE_ERROR error, u2_t offset) nxpt {
		if(CTTI_JSON_PARSE_ERROR_NONE == result.Error) {
			result.Error = error;
			result.ErrorOffset = offset;
		}
	}

	tplt<size_t N>
	inxp json_id_t cttiJSONTokenPush(SCTTIJSONReader<N> & result, json_id_t parent, JSON_TYPE type, u2_t begin) nxpt {
		if(result.Count >= N) {
			cttiJSONError(result, CTTI_JSON_PARSE_ERROR_TOKEN_COUNT, begin);
			rtrn -1;
		}
		result.Token.Storage[result.Count] = {parent, type, {begin, begin}, {}};
		rtrn (json_id_t)result.Count++;
	}

	tplt<size_t N>
	inxp b8_t cttiJSONSpace(sc_c (&source)[N], u2_t & cursor, u2_t length) nxpt {
		while(cursor < length) {
			sc_c character = source[cursor];
			if(' ' == character || '\t' == character || '\r' == character || '\n' == character || '\f' == character || '\b' == character) {
				++cursor;
				continue;
			}
			if('/' == character && cursor + 1 < length && '/' == source[cursor + 1]) {
				cursor += 2;
				while(cursor < length && '\n' != source[cursor])
					++cursor;
				continue;
			}
			break;
		}
		rtrn cursor < length;
	}

	inxp b8_t cttiJSONHex(sc_c character) nxpt {
		rtrn (character >= '0' && character <= '9') || (character >= 'a' && character <= 'f') || (character >= 'A' && character <= 'F');
	}

	tplt<size_t N>
	inxp json_id_t cttiJSONParseString(SCTTIJSONReader<N> & result, sc_c (&source)[N], u2_t & cursor, u2_t length, json_id_t parent) nxpt {
		u2_c quoteOffset = cursor++;
		json_id_t index = cttiJSONTokenPush(result, parent, JSON_TYPE_STRING, cursor);
		if(0 > index)
			rtrn index;
		b8_t escaping = false;
		while(cursor < length) {
			sc_c character = source[cursor];
			if(escaping) {
				if('u' == character) {
					if(cursor + 4 >= length) {
						cttiJSONError(result, CTTI_JSON_PARSE_ERROR_UNEXPECTED_END, cursor);
						rtrn -1;
					}
					for(u2_t iDigit = 1; iDigit <= 4; ++iDigit)
						if(false == cttiJSONHex(source[cursor + iDigit])) {
							cttiJSONError(result, CTTI_JSON_PARSE_ERROR_UNEXPECTED_CHARACTER, cursor + iDigit);
							rtrn -1;
						}
					cursor += 5;
					escaping = false;
					continue;
				}
				escaping = false;
				++cursor;
				continue;
			}
			if('\\' == character) {
				escaping = true;
				++cursor;
				continue;
			}
			if('"' == character) {
				result.Token.Storage[index].Span.End = cursor++;
				rtrn index;
			}
			++cursor;
		}
		cttiJSONError(result, CTTI_JSON_PARSE_ERROR_UNEXPECTED_END, quoteOffset);
		rtrn -1;
	}

	tplt<size_t N, size_t NLiteral>
	inxp b8_t cttiJSONLiteral(sc_c (&source)[N], u2_t cursor, u2_t length, sc_c (&literal)[NLiteral]) nxpt {
		if(cursor + NLiteral - 1 > length)
			rtrn false;
		for(u2_t iCharacter = 0; iCharacter < NLiteral - 1; ++iCharacter)
			if(source[cursor + iCharacter] != literal[iCharacter])
				rtrn false;
		rtrn true;
	}

	tplt<size_t N>
	inxp json_id_t cttiJSONParseValue(SCTTIJSONReader<N> & result, sc_c (&source)[N], u2_t & cursor, u2_t length, json_id_t parent) nxpt;

	tplt<size_t N>
	inxp json_id_t cttiJSONParseObject(SCTTIJSONReader<N> & result, sc_c (&source)[N], u2_t & cursor, u2_t length, json_id_t parent) nxpt {
		u2_c begin = cursor++;
		json_id_t object = cttiJSONTokenPush(result, parent, JSON_TYPE_OBJECT, begin);
		if(0 > object)
			rtrn object;
		cttiJSONSpace(source, cursor, length);
		if(cursor < length && '}' == source[cursor]) {
			result.Token.Storage[object].Span.End = ++cursor;
			rtrn object;
		}
		while(cursor < length) {
			if('"' != source[cursor]) {
				cttiJSONError(result, CTTI_JSON_PARSE_ERROR_UNEXPECTED_CHARACTER, cursor);
				rtrn -1;
			}
			u2_c keyBegin = cursor;
			json_id_t key = cttiJSONTokenPush(result, object, JSON_TYPE_KEY, keyBegin);
			if(0 > key || 0 > cttiJSONParseString(result, source, cursor, length, key))
				rtrn -1;
			cttiJSONSpace(source, cursor, length);
			if(cursor >= length || ':' != source[cursor]) {
				cttiJSONError(result, CTTI_JSON_PARSE_ERROR_EXPECTED_COLON, cursor);
				rtrn -1;
			}
			result.Token.Storage[key].Span.End = cursor++;
			cttiJSONSpace(source, cursor, length);
			json_id_t value = cttiJSONTokenPush(result, object, JSON_TYPE_VALUE, cursor);
			if(0 > value || 0 > cttiJSONParseValue(result, source, cursor, length, value))
				rtrn -1;
			result.Token.Storage[value].Span.End = cursor;
			cttiJSONSpace(source, cursor, length);
			if(cursor < length && '}' == source[cursor]) {
				result.Token.Storage[object].Span.End = ++cursor;
				rtrn object;
			}
			if(cursor >= length || ',' != source[cursor]) {
				cttiJSONError(result, CTTI_JSON_PARSE_ERROR_EXPECTED_SEPARATOR, cursor);
				rtrn -1;
			}
			++cursor;
			cttiJSONSpace(source, cursor, length);
			if(cursor < length && '}' == source[cursor]) {
				result.Token.Storage[object].Span.End = ++cursor;
				rtrn object;
			}
		}
		cttiJSONError(result, CTTI_JSON_PARSE_ERROR_UNEXPECTED_END, cursor);
		rtrn -1;
	}

	tplt<size_t N>
	inxp json_id_t cttiJSONParseArray(SCTTIJSONReader<N> & result, sc_c (&source)[N], u2_t & cursor, u2_t length, json_id_t parent) nxpt {
		u2_c begin = cursor++;
		json_id_t array = cttiJSONTokenPush(result, parent, JSON_TYPE_ARRAY, begin);
		if(0 > array)
			rtrn array;
		cttiJSONSpace(source, cursor, length);
		if(cursor < length && ']' == source[cursor]) {
			result.Token.Storage[array].Span.End = ++cursor;
			rtrn array;
		}
		while(cursor < length) {
			json_id_t value = cttiJSONTokenPush(result, array, JSON_TYPE_VALUE, cursor);
			if(0 > value || 0 > cttiJSONParseValue(result, source, cursor, length, value))
				rtrn -1;
			result.Token.Storage[value].Span.End = cursor;
			cttiJSONSpace(source, cursor, length);
			if(cursor < length && ']' == source[cursor]) {
				result.Token.Storage[array].Span.End = ++cursor;
				rtrn array;
			}
			if(cursor >= length || ',' != source[cursor]) {
				cttiJSONError(result, CTTI_JSON_PARSE_ERROR_EXPECTED_SEPARATOR, cursor);
				rtrn -1;
			}
			++cursor;
			cttiJSONSpace(source, cursor, length);
			if(cursor < length && ']' == source[cursor]) {
				result.Token.Storage[array].Span.End = ++cursor;
				rtrn array;
			}
		}
		cttiJSONError(result, CTTI_JSON_PARSE_ERROR_UNEXPECTED_END, cursor);
		rtrn -1;
	}

	tplt<size_t N>
	inxp json_id_t cttiJSONParseNumber(SCTTIJSONReader<N> & result, sc_c (&source)[N], u2_t & cursor, u2_t length, json_id_t parent) nxpt {
		u2_c begin = cursor;
		b8_t negative = false;
		if(cursor < length && ('+' == source[cursor] || '-' == source[cursor])) {
			negative = '-' == source[cursor];
			++cursor;
		}
		b8_t decimal = false;
		u2_t digits = 0;
		while(cursor < length) {
			sc_c character = source[cursor];
			if(character >= '0' && character <= '9') {
				++digits;
				++cursor;
				continue;
			}
			if('.' == character && false == decimal) {
				decimal = true;
				++cursor;
				continue;
			}
			break;
		}
		if(0 == digits) {
			cttiJSONError(result, CTTI_JSON_PARSE_ERROR_INVALID_NUMBER, begin);
			rtrn -1;
		}
		json_id_t number = cttiJSONTokenPush(result, parent, decimal ? JSON_TYPE_DECIMAL : JSON_TYPE_INTEGER, begin);
		if(0 > number)
			rtrn number;
		result.Token.Storage[number].Span.End = cursor;
		if(false == decimal) {
			u3_t value = 0;
			for(u2_t iCharacter = begin + (('+' == source[begin] || '-' == source[begin]) ? 1U : 0U); iCharacter < cursor; ++iCharacter)
				value = value * 10 + (u0_t)(source[iCharacter] - '0');
			result.Token.Storage[number].Value = negative ? 0ULL - value : value;
		}
		rtrn number;
	}

	tplt<size_t N>
	inxp json_id_t cttiJSONParseValue(SCTTIJSONReader<N> & result, sc_c (&source)[N], u2_t & cursor, u2_t length, json_id_t parent) nxpt {
		cttiJSONSpace(source, cursor, length);
		if(cursor >= length) {
			cttiJSONError(result, CTTI_JSON_PARSE_ERROR_UNEXPECTED_END, cursor);
			rtrn -1;
		}
		switch(source[cursor]) {
		case '{': rtrn cttiJSONParseObject(result, source, cursor, length, parent);
		case '[': rtrn cttiJSONParseArray(result, source, cursor, length, parent);
		case '"': rtrn cttiJSONParseString(result, source, cursor, length, parent);
		case 't': {
			if(false == cttiJSONLiteral(source, cursor, length, "true")) {
				cttiJSONError(result, CTTI_JSON_PARSE_ERROR_INVALID_LITERAL, cursor);
				rtrn -1;
			}
			json_id_t token = cttiJSONTokenPush(result, parent, JSON_TYPE_BOOLEAN, cursor);
			if(0 <= token) result.Token.Storage[token].Span.End = cursor += 4, result.Token.Storage[token].Value = 1;
			rtrn token;
		}
		case 'f': {
			if(false == cttiJSONLiteral(source, cursor, length, "false")) {
				cttiJSONError(result, CTTI_JSON_PARSE_ERROR_INVALID_LITERAL, cursor);
				rtrn -1;
			}
			json_id_t token = cttiJSONTokenPush(result, parent, JSON_TYPE_BOOLEAN, cursor);
			if(0 <= token) result.Token.Storage[token].Span.End = cursor += 5;
			rtrn token;
		}
		case 'n': {
			if(false == cttiJSONLiteral(source, cursor, length, "null")) {
				cttiJSONError(result, CTTI_JSON_PARSE_ERROR_INVALID_LITERAL, cursor);
				rtrn -1;
			}
			json_id_t token = cttiJSONTokenPush(result, parent, JSON_TYPE_NULL, cursor);
			if(0 <= token) result.Token.Storage[token].Span.End = cursor += 4;
			rtrn token;
		}
		default:
			rtrn cttiJSONParseNumber(result, source, cursor, length, parent);
		}
	}

	tplt<size_t N>
	inxp SCTTIJSONReader<N> jsonParseCTTI(sc_c (&source)[N]) nxpt {
		SCTTIJSONReader<N> result = {};
		u2_t cursor = 0;
		u2_c length = N && 0 == source[N - 1] ? (u2_t)N - 1 : (u2_t)N;
		cttiJSONSpace(source, cursor, length);
		if(cursor >= length) {
			cttiJSONError(result, CTTI_JSON_PARSE_ERROR_UNEXPECTED_END, cursor);
			rtrn result;
		}
		if('{' == source[cursor] || '[' == source[cursor])
			cttiJSONParseValue(result, source, cursor, length, -1);
		else {
			json_id_t value = cttiJSONTokenPush(result, -1, JSON_TYPE_VALUE, cursor);
			if(0 <= value && 0 <= cttiJSONParseValue(result, source, cursor, length, value))
				result.Token.Storage[value].Span.End = cursor;
		}
		rtrn result;
	}

} // nmsp

#endif // GPK_JSON_CTTI_H
