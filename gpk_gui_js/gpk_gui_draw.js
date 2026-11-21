(function (gpk) {
  "use strict";

  class PixelTarget {
    constructor(width, height) { this.resize(width || 1, height || 1); }
    resize(width, height) {
      this.width = Math.max(1, Math.trunc(width));
      this.height = Math.max(1, Math.trunc(height));
      this.data = new Uint8ClampedArray(this.width * this.height * 4);
      return this;
    }
    clear(value = gpk.rgba()) {
      this.data[0] = value.r; this.data[1] = value.g; this.data[2] = value.b; this.data[3] = value.a;
      for (let copied = 4; copied < this.data.length; copied <<= 1)
        this.data.copyWithin(copied, 0, Math.min(copied, this.data.length - copied));
    }
    pixel(x, y, value, blend = false) {
      x = Math.trunc(x); y = Math.trunc(y);
      if (x < 0 || y < 0 || x >= this.width || y >= this.height) return;
      const offset = (y * this.width + x) * 4;
      if (!blend || value.a === 255) {
        this.data[offset] = value.r; this.data[offset + 1] = value.g; this.data[offset + 2] = value.b; this.data[offset + 3] = value.a;
        return;
      }
      const alpha = value.a / 255, inverse = 1 - alpha;
      this.data[offset] = Math.round(value.r * alpha + this.data[offset] * inverse);
      this.data[offset + 1] = Math.round(value.g * alpha + this.data[offset + 1] * inverse);
      this.data[offset + 2] = Math.round(value.b * alpha + this.data[offset + 2] * inverse);
      this.data[offset + 3] = Math.round(value.a + this.data[offset + 3] * inverse);
    }
  }

  gpk.drawRectangle = function (target, value, rectangle) {
    const x0 = Math.max(0, Math.trunc(rectangle.Offset.x));
    const y0 = Math.max(0, Math.trunc(rectangle.Offset.y));
    const x1 = Math.min(target.width, Math.trunc(rectangle.Offset.x + Math.max(0, rectangle.Size.x)));
    const y1 = Math.min(target.height, Math.trunc(rectangle.Offset.y + Math.max(0, rectangle.Size.y)));
    for (let y = y0; y < y1; ++y) for (let x = x0; x < x1; ++x) target.pixel(x, y, value);
  };

  gpk.drawTriangle = function (target, value, triangle) {
    const [a, b, c] = triangle;
    const minimumX = Math.max(0, Math.floor(Math.min(a.x, b.x, c.x)));
    const maximumX = Math.min(target.width - 1, Math.ceil(Math.max(a.x, b.x, c.x)));
    const minimumY = Math.max(0, Math.floor(Math.min(a.y, b.y, c.y)));
    const maximumY = Math.min(target.height - 1, Math.ceil(Math.max(a.y, b.y, c.y)));
    const edge = (p, q, x, y) => (x - p.x) * (q.y - p.y) - (y - p.y) * (q.x - p.x);
    const winding = edge(a, b, c.x, c.y) < 0 ? -1 : 1;
    for (let y = minimumY; y <= maximumY; ++y)
      for (let x = minimumX; x <= maximumX; ++x)
        if (edge(a,b,x,y) * winding >= 0 && edge(b,c,x,y) * winding >= 0 && edge(c,a,x,y) * winding >= 0) target.pixel(x, y, value);
  };

  function textColors(gui, draw, paletteIndex) {
    const area = gpk.UI_CONTROL_AREA;
    if (!draw.UseNewPalettes) {
      const theme = gui.Colors.ControlThemes[draw.ColorTheme === 0 ? gui.ThemeDefault : draw.ColorTheme - 1];
      const combo = theme.ColorCombos[paletteIndex];
      return {face: gui.Colors.Palette[combo[area.TEXT_FACE]], back: gui.Colors.Palette[combo[area.TEXT_BACKGROUND]]};
    }
    const combo = gui.Colors.Palettes[draw.Palettes[paletteIndex]];
    return {face: combo[area.TEXT_FACE], back: combo[area.TEXT_BACKGROUND]};
  }

  function controlTextDraw(gui, id, target, disabled) {
    const state = gui.Controls.States[id], draw = gui.Controls.Draw[id];
    const normal = textColors(gui, draw, gpk.paletteIndexFromState(disabled, state));
    const palette = disabled ? gpk.GUI_CONTROL_PALETTE.SELECTED_DISABLED
      : state.IsDragging() ? gpk.GUI_CONTROL_PALETTE.SELECTED_DRAGGING
      : state.IsPressed() ? gpk.GUI_CONTROL_PALETTE.SELECTED_PRESSED
      : state.IsHovered() ? gpk.GUI_CONTROL_PALETTE.SELECTED_HOVER
      : gpk.GUI_CONTROL_PALETTE.SELECTED;
    const selected = textColors(gui, draw, palette);
    const rectangle = gpk.cloneRect2(gui.Controls.Area[id].Text);
    if (state.IsPressed() || state.IsHovered()) {
      rectangle.Offset.x += 1; rectangle.Offset.y += 1;
      rectangle.Size.x = Math.max(0, rectangle.Size.x - 1); rectangle.Size.y = Math.max(0, rectangle.Size.y - 1);
    }
    const controlText = gui.Controls.Text[id];
    const fontIndex = controlText.FontSelected >= 0 && controlText.FontSelected < gui.Fonts.length ? controlText.FontSelected : gui.SelectedFont;
    const font = gui.Fonts[fontIndex], textState = gui.Controls.TextState[id];
    const count = Math.min(textState.length, controlText.Text.length);
    for (let character = 0; character < count; ++character)
      if (textState[character].IsSelected()) gpk.drawRectangle(target, selected.back, gpk.rect2(rectangle.Offset.x + font.CharSize.x * character, rectangle.Offset.y, font.CharSize.x, font.CharSize.y));
    for (let character = 0; character < controlText.Text.length; ++character) {
      const glyphColor = character < textState.length && textState[character].IsSelected() ? selected.face : normal.face;
      for (let y = 0; y < font.CharSize.y; ++y)
        for (let x = 0; x < font.CharSize.x; ++x)
          if (font.glyphPixel(controlText.Text.charCodeAt(character), x, y)) {
            const px = rectangle.Offset.x + character * font.CharSize.x + x, py = rectangle.Offset.y + y;
            if (px >= rectangle.Offset.x && py >= rectangle.Offset.y && px < rectangle.Offset.x + rectangle.Size.x && py < rectangle.Offset.y + rectangle.Size.y) target.pixel(px, py, glyphColor);
          }
    }
  }

  function fillColors(gui, id, disabled) {
    const state = gui.Controls.States[id], draw = gui.Controls.Draw[id], old = gpk.GUI_CONTROL_AREA_OLD, area = gpk.UI_CONTROL_AREA;
    const output = Array.from({length: old.COUNT}, () => gpk.rgba());
    const mode = draw.ColorMode === gpk.GUI_COLOR_MODE.Default ? gui.ColorModeDefault : draw.ColorMode;
    const paletteIndex = gpk.paletteIndexFromState(disabled, state);
    let combo;
    if (draw.UseNewPalettes) combo = gui.Colors.Palettes[draw.Palettes[paletteIndex]];
    else {
      const theme = gui.Colors.ControlThemes[draw.ColorTheme === 0 ? gui.ThemeDefault : draw.ColorTheme - 1];
      combo = theme.ColorCombos[paletteIndex].map(index => gui.Colors.Palette[index]);
    }
    output[old.CLIENT] = combo[area.CLIENT];
    if (mode === gpk.GUI_COLOR_MODE.Mode3D || mode === gpk.GUI_COLOR_MODE.Default) {
      const bright = state.IsPressed() ? 0.4 : state.IsHovered() ? 0.8 : 1.2;
      const shaded = state.IsPressed() ? 1.5 : state.IsHovered() ? 1.2 : 0.8;
      output[old.BORDER_LEFT] = gpk.colorScale(output[old.CLIENT], bright);
      output[old.BORDER_TOP] = gpk.colorScale(output[old.CLIENT], bright);
      output[old.BORDER_RIGHT] = gpk.colorScale(output[old.CLIENT], shaded);
      output[old.BORDER_BOTTOM] = gpk.colorScale(output[old.CLIENT], shaded);
    }
    else {
      output[old.BORDER_LEFT] = combo[area.BORDER_LEFT]; output[old.BORDER_TOP] = combo[area.BORDER_TOP];
      output[old.BORDER_RIGHT] = combo[area.BORDER_RIGHT]; output[old.BORDER_BOTTOM] = combo[area.BORDER_BOTTOM];
    }
    return output;
  }

  function drawImage(target, image, rectangle, sourceOffset, invertY) {
    if (!image || !image.width || !image.height || !image.data) return;
    for (let y = 0; y < rectangle.Size.y; ++y) for (let x = 0; x < rectangle.Size.x; ++x) {
      const sourceX = sourceOffset.x + x;
      const sourceYUnflipped = sourceOffset.y + y;
      const sourceY = invertY ? image.height - 1 - sourceYUnflipped : sourceYUnflipped;
      if (sourceX < 0 || sourceY < 0 || sourceX >= image.width || sourceY >= image.height) continue;
      const offset = (sourceY * image.width + sourceX) * 4;
      target.pixel(rectangle.Offset.x + x, rectangle.Offset.y + y, {r: image.data[offset], g: image.data[offset + 1], b: image.data[offset + 2], a: image.data[offset + 3]}, true);
    }
  }

  function actualControlDraw(gui, id, target) {
    const disabled = gpk.controlDisabled(gui.Controls, id), metrics = gui.Controls.Area[id], draw = gui.Controls.Draw[id];
    const colors = fillColors(gui, id, disabled), old = gpk.GUI_CONTROL_AREA_OLD;
    for (let element = 0; element < old.COUNT; ++element) {
      if ((draw.NoBorder && element !== old.CLIENT) || (draw.NoClient && element === old.CLIENT)) continue;
      gpk.drawRectangle(target, colors[element], metrics.Rectangles[element]);
    }
    const indices = [old.BORDER_TOP, old.BORDER_LEFT, old.BORDER_TOP, old.BORDER_RIGHT, old.BORDER_LEFT, old.BORDER_BOTTOM, old.BORDER_RIGHT, old.BORDER_BOTTOM];
    const placement = gui.Controls.Placement[id], nc = gpk.controlNCSpacing(placement);
    if (!draw.NoBorder && (nc.Left + nc.Right) * (nc.Top + nc.Bottom))
      for (let index = 0; index < 8; ++index) gpk.drawTriangle(target, colors[indices[index]], metrics.Triangles[index]);
    const controlImage = gui.Controls.Images[id];
    if (controlImage.Image && controlImage.Image.width && controlImage.Image.height) {
      let rectangle = gpk.realignRectangle(metrics.Client.Global.Size, gpk.rect2(0, 0, controlImage.Image.width, controlImage.Image.height), controlImage.ImageAlign);
      rectangle.Offset.x += metrics.Client.Global.Offset.x; rectangle.Offset.y += metrics.Client.Global.Offset.y;
      rectangle.Size.x = Math.min(rectangle.Offset.x + rectangle.Size.x, metrics.Client.Global.Offset.x + metrics.Client.Global.Size.x) - rectangle.Offset.x;
      rectangle.Size.y = Math.min(rectangle.Offset.y + rectangle.Size.y, metrics.Client.Global.Offset.y + metrics.Client.Global.Size.y) - rectangle.Offset.y;
      if (gui.Controls.States[id].IsHovered()) { rectangle.Offset.x += 1; rectangle.Offset.y += 1; }
      drawImage(target, controlImage.Image, rectangle, controlImage.ImageOffset, controlImage.ImageInvertY !== 0);
    }
    controlTextDraw(gui, id, target, disabled);
  }

  gpk.controlDrawHierarchy = function controlDrawHierarchy(gui, id, target) {
    if (gpk.controlInvalid(gui.Controls, id)) throw new Error("Invalid control id: " + id);
    if (!gpk.controlHidden(gui.Controls, id)) {
      actualControlDraw(gui, id, target);
      for (const child of gui.Controls.Children[id]) controlDrawHierarchy(gui, child, target);
    }
    return 0;
  };
  gpk.guiDraw = function (gui, target) {
    if (gui.LastSize.x !== target.width || gui.LastSize.y !== target.height) {
      for (let id = 0; id < gui.Controls.States.length; ++id) gui.Controls.SetUpdated(id, false);
      gui.LastSize = {x:target.width, y:target.height};
    }
    for (let id = 0; id < gui.Controls.States.length; ++id)
      if (!gpk.controlInvalid(gui.Controls, id) && gpk.controlInvalid(gui.Controls, gui.Controls.States[id].Parent)) gpk.controlDrawHierarchy(gui, id, target);
    return 0;
  };

  gpk.PixelTarget = PixelTarget;
})(globalThis.gpk);
