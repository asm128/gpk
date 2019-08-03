#include "gpk_json_expression.h"

#include "gpk_expression.h"
#include "gpk_parse.h"

#define gpk_jexpr_info_printf // info_printf

//#define GPK_JSON_EXPRESSION_DEBUG

#if defined(GPK_JSON_EXPRESSION_DEBUG)
static ::gpk::error_t							printNode						(const ::gpk::SExpressionNode* node, const ::gpk::view_const_char& expression)			{
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

static const ::gpk::view_const_string			strEmptyObj								= "{}"		;
static const ::gpk::view_const_string			strEmptyArr								= "[]"		;
static const ::gpk::view_const_string			strNull									= "null"	;
static const ::gpk::view_const_string			strFalse								= "false"	;
static const ::gpk::view_const_string			strTrue									= "true"	;
static const ::gpk::view_const_string			strZero									= "0"		;


//static ::gpk::error_t							evaluateFirstLevelExpression			(const ::gpk::SExpressionReader & readerExpression, const uint32_t indexNodeExpression, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::view_const_string& output)	{
//
//	return 0;
//}
// Returns an index pointing to the resulting JSON element, or a -16 - the index of the resulting expression element.
static ::gpk::error_t							evaluateExpression						(const ::gpk::SExpressionReader & readerExpression, const uint32_t indexNodeExpression, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::view_const_string& output)	{
	ree_if(indexNodeJSON >= inputJSON.Tree.size(), "Invalid input json or index node: %i", indexNodeJSON)
	const ::gpk::SExpressionNode						* nodeExpression						= readerExpression.Tree[indexNodeExpression];	// Take this
	uint32_t											indexRootJSONNode						= indexNodeJSON;
	int32_t												indexJSONResult							= 0;
	::gpk::array_pod<char_t>							bufferFormat							= {};
	bufferFormat									= readerExpression.View[indexNodeExpression];
	bufferFormat.push_back(0);
	gpk_jexpr_info_printf("Processing expression: '%s'", bufferFormat.begin());
	bool												lastUnaryNot							= false;
	bool												lastCarryBool							= false;
	for(uint32_t iFirstLevelExpression = 0; iFirstLevelExpression < nodeExpression->Children.size(); ++iFirstLevelExpression) {
		const ::gpk::SExpressionNode						& childToSolve								= *nodeExpression->Children[iFirstLevelExpression];
		const ::gpk::SJSONNode								* currentJSON								= (0 <= (int32_t)indexNodeJSON) ? inputJSON.Tree[indexNodeJSON] : 0;
		//const ::gpk::SExpressionNode						* currentLiteral							= (0 <= (int32_t)indexNodeJSON) ? 0 : readerExpression.Tree[(((int32_t)indexNodeJSON) + 16) * -1];
		const ::gpk::view_const_string						currentView
			= indexNodeJSON == -3				? ::strTrue
			: indexNodeJSON == -4				? ::strFalse
			: (0 <= (int32_t)indexNodeJSON)	? inputJSON.View[indexNodeJSON]
			: readerExpression.View[(((int32_t)indexNodeJSON) + 16) * -1]
			;
		const ::gpk::view_const_string						currentExpressionView						= readerExpression.View[nodeExpression->Children[iFirstLevelExpression]->ObjectIndex];(void)currentExpressionView;
		gpk_jexpr_info_printf("Reading first-level expression: %u.", iFirstLevelExpression);
		bufferFormat									= currentView;
		bufferFormat.push_back(0);
		gpk_jexpr_info_printf("Current json view(%u): '%s'", bufferFormat.size() - 1, bufferFormat.begin());
		bufferFormat									= readerExpression.View[childToSolve.ObjectIndex];
		bufferFormat.push_back(0);
		gpk_jexpr_info_printf("Current child view (%u): '%s'", bufferFormat.size() - 1, bufferFormat.begin());
		if(::gpk::EXPRESSION_READER_TYPE_LITERAL == childToSolve.Object->Type) {
			output											= readerExpression.View[childToSolve.ObjectIndex];
			indexJSONResult									= -(childToSolve.ObjectIndex + 0xF);
			break;
		}
		else {
			if(::gpk::EXPRESSION_READER_TYPE_KEY == childToSolve.Object->Type) {
				::gpk::view_const_string							strKey										= readerExpression.View[childToSolve.ObjectIndex];
				gpk_necall(bufferFormat.resize(strKey.size() + 1024), "%s", "Out of memory?");
				sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Only objects can be accessed by key. JSON type: %%s. Key to solve: %%.%us.", strKey.size());
				ree_if(currentJSON && currentJSON->Object->Type != ::gpk::JSON_TYPE_OBJECT, bufferFormat.begin(), ::gpk::get_value_label(currentJSON->Object->Type).begin(), strKey.begin());
				if(1 == childToSolve.Children.size()
					&& (::gpk::EXPRESSION_READER_TYPE_TERM_KEY == childToSolve.Children[0]->Object->Type || ::gpk::EXPRESSION_READER_TYPE_TERM_ANY == childToSolve.Children[0]->Object->Type)) {
					const int32_t										indexOfResolvedSubExpression			= ::evaluateExpression(readerExpression, childToSolve.ObjectIndex, inputJSON, (0 >= indexJSONResult)? indexJSONResult : indexRootJSONNode, strKey);
					(void)indexOfResolvedSubExpression;
				}
				indexJSONResult									= ::gpk::jsonObjectValueGet(*inputJSON.Tree[indexNodeJSON], inputJSON.View, strKey);
				if errored(indexJSONResult) {
					sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Key not found: %%.%us.", strKey.size());
					error_printf(bufferFormat.begin(), strKey.begin());
					return -1;
				}
				output											= inputJSON.View[indexJSONResult];
			}
			else if(::gpk::EXPRESSION_READER_TYPE_TERM_INDEX == childToSolve.Object->Type) {
				ree_if(currentJSON		&& currentJSON		->Object->Type != ::gpk::JSON_TYPE_ARRAY, "Only arrays can be accessed by index. JSON type: %s.", ::gpk::get_value_label(currentJSON->Object->Type).begin());
				//ree_if(currentLiteral	&& currentLiteral	->Object->Type != ::gpk::EXPRESSION_READER_TYPE_LITERAL, "Only arrays can be accessed by index. JSON type: %s.", ::gpk::get_value_label(currentJSON->Object->Type).begin());
				::gpk::view_const_string							viewOfIndex								= {};
				const int32_t										indexOfResolvedSubExpression			= ::evaluateExpression(readerExpression, childToSolve.ObjectIndex, inputJSON, indexRootJSONNode, viewOfIndex);
				if (-1 == indexOfResolvedSubExpression) {
					gpk_necall(bufferFormat.resize(readerExpression.View[childToSolve.ObjectIndex].size() + 512), "%s", "Out of memory?");
					sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Failed to solve expression: %%.%us.", readerExpression.View[childToSolve.ObjectIndex].size());
					error_printf(bufferFormat.begin(), readerExpression.View[childToSolve.ObjectIndex].begin());
					return -1;
				}
				ree_if(indexOfResolvedSubExpression >= 0 && inputJSON.Object[indexOfResolvedSubExpression].Type != ::gpk::JSON_TYPE_NUMBER, "Arrays can only be accessed by index with a json number. Type found: %s.",::gpk::get_value_label(inputJSON.Object[indexOfResolvedSubExpression].Type).begin());
				uint64_t											numberRead								= 0;
				gpk_necall(::gpk::parseIntegerDecimal(viewOfIndex, &numberRead), "%s", "Out of memory?");
				indexJSONResult									= ::gpk::jsonArrayValueGet(*currentJSON, (uint32_t)numberRead);
				ree_if(errored(indexJSONResult), "Value not found for index: %lli.", numberRead);
				output											= inputJSON.View[indexJSONResult];
			}
			else if(::gpk::EXPRESSION_READER_TYPE_TERM_KEY == childToSolve.Object->Type) {
				ree_if(currentJSON && currentJSON->Object->Type != ::gpk::JSON_TYPE_OBJECT, "Only objects can be accessed by key. JSON type: %s.", ::gpk::get_value_label(currentJSON->Object->Type).begin());
				::gpk::view_const_string							strKey									= {};
				int32_t												indexOfResolvedSubExpression			= ::evaluateExpression(readerExpression, childToSolve.ObjectIndex, inputJSON, (0 >= indexJSONResult)? indexJSONResult : indexRootJSONNode, strKey);
				if (-1 == (indexJSONResult = indexOfResolvedSubExpression)) {
					gpk_necall(bufferFormat.resize(strKey.size() + 512), "%s", "Out of memory?");
					sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Key not found: %%.%us.", strKey.size());
					error_printf(bufferFormat.begin(), strKey.begin());
					return -1;
				}
				::gpk::SExpressionReader							reader;
				gpk_necall(bufferFormat.resize(strKey.size() + 1024), "%s", "Out of memory?");
				sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Failed to read JSONeN expression: '%%.%us'.", strKey.size());
				gpk_necall(::gpk::expressionReaderParse(reader, strKey), bufferFormat.begin(), strKey.begin());
				indexOfResolvedSubExpression					= ::gpk::jsonExpressionResolve(reader, inputJSON, indexNodeJSON, output);
				if (-1 == (indexJSONResult = indexOfResolvedSubExpression)) {
					gpk_necall(bufferFormat.resize(strKey.size() + 512), "%s", "Out of memory?");
					sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Key not found: %%.%us.", strKey.size());
					error_printf(bufferFormat.begin(), strKey.begin());
					return -1;
				}
				//indexJSONResult									= ::gpk::jsonObjectValueGet(*inputJSON.Tree[indexNodeJSON], inputJSON.View, strKey);
				//output											= inputJSON.View[indexJSONResult];
			}
			else if(::gpk::EXPRESSION_READER_TYPE_TERM_ANY == childToSolve.Object->Type) {
				//ree_if(currentJSON.Object->Type != ::gpk::JSON_TYPE_OBJECT, "Only objects can be accessed by key. JSON type: %s.", ::gpk::get_value_label(currentJSON.Object->Type).begin());
				::gpk::view_const_string							strKey									= {};
				//const int32_t										indexOfResolvedSubExpression			= ::evaluateExpression(readerExpression, childToSolve.ObjectIndex, inputJSON, (-1 != indexJSONResult)? indexJSONResult : indexRootJSONNode, strKey);
				const int32_t										indexOfResolvedSubExpression			= ::evaluateExpression(readerExpression, childToSolve.ObjectIndex, inputJSON, indexJSONResult, strKey);
				output											= strKey;
				ree_if(-1 == indexOfResolvedSubExpression, "Failed to resolve expression: %s.", "punto final");
				indexJSONResult									= indexOfResolvedSubExpression;
			}
			else if(::gpk::EXPRESSION_READER_TYPE_UNARY_NOT == childToSolve.Object->Type) {
				//ree_if(currentJSON.Object->Type != ::gpk::JSON_TYPE_OBJECT, "Only objects can be accessed by key. JSON type: %s.", ::gpk::get_value_label(currentJSON.Object->Type).begin());
				::gpk::view_const_string							strKey									= {};
				const int32_t										indexOfResolvedSubExpression			= ::evaluateExpression(readerExpression, childToSolve.ObjectIndex, inputJSON, indexJSONResult, strKey);
				output											= strKey;
				ree_if(-1 == indexOfResolvedSubExpression, "Failed to resolve expression: %s.", "punto final");
				int32_t												evalResult								= -1;
				indexJSONResult									= -(childToSolve.ObjectIndex + 0xF);
				if(lastCarryBool)
					evalResult										= lastUnaryNot ? 0 : 1;
				else {
					if(indexOfResolvedSubExpression < 0) {
						if(indexOfResolvedSubExpression <= -0xF)
							evalResult										= (strKey == strZero || strKey == strFalse ||  strKey == strNull || strKey == strEmptyObj  || 0 == strKey.size() || strKey == ::strEmptyArr) ? 0 : 1;
						else
							evalResult										= indexOfResolvedSubExpression == -3 ? 1 : 0;//(strKey == ::gpk::view_const_string{"0"} || strKey == ::gpk::view_const_string{"false"} ||  strKey == ::gpk::view_const_string{"null"} || strKey == ::gpk::view_const_string{"{}"}  || strKey == ::gpk::view_const_string{""} || strKey == ::gpk::view_const_string{"[]"}) ? 0 : 1;
					}
					else {
						const ::gpk::SJSONNode								& evaluatedJSON							= *inputJSON[indexOfResolvedSubExpression];
						if(evaluatedJSON.Object->Type == ::gpk::JSON_TYPE_NULL)
							evalResult										= 0;
						else if(evaluatedJSON.Object->Type == ::gpk::JSON_TYPE_BOOL)
							evalResult										= (strKey == strTrue) ? 1 : 0;
						else if(evaluatedJSON.Object->Type == ::gpk::JSON_TYPE_NUMBER)
							evalResult										= (strKey == strZero) ? 0 : 1;
						else if(evaluatedJSON.Object->Type == ::gpk::JSON_TYPE_STRING)
							evalResult										= strKey.size() ? 1 : 0;
						else if(evaluatedJSON.Object->Type == ::gpk::JSON_TYPE_ARRAY)
							evalResult										= inputJSON.Tree[indexOfResolvedSubExpression]->Children.size() ? 1 : 0;
						else if(evaluatedJSON.Object->Type == ::gpk::JSON_TYPE_OBJECT)
							evalResult										= inputJSON.Tree[indexOfResolvedSubExpression]->Children.size() ? 1 : 0;
						else {
							bufferFormat								= "Failed to solve expression: ";
							bufferFormat.append(readerExpression.View[childToSolve.ObjectIndex]);
							bufferFormat.push_back(0);
							warning_printf("%s", bufferFormat.begin());
							return -1;
						}
					}
				}
				indexJSONResult								= evalResult ? -4 : -3;
				lastUnaryNot								= evalResult ? false : true;
				lastCarryBool								= true;
				output										= lastUnaryNot ? ::gpk::view_const_string{strTrue.begin(), strTrue.size()} : ::gpk::view_const_string{strFalse.begin(), strFalse.size()};
			}
			else if(::gpk::EXPRESSION_READER_TYPE_TERM_BOOL == childToSolve.Object->Type) {
				int32_t												indexOfResolvedSubExpression			= -1;
				int32_t												evalResult								= -1;
				if(lastCarryBool) {
					evalResult										= lastUnaryNot ? 0 : 1;
					lastCarryBool									= false;
				}
				else {
					if(0 == currentJSON)
						evalResult										= indexNodeJSON == -3 ? 0 : 1;
					else {
						if(currentJSON->Object->Type == ::gpk::JSON_TYPE_NULL)
							evalResult										= 1;
						else if(currentJSON->Object->Type == ::gpk::JSON_TYPE_BOOL)
							evalResult										= (currentView == strTrue) ? 0 : 1;
						else if(currentJSON->Object->Type == ::gpk::JSON_TYPE_NUMBER)
							evalResult										= (currentView == strZero) ? 1 : 0;
						else if(currentJSON->Object->Type == ::gpk::JSON_TYPE_STRING)
							evalResult										= currentView.size() ? 0 : 1;
						else if(currentJSON->Object->Type == ::gpk::JSON_TYPE_ARRAY)
							evalResult										= inputJSON.Tree[indexNodeJSON]->Children.size() ? 0 : 1;
						else if(currentJSON->Object->Type == ::gpk::JSON_TYPE_OBJECT)
							evalResult										= inputJSON.Tree[indexNodeJSON]->Children.size() ? 0 : 1;
						else {
							bufferFormat								= "Failed to solve expression: ";
							bufferFormat.append(readerExpression.View[childToSolve.ObjectIndex]);
							bufferFormat.push_back(0);
							warning_printf("%s", bufferFormat.begin());
							return -1;
						}
					}
				}
				::gpk::view_const_string							viewOfExpressionResult				= {};
				{
					const uint32_t										expressionResultChildIndex			= (iFirstLevelExpression == nodeExpression->Children.size()-1) ? iFirstLevelExpression : iFirstLevelExpression + evalResult;
					const ::gpk::SExpressionNode						& childEval							= *nodeExpression->Children[expressionResultChildIndex];
					const ::gpk::view_const_string						& childEvalView						= readerExpression.View[childEval.ObjectIndex];(void)childEvalView;
					const ::gpk::SExpressionNode						& childExpr							= *childEval.Children[0];
					const ::gpk::view_const_string						& childExprView						= readerExpression.View[childExpr.ObjectIndex];(void)childExprView;
					indexOfResolvedSubExpression					= ::evaluateExpression(readerExpression, childEval.ObjectIndex, inputJSON, indexRootJSONNode, viewOfExpressionResult);
					if(-1 == indexOfResolvedSubExpression) {
						error_printf("Failed to resolve subexpression: '%s'.", childEvalView.begin());
						return -1;
					}
					output											= viewOfExpressionResult;
					indexJSONResult									= indexOfResolvedSubExpression;
				}
				iFirstLevelExpression							+= evalResult ? 0 : 1;
			}
			else {
				error_printf("Unrecognized expression reader type: '%s'.", ::gpk::get_value_label(childToSolve.Object->Type).begin());
				return -1;
			}
		}
		indexNodeJSON									= indexJSONResult;
	}
	inputJSON, indexNodeJSON, output;
	return indexJSONResult;
}

::gpk::error_t									gpk::jsonExpressionResolve				(const ::gpk::SExpressionReader & reader, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::view_const_string& output)								{
	::gpk::array_pod<char_t>							bufferFormat							= {};
	const ::gpk::array_obj<::gpk::view_const_string>	& expressionViews						= reader.View;
	for(uint32_t iView = 0; iView < expressionViews.size(); ++iView) {
		const ::gpk::view_const_string						& viewExpression						= expressionViews[iView];
		const ::gpk::SExpressionToken						& typeExpression						= reader.Object[iView];
		if(viewExpression.size()) {
			uint32_t											lenString								= viewExpression.size();
			gpk_necall(bufferFormat.resize(lenString + 1024), "%s", "Out of memory?");
			sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Expression element: %%.%us. Type: %s. Parent: %i. Begin: %u. End: %u.", lenString, ::gpk::get_value_label(typeExpression.Type).begin(), typeExpression.ParentIndex, typeExpression.Span.Begin, typeExpression.Span.End);
			gpk_jexpr_info_printf(bufferFormat.begin(), viewExpression.begin());
		}
	}
	::gpk::view_const_string							evaluated								= {};
	int32_t												jsonNodeResultOfEvaluation				= -1;
	if(reader.Tree.size()) {
#if defined(GPK_JSON_EXPRESSION_DEBUG)
		::printNode(reader.Tree[0], reader.View[0]);
#endif
		jsonNodeResultOfEvaluation						= ::evaluateExpression(reader, 0, inputJSON, indexNodeJSON, output);
		if (-1 == jsonNodeResultOfEvaluation) {
			gpk_necall(bufferFormat.resize(reader.View[0].size() + 1024), "%s", "Out of memory?");
			sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Failed to evaluate expression: %%.%us.", reader.View[0].size());
			error_printf(bufferFormat.begin(), reader.View[0].begin());
			return -1;
		}
		evaluated										= output;
	}
	if(evaluated.size()) {
		uint32_t											lenString								= evaluated.size();
		gpk_necall(bufferFormat.resize(lenString + 1024), "%s", "Out of memory?");
		sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Result of expression evaluation: %%.%us.", lenString);
		gpk_jexpr_info_printf(bufferFormat.begin(), evaluated.begin());
	}
	return jsonNodeResultOfEvaluation;
}

::gpk::error_t									gpk::jsonExpressionResolve				(const ::gpk::view_const_string & expression, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::view_const_string& output)								{
	::gpk::array_pod<char_t>							bufferFormat							= {};
	::gpk::SExpressionReader							reader;
	gpk_necall(bufferFormat.resize(expression.size() + 1024), "%s", "Out of memory?");
	sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Failed to read JSONeN expression: '%%.%us'.", expression.size());
	gpk_necall(::gpk::expressionReaderParse(reader, expression), bufferFormat.begin(), expression.begin());
	return ::gpk::jsonExpressionResolve(reader, inputJSON, indexNodeJSON, output);
}

::gpk::error_t									gpk::jsonStringFormat					(const ::gpk::view_const_string& format, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::array_pod<char_t>& output)					{
	::gpk::SStripLiteralState							stateLiteralStripper					= {};
	::gpk::array_pod<::gpk::SStripLiteralType>			typesLiteral							= {};
	gpk_necall(::gpk::stripLiteralParse(stateLiteralStripper, typesLiteral, format), "%s", "Unknown error.");	// strip root literals
	::gpk::array_obj<::gpk::view_const_string>			views									= {};
	gpk_necall(::gpk::stripLiteralGetViews(views, typesLiteral, format), "%s", "Unknown error.");
	gpk_jexpr_info_printf("-**- %s -**-", "Results of token search");
	for(uint32_t iView = 0; iView < views.size(); ++iView) {
		::gpk::view_const_string							& toResolve								= views[iView];
		if(typesLiteral[iView].Type == ::gpk::STRIP_LITERAL_TYPE_TOKEN)	{ // we only have to solve tokens
			char												bufferFormat [8192]						= {};
			gpk_necall(::gpk::jsonExpressionResolve(toResolve, inputJSON, indexNodeJSON, toResolve), bufferFormat, sprintf_s(bufferFormat, "%%u|Failed to resolve expression: %%.%us.", toResolve.size()), toResolve.begin());
		}
	}
	for(uint32_t iView = 0; iView < views.size(); ++iView)
		output.append(views[iView]);
	return 0;
}
