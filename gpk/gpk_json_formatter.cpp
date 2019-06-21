#include "gpk_json_formatter.h"

#include "gpk_parse.h"


namespace gpk
{
	GDEFINE_ENUM_TYPE (EXPRESSION_READER_TYPE, int8_t);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, EXPRESSION_KEY	, 0);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, EXPRESSION_INDEX	, 1);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, KEY				, 2);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, INDEX			, 3);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, UNKNOWN			, -1);

	struct SExpressionReaderType {
		int32_t																	ParentIndex;
		EXPRESSION_READER_TYPE													Type;
		::gpk::SSlice<uint32_t>													Span;
	};

	struct SExpressionReaderState {
		uint32_t																IndexCurrentChar				= 0;
		int32_t																	IndexCurrentElement				= -1;
		::gpk::SExpressionReaderType											* CurrentElement				= 0;
		int32_t																	NestLevel						= 0;
		char																	CharCurrent						= 0;
		bool																	Escaping						= false;
		bool																	ExpectsSeparator				= false;
	};

	struct SExpressionReaderNode {
		SExpressionReaderType													* Object						= 0;
		SExpressionReaderNode													* Parent						= 0;
		::gpk::array_obj<::gpk::ptr_obj<SExpressionReaderNode>>					Children						= 0;
		int32_t																	ObjectIndex						= -1;
	};
}

#define seterr_break_if(condition, format, ...)	\
	if(condition) {								\
		errVal					= -1;			\
		error_printf(format, __VA_ARGS__);		\
		break;									\
	}

::gpk::error_t															printNode						(::gpk::SExpressionReaderNode* node, const ::gpk::view_const_char& expression)			{
	char																		bufferFormat [1024]				= {};
	uint32_t																	lenString						= node->Object->Span.End - node->Object->Span.Begin;
	sprintf_s(bufferFormat, "Entering expression node type: %%u (%%s). Node Span: {%%u, %%u}. Parent index: %%u. Object index: %%u. Text: %%%u.%us", lenString, lenString);
	info_printf(bufferFormat, node->Object->Type, ::gpk::get_value_label(node->Object->Type).begin(), node->Object->Span.Begin, node->Object->Span.End, node->Object->ParentIndex, node->ObjectIndex, &expression[node->Object->Span.Begin]);
	for(uint32_t iChildren = 0; iChildren < node->Children.size(); ++iChildren)
		::printNode(node->Children[iChildren], expression);
	sprintf_s(bufferFormat, "Exiting expression node type: %%u (%%s). Node Span: {%%u, %%u}. Parent index: %%u. Object index: %%u. Text: %%%u.%us", lenString, lenString);
	info_printf(bufferFormat, node->Object->Type, ::gpk::get_value_label(node->Object->Type).begin(), node->Object->Span.Begin, node->Object->Span.End, node->Object->ParentIndex, node->ObjectIndex, &expression[node->Object->Span.Begin]);
	return 0;
}

