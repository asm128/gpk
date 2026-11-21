(function (gpk) {
  "use strict";

  gpk.virtualKeyboardSetup = function (gui, keyboard, rowWidth, keys) {
    keyboard.Keys = Array.from(keys); keyboard.IdRoot = gpk.controlCreate(gui);
    const font = gui.Fonts[gui.SelectedFont], keySize = {x: font.CharSize.x + 6, y: font.CharSize.y + 6}, actionWidth = 88;
    gui.Controls.Placement[keyboard.IdRoot].Area.Size = {x: keySize.x * rowWidth + 4 + actionWidth, y: keySize.y * Math.floor(keys.length / rowWidth) + 4};
    gui.Controls.Placement[keyboard.IdRoot].Align = gpk.ALIGN.CENTER_BOTTOM;
    for (let index = 0; index < keys.length; ++index) {
      const id = gpk.controlCreate(gui), placement = gui.Controls.Placement[id]; placement.Area.Size = gpk.cloneV2(keySize);
      placement.Area.Offset = {x: (index % rowWidth) * keySize.x, y: Math.floor(index / rowWidth) * keySize.y}; placement.Border = gpk.rect4(1,1,1,1);
      gui.Controls.Text[id].Align = gpk.ALIGN.CENTER; gui.Controls.Text[id].Text = String.fromCharCode(keys[index]); gpk.controlSetParent(gui.Controls, id, keyboard.IdRoot);
    }
    const args = new gpk.SControlPlacement(); args.Area.Size = {x: actionWidth, y: 20}; args.Align = gpk.ALIGN.TOP_RIGHT;
    gpk.guiCreateControlList(gui, 0, gpk.VK_SCANCODE_LABELS, keyboard.IdRoot, args, gpk.ALIGN.CENTER, keyboard.IdKeys);
    return 0;
  };
  gpk.virtualKeyboardSetup437 = function (gui, keyboard) { gpk.virtualKeyboardSetup(gui, keyboard, 32, gpk.GUI_KEYBOARD_Default_keys); gui.Controls.SetHidden(keyboard.IdRoot, true); return keyboard.IdRoot; };

  class SInputBox {
    constructor() { this.IdRoot = -1; this.IdText = -1; this.VirtualKeyboard = new gpk.SVirtualKeyboard(); this.Text = ""; this.MaxLength = 16; this.Editing = false; }
    SetText(gui, text) { this.Text = String(text); return gpk.controlTextSet(gui.Controls, this.IdText, this.Text); }
    Edit(gui, editing) { this.Editing = Boolean(editing); gui.Controls.SetHidden(this.IdRoot, !this.Editing); return this.Editing ? 1 : 0; }
    Update(gui, frameEvents, processableControls) {
      if (!this.Editing) return 0;
      let handled = 0, enter = false; const events = [];
      gpk.controlHandle(gui.Controls, processableControls, id => { if (gpk.virtualKeyboardHandleEvent(this.VirtualKeyboard, id, events)) handled = id; return handled; });
      for (const event of events) {
        if (event.Type === gpk.VK_EVENT.Release && this.Text.length < this.MaxLength) this.SetText(gui, this.Text + String.fromCharCode(event.ScanCode));
        else if (event.Type === gpk.VK_EVENT.Edit) {
          if (event.ScanCode === gpk.VK_SCANCODE.Backspace && this.Text.length) this.SetText(gui, this.Text.slice(0, -1));
          else if (event.ScanCode === gpk.VK_SCANCODE.Clear) this.SetText(gui, "");
          else if (event.ScanCode === gpk.VK_SCANCODE.Escape) this.Edit(gui, false);
          else if (event.ScanCode === gpk.VK_SCANCODE.Enter) enter = true;
        }
      }
      for (const event of frameEvents || []) {
        if (event.Type === "Text" && this.Text.length < this.MaxLength && event.Code >= 0x20 && event.Code <= 0x7F) this.SetText(gui, this.Text + String.fromCharCode(event.Code));
        if (event.Type === "KeyboardDown") {
          if (event.Key === "Escape") this.Edit(gui, false); else if (event.Key === "Enter") enter = true;
          else if (event.Key === "Backspace" && this.Text.length) this.SetText(gui, this.Text.slice(0, -1)); else if (event.Key === "Clear") this.SetText(gui, "");
        }
      }
      return enter ? 0x7FFFFFFF : handled;
    }
  }
  gpk.inputBoxCreate = function (input, gui, parent = -1) {
    input.IdRoot = gpk.controlCreate(gui); gui.Controls.States[input.IdRoot].Mask &= ~gpk.GUI_CONTROL_FLAG.Hovered;
    gui.Controls.Placement[input.IdRoot].Border = gpk.rect4(); gui.Controls.Placement[input.IdRoot].Margin = gpk.rect4(); gui.Controls.Draw[input.IdRoot].NoClient = 1;
    input.IdText = gpk.controlCreate(gui); gui.Controls.Placement[input.IdText].Border = gpk.rect4(2,2,2,2); gui.Controls.Placement[input.IdText].Margin = gpk.rect4(1,1,1,1);
    gui.Controls.Placement[input.IdText].Area.Size = {x: 0, y: 22}; gui.Controls.Text[input.IdText].Align = gpk.ALIGN.CENTER;
    gpk.virtualKeyboardSetup437(gui, input.VirtualKeyboard); gui.Controls.SetHidden(input.VirtualKeyboard.IdRoot, false);
    const keyboardPlacement = gui.Controls.Placement[input.VirtualKeyboard.IdRoot], textPlacement = gui.Controls.Placement[input.IdText];
    keyboardPlacement.Align = textPlacement.Align; keyboardPlacement.Area.Offset.y = textPlacement.Area.Offset.y + textPlacement.Area.Size.y; textPlacement.Area.Size.x = keyboardPlacement.Area.Size.x;
    gui.Controls.Placement[input.IdRoot].Area.Size = {x: Math.max(textPlacement.Area.Size.x, keyboardPlacement.Area.Size.x), y: gui.Controls.Placement[input.IdRoot].Area.Size.y + keyboardPlacement.Area.Size.y};
    gpk.controlSetParent(gui.Controls, input.IdText, input.IdRoot); gpk.controlSetParent(gui.Controls, input.VirtualKeyboard.IdRoot, input.IdRoot); if (parent >= 0) gpk.controlSetParent(gui.Controls, input.IdRoot, parent); return input.IdRoot;
  };
  gpk.SInputBox = SInputBox;
})(globalThis.gpk);
