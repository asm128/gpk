# JSON parser and accessor manual

This manual covers the active declarations in `llc/gpk_json.h`. Every named
function below has a call in `gpk_json_reference.cpp`; see [coverage](COVERAGE.md)
for the section name. The compiled demo uses the root object
`{"name":"Ada","age":37,"active":true,"ratio":1.5,"items":[10,20]}`.

## Parser, reader, and tree

`jsonParse(reader, text, buildTree, buildViews)` is the normal entry point.
The default values for both switches are `true`. With both enabled, the demo
gets 25 tokens, 25 views, and 25 tree nodes. With `buildTree=false`, the demo
calls `jsonTreeRebuild(reader.Token, reader.Tree)` explicitly and gets the
same node count. `jsonParseStep(reader, text)` is the lower-level entry point:
set `reader.StateRead.IndexCurrentChar` to the current position, then call
the step. It updates the reader's token and state data; a series of steps
does not by itself build a tree. The demo drives all steps and checks that
`NestLevel` returns to zero.

`SJSONReader` owns `Token`, `View`, `Tree`, and `StateRead` arrays. The views
refer to the supplied JSON bytes. `reader[index]` returns a node pointer;
`begin()` and `end()` delimit the node pointer array. `Reset()` clears token
and view arrays and restores parsing state. It does not clear `Tree` in this
header; create a fresh reader for an independent parse.

`SJSONToken` records `ParentIndex`, `Type`, source `Span`, and a numeric
`Value`. `SJSONNode` points to its token and parent and owns `Children` and
`ObjectIndex`. `SJSONReaderState` holds the cursor, current element, nesting,
current character, and flags for escaping, strings, separators, and
completion. `json_id_t` is the node identifier type. `JSON_TYPE` distinguishes
null, integer, decimal, Boolean, string, key, value, object, array, and
codepoint tokens; the enum also has `COUNT` and `UNKNOWN` values.

## Navigation

These calls can take a node directly or a `SJSONReader` and node index:

| Function | Example in the executable | Result |
| --- | --- | --- |
| `jsonArraySize` | `items` array | `2` |
| `jsonArrayValueGet` | `items`, index `1` | Node whose view is `20` |
| `jsonObjectKeyCount` | Root object | `5` |
| `jsonObjectValueGet` | Root, key `name` | Node whose view is `Ada` |
| `jsonObjectKeyList` | Root object | Five keys; three node and three reader overloads are called. |

The index-returning calls return an `err_t`; check their result before using
it to index `Tree` or `View`. `jsonObjectKeyList` can fill indices, key views,
or both. The demo prints `name`, `age`, `active`, `ratio`, and `items` in that
order. The key views borrow the reader's input bytes.

## Typed accessors

`jsonObjectGetString` and `jsonObjectGetCString` read the selected `name`
node; both print `Ada` in this fixture. `jsonObjectGetBoolean` reads
`active` as true. `jsonObjectGetInteger` reads `age` as `37` into all eight
integer widths: `u0_t`, `u1_t`, `u2_t`, `u3_t`, `s0_t`, `s1_t`, `s2_t`, and
`s3_t`. `jsonObjectGetDecimal` reads `ratio` as `1.5` into `f2_t` and
`f3_t`.

The `jsonObjectGetAs*` variants first locate a field by key. The demo calls
`jsonObjectGetAsString(root, "name", out)`,
`jsonObjectGetAsBoolean(root, "active", out)`, all eight
`jsonObjectGetAsInteger(root, "age", out)` overloads, and both
`jsonObjectGetAsDecimal(root, "ratio", out)` overloads. The direct variants
instead accept the already selected value node index.

The six explicit scalar conversion functions are also called:

| Call on this fixture | Printed value |
| --- | --- |
| `jsonObjectGetIntegerAsBoolean(age)` | `1` |
| `jsonObjectGetIntegerAsDecimal(age)` | `37` |
| `jsonObjectGetDecimalAsInteger(ratio)` | `1` |
| `jsonObjectGetDecimalAsBoolean(ratio)` | `1` |
| `jsonObjectGetBooleanAsInteger(active)` | `1` |
| `jsonObjectGetBooleanAsDecimal(active)` | `1` |

The integer result of `1.5` is shown by the implementation's current
conversion, rather than assumed as a general rounding contract. The
commented `TODO` string conversion declarations in the header have no
implementation and are not callable members of this API.

## Comparison, writing, splitting, and mapping

`jsonCompareNumber`, `jsonCompareArray`, and `jsonCompareObject` each have an
overload with explicit view collections and a convenience overload for two
nodes sharing one view collection. The demo calls both forms with a node
compared to itself and checks for the current equality result `1`.

`jsonWrite(node, views, output)` appends serialized JSON to an `asc_t`; the
demo writes the root object. `jsonArraySplit(arrayNode, views, blockSize,
parts)` splits `items` into `[10]` and `[20]` with block size `1`.
Use a positive block size.

`SJSONFieldBinding` pairs a field view with an expected `JSON_TYPE`.
`jsonMapToFields` maps each key/value entry to a field index;
`jsonFieldsToMap` maps each field back to an entry index. The demo uses the
`name` and `age` fields in reverse map order. It prints map-to-field indices
`1, 0` and field-to-map indices `1, 0`.

`SJSONFile` holds owned file bytes and a reader. If a filename is supplied
to the executable, it calls `jsonFileRead(file, filename)` and prints the
reader's token count. `TKeyValJSONFile` is the header's key/value alias for
this type and is instantiated in `demoTypes`.
