# JSON, expression, and formatting API reference

Authoritative declarations are in `llc/gpk_json.h`, `llc/gpk_expression.h`,
`llc/gpk_json_expression.h`, and `llc/gpk_parse.h`. This manual describes the
public declarations in those headers in the current Proofline checkout. The
executable examples are in `gpk_json_reference.cpp`; the larger expression
corpus is in `test_gpk_json/main.cpp`.

## Parse entry points

| API | Inputs and output | Use |
| --- | --- | --- |
| `jsonParse(SJSONReader&, vcsc_c&, bool buildTree=true, bool buildViews=true)` | Reader, JSON text, two build switches; returns `err_t` | Complete JSON parse. Normal mode fills tokens, views, tree. |
| `jsonParseStep(SJSONReader&, vcsc_c&)` | Reader state and JSON text | One scanner step at `StateRead.IndexCurrentChar`. The caller advances the cursor. |
| `jsonTreeRebuild(view<SJSONToken>&, apobj<SJSONNode>&)` | Existing tokens and destination nodes | Build a node tree from tokens. |
| `expressionReaderParse(SExpressionReader&, vcsc_t&)` | Reader and expression text | Complete expression parse, including tokens, views, and tree. |
| `expressionReaderParseStep(SExpressionReader&, vcsc_t&)` | Reader state and expression text | One parsing step at `StateRead.IndexCurrentChar`. |
| `stripLiteralParse(SStripLiteralState&, apod<SStripLiteralType>&, vcsc_c&)` | State, output spans, format text | Split literal text from `{expression}` tokens. |
| `stripLiteralParseStep(...)` | Same state, spans, text | One splitter step; `stripLiteralParse` drives this normally. |
| `stripLiteralGetViews(aobj<vcsc_t>&, view<const SStripLiteralType>&, vcsc_c&)` | Spans and source text | Views of each literal or token span. |

The names `expressionParse()` and `expressionParseStep()` do not appear in
these current headers. Their public entry points here are
`expressionReaderParse()` and `expressionReaderParseStep()`.

## Resolution and formatting

`jsonExpressionResolve` has these overloads:

1. Parsed `SExpressionReader`, `SJSONReader`, root node index,
   `SJSONExpressionSolver&`, and `vcsc_t& output`.
2. Parsed reader, JSON reader, root index, and output view; this creates its
   own solver.
3. Parsed reader, JSON reader, and root index; output is discarded.
4. Expression `vcst_c&`, JSON reader, root index, and output view; this parses
   the expression internally.
5. Expression text, JSON reader, and root index; output is discarded.

The first overload exposes `SJSONExpressionSolver::Results` and preserves
valid negative result indices used for Boolean and literal results. A result
index of `-1` is the error check used by the integrated expression test.

`jsonStringFormat(vcst_c& format, const SJSONReader&, uint32_t root,
apod<sc_t>& output)` splits literal text and `{expression}` tokens, resolves
tokens against the chosen JSON root, and appends the formatted result. Example:
`"Hello, {name}!"` with `{"name":"Ada"}` yields `Hello, Ada!`.

## JSON types and data ownership

| Type | Fields or role |
| --- | --- |
| `JSON_TYPE` | `NULL`, `INTEGER`, `DECIMAL`, `BOOLEAN`, `STRING`, `KEY`, `VALUE`, `OBJECT`, `ARRAY`, `CODEPOINT`, `COUNT`, `UNKNOWN`. |
| `json_id_t` | JSON node identifier (`s2_t` in the normal build; smaller on Arduino). |
| `SJSONToken` | `ParentIndex`, `Type`, `Span`, `Value`. |
| `SJSONNode` | `Token`, `Parent`, `Children`, `ObjectIndex`. |
| `SJSONReaderState` | Cursor, current element, nesting, character, and flags for escaping, strings, separators, and completion. |
| `SJSONReader` | `Token`, `View`, `Tree`, `StateRead`; `operator[]`, `begin()`, `end()`, `Reset()`. |
| `SJSONFile` | Owned `Bytes` plus a `Reader`; use with `jsonFileRead`. |
| `TKeyValJSONFile` | Key/value wrapper for a JSON file. |
| `SJSONFieldBinding` | `Field` and expected `Type` for field mapping. |

`SJSONReader::View` is a set of views into input bytes; keep the input alive.
`SJSONReader::Reset()` clears token and view arrays and resets state.

## JSON navigation, conversion, and output

Every navigation family has a node overload and/or a reader/index overload:

