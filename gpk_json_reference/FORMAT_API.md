# Format splitter and numeric helper manual

This manual covers `llc/gpk_parse.h` and the `jsonStringFormat` declaration
in `llc/gpk_json_expression.h`. The reference executable calls every active
named function in this area. It does not implement a tokenizer or formatter.

## Format strings

`jsonStringFormat(format, json, rootIndex, output)` appends the formatted
text to an `apod<sc_t>`. Its implementation calls the literal splitter,
resolves token spans with `jsonExpressionResolve`, then appends all spans.
The executable uses `"Hello, {name}!"`, which produces `Hello, Ada!`, and
`"Literal \\{name}; value {name}."`, which produces
`Literal \{name}; value Ada.`. The backslash keeps the first opening brace
literal in the current formatter behavior.

`SStripLiteralState` stores the cursor, current element and span, nesting,
current character, escape flag, token flag, and bracket depth.
`SStripLiteralType` stores a parent index, `STRIP_LITERAL_TYPE`, and source
span. The enum has `LITERAL`, `TOKEN`, `COUNT`, and `UNKNOWN` values.

`stripLiteralParse(state, types, format)` fills the span array. The demo then
calls `stripLiteralGetViews(views, types, format)` and prints these three
spans for `Hello, {name}!`:

| Type | View |
| --- | --- |
| `LITERAL` | `Hello, ` |
| `TOKEN` | `name` |
| `LITERAL` | `!` |

`stripLiteralParseStep` is the lower-level operation. The demo advances
`state.IndexCurrentChar`, sets `state.CharCurrent` from the source view,
calls the step, and obtains three spans. Call `stripLiteralParse` for normal
formatting. `stripLiteralGetViews` views borrow the format bytes; keep them
alive while using the views.

## Numeric and whitespace helpers

`parseArbitraryBaseInteger(base, symbols, source, output)` parses digits
from an explicit alphabet. With base `2`, symbols `01`, and source `101`,
the demo gets value `5` after three digits. `parseIntegerDecimal` and
`parseIntegerHexadecimal` are typed wrappers: the demo reads `123rest` as
`123` from three digits, and `2a!` as `42` from two hex digits. The header
also supplies `DIGITS_DECIMAL` and `DIGITS_HEX` views.

`isSpaceCharacter(' ')` returns true in the demo. The demo passes the view
`" \tname"` to `skipToNextCharacter(cursor, source)` and gets cursor `2`,
the first nonspace position. These are existing helpers for callers that
need numeric or whitespace handling; the reference executable does not
duplicate their scans.
