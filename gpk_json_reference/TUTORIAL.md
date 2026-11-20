# JSON and JSON expression tutorial

This tutorial accompanies `gpk_json_reference.exe`. Build it with
`D:\enterprise\operations\tools\build.bat Proofline gpk gpk_json_reference`
from the `gpk` checkout. Run `..\x64.Debug\gpk_json_reference.exe` from there.
The executable calls `llc`; it contains no JSON tokenizer, expression parser,
resolver, or formatter of its own.

## 1. Parse JSON

`gpk::jsonParse(reader, text)` fills `SJSONReader::Token`, `View`, and `Tree`.
The example uses:

```json
{"name":"Ada","age":37,"active":true,"ratio":1.5,"items":[10,20]}
```

The root node is index `0`. Keep the input text alive while using `reader.View`;
the views refer to spans of that text. `jsonParseStep(reader, text)` is the
low-level step used by `jsonParse`: set `reader.StateRead.IndexCurrentChar` for
each character and call it once. A step sequence produces tokens; use
`jsonParse` when you also need built views and a tree. The demo shows both
calls in separate readers.

## 2. Read values through accessors

`jsonObjectGetAsString(reader, 0, "name", name)` returns `Ada`.
`jsonObjectGetAsInteger(reader, 0, "age", age)` returns `37`.
`jsonObjectGetAsBoolean(reader, 0, "active", active)` returns true.
`jsonObjectValueGet(reader, 0, "items")` returns the array node index;
`jsonArraySize(reader, index)` returns `2`; and
`jsonArrayValueGet(reader, index, 1)` selects `20`.
`jsonWrite(reader.Tree[0], reader.View, output)` writes the parsed tree back
into an `asc_t` buffer.
The executable also calls `jsonObjectKeyCount`, all three
`jsonObjectKeyList` reader overloads, node string and scalar accessors,
`jsonArraySplit`, `jsonCompareNumber`, `jsonCompareArray`,
`jsonCompareObject`, `jsonTreeRebuild`, and `SJSONReader::Reset`.
It calls `jsonMapToFields` and `jsonFieldsToMap` on a two-field example.
`parseIntegerDecimal`, `parseIntegerHexadecimal`,
`parseArbitraryBaseInteger`, `skipToNextCharacter`, and
`isSpaceCharacter` have their own output lines.

## 3. Parse and resolve one expression

`expressionReaderParse(reader, "name")` fills an `SExpressionReader`.
`jsonExpressionResolve(reader, json, 0, solver, result)` resolves it against
the JSON root and returns `Ada` in `result`. The demo also resolves `age` and
`active`, producing `37` and `true`.

The solver overload is useful when inspecting `SJSONExpressionSolver::Results`
and when a Boolean or literal result has a negative result index. Check for
the resolver's `-1` failure value; a negative index such as `-5` can describe
a valid Boolean result. `expressionReaderParseStep` is the low-level parsing
step; the demo drives it with `SExpressionReader::StateRead.IndexCurrentChar`
before parsing the same expression normally.

Examples from the integrated `test_gpk_json` suite include:

| Expression | Meaning |
| --- | --- |
| `name` | Object key lookup |
| `children[('2')]` | Array element selected by a quoted index expression |
| `selection.active ? 'yes' : 'no'` | Conditional expression |
| `alive && !married` | Boolean operators |
| `parents['1'].property.(properties[selection.index].name)` | Computed key |

See `test_gpk_json/main.cpp` for the executable corpus and its expected
outputs. The exact JSON fixture there governs each example's result.
The [syntax guide](SYNTAX.md) explains every expression in this demo.

## 4. Format literal text with expressions

`jsonStringFormat("Hello, {name}!", json, 0, output)` produces
`Hello, Ada!`. Its implementation calls `stripLiteralParse` to identify
literal and token spans, `stripLiteralGetViews` to expose those spans,
`jsonExpressionResolve` for token spans, and `output.append` for the result.
The demo prints the three spans (`Hello, `, `name`, `!`) and calls the formatter.
Use `\{` to keep an opening brace literal, as shown by the integrated
`test_gpk_json` formatter example and the reference executable's
`Literal \{name}; value {name}.` example. The splitter step API is also
demonstrated explicitly.

`jsonStringFormat` accepts a format string containing literal text and
`{expression}` substitutions. `jsonExpressionResolve` accepts a single
expression. Use the former for `Hello, {name}!` and the latter for `name`.

## 5. Error flow and view lengths

The demo wraps calls that return `gpk::err_t` with `if_fail_fe`, and checks
the resolver's specific `-1` failure with `ree_if`. It passes `vcst_t` for
string literals so the terminating NUL is excluded from the view. It prints
views with `GPK_VIEW(view)` and `%.*s`.

The historic prose example in `json_expression_notation.txt` uses `people[2]`
and a curly computed key. The integrated current corpus instead demonstrates
quoted index expressions such as `[('2')]` and parenthesized computed keys.
This tutorial's runnable examples use the syntax verified by the current
executable tests.

## 6. Parse a JSON file

Pass a JSON filename as the reference executable's optional first argument.
It calls `jsonFileRead(SJSONFile&, filename)` and prints the parsed token
count. The no-argument run uses only in-memory JSON and does not write a file.
