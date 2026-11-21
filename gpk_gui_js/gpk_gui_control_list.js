(function (gpk) {
  "use strict";

  class SControlList {
    constructor() { this.IdControl = -1; this.Orientation = gpk.DIRECTION.RIGHT; this.IndexParentList = -1; this.IndexParentItem = -1; this.IdControls = []; this.Events = []; }
  }
  class SPaletteGrid { constructor() { this.IdControl = -1; this.IdControls = []; this.Colors = null; } }
  class SEditBox { constructor() { this.IdControl = -1; this.IdControls = []; this.Colors = null; } }
  const ncTotal = placement => { const nc = gpk.controlNCSpacing(placement); return {x: nc.Left + nc.Right, y: nc.Top + nc.Bottom}; };

  gpk.controlListInitialize = function (gui, list) {
    Object.assign(list, new SControlList());
    list.IdControl = gpk.controlCreate(gui);
    gui.Controls.Placement[list.IdControl].Border = gpk.rect4();
    gui.Controls.Placement[list.IdControl].Margin = gpk.rect4();
    gui.Controls.Text[list.IdControl].Text = " ";
    gui.Controls.Constraints[list.IdControl].AttachSizeToText = {x: false, y: false};
    return 0;
  };
  gpk.controlListArrange = function (gui, list) {
    const text = gui.Controls.Text[list.IdControl];
    const font = gui.Fonts[text.FontSelected >= 0 && text.FontSelected < gui.Fonts.length ? text.FontSelected : gui.SelectedFont];
    for (let index = 0; index < list.IdControls.length; ++index) {
      const id = list.IdControls[index], placement = gui.Controls.Placement[id], controlText = gui.Controls.Text[id], constraints = gui.Controls.Constraints[id];
      const nc = ncTotal(placement);
      placement.Area.Size.y = font.CharSize.y + nc.y;
      if (list.Orientation === gpk.DIRECTION.DOWN) {
        constraints.AttachSizeToControl.x = list.IdControl;
        if (index) placement.Area.Offset.y = gui.Controls.Placement[list.IdControls[index - 1]].Area.Offset.y + gui.Controls.Placement[list.IdControls[index - 1]].Area.Size.y;
      }
      else {
        placement.Area.Size.x = font.CharSize.x * controlText.Text.length + nc.x;
        if (index) placement.Area.Offset.x = gui.Controls.Placement[list.IdControls[index - 1]].Area.Offset.x + gui.Controls.Placement[list.IdControls[index - 1]].Area.Size.x;
      }
    }
    const root = gui.Controls.Placement[list.IdControl]; root.Area.Size = gpk.v2();
    for (const id of list.IdControls) {
      const placement = gui.Controls.Placement[id], controlText = gui.Controls.Text[id];
      if (list.Orientation === gpk.DIRECTION.DOWN) {
        root.Area.Size.y += placement.Area.Size.y;
        root.Area.Size.x = Math.max(root.Area.Size.x, controlText.Text.length * font.CharSize.x + ncTotal(placement).x);
      }
      else {
        root.Area.Size.x += placement.Area.Size.x;
        root.Area.Size.y = Math.max(root.Area.Size.y, placement.Area.Size.y);
      }
    }
    if (list.Orientation === gpk.DIRECTION.DOWN) root.Area.Size.x += 16;
    return 0;
  };
  gpk.controlListPush = function (gui, list, text, eventCode = -1) {
    if (list.IdControl === -1) gpk.controlListInitialize(gui, list);
    const id = gpk.controlCreate(gui);
    gpk.controlSetParent(gui.Controls, id, list.IdControl);
    gui.Controls.Placement[id].Margin = gpk.rect4(4, 2, 4, 2);
    gui.Controls.Text[id].Text = String(text);
    gui.Controls.Text[id].Align = gpk.ALIGN.CENTER_LEFT;
    list.IdControls.push(id); list.Events.push(eventCode);
    gpk.controlListArrange(gui, list);
    return 0;
  };
  gpk.controlListCreate = function (gui, list, labels, parent = -1, orientation = gpk.DIRECTION.RIGHT) {
    gpk.controlListInitialize(gui, list); list.Orientation = orientation;
    if (parent !== -1) gpk.controlSetParent(gui.Controls, list.IdControl, parent);
    for (const label of labels) gpk.controlListPush(gui, list, String(label));
    return 0;
  };
  gpk.paletteGridInitialize = function (gui, palette) {
    palette.IdControl = gpk.controlCreate(gui);
    const placement = gui.Controls.Placement[palette.IdControl], nc = ncTotal(placement);
    placement.Area.Size = {x: 256 + nc.x, y: 256 + nc.y}; placement.Align = gpk.ALIGN.CENTER;
    return 0;
  };
  gpk.paletteGridColorsSet = function (gui, palette, colors) {
    if (palette.IdControl === -1) gpk.paletteGridInitialize(gui, palette);
    const width = colors.width || (colors[0] ? colors[0].length : 0), height = colors.height || colors.length;
    palette.IdControls.length = width * height; palette.Colors = colors;
    const client = gui.Controls.Area[palette.IdControl].Client.Local.Size;
    const target = {x: client.x || 256, y: client.y || 256};
    const cell = {x: target.x / width, y: target.y / height};
    const read = (x, y) => colors.data ? {r:colors.data[(y * width + x) * 4], g:colors.data[(y * width + x) * 4 + 1], b:colors.data[(y * width + x) * 4 + 2], a:colors.data[(y * width + x) * 4 + 3]} : colors[y][x];
    for (let y = 0; y < height; ++y) for (let x = 0; x < width; ++x) {
      const value = read(x, y); let colorIndex = gui.Colors.Palette.findIndex(item => gpk.colorEquals(item, value)); let themeIndex = colorIndex;
      if (colorIndex < 0) {
        colorIndex = gui.Colors.Palette.push(gpk.cloneColor(value)) - 1;
        const combos = Array.from({length:gpk.GUI_CONTROL_PALETTE.COUNT}, () => Array(gpk.UI_CONTROL_AREA.COUNT).fill(0));
        for (const state of [gpk.GUI_CONTROL_PALETTE.DISABLED,gpk.GUI_CONTROL_PALETTE.PRESSED,gpk.GUI_CONTROL_PALETTE.SELECTED,gpk.GUI_CONTROL_PALETTE.HOVER,gpk.GUI_CONTROL_PALETTE.NORMAL]) {
          combos[state][gpk.UI_CONTROL_AREA.BACKGROUND] = colorIndex; combos[state][gpk.UI_CONTROL_AREA.CLIENT] = colorIndex;
        }
        themeIndex = gui.Colors.ControlThemes.push({ColorCombos:combos}) - 1;
      }
      const linear = y * width + x, id = palette.IdControls[linear] = gpk.controlCreate(gui);
      gui.Controls.Draw[id].ColorTheme = themeIndex + 1;
      gui.Controls.Placement[id].Area = gpk.rect2(Math.trunc(cell.x * x), Math.trunc(cell.y * y), Math.trunc(cell.x), Math.trunc(cell.y));
      gpk.controlSetParent(gui.Controls, id, palette.IdControl);
    }
    return 0;
  };

  Object.assign(gpk, {SControlList, SPaletteGrid, SEditBox});
})(globalThis.gpk);
