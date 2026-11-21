(function (gpk) {
  "use strict";

  const P = gpk.GUI_CONTROL_PALETTE;
  const A = gpk.UI_CONTROL_AREA;
  const color = gpk.rgba;
  const BLACK = color(0, 0, 0), BLUE = color(0, 0, 255), GREEN = color(0, 255, 0), CYAN = color(0, 255, 255);
  const RED = color(255, 0, 0), MAGENTA = color(255, 0, 255), YELLOW = color(255, 255, 0), WHITE = color(255, 255, 255);
  const GRAY = color(128, 128, 128), LIGHTGRAY = color(192, 192, 192), ORANGE = color(255, 165, 0), DARKCYAN = color(0, 128, 128);

  const blankCombo = () => Array.from({length: A.COUNT}, () => color());
  const blankIndexCombo = () => Array.from({length: A.COUNT}, () => 0);

  function shadeCombo(base, shade, shades) {
    const factor = (shades - shade) / shades;
    return base.map(value => gpk.colorScale(value, factor));
  }

  function setupDirectPalettes() {
    const output = {DefaultColors: Array(P.COUNT).fill(0), Palettes: []};
    const add = combo => output.Palettes.push(combo) - 1;
    let combo = blankCombo();
    Object.assign(combo, {[A.BACKGROUND]: BLUE, [A.CLIENT]: GRAY, [A.BORDER_LEFT]: YELLOW, [A.BORDER_TOP]: MAGENTA, [A.BORDER_RIGHT]: CYAN, [A.TEXT_BACKGROUND]: WHITE, [A.TEXT_FACE]: WHITE});
    output.DefaultColors[P.NORMAL] = add(combo);
    combo = blankCombo();
    Object.assign(combo, {[A.BACKGROUND]: LIGHTGRAY, [A.CLIENT]: ORANGE, [A.BORDER_LEFT]: YELLOW, [A.BORDER_TOP]: MAGENTA, [A.BORDER_RIGHT]: CYAN, [A.TEXT_BACKGROUND]: gpk.colorScale(LIGHTGRAY, 1.1), [A.TEXT_FACE]: gpk.colorScale(LIGHTGRAY, 1.1)});
    output.DefaultColors[P.DISABLED] = add(combo);
    combo = blankCombo();
    Object.assign(combo, {[A.BACKGROUND]: CYAN, [A.CLIENT]: YELLOW, [A.BORDER_LEFT]: MAGENTA, [A.BORDER_TOP]: GREEN, [A.BORDER_RIGHT]: ORANGE, [A.TEXT_BACKGROUND]: gpk.colorScale(DARKCYAN, 0.5), [A.TEXT_FACE]: gpk.colorScale(DARKCYAN, 0.5)});
    output.DefaultColors[P.HOVER] = add(combo);
    combo = blankCombo();
    Object.assign(combo, {[A.BACKGROUND]: RED, [A.CLIENT]: ORANGE, [A.BORDER_LEFT]: YELLOW, [A.BORDER_TOP]: MAGENTA, [A.BORDER_RIGHT]: CYAN, [A.TEXT_BACKGROUND]: WHITE, [A.TEXT_FACE]: WHITE});
    output.DefaultColors[P.PRESSED] = add(combo);
    output.DefaultColors[P.DRAGGING] = add(shadeCombo(output.Palettes[output.DefaultColors[P.NORMAL]], 1, 16));
    output.DefaultColors[P.SELECTED] = add(shadeCombo(output.Palettes[output.DefaultColors[P.NORMAL]], 8, 16));
    output.DefaultColors[P.SELECTED_DISABLED] = add(shadeCombo(output.Palettes[output.DefaultColors[P.DISABLED]], 8, 16));
    output.DefaultColors[P.SELECTED_HOVER] = add(shadeCombo(output.Palettes[output.DefaultColors[P.HOVER]], 8, 16));
    output.DefaultColors[P.SELECTED_PRESSED] = add(shadeCombo(output.Palettes[output.DefaultColors[P.PRESSED]], 8, 16));
    output.DefaultColors[P.SELECTED_DRAGGING] = add(shadeCombo(output.Palettes[output.DefaultColors[P.DRAGGING]], 8, 16));
    output.DefaultColors[P.OUTDATED] = add(output.Palettes[output.DefaultColors[P.NORMAL]].map(gpk.cloneColor));
    output.DefaultColors[P.HIDDEN] = output.DefaultColors[P.NORMAL];
    output.DefaultColors[P.SELECTED_HIDDEN] = 0;
    return output;
  }

  function setupLegacyPalette() {
    const vga = [
      color(0,0,0), color(0,0,170), color(0,170,0), color(0,170,170), color(170,0,0), color(170,0,170),
      color(170,85,0), color(170,170,170), color(85,85,85), color(85,85,255), color(85,255,85), color(85,255,255),
      color(255,85,85), color(255,85,255), color(255,255,85), color(255,255,255)
    ];
    const bases = vga.concat(vga.map(value => gpk.colorAdd(gpk.colorScale(value, 0.75), color(32,32,32,0))), [color(72,208,248), color(248,176,32), color(232,232,232)]);
    const palette = [];
    for (const base of bases)
      for (let shade = 0; shade < 16; ++shade) {
        const item = gpk.colorScale(base, (16 - shade) / 16);
        item.a = 255;
        palette.push(item);
      }
    return palette;
  }

  function setupLegacyThemes(palette) {
    const themes = [];
    for (let index = 0; index < palette.length; ++index) {
      const combos = Array.from({length: P.COUNT}, blankIndexCombo);
      const shade = index % 16;
      const text = 15 * 16;
      const selected = index + (shade < 8 ? 2 : -2);
      const dragging = index + (shade < 8 ? 1 : -1);
      const draggingShade = dragging % 16;
      const put = (state, background, edge, face) => {
        const value = combos[state];
        value[A.BACKGROUND] = background;
        value[A.CLIENT] = background;
        value[A.BORDER_LEFT] = value[A.BORDER_TOP] = value[A.BORDER_RIGHT] = value[A.BORDER_BOTTOM] = edge;
        value[A.TEXT_BACKGROUND] = background;
        value[A.TEXT_FACE] = face;
      };
      put(P.DISABLED, index, index + (shade > 7 ? -1 : 1), index + (shade > 7 ? -3 : 3));
      put(P.PRESSED, index, index + (shade > 7 ? -3 : 3), text + (shade > 7 ? 0 : 15));
      put(P.DRAGGING, dragging, dragging + (draggingShade > 7 ? -3 : 3), combos[P.PRESSED][A.TEXT_FACE]);
      put(P.SELECTED, selected, selected, text + (shade > 7 ? 4 : 12));
      put(P.HOVER, index, index + (shade > 7 ? -7 : 7), text + (shade > 7 ? 4 : 12));
      put(P.NORMAL, index, index + (shade > 7 ? -5 : 5), text + (shade > 7 ? 4 : 12));
      for (const combo of combos)
        for (let area = 0; area < combo.length; ++area) combo[area] = gpk.clamp(combo[area], 0, palette.length - 1);
      themes.push({ColorCombos: combos});
    }
    return themes;
  }

  gpk.guiSetupDefaultColors = function () {
    const direct = setupDirectPalettes();
    const palette = setupLegacyPalette();
    return {Palette: palette, ControlThemes: setupLegacyThemes(palette), DefaultColors: direct.DefaultColors, Palettes: direct.Palettes};
  };
})(globalThis.gpk);
