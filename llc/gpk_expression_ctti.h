#include "gpk_expression_core.h"
#include "gpk_array_static.h"

#ifndef GPK_EXPRESSION_CTTI_H
#define GPK_EXPRESSION_CTTI_H

nmsp gpk
{
	enum CTTI_EXPRESSION_PARSE_ERROR : u0_t
		{ CTTI_EXPRESSION_PARSE_ERROR_NONE				= 0
		, CTTI_EXPRESSION_PARSE_ERROR_UNEXPECTED_END		= 1
		, CTTI_EXPRESSION_PARSE_ERROR_UNEXPECTED_CHARACTER	= 2
		, CTTI_EXPRESSION_PARSE_ERROR_EXPECTED_SEPARATOR	= 3
		, CTTI_EXPRESSION_PARSE_ERROR_INVALID_OPERATOR		= 4
		, CTTI_EXPRESSION_PARSE_ERROR_INVALID_NESTING		= 5
		, CTTI_EXPRESSION_PARSE_ERROR_TOKEN_COUNT			= 6
		};

	tplt<size_t N>
	stct SCTTIExpressionReader {
		astatic<SExpressionToken, N>	Token		= {};
		u2_t						Count		= {};
		u2_t						ErrorOffset	= {};
		CTTI_EXPRESSION_PARSE_ERROR	Error		= CTTI_EXPRESSION_PARSE_ERROR_NONE;

		inxp b8_t Success() cnst nxpt { rtrn CTTI_EXPRESSION_PARSE_ERROR_NONE == Error; }
		inxp view<cnst SExpressionToken> Tokens() cnst nxpt { rtrn {Token.begin(), Count}; }
	};

	stct SCTTIExpressionState {
		u2_t	IndexCurrentChar	= {};
		s2_t	IndexCurrentElement	= -1;
		s2_t	NestLevel			= {};
		b8_t	Escaping			= false;
		b8_t	ExpectsSeparator	= false;
		b8_t	InsideString		= false;
		sc_t	ClosingQuotes		= '0';
	};

	tplt<size_t N>
	inxp void cttiExpressionError(SCTTIExpressionReader<N> & result, CTTI_EXPRESSION_PARSE_ERROR error, u2_t offset) nxpt {
		if(CTTI_EXPRESSION_PARSE_ERROR_NONE == result.Error) {
			result.Error = error;
			result.ErrorOffset = offset;
		}
	}

	tplt<size_t N>
	inxp s2_t cttiExpressionOpen(SCTTIExpressionReader<N> & result, SCTTIExpressionState & state, EXPRESSION_READER_TYPE type, u2_t begin) nxpt {
		if(result.Count >= N) {
			cttiExpressionError(result, CTTI_EXPRESSION_PARSE_ERROR_TOKEN_COUNT, begin);
			rtrn -1;
		}
		s2_c index = (s2_t)result.Count++;
		result.Token.Storage[index] = {state.IndexCurrentElement, type, {begin, begin}, false};
		state.IndexCurrentElement = index;
		++state.NestLevel;
		rtrn index;
	}

	tplt<size_t N>
	inxp b8_t cttiExpressionClose(SCTTIExpressionReader<N> & result, SCTTIExpressionState & state, EXPRESSION_READER_TYPE type, u2_t end) nxpt {
		if(0 > state.IndexCurrentElement || result.Token.Storage[state.IndexCurrentElement].Type != type) {
			cttiExpressionError(result, CTTI_EXPRESSION_PARSE_ERROR_INVALID_NESTING, state.IndexCurrentChar);
			rtrn false;
		}
		SExpressionToken & token = result.Token.Storage[state.IndexCurrentElement];
		token.Span.End = end;
		state.IndexCurrentElement = token.ParentIndex;
		--state.NestLevel;
		rtrn true;
	}

	tplt<size_t N>
	inxp b8_t cttiExpressionCloseIf(SCTTIExpressionReader<N> & result, SCTTIExpressionState & state, EXPRESSION_READER_TYPE type, u2_t end) nxpt {
		if(0 <= state.IndexCurrentElement && result.Token.Storage[state.IndexCurrentElement].Type == type)
			rtrn cttiExpressionClose(result, state, type, end);
		rtrn true;
	}

	tplt<size_t N>
	inxp b8_t cttiExpressionCloseTerm(SCTTIExpressionReader<N> & result, SCTTIExpressionState & state, u2_t end) nxpt {
		if(false == cttiExpressionCloseIf(result, state, EXPRESSION_READER_TYPE_KEY, end))
			rtrn false;
		while(0 <= state.IndexCurrentElement && EXPRESSION_READER_TYPE_UNARY_NOT == result.Token.Storage[state.IndexCurrentElement].Type)
			if(false == cttiExpressionClose(result, state, EXPRESSION_READER_TYPE_UNARY_NOT, end)) rtrn false;
		while(0 <= state.IndexCurrentElement && EXPRESSION_READER_TYPE_TERM_AND == result.Token.Storage[state.IndexCurrentElement].Type)
			if(false == cttiExpressionClose(result, state, EXPRESSION_READER_TYPE_TERM_AND, end)) rtrn false;
		while(0 <= state.IndexCurrentElement && EXPRESSION_READER_TYPE_TERM_OR == result.Token.Storage[state.IndexCurrentElement].Type)
			if(false == cttiExpressionClose(result, state, EXPRESSION_READER_TYPE_TERM_OR, end)) rtrn false;
		while(0 <= state.IndexCurrentElement && EXPRESSION_READER_TYPE_TERM_EQUALS == result.Token.Storage[state.IndexCurrentElement].Type)
			if(false == cttiExpressionClose(result, state, EXPRESSION_READER_TYPE_TERM_EQUALS, end)) rtrn false;
		rtrn cttiExpressionCloseIf(result, state, EXPRESSION_READER_TYPE_TERM_BOOL, end);
	}

	tplt<size_t N>
	inxp void cttiExpressionSkipSpace(sc_c (&source)[N], u2_t & cursor, u2_t length) nxpt {
		while(cursor < length) {
			sc_c character = source[cursor];
			if(' ' == character || '\t' == character || '\r' == character || '\n' == character || '\f' == character || '\b' == character)
				++cursor;
			else
				break;
		}
	}

	tplt<size_t N>
	inxp SCTTIExpressionReader<N> expressionParseCTTI(sc_c (&source)[N]) nxpt {
		SCTTIExpressionReader<N> result = {};
		SCTTIExpressionState state = {};
		u2_c length = N && 0 == source[N - 1] ? (u2_t)N - 1 : (u2_t)N;
		if(0 == length) {
			cttiExpressionError(result, CTTI_EXPRESSION_PARSE_ERROR_UNEXPECTED_END, 0);
			rtrn result;
		}
		for(state.IndexCurrentChar = 0; state.IndexCurrentChar < length && result.Success(); ++state.IndexCurrentChar) {
			sc_c character = source[state.IndexCurrentChar];
			if(state.InsideString) {
				if(state.Escaping) {
					state.Escaping = false;
					if('u' == character) {
						if(state.IndexCurrentChar + 4 >= length) {
							cttiExpressionError(result, CTTI_EXPRESSION_PARSE_ERROR_UNEXPECTED_END, state.IndexCurrentChar);
							break;
						}
						SExpressionToken codepoint = {state.IndexCurrentElement, EXPRESSION_READER_TYPE_CODEPOINT, {state.IndexCurrentChar + 1, state.IndexCurrentChar + 5}, false};
						if(result.Count >= N) {
							cttiExpressionError(result, CTTI_EXPRESSION_PARSE_ERROR_TOKEN_COUNT, state.IndexCurrentChar);
							break;
						}
						result.Token.Storage[result.Count++] = codepoint;
						state.IndexCurrentChar += 4;
					}
					continue;
				}
				if('\\' == character) {
					state.Escaping = true;
					continue;
				}
				if(character == state.ClosingQuotes) {
					if(false == cttiExpressionClose(result, state, EXPRESSION_READER_TYPE_LITERAL, state.IndexCurrentChar + 1))
						break;
					state.InsideString = false;
					state.ExpectsSeparator = true;
				}
				continue;
			}

			if('/' == character && state.IndexCurrentChar + 1 < length && '/' == source[state.IndexCurrentChar + 1]) {
				cttiExpressionCloseIf(result, state, EXPRESSION_READER_TYPE_KEY, state.IndexCurrentChar);
				while(state.IndexCurrentChar < length && '\n' != source[state.IndexCurrentChar])
					++state.IndexCurrentChar;
				continue;
			}
			if(' ' == character || '\t' == character || '\r' == character || '\n' == character || '\f' == character || '\b' == character) {
				cttiExpressionCloseIf(result, state, EXPRESSION_READER_TYPE_KEY, state.IndexCurrentChar);
				cttiExpressionSkipSpace(source, state.IndexCurrentChar, length);
				--state.IndexCurrentChar;
				continue;
			}
			if(0 == result.Count)
				if(0 > cttiExpressionOpen(result, state, EXPRESSION_READER_TYPE_TERM_ANY, state.IndexCurrentChar)) break;

			switch(character) {
			default:
				if(state.ExpectsSeparator) {
					cttiExpressionError(result, CTTI_EXPRESSION_PARSE_ERROR_EXPECTED_SEPARATOR, state.IndexCurrentChar);
					break;
				}
				if( EXPRESSION_READER_TYPE_TERM_KEY == result.Token.Storage[state.IndexCurrentElement].Type
				 || EXPRESSION_READER_TYPE_TERM_INDEX == result.Token.Storage[state.IndexCurrentElement].Type
				 || EXPRESSION_READER_TYPE_TERM_ANY == result.Token.Storage[state.IndexCurrentElement].Type
				 || EXPRESSION_READER_TYPE_TERM_BOOL == result.Token.Storage[state.IndexCurrentElement].Type
				 || EXPRESSION_READER_TYPE_TERM_AND == result.Token.Storage[state.IndexCurrentElement].Type
				 || EXPRESSION_READER_TYPE_TERM_OR == result.Token.Storage[state.IndexCurrentElement].Type
				 || EXPRESSION_READER_TYPE_TERM_EQUALS == result.Token.Storage[state.IndexCurrentElement].Type
				 || EXPRESSION_READER_TYPE_UNARY_NOT == result.Token.Storage[state.IndexCurrentElement].Type)
					cttiExpressionOpen(result, state, EXPRESSION_READER_TYPE_KEY, state.IndexCurrentChar);
				break;
			case '\'': case '"':
				if(state.ExpectsSeparator || 0 > cttiExpressionOpen(result, state, EXPRESSION_READER_TYPE_LITERAL, state.IndexCurrentChar)) {
					cttiExpressionError(result, CTTI_EXPRESSION_PARSE_ERROR_EXPECTED_SEPARATOR, state.IndexCurrentChar);
					break;
				}
				state.InsideString = true;
				state.ClosingQuotes = character;
				break;
			case '.':
				state.ExpectsSeparator = false;
				if(false == cttiExpressionCloseIf(result, state, EXPRESSION_READER_TYPE_KEY, state.IndexCurrentChar)) break;
				++state.IndexCurrentChar;
				cttiExpressionSkipSpace(source, state.IndexCurrentChar, length);
				cttiExpressionOpen(result, state, EXPRESSION_READER_TYPE_KEY, state.IndexCurrentChar);
				--state.IndexCurrentChar;
				break;
			case '[':
				state.ExpectsSeparator = false;
				if(false == cttiExpressionCloseIf(result, state, EXPRESSION_READER_TYPE_KEY, state.IndexCurrentChar)) break;
				cttiExpressionOpen(result, state, EXPRESSION_READER_TYPE_TERM_INDEX, state.IndexCurrentChar);
				break;
			case ']':
				state.ExpectsSeparator = false;
				if(false == cttiExpressionCloseIf(result, state, EXPRESSION_READER_TYPE_KEY, state.IndexCurrentChar)) break;
				while(0 <= state.IndexCurrentElement && EXPRESSION_READER_TYPE_UNARY_NOT == result.Token.Storage[state.IndexCurrentElement].Type)
					if(false == cttiExpressionClose(result, state, EXPRESSION_READER_TYPE_UNARY_NOT, state.IndexCurrentChar)) break;
				if(false == cttiExpressionCloseIf(result, state, EXPRESSION_READER_TYPE_TERM_BOOL, state.IndexCurrentChar)) break;
				cttiExpressionClose(result, state, EXPRESSION_READER_TYPE_TERM_INDEX, state.IndexCurrentChar + 1);
				break;
			case '(':
				if(state.ExpectsSeparator) {
					cttiExpressionError(result, CTTI_EXPRESSION_PARSE_ERROR_EXPECTED_SEPARATOR, state.IndexCurrentChar);
					break;
				}
				cttiExpressionOpen(result, state, EXPRESSION_READER_TYPE_TERM_ANY, state.IndexCurrentChar);
				break;
			case ')':
				state.ExpectsSeparator = false;
				if(false == cttiExpressionCloseTerm(result, state, state.IndexCurrentChar)) break;
				cttiExpressionClose(result, state, EXPRESSION_READER_TYPE_TERM_ANY, state.IndexCurrentChar + 1);
				break;
			case '!':
				cttiExpressionOpen(result, state, EXPRESSION_READER_TYPE_UNARY_NOT, state.IndexCurrentChar + 1);
				break;
			case '=':
				state.ExpectsSeparator = false;
				if(state.IndexCurrentChar + 1 >= length || '=' != source[state.IndexCurrentChar + 1]) {
					cttiExpressionError(result, CTTI_EXPRESSION_PARSE_ERROR_INVALID_OPERATOR, state.IndexCurrentChar);
					break;
				}
				if(false == cttiExpressionCloseTerm(result, state, state.IndexCurrentChar)) break;
				++state.IndexCurrentChar;
				cttiExpressionOpen(result, state, EXPRESSION_READER_TYPE_TERM_EQUALS, state.IndexCurrentChar + 1);
				break;
			case '&': case '|': {
				sc_c operatorCharacter = character;
				state.ExpectsSeparator = false;
				if(state.IndexCurrentChar + 1 >= length || operatorCharacter != source[state.IndexCurrentChar + 1]) {
					cttiExpressionError(result, CTTI_EXPRESSION_PARSE_ERROR_INVALID_OPERATOR, state.IndexCurrentChar);
					break;
				}
				if(false == cttiExpressionCloseTerm(result, state, state.IndexCurrentChar)) break;
				++state.IndexCurrentChar;
				cttiExpressionOpen(result, state, '&' == operatorCharacter ? EXPRESSION_READER_TYPE_TERM_AND : EXPRESSION_READER_TYPE_TERM_OR, state.IndexCurrentChar + 1);
				break;
			}
			case '?':
				state.ExpectsSeparator = false;
				if(false == cttiExpressionCloseIf(result, state, EXPRESSION_READER_TYPE_KEY, state.IndexCurrentChar)) break;
				while(0 <= state.IndexCurrentElement && EXPRESSION_READER_TYPE_UNARY_NOT == result.Token.Storage[state.IndexCurrentElement].Type)
					if(false == cttiExpressionClose(result, state, EXPRESSION_READER_TYPE_UNARY_NOT, state.IndexCurrentChar)) break;
				while(0 <= state.IndexCurrentElement && EXPRESSION_READER_TYPE_TERM_AND == result.Token.Storage[state.IndexCurrentElement].Type)
					if(false == cttiExpressionClose(result, state, EXPRESSION_READER_TYPE_TERM_AND, state.IndexCurrentChar)) break;
				while(0 <= state.IndexCurrentElement && EXPRESSION_READER_TYPE_TERM_OR == result.Token.Storage[state.IndexCurrentElement].Type)
					if(false == cttiExpressionClose(result, state, EXPRESSION_READER_TYPE_TERM_OR, state.IndexCurrentChar)) break;
				while(0 <= state.IndexCurrentElement && EXPRESSION_READER_TYPE_TERM_EQUALS == result.Token.Storage[state.IndexCurrentElement].Type)
					if(false == cttiExpressionClose(result, state, EXPRESSION_READER_TYPE_TERM_EQUALS, state.IndexCurrentChar)) break;
				cttiExpressionOpen(result, state, EXPRESSION_READER_TYPE_TERM_BOOL, state.IndexCurrentChar + 1);
				break;
			case ':':
				state.ExpectsSeparator = false;
				if(false == cttiExpressionCloseTerm(result, state, state.IndexCurrentChar)) break;
				if(0 > cttiExpressionOpen(result, state, EXPRESSION_READER_TYPE_TERM_BOOL, state.IndexCurrentChar + 1)) break;
				result.Token.Storage[state.IndexCurrentElement].ClosingCondition = true;
				break;
			case '{':
				if(state.ExpectsSeparator) {
					cttiExpressionError(result, CTTI_EXPRESSION_PARSE_ERROR_EXPECTED_SEPARATOR, state.IndexCurrentChar);
					break;
				}
				cttiExpressionOpen(result, state, EXPRESSION_READER_TYPE_TERM_KEY, state.IndexCurrentChar);
				break;
			case '}':
				state.ExpectsSeparator = false;
				if(false == cttiExpressionCloseTerm(result, state, state.IndexCurrentChar)) break;
				cttiExpressionClose(result, state, EXPRESSION_READER_TYPE_TERM_KEY, state.IndexCurrentChar);
				break;
			}
		}
		if(result.Success() && state.InsideString)
			cttiExpressionError(result, CTTI_EXPRESSION_PARSE_ERROR_UNEXPECTED_END, length);
		if(result.Success()) {
			state.IndexCurrentChar = length;
			cttiExpressionCloseTerm(result, state, length);
			cttiExpressionCloseIf(result, state, EXPRESSION_READER_TYPE_TERM_ANY, length);
			if(0 != state.NestLevel)
				cttiExpressionError(result, CTTI_EXPRESSION_PARSE_ERROR_INVALID_NESTING, length);
		}
		rtrn result;
	}
} // nmsp

#endif // GPK_EXPRESSION_CTTI_H
