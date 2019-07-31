#include "gpk_expression.h"

#define gpk_expression_info_printf // info_printf

static	::gpk::error_t										expressionReaderViews					(::gpk::array_pod<::gpk::SExpressionReaderType>& parsed, ::gpk::array_obj<::gpk::view_const_string>& views, const ::gpk::view_const_string& expression) {
	for(uint32_t iTag = 0; iTag < parsed.size(); ++iTag) {
		const ::gpk::SExpressionReaderType								& type									 = parsed[iTag];
		if(iTag && (::gpk::EXPRESSION_READER_TYPE_EXPRESSION_INDEX == type.Type || ::gpk::EXPRESSION_READER_TYPE_EXPRESSION_KEY == type.Type) && (type.Span.End - type.Span.Begin) <= expression.size()) // doesn't count for root expression
			gpk_necall(views.push_back({&expression[type.Span.Begin + 1], type.Span.End - type.Span.Begin - 2}), "%s", "Out of memory?");
		else
			gpk_necall(views.push_back({&expression[type.Span.Begin], type.Span.End - type.Span.Begin}), "%s", "Out of memory?");
	}
	return 0;
}

static	::gpk::error_t										expressionTreeRebuild					(::gpk::view_array<::gpk::SExpressionReaderType>& in_object, ::gpk::array_obj<::gpk::ptr_obj<::gpk::SExpressionNode>> & out_nodes)								{
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SExpressionNode>>		& tree									= out_nodes;
	gpk_necall(tree.resize(in_object.size()), "Out of memory? Object count: %u.", in_object.size());

	// -- Build all nodes linearly, without assigning the children
	for(uint32_t iObject = 0; iObject < tree.size(); ++iObject) {
		::gpk::ptr_obj<::gpk::SExpressionNode>							& nodeCurrent										= tree[iObject];
		nodeCurrent->Object											= &in_object[iObject];
		nodeCurrent->Parent											= ((uint32_t)nodeCurrent->Object->ParentIndex < tree.size()) ? (gpk::SExpressionNode*)tree[nodeCurrent->Object->ParentIndex] : nullptr;
		nodeCurrent->ObjectIndex									= iObject;
	}

	// -- Assign the children to every object of the hierarchy
	for(uint32_t iObject = 0, countNodes = tree.size(); iObject < countNodes; ++iObject) {
		for(uint32_t iOther = 0; iOther < countNodes; ++iOther) {
			const ::gpk::ptr_obj<::gpk::SExpressionNode>					& nodeOther											= tree[iOther];
			if(((uint32_t)nodeOther->Object->ParentIndex) == iObject)
				gpk_necall(tree[iObject]->Children.push_back(nodeOther), "%s", "Failed to push tree node. Out of memory?");
		}
	}

	// -- Remove the key/value wrappers from objects.
	for(uint32_t iObject = 0, countNodes = tree.size(); iObject < countNodes; ++iObject) {
		::gpk::ptr_obj<::gpk::SExpressionNode>							& nodeCurrent										= tree[iObject];
		if(iObject && (::gpk::EXPRESSION_READER_TYPE_EXPRESSION_INDEX != nodeCurrent->Object->Type))
			continue;
		if(1 != nodeCurrent->Children.size() || ::gpk::EXPRESSION_READER_TYPE_INDEX != nodeCurrent->Children[0]->Object->Type)
			continue;
		for(uint32_t iChild = 0; iChild < nodeCurrent->Parent->Children.size(); ++iChild)
			if(nodeCurrent->Parent->Children[iChild] == nodeCurrent) {
				nodeCurrent->Parent->Children[iChild]							= nodeCurrent->Children[0];
				break;
			}
	}
	// -- Remove the key/value wrappers from objects.
	for(uint32_t iObject = 0, countNodes = tree.size(); iObject < countNodes; ++iObject) {
		::gpk::ptr_obj<::gpk::SExpressionNode>							& nodeCurrent										= tree[iObject];
		if(iObject && (::gpk::EXPRESSION_READER_TYPE_KEY != nodeCurrent->Object->Type))
			continue;
		if(1 != nodeCurrent->Children.size() || ::gpk::EXPRESSION_READER_TYPE_EXPRESSION_KEY != nodeCurrent->Children[0]->Object->Type)
			continue;
		for(uint32_t iChild = 0; iChild < nodeCurrent->Parent->Children.size(); ++iChild)
			if(nodeCurrent->Parent->Children[iChild] == nodeCurrent) {
				nodeCurrent->Parent->Children[iChild]							= nodeCurrent->Children[0];
				break;
			}
	}
	return 0;
}

