# Expression and format syntax shown by the reference executable

Each example below is executed by `gpk_json_reference.exe`. The program
prints its result and stops through the `llc` error macros if a call fails.
The source of truth for the larger language corpus is
`test_gpk_json/main.cpp`.

## JSON values and paths

The first fixture is
`{"name":"Ada","age":37,"active":true,"ratio":1.5,"items":[10,20]}`.
The parser produces token, view, and tree arrays. A path starts at JSON node
`0` in these examples:

| Input expression | Result | API behavior |
| --- | --- | --- |
| `name` | `Ada` | Select the root object's `name` key. |
| `age` | `37` | Select an integer field. |
| `active` | `true` | Select a Boolean field. |

The second fixture adds `selection`, `children`, `parents`, and `properties`
so the following examples have concrete results:

| Input expression | Result | Syntax |
| --- | --- | --- |
| `children[('2')]` | `Jamie` | Parenthesized quoted index expression selects array element 2. |
| `'Ada'` | `Ada` | Quoted literal value. |
| `selection.active ? 'yes' : 'no'` | `yes` | Conditional based on a nested Boolean field. |
| `alive && !married` | `true` | AND and unary NOT. |
| `alive || married` | `true` | OR. |
| `!married` | `true` | Unary NOT. |
| `selection.index == selection.index` | `true` | Equality. |
| `parents['1'].property.(properties[selection.index].name)` | `green` | The expression in parentheses yields the property key `color`. |

The demo calls `expressionReaderParse` for each expression, then the
`SJSONExpressionSolver` overload of `jsonExpressionResolve`. The parser's
`SExpressionToken` and `SExpressionNode` arrays describe syntax; the solver
returns a result view and records evaluation steps. A Boolean result can have
a valid negative result index, so the demo checks specifically for `-1`.

## Format strings

`jsonStringFormat` accepts literal text with `{expression}` replacements:

| Format | Output |
| --- | --- |
| `Hello, {name}!` | `Hello, Ada!` |
| `Literal \{name}; value {name}.` | `Literal \{name}; value Ada.` |

The reference executable also calls `stripLiteralParse` and
`stripLiteralGetViews` on the first format, printing three spans: literal
`Hello, `, token `name`, and literal `!`. It drives
`stripLiteralParseStep` separately. `jsonStringFormat` performs this split,
resolves the token spans, and appends the result. Use
`jsonExpressionResolve` directly when the whole input is one expression.

## Views and errors

`vcst_t` excludes the trailing NUL of a C++ string literal. `vcsc_t` is a
character view with an explicit size; it is used for parsed spans and
resolved results. Pass views to `printf("%.*s", GPK_VIEW(view))` in this
checkout. The demo wraps API calls with `if_fail_fe` and checks conditions
with `ree_if`.
