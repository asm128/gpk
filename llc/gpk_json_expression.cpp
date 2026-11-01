#ifndef GPK_DISABLE_DEBUG_BREAK_ON_ERROR_LOG
#	define GPK_DISABLE_DEBUG_BREAK_ON_ERROR_LOG
#endif
#include "gpk_json_expression.h"

#include "gpk_expression.h"
#include "gpk_parse.h"

#define gpk_jexpr_info_printf(...) // info_printf


GPK_USING_APOD();
GPK_USING_VIEW();
GPK_USING_TYPEINT();

//#define GPK_JSON_EXPRESSION_DEBUG

#if defined(GPK_JSON_EXPRESSION_DEBUG)
static	gpk::err_t printNode		(const ::gpk::SExpressionNode* node, const ::gpk::vcst_t & expression)			{
	const ::gpk::apod<sc_t>					viewPrintable					= ::gpk::string({&expression[node->Token->Span.Begin], node->Token->Span.End - node->Token->Span.Begin});
	const ::gpk::vcst_t						viewTokenType					= ::gpk::get_value_label(node->Token->Type);
	info_printf("Entering expression node type: %u (%s). Node Span: {%u, %u}. Parent index: %u. Object index: %u. Text: %s", node->Token->Type, viewTokenType.begin(), node->Token->Span.Begin, node->Token->Span.End, node->Token->ParentIndex, node->ObjectIndex, viewPrintable.begin());
	for(::gpk::u2_t iChildren = 0; iChildren < node->Children.size(); ++iChildren)
		::printNode(node->Children[iChildren], expression);
	info_printf("Exiting expression node type: %u (%s). Node Span: {%u, %u}. Parent index: %u. Object index: %u. Text: %s", node->Token->Type, viewTokenType.begin(), node->Token->Span.Begin, node->Token->Span.End, node->Token->ParentIndex, node->ObjectIndex, viewPrintable.begin());
	return 0;
}
#endif

static	gpk::err_t evaluateJSONBool		(const ::gpk::SJSONNode & jsonToEvaluate, ::gpk::vcst_c & boolView)	{
	s2_t												evalResult								= -1;
	if(jsonToEvaluate.Token->Type == ::gpk::JSON_TYPE_NULL)
		evalResult										= 0;
	else if(jsonToEvaluate.Token->Type == ::gpk::JSON_TYPE_BOOLEAN)
		evalResult										= (boolView == ::gpk::strBool[1]) ? 1 : 0;
	else if(jsonToEvaluate.Token->Type == ::gpk::JSON_TYPE_INTEGER)
		evalResult										= jsonToEvaluate.Token->Value == 0 ? 0 : 1;
	else if(jsonToEvaluate.Token->Type == ::gpk::JSON_TYPE_DECIMAL) {
		::gpk::f3_t									value									= {};
		memcpy(&value, &jsonToEvaluate.Token->Value, sizeof(value));
		evalResult										= value == 0 ? 0 : 1;
	}
	else if(jsonToEvaluate.Token->Type == ::gpk::JSON_TYPE_STRING)
		evalResult										= boolView.size() ? 1 : 0;
	else if(jsonToEvaluate.Token->Type == ::gpk::JSON_TYPE_ARRAY)
		evalResult										= jsonToEvaluate.Children.size() ? 1 : 0;
	else if(jsonToEvaluate.Token->Type == ::gpk::JSON_TYPE_OBJECT)
		evalResult										= jsonToEvaluate.Children.size() ? 1 : 0;
	else
		error_printf("Failed to solve expression: %s", ::gpk::string(boolView).begin());
	return evalResult;
}

static	gpk::err_t evaluateBoolResult		(const ::gpk::SJSONNode * jsonToEvaluate, s2_t indexNodeJSON, const ::gpk::vcst_t & boolView)	{
	gpk::err_t 			evalResult								= -1;
	if(indexNodeJSON < 0)
		evalResult										= (0 == boolView.size() || boolView == ::gpk::strZero || boolView == ::gpk::strBool[0] ||  boolView == ::gpk::strNull || boolView == ::gpk::strEmptyObj || boolView == ::gpk::strEmptyArr) ? 0 : 1;
	else if(indexNodeJSON >= 0)
		evalResult										= ::evaluateJSONBool(*jsonToEvaluate, boolView);
	return evalResult;
}

