(function (gpk) {
  "use strict";

  gpk.VIEWPORT_CONTROL = Object.freeze({RESIZE_LEFT:0, RESIZE_TOP:1, RESIZE_RIGHT:2, RESIZE_BOTTOM:3, RESIZE_TOP_LEFT:4, RESIZE_TOP_RIGHT:5, RESIZE_BOTTOM_LEFT:6, RESIZE_BOTTOM_RIGHT:7, TITLE:8, CLOSE:9, TARGET:10, COUNT:11});
  class SViewport { constructor() { this.IdControl = -1; this.IdControls = Array(gpk.VIEWPORT_CONTROL.COUNT).fill(-1); this.IdDesktopElement = -1; this.ControlType = -1; } }
  let viewportCount = 0;
  gpk.viewportInitialize = function (gui, viewport) {
    const font = gui.Fonts[gui.SelectedFont], titleHeight = font.CharSize.y + 4, targetSize = {x: 800, y: 450};
    viewport.IdControl = gpk.controlCreate(gui); const root = gui.Controls.Placement[viewport.IdControl]; root.Align = gpk.ALIGN.TOP_LEFT; root.Border = root.Margin = gpk.rect4(1,1,1,1);
    const nc = gpk.controlNCSpacing(root); root.Area.Size = {x: targetSize.x + nc.Left + nc.Right, y: targetSize.y + titleHeight + nc.Top + nc.Bottom};
    for (let index = 0; index < viewport.IdControls.length; ++index) {
      const id = viewport.IdControls[index] = gpk.controlCreate(gui); gui.Controls.Constraints[id].AttachSizeToControl.x = id; gpk.controlSetParent(gui.Controls, id, viewport.IdControl);
      if (index < gpk.VIEWPORT_CONTROL.TITLE) { gui.Controls.Placement[id].Area.Size = {x:16,y:16}; gui.Controls.Placement[id].Area.Offset = {x:-2,y:-2}; gui.Controls.Constraints[id].AttachSizeToControl = {x:-1,y:-1}; }
    }
    const C = gpk.VIEWPORT_CONTROL, alignments = [gpk.ALIGN.CENTER_LEFT,gpk.ALIGN.CENTER_TOP,gpk.ALIGN.CENTER_RIGHT,gpk.ALIGN.CENTER_BOTTOM,gpk.ALIGN.TOP_LEFT,gpk.ALIGN.TOP_RIGHT,gpk.ALIGN.BOTTOM_LEFT,gpk.ALIGN.BOTTOM_RIGHT];
    for (let index = 0; index < 8; ++index) gui.Controls.Placement[viewport.IdControls[index]].Align = alignments[index];
    let id = viewport.IdControls[C.CLOSE], placement = gui.Controls.Placement[id]; placement.Area.Size = {x:titleHeight,y:titleHeight}; placement.Align = gpk.ALIGN.TOP_RIGHT;
    gui.Controls.Text[id].Text = "X"; gui.Controls.Text[id].Align = gpk.ALIGN.CENTER; gui.Controls.Constraints[id].AttachSizeToControl = {x:-1,y:-1};
    id = viewport.IdControls[C.TITLE]; placement = gui.Controls.Placement[id]; placement.Area.Size.y = titleHeight; placement.Area.Offset.x = titleHeight; placement.Align = gpk.ALIGN.TOP_RIGHT;
    gui.Controls.Text[id].Text = "Viewport #" + (++viewportCount); gui.Controls.Text[id].Align = gpk.ALIGN.CENTER; gui.Controls.Constraints[id].DockToControl.Right = viewport.IdControls[C.CLOSE];
    id = viewport.IdControls[C.TARGET]; placement = gui.Controls.Placement[id]; placement.Border = gpk.rect4(); placement.Margin = gpk.rect4(); placement.Area.Size = targetSize;
    gui.Controls.Text[id].Align = gpk.ALIGN.CENTER_TOP; gui.Controls.Draw[id].ColorTheme = 60; gui.Controls.Constraints[id].DockToControl.Bottom = viewport.IdControls[C.TITLE]; return 0;
  };
  gpk.SViewport = SViewport;
})(globalThis.gpk);
