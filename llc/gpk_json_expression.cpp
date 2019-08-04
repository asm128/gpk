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

static ::gpk::error_t							evaluateJSONBool						(const ::gpk::SJSONNode & jsonToEvaluate, const ::gpk::view_const_string & boolView)	{
	int32_t												evalResult								= -1;
	if(jsonToEvaluate.Object->Type == ::gpk::JSON_TYPE_NULL)
		evalResult										= 0;
	else if(jsonToEvaluate.Object->Type == ::gpk::JSON_TYPE_BOOL)
		evalResult										= (boolView == strTrue) ? 1 : 0;
	else if(jsonToEvaluate.Object->Type == ::gpk::JSON_TYPE_NUMBER)
		evalResult										= (boolView == strZero) ? 0 : 1;
	else if(jsonToEvaluate.Object->Type == ::gpk::JSON_TYPE_STRING)
		evalResult										= boolView.size() ? 1 : 0;
	else if(jsonToEvaluate.Object->Type == ::gpk::JSON_TYPE_ARRAY)
		evalResult										= jsonToEvaluate.Children.size() ? 1 : 0;
	else if(jsonToEvaluate.Object->Type == ::gpk::JSON_TYPE_OBJECT)
		evalResult										= jsonToEvaluate.Children.size() ? 1 : 0;
	else {
		::gpk::array_pod<char_t>							bufferFormat							= {};
		bufferFormat									= "Failed to solve expression: ";
		bufferFormat.append(boolView);
		bufferFormat.push_back(0);
		error_printf("%s", bufferFormat.begin());
	}
	return evalResult;
}

static ::gpk::error_t							evaluateBoolResult						(const ::gpk::SJSONNode * jsonToEvaluate, int32_t indexNodeJSON, const ::gpk::view_const_string & boolView)	{
	::gpk::error_t										evalResult								= -1;
		if(indexNodeJSON <= -0xF)
			evalResult										= (boolView == strZero || boolView == strFalse ||  boolView == strNull || boolView == strEmptyObj  || 0 == boolView.size() || boolView == ::strEmptyArr) ? 0 : 1;
		else if(indexNodeJSON >= 0)
			evalResult										= ::evaluateJSONBool(*jsonToEvaluate, boolView);
		else
			evalResult										= indexNodeJSON == -3 ? 1 : 0;//(strKey == ::gpk::view_const_string{"0"} || strKey == ::gpk::view_const_string{"false"} ||  strKey == ::gpk::view_const_string{"null"} || strKey == ::gpk::view_const_string{"{}"}  || strKey == ::gpk::view_const_string{""} || strKey == ::gpk::view_const_string{"[]"}) ? 0 : 1;
	return evalResult;
}
struct SEvaluationStepResult {
	uint32_t											IndexRootJSONNode						;
	int32_t												IndexJSONResult							;
	bool												LastBoolValue							;
	bool												LastBoolCarry							;

	::gpk::error_t										SetBoolCarry							(int32_t evalResult, ::gpk::view_const_string & output)	{
		if(evalResult) {
			output												= ::strTrue	;
			IndexJSONResult										= -3	;
			LastBoolValue										= true	;
			LastBoolCarry										= true	;
		}
		else {
			output												= ::strFalse;
			IndexJSONResult										= -4	;
			LastBoolValue										= false	;
			LastBoolCarry										= false	;
		}
		return 0;
	}
};

static ::gpk::error_t							evaluateAndClearBoolCarry				(SEvaluationStepResult & lastResult, const ::gpk::SJSONNode * jsonToEvaluate, int32_t indexNodeJSON, const ::gpk::view_const_string & boolView)	{
	if(false == lastResult.LastBoolCarry)
		return ::evaluateBoolResult(jsonToEvaluate, (int32_t)indexNodeJSON, boolView);
	else {
		lastResult.LastBoolCarry						= false;
		return lastResult.LastBoolValue ? 1 : 0;
	}
}