static	gpk::err_t evaluateAndClearBoolCarry				(::gpk::SEvaluationStepResult & lastResult, const ::gpk::SJSONNode * jsonToEvaluate, s2_t indexNodeJSON, const ::gpk::vcst_t & boolView)	{
	if(false == lastResult.LastValueCarry)
		return ::evaluateBoolResult(jsonToEvaluate, (s2_t)indexNodeJSON, boolView);
	else {
		lastResult.LastValueCarry						= false;
		return lastResult.LastValue ? 1 : 0;
	}
}

static	gpk::err_t evaluateExpression						(::gpk::SJSONExpressionSolver& results, const ::gpk::SExpressionReader & readerExpression, const ::gpk::u2_t indexNodeExpression, const ::gpk::SJSONReader& inputJSON, ::gpk::u2_t indexNodeJSON, ::gpk::vcst_t & output);

static	gpk::err_t evaluateExpressionAndBoolResult			(::gpk::SJSONExpressionSolver& results, const ::gpk::SExpressionReader & readerExpression, ::gpk::u2_t indexExpressionToken, const ::gpk::SJSONReader & jsonInput, const s2_t indexJSONNode)	{
	::gpk::vcst_t								viewOfExpressionResult					= {};
	const s2_t										indexOfResolvedSubExpression			= ::evaluateExpression(results, readerExpression, indexExpressionToken, jsonInput, indexJSONNode, viewOfExpressionResult);
	if_true_fef(-1 == indexOfResolvedSubExpression, "Failed to resolve subexpression: '%s'.", ::gpk::string(readerExpression.View[indexExpressionToken]).begin());
	return ::evaluateBoolResult((indexOfResolvedSubExpression >= 0) ? (const ::gpk::SJSONNode*)jsonInput[indexOfResolvedSubExpression] : 0, indexOfResolvedSubExpression, viewOfExpressionResult);
}