static	::gpk::error_t										expressionReaderCloseType				(::gpk::SExpressionReaderState& stateSolver, ::gpk::array_pod<::gpk::SExpressionReaderType>& parsed, ::gpk::EXPRESSION_READER_TYPE type, int32_t indexCurrentChar) {
	ree_if(type != stateSolver.CurrentElement->Type, "Invalid type to close: %s. Current type: %s.", ::gpk::get_value_label(type).begin(), ::gpk::get_value_label(stateSolver.CurrentElement->Type).begin());
	stateSolver.CurrentElement->Span.End						= indexCurrentChar;
	stateSolver.IndexCurrentElement								= stateSolver.CurrentElement->ParentIndex;
	stateSolver.CurrentElement									= (-1 != stateSolver.IndexCurrentElement) ? &parsed[stateSolver.IndexCurrentElement] : nullptr;
	gpk_expression_info_printf("Closing expression type: %s. Parent type: %s. Nest level: %i.", ::gpk::get_value_label(type).begin(), stateSolver.CurrentElement ? ::gpk::get_value_label(stateSolver.CurrentElement->Type).begin() : "UNKNOWN", stateSolver.NestLevel);
	--stateSolver.NestLevel;
	return 0;
}

static	::gpk::error_t										expressionReaderCloseIfType				(::gpk::SExpressionReaderState& stateSolver, ::gpk::array_pod<::gpk::SExpressionReaderType>& parsed, ::gpk::EXPRESSION_READER_TYPE type)	{
	rni_if(0 == stateSolver.CurrentElement, "Cannot close %s. Nothing to close.", ::gpk::get_value_label(type).begin());
	if(type == stateSolver.CurrentElement->Type) {
		gpk_necall(::expressionReaderCloseType(stateSolver, parsed, type, stateSolver.IndexCurrentChar), "Failed to close type: %s. Why would this happen?", ::gpk::get_value_label(type).begin());
		return 1;
	}
	return 0;
}

static	::gpk::error_t										expressionReaderOpenLevel				(::gpk::SExpressionReaderState& stateSolver, ::gpk::array_pod<::gpk::SExpressionReaderType>& parsed, ::gpk::EXPRESSION_READER_TYPE type, uint32_t iChar)	{
	stateSolver.IndexCurrentElement								= parsed.push_back({stateSolver.IndexCurrentElement, type, {iChar, iChar}});
	stateSolver.CurrentElement									= &parsed[stateSolver.IndexCurrentElement];
	++stateSolver.NestLevel;
	gpk_expression_info_printf("Entering expression type: %s. Parent type: %s. Nest level: %u.", ::gpk::get_value_label(stateSolver.CurrentElement->Type).begin(), stateSolver.CurrentElement ? ::gpk::get_value_label(stateSolver.CurrentElement->Type).begin() : "UNKNOWN", stateSolver.NestLevel);
	return 0;
}

static	bool												isSpaceCharacter						(const char characterToTest)		{
	switch(characterToTest) {
	case ' ': case '\t': case '\r': case '\n':
		return true;
	default:
		return false;
	}
}

