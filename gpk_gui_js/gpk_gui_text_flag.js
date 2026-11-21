(function (gpk) {
  "use strict";

  const FLAG = gpk.GUI_TEXT_FLAG = Object.freeze({
    Selected: 0x01,
    Cursor: 0x02,
    SwapColors: 0x04,
    Bold: 0x08,
    Underline: 0x10,
    Strikethrough: 0x20,
    Italic: 0x40,
    All: 0xFFFF
  });

  class STextState {
    constructor(parent = gpk.CID_INVALID, flag = 0, mask = FLAG.All) {
      this.Parent = parent;
      this.Flag = flag;
      this.Mask = mask;
    }
    clone() { return new STextState(this.Parent, this.Flag, this.Mask); }
    _is(bit) { return (this.Flag & this.Mask & bit) !== 0; }
    _set(idControl, idCharacter, queue, bit, value) {
      const oldState = this.Flag;
      this.Flag = gpk.setBitMasked(this.Flag, this.Mask, bit, value);
      if (oldState !== this.Flag && queue)
        queue.push({System: "GUI", Type: "TextStateChange", IdControl: idControl, IdCharacter: idCharacter, Clear: oldState & ~this.Flag, Set: this.Flag & ~oldState});
      return this._is(bit);
    }
    IsSelected() { return this._is(FLAG.Selected); }
    IsCursor() { return this._is(FLAG.Cursor); }
    IsSwapColors() { return this._is(FLAG.SwapColors); }
    IsBold() { return this._is(FLAG.Bold); }
    IsUnderline() { return this._is(FLAG.Underline); }
    IsStrikethrough() { return this._is(FLAG.Strikethrough); }
    IsItalic() { return this._is(FLAG.Italic); }
    SetSelected(id, character, queue, value) { return this._set(id, character, queue, FLAG.Selected, value); }
    SetCursor(id, character, queue, value) { return this._set(id, character, queue, FLAG.Cursor, value); }
    SetSwapColors(id, character, queue, value) { return this._set(id, character, queue, FLAG.SwapColors, value); }
    SetBold(id, character, queue, value) { return this._set(id, character, queue, FLAG.Bold, value); }
    SetUnderline(id, character, queue, value) { return this._set(id, character, queue, FLAG.Underline, value); }
    SetStrikethrough(id, character, queue, value) { return this._set(id, character, queue, FLAG.Strikethrough, value); }
    SetItalic(id, character, queue, value) { return this._set(id, character, queue, FLAG.Italic, value); }
  }

  gpk.STextState = STextState;
})(globalThis.gpk);
