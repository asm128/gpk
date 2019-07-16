#include "gpk_json_expression.h"

#include "gpk_expression.h"
#include "gpk_parse.h"

#define gpk_jexpr_info_printf // info_printf

//#define GPK_JSON_EXPRESSION_DEBUG

#if defined(GPK_JSON_EXPRESSION_DEBUG)
static ::gpk::error_t							printNode						(::gpk::SExpressionNode* node, const ::gpk::view_const_char& expression)			{
	uint32_t											lenString						= node->Object->Span.End - node->Object->Span.Begin;
	::gpk::array_pod<char_t>							bufferFormat					= {};
	bufferFormat.resize(2 * lenString + 1024);
	sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Entering expression node type: %%u (%%s). Node Span: {%%u, %%u}. Parent index: %%u. Object index: %%u. Text: %%.%us", lenString);
	info_printf(bufferFormat.begin(), node->Object->Type, ::gpk::get_value_label(node->Object->Type).begin(), node->Object->Span.Begin, node->Object->Span.End, node->Object->ParentIndex, node->ObjectIndex, &expression[node->Object->Span.Begin]);
	for(uint32_t iChildren = 0; iChildren < node->Children.size(); ++iChildren)
		::printNode(node->Children[iChildren], expression);
	sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Exiting expression node type: %%u (%%s). Node Span: {%%u, %%u}. Parent index: %%u. Object index: %%u. Text: %%.%us", lenString);
	info_printf(bufferFormat.begin(), node->Object->Type, ::gpk::get_value_label(node->Object->Type).begin(), node->Object->Span.Begin, node->Object->Span.End, node->Object->ParentIndex, node->ObjectIndex, &expression[node->Object->Span.Begin]);
	return 0;
}
#endif