| Family | Return or effect |
| --- | --- |
| `jsonArraySize`, `jsonObjectKeyCount` | Number of array values or object keys. |
| `jsonArrayValueGet`, `jsonObjectValueGet` | Index of a selected child value. |
| `jsonObjectKeyList` | Object keys and optionally their indices. |
| `jsonCompareNumber`, `jsonCompareArray`, `jsonCompareObject` | Compare corresponding JSON values; overloads accept explicit view collections or shared views. |
| `jsonObjectGetCString`, `jsonObjectGetString` | Read string value from a selected node. |
| `jsonObjectGetBoolean`, `jsonObjectGetInteger`, `jsonObjectGetDecimal` | Read typed Boolean, integer, or decimal from a selected node. Integer overloads cover `u0_t` through `u3_t` and `s0_t` through `s3_t`; decimal overloads cover `f2_t`, `f3_t`. |
| `jsonObjectGetIntegerAsBoolean`, `jsonObjectGetIntegerAsDecimal`, `jsonObjectGetDecimalAsInteger`, `jsonObjectGetDecimalAsBoolean`, `jsonObjectGetBooleanAsInteger`, `jsonObjectGetBooleanAsDecimal` | Explicit conversions between JSON scalar kinds. |
| `jsonObjectGetAsString`, `jsonObjectGetAsBoolean`, `jsonObjectGetAsInteger`, `jsonObjectGetAsDecimal` | Find an object field by key and read a typed value. Integer and decimal overloads cover the scalar types above. |
| `jsonFileRead` | Load file bytes into `SJSONFile` and parse them. |
| `jsonWrite` | Serialize a node into `apod<sc_t>`. |
| `jsonArraySplit` | Split an array into blocks of serialized JSON. |
| `jsonMapToFields`, `jsonFieldsToMap` | Map field indices and key/value bindings. |

The commented `TODO` declarations in `gpk_json.h` are not callable APIs.
Examples of `jsonObjectGetAs*`, `jsonArray*`, and `jsonWrite` are executable
in the reference project.

## Expression types and inspection

| Type | Fields or role |
| --- | --- |
| `EXPRESSION_READER_TYPE` | `TERM_ANY`, `TERM_KEY`, `TERM_INDEX`, `TERM_BOOL`, `TERM_AND`, `TERM_OR`, `TERM_EQUALS`, `KEY`, `LITERAL`, `UNARY_NOT`, `CODEPOINT`, `UNKNOWN`. |
| `SExpressionToken` | `ParentIndex`, `Type`, `Span`, `ClosingCondition`. |
| `SExpressionReaderState` | Cursor, current element, nesting, character, escaping, separator, and quoting state. |
| `SExpressionNode` | Token pointer, parent pointer, children, and object index. |
| `SExpressionReader` | Token, view, and tree arrays; parser state; subexpression cache. |
| `SEvaluationStepResult` | Root/result indices, carried Boolean value, JSON reader, output and expression views; `SetBoolCarry()` sets a Boolean result. |
| `SJSONExpressionSolver` | `Results` array of evaluation steps. |

The constant views `strEmptyObj`, `strEmptyArr`, `strNull`, `strBool`, and
`strZero` are declared in `gpk_json_expression.h` and used by evaluation.

## Literal splitter and numeric helpers

`STRIP_LITERAL_TYPE` is `LITERAL`, `TOKEN`, `COUNT`, or `UNKNOWN`.
`SStripLiteralType` contains `ParentIndex`, `Type`, and `Span`.
`SStripLiteralState` contains the cursor, current span, nesting, current
character, escaping and token flags, and bracket depth. The formatter uses
these types; the demo prints their spans without inspecting characters.

`parseArbitraryBaseInteger(base, symbols, source, output)` parses digits from
a chosen alphabet. `parseIntegerDecimal` and `parseIntegerHexadecimal` are
typed wrappers. `DIGITS_DECIMAL` and `DIGITS_HEX` are the supplied alphabets.
`isSpaceCharacter` classifies whitespace; `skipToNextCharacter` advances a
view cursor over whitespace. Use these helpers instead of duplicating scans.

## Error flow

The APIs return `gpk::err_t`. The demo uses `if_fail_fe` for called operations
and `ree_if` for checked conditions. The resolver's solver overload is checked
for its specific `-1` failure value. String literal inputs are `vcst_t` so
their terminating NUL is outside the view; use `GPK_VIEW(view)` with `%.*s`
when printing a view.
