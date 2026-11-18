(function (gpk) {
  "use strict";

  gpk.GUI_KEYBOARD_Default_row_width = 32;
  gpk.GUI_KEYBOARD_Default_keys = Array.from({length: 254}, (_, index) => index + 1).concat(["!".charCodeAt(0), "?".charCodeAt(0)]);
  gpk.VK_EVENT = Object.freeze({Press: 0, Release: 1, Hold: 2, Edit: 3, Arrow: 4});
  gpk.VK_SCANCODE = Object.freeze({
    Escape: 0, Enter: 1, Backspace: 2, Clear: 3, Left: 4, Right: 5, Up: 6, Down: 7,
    Front: 8, Back: 9, Delete: 10, Insert: 11, Home: 12, End: 13, PageUp: 14, PageDown: 15,
    LeftShift: 16, RightShift: 17, Alt: 18, AltGr: 19, LControl: 20, RControl: 21, System: 22
  });
  gpk.VK_SCANCODE_LABELS = ["Escape", "Enter", "Backspace", "Clear", "Left", "Right", "Up", "Down", "Front", "Back", "Delete", "Insert", "Home", "End", "Page Up", "Page Down", "Left Shift", "Right Shift", "Alt", "Alt Gr", "Left Control", "Right Control", "System"];

  class SVirtualKeyboard {
    constructor() { this.IdRoot = gpk.CID_INVALID; this.IdKeys = []; this.Keys = []; this.Events = []; }
  }
  gpk.virtualKeyboardHandleEvent = function (keyboard, id, events = keyboard.Events) {
    const first = keyboard.IdRoot + 1, stop = first + keyboard.Keys.length;
    if (id >= first && id < stop) {
      events.push({Type: gpk.VK_EVENT.Release, ScanCode: keyboard.Keys[id - first]});
      return events.length;
    }
    if (id >= stop && id < stop + gpk.VK_SCANCODE_LABELS.length) {
      events.push({Type: gpk.VK_EVENT.Edit, ScanCode: id - stop});
      return events.length;
    }
    return 0;
  };
  gpk.SVirtualKeyboard = SVirtualKeyboard;
})(globalThis.gpk);