static	::gpk::error_t										expressionReaderProcessCharacter		(::gpk::SExpressionReaderState& stateSolver, ::gpk::array_pod<::gpk::SExpressionReaderType>& parsed, const ::gpk::view_const_string& expression)	{
	if(0 == parsed.size())
		gpk_necall(::expressionReaderOpenLevel(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_EXPRESSION_KEY, stateSolver.IndexCurrentChar), "Failed to open expression at index %i", stateSolver.IndexCurrentChar);

#define test_first_position()	ree_if(0 == stateSolver.CharCurrent, "Character found at invalid position. Index: %u. Character: %c.", stateSolver.IndexCurrentChar, stateSolver.CharCurrent);
#define skip_if_escaping()		if(true == stateSolver.Escaping) break

	switch(stateSolver.CharCurrent) {
	case 'i':
		if(expression.size() > stateSolver.IndexCurrentChar + 1 && expression[stateSolver.IndexCurrentChar + 1] == 'f' && ::isSpaceCharacter(expression[stateSolver.IndexCurrentChar + 1])) {
			if(::gpk::EXPRESSION_READER_TYPE_EXPRESSION_KEY == stateSolver.CurrentElement->Type || ::gpk::EXPRESSION_READER_TYPE_EXPRESSION_INDEX == stateSolver.CurrentElement->Type)
				gpk_necall(::expressionReaderOpenLevel(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_CONDITION, stateSolver.IndexCurrentChar), "Failed to open key at index %i", stateSolver.IndexCurrentChar);
			else
				error_printf("%s", "Unexpected situation.");
			break;
		}
	default	:
		ree_if(stateSolver.ExpectsSeparator, "Separator expected, found: %c (%i).", stateSolver.CharCurrent, (int32_t)stateSolver.CharCurrent);
		if(::gpk::EXPRESSION_READER_TYPE_EXPRESSION_KEY == stateSolver.CurrentElement->Type || ::gpk::EXPRESSION_READER_TYPE_EXPRESSION_INDEX == stateSolver.CurrentElement->Type)
			gpk_necall(::expressionReaderOpenLevel(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_KEY, stateSolver.IndexCurrentChar), "Failed to open key at index %i", stateSolver.IndexCurrentChar);
		else
			error_printf("%s", "Unexpected situation.");
		break;
	case '0'	: case '1'	: case '2'	: case '3'	: case '4'	: case '5'	: case '6'	: case '7'	: case '8'	: case '9'	:
		ree_if(stateSolver.ExpectsSeparator, "Separator expected, found: %c (%i).", stateSolver.CharCurrent, (int32_t)stateSolver.CharCurrent);
		if( ::gpk::EXPRESSION_READER_TYPE_EXPRESSION_INDEX == stateSolver.CurrentElement->Type)
			gpk_necall(::expressionReaderOpenLevel(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_INDEX, stateSolver.IndexCurrentChar), "Failed to open index at position %i", stateSolver.IndexCurrentChar);
		else if(::gpk::EXPRESSION_READER_TYPE_EXPRESSION_KEY == stateSolver.CurrentElement->Type)
			gpk_necall(::expressionReaderOpenLevel(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_KEY, stateSolver.IndexCurrentChar), "Failed to open key at index %i", stateSolver.IndexCurrentChar);
		else
			error_printf("%s", "Unexpected situation.");
		break;
	case ' ': case '\t': case '\r': case '\n':
		skip_if_escaping();
		if( ::gpk::EXPRESSION_READER_TYPE_KEY	== stateSolver.CurrentElement->Type
		 || ::gpk::EXPRESSION_READER_TYPE_INDEX	== stateSolver.CurrentElement->Type
		 ) {
			gpk_necall(::expressionReaderCloseType(stateSolver, parsed, stateSolver.CurrentElement->Type, stateSolver.IndexCurrentChar), "Failed to close type: %s.", ::gpk::get_value_label(stateSolver.CurrentElement->Type).begin(), "%s", "Failed to close type.");
			stateSolver.ExpectsSeparator								= true;
		}
		break;	// skip blank lines
	case '.':
		stateSolver.ExpectsSeparator								= false;
		test_first_position();
		skip_if_escaping();
		gpk_necall(::expressionReaderCloseIfType(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_KEY), "%s", "Failed to close type.");
		gpk_necall(::expressionReaderOpenLevel(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_KEY, stateSolver.IndexCurrentChar + 1), "Failed to open key at index %i", stateSolver.IndexCurrentChar);
		break;
	case '{':
		skip_if_escaping();
		ree_if(stateSolver.ExpectsSeparator, "%s", "This character can only be used after the . separator, so this requirement should be canceled already.");
		ree_if(::gpk::EXPRESSION_READER_TYPE_KEY != stateSolver.CurrentElement->Type, "Can only open brackets in EXPRESSION_KEY types. Current expression type: %s.", ::gpk::get_value_label(stateSolver.CurrentElement->Type).begin());
		gpk_necall(::expressionReaderOpenLevel(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_EXPRESSION_KEY, stateSolver.IndexCurrentChar), "Failed to open expression at index %i", stateSolver.IndexCurrentChar);	// Enter sub-expression
		break;
	case '}':
		skip_if_escaping();
		stateSolver.ExpectsSeparator								= false;
		gpk_necall(::expressionReaderCloseIfType(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_KEY), "%s", "Failed to close type.");
		gpk_necall(::expressionReaderCloseType(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_EXPRESSION_KEY, stateSolver.IndexCurrentChar + 1), "Failed to close type: %s.", ::gpk::get_value_label(stateSolver.CurrentElement->Type).begin());
		break;
	case '[':
		stateSolver.ExpectsSeparator								= false;
		skip_if_escaping();
		gpk_necall(::expressionReaderCloseIfType(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_KEY), "%s", "Failed to close type.");
		gpk_necall(::expressionReaderOpenLevel(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_EXPRESSION_INDEX, stateSolver.IndexCurrentChar), "Failed to open expression at index %i", stateSolver.IndexCurrentChar);	// Enter sub-expression
		break;
	case ']':
		stateSolver.ExpectsSeparator								= false;
		skip_if_escaping();
		gpk_necall(::expressionReaderCloseIfType(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_INDEX), "%s", "Failed to close type.");
		gpk_necall(::expressionReaderCloseIfType(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_KEY), "%s", "Failed to close type.");
		gpk_necall(::expressionReaderCloseType(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_EXPRESSION_INDEX, stateSolver.IndexCurrentChar + 1), "Failed to close type: %s.", ::gpk::get_value_label(stateSolver.CurrentElement->Type).begin());
		break;
	}
	if(stateSolver.IndexCurrentChar == expression.size() - 1) { // if this is the last character, make sure to close open key and root expression
		++stateSolver.IndexCurrentChar;
		gpk_necall(::expressionReaderCloseIfType(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_KEY), "%s", "Failed to close type.");
		gpk_necall(::expressionReaderCloseIfType(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_EXPRESSION_KEY), "%s", "Failed to close type.");
	}
	stateSolver.Escaping										= false;
	return 0;
}