static ::gpk::error_t							evaluateExpression						(::gpk::SExpressionReader & readerExpression, uint32_t indexNodeExpression, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::view_const_string& output)	{
	ree_if(indexNodeJSON >= inputJSON.Tree.size(), "Invalid input json or index node: %i", indexNodeJSON)
	const ::gpk::SExpressionNode						* nodeExpression						= readerExpression.Tree[indexNodeExpression];	// Take this 
	//const ::gpk::SJSONNode								& inputJSONNode							= *inputJSON.Tree[indexNodeJSON]; 

	uint32_t											indexRootJSONNode						= indexNodeJSON;
	int32_t												indexJSONResult							= -1;
	::gpk::array_pod<int32_t>							listOfJSONElemIndices					= {};	// U
	::gpk::array_pod<char_t>							bufferFormat							= {};
	for(uint32_t iFirstLevelExpression = 0; iFirstLevelExpression < nodeExpression->Children.size(); ++iFirstLevelExpression) {
		const ::gpk::SExpressionNode						& childToSolve								= *nodeExpression->Children[iFirstLevelExpression];
		const ::gpk::SJSONNode								& currentJSON								= *inputJSON.Tree[indexNodeJSON];
		if(::gpk::EXPRESSION_READER_TYPE_KEY == childToSolve.Object->Type) {
			const ::gpk::view_const_string						& strKey									= readerExpression.View[childToSolve.ObjectIndex];
			bufferFormat.resize(strKey.size() + 1024);
			sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Only objects can be accessed by key. JSON type: %%s. Key to solve: %%.%us.", strKey.size());
			ree_if(currentJSON.Object->Type != ::gpk::JSON_TYPE_OBJECT, bufferFormat.begin(), ::gpk::get_value_label(currentJSON.Object->Type).begin(), strKey.begin());
			indexJSONResult									= ::gpk::jsonObjectValueGet(*inputJSON.Tree[indexNodeJSON], inputJSON.View, strKey);
			if errored(indexJSONResult) {
				sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Key not found: %%.%us.", strKey.size());
				error_printf(bufferFormat.begin(), strKey.begin());
				return -1;
			}
			output											= inputJSON.View[indexJSONResult];
		}
		else if(::gpk::EXPRESSION_READER_TYPE_INDEX == childToSolve.Object->Type) {
			ree_if(currentJSON.Object->Type != ::gpk::JSON_TYPE_ARRAY, "Only arrays can be accessed by key. JSON type: %s.", ::gpk::get_value_label(currentJSON.Object->Type).begin());
			uint64_t											numberRead								= 0;
			const ::gpk::view_const_string						& viewOfIndex							= readerExpression.View[childToSolve.ObjectIndex];
			uint32_t											countDigits								= (uint32_t)::gpk::parseArbitraryBaseInteger(10, "0123456789", viewOfIndex, &numberRead);
			gwarn_if(countDigits != viewOfIndex.size(), "countDigits: %u, viewOfIndex: %u.", countDigits, viewOfIndex.size())
			indexJSONResult									= ::gpk::jsonArrayValueGet(currentJSON, (uint32_t)numberRead);
			ree_if(errored(indexJSONResult), "Value not found for index: %lli.", numberRead);
			output											= inputJSON.View[indexJSONResult];
		}
		else if(::gpk::EXPRESSION_READER_TYPE_EXPRESSION_INDEX == childToSolve.Object->Type) {
			ree_if(currentJSON.Object->Type != ::gpk::JSON_TYPE_ARRAY, "Only arrays can be accessed by key. JSON type: %s.", ::gpk::get_value_label(currentJSON.Object->Type).begin());
			::gpk::view_const_string							viewOfIndex								= {};
			const int32_t										indexOfResolvedSubExpression			= ::evaluateExpression(readerExpression, childToSolve.ObjectIndex, inputJSON, indexRootJSONNode, viewOfIndex);
			if errored(indexOfResolvedSubExpression) {
				bufferFormat.resize(readerExpression.View[childToSolve.ObjectIndex].size() + 512);
				sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Failed to solve expression: %%.%us.", readerExpression.View[childToSolve.ObjectIndex].size());
				error_printf(bufferFormat.begin(), readerExpression.View[childToSolve.ObjectIndex].begin());
				return -1;
			}
			uint64_t											numberRead								= 0;
			::gpk::parseArbitraryBaseInteger(10, "0123456789", viewOfIndex, &numberRead);
			indexJSONResult									= ::gpk::jsonArrayValueGet(currentJSON, (uint32_t)numberRead);
			ree_if(errored(indexJSONResult), "Value not found for index: %lli.", numberRead);
			output											= inputJSON.View[indexJSONResult];
		}
		else if(::gpk::EXPRESSION_READER_TYPE_EXPRESSION_KEY == childToSolve.Object->Type) {
			ree_if(currentJSON.Object->Type != ::gpk::JSON_TYPE_OBJECT, "Only objects can be accessed by key. JSON type: %s.", ::gpk::get_value_label(currentJSON.Object->Type).begin());
			::gpk::view_const_string							strKey									= {};
			const int32_t										indexOfResolvedSubExpression			= ::evaluateExpression(readerExpression, childToSolve.ObjectIndex, inputJSON, indexRootJSONNode, strKey);
			(void)indexOfResolvedSubExpression;
			indexJSONResult									= ::gpk::jsonObjectValueGet(*inputJSON.Tree[indexNodeJSON], inputJSON.View, strKey);
			if errored(indexJSONResult) {
				bufferFormat.resize(strKey.size() + 512);
				sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Key not found: %%.%us.", strKey.size());
				error_printf(bufferFormat.begin(), strKey.begin());
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

::gpk::error_t									gpk::jsonExpressionResolve				(const ::gpk::view_const_string & expression, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::view_const_string& output)								{
	::gpk::array_pod<char_t>							bufferFormat							= {};
	::gpk::view_const_string							resultOfExpressionEval					= {};
	::gpk::SExpressionReader							reader;
	bufferFormat.resize(expression.size() + 1024);
	sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Failed to read JSONeN expression: '%%.%us'.", expression.size());
	gpk_necall(::gpk::expressionReaderParse(reader, expression), bufferFormat.begin(), expression.begin());
	const ::gpk::array_obj<::gpk::view_const_string>	& expressionViews						= reader.View;
	for(uint32_t iView = 0; iView < expressionViews.size(); ++iView) {
		const ::gpk::view_const_string						& viewExpression						= expressionViews[iView];
		const ::gpk::SExpressionReaderType					& typeExpression						= reader.Object[iView];
		if(viewExpression.size()) {
			uint32_t											lenString								= viewExpression.size();
			bufferFormat.resize(lenString + 1024);
			sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Expression element: %%.%us. Type: %s. Parent: %i. Begin: %u. End: %u.", lenString, ::gpk::get_value_label(typeExpression.Type).begin(), typeExpression.ParentIndex, typeExpression.Span.Begin, typeExpression.Span.End);
			gpk_jexpr_info_printf(bufferFormat.begin(), viewExpression.begin());
		}
	}
	::gpk::view_const_string							evaluated								= {};
	int32_t												jsonNodeResultOfEvaluation				= -1;
	if(reader.Tree.size()) {
#if defined(GPK_JSON_EXPRESSION_DEBUG)
		::printNode(reader.Tree[0], expression);
#endif
		jsonNodeResultOfEvaluation						= ::evaluateExpression(reader, 0, inputJSON, indexNodeJSON, output);
		if errored(jsonNodeResultOfEvaluation) {
			bufferFormat.resize(expression.size() + 1024);
			sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Failed to evaluate expression: %%.%us.", expression.size());
			error_printf(bufferFormat.begin(), expression.begin());
			return -1;
		}
		evaluated										= output;
	}
	if(evaluated.size()) {
		uint32_t											lenString								= evaluated.size();
		bufferFormat.resize(lenString + 1024);
		sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Result of expression evaluation: %%.%us.", lenString);
		gpk_jexpr_info_printf(bufferFormat.begin(), evaluated.begin());
	}
	return jsonNodeResultOfEvaluation;
}

::gpk::error_t									gpk::jsonStringFormat					(const ::gpk::view_const_string& format, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::array_pod<char_t>& output)					{
	::gpk::SStripLiteralState							stateLiteralStripper					= {};
	::gpk::array_pod<::gpk::SStripLiteralType>			typesLiteral							= {};
	::gpk::stripLiteralParse(stateLiteralStripper, typesLiteral, format);	// strip root literals 
	::gpk::array_obj<::gpk::view_const_string>			views									= {};
	::gpk::stripLiteralGetViews(views, typesLiteral, format);
	gpk_jexpr_info_printf("-**- %s -**-", "Results of token search");
	for(uint32_t iView = 0; iView < views.size(); ++iView) {
		if(typesLiteral[iView].Type == ::gpk::STRIP_LITERAL_TYPE_TOKEN)	{ // we only have to solve tokens
			::gpk::view_const_string							& toResolve								= views[iView];
			char												bufferFormat [8192]						= {};
			gpk_necall(::gpk::jsonExpressionResolve(toResolve, inputJSON, indexNodeJSON, toResolve), bufferFormat, sprintf_s(bufferFormat, "%%u|Failed to resolve expression: %%.%us.", toResolve.size()), toResolve.begin());
		}
	}
	for(uint32_t iView = 0; iView < views.size(); ++iView)
		output.append(views[iView]);
	return 0;
}