// Returns an index pointing to the resulting JSON element, or a -0xF - the index of the resulting expression element.
static ::gpk::error_t							evaluateExpression						(const ::gpk::SExpressionReader & readerExpression, const uint32_t indexNodeExpression, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::view_const_string& output)	{
	ree_if(indexNodeJSON >= inputJSON.Tree.size(), "Invalid input json or index node: %i", indexNodeJSON)
	const ::gpk::SExpressionNode						* nodeExpression						= readerExpression.Tree[indexNodeExpression];	// Take this
	::gpk::array_pod<char_t>							bufferFormat							= {};
	bufferFormat									= readerExpression.View[indexNodeExpression];
	bufferFormat.push_back(0);
	gpk_jexpr_info_printf("Processing expression: '%s'", bufferFormat.begin());
	SEvaluationStepResult								lastResult								= {indexNodeJSON,};
	for(uint32_t iFirstLevelExpression = 0; iFirstLevelExpression < nodeExpression->Children.size(); ++iFirstLevelExpression) {
		const ::gpk::SExpressionNode						& childToSolve								= *nodeExpression->Children[iFirstLevelExpression];
		const ::gpk::SJSONNode								* currentJSON								= (0 <= (int32_t)indexNodeJSON) ? inputJSON.Tree[indexNodeJSON] : 0;
		//const ::gpk::SExpressionNode						* currentLiteral							= (0 <= (int32_t)indexNodeJSON) ? 0 : readerExpression.Tree[(((int32_t)indexNodeJSON) + 0xF) * -1];
		const ::gpk::view_const_string						currentView
			= indexNodeJSON == -3				? ::strTrue
			: indexNodeJSON == -4				? ::strFalse
			: (0 <= (int32_t)indexNodeJSON)	? inputJSON.View[indexNodeJSON]
			: readerExpression.View[(((int32_t)indexNodeJSON) + 0xF) * -1]
			;
		const ::gpk::view_const_string						currentExpressionView						= readerExpression.View[nodeExpression->Children[iFirstLevelExpression]->ObjectIndex];(void)currentExpressionView;
		gpk_jexpr_info_printf("Reading first-level expression: %u.", iFirstLevelExpression);
		bufferFormat									= currentView;
		bufferFormat.push_back(0);
		gpk_jexpr_info_printf("Current json view(%u): '%s'", bufferFormat.size() - 1, bufferFormat.begin());
		bufferFormat									= readerExpression.View[childToSolve.ObjectIndex];
		bufferFormat.push_back(0);
		gpk_jexpr_info_printf("Current child view (%u): '%s'", bufferFormat.size() - 1, bufferFormat.begin());
		if(::gpk::EXPRESSION_READER_TYPE_LITERAL == childToSolve.Token->Type) {
			output											= readerExpression.View[childToSolve.ObjectIndex];
			lastResult.IndexJSONResult									= -(childToSolve.ObjectIndex + 0xF);
		}
		else {
			if(::gpk::EXPRESSION_READER_TYPE_KEY == childToSolve.Token->Type) {
				::gpk::view_const_string							strKey										= readerExpression.View[childToSolve.ObjectIndex];
				gpk_necall(bufferFormat.resize(strKey.size() + 1024), "%s", "Out of memory?");
				sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Only objects can be accessed by key. JSON type: %%s. Key to solve: %%.%us.", strKey.size());
				ree_if(currentJSON && currentJSON->Object->Type != ::gpk::JSON_TYPE_OBJECT, bufferFormat.begin(), ::gpk::get_value_label(currentJSON->Object->Type).begin(), strKey.begin());
				if(1 == childToSolve.Children.size()
					&& (::gpk::EXPRESSION_READER_TYPE_TERM_KEY == childToSolve.Children[0]->Token->Type || ::gpk::EXPRESSION_READER_TYPE_TERM_ANY == childToSolve.Children[0]->Token->Type)) {
					const int32_t										indexOfResolvedSubExpression			= ::evaluateExpression(readerExpression, childToSolve.ObjectIndex, inputJSON, (0 >= lastResult.IndexJSONResult)? lastResult.IndexJSONResult : lastResult.IndexRootJSONNode, strKey);
					(void)indexOfResolvedSubExpression;
				}
				lastResult.IndexJSONResult									= ::gpk::jsonObjectValueGet(*inputJSON.Tree[indexNodeJSON], inputJSON.View, strKey);
				if errored(lastResult.IndexJSONResult) {
					sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Key not found: %%.%us.", strKey.size());
					error_printf(bufferFormat.begin(), strKey.begin());
					return -1;
				}
				output											= inputJSON.View[lastResult.IndexJSONResult];
			}
			else if(::gpk::EXPRESSION_READER_TYPE_TERM_INDEX == childToSolve.Token->Type) {
				ree_if(currentJSON && currentJSON->Object->Type != ::gpk::JSON_TYPE_ARRAY && currentJSON->Object->Type != ::gpk::JSON_TYPE_STRING, "Only arrays can be accessed by index. JSON type: %s.", ::gpk::get_value_label(currentJSON->Object->Type).begin());
				::gpk::view_const_string							viewOfIndex								= {};
				const int32_t										indexOfResolvedSubExpression			= ::evaluateExpression(readerExpression, childToSolve.ObjectIndex, inputJSON, lastResult.IndexRootJSONNode, viewOfIndex);
				if (-1 == indexOfResolvedSubExpression) {
					gpk_necall(bufferFormat.resize(readerExpression.View[childToSolve.ObjectIndex].size() + 512), "%s", "Out of memory?");
					sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Failed to solve expression: %%.%us.", readerExpression.View[childToSolve.ObjectIndex].size());
					error_printf(bufferFormat.begin(), readerExpression.View[childToSolve.ObjectIndex].begin());
					return -1;
				}
				ree_if(indexOfResolvedSubExpression >= 0 && inputJSON.Object[indexOfResolvedSubExpression].Type != ::gpk::JSON_TYPE_NUMBER, "Arrays can only be accessed by index with a json number. Type found: %s.",::gpk::get_value_label(inputJSON.Object[indexOfResolvedSubExpression].Type).begin());
				uint64_t											numberRead								= 0;
				gpk_necall(::gpk::parseIntegerDecimal(viewOfIndex, &numberRead), "%s", "Out of memory?");
				if(currentJSON->Object->Type == ::gpk::JSON_TYPE_STRING)
					output											= {&inputJSON.View[lastResult.IndexJSONResult][(uint32_t)numberRead], 1};
				else {
					lastResult.IndexJSONResult						= ::gpk::jsonArrayValueGet(*currentJSON, (uint32_t)numberRead);
					ree_if(errored(lastResult.IndexJSONResult), "Value not found for index: %lli.", numberRead);
					output											= inputJSON.View[lastResult.IndexJSONResult];
				}
			}
			else if(::gpk::EXPRESSION_READER_TYPE_TERM_KEY == childToSolve.Token->Type) {
				ree_if(currentJSON && currentJSON->Object->Type != ::gpk::JSON_TYPE_OBJECT, "Only objects can be accessed by key. JSON type: %s.", ::gpk::get_value_label(currentJSON->Object->Type).begin());
				::gpk::view_const_string							strKey									= {};
				int32_t												indexOfResolvedSubExpression			= ::evaluateExpression(readerExpression, childToSolve.ObjectIndex, inputJSON, (0 >= lastResult.IndexJSONResult)? lastResult.IndexJSONResult : lastResult.IndexRootJSONNode, strKey);
				if (-1 == (lastResult.IndexJSONResult = indexOfResolvedSubExpression)) {
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
				if (-1 == (lastResult.IndexJSONResult = indexOfResolvedSubExpression)) {
					gpk_necall(bufferFormat.resize(strKey.size() + 512), "%s", "Out of memory?");
					sprintf_s(bufferFormat.begin(), bufferFormat.size(), "Key not found: %%.%us.", strKey.size());
					error_printf(bufferFormat.begin(), strKey.begin());
					return -1;
				}
			}
			else if(::gpk::EXPRESSION_READER_TYPE_TERM_ANY == childToSolve.Token->Type) {
				::gpk::view_const_string							strKey									= {};
				const int32_t										indexOfResolvedSubExpression			= ::evaluateExpression(readerExpression, childToSolve.ObjectIndex, inputJSON, lastResult.IndexJSONResult, strKey);
				output											= strKey;
				ree_if(-1 == indexOfResolvedSubExpression, "Failed to resolve expression: %s.", "punto final");
				lastResult.IndexJSONResult									= indexOfResolvedSubExpression;
			}
			else if(::gpk::EXPRESSION_READER_TYPE_UNARY_NOT == childToSolve.Token->Type) {
				::gpk::view_const_string							viewOfExpressionResult					= {};
				const int32_t										indexOfResolvedSubExpression			= ::evaluateExpression(readerExpression, childToSolve.ObjectIndex, inputJSON, lastResult.IndexJSONResult, viewOfExpressionResult);
				output											= viewOfExpressionResult;
				ree_if(-1 == indexOfResolvedSubExpression, "Failed to resolve expression: %s.", "punto final");
				int32_t												evalResult								= -1;
				if(lastResult.LastBoolCarry)
					evalResult										= lastResult.LastBoolValue ? 1 : 0;
				else
					evalResult										= ::evaluateBoolResult((indexOfResolvedSubExpression >= 0) ? inputJSON[indexOfResolvedSubExpression] : 0, indexOfResolvedSubExpression, viewOfExpressionResult);

				lastResult.SetBoolCarry(0 == evalResult, output);
			}
			else if(::gpk::EXPRESSION_READER_TYPE_TERM_BOOL == childToSolve.Token->Type) {
				int32_t												indexOfResolvedSubExpression		= -1;
				const int32_t										prevResult							= ::evaluateAndClearBoolCarry(lastResult, currentJSON, (int32_t)indexNodeJSON, currentView);
				const int32_t										evalResult							= prevResult ? 0 : 1;
				{
					::gpk::view_const_string							viewOfExpressionResult				= {};
					const uint32_t										expressionResultChildIndex			= (iFirstLevelExpression == nodeExpression->Children.size()-1) ? iFirstLevelExpression : iFirstLevelExpression + evalResult;
					const ::gpk::SExpressionNode						& childEval							= *nodeExpression->Children[expressionResultChildIndex];
					const ::gpk::view_const_string						& childEvalView						= readerExpression.View[childEval.ObjectIndex];(void)childEvalView;
					indexOfResolvedSubExpression					= ::evaluateExpression(readerExpression, childEval.ObjectIndex, inputJSON, lastResult.IndexRootJSONNode, viewOfExpressionResult);
					ree_if(-1 == indexOfResolvedSubExpression, "Failed to resolve subexpression: '%s'.", childEvalView.begin());
					output											= viewOfExpressionResult;
					lastResult.IndexJSONResult						= indexOfResolvedSubExpression;
				}
				iFirstLevelExpression							+= prevResult;
			}
			else if(::gpk::EXPRESSION_READER_TYPE_TERM_AND == childToSolve.Token->Type) {
				const int32_t										prevResult							= ::evaluateAndClearBoolCarry(lastResult, currentJSON, (int32_t)indexNodeJSON, currentView);
				int32_t												evalResult							= -1;
				{
					::gpk::view_const_string							viewOfExpressionResult				= {};
					const int32_t										indexOfResolvedSubExpression		= ::evaluateExpression(readerExpression, childToSolve.ObjectIndex, inputJSON, lastResult.IndexRootJSONNode, viewOfExpressionResult);
					ree_if(-1 == indexOfResolvedSubExpression, "Failed to resolve subexpression: '%s'.", readerExpression.View[childToSolve.ObjectIndex].begin());
					evalResult										= ::evaluateBoolResult((indexOfResolvedSubExpression >= 0) ? inputJSON[indexOfResolvedSubExpression] : 0, indexOfResolvedSubExpression, viewOfExpressionResult);
				}
				lastResult.SetBoolCarry(evalResult && prevResult, output);
			}
			else if(::gpk::EXPRESSION_READER_TYPE_TERM_OR == childToSolve.Token->Type) {
				const int32_t										prevResult							= ::evaluateAndClearBoolCarry(lastResult, currentJSON, (int32_t)indexNodeJSON, currentView);
				if(prevResult)
					lastResult.SetBoolCarry(1, output);
				else {
					int32_t												evalResult							= -1;
					{
						::gpk::view_const_string							viewOfExpressionResult				= {};
						const int32_t										indexOfResolvedSubExpression		= ::evaluateExpression(readerExpression, childToSolve.ObjectIndex, inputJSON, lastResult.IndexRootJSONNode, viewOfExpressionResult);
						ree_if(-1 == indexOfResolvedSubExpression, "Failed to resolve subexpression: '%s'.", readerExpression.View[childToSolve.ObjectIndex].begin());
						evalResult										= ::evaluateBoolResult((indexOfResolvedSubExpression >= 0) ? inputJSON[indexOfResolvedSubExpression] : 0, indexOfResolvedSubExpression, viewOfExpressionResult);
					}
					lastResult.SetBoolCarry((evalResult || prevResult) ? 1 : 0, output);
				}
			}
			else if(::gpk::EXPRESSION_READER_TYPE_TERM_EQUALS == childToSolve.Token->Type) {
				const int32_t										prevResult							= ::evaluateAndClearBoolCarry(lastResult, currentJSON, (int32_t)indexNodeJSON, currentView);
				if(prevResult)
					lastResult.SetBoolCarry(1, output);
				else {
					int32_t												evalResult							= -1;
					{
						::gpk::view_const_string							viewOfExpressionResult				= {};
						const int32_t										indexOfResolvedSubExpression		= ::evaluateExpression(readerExpression, childToSolve.ObjectIndex, inputJSON, lastResult.IndexRootJSONNode, viewOfExpressionResult);
						ree_if(-1 == indexOfResolvedSubExpression, "Failed to resolve subexpression: '%s'.", readerExpression.View[childToSolve.ObjectIndex].begin());
						evalResult										= ::evaluateBoolResult((indexOfResolvedSubExpression >= 0) ? inputJSON[indexOfResolvedSubExpression] : 0, indexOfResolvedSubExpression, viewOfExpressionResult);
					}
					lastResult.SetBoolCarry((evalResult || prevResult) ? 1 : 0, output);
				}
			}
			else {
				error_printf("Unrecognized expression reader type: '%s'.", ::gpk::get_value_label(childToSolve.Token->Type).begin());
				return -1;
			}
		}
		indexNodeJSON									= lastResult.IndexJSONResult;
	}
	return lastResult.IndexJSONResult;
}

::gpk::error_t									gpk::jsonExpressionResolve				(const ::gpk::SExpressionReader & reader, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::view_const_string& output)								{
	::gpk::array_pod<char_t>							bufferFormat							= {};
	const ::gpk::array_obj<::gpk::view_const_string>	& expressionViews						= reader.View;
	for(uint32_t iView = 0; iView < expressionViews.size(); ++iView) {
		const ::gpk::view_const_string						& viewExpression						= expressionViews[iView];
		const ::gpk::SExpressionToken						& typeExpression						= reader.Token[iView];
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
