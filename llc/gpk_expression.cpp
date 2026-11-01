#include "gpk_expression.h"

GPK_USING_TYPEINT();

#define gpk_expression_info_printf(...) //info_printf

sttc	gpk::err_t	expressionReaderViews					(::gpk::apod<::gpk::SExpressionToken> & tokens, ::gpk::aobj<::gpk::vcst_t> & views, ::gpk::vcst_t expression) {
	for(::gpk::u2_t iTag = 0; iTag < tokens.size(); ++iTag) {
		const ::gpk::SExpressionToken								& type									 = tokens[iTag];
		if(iTag && (::gpk::EXPRESSION_READER_TYPE_TERM_INDEX == type.Type || ::gpk::EXPRESSION_READER_TYPE_TERM_KEY == type.Type || ::gpk::EXPRESSION_READER_TYPE_LITERAL
			== type.Type) && (type.Span.End - type.Span.Begin) <= expression.size()) {// doesn't count for root expression
			if_fail_fe(views.push_back({&expression[type.Span.Begin + 1], type.Span.End - type.Span.Begin - 2}));
		}
		else
			if_fail_fe(views.push_back({&expression[type.Span.Begin], type.Span.End - type.Span.Begin}));
	}
	return 0;
}

sttc	gpk::err_t	expressionTreeRebuild					(::gpk::view<::gpk::SExpressionToken> & in_object, ::gpk::apobj<::gpk::SExpressionNode> & out_nodes)								{
	::gpk::apobj<::gpk::SExpressionNode>		& tree									= out_nodes;
	if_fail_fef(tree.resize(in_object.size()), "Out of memory? Object count: %u.", in_object.size());

	// -- Build all nodes linearly, without assigning the children
	for(::gpk::u2_t iObject = 0; iObject < tree.size(); ++iObject) {
		::gpk::pobj<::gpk::SExpressionNode>							& nodeCurrent										= tree[iObject];
		nodeCurrent->Token											= &in_object[iObject];
		nodeCurrent->Parent											= ((::gpk::u2_t)nodeCurrent->Token->ParentIndex < tree.size()) ? (gpk::SExpressionNode*)tree[nodeCurrent->Token->ParentIndex] : nullptr;
		nodeCurrent->ObjectIndex									= iObject;
	}

	// -- Assign the children to every object of the hierarchy
	for(::gpk::u2_t iObject = 0, countNodes = tree.size(); iObject < countNodes; ++iObject) {
		for(::gpk::u2_t iOther = 0; iOther < countNodes; ++iOther) {
			const ::gpk::pobj<::gpk::SExpressionNode>					& nodeOther											= tree[iOther];
			if(((::gpk::u2_t)nodeOther->Token->ParentIndex) == iObject)
				if_fail_fef(tree[iObject]->Children.push_back(nodeOther), "%s", "Failed to push tree node. Out of memory?");
		}
	}
	return 0;
}

sttc	gpk::err_t	expressionReaderCloseType				(::gpk::SExpressionReaderState & stateReader, ::gpk::apod<::gpk::SExpressionToken> & tokens, ::gpk::EXPRESSION_READER_TYPE type, s2_t indexCurrentChar) {
	if_true_fef(type != stateReader.CurrentElement->Type, "Invalid type to close: %s. Current type: %s.", ::gpk::get_value_label(type).begin(), ::gpk::get_value_label(stateReader.CurrentElement->Type).begin());
	stateReader.CurrentElement->Span.End						= indexCurrentChar;
	stateReader.IndexCurrentElement								= stateReader.CurrentElement->ParentIndex;
	stateReader.CurrentElement									= (-1 != stateReader.IndexCurrentElement) ? &tokens[stateReader.IndexCurrentElement] : nullptr;
	gpk_expression_info_printf("Closing expression type: %s. Parent type: %s. Nest level: %i.", ::gpk::get_value_label(type).begin(), stateReader.CurrentElement ? ::gpk::get_value_label(stateReader.CurrentElement->Type).begin() : "UNKNOWN", stateReader.NestLevel);
	--stateReader.NestLevel;
	return 0;
}

