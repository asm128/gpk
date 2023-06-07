#define GPK_DISABLE_DEBUG_BREAK_ON_ERROR_LOG
#include "gpk_json_expression.h"

#include "gpk_expression.h"
#include "gpk_parse.h"

#define gpk_jexpr_info_printf(...) // info_printf

//#define GPK_JSON_EXPRESSION_DEBUG

#if defined(GPK_JSON_EXPRESSION_DEBUG)
static	::gpk::error_t	printNode		(const ::gpk::SExpressionNode* node, const ::gpk::vcc & expression)			{
	const ::gpk::apod<char>						viewPrintable					= ::gpk::toString({&expression[node->Token->Span.Begin], node->Token->Span.End - node->Token->Span.Begin});
	const ::gpk::vcc						viewTokenType					= ::gpk::get_value_label(node->Token->Type);
	info_printf("Entering expression node type: %u (%s). Node Span: {%u, %u}. Parent index: %u. Object index: %u. Text: %s", node->Token->Type, viewTokenType.begin(), node->Token->Span.Begin, node->Token->Span.End, node->Token->ParentIndex, node->ObjectIndex, viewPrintable.begin());
	for(uint32_t iChildren = 0; iChildren < node->Children.size(); ++iChildren)
		::printNode(node->Children[iChildren], expression);
	info_printf("Exiting expression node type: %u (%s). Node Span: {%u, %u}. Parent index: %u. Object index: %u. Text: %s", node->Token->Type, viewTokenType.begin(), node->Token->Span.Begin, node->Token->Span.End, node->Token->ParentIndex, node->ObjectIndex, viewPrintable.begin());
	return 0;
}
#endif

static	::gpk::error_t	evaluateJSONBool		(const ::gpk::SJSONNode & jsonToEvaluate, const ::gpk::vcc & boolView)	{
	int32_t												evalResult								= -1;
	if(jsonToEvaluate.Token->Type == ::gpk::JSON_TYPE_NULL)
		evalResult										= 0;
	else if(jsonToEvaluate.Token->Type == ::gpk::JSON_TYPE_BOOL)
		evalResult										= (boolView == ::gpk::strBool[1]) ? 1 : 0;
	else if(jsonToEvaluate.Token->Type == ::gpk::JSON_TYPE_INTEGER)
		evalResult										= jsonToEvaluate.Token->Value == 0 ? 0 : 1;
	else if(jsonToEvaluate.Token->Type == ::gpk::JSON_TYPE_DOUBLE)
		evalResult										= jsonToEvaluate.Token->Value == 0 ? 0 : 1;
	else if(jsonToEvaluate.Token->Type == ::gpk::JSON_TYPE_STRING)
		evalResult										= boolView.size() ? 1 : 0;
	else if(jsonToEvaluate.Token->Type == ::gpk::JSON_TYPE_ARRAY)
		evalResult										= jsonToEvaluate.Children.size() ? 1 : 0;
	else if(jsonToEvaluate.Token->Type == ::gpk::JSON_TYPE_OBJECT)
		evalResult										= jsonToEvaluate.Children.size() ? 1 : 0;
	else
		error_printf("Failed to solve expression: %s", ::gpk::toString(boolView).begin());
	return evalResult;
}

static	::gpk::error_t	evaluateBoolResult		(const ::gpk::SJSONNode * jsonToEvaluate, int32_t indexNodeJSON, const ::gpk::vcc & boolView)	{
	::gpk::error_t										evalResult								= -1;
	if(indexNodeJSON < 0)
		evalResult										= (0 == boolView.size() || boolView == ::gpk::strZero || boolView == ::gpk::strBool[0] ||  boolView == ::gpk::strNull || boolView == ::gpk::strEmptyObj || boolView == ::gpk::strEmptyArr) ? 0 : 1;
	else if(indexNodeJSON >= 0)
		evalResult										= ::evaluateJSONBool(*jsonToEvaluate, boolView);
	return evalResult;
}

