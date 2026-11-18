(function (gpk) {
  "use strict";

  const gui = new gpk.SGUI();
  const ids = gpk.guiDocumentCreate(gui, globalThis.GUI_TYPE_EXPLORER_DOCUMENT);
  const canvas = document.getElementById("gui");
  const status = document.getElementById("status");
  let lastAction = -1;
  const host = new gpk.SGUIHost(canvas, gui, {
    background: gpk.rgba(10, 14, 20, 255),
    onFrame: () => {
      for (let id = 0; id < gui.Controls.States.length; ++id) if (gui.Controls.States[id].IsAction() && id !== lastAction) {
        lastAction = id; const name = gui.Controls.Identifier[id] || ("Control " + id); status.textContent = "Action: " + name;
        if (name.startsWith("structure_") || name.startsWith("enum_")) gpk.controlSelect(gui.Controls, id);
      }
    }
  });
  globalThis.gpkDemo = {gui, ids, host};
  host.start();
})(globalThis.gpk);