::gpk::error_t															expressionReaderViews			(::gpk::array_pod<::gpk::SExpressionReaderType>& parsed, ::gpk::array_pod<::gpk::view_const_string>& views, const ::gpk::view_const_string& expression) { 
	for(uint32_t iTag = 0; iTag < parsed.size(); ++iTag) {
		const ::gpk::SExpressionReaderType										& type							 = parsed[iTag];
		if(iTag && ::gpk::EXPRESSION_READER_TYPE_EXPRESSION_INDEX == type.Type && (type.Span.End - type.Span.Begin) <= expression.size()) // doesn't count for root expression
			views.push_back({&expression[type.Span.Begin + 1], type.Span.End - type.Span.Begin - 2});
		else
			views.push_back({&expression[type.Span.Begin], type.Span.End - type.Span.Begin});
	}
	return 0;
}


						::gpk::error_t									expressionTreeRebuild								(::gpk::view_array<::gpk::SExpressionReaderType>& in_object, ::gpk::array_obj<::gpk::ptr_obj<::gpk::SExpressionReaderNode>> & out_nodes)								{
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SExpressionReaderNode>>				& tree												= out_nodes;
	gpk_necall(tree.resize(in_object.size()), "Out of memory? Object count: %u.", in_object.size());

	// -- Build all nodes linearly, without assigning the children 
	for(uint32_t iObject = 0; iObject < tree.size(); ++iObject) { 
		::gpk::ptr_obj<::gpk::SExpressionReaderNode>								& nodeCurrent										= tree[iObject];
		nodeCurrent->Object														= &in_object[iObject];
		nodeCurrent->Parent														= ((uint32_t)nodeCurrent->Object->ParentIndex < tree.size()) ? (gpk::SExpressionReaderNode*)tree[nodeCurrent->Object->ParentIndex] : nullptr;
		nodeCurrent->ObjectIndex												= iObject;
	}

	// -- Assign the children to every object of the hierarchy
	for(uint32_t iObject = 0, countNodes = tree.size(); iObject < countNodes; ++iObject) { 
		for(uint32_t iOther = 0; iOther < countNodes; ++iOther) {
			const ::gpk::ptr_obj<::gpk::SExpressionReaderNode>							& nodeOther											= tree[iOther];
			if(((uint32_t)nodeOther->Object->ParentIndex) == iObject)
				gpk_necall(tree[iObject]->Children.push_back(nodeOther), "Failed to push tree node. Out of memory?");
		}
	}

	// -- Remove the key/value wrappers from objects.
	for(uint32_t iObject = 0, countNodes = tree.size(); iObject < countNodes; ++iObject) { 
		::gpk::ptr_obj<::gpk::SExpressionReaderNode>								& nodeCurrent										= tree[iObject];
		if(iObject && (::gpk::EXPRESSION_READER_TYPE_EXPRESSION_INDEX != nodeCurrent->Object->Type))
			continue;
		if(1 == nodeCurrent->Children.size() && ::gpk::EXPRESSION_READER_TYPE_INDEX == nodeCurrent->Children[0]->Object->Type) {
			for(uint32_t iChild = 0; iChild < nodeCurrent->Parent->Children.size(); ++iChild) {
				if(nodeCurrent->Parent->Children[iChild] == nodeCurrent) {
					nodeCurrent->Parent->Children[iChild] = nodeCurrent->Children[0];
					break;
				}
			}
		}
	}
	return 0;
}

::gpk::error_t									expressionReaderCloseType	(::gpk::SExpressionReaderState& stateSolver, ::gpk::array_pod<::gpk::SExpressionReaderType>& parsed, ::gpk::EXPRESSION_READER_TYPE type) { 
	stateSolver.CurrentElement->Span.End			= stateSolver.IndexCurrentChar; 
	stateSolver.IndexCurrentElement					= stateSolver.CurrentElement->ParentIndex; 
	stateSolver.CurrentElement						= (-1 != stateSolver.IndexCurrentElement) ? &parsed[stateSolver.IndexCurrentElement] : nullptr; 
	info_printf("Closing expression type: %s. Parent type: %s.", ::gpk::get_value_label(type).begin(), stateSolver.CurrentElement ? ::gpk::get_value_label(stateSolver.CurrentElement->Type).begin() : "UNKNOWN");
	return 0;
}

::gpk::error_t									expressionReaderCloseIfType	(::gpk::SExpressionReaderState& stateSolver, ::gpk::array_pod<::gpk::SExpressionReaderType>& parsed, ::gpk::EXPRESSION_READER_TYPE type)	{
	if(type == stateSolver.CurrentElement->Type) {
		::expressionReaderCloseType(stateSolver, parsed, type); 
		--stateSolver.NestLevel; 
		info_printf("Nest level: %i.", stateSolver.NestLevel);
		return 1;
	}
	return 0;
}

