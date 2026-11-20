# Expression parser and resolver manual

This manual covers `llc/gpk_expression.h` and
`llc/gpk_json_expression.h`. The current headers export
`expressionReaderParse` and `expressionReaderParseStep`; there are no public
functions spelled `expressionParse` or `expressionParseStep` in this checkout.
The executable calls the actual entry points and shows their results.

## Parse a whole expression or drive individual steps

`expressionReaderParse(reader, expression)` fills
`SExpressionReader::Token`, `View`, and `Tree`. For `name`, the demo reports
two tokens. `expressionReaderParseStep(reader, expression)` handles one
position at `reader.StateRead.IndexCurrentChar`; the demo advances that
cursor and calls the step for each character. Use the whole-expression call
for ordinary work. `SExpressionReaderState` carries the cursor, current
element, nesting, current character, escaping, separator, and string quote
state between steps.

An `SExpressionToken` records a parent index, token type, source span, and
closing-condition flag. `SExpressionNode` links a token to a parent, children,
and object index. `SExpressionReader` also contains a subexpression cache.
The demo names and inspects each type in `demoTypes`.

`EXPRESSION_READER_TYPE` has `KEY`, `LITERAL`, `CODEPOINT`, `UNARY_NOT`, and
the `TERM_*` values for any, key, index, Boolean, AND, OR, and equals terms;
`UNKNOWN` is its sentinel. These are parser token categories, not strings
to pass to the resolver.

## Resolve against a JSON root

`jsonExpressionResolve` takes a JSON reader and a root node index. The demo
uses root `0`. The overload accepting a parsed `SExpressionReader`,
`SJSONExpressionSolver&`, and `vcsc_t& result` exposes the evaluation record.
The demo resolves `name` to `Ada`, `age` to `37`, and `active` to `true` with
that overload.

The remaining overloads accept a parsed reader with output, a parsed reader
with discarded output, expression text with output, or expression text with
discarded output. `demoResolveOverloads` calls all four on `name`. The text
overloads parse the expression for the caller. For the solver overload,
`-1` is the failure index checked by the demo; Boolean results can use a
different negative index, such as `-5`, while still producing a valid result.

`SJSONExpressionSolver::Results` holds `SEvaluationStepResult` entries.
Each step records the JSON root/result indices, carry state, output and
expression views, and JSON reader reference. `SetBoolCarry` stores a Boolean
result and output view; `demoResolveOverloads` calls it with `true`.
The constants `strBool`, `strNull`, `strZero`, `strEmptyObj`, and
`strEmptyArr` are supplied as views by the header.

## Syntax exercised by the executable

| Expression | Output | Facility shown |
| --- | --- | --- |
| `children[('2')]` | `Jamie` | Array index expression. |
| `'Ada'` | `Ada` | Literal. |
| `selection.active ? 'yes' : 'no'` | `yes` | Conditional. |
| `alive && !married` | `true` | AND and unary NOT. |
| `alive || married` | `true` | OR. |
| `!married` | `true` | Unary NOT alone. |
| `selection.index == selection.index` | `true` | Equality. |
| `parents['1'].property.(properties[selection.index].name)` | `green` | Computed property key. |

The second JSON fixture in `demoExpressionExamples` gives these expressions
their values. [SYNTAX.md](SYNTAX.md) walks through the syntax. The integrated
`test_gpk_json` project contains the larger 179-expression corpus and a
formatter example; it is separate from this reference executable.
