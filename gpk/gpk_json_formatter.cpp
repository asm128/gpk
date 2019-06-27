#include "gpk_expression.h"
#include "gpk_json_formatter.h"

#include "gpk_parse.h"

static char										g_bufferFormat [8192]				= {};

static ::gpk::error_t							printNode						(::gpk::SExpressionNode* node, const ::gpk::view_const_char& expression)			{
	uint32_t											lenString						= node->Object->Span.End - node->Object->Span.Begin;
	sprintf_s(g_bufferFormat, "Entering expression node type: %%u (%%s). Node Span: {%%u, %%u}. Parent index: %%u. Object index: %%u. Text: %%.%us", lenString);
	info_printf(g_bufferFormat, node->Object->Type, ::gpk::get_value_label(node->Object->Type).begin(), node->Object->Span.Begin, node->Object->Span.End, node->Object->ParentIndex, node->ObjectIndex, &expression[node->Object->Span.Begin]);
	for(uint32_t iChildren = 0; iChildren < node->Children.size(); ++iChildren)
		::printNode(node->Children[iChildren], expression);
	sprintf_s(g_bufferFormat, "Exiting expression node type: %%u (%%s). Node Span: {%%u, %%u}. Parent index: %%u. Object index: %%u. Text: %%.%us", lenString);
	info_printf(g_bufferFormat, node->Object->Type, ::gpk::get_value_label(node->Object->Type).begin(), node->Object->Span.Begin, node->Object->Span.End, node->Object->ParentIndex, node->ObjectIndex, &expression[node->Object->Span.Begin]);
	return 0;
}

