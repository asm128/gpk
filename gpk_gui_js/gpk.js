(function (global) {
  "use strict";

  const gpk = global.gpk = global.gpk || {};

  gpk.CID_INVALID = -1;
  gpk.v2 = (x = 0, y = 0) => ({x, y});
  gpk.cloneV2 = value => ({x: value.x, y: value.y});
  gpk.rect2 = (x = 0, y = 0, width = 0, height = 0) => ({Offset: {x, y}, Size: {x: width, y: height}});
  gpk.cloneRect2 = value => ({Offset: gpk.cloneV2(value.Offset), Size: gpk.cloneV2(value.Size)});
  gpk.rect4 = (left = 0, top = 0, right = 0, bottom = 0) => ({Left: left, Top: top, Right: right, Bottom: bottom});
  gpk.cloneRect4 = value => ({Left: value.Left, Top: value.Top, Right: value.Right, Bottom: value.Bottom});
  gpk.rectLimit = value => ({x: value.Offset.x + value.Size.x, y: value.Offset.y + value.Size.y});
  gpk.rectArea = value => Math.max(0, value.Size.x) * Math.max(0, value.Size.y);
  gpk.inRange = (point, value) => point.x >= value.Offset.x && point.y >= value.Offset.y && point.x < value.Offset.x + value.Size.x && point.y < value.Offset.y + value.Size.y;
  gpk.clamp = (value, minimum, maximum) => Math.max(minimum, Math.min(maximum, value));
  gpk.bitTrue = (value, mask) => (value & mask) === mask;
  gpk.bitFalse = (value, mask) => (value & mask) !== mask;
  gpk.setBitMasked = (state, mask, bit, value) => {
    const masked = mask & bit;
    if (masked === 0) return state;
    return value ? (state | masked) : (state & ~masked);
  };
  gpk.deepClone = value => value === undefined ? undefined : JSON.parse(JSON.stringify(value));

  gpk.AXIS = Object.freeze({
    ORIGIN: 0x00,
    X_NEGATIVE: 0x01,
    X_POSITIVE: 0x02,
    Y_NEGATIVE: 0x04,
    Y_POSITIVE: 0x08,
    Z_NEGATIVE: 0x10,
    Z_POSITIVE: 0x20
  });
  gpk.ALIGN = Object.freeze({
    ORIGIN: 0x00,
    LEFT: 0x01,
    RIGHT: 0x02,
    HCENTER: 0x03,
    TOP: 0x04,
    BOTTOM: 0x08,
    VCENTER: 0x0C,
    NEAR: 0x10,
    FAR: 0x20,
    DCENTER: 0x30,
    TOP_LEFT: 0x05,
    TOP_RIGHT: 0x06,
    BOTTOM_LEFT: 0x09,
    BOTTOM_RIGHT: 0x0A,
    CENTER_TOP: 0x07,
    CENTER_BOTTOM: 0x0B,
    CENTER_LEFT: 0x0D,
    CENTER_RIGHT: 0x0E,
    CENTER: 0x0F
  });
  gpk.DIRECTION = Object.freeze({BACK: 0x01, FRONT: 0x02, DOWN: 0x04, UP: 0x08, LEFT: 0x10, RIGHT: 0x20});

  gpk.rgba = (r = 0, g = 0, b = 0, a = 255) => ({r, g, b, a});
  gpk.cloneColor = value => ({r: value.r, g: value.g, b: value.b, a: value.a});
  gpk.colorScale = (value, scale) => ({
    r: gpk.clamp(Math.round(value.r * scale), 0, 255),
    g: gpk.clamp(Math.round(value.g * scale), 0, 255),
    b: gpk.clamp(Math.round(value.b * scale), 0, 255),
    a: value.a
  });
  gpk.colorAdd = (left, right) => ({
    r: gpk.clamp(Math.round(left.r + right.r), 0, 255),
    g: gpk.clamp(Math.round(left.g + right.g), 0, 255),
    b: gpk.clamp(Math.round(left.b + right.b), 0, 255),
    a: gpk.clamp(Math.round(left.a + right.a), 0, 255)
  });
  gpk.colorEquals = (left, right) => left.r === right.r && left.g === right.g && left.b === right.b && left.a === right.a;

  gpk.assert = (condition, message) => {
    if (!condition)
      throw new Error(message || "Assertion failed");
  };
})(globalThis);