sttc	gpk::err_t	expressionReaderCloseIfType				(::gpk::SExpressionReaderState & stateReader, ::gpk::apod<::gpk::SExpressionToken> & tokens, ::gpk::EXPRESSION_READER_TYPE type)	{
	if_null_vif(0, stateReader.CurrentElement, "Cannot close %s. Nothing to close.", ::gpk::get_value_label(type).begin());
	if(type == stateReader.CurrentElement->Type) {
		if_fail_fef(::expressionReaderCloseType(stateReader, tokens, type, stateReader.IndexCurrentChar), "Failed to close type: %s. Why would this happen?", ::gpk::get_value_label(type).begin());
		return 1;
	}
	return 0;
}

sttc	gpk::err_t	expressionReaderOpenLevel				(::gpk::SExpressionReaderState & stateReader, ::gpk::apod<::gpk::SExpressionToken> & tokens, ::gpk::EXPRESSION_READER_TYPE type, ::gpk::u2_t iChar)	{
	stateReader.IndexCurrentElement								= tokens.push_back({stateReader.IndexCurrentElement, type, {iChar, iChar}});
	stateReader.CurrentElement									= &tokens[stateReader.IndexCurrentElement];
	++stateReader.NestLevel;
	gpk_expression_info_printf("Entering expression type: %s. Parent type: %s. Nest level: %u.", ::gpk::get_value_label(stateReader.CurrentElement->Type).begin(), stateReader.CurrentElement ? ::gpk::get_value_label(stateReader.CurrentElement->Type).begin() : "UNKNOWN", stateReader.NestLevel);
	return 0;
}

#define seterr_break_if(condition, format, ...)	\
	if(condition) {								\
		errVal					= -1;			\
		error_printf(format, __VA_ARGS__);		\
		break;									\
	}

#define seterr_if(condition, format, ...)	\
	if(condition) {							\
		errVal					= -1;		\
		error_printf(format, __VA_ARGS__);	\
	}

