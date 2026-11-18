(function (gpk) {
  "use strict";

  const FLAG = gpk.GUI_CONTROL_FLAG = Object.freeze({
    Hidden: 0x001,
    Disabled: 0x002,
    Hovered: 0x004,
    Focused: 0x008,
    Pressed: 0x010,
    Dragging: 0x020,
    Selected: 0x040,
    Updated: 0x080,
    Action: 0x100,
    Unused: 0xFFFF,
    All: 0xFFFF
  });

  class SControlState {
    constructor(parent = gpk.CID_INVALID, flag = 0, mask = FLAG.All) {
      this.Parent = parent;
      this.Flag = flag;
      this.Mask = mask;
    }
    clone() { return new SControlState(this.Parent, this.Flag, this.Mask); }
    _is(bit) { return (this.Flag & this.Mask & bit) !== 0; }
    _set(id, queue, bit, value, ignoreMask = false) {
      const oldState = this.Flag;
      if (ignoreMask)
        this.Flag = value ? (this.Flag | bit) : (this.Flag & ~bit);
      else
        this.Flag = gpk.setBitMasked(this.Flag, this.Mask, bit, value);
      if (oldState !== this.Flag && queue)
        queue.push({System: "GUI", Type: "ControlStateChange", Id: id, Clear: oldState & ~this.Flag, Set: this.Flag & ~oldState});
      return this.Flag !== 0;
    }
    SetHovered(id, queue, value) { return this._set(id, queue, FLAG.Hovered, value); }
    SetHidden(id, queue, value) { return this._set(id, queue, FLAG.Hidden, value); }
    SetFocused(id, queue, value) { return this._set(id, queue, FLAG.Focused, value); }
    SetSelected(id, queue, value) { return this._set(id, queue, FLAG.Selected, value); }
    SetDisabled(id, queue, value) { return this._set(id, queue, FLAG.Disabled, value); }
    SetUpdated(id, queue, value) { return this._set(id, queue, FLAG.Updated, value); }
    SetDragging(id, queue, value) { return this._set(id, queue, FLAG.Dragging, value); }
    SetAction(id, queue, value) { return this._set(id, queue, FLAG.Action, value); }
    SetUnused(id, queue, value) { return this._set(id, queue, FLAG.Unused, value, true); }
    SetPressed(id, queue, value) {
      const becomesAction = value === false && this.IsPressed() && this.IsHovered() && !this.IsDragging();
      this._set(id, queue, FLAG.Pressed, value);
      if (becomesAction)
        this._set(id, queue, FLAG.Action, true);
      return this.Flag !== 0;
    }
    IsHovered() { return this._is(FLAG.Hovered); }
    IsPressed() { return this._is(FLAG.Pressed); }
    IsHidden() { return this._is(FLAG.Hidden); }
    IsFocused() { return this._is(FLAG.Focused); }
    IsSelected() { return this._is(FLAG.Selected); }
    IsDisabled() { return this._is(FLAG.Disabled); }
    IsUpdated() { return this._is(FLAG.Updated); }
    IsDragging() { return this._is(FLAG.Dragging); }
    IsAction() { return this._is(FLAG.Action); }
    IsUnused() { return (this.Flag & FLAG.Unused) === FLAG.Unused; }
  }

  gpk.SControlState = SControlState;
})(globalThis.gpk);
