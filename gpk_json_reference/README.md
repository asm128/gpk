# GPK JSON reference executable

`gpk_json_reference` is a copy of the `jen` project setup with a separate
project identity. Its source calls existing `llc` JSON, expression, parsing,
resolution, and formatting facilities; it implements none of them.

- [Tutorial](TUTORIAL.md): build, run, and follow the examples.
- [Syntax guide](SYNTAX.md): expressions and format strings run by the demo.
- [API reference](REFERENCE.md): public declarations and types.
- [JSON manual](JSON_API.md): parsing, navigation, conversions, and output.
- [Expression manual](EXPRESSION_API.md): parser types, resolver overloads, and syntax.
- [Format manual](FORMAT_API.md): literal spans, formatting, and numeric helpers.
- [Coverage map](COVERAGE.md): each active API family and its runnable call site.

From the `gpk` checkout, build with
`D:\enterprise\operations\tools\build.bat Proofline gpk gpk_json_reference`.
Run `..\x64.Debug\gpk_json_reference.exe` for in-memory examples, or pass a
JSON filename to exercise `jsonFileRead` as well. The program prints one
result per section and returns a failure through the existing `llc` macros.

The project leaves `llc`, `jen`, and `test_gpk_json` source unchanged. The
separate `test_gpk_json` project carries the larger expression corpus.