// Returns an index pointing to the resulting JSON element, or a -0xF - the index of the resulting expression element.
static	gpk::err_t evaluateExpression						(::gpk::SJSONExpressionSolver& results, const ::gpk::SExpressionReader & readerExpression, const ::gpk::u2_t indexNodeExpression, const ::gpk::SJSONReader& inputJSON, ::gpk::u2_t indexNodeJSON, ::gpk::vcst_t & output)	{
	if_true_fef(indexNodeExpression >= readerExpression.Tree.size() || 0 == readerExpression.Tree[indexNodeExpression], "Invalid expression node: %u.", indexNodeExpression);
	if_true_fef(indexNodeJSON >= inputJSON.Tree.size() || 0 == inputJSON.Tree[indexNodeJSON], "Invalid input json or index node: %u.", indexNodeJSON);
	const ::gpk::SExpressionNode	* nodeExpression					= readerExpression.Tree[indexNodeExpression];	// Take this
	gpk_jexpr_info_printf("Processing expression: '%s'", ::gpk::string(readerExpression.View[indexNodeExpression]).begin());
	::gpk::SEvaluationStepResult	lastResult							= {indexNodeJSON,};
	for(::gpk::u2_t iFirstLevelExpression = 0; iFirstLevelExpression < nodeExpression->Children.size(); ++iFirstLevelExpression) {
		const ::gpk::SExpressionNode		& childToSolve					= *nodeExpression->Children[iFirstLevelExpression];
		const ::gpk::SJSONNode				* currentJSON					= (0 <= (s2_t)indexNodeJSON) ? (const ::gpk::SJSONNode*)inputJSON.Tree[indexNodeJSON] : 0;
		//const ::gpk::SExpressionNode		* currentLiteral				= (0 <= (s2_t)indexNodeJSON) ? 0 : readerExpression.Tree[(((s2_t)indexNodeJSON) + 0xF) * -1];
		const ::gpk::vcst_t					currentView						= lastResult.Output;
		const ::gpk::vcst_t					currentExpressionView			= readerExpression.View[childToSolve.ObjectIndex];
		const ::gpk::JSON_TYPE				currentType						= currentJSON ? currentJSON->Token->Type : ::gpk::JSON_TYPE_UNKNOWN;
		const ::gpk::EXPRESSION_READER_TYPE	expressionType					= childToSolve.Token->Type;
		lastResult.Expression			= currentExpressionView;
		gpk_jexpr_info_printf("Reading first-level expression: %u.", iFirstLevelExpression);
		gpk_jexpr_info_printf("Current json view  (%u): '%s'", ::gpk::string(currentView).begin());
		gpk_jexpr_info_printf("Current child view (%u): '%s'", ::gpk::string(currentExpressionView).begin());
		if(::gpk::EXPRESSION_READER_TYPE_LITERAL == expressionType) {
			lastResult.Output			= output = currentExpressionView;
			lastResult.IndexJSONResult	= -(childToSolve.ObjectIndex + 0xF);
		}
		else {
			if(::gpk::EXPRESSION_READER_TYPE_KEY == expressionType) {
				::gpk::vcst_t				strKey							= currentExpressionView;
				if_true_fef(currentJSON && currentType != ::gpk::JSON_TYPE_OBJECT, "Only objects can be accessed by key. JSON type: %s. Key to solve: %s.", ::gpk::get_value_label(currentType).begin(), ::gpk::string(currentExpressionView).begin());
				if(1 == childToSolve.Children.size()
					&& (::gpk::EXPRESSION_READER_TYPE_TERM_KEY == childToSolve.Children[0]->Token->Type || ::gpk::EXPRESSION_READER_TYPE_TERM_ANY == childToSolve.Children[0]->Token->Type)) {
					cnst ::gpk::err_t indexOfResolvedSubExpression = ::evaluateExpression(results, readerExpression, childToSolve.ObjectIndex, inputJSON, (0 >= lastResult.IndexJSONResult) ? lastResult.IndexJSONResult : lastResult.IndexRootJSONNode, strKey);
					(void)indexOfResolvedSubExpression;
				}
				if_fail_fef(lastResult.IndexJSONResult = ::gpk::jsonObjectValueGet(*currentJSON, inputJSON.View, {strKey.begin(), strKey.size()}), "Key not found: %s.", ::gpk::string(strKey).begin());
				lastResult.Output	= output = inputJSON.View[lastResult.IndexJSONResult];
			}
			else {
				::gpk::vcst_t				viewOfExpressionResult			= {};
				if(::gpk::EXPRESSION_READER_TYPE_TERM_INDEX == expressionType) {
					if_true_fef(currentJSON && currentType != ::gpk::JSON_TYPE_ARRAY && currentType != ::gpk::JSON_TYPE_STRING, "Only arrays can be accessed by index. JSON type: %s.", ::gpk::get_value_label(currentType).begin());
					const s2_t			indexOfResolvedSubExpression	= ::evaluateExpression(results, readerExpression, childToSolve.ObjectIndex, inputJSON, lastResult.IndexRootJSONNode, viewOfExpressionResult);
					if_true_fef(-1 == indexOfResolvedSubExpression, "Failed to solve expression: %s.", ::gpk::string(currentExpressionView).begin());
					if_true_fef(indexOfResolvedSubExpression >= 0 && inputJSON.Token[indexOfResolvedSubExpression].Type != ::gpk::JSON_TYPE_INTEGER, "Arrays can only be accessed by index with a json integer. Type found: %s.",::gpk::get_value_label(inputJSON.Token[indexOfResolvedSubExpression].Type).begin());
					u3_t				numberRead						= 0;
					if_fail_fe(gpk::parseIntegerDecimal(viewOfExpressionResult, numberRead));
					if(currentType == ::gpk::JSON_TYPE_STRING) {
						if_true_fef(numberRead >= currentView.size(), "String index out of range: %" GPK_FMT_U3 ". String size: %u.", numberRead, currentView.size());
						lastResult.Output	= output = {&inputJSON.View[lastResult.IndexJSONResult][(::gpk::u2_t)numberRead], 1};
					}
					else {
						if_fail_fef(lastResult.IndexJSONResult = ::gpk::jsonArrayValueGet(*currentJSON, (::gpk::u2_t)numberRead), "Value not found for index: %lli.", numberRead);
						lastResult.Output	= output = inputJSON.View[lastResult.IndexJSONResult];
					}
				}
				else if(::gpk::EXPRESSION_READER_TYPE_TERM_KEY == expressionType) {
					if_true_fef(currentJSON && currentType != ::gpk::JSON_TYPE_OBJECT, "Only objects can be accessed by key. JSON type: %s.", ::gpk::get_value_label(currentType).begin());
					s2_t						indexOfResolvedSubExpression			= ::evaluateExpression(results, readerExpression, childToSolve.ObjectIndex, inputJSON, lastResult.IndexRootJSONNode, viewOfExpressionResult);
					if_true_fef(-1 == (lastResult.IndexJSONResult = indexOfResolvedSubExpression), "Failed to solve expression: %s.", ::gpk::string(currentExpressionView).begin());
					::gpk::SExpressionReader	reader;
					if_fail_fef(gpk::expressionReaderParse(reader, viewOfExpressionResult), "Failed to read JSONeN expression: '%s'.", ::gpk::string(viewOfExpressionResult).begin());
					indexOfResolvedSubExpression	= ::gpk::jsonExpressionResolve(reader, inputJSON, indexNodeJSON, results, output);
					lastResult.Output		= output;
					if_true_fef(-1 == (lastResult.IndexJSONResult = indexOfResolvedSubExpression), "Failed to evaluate child expression: %.*s.", GPK_VIEW_ARGS(viewOfExpressionResult));
				}
				else if(::gpk::EXPRESSION_READER_TYPE_UNARY_NOT == expressionType) {
					const s2_t										evalResult							= ::evaluateExpressionAndBoolResult(results, readerExpression, childToSolve.ObjectIndex, inputJSON, lastResult.IndexJSONResult);
					lastResult.SetBoolCarry(0 == evalResult, output);
					lastResult.Output								= output;
				}
				else if(::gpk::EXPRESSION_READER_TYPE_TERM_AND == expressionType) {
					const s2_t										prevResult							= ::evaluateAndClearBoolCarry(lastResult, currentJSON, (s2_t)indexNodeJSON, currentView);
					s2_t												evalResult							= ::evaluateExpressionAndBoolResult(results, readerExpression, childToSolve.ObjectIndex, inputJSON, lastResult.IndexRootJSONNode);
					lastResult.SetBoolCarry(evalResult && prevResult, output);
					lastResult.Output								= output;
				}
				else if(::gpk::EXPRESSION_READER_TYPE_TERM_OR == expressionType) {
					const s2_t										prevResult							= ::evaluateAndClearBoolCarry(lastResult, currentJSON, (s2_t)indexNodeJSON, currentView);
					const s2_t										evalResult							= ::evaluateExpressionAndBoolResult(results, readerExpression, childToSolve.ObjectIndex, inputJSON, lastResult.IndexRootJSONNode);
					lastResult.SetBoolCarry(evalResult || prevResult, output);
					lastResult.Output								= output;
				}
				else if(::gpk::EXPRESSION_READER_TYPE_TERM_ANY == expressionType) {
					const s2_t										indexOfResolvedSubExpression		= ::evaluateExpression(results, readerExpression, childToSolve.ObjectIndex, inputJSON, lastResult.IndexJSONResult, viewOfExpressionResult);
					lastResult.Output								= output = viewOfExpressionResult;
					if_true_fef(-1 == indexOfResolvedSubExpression, "Failed to resolve expression: %s.", ::gpk::string(currentExpressionView).begin());
					lastResult.IndexJSONResult									= indexOfResolvedSubExpression;
				}
				else {
					if(::gpk::EXPRESSION_READER_TYPE_TERM_BOOL == expressionType) {
						const s2_t										prevResult							= ::evaluateAndClearBoolCarry(lastResult, currentJSON, (s2_t)indexNodeJSON, currentView);
						const s2_t										evalResult							= prevResult ? 0 : 1;
						{
							const ::gpk::u2_t										expressionResultChildIndex			= (iFirstLevelExpression == nodeExpression->Children.size()-1) ? iFirstLevelExpression : iFirstLevelExpression + evalResult;
							const ::gpk::SExpressionNode						& childEval							= *nodeExpression->Children[expressionResultChildIndex];
							const ::gpk::vcst_t						& childEvalView						= readerExpression.View[childEval.ObjectIndex];(void)childEvalView;
							const s2_t										indexOfResolvedSubExpression		= ::evaluateExpression(results, readerExpression, childEval.ObjectIndex, inputJSON, lastResult.IndexRootJSONNode, viewOfExpressionResult);
							if_true_fef(-1 == indexOfResolvedSubExpression, "Failed to resolve subexpression: '%s'.", ::gpk::string(childEvalView).begin());
							lastResult.Output								= output = viewOfExpressionResult;
							lastResult.IndexJSONResult						= indexOfResolvedSubExpression;
						}
						iFirstLevelExpression							+= prevResult;
					}
					else if(::gpk::EXPRESSION_READER_TYPE_TERM_EQUALS == expressionType) {
						const s2_t										indexOfResolvedSubExpression		= ::evaluateExpression(results, readerExpression, childToSolve.ObjectIndex, inputJSON, lastResult.IndexRootJSONNode, viewOfExpressionResult);
						if_true_fef(-1 == indexOfResolvedSubExpression, "Failed to resolve subexpression: '%s'.", ::gpk::string(currentExpressionView).begin());
						const ::gpk::JSON_TYPE								resultType							= (indexOfResolvedSubExpression >= 0) ? inputJSON.Token[indexOfResolvedSubExpression].Type : ::gpk::JSON_TYPE_UNKNOWN;
						const ::gpk::SJSONNode								* resultJSON						= (indexOfResolvedSubExpression >= 0) ? (const ::gpk::SJSONNode*)inputJSON[indexOfResolvedSubExpression] : nullptr;
						if( viewOfExpressionResult == lastResult.Output
						 || (0 == lastResult.Output			.size() && ::gpk::strNull		== viewOfExpressionResult	)
						 || (0 == lastResult.Output			.size() && ::gpk::strBool[0]	== viewOfExpressionResult	)
						 || (0 == viewOfExpressionResult	.size() && ::gpk::strNull		== lastResult.Output		)
						 || (0 == viewOfExpressionResult	.size() && ::gpk::strBool[0]	== lastResult.Output		)
						 || (::gpk::strBool[0]	== viewOfExpressionResult	&& ::gpk::strNull		== lastResult.Output)
						 || (::gpk::strNull		== viewOfExpressionResult	&& ::gpk::strBool[0]	== lastResult.Output)
						 ) {
							lastResult.SetBoolCarry(true, output);
							lastResult.Output								= output;
						} else {
							if(currentType == ::gpk::JSON_TYPE_STRING || resultType == ::gpk::JSON_TYPE_STRING) {
								lastResult.SetBoolCarry(false, output);
								lastResult.Output								= output;
							}
							else if((currentType == ::gpk::JSON_TYPE_INTEGER	&& resultType == ::gpk::JSON_TYPE_INTEGER	)
								 || (currentType == ::gpk::JSON_TYPE_DECIMAL		&& resultType == ::gpk::JSON_TYPE_DECIMAL	)
							) {
								f3_t												valueLeft				= 0;
								f3_t												valueRight				= 0;
								if(currentType == ::gpk::JSON_TYPE_INTEGER) {
									valueLeft										= (f3_t)currentJSON->Token->Value;
									valueRight										= (f3_t)resultJSON->Token->Value;
								}
								else {
									memcpy(&valueLeft , &currentJSON->Token->Value, sizeof(u3_t));
									memcpy(&valueRight, &resultJSON ->Token->Value, sizeof(u3_t));
								}
								lastResult.SetBoolCarry(valueLeft == valueRight, output);
								lastResult.Output								= output;
							}
							else if(currentType == ::gpk::JSON_TYPE_INTEGER	&& resultType == ::gpk::JSON_TYPE_DECIMAL) {
								const f3_t 										valueLeft				= (f3_t)currentJSON->Token->Value;
								f3_t 												valueRight				= 0;
								memcpy(&valueRight, &resultJSON->Token->Value, sizeof(u3_t));
								lastResult.SetBoolCarry(valueLeft == valueRight, output);
								lastResult.Output								= output;
							}
							else if(currentType == ::gpk::JSON_TYPE_DECIMAL && resultType == ::gpk::JSON_TYPE_INTEGER) {
								const f3_t 										valueRight				= (f3_t)resultJSON->Token->Value;
								f3_t 												valueLeft				= 0;
								memcpy(&valueLeft, &currentJSON->Token->Value, sizeof(u3_t));
								lastResult.SetBoolCarry(valueLeft == valueRight, output);
								lastResult.Output								= output;
							}
							else if(::gpk::JSON_TYPE_BOOLEAN == resultType	|| ::gpk::JSON_TYPE_NULL == resultType
								 || ::gpk::JSON_TYPE_BOOLEAN == currentType	|| ::gpk::JSON_TYPE_NULL == currentType	) {
								const s2_t										prevResult							= ::evaluateAndClearBoolCarry	(lastResult, currentJSON, (s2_t)indexNodeJSON, currentView);
								const s2_t										evalResult							= ::evaluateBoolResult			(resultJSON, indexOfResolvedSubExpression, viewOfExpressionResult);;
								lastResult.SetBoolCarry(evalResult == prevResult, output);
								lastResult.Output								= output;
							}
							else if(currentJSON) {
								if((indexOfResolvedSubExpression >= 0) && currentType != resultType) {
									lastResult.SetBoolCarry(false, output);
									lastResult.Output								= output;
								}
								else if(currentType == ::gpk::JSON_TYPE_ARRAY	) { if(indexOfResolvedSubExpression < 0) { lastResult.SetBoolCarry(lastResult.Output == viewOfExpressionResult, output); lastResult.Output = output; } else { lastResult.SetBoolCarry(::gpk::jsonCompareArray	(*currentJSON, *resultJSON, inputJSON.View), output); lastResult.Output = output; }  }
								else if(currentType == ::gpk::JSON_TYPE_OBJECT	) { if(indexOfResolvedSubExpression < 0) { lastResult.SetBoolCarry(lastResult.Output == viewOfExpressionResult, output); lastResult.Output = output; } else { lastResult.SetBoolCarry(::gpk::jsonCompareObject	(*currentJSON, *resultJSON, inputJSON.View), output); lastResult.Output = output; }  }
							}
							else {
								if(indexOfResolvedSubExpression < 0) {
									lastResult.SetBoolCarry(lastResult.Output == viewOfExpressionResult, output);
									lastResult.Output								= output;
								}
								else if(resultType == ::gpk::JSON_TYPE_ARRAY	) { lastResult.SetBoolCarry(lastResult.Output == viewOfExpressionResult, output); lastResult.Output = output; }
								else if(resultType == ::gpk::JSON_TYPE_OBJECT	) { lastResult.SetBoolCarry(lastResult.Output == viewOfExpressionResult, output); lastResult.Output = output; }
							}
						}
					}
					else {
						error_printf("Unrecognized expression reader type: '%s'.", ::gpk::get_value_label(childToSolve.Token->Type).begin());
						return -1;
					}
				}
			}
		}
		if_fail_fe(results.Results.push_back(lastResult));
		indexNodeJSON									= lastResult.IndexJSONResult;
	}
	return lastResult.IndexJSONResult;
}