::gpk::error_t												gpk::expressionReaderParseStep			(::gpk::SExpressionReader& reader, const ::gpk::view_const_string& expression)	{
	::gpk::array_pod<::gpk::SExpressionReaderType>					& parsed								= reader.Object;
	::gpk::SExpressionReaderState									& stateSolver							= reader.StateRead;
	stateSolver.CharCurrent										= expression[stateSolver.IndexCurrentChar];
	if errored(::expressionReaderProcessCharacter(stateSolver, parsed, expression)) {
		const bool														validElement										= (uint32_t)reader.StateRead.IndexCurrentElement < reader.Object.size();
		const ::gpk::SExpressionReaderType								* currentElement									= validElement ? &reader.Object[reader.StateRead.IndexCurrentElement] : 0;
		const ::gpk::view_const_string									currentView											= validElement ? reader.View[reader.StateRead.IndexCurrentElement] : ::gpk::view_const_string{};
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
			"\nJSON             : %s."
			, reader.StateRead.IndexCurrentChar
			, reader.StateRead.CharCurrent, reader.StateRead.CharCurrent
			, reader.StateRead.IndexCurrentElement
			, reader.StateRead.ExpectsSeparator	? "true" : "false"
			, reader.StateRead.Escaping			? "true" : "false"
			, reader.StateRead.NestLevel
			, validElement ? currentElement->ParentIndex							: -1
			, validElement ? currentElement->Type									: -1
			, validElement ? ::gpk::get_value_label(currentElement->Type).begin()	: "N/A"
			, validElement ? currentElement->Span.Begin								: -1
			, validElement ? currentView.begin()									: ""
			, expression.begin()
		);
	}
	return 0;
}

::gpk::error_t												gpk::expressionReaderParse				(::gpk::SExpressionReader& reader, const ::gpk::view_const_string& expression)	{
	::gpk::SExpressionReaderState									& stateSolver							= reader.StateRead;
	::gpk::array_pod<::gpk::SExpressionReaderType>					& parsed								= reader.Object;
	for(stateSolver.IndexCurrentChar = 0; stateSolver.IndexCurrentChar < expression.size(); ++stateSolver.IndexCurrentChar) {
		gpk_necall(::gpk::expressionReaderParseStep(reader, expression), "%s", "Unknown error.");
		gpk_necall(stateSolver.NestLevel, "Nest level cannot be negative. Current level: %i.", stateSolver.NestLevel);
	}
	ree_if(0 != stateSolver.NestLevel, "Nest level: %i (Needs to be zero).", stateSolver.NestLevel);
	gpk_necall(::expressionReaderViews(parsed, reader.View, expression), "%s", "Failed to build views. Corrupt info?");
	gpk_necall(::expressionTreeRebuild(parsed, reader.Tree), "%s", "Failed to rebuild tree. Corrupt tree?");
	return 0;
}
