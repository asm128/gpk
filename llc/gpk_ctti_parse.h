#include "gpk_view.h"

#ifndef GPK_CTTI_PARSE_H
#define GPK_CTTI_PARSE_H

namespace gpk
{
	inxp b8_t cttiSpace		(sc_c character) nxpt { rtrn ' ' == character || '\t' == character || '\r' == character || '\n' == character; }
	inxp b8_t cttiNameStart	(sc_c character) nxpt { rtrn '_' == character || (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z'); }
	inxp b8_t cttiNamePart	(sc_c character) nxpt { rtrn cttiNameStart(character) || (character >= '0' && character <= '9'); }
	tplt<tpnm T>
	inxp auto cttiFirstMember(cnst T & source) nxpt {
		cnst auto & [result] = source;
		rtrn result;
	}

	stct SCTTISpan {
		u2_t		Offset	= {};
		u2_t		Count	= {};
		inxp b8_t	empty	() cnst nxpt { rtrn 0 == Count; }
	};

	tplt<size_t N>
	inxp SCTTISpan cttiTrim(sc_c (&text)[N], u2_t offset, u2_t count) nxpt {
		while(count && cttiSpace(text[offset]))
			++offset, --count;
		while(count && cttiSpace(text[offset + count - 1]))
			--count;
		rtrn {offset, count};
	}
	tplt<size_t N>
	inxp u2_t cttiFind(sc_c (&text)[N], cnst SCTTISpan span, sc_c delimiter) nxpt {
		for(u2_t iChar = 0; iChar < span.Count; ++iChar)
			if(delimiter == text[span.Offset + iChar])
				rtrn span.Offset + iChar;
		rtrn span.Offset + span.Count;
	}
	tplt<size_t N>
	inxp b8_t cttiInteger(sc_c (&text)[N], SCTTISpan span, s3_t & value) nxpt {
		span = cttiTrim(text, span.Offset, span.Count);
		b8_t negative = false;
		if(span.Count && ('-' == text[span.Offset] || '+' == text[span.Offset])) {
			negative = '-' == text[span.Offset];
			++span.Offset;
			--span.Count;
		}
		u0_t base = 10;
		if(2 <= span.Count && '0' == text[span.Offset] && ('x' == text[span.Offset + 1] || 'X' == text[span.Offset + 1])) {
			base = 16;
			span.Offset += 2;
			span.Count -= 2;
		}
		u3_t result = 0;
		u2_t digitCount = 0;
		while(digitCount < span.Count) {
			sc_c character = text[span.Offset + digitCount];
			u0_t digit = character >= '0' && character <= '9' ? u0_t(character - '0')
				: character >= 'a' && character <= 'f' ? u0_t(character - 'a' + 10)
				: character >= 'A' && character <= 'F' ? u0_t(character - 'A' + 10)
				: 0xFFU;
			if(digit >= base)
				break;
			result = result * base + digit;
			++digitCount;
		}
		if(0 == digitCount)
			rtrn false;
		span.Offset += digitCount;
		span.Count -= digitCount;
		while(span.Count && ('u' == text[span.Offset] || 'U' == text[span.Offset] || 'l' == text[span.Offset] || 'L' == text[span.Offset])) {
			++span.Offset;
			--span.Count;
		}
		span = cttiTrim(text, span.Offset, span.Count);
		if(span.Count && (span.Count < 2 || '/' != text[span.Offset] || '/' != text[span.Offset + 1]))
			rtrn false;
		value = negative ? -(s3_t)result : (s3_t)result;
		rtrn true;
	}

	tplt<size_t N>
	inxp b8_t cttiIdentifier(sc_c (&text)[N], cnst SCTTISpan span) nxpt {
		if(span.empty() || false == cttiNameStart(text[span.Offset]))
			rtrn false;
		for(u2_t iChar = 1; iChar < span.Count; ++iChar)
			if(false == cttiNamePart(text[span.Offset + iChar]))
				rtrn false;
		rtrn true;
	}
	tplt<size_t N>
	inxp u2_t cttiIdentifierError(sc_c (&text)[N], cnst SCTTISpan span) nxpt {
		if(span.empty() || false == cttiNameStart(text[span.Offset]))
			rtrn span.Offset;
		for(u2_t iChar = 1; iChar < span.Count; ++iChar)
			if(false == cttiNamePart(text[span.Offset + iChar]))
				rtrn span.Offset + iChar;
		rtrn span.Offset + span.Count;
	}
	tplt<size_t NText, size_t NValue>
	inxp b8_t cttiSpanEquals(sc_c (&text)[NText], cnst SCTTISpan span, sc_c (&value)[NValue]) nxpt {
		if(span.Count != NValue - 1)
			rtrn false;
		for(u2_t iChar = 0; iChar < span.Count; ++iChar)
			if(text[span.Offset + iChar] != value[iChar])
				rtrn false;
		rtrn true;
	}
}

#endif // GPK_CTTI_PARSE_H