::gpk::error_t									expressionReaderParseStep	(::gpk::SExpressionReaderState& stateSolver, ::gpk::array_pod<::gpk::SExpressionReaderType>& parsed, const ::gpk::view_const_string& expression)	{
	if(0 == parsed.size()) {
		stateSolver.IndexCurrentElement					= parsed.push_back({stateSolver.IndexCurrentElement, ::gpk::EXPRESSION_READER_TYPE_EXPRESSION_KEY, stateSolver.IndexCurrentChar, stateSolver.IndexCurrentChar});
		stateSolver.CurrentElement						= &parsed[stateSolver.IndexCurrentElement];
		++stateSolver.NestLevel;
		info_printf("Entering %s. Nest level: %u.", ::gpk::get_value_label(stateSolver.CurrentElement->Type).begin(), stateSolver.NestLevel); 
	}
#define test_first_position()	seterr_break_if(0 == stateSolver.CharCurrent, "Character found at invalid position. Index: %u. Character: %c.", stateSolver.IndexCurrentChar, stateSolver.CharCurrent);
#define skip_if_escaping()		if(true == stateSolver.Escaping) break
	::gpk::error_t										errVal						= 0;
	switch(stateSolver.CharCurrent) {
	default	: 
		ree_if(stateSolver.ExpectsSeparator, "Separator expected, found: %c (%i).", stateSolver.CharCurrent, (int32_t)stateSolver.CharCurrent);
		if(::gpk::EXPRESSION_READER_TYPE_EXPRESSION_KEY == stateSolver.CurrentElement->Type) {
			stateSolver.IndexCurrentElement					= parsed.push_back({stateSolver.IndexCurrentElement, ::gpk::EXPRESSION_READER_TYPE_KEY, stateSolver.IndexCurrentChar, stateSolver.IndexCurrentChar});
			stateSolver.CurrentElement						= &parsed[stateSolver.IndexCurrentElement];
			++stateSolver.NestLevel; 
			info_printf("Entering KEY. Nest level: %u.", stateSolver.NestLevel);
		}
		else if(::gpk::EXPRESSION_READER_TYPE_INDEX == stateSolver.CurrentElement->Type)
			stateSolver.CurrentElement->Type				= ::gpk::EXPRESSION_READER_TYPE_KEY;
		break;
	case '0'	: case '1'	: case '2'	: case '3'	: case '4'	: case '5'	: case '6'	: case '7'	: case '8'	: case '9'	:
		ree_if(stateSolver.ExpectsSeparator, "Separator expected, found: %c (%i).", stateSolver.CharCurrent, (int32_t)stateSolver.CharCurrent);
		if(::gpk::EXPRESSION_READER_TYPE_EXPRESSION_INDEX == stateSolver.CurrentElement->Type) {
			stateSolver.IndexCurrentElement					= parsed.push_back({stateSolver.IndexCurrentElement, (0 != stateSolver.IndexCurrentElement) ? ::gpk::EXPRESSION_READER_TYPE_INDEX : ::gpk::EXPRESSION_READER_TYPE_KEY, stateSolver.IndexCurrentChar, stateSolver.IndexCurrentChar});
			stateSolver.CurrentElement						= &parsed[stateSolver.IndexCurrentElement];
			++stateSolver.NestLevel; 
			info_printf("Entering %s. Nest level: %u.", ::gpk::get_value_label(stateSolver.CurrentElement->Type).begin(), stateSolver.NestLevel);
		}
		break;
	case ' ': case '\t': case '\r': case '\n':
		skip_if_escaping(); 
		if(::gpk::EXPRESSION_READER_TYPE_KEY == stateSolver.CurrentElement->Type) {
			::expressionReaderCloseType(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_KEY); 
			--stateSolver.NestLevel; 
			info_printf("Nest level: %u.", stateSolver.NestLevel);
			stateSolver.ExpectsSeparator	= true;
		}
		break;	// skip blank lines
	case '.': 
		stateSolver.ExpectsSeparator	= false;
		test_first_position(); 
		skip_if_escaping(); 
		::expressionReaderCloseIfType(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_KEY); 
		break;
	case '[': 
		stateSolver.ExpectsSeparator	= false;
		test_first_position(); 
		skip_if_escaping(); 
		::expressionReaderCloseIfType(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_KEY); 
		// Enter sub-expression
		stateSolver.IndexCurrentElement					= parsed.push_back({stateSolver.IndexCurrentElement, ::gpk::EXPRESSION_READER_TYPE_EXPRESSION_INDEX, stateSolver.IndexCurrentChar, stateSolver.IndexCurrentChar});
		stateSolver.CurrentElement						= &parsed[stateSolver.IndexCurrentElement];
		++stateSolver.NestLevel; 
		info_printf("Entering %s. Nest level: %u.", ::gpk::get_value_label(stateSolver.CurrentElement->Type).begin(), stateSolver.NestLevel); 
		break;
	case ']': 
		stateSolver.ExpectsSeparator	= false;
		test_first_position(); 
		skip_if_escaping(); 
		::expressionReaderCloseIfType(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_INDEX); 
		::expressionReaderCloseIfType(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_KEY); 
		++stateSolver.IndexCurrentChar;
		::expressionReaderCloseIfType(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_EXPRESSION_INDEX); 
		break;
	}
	if(stateSolver.IndexCurrentChar == expression.size() - 1) { // if this is the last character, make sure to close open key and root expression
		++stateSolver.IndexCurrentChar;
		::expressionReaderCloseIfType(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_KEY); 
		::expressionReaderCloseIfType(stateSolver, parsed, ::gpk::EXPRESSION_READER_TYPE_EXPRESSION_KEY); 
	}
	stateSolver.Escaping							= false;
	(void)expression;
	return errVal;
}