sttc	gpk::err_t	expressionReaderCloseTerm							(::gpk::SExpressionReaderState & stateReader, ::gpk::apod<::gpk::SExpressionToken> & tokens)	{
	if_fail_fef(::expressionReaderCloseIfType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_KEY			), "%s", "Failed to close type.");
	while(::gpk::EXPRESSION_READER_TYPE_UNARY_NOT == stateReader.CurrentElement->Type)
		if_fail_fef(::expressionReaderCloseType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_UNARY_NOT	, stateReader.IndexCurrentChar), "%s", "Failed to close type.");
	while(::gpk::EXPRESSION_READER_TYPE_TERM_AND == stateReader.CurrentElement->Type)
		if_fail_fef(::expressionReaderCloseType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_AND	, stateReader.IndexCurrentChar), "%s", "Failed to close type.");
	while(::gpk::EXPRESSION_READER_TYPE_TERM_OR == stateReader.CurrentElement->Type)
		if_fail_fef(::expressionReaderCloseType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_OR	, stateReader.IndexCurrentChar), "%s", "Failed to close type.");
	while(::gpk::EXPRESSION_READER_TYPE_TERM_EQUALS == stateReader.CurrentElement->Type)
		if_fail_fef(::expressionReaderCloseType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_EQUALS	, stateReader.IndexCurrentChar), "%s", "Failed to close type.");
	if_fail_fef(::expressionReaderCloseIfType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_BOOL	), "Failed to open key at index %i", stateReader.IndexCurrentChar);
	return 0;
}

			gpk::err_t 		expressionReaderProcessStringCharacter				(::gpk::SExpressionReaderState & stateReader, ::gpk::apod<::gpk::SExpressionToken> & tokens, const ::gpk::vcst_t & expression)	{
	::gpk::SExpressionToken											currentElement										= {};
	gpk::err_t 						errVal												= 0;
	switch(stateReader.CharCurrent) {
	default:
		seterr_break_if(stateReader.CharCurrent < 0x20 || ((u0_t)stateReader.CharCurrent) > 0xFE, "Invalid character: %i (%u) '%c'.", stateReader.CharCurrent, (::gpk::uc_t)stateReader.CharCurrent, stateReader.CharCurrent);
		seterr_break_if(stateReader.Escaping, "Cannot escape character: %i (%u) '%c'.", stateReader.CharCurrent, (u0_t)stateReader.CharCurrent, stateReader.CharCurrent);
		break;
	case 'b': case 'f': case 'n': case 'r': case 't':
		break;	// these characters are both valid as part of the string and as escapable characters.
	case '\b': case '\f': case '\n': case '\r': case '\t':
		break;	// these characters are both valid as part of the string and as escapable characters.
	case 'u':
		if(false == stateReader.Escaping)
			break;
		stateReader.IndexCurrentChar								+= 1;	// skip the u to get the next 4 digits.
		seterr_break_if(expression.size() - stateReader.IndexCurrentChar < 4, "End of stream during unicode code point parsing. JSON length: %u. Current index: %u.", expression.size(), stateReader.IndexCurrentChar);
		info_printf("Unicode code point found: %4.4s", &expression[stateReader.IndexCurrentChar]);
		currentElement												= {stateReader.IndexCurrentElement, ::gpk::EXPRESSION_READER_TYPE_CODEPOINT, {stateReader.IndexCurrentChar, stateReader.IndexCurrentChar + 4}};
		seterr_if(gpk::failed(tokens.push_back(currentElement)), "%s", "");
		stateReader.CurrentElement									= &tokens[stateReader.IndexCurrentElement];
		stateReader.IndexCurrentChar								+= 3;	// Parse unicode code point
		break;
	case '\\'	:
		if(false == stateReader.Escaping) {
			stateReader.Escaping										= true;
			return 0;
		}
		break;
	case '"'	:case '\''	:
		if(false == stateReader.Escaping && stateReader.ClosingQuotes == stateReader.CharCurrent) {
			seterr_if(gpk::failed(::expressionReaderCloseType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_LITERAL, stateReader.IndexCurrentChar + 1)), "Failed to close string elment. %s", "Unknown error.");
			if(stateReader.CurrentElement->ClosingCondition && stateReader.CurrentElement->Type == ::gpk::EXPRESSION_READER_TYPE_TERM_BOOL)
				if_fail_fef(::expressionReaderCloseType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_BOOL, stateReader.IndexCurrentChar + 1), "%s", "Failed to close type.");
			stateReader.InsideString									= false;
			if(stateReader.IndexCurrentChar >= expression.size() - 1) { // if this is the last character, make sure to close open key and root expression
				if(stateReader.IndexCurrentChar == expression.size() - 1)
					++stateReader.IndexCurrentChar;
				if_fail_fef(::expressionReaderCloseTerm	(stateReader, tokens), "Failed to close term at index %i", stateReader.IndexCurrentChar);
				if_fail_fef(::expressionReaderCloseIfType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_KEY), "%s", "Failed to close type.");
				if_fail_fef(::expressionReaderCloseIfType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_ANY), "%s", "Failed to close type.");
			}
			stateReader.ExpectsSeparator								= true;	// actually we expect the separator AFTER calling jsonCloseElement(). However, such function doesn't care about this value, so we can simplify the code by reversing the operations.
		}
	}
	stateReader.Escaping										= false;
	return errVal;
}

static	bool												isSpaceCharacter		(const sc_t characterToTest)		{
	switch(characterToTest) {
	case ' ': case '\t': case '\r': case '\n'	: case '\f'	: case '\b'	:
		return true;
	default:
		return false;
	}
}

//static	bool												isAnyOfCharacters		(const ::gpk::vcst_t & charactersToFind, const sc_t characterToTest)		{
//	for(::gpk::u2_t iCharacter = 0; iCharacter < charactersToFind.size(); ++iCharacter)
//		if(characterToTest == charactersToFind[iCharacter])
//			return true;
//	return false;
//}