gpk::err_t 		gpk::jsonExpressionResolve				(const ::gpk::SExpressionReader & reader, const ::gpk::SJSONReader & inputJSON, ::gpk::u2_t indexNodeJSON, ::gpk::SJSONExpressionSolver& results, ::gpk::vcst_t & output) {
	const ::gpk::aobj<::gpk::vcst_t>	& expressionViews						= reader.View;
	for(::gpk::u2_t iView = 0; iView < expressionViews.size(); ++iView) {
		const ::gpk::vcst_t				& viewExpression						= expressionViews[iView];
		const ::gpk::SExpressionToken	& typeExpression						= reader.Token[iView];
		(void)typeExpression;
		if(viewExpression.size()) {
			gpk_jexpr_info_printf("Expression element: %s. Type: %s. Parent: %i. Begin: %u. End: %u.", ::gpk::string(viewExpression).begin(), ::gpk::get_value_namep(typeExpression.Type), typeExpression.ParentIndex, typeExpression.Span.Begin, typeExpression.Span.End);
		}
	}
	::gpk::vcst_t					evaluated								= {};
	::gpk::SJSONExpressionSolver	pendingResults							= {};
	s2_t						jsonNodeResultOfEvaluation				= -1;
	if(reader.Tree.size()) {
#if defined(GPK_JSON_EXPRESSION_DEBUG)
		::printNode(reader.Tree[0], reader.View[0]);
#endif
		jsonNodeResultOfEvaluation	= ::evaluateExpression(pendingResults, reader, 0, inputJSON, indexNodeJSON, evaluated);
		if_true_fef(-1 == jsonNodeResultOfEvaluation, "Failed to evaluate expression: %s.", ::gpk::string(reader.View[0]).begin());
	}
	if(evaluated.size()) {
		gpk_jexpr_info_printf("Result of expression evaluation: %s.", ::gpk::string(evaluated).begin());
	}
	if_fail_fe(results.Results.reserve(results.Results.size() + pendingResults.Results.size()));
	for(::gpk::u2_t iResult = 0; iResult < pendingResults.Results.size(); ++iResult)
		if_fail_fe(results.Results.push_back(pendingResults.Results[iResult]));
	output												= evaluated;
	return jsonNodeResultOfEvaluation;
}

