# Runnable API coverage

This map ties the public API reference to calls in
`gpk_json_reference.cpp`. The functions in `main` only select and sequence
demonstrations. All parsing, navigation, comparison, resolution, and
formatting work is done by `llc`.

| API or overload family | Runnable section |
| --- | --- |
| `jsonParseStep` | `demoJSONSteps` |
| `jsonParse` with default and custom build flags | `demoJSONParse`, `demoTreeRebuild`, `demoExpressionExamples` |
| `jsonTreeRebuild`, `SJSONReader::Reset` | `demoTreeRebuild` |
| `SJSONReader::operator[]`, `begin`, `end` | `demoTypes` |
| `jsonArraySize`, `jsonArrayValueGet`, node and reader overloads | `demoJSONAccess`, `demoJSONNavigation` |
| `jsonObjectKeyCount`, node and reader overloads | `demoJSONNavigation` |
| `jsonObjectValueGet`, node and reader overloads | `demoJSONNavigation` |
| `jsonObjectKeyList`, three node and three reader overloads | `demoJSONNavigation` |
| `jsonCompareNumber`, `jsonCompareArray`, `jsonCompareObject`, explicit and shared view overloads | `demoJSONNavigation` |
| `jsonObjectGetCString`, `jsonObjectGetString` | `demoJSONNavigation` |
| `jsonObjectGetBoolean`, `jsonObjectGetInteger`, `jsonObjectGetDecimal` | `demoJSONNavigation`, `demoScalarOverloads` |
| `jsonObjectGetAsString`, `jsonObjectGetAsBoolean`, `jsonObjectGetAsInteger`, `jsonObjectGetAsDecimal` | `demoJSONAccess`, `demoScalarOverloads` |
| Six `jsonObjectGet*As*` scalar conversions | `demoScalarOverloads` |
| `jsonWrite`, `jsonArraySplit` | `demoJSONAccess`, `demoJSONNavigation` |
| `jsonMapToFields`, `jsonFieldsToMap` | `demoFieldMapping` |
| `jsonFileRead` | Optional filename branch in `main` |
| `expressionReaderParseStep` | `demoExpressionSteps` |
| `expressionReaderParse` | `demoResolve`, `demoResolveOverloads`, `demoTypes` |
| All five `jsonExpressionResolve` overloads | `demoResolve`, `demoResolveOverloads` |
| `SEvaluationStepResult::SetBoolCarry` | `demoResolveOverloads` |
| `stripLiteralParseStep` | `demoLiteralSteps` |
| `stripLiteralParse`, `stripLiteralGetViews` | `demoLiteralSplit`, `demoTypes` |
| `jsonStringFormat` | `demoFormat` |
| `parseArbitraryBaseInteger`, `parseIntegerDecimal`, `parseIntegerHexadecimal` | `demoNumericHelpers` |
| `isSpaceCharacter`, `skipToNextCharacter` | `demoNumericHelpers` |

`demoTypes` names and inspects `JSON_TYPE`, `json_id_t`, `SJSONToken`,
`SJSONNode`, `SJSONReaderState`, `SJSONReader`, `SJSONFile`,
`TKeyValJSONFile`, `EXPRESSION_READER_TYPE`, `SExpressionToken`,
`SExpressionNode`, `SExpressionReaderState`, `SExpressionReader`,
`SJSONExpressionSolver`, `SEvaluationStepResult`, `STRIP_LITERAL_TYPE`,
`SStripLiteralType`, and `SStripLiteralState`.
`demoFieldMapping` constructs `SJSONFieldBinding` values.

The commented `TODO` conversion declarations in `gpk_json.h` are absent from
the compiled API and are excluded. The current headers export
`expressionReaderParse` and `expressionReaderParseStep`, not functions named
`expressionParse` or `expressionParseStep`.