sttc	gpk::err_t	skipToNextCharacter						(::gpk::u2_t & indexCurrentChar, const ::gpk::vcst_t & expression)		{
	while(indexCurrentChar < expression.size()) {
		if(::isSpaceCharacter(expression[indexCurrentChar]))
			++indexCurrentChar;
		else
			break;
	}
	return 0;
}

sttc	gpk::err_t	expressionReaderProcessDocCharacter		(::gpk::SExpressionReaderState & stateReader, ::gpk::apod<::gpk::SExpressionToken> & tokens, const ::gpk::vcst_t & expression)	{
	if(0 == tokens.size())
		if_fail_fef(::expressionReaderOpenLevel(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_ANY, stateReader.IndexCurrentChar), "Failed to open expression at index %i", stateReader.IndexCurrentChar);

#define test_first_position()	if_true_fef(0 == stateReader.CharCurrent, "Character found at invalid position. Index: %u. Character: %c.", stateReader.IndexCurrentChar, stateReader.CharCurrent);
#define skip_if_escaping()		if(true == stateReader.Escaping) break

	switch(stateReader.CharCurrent) {
	case '"'	:case '\''	:
		if_true_fef(stateReader.ExpectsSeparator, "Separator expected, found: %c (%i).", stateReader.CharCurrent, (s2_t)stateReader.CharCurrent);
		if_fail_fef(::expressionReaderOpenLevel(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_LITERAL, stateReader.IndexCurrentChar), "Failed to open string at index %i", stateReader.IndexCurrentChar);	// skip the " character in order to set the begin the string
		stateReader.InsideString									= true;
		stateReader.ClosingQuotes									= stateReader.CharCurrent;
		break;
	default	:
		if_true_fef(stateReader.ExpectsSeparator, "Separator expected, found: %c (%i).", stateReader.CharCurrent, (s2_t)stateReader.CharCurrent);
		if( ::gpk::EXPRESSION_READER_TYPE_TERM_KEY		== stateReader.CurrentElement->Type
		 || ::gpk::EXPRESSION_READER_TYPE_TERM_INDEX	== stateReader.CurrentElement->Type
		 || ::gpk::EXPRESSION_READER_TYPE_TERM_ANY		== stateReader.CurrentElement->Type
		 || ::gpk::EXPRESSION_READER_TYPE_TERM_BOOL		== stateReader.CurrentElement->Type
		 || ::gpk::EXPRESSION_READER_TYPE_TERM_AND		== stateReader.CurrentElement->Type
		 || ::gpk::EXPRESSION_READER_TYPE_TERM_OR		== stateReader.CurrentElement->Type
		 || ::gpk::EXPRESSION_READER_TYPE_TERM_EQUALS	== stateReader.CurrentElement->Type
		 || ::gpk::EXPRESSION_READER_TYPE_UNARY_NOT		== stateReader.CurrentElement->Type
		 ) {
			if_fail_fef(::expressionReaderOpenLevel(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_KEY, stateReader.IndexCurrentChar), "Failed to open key at index %i", stateReader.IndexCurrentChar);
		}
		else if(0x20 > stateReader.CharCurrent || 0x7F < stateReader.CharCurrent)
			error_printf("Invalid character: %c. Only ascii printable characters are allowed in expressions.", stateReader.CharCurrent);
		break;
	case '/':
		skip_if_escaping();
		if( ::gpk::EXPRESSION_READER_TYPE_KEY	== stateReader.CurrentElement->Type ) {
			if_fail_fef(::expressionReaderCloseType(stateReader, tokens, stateReader.CurrentElement->Type, stateReader.IndexCurrentChar), "Failed to close type: %s.", ::gpk::get_value_label(stateReader.CurrentElement->Type).begin(), "%s", "Failed to close type.");
			stateReader.ExpectsSeparator								= true;
		}
		if(stateReader.IndexCurrentChar + 1 < expression.size() && '/' == expression[stateReader.IndexCurrentChar + 1]) {	// This is an end-of-line comment.
			while(stateReader.IndexCurrentChar < expression.size() &&  '\n' != expression[stateReader.IndexCurrentChar])
				++stateReader.IndexCurrentChar;
		}
		break;
	case '=':
		stateReader.ExpectsSeparator								= false;
		skip_if_escaping();
		if_true_fef(stateReader.IndexCurrentChar >= expression.size() - 1 || '=' != expression[stateReader.IndexCurrentChar + 1], "%s", "Invalid syntax.");
		if_fail_fef(::expressionReaderCloseTerm(stateReader, tokens), "Failed to close term at index %i", stateReader.IndexCurrentChar);
		++stateReader.IndexCurrentChar;
		if_fail_fef(::expressionReaderOpenLevel	(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_EQUALS	, stateReader.IndexCurrentChar + 1), "Failed to open key at index %i", stateReader.IndexCurrentChar);

		break;
	case ' ': case '\t': case '\r': case '\n'	: case '\f'	: case '\b'	:
		skip_if_escaping();
		if(stateReader.CurrentElement && ::gpk::EXPRESSION_READER_TYPE_KEY	== stateReader.CurrentElement->Type) {
			if_fail_fef(::expressionReaderCloseType(stateReader, tokens, stateReader.CurrentElement->Type, stateReader.IndexCurrentChar), "Failed to close type: %s.", ::gpk::get_value_label(stateReader.CurrentElement->Type).begin(), "%s", "Failed to close type.");
			stateReader.ExpectsSeparator								= true;
		}
		::skipToNextCharacter(stateReader.IndexCurrentChar, expression);	// Skip blanks after this.
		--stateReader.IndexCurrentChar;
		break;
	case '&':
		stateReader.ExpectsSeparator								= false;
		skip_if_escaping();
		if_true_fef(stateReader.IndexCurrentChar >= expression.size() - 1 || '&' != expression[stateReader.IndexCurrentChar + 1], "%s", "Invalid syntax.");
		if_fail_fef(::expressionReaderCloseTerm(stateReader, tokens), "Failed to close term at index %i", stateReader.IndexCurrentChar);
		++stateReader.IndexCurrentChar;
		if_fail_fef(::expressionReaderOpenLevel	(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_AND	, stateReader.IndexCurrentChar + 1), "Failed to open key at index %i", stateReader.IndexCurrentChar);
		break;
	case '|':
		stateReader.ExpectsSeparator								= false;
		skip_if_escaping();
		if_true_fef(stateReader.IndexCurrentChar >= expression.size() - 2 || '|' != expression[stateReader.IndexCurrentChar + 1], "%s", "Invalid syntax.");
		if_fail_fef(::expressionReaderCloseTerm(stateReader, tokens), "Failed to close term at index %i", stateReader.IndexCurrentChar);
		++stateReader.IndexCurrentChar;
		if_fail_fef(::expressionReaderOpenLevel	(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_OR		, stateReader.IndexCurrentChar + 1), "Failed to open key at index %i", stateReader.IndexCurrentChar);
		break;
	case '.':
		stateReader.ExpectsSeparator								= false;
		test_first_position();
		skip_if_escaping();
		if_fail_fef(::expressionReaderCloseIfType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_KEY			), "%s", "Failed to close type.");
		++stateReader.IndexCurrentChar;
		::skipToNextCharacter(stateReader.IndexCurrentChar, expression);	// Skip blanks after this.
		if_fail_fef(::expressionReaderOpenLevel	(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_KEY			, stateReader.IndexCurrentChar), "Failed to open key at index %i", stateReader.IndexCurrentChar);
		--stateReader.IndexCurrentChar;
		break;
	case '?':
		test_first_position();
		stateReader.ExpectsSeparator								= false;
		skip_if_escaping();
		if_fail_fef(::expressionReaderCloseIfType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_KEY			), "%s", "Failed to close type.");
		while(::gpk::EXPRESSION_READER_TYPE_UNARY_NOT == stateReader.CurrentElement->Type)
			if_fail_fef(::expressionReaderCloseType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_UNARY_NOT	, stateReader.IndexCurrentChar), "%s", "Failed to close type.");
		while(::gpk::EXPRESSION_READER_TYPE_TERM_AND == stateReader.CurrentElement->Type)
			if_fail_fef(::expressionReaderCloseType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_AND	, stateReader.IndexCurrentChar), "%s", "Failed to close type.");
		while(::gpk::EXPRESSION_READER_TYPE_TERM_OR == stateReader.CurrentElement->Type)
			if_fail_fef(::expressionReaderCloseType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_OR	, stateReader.IndexCurrentChar), "%s", "Failed to close type.");
		while(::gpk::EXPRESSION_READER_TYPE_TERM_EQUALS == stateReader.CurrentElement->Type)
			if_fail_fef(::expressionReaderCloseType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_EQUALS	, stateReader.IndexCurrentChar), "%s", "Failed to close type.");
		if_fail_fef(::expressionReaderOpenLevel	(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_BOOL	, stateReader.IndexCurrentChar + 1), "Failed to open evaluation at index %i", stateReader.IndexCurrentChar + 1);
		break;
	case ':':
		test_first_position();
		stateReader.ExpectsSeparator								= false;
		skip_if_escaping();
		if_fail_fef(::expressionReaderCloseTerm	(stateReader, tokens), "Failed to close term at index %i", stateReader.IndexCurrentChar);
		if_fail_fef(::expressionReaderOpenLevel	(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_BOOL	, stateReader.IndexCurrentChar + 1), "Failed to open evaluation at index %i", stateReader.IndexCurrentChar + 1);
		stateReader.CurrentElement->ClosingCondition				= true;
		break;
	case '{':
		skip_if_escaping();
		if_true_fef(stateReader.ExpectsSeparator, "%s", "This character can only be used after the . separator, so this requirement should be canceled already.");
		if_fail_fef(::expressionReaderOpenLevel	(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_KEY, stateReader.IndexCurrentChar), "Failed to open expression at index %i", stateReader.IndexCurrentChar);	// Enter sub-expression
		break;
	case '}':
		test_first_position();
		stateReader.ExpectsSeparator								= false;
		skip_if_escaping();
		if_fail_fef(::expressionReaderCloseTerm	(stateReader, tokens), "Failed to close term at index %i", stateReader.IndexCurrentChar);
		if_fail_fef(::expressionReaderCloseType	(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_KEY	, stateReader.IndexCurrentChar), "Failed to close type: %s.", ::gpk::get_value_label(stateReader.CurrentElement->Type).begin());
		break;
	case '[':
		stateReader.ExpectsSeparator								= false;
		skip_if_escaping();
		if_fail_fef(::expressionReaderCloseIfType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_KEY			), "%s", "Failed to close type.");
		if_fail_fef(::expressionReaderOpenLevel	(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_INDEX	, stateReader.IndexCurrentChar), "Failed to open expression at index %i", stateReader.IndexCurrentChar);	// Enter sub-expression
		break;
	case ']':
		test_first_position();
		stateReader.ExpectsSeparator								= false;
		skip_if_escaping();
		if_fail_fef(::expressionReaderCloseIfType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_KEY			), "%s", "Failed to close type.");
		while(::gpk::EXPRESSION_READER_TYPE_UNARY_NOT == stateReader.CurrentElement->Type)
			if_fail_fef(::expressionReaderCloseType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_UNARY_NOT	, stateReader.IndexCurrentChar), "%s", "Failed to close type.");
		if_fail_fef(::expressionReaderCloseIfType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_BOOL	), "%s", "Failed to close type.");
		if_fail_fef(::expressionReaderCloseType	(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_INDEX	, stateReader.IndexCurrentChar + 1), "Failed to close type: %s.", ::gpk::get_value_label(stateReader.CurrentElement->Type).begin());
		break;
	case '(':
		skip_if_escaping();
		if_true_fef(stateReader.ExpectsSeparator, "%s", "This character can only be used after some separator, so this requirement should be canceled already.");
		if_fail_fef(::expressionReaderOpenLevel	(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_ANY	, stateReader.IndexCurrentChar), "Failed to open expression at index %i", stateReader.IndexCurrentChar);	// Enter sub-expression
		break;
	case ')':
		test_first_position();
		stateReader.ExpectsSeparator								= false;
		skip_if_escaping();
		if_fail_fef(::expressionReaderCloseTerm	(stateReader, tokens), "Failed to close term at index %i", stateReader.IndexCurrentChar);
		if_fail_fef(::expressionReaderCloseType	(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_ANY	, stateReader.IndexCurrentChar + 1), "Failed to close type: %s.", ::gpk::get_value_label(stateReader.CurrentElement->Type).begin());
		break;
	case '!':
		skip_if_escaping();
		if_fail_fef(::expressionReaderOpenLevel	(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_UNARY_NOT	, stateReader.IndexCurrentChar + 1), "Failed to open expression at index %i", stateReader.IndexCurrentChar);	// Enter sub-expression
		break;
	}
	if(stateReader.IndexCurrentChar >= expression.size() - 1) { // if this is the last character, make sure to close open key and root expression
		if(stateReader.IndexCurrentChar == expression.size() - 1)
			++stateReader.IndexCurrentChar;
		if_fail_fef(::expressionReaderCloseTerm	(stateReader, tokens), "Failed to close term at index %i", stateReader.IndexCurrentChar);
		if_fail_fef(::expressionReaderCloseIfType(stateReader, tokens, ::gpk::EXPRESSION_READER_TYPE_TERM_ANY), "%s", "Failed to close type.");
	}
	stateReader.Escaping										= false;
	return 0;
}