static	::gpk::error_t	evaluateAndClearBoolCarry				(::gpk::SEvaluationStepResult & lastResult, const ::gpk::SJSONNode * jsonToEvaluate, int32_t indexNodeJSON, const ::gpk::vcc & boolView)	{
	if(false == lastResult.LastValueCarry)
		return ::evaluateBoolResult(jsonToEvaluate, (int32_t)indexNodeJSON, boolView);
	else {
		lastResult.LastValueCarry						= false;
		return lastResult.LastValue ? 1 : 0;
	}
}

static	::gpk::error_t	evaluateExpression						(::gpk::SJSONExpressionSolver& results, const ::gpk::SExpressionReader & readerExpression, const uint32_t indexNodeExpression, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::vcc & output);

static	::gpk::error_t	evaluateExpressionAndBoolResult			(::gpk::SJSONExpressionSolver& results, const ::gpk::SExpressionReader & readerExpression, uint32_t indexExpressionToken, const ::gpk::SJSONReader & jsonInput, const int32_t indexJSONNode)	{
	::gpk::vcc								viewOfExpressionResult					= {};
	const int32_t										indexOfResolvedSubExpression			= ::evaluateExpression(results, readerExpression, indexExpressionToken, jsonInput, indexJSONNode, viewOfExpressionResult);
	ree_if(-1 == indexOfResolvedSubExpression, "Failed to resolve subexpression: '%s'.", ::gpk::toString(readerExpression.View[indexExpressionToken]).begin());
	return ::evaluateBoolResult((indexOfResolvedSubExpression >= 0) ? (const ::gpk::SJSONNode*)jsonInput[indexOfResolvedSubExpression] : 0, indexOfResolvedSubExpression, viewOfExpressionResult);
}