::gpk::error_t									expressionReaderParse					(::gpk::SExpressionReaderState& stateSolver, ::gpk::array_pod<::gpk::SExpressionReaderType>& parsed, const ::gpk::view_const_string& expression)	{
	for(stateSolver.IndexCurrentChar = 0; stateSolver.IndexCurrentChar < expression.size(); ++stateSolver.IndexCurrentChar) {
		stateSolver.CharCurrent							= expression[stateSolver.IndexCurrentChar];
		gpk_necall(::expressionReaderParseStep(stateSolver, parsed, expression), "%s", "Unknown error.");
		gpk_necall(stateSolver.NestLevel, "Nest level cannot be negative. Current level: %i.", stateSolver.NestLevel);
	}
	ree_if(0 != stateSolver.NestLevel, "Nest level: %i (Needs to be zero).", stateSolver.NestLevel);
	return 0;
}

::gpk::error_t									gpk::jsonStringFormat					(const ::gpk::view_const_string& format, const ::gpk::SJSONNode& input, ::gpk::array_pod<char_t>& output)													{
	::gpk::SStripLiteralState							stateLiteralStripper					= {};
	::gpk::array_pod<::gpk::SStripLiteralType>			literalTypes							= {};
	::gpk::stripLiteralParse(stateLiteralStripper, literalTypes, format);
	::gpk::array_obj<::gpk::view_const_string>			viewsAll								= {};
	::gpk::stripLiteralGetViews(viewsAll, literalTypes, format);
	{
		info_printf("-**- %s -**-", "Results of token search");
		for(uint32_t iAll = 0; iAll < viewsAll.size(); ++iAll) {
			static constexpr const char							bufferFormatIn	[]						= "%s (%%u): %%.%us";
			char												bufferFormatOut	[8192]					= {};
			bool												isLiteral								= (literalTypes[iAll].Type == ::gpk::STRIP_LITERAL_TYPE_LITERAL);
			if(isLiteral)	// we only have to solve tokens
				continue;

			const ::gpk::view_const_string						& expression							= viewsAll[iAll];
			sprintf_s(bufferFormatOut, bufferFormatIn, isLiteral ? "Literal" : "Token", expression.size());
			info_printf(bufferFormatOut, iAll, expression.begin());
			::gpk::SExpressionReaderState						stateSolver;
			::gpk::view_const_string							resultOfExpressionEval					= {};
			::gpk::array_pod<::gpk::SExpressionReaderType>	parsed;
			::expressionReaderParse(stateSolver, parsed, expression);
			::gpk::array_pod<::gpk::view_const_string>			expressionViews;
			::expressionReaderViews(parsed, expressionViews, expression);
			for(uint32_t iView = 0; iView < expressionViews.size(); ++iView) {
				const ::gpk::view_const_string						& viewExpression						= expressionViews[iView];
				const ::gpk::SExpressionReaderType					& typeExpression						= parsed[iView];
				if(viewExpression.size()) {
					uint32_t										lenString									= viewExpression.size();
					char											bufferFormat [8192]							= {};
					sprintf_s(bufferFormat, "Expression element: %%.%us. Type: %s. Parent: %i. Begin: %u. End: %u.", lenString, ::gpk::get_value_label(typeExpression.Type).begin(), typeExpression.ParentIndex, typeExpression.Span.Begin, typeExpression.Span.End);
					info_printf(bufferFormat, viewExpression.begin());
				}
			}
			::gpk::array_obj<::gpk::ptr_obj<::gpk::SExpressionReaderNode>> nodesExpressionTree;
			::expressionTreeRebuild(parsed, nodesExpressionTree);
			::printNode(nodesExpressionTree[0], expression);
			// TODO: Implement expression evaluation.
			::gpk::view_const_string							evaluated								= {};
			if(evaluated.size()) {
				uint32_t											lenString								= evaluated.size();
				char												bufferFormat [8192]						= {};
				sprintf_s(bufferFormat, "Result of expression evaluation: %%.%us.", lenString);
				info_printf(bufferFormat, evaluated.begin());
			}
		}
	}
	(void)input, (void)output;
	return 0;
}