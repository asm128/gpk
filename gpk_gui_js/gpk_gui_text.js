(function (gpk) {
  "use strict";
  gpk.textLineCalcWidth = (sizeCharCell, text) => sizeCharCell.x * String(text).length;
  gpk.textLineRaster = function (targetMetrics, font, targetRect, text) {
    const output = [];
    for (let character = 0; character < text.length; ++character)
      for (let y = 0; y < font.CharSize.y; ++y)
        for (let x = 0; x < font.CharSize.x; ++x) {
          const px = targetRect.Offset.x + character * font.CharSize.x + x, py = targetRect.Offset.y + y;
          if (font.glyphPixel(text.charCodeAt(character), x, y) && px >= 0 && py >= 0 && px < targetMetrics.x && py < targetMetrics.y) output.push({x:px,y:py});
        }
    return output;
  };
})(globalThis.gpk);
