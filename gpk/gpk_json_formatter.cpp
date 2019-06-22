#include "gpk_expression.h"
#include "gpk_json_formatter.h"

#include "gpk_parse.h"

::gpk::error_t									printNode						(::gpk::SExpressionNode* node, const ::gpk::view_const_char& expression)			{
	char												bufferFormat [1024]				= {};
	uint32_t											lenString						= node->Object->Span.End - node->Object->Span.Begin;
	sprintf_s(bufferFormat, "Entering expression node type: %%u (%%s). Node Span: {%%u, %%u}. Parent index: %%u. Object index: %%u. Text: %%%u.%us", lenString, lenString);
	info_printf(bufferFormat, node->Object->Type, ::gpk::get_value_label(node->Object->Type).begin(), node->Object->Span.Begin, node->Object->Span.End, node->Object->ParentIndex, node->ObjectIndex, &expression[node->Object->Span.Begin]);
	for(uint32_t iChildren = 0; iChildren < node->Children.size(); ++iChildren)
		::printNode(node->Children[iChildren], expression);
	sprintf_s(bufferFormat, "Exiting expression node type: %%u (%%s). Node Span: {%%u, %%u}. Parent index: %%u. Object index: %%u. Text: %%%u.%us", lenString, lenString);
	info_printf(bufferFormat, node->Object->Type, ::gpk::get_value_label(node->Object->Type).begin(), node->Object->Span.Begin, node->Object->Span.End, node->Object->ParentIndex, node->ObjectIndex, &expression[node->Object->Span.Begin]);
	return 0;
}

::gpk::error_t									jsonStringFormatResolve					(const ::gpk::view_const_string & expression, const ::gpk::SJSONNode& input, ::gpk::array_pod<char_t>& output)													{
	::gpk::view_const_string							resultOfExpressionEval					= {};
	::gpk::SExpressionReader							reader;
	::gpk::expressionReaderParse(reader, expression);
	const ::gpk::array_obj<::gpk::view_const_string>	& expressionViews						= reader.View;
	for(uint32_t iView = 0; iView < expressionViews.size(); ++iView) {
		const ::gpk::view_const_string						& viewExpression						= expressionViews[iView];
		const ::gpk::SExpressionReaderType					& typeExpression						= reader.Object[iView];
		if(viewExpression.size()) {
			uint32_t										lenString									= viewExpression.size();
			char											bufferFormat [8192]							= {};
			sprintf_s(bufferFormat, "Expression element: %%.%us. Type: %s. Parent: %i. Begin: %u. End: %u.", lenString, ::gpk::get_value_label(typeExpression.Type).begin(), typeExpression.ParentIndex, typeExpression.Span.Begin, typeExpression.Span.End);
			info_printf(bufferFormat, viewExpression.begin());
		}
	}
	if(reader.Tree.size())
		::printNode(reader.Tree[0], expression);
	// TODO: Implement expression evaluation.
	::gpk::view_const_string							evaluated								= {};
	if(evaluated.size()) {
		uint32_t											lenString								= evaluated.size();
		char												bufferFormat [8192]						= {};
		sprintf_s(bufferFormat, "Result of expression evaluation: %%.%us.", lenString);
		info_printf(bufferFormat, evaluated.begin());
	}
	output, input;
	return 0;
}

::gpk::error_t									gpk::jsonStringFormat					(const ::gpk::view_const_string& format, const ::gpk::SJSONNode& input, ::gpk::array_pod<char_t>& output)													{
	::gpk::SStripLiteralState							stateLiteralStripper					= {};
	::gpk::array_pod<::gpk::SStripLiteralType>			typesLiteral							= {};
	::gpk::stripLiteralParse(stateLiteralStripper, typesLiteral, format);	// strip root literals 
	::gpk::array_obj<::gpk::view_const_string>			views									= {};
	::gpk::stripLiteralGetViews(views, typesLiteral, format);

	::gpk::array_obj<::gpk::array_pod<char_t>>			resolved;
	resolved.resize(views.size());
	info_printf("-**- %s -**-", "Results of token search");
	for(uint32_t iView = 0; iView < views.size(); ++iView) {
		if(typesLiteral[iView].Type == ::gpk::STRIP_LITERAL_TYPE_TOKEN)	// we only have to solve tokens
			::jsonStringFormatResolve(views[iView], input, resolved[iView]);
	}
	for(uint32_t iView = 0; iView < views.size(); ++iView)
		output.append(views[iView]);
	return 0;
}