static ::gpk::error_t							evaluateNode							(::gpk::SExpressionReader & readerExpression, uint32_t indexNodeExpression, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::view_const_string& output)						{
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

static ::gpk::error_t							evaluateExpression						(::gpk::SExpressionReader & readerExpression, uint32_t indexNodeExpression, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::view_const_string& output)	{
	const ::gpk::SExpressionNode						* nodeExpression						= readerExpression.Tree[indexNodeExpression];	// Take this 
	//const ::gpk::SJSONNode								& inputJSONNode							= *inputJSON.Tree[indexNodeJSON]; 

	uint32_t											indexRootJSONNode						= indexNodeJSON;
	int32_t												indexJSONResult							= -1;
	::gpk::array_pod<int32_t>							listOfJSONElemIndices					= {};	// U
	char												bufferFormat [4096]						= {};
	for(uint32_t iFirstLevelExpression = 0; iFirstLevelExpression < nodeExpression->Children.size(); ++iFirstLevelExpression) {
		const ::gpk::SExpressionNode						& childToSolve								= *nodeExpression->Children[iFirstLevelExpression];
		const ::gpk::SJSONNode								& currentJSON								= *inputJSON.Tree[indexNodeJSON];
		if(::gpk::EXPRESSION_READER_TYPE_KEY == childToSolve.Object->Type) {
			const ::gpk::view_const_string						& strKey									= readerExpression.View[childToSolve.ObjectIndex];
			sprintf_s(bufferFormat, "Only objects can be accessed by key. JSON type: %%s. Key to solve: %%.%us.", strKey.size());
			ree_if(currentJSON.Object->Type != ::gpk::JSON_TYPE_OBJECT, bufferFormat, ::gpk::get_value_label(currentJSON.Object->Type).begin(), strKey.begin());
			indexJSONResult									= ::gpk::jsonObjectValueGet(*inputJSON.Tree[indexNodeJSON], inputJSON.View, strKey);
			if errored(indexJSONResult) {
				sprintf_s(bufferFormat, "Key not found: %%.%us.", strKey.size());
				error_printf(bufferFormat, strKey.begin());
				return -1;
			}
			output											= inputJSON.View[indexJSONResult];
		}
		else if(::gpk::EXPRESSION_READER_TYPE_INDEX == childToSolve.Object->Type) {
			ree_if(currentJSON.Object->Type != ::gpk::JSON_TYPE_ARRAY, "Only arrays can be accessed by key. JSON type: %s.", ::gpk::get_value_label(currentJSON.Object->Type).begin());
			uint64_t											numberRead								= 0;
			const ::gpk::view_const_string						& viewOfIndex							= readerExpression.View[childToSolve.ObjectIndex];
			::gpk::parseArbitraryBaseInteger(10, "0123456789", viewOfIndex, &numberRead);
			indexJSONResult									= ::gpk::jsonArrayValueGet(currentJSON, (uint32_t)numberRead);
			ree_if(errored(indexJSONResult), "Value not found for index: %lli.", numberRead);
			output											= inputJSON.View[indexJSONResult];
		}
		else if(::gpk::EXPRESSION_READER_TYPE_EXPRESSION_INDEX == childToSolve.Object->Type) {
			ree_if(currentJSON.Object->Type != ::gpk::JSON_TYPE_ARRAY, "Only arrays can be accessed by key. JSON type: %s.", ::gpk::get_value_label(currentJSON.Object->Type).begin());
			::gpk::view_const_string							viewOfIndex								= {};
			const int32_t										indexOfResolvedSubExpression			= ::evaluateExpression(readerExpression, childToSolve.ObjectIndex, inputJSON, indexRootJSONNode, viewOfIndex);
			if errored(indexJSONResult) {
				sprintf_s(bufferFormat, "Failed to solve expression: %%.%us.", readerExpression.View[childToSolve.ObjectIndex].size());
				error_printf(bufferFormat, readerExpression.View[childToSolve.ObjectIndex].begin());
				return -1;
			}
			uint64_t											numberRead								= 0;
			::gpk::parseArbitraryBaseInteger(10, "0123456789", viewOfIndex, &numberRead);
			ree_if(errored(indexJSONResult), "Value not found for index: %lli.", numberRead);
			indexJSONResult									= ::gpk::jsonArrayValueGet(currentJSON, (uint32_t)numberRead);
			output											= inputJSON.View[indexJSONResult];
		}
		else if(::gpk::EXPRESSION_READER_TYPE_EXPRESSION_KEY == childToSolve.Object->Type) {
			ree_if(currentJSON.Object->Type != ::gpk::JSON_TYPE_OBJECT, "Only objects can be accessed by key. JSON type: %s.", ::gpk::get_value_label(currentJSON.Object->Type).begin());
			::gpk::view_const_string							strKey									= {};
			const int32_t										indexOfResolvedSubExpression			= ::evaluateExpression(readerExpression, childToSolve.ObjectIndex, inputJSON, indexRootJSONNode, strKey);
			indexJSONResult									= ::gpk::jsonObjectValueGet(*inputJSON.Tree[indexNodeJSON], inputJSON.View, strKey);
			if errored(indexJSONResult) {
				sprintf_s(bufferFormat, "Key not found: %%.%us.", strKey.size());
				error_printf(bufferFormat, strKey.begin());
				return -1;
			}
			output											= inputJSON.View[indexJSONResult];
		}
		else {
			error_printf("Unrecognized expression reader type: '%s'.", ::gpk::get_value_label(childToSolve.Object->Type).begin());
			return -1;
		}
		indexNodeJSON									= indexJSONResult;
	}
	inputJSON, indexNodeJSON, output;
	return indexJSONResult;
}

static ::gpk::error_t							jsonStringFormatResolve					(const ::gpk::view_const_string & expression, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::view_const_string& output)								{
	::gpk::view_const_string							resultOfExpressionEval					= {};
	::gpk::SExpressionReader							reader;
	gpk_necall(::gpk::expressionReaderParse(reader, expression), "Failed to read JSONeN expression: '%s'.", expression.begin());
	const ::gpk::array_obj<::gpk::view_const_string>	& expressionViews						= reader.View;
	for(uint32_t iView = 0; iView < expressionViews.size(); ++iView) {
		const ::gpk::view_const_string						& viewExpression						= expressionViews[iView];
		const ::gpk::SExpressionReaderType					& typeExpression						= reader.Object[iView];
		if(viewExpression.size()) {
			uint32_t											lenString								= viewExpression.size();
			char												bufferFormat [8192]						= {};
			sprintf_s(bufferFormat, "Expression element: %%.%us. Type: %s. Parent: %i. Begin: %u. End: %u.", lenString, ::gpk::get_value_label(typeExpression.Type).begin(), typeExpression.ParentIndex, typeExpression.Span.Begin, typeExpression.Span.End);
			info_printf(bufferFormat, viewExpression.begin());
		}
	}
	::gpk::view_const_string							evaluated								= {};
	if(reader.Tree.size()) {
		::printNode(reader.Tree[0], expression);
		//int32_t												jsonNodeResultOfEvaluation				= ::evaluateNode(reader, 0, inputJSON, indexNodeJSON, output);
		int32_t												jsonNodeResultOfEvaluation				= ::evaluateExpression(reader, 0, inputJSON, indexNodeJSON, output);
		if errored(jsonNodeResultOfEvaluation) {
			char												bufferFormat[8192] = {};
			sprintf_s(bufferFormat, "Failed to evaluate expression: %%.%us.", expression.size());
			error_printf(bufferFormat, expression.begin());
			return -1;
		}
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
			char												bufferFormat [8192]						= {};
			sprintf_s(bufferFormat, "Failed to resolve expression: %%.%us.", toResolve.size());
			gpk_necall(::jsonStringFormatResolve(toResolve, inputJSON, indexNodeJSON, toResolve), bufferFormat, toResolve.begin());
		}
	}
	for(uint32_t iView = 0; iView < views.size(); ++iView)
		output.append(views[iView]);
	return 0;
}