gpk::err_t			gpk::expressionReaderParseStep			(::gpk::SExpressionReader & reader, ::gpk::vcst_t expression)	{
	::gpk::apod<::gpk::SExpressionToken>					& tokens								= reader.Token;
	::gpk::SExpressionReaderState									& stateReader							= reader.StateRead;
	if_true_fef(stateReader.IndexCurrentChar >= expression.size(), "Invalid expression index: %u. Expression size: %u.", stateReader.IndexCurrentChar, expression.size());
	stateReader.CharCurrent										= expression[stateReader.IndexCurrentChar];
	gpk::err_t 														result									= reader.StateRead.InsideString
		? ::expressionReaderProcessStringCharacter	(stateReader, tokens, expression)
		: ::expressionReaderProcessDocCharacter		(stateReader, tokens, expression)
		;
	if (gpk::failed(result)) {
		const bool														validElement										= (::gpk::u2_t)reader.StateRead.IndexCurrentElement < reader.Token.size();
		const ::gpk::SExpressionToken									* currentElement									= validElement ? &reader.Token[reader.StateRead.IndexCurrentElement] : 0;
		error_printf("Error during read step. Malformed expression?"
			"\nPosition         : %i."
			"\nCharacter        : '%c' (0x%x)."
			"\nElement          : %i."
			"\nExpectsSeparator : %s."
			"\nEscaping         : %s."
			"\nNestLevel        : %i."
			"\nParent           : %i."
			"\nType             : %i (%s)."
			"\nOffset           : %i."
			"\nView             : %s."
			"\nExpression       : %s."
			, reader.StateRead.IndexCurrentChar
			, reader.StateRead.CharCurrent, reader.StateRead.CharCurrent
			, reader.StateRead.IndexCurrentElement
			, reader.StateRead.ExpectsSeparator	? "true" : "false"
			, reader.StateRead.Escaping			? "true" : "false"
			, reader.StateRead.NestLevel
			, validElement ? currentElement->ParentIndex								: -1
			, validElement ? currentElement->Type										: -1
			, validElement ? ::gpk::get_value_label(currentElement->Type).begin()		: "N/A"
			, validElement ? currentElement->Span.Begin									: -1
			, validElement ? &(::gpk::string(expression)[currentElement->Span.Begin])	: ""
			, ::gpk::string(expression).begin()
		);
	}
	return result;
}

