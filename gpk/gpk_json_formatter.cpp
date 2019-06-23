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

::gpk::error_t									evaluateNode							(::gpk::SExpressionReader & readerExpression, uint32_t indexNodeExpression, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::view_const_string& output)						{
	const ::gpk::SExpressionNode						* nodeExpression						= readerExpression.Tree[indexNodeExpression];
	const ::gpk::view_array<::gpk::view_const_string>	& viewsExpression						= readerExpression.View;
	const ::gpk::SJSONNode								& input									= *inputJSON.Tree[indexNodeJSON]; 
	const ::gpk::view_array<::gpk::view_const_string>	& viewsJSON								= inputJSON.View;
	if(0 == nodeExpression->Children.size()) {
		if(nodeExpression->Object->Type == ::gpk::EXPRESSION_READER_TYPE_KEY) {
			ree_if(input.Object->Type != ::gpk::JSON_TYPE_OBJECT, "Only objects can be accessed by key. JSON type: %s.", ::gpk::get_value_label(input.Object->Type).begin());
			const ::gpk::view_const_string						& viewExpression						= viewsExpression[nodeExpression->ObjectIndex];
			int32_t												jsonIndex								= ::gpk::jsonObjectValueGet(input, viewsJSON, viewExpression);
			char												bufferFormat [4096]						= {};
			sprintf_s(bufferFormat, "Key not found: %%.%us.", viewExpression.size());
			ree_if(errored(jsonIndex), bufferFormat, viewExpression.begin());
			output											= viewsJSON[jsonIndex];
			return jsonIndex;
		}
		else if(nodeExpression->Object->Type == ::gpk::EXPRESSION_READER_TYPE_INDEX) {
			ree_if(input.Object->Type != ::gpk::JSON_TYPE_ARRAY, "Only arrays can be accessed by key. JSON type: %s.", ::gpk::get_value_label(input.Object->Type).begin());
			uint64_t											numberRead								= 0;
			::gpk::parseArbitraryBaseInteger(10, "0123456789", viewsJSON[input.ObjectIndex], &numberRead);
			int32_t												jsonIndex								= ::gpk::jsonArrayValueGet(input, (uint32_t)numberRead);
			output											= viewsJSON[jsonIndex];
			return jsonIndex;
		}
	}
	else
			 if(nodeExpression->Object->Type == ::gpk::EXPRESSION_READER_TYPE_EXPRESSION_KEY)	{}
		else if(nodeExpression->Object->Type == ::gpk::EXPRESSION_READER_TYPE_EXPRESSION_INDEX) {}
		for(uint32_t iChild = 0; iChild < nodeExpression->Children.size(); ++iChild) {
			int32_t												jsonIndex								= ::evaluateNode(readerExpression, nodeExpression->Children[iChild]->ObjectIndex, inputJSON, indexNodeJSON, output);
			gpk_necall(jsonIndex, "Failed to evaluate node at index %i.", nodeExpression->Children[iChild]->ObjectIndex);
		}
	return 0;
}

::gpk::error_t									jsonStringFormatResolve					(const ::gpk::view_const_string & expression, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::view_const_string& output)		{
	::gpk::view_const_string							resultOfExpressionEval					= {};
	::gpk::SExpressionReader							reader;
	gpk_necall(::gpk::expressionReaderParse(reader, expression), "Failed to read JSONeN expression: '%s'.", expression.begin());
	const ::gpk::array_obj<::gpk::view_const_string>	& expressionViews						= reader.View;
	for(uint32_t iView = 0; iView < expressionViews.size(); ++iView) {
		const ::gpk::view_const_string						& viewExpression						= expressionViews[iView];
		const ::gpk::SExpressionReaderType					& typeExpression						= reader.Object[iView];
		if(viewExpression.size()) {
			uint32_t										
				lenString								= viewExpression.size();
			char												bufferFormat [8192]						= {};
			sprintf_s(bufferFormat, "Expression element: %%.%us. Type: %s. Parent: %i. Begin: %u. End: %u.", lenString, ::gpk::get_value_label(typeExpression.Type).begin(), typeExpression.ParentIndex, typeExpression.Span.Begin, typeExpression.Span.End);
			info_printf(bufferFormat, viewExpression.begin());
		}
	}
	::gpk::view_const_string							evaluated								= {};
	if(reader.Tree.size()) {
		::printNode(reader.Tree[0], expression);
		int32_t												jsonNodeResultOfEvaluation				= ::evaluateNode(reader, 0, inputJSON, indexNodeJSON, output);
		gpk_necall(jsonNodeResultOfEvaluation, "Failed to evaluate expression: '%s'.", expression.begin());
		evaluated										= output;
	}
	
	// TODO: Implement expression evaluation.

	if(evaluated.size()) {
		uint32_t											lenString								= evaluated.size();
		char												bufferFormat [8192]						= {};
		sprintf_s(bufferFormat, "Result of expression evaluation: %%.%us.", lenString);
		info_printf(bufferFormat, evaluated.begin());
	}
	return 0;
}

::gpk::error_t									gpk::jsonStringFormat					(const ::gpk::view_const_string& format, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::array_pod<char_t>& output)					{
	::gpk::SStripLiteralState							stateLiteralStripper					= {};
	::gpk::array_pod<::gpk::SStripLiteralType>			typesLiteral							= {};
	::gpk::stripLiteralParse(stateLiteralStripper, typesLiteral, format);	// strip root literals 
	::gpk::array_obj<::gpk::view_const_string>			views									= {};
	::gpk::stripLiteralGetViews(views, typesLiteral, format);
	info_printf("-**- %s -**-", "Results of token search");
	for(uint32_t iView = 0; iView < views.size(); ++iView) {
		if(typesLiteral[iView].Type == ::gpk::STRIP_LITERAL_TYPE_TOKEN)	{ // we only have to solve tokens
			::gpk::view_const_string							& toResolve								= views[iView];
			gpk_necall(::jsonStringFormatResolve(views[iView], inputJSON, indexNodeJSON, toResolve), "Failed to resolve expression: '%s'", views[iView].begin());
		}
	}
	for(uint32_t iView = 0; iView < views.size(); ++iView)
		output.append(views[iView]);
	return 0;
}