gpk::err_t 		gpk::jsonExpressionResolve				(::gpk::vcst_t expression, const ::gpk::SJSONReader & inputJSON, ::gpk::u2_t indexNodeJSON, ::gpk::vcst_t & output)								{
	::gpk::SExpressionReader	reader;
	if_fail_fef(gpk::expressionReaderParse(reader, expression), "Failed to read JSONeN expression: '%s'.", ::gpk::string(expression).begin());
	return ::gpk::jsonExpressionResolve(reader, inputJSON, indexNodeJSON, output);
}

gpk::err_t 		gpk::jsonExpressionResolve				(const ::gpk::SExpressionReader & reader, const ::gpk::SJSONReader & inputJSON, ::gpk::u2_t indexNodeJSON, ::gpk::vcst_t & output){
	gpk::SJSONExpressionSolver	results;
	gpk::err_t 			jsonNodeResultOfEvaluation;
	jsonNodeResultOfEvaluation = ::gpk::jsonExpressionResolve(reader, inputJSON, indexNodeJSON, results, output);
	if_true_fef(-1 == jsonNodeResultOfEvaluation, "%s", "Failed to resolve JSON expression.");
	for(::gpk::u2_t iResult = 0; iResult < results.Results.size(); ++iResult)
		gpk_jexpr_info_printf("Result %u: '%s' | '%s'", iResult, ::gpk::string(results.Results[iResult].Expression).begin(), ::gpk::string(results.Results[iResult].Output).begin());
	return jsonNodeResultOfEvaluation;
}

gpk::err_t 		gpk::jsonStringFormat					(::gpk::vcst_t format, const ::gpk::SJSONReader& inputJSON, ::gpk::u2_t indexNodeJSON, ::gpk::asc_t & output)					{
	rtrn ::gpk::expressionStringFormat(format, [&](::gpk::vcst_c & expression, ::gpk::vcst_t & resolved) {
		rtrn ::gpk::jsonExpressionResolve(expression, inputJSON, indexNodeJSON, resolved);
	}, output);
}
