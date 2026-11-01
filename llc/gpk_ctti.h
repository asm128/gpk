#include "llc_typeint.h"

#ifndef LLC_CTTI_H
#define LLC_CTTI_H

namespace llc
{
	enum CTTI_TYPE : u0_t
		{ CTTI_TYPE_UNKNOWN
		, CTTI_TYPE_B8
		, CTTI_TYPE_SC
		, CTTI_TYPE_UC
		, CTTI_TYPE_U0
		, CTTI_TYPE_U1
		, CTTI_TYPE_U2
		, CTTI_TYPE_U3
		, CTTI_TYPE_S0
		, CTTI_TYPE_S1
		, CTTI_TYPE_S2
		, CTTI_TYPE_S3
		, CTTI_TYPE_F2
		, CTTI_TYPE_F3
		};

	enum CTTI_PARSE_STATE : u0_t
		{ CTTI_PARSE_STATE_STRUCT_NAME
		, CTTI_PARSE_STATE_TYPE
		, CTTI_PARSE_STATE_MEMBER_NAME
		, CTTI_PARSE_STATE_COMPLETE
		};

	enum CTTI_PARSE_ERROR : u0_t
		{ CTTI_PARSE_ERROR_NONE
		, CTTI_PARSE_ERROR_INVALID_STRUCT_NAME
		, CTTI_PARSE_ERROR_EXPECTED_TYPE
		, CTTI_PARSE_ERROR_UNSUPPORTED_TYPE
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
		SCTTISpan	TypeName	= {};
		SCTTISpan	Name		= {};
		CTTI_TYPE	Type		= CTTI_TYPE_UNKNOWN;
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

	tplt<size_t NText, size_t NLiteral>
	inxp bool cttiSpanEquals(cnst sc_t (&text)[NText], cnst SCTTISpan span, cnst sc_t (&literal)[NLiteral]) nxpt {
		if(span.Count != NLiteral - 1)
			rtrn false;
		for(u2_t iChar = 0; iChar < span.Count; ++iChar)
			if(text[span.Offset + iChar] != literal[iChar])
				rtrn false;
		rtrn true;
	}
	tplt<size_t NText>
	inxp bool cttiSpanEqualsP(cnst sc_t (&text)[NText], cnst SCTTISpan span, cnst sc_c * literal) nxpt {
		u2_t literalLength = 0;
		while(literal[literalLength])
			++literalLength;
		if(span.Count != literalLength)
			rtrn false;
		for(u2_t iChar = 0; iChar < span.Count; ++iChar)
			if(text[span.Offset + iChar] != literal[iChar])
				rtrn false;
		rtrn true;
	}

	tplt<size_t NText, size_t NLiteral>
	inxp bool cttiSpanStartsWith(cnst sc_t (&text)[NText], cnst SCTTISpan span, cnst sc_t (&literal)[NLiteral]) nxpt {
		if(span.Count < NLiteral - 1)
			rtrn false;
		for(u2_t iChar = 0; iChar < NLiteral - 1; ++iChar)
			if(text[span.Offset + iChar] != literal[iChar])
				rtrn false;
		rtrn true;
	}