// Returns an index pointing to the resulting JSON element, or a -0xF - the index of the resulting expression element.
static	::gpk::error_t	evaluateExpression						(::gpk::SJSONExpressionSolver& results, const ::gpk::SExpressionReader & readerExpression, const uint32_t indexNodeExpression, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::vcc & output)	{
	ree_if(indexNodeJSON >= inputJSON.Tree.size(), "Invalid input json or index node: %i", indexNodeJSON);
	const ::gpk::SExpressionNode						* nodeExpression						= readerExpression.Tree[indexNodeExpression];	// Take this
	gpk_jexpr_info_printf("Processing expression: '%s'", ::gpk::toString(readerExpression.View[indexNodeExpression]).begin());
	::gpk::SEvaluationStepResult						lastResult								= {indexNodeJSON,};
	for(uint32_t iFirstLevelExpression = 0; iFirstLevelExpression < nodeExpression->Children.size(); ++iFirstLevelExpression) {
		const ::gpk::SExpressionNode						& childToSolve							= *nodeExpression->Children[iFirstLevelExpression];
		const ::gpk::SJSONNode								* currentJSON							= (0 <= (int32_t)indexNodeJSON) ? (const ::gpk::SJSONNode*)inputJSON.Tree[indexNodeJSON] : 0;
		//const ::gpk::SExpressionNode						* currentLiteral						= (0 <= (int32_t)indexNodeJSON) ? 0 : readerExpression.Tree[(((int32_t)indexNodeJSON) + 0xF) * -1];
		const ::gpk::vcc						currentView								= lastResult.Output;
		const ::gpk::vcc						currentExpressionView					= readerExpression.View[childToSolve.ObjectIndex];
		const ::gpk::JSON_TYPE								currentType								= currentJSON ? currentJSON->Token->Type : ::gpk::JSON_TYPE_UNKNOWN;
		const ::gpk::EXPRESSION_READER_TYPE					expressionType							= childToSolve.Token->Type;
		lastResult.Expression							= currentExpressionView;
		gpk_jexpr_info_printf("Reading first-level expression: %u.", iFirstLevelExpression);
		gpk_jexpr_info_printf("Current json view  (%u): '%s'", ::gpk::toString(currentView).begin());
		gpk_jexpr_info_printf("Current child view (%u): '%s'", ::gpk::toString(currentExpressionView).begin());
		if(::gpk::EXPRESSION_READER_TYPE_LITERAL == expressionType) {
			lastResult.Output								= output = currentExpressionView;
			lastResult.IndexJSONResult						= -(childToSolve.ObjectIndex + 0xF);
		}
		else {
			if(::gpk::EXPRESSION_READER_TYPE_KEY == expressionType) {
				::gpk::vcc								strKey										= currentExpressionView;
				ree_if(currentJSON && currentType != ::gpk::JSON_TYPE_OBJECT, "Only objects can be accessed by key. JSON type: %s. Key to solve: %s.", ::gpk::get_value_label(currentType).begin(), ::gpk::toString(currentExpressionView).begin());
				if(1 == childToSolve.Children.size()
					&& (::gpk::EXPRESSION_READER_TYPE_TERM_KEY == childToSolve.Children[0]->Token->Type || ::gpk::EXPRESSION_READER_TYPE_TERM_ANY == childToSolve.Children[0]->Token->Type)) {
					const int32_t										indexOfResolvedSubExpression			= ::evaluateExpression(results, readerExpression, childToSolve.ObjectIndex, inputJSON, (0 >= lastResult.IndexJSONResult)? lastResult.IndexJSONResult : lastResult.IndexRootJSONNode, strKey);
					(void)indexOfResolvedSubExpression;
				}
				lastResult.IndexJSONResult						= ::gpk::jsonObjectValueGet(*inputJSON.Tree[indexNodeJSON], inputJSON.View, {strKey.begin(), strKey.size()});
				ree_if(errored(lastResult.IndexJSONResult), "Key not found: %s.", ::gpk::toString(strKey).begin());
				lastResult.Output								= output = inputJSON.View[lastResult.IndexJSONResult];
			}
			else {
				::gpk::vcc								viewOfExpressionResult				= {};
				if(::gpk::EXPRESSION_READER_TYPE_TERM_INDEX == expressionType) {
					ree_if(currentJSON && currentType != ::gpk::JSON_TYPE_ARRAY && currentType != ::gpk::JSON_TYPE_STRING, "Only arrays can be accessed by index. JSON type: %s.", ::gpk::get_value_label(currentType).begin());
					const int32_t										indexOfResolvedSubExpression			= ::evaluateExpression(results, readerExpression, childToSolve.ObjectIndex, inputJSON, lastResult.IndexRootJSONNode, viewOfExpressionResult);
					ree_if(-1 == indexOfResolvedSubExpression, "Failed to solve expression: %s.", ::gpk::toString(currentExpressionView).begin());
					ree_if(indexOfResolvedSubExpression >= 0 && inputJSON.Token[indexOfResolvedSubExpression].Type != ::gpk::JSON_TYPE_INTEGER, "Arrays can only be accessed by index with a json integer. Type found: %s.",::gpk::get_value_label(inputJSON.Token[indexOfResolvedSubExpression].Type).begin());
					uint64_t											numberRead								= 0;
					gpk_necall(::gpk::parseIntegerDecimal(viewOfExpressionResult, &numberRead), "%s", "Out of memory?");
					if(currentType == ::gpk::JSON_TYPE_STRING)
						lastResult.Output								= output = {&inputJSON.View[lastResult.IndexJSONResult][(uint32_t)numberRead], 1};
					else {
						lastResult.IndexJSONResult						= ::gpk::jsonArrayValueGet(*currentJSON, (uint32_t)numberRead);
						ree_if(errored(lastResult.IndexJSONResult), "Value not found for index: %lli.", numberRead);
						lastResult.Output								= output = inputJSON.View[lastResult.IndexJSONResult];
					}
				}
				else if(::gpk::EXPRESSION_READER_TYPE_TERM_KEY == expressionType) {
					ree_if(currentJSON && currentType != ::gpk::JSON_TYPE_OBJECT, "Only objects can be accessed by key. JSON type: %s.", ::gpk::get_value_label(currentType).begin());
					int32_t												indexOfResolvedSubExpression			= ::evaluateExpression(results, readerExpression, childToSolve.ObjectIndex, inputJSON, (0 >= lastResult.IndexJSONResult)? lastResult.IndexJSONResult : lastResult.IndexRootJSONNode, viewOfExpressionResult);
					ree_if(-1 == (lastResult.IndexJSONResult = indexOfResolvedSubExpression), "Failed to solve expression: %s.", ::gpk::toString(currentExpressionView).begin());
					::gpk::SExpressionReader							reader;
					gpk_necall(::gpk::expressionReaderParse(reader, viewOfExpressionResult), "Failed to read JSONeN expression: '%s'.", ::gpk::toString(viewOfExpressionResult).begin());
					indexOfResolvedSubExpression					= ::gpk::jsonExpressionResolve(reader, inputJSON, indexNodeJSON, results, output);
					lastResult.Output								= output;
					ree_if(-1 == (lastResult.IndexJSONResult = indexOfResolvedSubExpression), "Failed to evaluate child expression: %s.", ::gpk::toString(viewOfExpressionResult).size());
				}
				else if(::gpk::EXPRESSION_READER_TYPE_UNARY_NOT == expressionType) {
					const int32_t										evalResult							= ::evaluateExpressionAndBoolResult(results, readerExpression, childToSolve.ObjectIndex, inputJSON, lastResult.IndexJSONResult);
					lastResult.SetBoolCarry(0 == evalResult, output);
					lastResult.Output								= output;
				}
				else if(::gpk::EXPRESSION_READER_TYPE_TERM_AND == expressionType) {
					const int32_t										prevResult							= ::evaluateAndClearBoolCarry(lastResult, currentJSON, (int32_t)indexNodeJSON, currentView);
					int32_t												evalResult							= ::evaluateExpressionAndBoolResult(results, readerExpression, childToSolve.ObjectIndex, inputJSON, lastResult.IndexRootJSONNode);
					lastResult.SetBoolCarry(evalResult && prevResult, output);
					lastResult.Output								= output;
				}
				else if(::gpk::EXPRESSION_READER_TYPE_TERM_OR == expressionType) {
					const int32_t										prevResult							= ::evaluateAndClearBoolCarry(lastResult, currentJSON, (int32_t)indexNodeJSON, currentView);
					const int32_t										evalResult							= ::evaluateExpressionAndBoolResult(results, readerExpression, childToSolve.ObjectIndex, inputJSON, lastResult.IndexRootJSONNode);
					lastResult.SetBoolCarry(evalResult || prevResult, output);
					lastResult.Output								= output;
				}
				else if(::gpk::EXPRESSION_READER_TYPE_TERM_ANY == expressionType) {
					const int32_t										indexOfResolvedSubExpression		= ::evaluateExpression(results, readerExpression, childToSolve.ObjectIndex, inputJSON, lastResult.IndexJSONResult, viewOfExpressionResult);
					lastResult.Output								= output = viewOfExpressionResult;
					ree_if(-1 == indexOfResolvedSubExpression, "Failed to resolve expression: %s.", ::gpk::toString(currentExpressionView).begin());
					lastResult.IndexJSONResult									= indexOfResolvedSubExpression;
				}
				else {
					if(::gpk::EXPRESSION_READER_TYPE_TERM_BOOL == expressionType) {
						const int32_t										prevResult							= ::evaluateAndClearBoolCarry(lastResult, currentJSON, (int32_t)indexNodeJSON, currentView);
						const int32_t										evalResult							= prevResult ? 0 : 1;
						{
							const uint32_t										expressionResultChildIndex			= (iFirstLevelExpression == nodeExpression->Children.size()-1) ? iFirstLevelExpression : iFirstLevelExpression + evalResult;
							const ::gpk::SExpressionNode						& childEval							= *nodeExpression->Children[expressionResultChildIndex];
							const ::gpk::vcc						& childEvalView						= readerExpression.View[childEval.ObjectIndex];(void)childEvalView;
							const int32_t										indexOfResolvedSubExpression		= ::evaluateExpression(results, readerExpression, childEval.ObjectIndex, inputJSON, lastResult.IndexRootJSONNode, viewOfExpressionResult);
							ree_if(-1 == indexOfResolvedSubExpression, "Failed to resolve subexpression: '%s'.", ::gpk::toString(childEvalView).begin());
							lastResult.Output								= output = viewOfExpressionResult;
							lastResult.IndexJSONResult						= indexOfResolvedSubExpression;
						}
						iFirstLevelExpression							+= prevResult;
					}
					else if(::gpk::EXPRESSION_READER_TYPE_TERM_EQUALS == expressionType) {
						const int32_t										indexOfResolvedSubExpression		= ::evaluateExpression(results, readerExpression, childToSolve.ObjectIndex, inputJSON, lastResult.IndexRootJSONNode, viewOfExpressionResult);
						ree_if(-1 == indexOfResolvedSubExpression, "Failed to resolve subexpression: '%s'.", ::gpk::toString(currentExpressionView).begin());
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
								 || (currentType == ::gpk::JSON_TYPE_DOUBLE		&& resultType == ::gpk::JSON_TYPE_DOUBLE	)
							) {
								double												valueLeft				= 0;
								double												valueRight				= 0;
								if(currentType == ::gpk::JSON_TYPE_INTEGER) {
									valueLeft										= (double)currentJSON->Token->Value;
									valueRight										= (double)resultJSON->Token->Value;
								}
								else {
									memcpy(&valueLeft , &currentJSON->Token->Value, sizeof(uint64_t));
									memcpy(&valueRight, &resultJSON ->Token->Value, sizeof(uint64_t));
								}
								lastResult.SetBoolCarry(valueLeft == valueRight, output);
								lastResult.Output								= output;
							}
							else if(currentType == ::gpk::JSON_TYPE_INTEGER	&& resultType == ::gpk::JSON_TYPE_DOUBLE) {
								const double 										valueLeft				= (double)currentJSON->Token->Value;
								double 												valueRight				= 0;
								memcpy(&valueRight, &resultJSON->Token->Value, sizeof(uint64_t));
								lastResult.SetBoolCarry(valueLeft == valueRight, output);
								lastResult.Output								= output;
							}
							else if(currentType == ::gpk::JSON_TYPE_DOUBLE && resultType == ::gpk::JSON_TYPE_INTEGER) {
								const double 										valueRight				= (double)resultJSON->Token->Value;
								double 												valueLeft				= 0;
								memcpy(&valueLeft, &currentJSON->Token->Value, sizeof(uint64_t));
								lastResult.SetBoolCarry(valueLeft == valueRight, output);
								lastResult.Output								= output;
							}
							else if(::gpk::JSON_TYPE_BOOL == resultType		|| ::gpk::JSON_TYPE_NULL == resultType
								 || ::gpk::JSON_TYPE_BOOL == currentType	|| ::gpk::JSON_TYPE_NULL == currentType	) {
								const int32_t										prevResult							= ::evaluateAndClearBoolCarry	(lastResult, currentJSON, (int32_t)indexNodeJSON, currentView);
								const int32_t										evalResult							= ::evaluateBoolResult			(resultJSON, indexOfResolvedSubExpression, viewOfExpressionResult);;
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
		results.Results.push_back(lastResult);
		indexNodeJSON									= lastResult.IndexJSONResult;
	}
	return lastResult.IndexJSONResult;
}

::gpk::error_t			gpk::jsonExpressionResolve				(const ::gpk::SExpressionReader & reader, const ::gpk::SJSONReader & inputJSON, uint32_t indexNodeJSON, ::gpk::SJSONExpressionSolver& results, ::gpk::vcc & output)			{
	const ::gpk::aobj<::gpk::vcc>						& expressionViews						= reader.View;
	for(uint32_t iView = 0; iView < expressionViews.size(); ++iView) {
		const ::gpk::vcc									& viewExpression						= expressionViews[iView];
		const ::gpk::SExpressionToken						& typeExpression						= reader.Token[iView];
		(void)typeExpression;
		if(viewExpression.size()) {
			gpk_jexpr_info_printf("Expression element: %s. Type: %s. Parent: %i. Begin: %u. End: %u.", ::gpk::toString(viewExpression).begin(), ::gpk::get_value_label(typeExpression.Type).begin(), typeExpression.ParentIndex, typeExpression.Span.Begin, typeExpression.Span.End);
		}
	}
	::gpk::vcc											evaluated								= {};
	int32_t												jsonNodeResultOfEvaluation				= -1;
	if(reader.Tree.size()) {
#if defined(GPK_JSON_EXPRESSION_DEBUG)
		::printNode(reader.Tree[0], reader.View[0]);
#endif
		jsonNodeResultOfEvaluation						= ::evaluateExpression(results, reader, 0, inputJSON, indexNodeJSON, output);
		ree_if(-1 == jsonNodeResultOfEvaluation, "Failed to evaluate expression: %s.", ::gpk::toString(reader.View[0]).begin());
		evaluated										= output;
	}
	if(evaluated.size()) {
		gpk_jexpr_info_printf("Result of expression evaluation: %s.", ::gpk::toString(evaluated).begin());
	}
	return jsonNodeResultOfEvaluation;
}

::gpk::error_t			gpk::jsonExpressionResolve				(const ::gpk::vcs & expression, const ::gpk::SJSONReader & inputJSON, uint32_t indexNodeJSON, ::gpk::vcc & output)								{
	::gpk::SExpressionReader							reader;
	gpk_necall(::gpk::expressionReaderParse(reader, expression), "Failed to read JSONeN expression: '%s'.", ::gpk::toString(expression).begin());
	return ::gpk::jsonExpressionResolve(reader, inputJSON, indexNodeJSON, output);
}

::gpk::error_t			gpk::jsonExpressionResolve				(const ::gpk::SExpressionReader & reader, const ::gpk::SJSONReader & inputJSON, uint32_t indexNodeJSON, ::gpk::vcc & output){
	::gpk::SJSONExpressionSolver						results;
	::gpk::error_t										jsonNodeResultOfEvaluation				= ::gpk::jsonExpressionResolve(reader, inputJSON, indexNodeJSON, results, output);
	ree_if(-1 == jsonNodeResultOfEvaluation, "Failed to evaluate expression: %s.", reader.View.size() ? ::gpk::toString(reader.View[0]).begin() : "null");
	for(uint32_t iResult = 0; iResult < results.Results.size(); ++iResult)
		gpk_jexpr_info_printf("Result %u: '%s' | '%s'", iResult, ::gpk::toString(results.Results[iResult].Expression).begin(), ::gpk::toString(results.Results[iResult].Output).begin());
	return jsonNodeResultOfEvaluation;
}

::gpk::error_t			gpk::jsonStringFormat					(const ::gpk::vcs& format, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::apod<char>& output)					{
	::gpk::SStripLiteralState							stateLiteralStripper					= {};
	::gpk::apod<::gpk::SStripLiteralType>			typesLiteral							= {};
	gpk_necall(::gpk::stripLiteralParse(stateLiteralStripper, typesLiteral, format), "%s", "Unknown error.");	// strip root literals
	::gpk::aobj<::gpk::vcc>			views									= {};
	gpk_necall(::gpk::stripLiteralGetViews(views, typesLiteral, format), "%s", "Unknown error.");
	gpk_jexpr_info_printf("-**- %s -**-", "Results of token search");
	for(uint32_t iView = 0; iView < views.size(); ++iView) {
		::gpk::vcc								& toResolve								= views[iView];
		if(::gpk::STRIP_LITERAL_TYPE_TOKEN == typesLiteral[iView].Type)	 // we only have to solve tokens
			gpk_necall(::gpk::jsonExpressionResolve(toResolve, inputJSON, indexNodeJSON, toResolve), "%%u|Failed to resolve expression: %%.%us.", ::gpk::toString(toResolve).begin());
	}
	for(uint32_t iView = 0; iView < views.size(); ++iView)
		output.append(views[iView]);
	return 0;
}
