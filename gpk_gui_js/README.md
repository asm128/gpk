# GPK GUI JavaScript port

This folder is an almost literal JavaScript translation of the LLC GUI implementation supplied on 2026-11-18. It preserves control IDs, the struct-of-arrays control table, state masks, parent/child ownership, invalidation, layout and docking, JSON patches, scripts, input transitions, palette selection, bitmap fonts, recursive drawing, and composite controls.

`gpk_expression.js` translates the expression reader into a tokenizer, reader token/tree model, precedence parser, and nested format-token parser. `gpk_json_expression.js` translates the JSON evaluator, including computed keys and indices, JSON truth rules, deep array/object equality, unary and boolean operators, and lazy conditional branches. Evaluation never uses JavaScript `eval` or `Function`.

`index.html` runs directly from disk. The canvas is only the pixel presentation target; it does not replace the GUI with DOM or CSS layout. `test.html` runs the browser-side behavioral checks. Neither page needs a server, package manager, generated bundle, or external dependency.

The core bit helpers retain LLC's `bit_all` behavior: compound alignment masks such as `HCENTER` and `VCENTER` match only when every constituent bit is present. The supplied document therefore keeps its authored top-left workspace and panel coordinates instead of treating edge alignment as center alignment. `gpk_gui_document.js` keeps the fixture bootstrap separate and makes the complete document geometry available to the browser checks.

The core `guiJSONLoad()` behavior matches the inspected C++ source: it patches controls that already exist and are identified. `demo.js` separately bootstraps the control tree from Pablo's supplied authoring document, then applies the normal identified-control patch. This does not add control creation to the core JSON loader.

Standard JSON documents continue through the browser's native `JSON.parse()`. The inspected LLC GUI depends on a custom expression grammar layered over JSON, not on a second incompatible JSON document grammar.

`Verify-GPKGUIJS.ps1` performs a non-executing integrity check over the exact file graph, embedded raster fonts, and supplied document. It does not replace the behavioral checks in `test.html`.