gpk::err_t			gpk::expressionReaderParse			(::gpk::SExpressionReader & reader, ::gpk::vcst_t expression)	{
	::gpk::SExpressionReaderState				& stateReader						= reader.StateRead;
	::gpk::apod<::gpk::SExpressionToken>		& tokens							= reader.Token;
	for(stateReader.IndexCurrentChar = 0; stateReader.IndexCurrentChar < expression.size(); ++stateReader.IndexCurrentChar) {
		if_fail_fe(gpk::expressionReaderParseStep(reader, expression));
		if(stateReader.IndexCurrentChar < expression.size() && expression[stateReader.IndexCurrentChar] == 0)
			break;
		if_fail_fef(stateReader.NestLevel, "Nest level cannot be negative. Current level: %i.", stateReader.NestLevel);
	}
	if_true_fef(0 != stateReader.NestLevel, "Nest level: %i (Needs to be zero).", stateReader.NestLevel);
	if_fail_fef(::expressionReaderViews(tokens, reader.View, expression), "%s", "Failed to build views. Corrupt info?");
	if_fail_fef(::expressionTreeRebuild(tokens, reader.Tree), "%s", "Failed to rebuild tree. Corrupt tree?");
	return 0;
}

::gpk::err_t gpk::expressionStringFormat(::gpk::vcst_t format, ::gpk::view<cnst ::gpk::SStripLiteralType> tokens, cnst ::gpk::FError<::gpk::vcst_c &, ::gpk::vcst_t &> & resolve, ::gpk::asc_t & output) {
	::gpk::asc_t					formatted;
	for(::gpk::u2_t iType = 0; iType < tokens.size(); ++iType) {
		cnst ::gpk::SStripLiteralType	& type			= tokens[iType];
		cnst bool						isToken			= ::gpk::STRIP_LITERAL_TYPE_TOKEN == type.Type;
		if_true_fef(type.Span.Begin > type.Span.End || type.Span.End > format.size(), "Invalid format span: %u..%u, size:%u.", type.Span.Begin, type.Span.End, format.size());
		if_true_fef(isToken && type.Span.End - type.Span.Begin < 2, "Invalid token span: %u..%u.", type.Span.Begin, type.Span.End);
		::gpk::u2_c						offsetView		= isToken ? type.Span.Begin + 1 : type.Span.Begin;
		::gpk::u2_c						lengthView		= isToken ? type.Span.End - type.Span.Begin - 2 : type.Span.End - type.Span.Begin;
		if(0 == lengthView)
			continue;
		::gpk::vcst_t					viewToAppend	= {&format[offsetView], lengthView};
		if(isToken) {
			::gpk::vcst_t					resolved;
			if_fail_fef(resolve(viewToAppend, resolved), "Failed to resolve expression: %.*s.", GPK_VIEW_ARGS(viewToAppend));
			viewToAppend				= resolved;
		}
		if_fail_fef(formatted.append(viewToAppend), "iType: %u", iType);
	}
	if_fail_fe(output.append(formatted));
	rtrn 0;
}

::gpk::err_t gpk::expressionStringFormat(::gpk::vcst_t format, cnst ::gpk::FError<::gpk::vcst_c &, ::gpk::vcst_t &> & resolve, ::gpk::asc_t & output) {
	::gpk::apod<::gpk::SStripLiteralType>	tokens;
	::gpk::SStripLiteralState				state;
	if_fail_fef(::gpk::stripLiteralParse(state, tokens, format), "%.*s", GPK_VIEW_ARGS(format));
	rtrn ::gpk::expressionStringFormat(format, tokens, resolve, output);
}
