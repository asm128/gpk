(function (gpk) {
  "use strict";

  gpk.GUI_TYPE = Object.freeze({ControlList: 0, ControlGrid: 1, Slider: 2, Dropdown: 3, Keyboard: 4, COUNT: 5});
  const ncTotal = placement => { const value = gpk.controlNCSpacing(placement); return {x: value.Left + value.Right, y: value.Top + value.Bottom}; };

  gpk.guiCreateKeyboard = function (gui, system, rowWidth = gpk.GUI_KEYBOARD_Default_row_width, keys = gpk.GUI_KEYBOARD_Default_keys) {
    if (!rowWidth) throw new Error("Keyboard row width must be non-zero");
    const root = gpk.controlCreate(gui), font = gui.Fonts[gui.SelectedFont];
    const keySize = {x: font.CharSize.x + 6, y: font.CharSize.y + 6}, actionWidth = 88;
    gui.Controls.Placement[root].Area.Size = {x: keySize.x * rowWidth + 4 + actionWidth, y: keySize.y * Math.floor(keys.length / rowWidth) + 4};
    gui.Controls.Placement[root].Align = gpk.ALIGN.CENTER_BOTTOM;
    for (let index = 0; index < keys.length; ++index) {
      const id = gpk.controlCreate(gui), placement = gui.Controls.Placement[id];
      placement.Area.Size = gpk.cloneV2(keySize); placement.Area.Offset = {x: (index % rowWidth) * keySize.x, y: Math.floor(index / rowWidth) * keySize.y};
      placement.Border = gpk.rect4(1,1,1,1); gui.Controls.Text[id].Align = gpk.ALIGN.CENTER; gui.Controls.Text[id].Text = String.fromCharCode(keys[index]);
      gpk.controlSetParent(gui.Controls, id, root);
    }
    const actionIds = [];
    const args = new gpk.SControlPlacement(); args.Area.Size = {x: actionWidth, y: 20}; args.Align = gpk.ALIGN.TOP_RIGHT;
    gpk.guiCreateControlList(gui, 0, gpk.VK_SCANCODE_LABELS, root, args, gpk.ALIGN.CENTER, actionIds);
    system[gpk.GUI_TYPE.Keyboard].push(root);
    return root;
  };

  function gridMetrics(gui, grid, direction) {
    if (gpk.controlInvalid(gui.Controls, grid)) return {x: 0, y: 0};
    const lists = gui.Controls.Children[grid]; if (!lists.length) return {x: 0, y: 0};
    let capacity = 0; for (const list of lists) capacity = Math.max(capacity, gui.Controls.Children[list].length);
    const horizontal = direction === gpk.DIRECTION.RIGHT || direction === gpk.DIRECTION.LEFT;
    return horizontal ? {x: capacity, y: lists.length} : {x: lists.length, y: capacity};
  }
  gpk.guiControlGridControl = function (gui, grid, direction, cellOrIndex) {
    const lists = gui.Controls.Children[grid] || [];
    if (typeof cellOrIndex === "number") {
      let index = cellOrIndex;
      for (const list of lists) { const controls = gui.Controls.Children[list]; if (index < controls.length) return controls[index]; index -= controls.length; }
      return -1;
    }
    const metrics = gridMetrics(gui, grid, direction), cell = cellOrIndex;
    if (cell.x >= metrics.x || cell.y >= metrics.y) return -1;
    const horizontal = direction === gpk.DIRECTION.RIGHT || direction === gpk.DIRECTION.LEFT;
    const listIndex = horizontal ? cell.y : cell.x;
    let controlIndex = horizontal ? cell.x : cell.y;
    if (direction === gpk.DIRECTION.LEFT) controlIndex = metrics.x - 1 - cell.x;
    else if (direction === gpk.DIRECTION.UP) controlIndex = metrics.y - 1 - cell.y;
    return lists[listIndex] === undefined || gui.Controls.Children[lists[listIndex]][controlIndex] === undefined ? -1 : gui.Controls.Children[lists[listIndex]][controlIndex];
  };
  gpk.guiControlGridArrange = function (gui, grid, direction, cellSize, spacing = gpk.v2(), offset = gpk.v2(), align = gpk.ALIGN.TOP_LEFT) {
    const metrics = gridMetrics(gui, grid, direction);
    const gridSize = {x: metrics.x * cellSize.x + Math.max(0, metrics.x - 1) * spacing.x, y: metrics.y * cellSize.y + Math.max(0, metrics.y - 1) * spacing.y};
    const gridPlacement = gui.Controls.Placement[grid]; gridPlacement.Area = {Offset: gpk.cloneV2(offset), Size: gridSize}; gridPlacement.Align = align;
    const horizontal = direction === gpk.DIRECTION.RIGHT || direction === gpk.DIRECTION.LEFT, lists = gui.Controls.Children[grid];
    for (let listIndex = 0; listIndex < lists.length; ++listIndex) {
      const list = lists[listIndex], listPlacement = gui.Controls.Placement[list];
      listPlacement.Area = horizontal ? gpk.rect2(0, listIndex * (cellSize.y + spacing.y), gridSize.x, cellSize.y) : gpk.rect2(listIndex * (cellSize.x + spacing.x), 0, cellSize.x, gridSize.y);
      listPlacement.Align = gpk.ALIGN.TOP_LEFT; gui.Controls.Draw[list].NoClient = 1; gui.Controls.States[list].Mask &= ~gpk.GUI_CONTROL_FLAG.Hovered;
      const controls = gui.Controls.Children[list];
      for (let index = 0; index < controls.length; ++index) {
        const id = controls[index], placement = gui.Controls.Placement[id], constraints = gui.Controls.Constraints[id];
        let cell = horizontal ? {x: index, y: 0} : {x: 0, y: index};
        if (direction === gpk.DIRECTION.LEFT) cell.x = metrics.x - 1 - index;
        else if (direction === gpk.DIRECTION.UP) cell.y = metrics.y - 1 - index;
        placement.Area = gpk.rect2(cell.x * (cellSize.x + spacing.x), cell.y * (cellSize.y + spacing.y), cellSize.x, cellSize.y);
        placement.Align = gpk.ALIGN.TOP_LEFT; constraints.AttachSizeToControl = {x: -1, y: -1}; constraints.DockToControl = {Left: -1, Top: -1, Right: -1, Bottom: -1};
      }
    }
    return gpk.controlMetricsInvalidate(gui.Controls, grid);
  };
  gpk.guiControlListInitialize = function (gui, parent = -1) {
    const root = gpk.controlCreate(gui); gui.Controls.Placement[root].Border = gpk.rect4(); gui.Controls.Placement[root].Margin = gpk.rect4();
    gui.Controls.Text[root].Text = " "; gui.Controls.Constraints[root].AttachSizeToText = {x: false, y: false};
    if (parent !== -1) gpk.controlSetParent(gui.Controls, root, parent); return root;
  };
  gpk.guiControlListArrange = function (gui, root, direction) {
    const rootText = gui.Controls.Text[root], font = gui.Fonts[rootText.FontSelected >= 0 && rootText.FontSelected < gui.Fonts.length ? rootText.FontSelected : gui.SelectedFont];
    const controls = gui.Controls.Children[root]; let previous = -1;
    for (const id of controls) {
      const constraints = gui.Controls.Constraints[id], placement = gui.Controls.Placement[id], text = gui.Controls.Text[id], nc = ncTotal(placement);
      constraints.DockToControl = {Left: -1, Top: -1, Right: -1, Bottom: -1}; constraints.AttachSizeToControl = {x: -1, y: -1};
      if (gui.Controls.States[id].IsHidden()) continue;
      placement.Area.Size.y = font.CharSize.y + nc.y;
      if (direction === gpk.DIRECTION.UP || direction === gpk.DIRECTION.DOWN) { constraints.AttachSizeToControl.x = root; placement.Area.Offset.y = 0; }
      else { placement.Area.Size.x = font.CharSize.x * text.Text.length + nc.x; placement.Area.Offset.x = 0; }
      if (previous !== -1) {
        if (direction === gpk.DIRECTION.LEFT) constraints.DockToControl.Left = previous;
        else if (direction === gpk.DIRECTION.UP) constraints.DockToControl.Top = previous;
        else if (direction === gpk.DIRECTION.DOWN) constraints.DockToControl.Bottom = previous;
        else constraints.DockToControl.Right = previous;
      }
      previous = id;
    }
    const placement = gui.Controls.Placement[root]; placement.Area.Size = gpk.v2();
    for (const id of controls) {
      if (gui.Controls.States[id].IsHidden()) continue;
      const item = gui.Controls.Placement[id], text = gui.Controls.Text[id];
      if (direction === gpk.DIRECTION.UP || direction === gpk.DIRECTION.DOWN) { placement.Area.Size.y += item.Area.Size.y; placement.Area.Size.x = Math.max(placement.Area.Size.x, text.Text.length * font.CharSize.x + ncTotal(item).x); }
      else { placement.Area.Size.x += item.Area.Size.x; placement.Area.Size.y = Math.max(placement.Area.Size.y, item.Area.Size.y); }
    }
    if (direction === gpk.DIRECTION.UP || direction === gpk.DIRECTION.DOWN) placement.Area.Size.x += 16;
    return 0;
  };
  gpk.guiControlListPush = function (gui, root, direction, text) {
    const placement = new gpk.SControlPlacement(); placement.Margin = gpk.rect4(4,2,4,2);
    const ids = []; gpk.guiCreateControlList(gui, direction, [String(text)], root, placement, gpk.ALIGN.CENTER_LEFT, ids);
    gui.Controls.Constraints[ids[0]].AttachSizeToText = {x: false, y: false}; gpk.guiControlListArrange(gui, root, direction); return ids[0];
  };
  gpk.guiControlListCreate = function (gui, direction, labels, parent = -1) {
    const root = gpk.guiControlListInitialize(gui, parent); for (const label of labels) gpk.guiControlListPush(gui, root, direction, label); return root;
  };
  gpk.guiControlGridCreate = function (gui, labels, parent, direction, columnCount, cellSize, spacing = gpk.v2(), offset = gpk.v2(), align = gpk.ALIGN.TOP_LEFT, textAlign = gpk.ALIGN.CENTER) {
    const grid = gpk.controlCreate(gui), placement = gui.Controls.Placement[grid]; placement.Border = gpk.rect4(); placement.Margin = gpk.rect4();
    gui.Controls.Draw[grid].NoClient = 1; gui.Controls.States[grid].Mask &= ~gpk.GUI_CONTROL_FLAG.Hovered; if (parent !== -1) gpk.controlSetParent(gui.Controls, grid, parent);
    const rows = Math.ceil(labels.length / columnCount), horizontal = direction === gpk.DIRECTION.RIGHT || direction === gpk.DIRECTION.LEFT;
    const listCount = horizontal ? rows : columnCount, capacity = horizontal ? columnCount : rows;
    for (let list = 0; list < listCount; ++list) {
      const root = gpk.guiControlListCreate(gui, direction, labels.slice(list * capacity, Math.min(labels.length, (list + 1) * capacity)), grid);
      for (const id of gui.Controls.Children[root]) gui.Controls.Text[id].Align = textAlign;
    }
    gpk.guiControlGridArrange(gui, grid, direction, cellSize, spacing, offset, align); return grid;
  };
  gpk.guiCreateDropdown = function (gui, system, labels, anchor, args, textAlign) {
    const parent = gpk.controlParent(gui.Controls, anchor); if (gpk.controlInvalid(gui.Controls, parent)) throw new Error("Dropdown anchor requires a parent");
    for (const sibling of gui.Controls.Children[parent]) {
      const constraints = gui.Controls.Constraints[sibling];
      if (constraints.DockToControl.Left === anchor && constraints.DockToControl.Bottom === anchor) { if (!system[gpk.GUI_TYPE.Dropdown].includes(sibling)) system[gpk.GUI_TYPE.Dropdown].push(sibling); return sibling; }
    }
    const dropdown = gpk.controlCreateChild(gui, parent), placement = gui.Controls.Placement[dropdown], constraints = gui.Controls.Constraints[dropdown];
    placement.Border = gpk.rect4(); placement.Margin = gpk.rect4(); placement.Area.Size = {x: args.Area.Size.x, y: args.Area.Size.y * labels.length};
    constraints.DockToControl.Left = anchor; constraints.DockToControl.Bottom = anchor; gui.Controls.Draw[dropdown].NoClient = 1;
    gui.Controls.States[dropdown].Mask &= ~gpk.GUI_CONTROL_FLAG.Hovered; gui.Controls.States[dropdown].Mask &= ~gpk.GUI_CONTROL_FLAG.Action;
    gpk.guiCreateControlList(gui, gpk.DIRECTION.DOWN, labels, dropdown, args, textAlign, []); gui.Controls.SetHidden(dropdown, true); system[gpk.GUI_TYPE.Dropdown].push(dropdown); return dropdown;
  };
  gpk.dropdownExpand = function (gui, dropdown) {
    const parent = gpk.controlParent(gui.Controls, dropdown), anchor = gui.Controls.Constraints[dropdown].DockToControl.Bottom;
    gpk.controlSetParent(gui.Controls, dropdown, anchor); gpk.controlSetParent(gui.Controls, dropdown, parent); gui.Controls.SetHidden(dropdown, false); return gpk.controlMetricsInvalidate(gui.Controls, dropdown);
  };
  gpk.dropdownCollapse = function (gui, dropdown) { gui.Controls.SetHidden(dropdown, true); return 0; };
})(globalThis.gpk);
