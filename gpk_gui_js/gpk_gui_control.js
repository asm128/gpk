(function (gpk) {
  "use strict";

  gpk.GUI_COLOR_MODE = Object.freeze({Default: 0, Mode3D: 1, Flat: 2});
  gpk.GUI_CONTROL_LIMIT = Object.freeze({Left: gpk.ALIGN.LEFT, Top: gpk.ALIGN.TOP, Right: gpk.ALIGN.RIGHT, Bottom: gpk.ALIGN.BOTTOM, Outer: gpk.ALIGN.FAR, Inner: gpk.ALIGN.NEAR});
  gpk.GUI_CONTROL_PALETTE = Object.freeze({
    NORMAL: 0, HIDDEN: 1, DISABLED: 2, HOVER: 3, PRESSED: 4, DRAGGING: 5,
    SELECTED: 6, SELECTED_HIDDEN: 7, SELECTED_DISABLED: 8, SELECTED_HOVER: 9,
    SELECTED_PRESSED: 10, SELECTED_DRAGGING: 11, OUTDATED: 12, COUNT: 13
  });
  gpk.GUI_CONTROL_AREA_OLD = Object.freeze({CLIENT: 0, BORDER_LEFT: 1, BORDER_TOP: 2, BORDER_RIGHT: 3, BORDER_BOTTOM: 4, COUNT: 5});
  gpk.UI_CONTROL_AREA = Object.freeze({
    BACKGROUND: 0, CLIENT: 1, BORDER_LEFT: 2, BORDER_TOP: 3, BORDER_RIGHT: 4, BORDER_BOTTOM: 5,
    TEXT_BACKGROUND: 6, TEXT_FACE: 7, MARGIN_LEFT: 8, MARGIN_TOP: 9, MARGIN_RIGHT: 10, MARGIN_BOTTOM: 11,
    CORNER_LEFT_TOP_TOP: 12, CORNER_LEFT_TOP_LEFT: 13, CORNER_RIGHT_TOP_TOP: 14, CORNER_RIGHT_TOP_RIGHT: 15,
    CORNER_LEFT_BOTTOM_LEFT: 16, CORNER_LEFT_BOTTOM_BOTTOM: 17, CORNER_RIGHT_BOTTOM_RIGHT: 18,
    CORNER_RIGHT_BOTTOM_BOTTOM: 19, COUNT: 20
  });

  class SControlPlacement {
    constructor() {
      this.Area = gpk.rect2(0, 0, 16, 16);
      this.Margin = gpk.rect4(1, 1, 1, 1);
      this.Border = gpk.rect4(1, 1, 1, 1);
      this.Align = gpk.ALIGN.TOP_LEFT;
      this.RelativeBorderSize = 0;
      this.RelativeMarginSize = 0;
      this.RelativeMetrics = 0;
    }
  }
  class SControlDraw {
    constructor(defaultColors) {
      this.ColorMode = gpk.GUI_COLOR_MODE.Default;
      this.FrameOut = 0;
      this.NoBorder = 0;
      this.NoClient = 0;
      this.UseNewPalettes = 0;
      this.ColorTheme = 0;
      this.Palettes = Array.from({length: gpk.GUI_CONTROL_PALETTE.COUNT}, (_, index) => defaultColors && index < defaultColors.length ? defaultColors[index] : index);
    }
  }
  class SControlRectangle {
    constructor() { this.Local = gpk.rect2(); this.Global = gpk.rect2(); }
  }
  class SControlArea {
    constructor() {
      this.Total = new SControlRectangle();
      this.Client = new SControlRectangle();
      this.Text = gpk.rect2();
      this.Rectangles = Array.from({length: gpk.GUI_CONTROL_AREA_OLD.COUNT}, () => gpk.rect2());
      this.Triangles = Array.from({length: 8}, () => [gpk.v2(), gpk.v2(), gpk.v2()]);
    }
  }
  class SControlConstraints {
    constructor() {
      this.AttachSizeToControl = {x: gpk.CID_INVALID, y: gpk.CID_INVALID};
      this.AttachSizeToText = {x: false, y: false};
      this.DockToControl = {Left: gpk.CID_INVALID, Top: gpk.CID_INVALID, Right: gpk.CID_INVALID, Bottom: gpk.CID_INVALID};
      this.SizeMinMax = {Min: gpk.v2(), Max: gpk.v2(0x7FFF, 0x7FFF)};
    }
  }
  class SControlText {
    constructor() {
      this.Text = "$";
      this.Align = gpk.ALIGN.CENTER;
      this.Selection = {Min: 0, Max: 1};
      this.FontSelected = -1;
    }
  }
  class SControlImage {
    constructor() {
      this.Temp = null;
      this.Image = null;
      this.ImageOffset = gpk.v2();
      this.ImageAlign = gpk.ALIGN.CENTER;
      this.ImageInvertX = 0;
      this.ImageInvertY = 0;
      this.Rotation = 0;
    }
  }
  class SControlTable {
    constructor() {
      this.States = [];
      this.TextState = [];
      this.Identifier = [];
      this.Script = [];
      this.Constraints = [];
      this.Placement = [];
      this.Area = [];
      this.Draw = [];
      this.Text = [];
      this.Images = [];
      this.Children = [];
      this.EventQueue = [];
    }
    _set(id, method, value) { return this.States[id][method](id, this.EventQueue, value); }
    SetUnused(id, value) { return this._set(id, "SetUnused", value); }
    SetHidden(id, value) { return this._set(id, "SetHidden", value); }
    SetDisabled(id, value) { return this._set(id, "SetDisabled", value); }
    SetHovered(id, value) { return this._set(id, "SetHovered", value); }
    SetFocused(id, value) { return this._set(id, "SetFocused", value); }
    SetPressed(id, value) { return this._set(id, "SetPressed", value); }
    SetDragging(id, value) { return this._set(id, "SetDragging", value); }
    SetSelected(id, value) { return this._set(id, "SetSelected", value); }
    SetUpdated(id, value) { return this._set(id, "SetUpdated", value); }
    SetAction(id, value) { return this._set(id, "SetAction", value); }
    IsUnused(id) { return this.States[id].IsUnused(); }
    IsHidden(id) { return this.States[id].IsHidden(); }
    IsDisabled(id) { return this.States[id].IsDisabled(); }
    IsHovered(id) { return this.States[id].IsHovered(); }
    IsFocused(id) { return this.States[id].IsFocused(); }
    IsPressed(id) { return this.States[id].IsPressed(); }
    IsDragging(id) { return this.States[id].IsDragging(); }
    IsSelected(id) { return this.States[id].IsSelected(); }
    IsUpdated(id) { return this.States[id].IsUpdated(); }
    IsAction(id) { return this.States[id].IsAction(); }
  }

  gpk.controlNCSpacing = placement => ({
    Left: placement.Border.Left + placement.Margin.Left,
    Top: placement.Border.Top + placement.Margin.Top,
    Right: placement.Border.Right + placement.Margin.Right,
    Bottom: placement.Border.Bottom + placement.Margin.Bottom
  });
  gpk.paletteIndexFromState = (disabled, state) => disabled ? gpk.GUI_CONTROL_PALETTE.DISABLED
    : state.IsDragging() ? gpk.GUI_CONTROL_PALETTE.DRAGGING
    : state.IsPressed() ? gpk.GUI_CONTROL_PALETTE.PRESSED
    : state.IsSelected() ? gpk.GUI_CONTROL_PALETTE.SELECTED
    : state.IsHovered() ? gpk.GUI_CONTROL_PALETTE.HOVER
    : gpk.GUI_CONTROL_PALETTE.NORMAL;

  gpk.controlInvalid = (controls, id) => !Number.isInteger(id) || id < 0 || id >= controls.States.length || controls.IsUnused(id);
  gpk.controlParent = (controls, id) => gpk.controlInvalid(controls, id) ? gpk.CID_INVALID : controls.States[id].Parent;
  gpk.controlHidden = function (controls, id) {
    if (gpk.controlInvalid(controls, id) || controls.States[id].IsHidden()) return true;
    const parent = controls.States[id].Parent;
    return !gpk.controlInvalid(controls, parent) && gpk.controlHidden(controls, parent);
  };
  gpk.controlDisabled = function (controls, id) {
    if (gpk.controlInvalid(controls, id) || controls.States[id].IsDisabled()) return true;
    const parent = controls.States[id].Parent;
    return !gpk.controlInvalid(controls, parent) && gpk.controlDisabled(controls, parent);
  };
  gpk.controlInstanceReset = function (controls, id, defaultColors) {
    controls.States[id] = new gpk.SControlState();
    controls.TextState[id] = [];
    controls.Identifier[id] = "";
    controls.Script[id] = "";
    controls.Constraints[id] = new SControlConstraints();
    controls.Placement[id] = new SControlPlacement();
    controls.Area[id] = new SControlArea();
    controls.Draw[id] = new SControlDraw(defaultColors);
    controls.Text[id] = new SControlText();
    controls.Images[id] = new SControlImage();
    controls.Children[id] = [];
    return id;
  };
  gpk.controlCreate = function (controls, defaultColors) {
    for (let id = 0; id < controls.States.length; ++id)
      if (controls.IsUnused(id)) return gpk.controlInstanceReset(controls, id, defaultColors);
    const id = controls.States.length;
    return gpk.controlInstanceReset(controls, id, defaultColors);
  };
  gpk.controlSetParent = function (controls, id, parent) {
    if (gpk.controlInvalid(controls, id)) throw new Error("Invalid control id: " + id);
    const state = controls.States[id];
    const oldParent = state.Parent;
    if (oldParent === parent) return 0;
    state.Parent = parent;
    if (!gpk.controlInvalid(controls, oldParent)) {
      const children = controls.Children[oldParent];
      const index = children.indexOf(id);
      if (index >= 0) children.splice(index, 1);
      controls.SetUpdated(oldParent, false);
    }
    if (gpk.controlInvalid(controls, parent)) {
      if (parent !== gpk.CID_INVALID) throw new Error("Invalid parent control id: " + parent);
      controls.SetUpdated(id, false);
      return 0;
    }
    if (!controls.Children[parent].includes(id)) controls.Children[parent].push(id);
    controls.SetUpdated(parent, false);
    controls.SetUpdated(id, false);
    return 0;
  };
  gpk.controlToFront = function (controls, id) {
    const parent = gpk.controlParent(controls, id);
    gpk.controlSetParent(controls, id, gpk.CID_INVALID);
    gpk.controlSetParent(controls, id, parent);
    return 0;
  };
  gpk.controlMetricsInvalidate = function controlMetricsInvalidate(controls, id) {
    controls.SetUpdated(id, false);
    for (const child of controls.Children[id]) controlMetricsInvalidate(controls, child);
    return 0;
  };
  gpk.controlTextSet = function (controls, id, text) {
    if (controls.Text[id].Text === text) return 0;
    controls.Text[id].Text = String(text);
    controls.SetUpdated(id, false);
    return 1;
  };
  gpk.controlFontSet = function (controls, id, font) {
    if (controls.Text[id].FontSelected === font) return 0;
    controls.Text[id].FontSelected = font;
    controls.SetUpdated(id, false);
    return 1;
  };
  gpk.controlDeselect = function (controls) {
    for (let id = 0; id < controls.States.length; ++id) controls.SetSelected(id, false);
    return 0;
  };
  gpk.controlSelect = function (controls, selected) {
    if (gpk.controlInvalid(controls, selected)) throw new Error("Invalid control id: " + selected);
    const parent = controls.States[selected].Parent;
    for (let id = 0; id < controls.States.length; ++id)
      if (controls.States[id].Parent === parent) controls.SetSelected(id, id === selected);
    return 0;
  };
  gpk.controlDelete = function controlDelete(controls, id, recursive = true) {
    if (gpk.controlInvalid(controls, id)) throw new Error("Invalid control id: " + id);
    if (recursive)
      for (const child of controls.Children[id].slice()) controlDelete(controls, child, true);
    const parent = controls.States[id].Parent;
    if (!gpk.controlInvalid(controls, parent)) {
      const children = controls.Children[parent];
      const index = children.indexOf(id);
      if (index >= 0) children.splice(index, 1);
    }
    controls.SetUnused(id, true);
    return 0;
  };
  gpk.controlActions = function (controls) {
    const output = [];
    for (let id = 0; id < controls.States.length; ++id)
      if (!controls.States[id].IsUnused() && !gpk.controlDisabled(controls, id) && !gpk.controlHidden(controls, id)) output.push(id);
    return output;
  };
  gpk.controlAction = function (controls) { const controlsToProcess = gpk.controlActions(controls); return controlsToProcess.length ? controlsToProcess[0] : 0; };
  gpk.controlHandle = function (controls, ids, callback) {
    if (typeof ids === "function") { callback = ids; ids = gpk.controlActions(controls); }
    for (const id of ids) if (controls.States[id].IsAction()) return callback(id);
    return 0;
  };

  Object.assign(gpk, {SControlPlacement, SControlDraw, SControlRectangle, SControlArea, SControlConstraints, SControlText, SControlImage, SControlTable});
})(globalThis.gpk);
