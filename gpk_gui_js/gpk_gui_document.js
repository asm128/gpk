(function (gpk) {
  "use strict";

  const ALIGN_NAMES = Object.freeze({
    ORIGIN: gpk.ALIGN.ORIGIN, LEFT: gpk.ALIGN.LEFT, RIGHT: gpk.ALIGN.RIGHT, TOP: gpk.ALIGN.TOP, BOTTOM: gpk.ALIGN.BOTTOM,
    HCENTER: gpk.ALIGN.HCENTER, VCENTER: gpk.ALIGN.VCENTER, TOP_LEFT: gpk.ALIGN.TOP_LEFT, TOP_RIGHT: gpk.ALIGN.TOP_RIGHT,
    BOTTOM_LEFT: gpk.ALIGN.BOTTOM_LEFT, BOTTOM_RIGHT: gpk.ALIGN.BOTTOM_RIGHT, CENTER_TOP: gpk.ALIGN.CENTER_TOP,
    CENTER_BOTTOM: gpk.ALIGN.CENTER_BOTTOM, CENTER_LEFT: gpk.ALIGN.CENTER_LEFT, CENTER_RIGHT: gpk.ALIGN.CENTER_RIGHT, CENTER: gpk.ALIGN.CENTER
  });

  function guiDocumentNormalize(value) {
    if (Array.isArray(value)) return value.map(guiDocumentNormalize);
    if (!value || typeof value !== "object") return value;
    const output = {};
    for (const [key, child] of Object.entries(value))
      output[key] = key === "Align" && typeof child === "string" ? ALIGN_NAMES[child] : guiDocumentNormalize(child);
    return output;
  }

  function guiDocumentBuildNode(gui, parent, record) {
    const names = Object.keys(record);
    if (names.length !== 1) throw new Error("Each GUI document node must contain exactly one named control");
    const name = names[0], source = record[name], id = gpk.controlCreate(gui);
    gui.Controls.Identifier[id] = name;
    if (parent !== gpk.CID_INVALID) gpk.controlSetParent(gui.Controls, id, parent);
    const patch = guiDocumentNormalize(Object.fromEntries(Object.entries(source).filter(([key]) => key !== "children")));
    const childPatch = {};
    for (const child of source.children || []) {
      const result = guiDocumentBuildNode(gui, id, child);
      childPatch[result.name] = result.patch;
    }
    Object.assign(patch, childPatch);
    return {name, patch, id};
  }

  function guiDocumentCreate(gui, document) {
    if (!document || !Array.isArray(document.root)) throw new Error("GUI document requires a root array");
    const patch = {}, ids = {};
    const collect = id => {
      ids[gui.Controls.Identifier[id]] = id;
      for (const child of gui.Controls.Children[id]) collect(child);
    };
    for (const root of document.root) {
      const result = guiDocumentBuildNode(gui, gpk.CID_INVALID, root);
      patch[result.name] = result.patch;
      collect(result.id);
    }
    gpk.guiJSONLoad(gui, patch);
    return ids;
  }

  Object.assign(gpk, {GUI_DOCUMENT_ALIGN_NAMES: ALIGN_NAMES, guiDocumentNormalize, guiDocumentBuildNode, guiDocumentCreate});
})(globalThis.gpk);
