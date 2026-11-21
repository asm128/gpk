(function (gpk) {
  "use strict";

  const textEncoder = new TextEncoder();
  const textDecoder = new TextDecoder();
  const SYSTEM_EVENT_GUI = 8;
  const EVENT_GUI_CONTROL_STATE_CHANGE = 2;

  class ByteWriter {
    constructor() { this.Buffer = new Uint8Array(256); this.Length = 0; }
    ensure(count) {
      const required = this.Length + count;
      if (required <= this.Buffer.length) return;
      let capacity = this.Buffer.length;
      while (capacity < required) capacity *= 2;
      const replacement = new Uint8Array(capacity);
      replacement.set(this.Buffer);
      this.Buffer = replacement;
    }
    u8(value) { this.ensure(1); this.Buffer[this.Length++] = value & 0xFF; }
    u16(value) { this.ensure(2); new DataView(this.Buffer.buffer).setUint16(this.Length, value, true); this.Length += 2; }
    i16(value) { this.ensure(2); new DataView(this.Buffer.buffer).setInt16(this.Length, value, true); this.Length += 2; }
    u32(value) { this.ensure(4); new DataView(this.Buffer.buffer).setUint32(this.Length, value, true); this.Length += 4; }
    bytes(value) { this.ensure(value.length); this.Buffer.set(value, this.Length); this.Length += value.length; }
    packedU32(value) {
      value = Number(value);
      if (!Number.isInteger(value) || value < 0 || value > 0x3FFFFFFF) throw new RangeError("packed_uint<u2_t> value out of range: " + value);
      const tailWidth = value > 0x3FFFFF ? 3 : value > 0x3FFF ? 2 : value > 0x3F ? 1 : 0;
      const multiplier = Math.floor(value / (2 ** (tailWidth * 8)));
      this.u8((multiplier << 2) | tailWidth);
      for (let byte = 0; byte < tailWidth; ++byte)
        this.u8(Math.floor(value / (2 ** (byte * 8))) & 0xFF);
    }
    string(value) {
      const bytes = textEncoder.encode(String(value || ""));
      this.packedU32(bytes.length);
      this.bytes(bytes);
    }
    finish() { return this.Buffer.slice(0, this.Length); }
  }

  class ByteReader {
    constructor(input) {
      if (input instanceof ArrayBuffer) input = new Uint8Array(input);
      if (!ArrayBuffer.isView(input)) throw new TypeError("SControlTable.Load() requires an ArrayBuffer view");
      this.Input = new Uint8Array(input.buffer, input.byteOffset, input.byteLength);
      this.View = new DataView(input.buffer, input.byteOffset, input.byteLength);
      this.Offset = 0;
    }
    require(count) {
      if (this.Offset + count > this.Input.length)
        throw new RangeError("Truncated SControlTable input at byte " + this.Offset + "; need " + count + " byte(s)");
    }
    u8() { this.require(1); return this.Input[this.Offset++]; }
    u16() { this.require(2); const value = this.View.getUint16(this.Offset, true); this.Offset += 2; return value; }
    i16() { this.require(2); const value = this.View.getInt16(this.Offset, true); this.Offset += 2; return value; }
    u32() { this.require(4); const value = this.View.getUint32(this.Offset, true); this.Offset += 4; return value; }
    bytes(count) { this.require(count); const value = this.Input.subarray(this.Offset, this.Offset + count); this.Offset += count; return value; }
    packedU32() {
      const first = this.u8();
      const tailWidth = first & 0x03;
      const multiplier = first >>> 2;
      this.require(tailWidth);
      let tail = 0;
      for (let byte = 0; byte < tailWidth; ++byte)
        tail += this.u8() * (2 ** (byte * 8));
      return multiplier * (2 ** (tailWidth * 8)) + tail;
    }
    string() { return textDecoder.decode(this.bytes(this.packedU32())); }
    remaining() { return this.Input.subarray(this.Offset); }
  }

  function requireParallelArrays(controls) {
    const count = controls.States.length;
    for (const name of ["TextState", "Identifier", "Script", "Constraints", "Placement", "Area", "Draw", "Text", "Images", "Children"])
      if (controls[name].length !== count) throw new Error("SControlTable parallel array size mismatch: " + name);
    return count;
  }

  function writeState(writer, value) { writer.i16(value.Parent); writer.u16(value.Flag); writer.u16(value.Mask); }
  function readState(reader, Type) { return new Type(reader.i16(), reader.u16(), reader.u16()); }
  function writeConstraints(writer, value) {
    writer.i16(value.AttachSizeToControl.x); writer.i16(value.AttachSizeToControl.y);
    writer.u8(value.AttachSizeToText.x ? 1 : 0); writer.u8(value.AttachSizeToText.y ? 1 : 0);
    writer.i16(value.DockToControl.Left); writer.i16(value.DockToControl.Top); writer.i16(value.DockToControl.Right); writer.i16(value.DockToControl.Bottom);
    writer.i16(value.SizeMinMax.Min.x); writer.i16(value.SizeMinMax.Min.y); writer.i16(value.SizeMinMax.Max.x); writer.i16(value.SizeMinMax.Max.y);
  }
  function readConstraints(reader) {
    const value = new gpk.SControlConstraints();
    value.AttachSizeToControl = {x: reader.i16(), y: reader.i16()};
    value.AttachSizeToText = {x: reader.u8() !== 0, y: reader.u8() !== 0};
    value.DockToControl = {Left: reader.i16(), Top: reader.i16(), Right: reader.i16(), Bottom: reader.i16()};
    value.SizeMinMax = {Min: {x: reader.i16(), y: reader.i16()}, Max: {x: reader.i16(), y: reader.i16()}};
    return value;
  }
  function writePlacement(writer, value) {
    writer.i16(value.Area.Offset.x); writer.i16(value.Area.Offset.y); writer.i16(value.Area.Size.x); writer.i16(value.Area.Size.y);
    writer.u16(value.Margin.Left); writer.u16(value.Margin.Top); writer.u16(value.Margin.Right); writer.u16(value.Margin.Bottom);
    writer.u16(value.Border.Left); writer.u16(value.Border.Top); writer.u16(value.Border.Right); writer.u16(value.Border.Bottom);
    writer.u8(value.Align);
    writer.u8((value.RelativeBorderSize & 0x0F) | ((value.RelativeMarginSize & 0x0F) << 4));
    writer.u16(value.RelativeMetrics);
  }
  function readPlacement(reader) {
    const value = new gpk.SControlPlacement();
    value.Area = gpk.rect2(reader.i16(), reader.i16(), reader.i16(), reader.i16());
    value.Margin = gpk.rect4(reader.u16(), reader.u16(), reader.u16(), reader.u16());
    value.Border = gpk.rect4(reader.u16(), reader.u16(), reader.u16(), reader.u16());
    value.Align = reader.u8();
    const relativeSizes = reader.u8();
    value.RelativeBorderSize = relativeSizes & 0x0F;
    value.RelativeMarginSize = relativeSizes >>> 4;
    value.RelativeMetrics = reader.u16();
    return value;
  }
  function writeDraw(writer, value) {
    writer.u8((value.ColorMode & 0x03) | ((value.FrameOut ? 1 : 0) << 2) | ((value.NoBorder ? 1 : 0) << 3) | ((value.NoClient ? 1 : 0) << 4) | ((value.UseNewPalettes ? 1 : 0) << 5));
    writer.i16(value.ColorTheme);
    for (let index = 0; index < gpk.GUI_CONTROL_PALETTE.COUNT; ++index) writer.i16(value.Palettes[index]);
  }
  function readDraw(reader) {
    const value = new gpk.SControlDraw();
    const flags = reader.u8();
    value.ColorMode = flags & 0x03;
    value.FrameOut = (flags >>> 2) & 1;
    value.NoBorder = (flags >>> 3) & 1;
    value.NoClient = (flags >>> 4) & 1;
    value.UseNewPalettes = (flags >>> 5) & 1;
    value.ColorTheme = reader.i16();
    value.Palettes = Array.from({length: gpk.GUI_CONTROL_PALETTE.COUNT}, () => reader.i16());
    return value;
  }
  function writeV2I16(writer, value) { writer.i16(value.x); writer.i16(value.y); }
  function readV2I16(reader) { return {x: reader.i16(), y: reader.i16()}; }
  function writeRect2I16(writer, value) { writeV2I16(writer, value.Offset); writeV2I16(writer, value.Size); }
  function readRect2I16(reader) { return {Offset: readV2I16(reader), Size: readV2I16(reader)}; }
  function writeControlRectangle(writer, value) { writeRect2I16(writer, value.Local); writeRect2I16(writer, value.Global); }
  function readControlRectangle(reader) { return {Local: readRect2I16(reader), Global: readRect2I16(reader)}; }
  function writeArea(writer, value) {
    writeControlRectangle(writer, value.Total);
    writeControlRectangle(writer, value.Client);
    writeRect2I16(writer, value.Text);
    for (const rectangle of value.Rectangles) writeRect2I16(writer, rectangle);
    for (const triangle of value.Triangles) for (const point of triangle) writeV2I16(writer, point);
  }
  function readArea(reader) {
    const value = new gpk.SControlArea();
    value.Total = readControlRectangle(reader);
    value.Client = readControlRectangle(reader);
    value.Text = readRect2I16(reader);
    value.Rectangles = Array.from({length: gpk.GUI_CONTROL_AREA_OLD.COUNT}, () => readRect2I16(reader));
    value.Triangles = Array.from({length: 8}, () => Array.from({length: 3}, () => readV2I16(reader)));
    return value;
  }
  function writeImageMetrics(writer, value) {
    const width = value.Image ? Math.trunc(value.Image.width || 0) : 0;
    const height = value.Image ? Math.trunc(value.Image.height || 0) : 0;
    if (width < 0 || width > 0xFFFF || height < 0 || height > 0xFFFF)
      throw new RangeError("Control-image metrics exceed n2u2_t");
    writer.u16(width); writer.u16(height);
  }
  function writeEvent(writer, event) {
    if (event == null) {
      writer.u8(0); writer.packedU32(0);
      return;
    }
    if (event && event.System === "GUI" && event.Type === "ControlStateChange") {
      const data = new ByteWriter();
      data.u8(EVENT_GUI_CONTROL_STATE_CHANGE);
      data.packedU32(6);
      data.i16(event.Id); data.u16(event.Clear); data.u16(event.Set);
      const bytes = data.finish();
      writer.u8(SYSTEM_EVENT_GUI); writer.packedU32(bytes.length); writer.bytes(bytes);
      return;
    }
    if (!event || !Number.isInteger(event.System) || !(event.Data instanceof Uint8Array))
      throw new TypeError("Unsupported SControlTable EventQueue entry");
    writer.u8(event.System); writer.packedU32(event.Data.length); writer.bytes(event.Data);
  }
  function readEvent(reader) {
    const system = reader.u8(), data = reader.bytes(reader.packedU32());
    if (system !== SYSTEM_EVENT_GUI) return {System: system, Data: data.slice()};
    const child = new ByteReader(data), type = child.u8(), payload = child.bytes(child.packedU32());
    if (type !== EVENT_GUI_CONTROL_STATE_CHANGE) return {System: system, Data: data.slice()};
    if (payload.length !== 6 || child.remaining().length) throw new Error("Invalid GUI control-state event payload");
    const state = new ByteReader(payload);
    return {System: "GUI", Type: "ControlStateChange", Id: state.i16(), Clear: state.u16(), Set: state.u16()};
  }

  gpk.SControlTable.prototype.Save = function () {
    const count = requireParallelArrays(this), writer = new ByteWriter();
    writer.packedU32(count);
    for (const state of this.States) writeState(writer, state);
    for (const states of this.TextState) { writer.packedU32(states.length); for (const state of states) writeState(writer, state); }
    for (const identifier of this.Identifier) writer.string(identifier);
    for (const script of this.Script) writer.string(script);
    writer.packedU32(count); for (const constraints of this.Constraints) writeConstraints(writer, constraints);
    writer.packedU32(count); for (const placement of this.Placement) writePlacement(writer, placement);
    writer.packedU32(count); for (const area of this.Area) writeArea(writer, area);
    writer.packedU32(count); for (const draw of this.Draw) writeDraw(writer, draw);
    for (const text of this.Text) { writer.string(text.Text); writer.u8(text.Align); writer.u16(text.Selection.Min); writer.u16(text.Selection.Max); writer.i16(text.FontSelected); }
    for (const image of this.Images) writeImageMetrics(writer, image);
    for (const children of this.Children) { writer.packedU32(children.length); for (const child of children) writer.i16(child); }
    writer.packedU32(this.EventQueue.length); for (const event of this.EventQueue) writeEvent(writer, event);
    return writer.finish();
  };

  gpk.SControlTable.prototype.Load = function (input) {
    const reader = new ByteReader(input), count = reader.packedU32();
    for (const name of ["States", "TextState", "Identifier", "Script", "Constraints", "Placement", "Area", "Draw", "Text", "Children"])
      this[name].length = 0;
    this.EventQueue.length = 0;
    this.Images.length = count;
    for (let id = 0; id < count; ++id) {
      const image = this.Images[id];
      gpk.controlInstanceReset(this, id);
      if (image) this.Images[id] = image;
    }
    for (let id = 0; id < count; ++id) this.States[id] = readState(reader, gpk.SControlState);
    for (let id = 0; id < count; ++id) {
      const textStateCount = reader.packedU32();
      this.TextState[id] = Array.from({length: textStateCount}, () => readState(reader, gpk.STextState));
    }
    for (let id = 0; id < count; ++id) this.Identifier[id] = reader.string();
    for (let id = 0; id < count; ++id) this.Script[id] = reader.string();
    const constraintsCount = reader.packedU32();
    if (constraintsCount !== count) throw new Error("Serialized Constraints count does not match States");
    for (let id = 0; id < count; ++id) this.Constraints[id] = readConstraints(reader);
    const placementCount = reader.packedU32();
    if (placementCount !== count) throw new Error("Serialized Placement count does not match States");
    for (let id = 0; id < count; ++id) this.Placement[id] = readPlacement(reader);
    const areaCount = reader.packedU32();
    if (areaCount !== count) throw new Error("Serialized Area count does not match States");
    for (let id = 0; id < count; ++id) this.Area[id] = readArea(reader);
    const drawCount = reader.packedU32();
    if (drawCount !== count) throw new Error("Serialized Draw count does not match States");
    for (let id = 0; id < count; ++id) this.Draw[id] = readDraw(reader);
    for (let id = 0; id < count; ++id) {
      const text = this.Text[id];
      text.Text = reader.string(); text.Align = reader.u8(); text.Selection = {Min: reader.u16(), Max: reader.u16()}; text.FontSelected = reader.i16();
    }
    for (let id = 0; id < count; ++id) { reader.u16(); reader.u16(); }
    for (let id = 0; id < count; ++id) this.Children[id] = Array.from({length: reader.packedU32()}, () => reader.i16());
    this.EventQueue = Array.from({length: reader.packedU32()}, () => readEvent(reader));
    return reader.remaining();
  };
})(globalThis.gpk);