	tplt<size_t N>
	inxp CTTI_TYPE cttiType(cnst sc_t (&text)[N], SCTTISpan span) nxpt {
		if(cttiSpanStartsWith(text, span, "::llc::"))
			span.Offset += 7, span.Count -= 7;
		else if(cttiSpanStartsWith(text, span, "llc::"))
			span.Offset += 5, span.Count -= 5;

		rtrn
			cttiSpanEquals(text, span, "b8_t") ? CTTI_TYPE_B8 :
			cttiSpanEquals(text, span, "sc_t") ? CTTI_TYPE_SC :
			cttiSpanEquals(text, span, "uc_t") ? CTTI_TYPE_UC :
			cttiSpanEquals(text, span, "u0_t") ? CTTI_TYPE_U0 :
			cttiSpanEquals(text, span, "u1_t") ? CTTI_TYPE_U1 :
			cttiSpanEquals(text, span, "u2_t") ? CTTI_TYPE_U2 :
			cttiSpanEquals(text, span, "u3_t") ? CTTI_TYPE_U3 :
			cttiSpanEquals(text, span, "s0_t") ? CTTI_TYPE_S0 :
			cttiSpanEquals(text, span, "s1_t") ? CTTI_TYPE_S1 :
			cttiSpanEquals(text, span, "s2_t") ? CTTI_TYPE_S2 :
			cttiSpanEquals(text, span, "s3_t") ? CTTI_TYPE_S3 :
			cttiSpanEquals(text, span, "f2_t") ? CTTI_TYPE_F2 :
			cttiSpanEquals(text, span, "f3_t") ? CTTI_TYPE_F3 :
			CTTI_TYPE_UNKNOWN;
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

		result.State = CTTI_PARSE_STATE_TYPE;
		SCTTISpan	currentType	= {};
		CTTI_TYPE	currentTypeId	= CTTI_TYPE_UNKNOWN;
		bool		firstDeclarator	= true;
		u2_t		cursor			= 0;
		cnst u2_t	textLength		= (u2_t)NMemberText - 1;
		while(true) {
			while(cursor < textLength && cttiSpace(result.MemberText[cursor]))
				++cursor;
			if(cursor == textLength) {
				if(false == firstDeclarator) {
					result.Error		= CTTI_PARSE_ERROR_EXPECTED_MEMBER_NAME;
					result.State		= CTTI_PARSE_STATE_MEMBER_NAME;
					result.ErrorOffset	= cursor;
					rtrn result;
				}
				result.State = CTTI_PARSE_STATE_COMPLETE;
				rtrn result;
			}

			cnst u2_t segmentOffset = cursor;
			while(cursor < textLength && ',' != result.MemberText[cursor] && ';' != result.MemberText[cursor])
				++cursor;
			cnst SCTTISpan segment = cttiTrim(result.MemberText, segmentOffset, cursor - segmentOffset);
			if(segment.empty()) {
				result.Error		= firstDeclarator ? CTTI_PARSE_ERROR_EXPECTED_TYPE : CTTI_PARSE_ERROR_EXPECTED_MEMBER_NAME;
				result.State		= firstDeclarator ? CTTI_PARSE_STATE_TYPE : CTTI_PARSE_STATE_MEMBER_NAME;
				result.ErrorOffset	= segmentOffset;
				rtrn result;
			}

			SCTTISpan memberName = segment;
			if(firstDeclarator) {
				u2_t nameOffset = segment.Offset + segment.Count;
				while(nameOffset > segment.Offset && cttiNamePart(result.MemberText[nameOffset - 1]))
					--nameOffset;
				memberName = {nameOffset, segment.Offset + segment.Count - nameOffset};
				if(false == cttiIdentifier(result.MemberText, memberName)) {
					result.Error		= CTTI_PARSE_ERROR_INVALID_MEMBER_NAME;
					result.State		= CTTI_PARSE_STATE_MEMBER_NAME;
					result.ErrorOffset	= cttiIdentifierError(result.MemberText, memberName);
					rtrn result;
				}
				currentType = cttiTrim(result.MemberText, segment.Offset, nameOffset - segment.Offset);
				if(currentType.empty()) {
					result.Error		= CTTI_PARSE_ERROR_EXPECTED_TYPE;
					result.State		= CTTI_PARSE_STATE_TYPE;
					result.ErrorOffset	= segment.Offset;
					rtrn result;
				}
				currentTypeId = cttiType(result.MemberText, currentType);
				if(CTTI_TYPE_UNKNOWN == currentTypeId) {
					result.Error		= CTTI_PARSE_ERROR_UNSUPPORTED_TYPE;
					result.State		= CTTI_PARSE_STATE_TYPE;
					result.ErrorOffset	= currentType.Offset;
					rtrn result;
				}
			}
			else if(false == cttiIdentifier(result.MemberText, memberName)) {
				result.Error		= CTTI_PARSE_ERROR_INVALID_MEMBER_NAME;
				result.State		= CTTI_PARSE_STATE_MEMBER_NAME;
				result.ErrorOffset	= cttiIdentifierError(result.MemberText, memberName);
				rtrn result;
			}

			result.Members[result.Count++] = {currentType, memberName, currentTypeId};
			if(cursor == textLength) {
				result.Error		= CTTI_PARSE_ERROR_EXPECTED_SEMICOLON;
				result.State		= CTTI_PARSE_STATE_MEMBER_NAME;
				result.ErrorOffset	= cursor;
				rtrn result;
			}
			if(',' == result.MemberText[cursor]) {
				firstDeclarator	= false;
				result.State		= CTTI_PARSE_STATE_MEMBER_NAME;
			}
			else {
				firstDeclarator	= true;
				result.State		= CTTI_PARSE_STATE_TYPE;
				currentType		= {};
				currentTypeId	= CTTI_TYPE_UNKNOWN;
			}
			++cursor;
		}
	}
}

#endif // LLC_CTTI_H
