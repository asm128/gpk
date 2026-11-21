(function (gpk) {
  "use strict";

  class SGUIZoom {
    constructor() {
      this.DPI = {x: 1, y: 1};
      this.ZoomLimits = {Min: 0.1, Max: 10};
      this.ZoomLevel = 1;
    }
    clone() {
      const value = new SGUIZoom();
      value.DPI = gpk.cloneV2(this.DPI);
      value.ZoomLimits = {Min: this.ZoomLimits.Min, Max: this.ZoomLimits.Max};
      value.ZoomLevel = this.ZoomLevel;
      return value;
    }
  }

  class SGUI {
    constructor() {
      this.LastSize = gpk.v2();
      this.CursorPos = gpk.v2();
      this.Controls = new gpk.SControlTable();
      this.Colors = null;
      this.Fonts = [];
      this.LastZoom = new SGUIZoom();
      this.Zoom = new SGUIZoom();
      this.SelectedFont = 7;
      this.ThemeDefault = 3 * 16 + 8;
      this.ColorModeDefault = gpk.GUI_COLOR_MODE.Mode3D;
      this.InputModeTouch = true;
    }
  }

  const controlCreateTable = gpk.controlCreate;
  gpk.controlCreate = function (owner, defaultColors) {
    if (!(owner instanceof SGUI)) return controlCreateTable(owner, defaultColors);
    if (!owner.Colors) owner.Colors = gpk.guiSetupDefaultColors();
    if (owner.Fonts.length === 0) owner.Fonts = gpk.rasterFontDefaults();
    if (owner.SelectedFont >= owner.Fonts.length) owner.SelectedFont = owner.Fonts.length - 1;
    const id = controlCreateTable(owner.Controls, owner.Colors.DefaultColors);
    if (typeof defaultColors === "function") {
      const result = defaultColors(owner.Controls.States[id], owner.Controls.TextState[id], owner.Controls.Identifier[id], owner.Controls.Constraints[id], owner.Controls.Placement[id], owner.Controls.Area[id], owner.Controls.Draw[id], owner.Controls.Text[id], owner.Controls.Images[id]);
      if (result < 0) { gpk.controlDelete(owner.Controls, id); return -1; }
    }
    return id;
  };
  gpk.controlCreateChild = function (gui, parent) {
    const id = gpk.controlCreate(gui);
    gpk.controlSetParent(gui.Controls, id, parent);
    return id;
  };
  gpk.guiSetupDefaultFonts = function (gui) { gui.Fonts = gpk.rasterFontDefaults(); gpk.guiUpdateMetrics(gui, gui.LastSize, true); return 0; };
  gpk.guiSelect = (gui, id) => gpk.controlSelect(gui.Controls, id);
  gpk.guiDeselect = gui => gpk.controlDeselect(gui.Controls);
  gpk.guiGetProcessableControls = gui => gpk.controlActions(gui.Controls);
  gpk.guiProcessControls = (gui, ids, callback) => typeof ids === "function" ? gpk.controlHandle(gui.Controls, ids) : gpk.controlHandle(gui.Controls, ids, callback);

  function identifiedAncestor(gui, id) {
    for (let parent = gui.Controls.States[id].Parent; !gpk.controlInvalid(gui.Controls, parent); parent = gui.Controls.States[parent].Parent)
      if (gui.Controls.Identifier[parent].length) return true;
    return false;
  }
  function identifiedRoot(gui, id) {
    let root = gui.Controls.Identifier[id].length ? id : gpk.CID_INVALID;
    for (let parent = gui.Controls.States[id].Parent; !gpk.controlInvalid(gui.Controls, parent); parent = gui.Controls.States[parent].Parent)
      if (gui.Controls.Identifier[parent].length) root = parent;
    return root;
  }
  function serializableState(value) { return {Parent: value.Parent, Flag: value.Flag, Mask: value.Mask}; }
  function serializableTextState(values) { return values.map(serializableState); }
  function controlJSONSave(gui, id) {
    const output = {
      Id: id,
      State: serializableState(gui.Controls.States[id]),
      TextState: serializableTextState(gui.Controls.TextState[id]),
      Constraints: gpk.deepClone(gui.Controls.Constraints[id]),
      Placement: gpk.deepClone(gui.Controls.Placement[id]),
      Draw: gpk.deepClone(gui.Controls.Draw[id]),
      Text: gpk.deepClone(gui.Controls.Text[id])
    };
    const visit = parent => {
      for (const child of gui.Controls.Children[parent]) {
        if (gpk.controlInvalid(gui.Controls, child)) continue;
        const identifier = gui.Controls.Identifier[child];
        if (identifier) output[identifier] = controlJSONSave(gui, child);
        else visit(child);
      }
    };
    visit(id);
    return output;
  }
  gpk.guiJSONSave = function (gui) {
    const output = {};
    for (let id = 0; id < gui.Controls.States.length; ++id) {
      if (gpk.controlInvalid(gui.Controls, id) || !gui.Controls.Identifier[id] || identifiedAncestor(gui, id)) continue;
      output[gui.Controls.Identifier[id]] = controlJSONSave(gui, id);
    }
    return output;
  };

  function patchObject(target, patch) {
    if (!patch || typeof patch !== "object") return false;
    let changed = false;
    for (const key of Object.keys(patch)) {
      if (!(key in target)) continue;
      const next = patch[key];
      if (Array.isArray(target[key]) && Array.isArray(next)) {
        target[key].length = 0;
        for (const item of next) target[key].push(item && typeof item === "object" && "Flag" in item ? new gpk.STextState(item.Parent, item.Flag, item.Mask) : gpk.deepClone(item));
        changed = true;
      }
      else if (target[key] && typeof target[key] === "object" && next && typeof next === "object")
        changed = patchObject(target[key], next) || changed;
      else if (target[key] !== next) {
        target[key] = next;
        changed = true;
      }
    }
    return changed;
  }
  function controlJSONLoad(gui, id, input) {
    let loaded = false;
    if (input.State) {
      const oldParent = gui.Controls.States[id].Parent;
      const state = gui.Controls.States[id].clone();
      patchObject(state, input.State);
      if (state.Parent !== oldParent) gpk.controlSetParent(gui.Controls, id, state.Parent);
      gui.Controls.States[id] = state;
      loaded = true;
    }
    if (input.TextState) {
      gui.Controls.TextState[id] = input.TextState.map(value => new gpk.STextState(value.Parent, value.Flag, value.Mask));
      loaded = true;
    }
    loaded = patchObject(gui.Controls.Constraints[id], input.Constraints) || loaded;
    loaded = patchObject(gui.Controls.Placement[id], input.Placement) || loaded;
    loaded = patchObject(gui.Controls.Draw[id], input.Draw) || loaded;
    loaded = patchObject(gui.Controls.Text[id], input.Text) || loaded;
    if (loaded) gpk.controlMetricsInvalidate(gui.Controls, id);
    const visit = parent => {
      for (const child of gui.Controls.Children[parent]) {
        if (gpk.controlInvalid(gui.Controls, child)) continue;
        const identifier = gui.Controls.Identifier[child];
        if (identifier && Object.prototype.hasOwnProperty.call(input, identifier)) controlJSONLoad(gui, child, input[identifier]);
        else if (!identifier) visit(child);
      }
    };
    visit(id);
    return loaded ? 1 : 0;
  }
  gpk.guiJSONLoad = function (gui, input) {
    if (typeof input === "string") input = JSON.parse(input);
    if (!input || typeof input !== "object" || Array.isArray(input)) throw new Error("GUI patch root must be an object");
    for (let id = 0; id < gui.Controls.States.length; ++id) {
      const identifier = gui.Controls.Identifier[id];
      if (gpk.controlInvalid(gui.Controls, id) || !identifier || identifiedAncestor(gui, id)) continue;
      if (Object.prototype.hasOwnProperty.call(input, identifier)) controlJSONLoad(gui, id, input[identifier]);
    }
    return 0;
  };

  function templateDynamic(value, rootIdentifier, result) {
    if (typeof value === "string") {
      const tokens = gpk.expressionStringTokens(value);
      for (const token of tokens) {
        if (token.Type !== gpk.STRIP_LITERAL_TYPE.TOKEN) continue;
        result.dynamic = true;
        const expression = token.Text;
        if (!rootIdentifier || !(expression === rootIdentifier || expression.startsWith(rootIdentifier + ".") || expression.startsWith(rootIdentifier + "["))) result.scoped = false;
      }
    }
    else if (value && typeof value === "object")
      for (const child of Object.values(value)) templateDynamic(child, rootIdentifier, result);
  }
  function templateResolve(value, values) {
    if (Array.isArray(value)) return value.map(item => templateResolve(item, values));
    if (value && typeof value === "object") {
      const output = {};
      for (const [key, child] of Object.entries(value)) output[key] = templateResolve(child, values);
      return output;
    }
    if (typeof value !== "string") return value;
    const tokens = gpk.expressionStringTokens(value);
    if (tokens.length === 1 && tokens[0].Type === gpk.STRIP_LITERAL_TYPE.TOKEN && tokens[0].Span.Begin === 0 && tokens[0].Span.End === value.length)
      return gpk.deepClone(gpk.jsonExpressionResolve(tokens[0].Text, values));
    return gpk.jsonStringFormat(value, values);
  }
  gpk.guiScriptRun = function (gui, id) {
    if (gpk.controlInvalid(gui.Controls, id)) throw new Error("Invalid control id: " + id);
    const script = gui.Controls.Script[id];
    if (!script) return 0;
    const source = typeof script === "string" ? JSON.parse(script) : gpk.deepClone(script);
    const root = identifiedRoot(gui, id);
    const rootIdentifier = gpk.controlInvalid(gui.Controls, root) ? "" : gui.Controls.Identifier[root];
    const state = {dynamic: false, scoped: true};
    templateDynamic(source, rootIdentifier, state);
    if (!state.dynamic) return gpk.guiJSONLoad(gui, source);
    const values = state.scoped ? {[rootIdentifier]: controlJSONSave(gui, root)} : gpk.guiJSONSave(gui);
    return gpk.guiJSONLoad(gui, templateResolve(source, values));
  };

  function buildControlGeometry(placement, metrics) {
    const area = gpk.GUI_CONTROL_AREA_OLD;
    const total = metrics.Total.Global;
    metrics.Rectangles[area.CLIENT] = gpk.cloneRect2(metrics.Client.Global);
    metrics.Rectangles[area.BORDER_LEFT] = gpk.rect2(total.Offset.x, total.Offset.y, placement.Border.Left, total.Size.y);
    metrics.Rectangles[area.BORDER_TOP] = gpk.rect2(total.Offset.x, total.Offset.y, total.Size.x, placement.Border.Top);
    metrics.Rectangles[area.BORDER_RIGHT] = gpk.rect2(total.Offset.x + total.Size.x - placement.Border.Right, total.Offset.y, placement.Border.Right, total.Size.y);
    metrics.Rectangles[area.BORDER_BOTTOM] = gpk.rect2(total.Offset.x, total.Offset.y + total.Size.y - placement.Border.Bottom, total.Size.x, placement.Border.Bottom);
    const client = metrics.Rectangles[area.CLIENT];
    client.Offset.x -= placement.Margin.Left;
    client.Offset.y -= placement.Margin.Top;
    client.Size.x += placement.Margin.Left + placement.Margin.Right;
    client.Size.y += placement.Margin.Top + placement.Margin.Bottom;
    const point = (x, y) => ({x, y});
    let x = total.Offset.x, y = total.Offset.y;
    metrics.Triangles[0] = [point(x,y), point(x + placement.Border.Left, y + placement.Border.Top), point(x + placement.Border.Left, y)];
    metrics.Triangles[1] = [point(x,y), point(x, y + placement.Border.Top), point(x + placement.Border.Left, y + placement.Border.Top)];
    x = total.Offset.x + total.Size.x - placement.Border.Right;
    metrics.Triangles[2] = [point(x,y), point(x, y + placement.Border.Top), point(x + placement.Border.Right, y)];
    metrics.Triangles[3] = [point(x + placement.Border.Right,y), point(x, y + placement.Border.Top), point(x + placement.Border.Right, y + placement.Border.Top)];
    x = total.Offset.x; y = total.Offset.y + total.Size.y - placement.Border.Bottom;
    metrics.Triangles[4] = [point(x,y), point(x, y + placement.Border.Bottom), point(x + placement.Border.Left, y)];
    metrics.Triangles[5] = [point(x + placement.Border.Right,y), point(x, y + placement.Border.Top), point(x + placement.Border.Right, y + placement.Border.Top)];
    x = total.Offset.x + total.Size.x - placement.Border.Right;
    y = total.Offset.y + total.Size.y - placement.Border.Bottom;
    metrics.Triangles[6] = [point(x,y), point(x + placement.Border.Right, y + placement.Border.Bottom), point(x + placement.Border.Right, y)];
    metrics.Triangles[7] = [point(x,y), point(x, y + placement.Border.Bottom), point(x + placement.Border.Right, y + placement.Border.Bottom)];
  }

  function controlDockTarget(gui, target, side) {
    for (let count = 0; count < gui.Controls.States.length && target !== gpk.CID_INVALID; ++count) {
      if (target < 0 || target >= gui.Controls.States.length) return target;
      if (!gpk.controlHidden(gui.Controls, target)) return target;
      const dock = gui.Controls.Constraints[target].DockToControl;
      target = side === gpk.GUI_CONTROL_LIMIT.Left ? dock.Left : side === gpk.GUI_CONTROL_LIMIT.Top ? dock.Top : side === gpk.GUI_CONTROL_LIMIT.Right ? dock.Right : dock.Bottom;
    }
    return gpk.CID_INVALID;
  }

  function controlUpdateMetrics(gui, id, targetSize) {
    if (gpk.controlInvalid(gui.Controls, id)) throw new Error("Invalid control id: " + id);
    const state = gui.Controls.States[id];
    const placement = gui.Controls.Placement[id];
    const scale = {x: gui.Zoom.DPI.x * gui.Zoom.ZoomLevel, y: gui.Zoom.DPI.y * gui.Zoom.ZoomLevel};
    if (Math.abs(1 - scale.x) < 0.001) scale.x = 1;
    if (Math.abs(1 - scale.y) < 0.001) scale.y = 1;
    const position = {x: placement.Area.Offset.x * scale.x, y: placement.Area.Offset.y * scale.y};
    const size = {x: placement.Area.Size.x * scale.x, y: placement.Area.Size.y * scale.y};
    const metrics = gui.Controls.Area[id];
    const text = gui.Controls.Text[id];
    const fontIndex = text.FontSelected >= 0 && text.FontSelected < gui.Fonts.length ? text.FontSelected : gui.SelectedFont;
    const font = gui.Fonts[fontIndex];
    const textSize = {x: font.CharSize.x * text.Text.length * scale.x, y: font.CharSize.y * scale.y};
    const constraints = gui.Controls.Constraints[id];
    const nc = gpk.controlNCSpacing(placement);
    const ncScaled = {Left: nc.Left * scale.x, Top: nc.Top * scale.y, Right: nc.Right * scale.x, Bottom: nc.Bottom * scale.y};
    const ncTotal = {x: ncScaled.Left + ncScaled.Right, y: ncScaled.Top + ncScaled.Bottom};
    if (constraints.AttachSizeToText.x) size.x = textSize.x + ncTotal.x;
    if (constraints.AttachSizeToText.y) size.y = textSize.y + ncTotal.y;
    const hasParent = !gpk.controlInvalid(gui.Controls, state.Parent);
    const available = hasParent ? gui.Controls.Area[state.Parent].Client.Global.Size : targetSize;
    if (constraints.AttachSizeToControl.x === id) { if (constraints.DockToControl.Left === -1) position.x = 0; size.x = available.x; }
    else if (!gpk.controlInvalid(gui.Controls, constraints.AttachSizeToControl.x)) { if (constraints.DockToControl.Left === -1) position.x = 0; size.x = gui.Controls.Area[constraints.AttachSizeToControl.x].Total.Global.Size.x; }
    if (constraints.AttachSizeToControl.y === id) { if (constraints.DockToControl.Top === -1) position.y = 0; size.y = available.y; }
    else if (!gpk.controlInvalid(gui.Controls, constraints.AttachSizeToControl.y)) { if (constraints.DockToControl.Top === -1) position.y = 0; size.y = gui.Controls.Area[constraints.AttachSizeToControl.y].Total.Global.Size.y; }
    metrics.Client.Local = gpk.rect2(Math.trunc(ncScaled.Left), Math.trunc(ncScaled.Top), Math.trunc(size.x - ncTotal.x), Math.trunc(size.y - ncTotal.y));
    size.x = gpk.clamp(size.x, constraints.SizeMinMax.Min.x * scale.x, constraints.SizeMinMax.Max.x * scale.x);
    size.y = gpk.clamp(size.y, constraints.SizeMinMax.Min.y * scale.y, constraints.SizeMinMax.Max.y * scale.y);
    metrics.Total.Local = gpk.realignRectangle(available, gpk.rect2(Math.trunc(position.x), Math.trunc(position.y), Math.trunc(size.x), Math.trunc(size.y)), placement.Align);
    metrics.Total.Global = gpk.cloneRect2(metrics.Total.Local);
    metrics.Client.Global = gpk.cloneRect2(metrics.Client.Local);
    metrics.Client.Global.Offset.x += metrics.Total.Local.Offset.x;
    metrics.Client.Global.Offset.y += metrics.Total.Local.Offset.y;
    if (hasParent) {
      const parent = gui.Controls.Area[state.Parent].Client.Global.Offset;
      metrics.Total.Global.Offset.x += parent.x; metrics.Total.Global.Offset.y += parent.y;
      metrics.Client.Global.Offset.x += parent.x; metrics.Client.Global.Offset.y += parent.y;
    }
    const dock = constraints.DockToControl;
    let dockId;
    if (dock.Right !== -1 && (dockId = controlDockTarget(gui, dock.Right, gpk.GUI_CONTROL_LIMIT.Right)) !== gpk.CID_INVALID) {
      gpk.controlUpdateMetricsTopToDown(gui, dockId, available, true);
      const otherPlacement = gui.Controls.Placement[dockId], other = gui.Controls.Area[dockId];
      if (gpk.bitTrue(otherPlacement.Align, gpk.ALIGN.RIGHT) && gpk.bitFalse(otherPlacement.Align, gpk.ALIGN.HCENTER)) {
        metrics.Total.Global.Offset.x = other.Total.Global.Offset.x - metrics.Total.Global.Size.x;
        metrics.Client.Global.Offset.x = Math.trunc(metrics.Total.Global.Offset.x + ncScaled.Left);
        const difference = available.x - other.Total.Local.Offset.x;
        if (constraints.AttachSizeToControl.x === id) {
          metrics.Total.Global.Size.x -= difference; metrics.Client.Global.Size.x -= difference;
          metrics.Total.Global.Offset.x += difference; metrics.Client.Global.Offset.x += difference;
        }
      }
      else {
        metrics.Total.Global.Offset.x = other.Total.Global.Offset.x + other.Total.Global.Size.x;
        metrics.Client.Global.Offset.x = Math.trunc(metrics.Total.Global.Offset.x + ncScaled.Left);
      }
    }
    if (dock.Bottom !== -1 && (dockId = controlDockTarget(gui, dock.Bottom, gpk.GUI_CONTROL_LIMIT.Bottom)) !== gpk.CID_INVALID) {
      const otherPlacement = gui.Controls.Placement[dockId], other = gui.Controls.Area[dockId];
      metrics.Total.Global.Offset.y = gpk.bitTrue(otherPlacement.Align, gpk.ALIGN.BOTTOM) && gpk.bitFalse(otherPlacement.Align, gpk.ALIGN.VCENTER)
        ? other.Total.Global.Offset.y - metrics.Total.Global.Size.y
        : other.Total.Global.Offset.y + other.Total.Global.Size.y;
      metrics.Client.Global.Offset.y = Math.trunc(metrics.Total.Global.Offset.y + ncScaled.Top);
    }
    if (dock.Left !== -1 && (dockId = controlDockTarget(gui, dock.Left, gpk.GUI_CONTROL_LIMIT.Left)) !== gpk.CID_INVALID) {
      gpk.controlUpdateMetricsTopToDown(gui, dockId, available, true);
      const otherPlacement = gui.Controls.Placement[dockId], other = gui.Controls.Area[dockId];
      metrics.Total.Global.Offset.x = gpk.bitTrue(otherPlacement.Align, gpk.ALIGN.RIGHT) && gpk.bitFalse(otherPlacement.Align, gpk.ALIGN.HCENTER)
        ? other.Total.Global.Offset.x + other.Total.Global.Size.x - metrics.Total.Global.Size.x
        : other.Total.Global.Offset.x;
      metrics.Client.Global.Offset.x = Math.trunc(metrics.Total.Global.Offset.x + ncScaled.Left);
    }
    if (dock.Top !== -1 && (dockId = controlDockTarget(gui, dock.Top, gpk.GUI_CONTROL_LIMIT.Top)) !== gpk.CID_INVALID) {
      const otherPlacement = gui.Controls.Placement[dockId], other = gui.Controls.Area[dockId];
      metrics.Total.Global.Offset.y = gpk.bitTrue(otherPlacement.Align, gpk.ALIGN.BOTTOM) && gpk.bitFalse(otherPlacement.Align, gpk.ALIGN.VCENTER)
        ? other.Total.Global.Offset.y - metrics.Total.Global.Size.y
        : other.Total.Global.Offset.y + metrics.Total.Global.Offset.y;
      metrics.Client.Global.Offset.y = metrics.Total.Global.Offset.y + nc.Top;
    }
    metrics.Text = gpk.realignRectangle(metrics.Client.Global.Size, gpk.rect2(0, 0, Math.trunc(textSize.x), Math.trunc(textSize.y)), text.Align);
    metrics.Text.Offset.x += metrics.Client.Global.Offset.x;
    metrics.Text.Offset.y += metrics.Client.Global.Offset.y;
    buildControlGeometry(placement, metrics);
    gui.Controls.SetUpdated(id, true);
    return 0;
  }

  gpk.controlUpdateMetricsTopToDown = function controlUpdateMetricsTopToDown(gui, id, targetSize, forceUpdate) {
    if (!gui.Controls.IsUpdated(id) || forceUpdate) controlUpdateMetrics(gui, id, targetSize);
    for (const child of gui.Controls.Children[id]) controlUpdateMetricsTopToDown(gui, child, targetSize, forceUpdate);
    return 0;
  };
  gpk.guiUpdateMetrics = function (gui, targetSize, forceUpdate = false) {
    for (let id = 0; id < gui.Controls.States.length; ++id)
      if (!gpk.controlInvalid(gui.Controls, id) && gpk.controlInvalid(gui.Controls, gui.Controls.States[id].Parent))
        gpk.controlUpdateMetricsTopToDown(gui, id, targetSize, forceUpdate);
    return 0;
  };
  gpk.guiUpdate = function (gui, targetSize) {
    if (gui.LastSize.x !== targetSize.x || gui.LastSize.y !== targetSize.y || gui.LastZoom.DPI.x !== gui.Zoom.DPI.x || gui.LastZoom.DPI.y !== gui.Zoom.DPI.y || gui.LastZoom.ZoomLevel !== gui.Zoom.ZoomLevel) {
      for (let id = 0; id < gui.Controls.States.length; ++id) gui.Controls.SetUpdated(id, false);
      gui.LastSize = gpk.cloneV2(targetSize);
      gui.LastZoom = gui.Zoom.clone();
    }
    return gpk.guiUpdateMetrics(gui, targetSize, false);
  };

  function updateHovered(gui, id, state, input, disabled) {
    if (state.IsHovered()) {
      if (input.ButtonDown(0) && !state.IsPressed()) gui.Controls.SetPressed(id, true);
      else if (input.ButtonUp(0)) { gui.Controls.SetPressed(id, false); gui.Controls.SetDragging(id, false); }
    }
    else if (!disabled) gui.Controls.SetHovered(id, true);
    return state.IsHovered() ? id : -1;
  }
  function controlProcessInput(gui, input, id) {
    const state = gui.Controls.States[id];
    let hovered = -1, branch = -1;
    if (state.IsPressed() && input.MouseCurrent.ButtonState[0]) {
      if (input.MouseCurrent.Position.x !== input.MousePrevious.Position.x || input.MouseCurrent.Position.y !== input.MousePrevious.Position.y) gui.Controls.SetDragging(id, true);
      return id;
    }
    for (const child of gui.Controls.Children[id]) {
      if (gpk.controlHidden(gui.Controls, child)) continue;
      const pressed = controlProcessInput(gui, input, child);
      if (!gpk.controlInvalid(gui.Controls, pressed)) {
        gui.Controls.SetPressed(id, false);
        hovered = pressed;
        branch = child;
      }
    }
    if (input.ButtonDown(0) && gpk.controlInvalid(gui.Controls, state.Parent) && !gpk.controlInvalid(gui.Controls, branch)) gpk.controlToFront(gui.Controls, branch);
    if (hovered === -1 && gpk.inRange(gui.CursorPos, gui.Controls.Area[id].Total.Global))
      hovered = updateHovered(gui, id, state, input, gpk.controlDisabled(gui.Controls, id) || gpk.controlHidden(gui.Controls, id));
    else {
      gui.Controls.SetHovered(id, false);
      if (input.ButtonUp(0)) { gui.Controls.SetPressed(id, false); gui.Controls.SetDragging(id, false); }
    }
    return hovered;
  }
  gpk.guiProcessInput = function (gui, input) {
    if (gui.LastSize.x * gui.LastSize.x + gui.LastSize.y * gui.LastSize.y === 0) return 0;
    gui.Controls.EventQueue.length = 0;
    gpk.guiUpdateMetrics(gui, gui.LastSize, false);
    const roots = [];
    for (let id = 0; id < gui.Controls.States.length; ++id) {
      const state = gui.Controls.States[id];
      if (state.IsUnused()) continue;
      gui.Controls.SetAction(id, false);
      if (state.IsDisabled() || gpk.controlHidden(gui.Controls, id) || !gpk.controlInvalid(gui.Controls, state.Parent)) continue;
      roots.push(id);
    }
    gui.CursorPos = gpk.cloneV2(input.MouseCurrent.Position);
    let hovered = -1;
    for (const root of roots) {
      if (gpk.controlHidden(gui.Controls, root)) continue;
      const result = controlProcessInput(gui, input, root);
      if (!gpk.controlInvalid(gui.Controls, result)) hovered = result;
    }
    for (let id = 0; id < gui.Controls.States.length; ++id) {
      if (input.ButtonDown(0)) gui.Controls.SetFocused(id, id === hovered);
      if (id !== hovered) {
        gui.Controls.SetHovered(id, false);
        if (!input.MouseCurrent.ButtonState[0]) { gui.Controls.SetPressed(id, false); gui.Controls.SetDragging(id, false); }
      }
      const state = gui.Controls.States[id];
      if (state.IsDragging() && gpk.controlInvalid(gui.Controls, state.Parent)) {
        const delta = {x: (input.MouseCurrent.Position.x - input.MousePrevious.Position.x) / gui.Zoom.DPI.x / gui.Zoom.ZoomLevel, y: (input.MouseCurrent.Position.y - input.MousePrevious.Position.y) / gui.Zoom.DPI.y / gui.Zoom.ZoomLevel};
        if (delta.x || delta.y) {
          const placement = gui.Controls.Placement[id];
          placement.Area.Offset.x += (placement.Align & gpk.ALIGN.HCENTER) === gpk.ALIGN.RIGHT ? -delta.x : delta.x;
          placement.Area.Offset.y += (placement.Align & gpk.ALIGN.VCENTER) === gpk.ALIGN.BOTTOM ? -delta.y : delta.y;
          gpk.controlMetricsInvalidate(gui.Controls, id);
        }
      }
    }
    for (let id = 0; id < gui.Controls.States.length; ++id)
      if (gui.Controls.States[id].IsAction() && gui.Controls.Script[id]) gpk.guiScriptRun(gui, id);
    return hovered;
  };

  gpk.createScreenLayout = function (gui, parent = gpk.CID_INVALID) {
    const id = gpk.controlCreate(gui);
    gui.Controls.Placement[id].Border = gpk.rect4();
    gui.Controls.Placement[id].Margin = gpk.rect4();
    gui.Controls.Constraints[id].AttachSizeToControl = {x: id, y: id};
    gui.Controls.Draw[id].NoClient = 1;
    gui.Controls.States[id].Mask &= ~gpk.GUI_CONTROL_FLAG.Hovered;
    if (parent !== gpk.CID_INVALID) gpk.controlSetParent(gui.Controls, id, parent);
    return id;
  };
  gpk.guiCreateControlList = function (gui, direction, labels, parent, args, textAlign, output = []) {
    let first = -1;
    for (let index = 0; index < labels.length; ++index) {
      const id = gpk.controlCreate(gui);
      output.push(id);
      if (index === 0) first = id;
      const placement = gui.Controls.Placement[id];
      patchObject(placement, gpk.deepClone(args));
      if (direction & (gpk.DIRECTION.LEFT | gpk.DIRECTION.RIGHT)) placement.Area.Offset.x += args.Area.Size.x * index;
      else placement.Area.Offset.y += args.Area.Size.y * index;
      gui.Controls.Text[id].Text = labels[index];
      gui.Controls.Text[id].Align = textAlign;
      const constraints = gui.Controls.Constraints[id];
      if (direction & (gpk.DIRECTION.UP | gpk.DIRECTION.DOWN)) constraints.AttachSizeToControl.x = parent;
      else if (direction & (gpk.DIRECTION.LEFT | gpk.DIRECTION.RIGHT)) { constraints.AttachSizeToText.x = true; constraints.AttachSizeToControl.y = parent; }
      if (index) {
        if (direction & gpk.DIRECTION.LEFT) constraints.DockToControl.Left = output[index - 1];
        else if (direction & gpk.DIRECTION.RIGHT) constraints.DockToControl.Right = output[index - 1];
        else if (direction & gpk.DIRECTION.UP) constraints.DockToControl.Top = output[index - 1];
        else if (direction & gpk.DIRECTION.DOWN) constraints.DockToControl.Bottom = output[index - 1];
      }
      gpk.controlSetParent(gui.Controls, id, parent);
    }
    return first;
  };

  Object.assign(gpk, {SGUIZoom, SGUI});
})(globalThis.gpk);
