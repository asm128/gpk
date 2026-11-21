(function (gpk) {
  "use strict";

  gpk.realignCoord = function (targetSize, source, align) {
    const output = gpk.cloneV2(source);
    if (gpk.bitTrue(align, gpk.ALIGN.HCENTER)) output.x += targetSize.x >> 1;
    else if (gpk.bitTrue(align, gpk.ALIGN.RIGHT)) output.x = targetSize.x - 1 - source.x;
    if (gpk.bitTrue(align, gpk.ALIGN.VCENTER)) output.y += targetSize.y >> 1;
    else if (gpk.bitTrue(align, gpk.ALIGN.BOTTOM)) output.y = targetSize.y - 1 - source.y;
    return output;
  };

  gpk.realignRectangle = function (targetSize, source, align) {
    const output = gpk.cloneRect2(source);
    if (gpk.bitTrue(align, gpk.ALIGN.HCENTER))
      output.Offset.x += (targetSize.x >> 1) - (output.Size.x >> 1);
    else if (gpk.bitTrue(align, gpk.ALIGN.RIGHT))
      output.Offset.x = targetSize.x - (output.Size.x + output.Offset.x);
    if (gpk.bitTrue(align, gpk.ALIGN.VCENTER))
      output.Offset.y += (targetSize.y >> 1) - (output.Size.y >> 1);
    else if (gpk.bitTrue(align, gpk.ALIGN.BOTTOM))
      output.Offset.y = targetSize.y - (output.Size.y + output.Offset.y);
    return output;
  };
})(globalThis.gpk);
