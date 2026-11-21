(function (gpk) {
  "use strict";

  class SInputMouse {
    constructor() {
      this.Position = gpk.v2();
      this.Deltas = {x: 0, y: 0, z: 0};
      this.ButtonState = new Uint8Array(32);
      this.ButtonTime = new Float64Array(32);
      this.ButtonStrength = new Float64Array(32);
    }
    copyFrom(source) {
      this.Position = gpk.cloneV2(source.Position);
      this.Deltas = {x: source.Deltas.x, y: source.Deltas.y, z: source.Deltas.z};
      this.ButtonState.set(source.ButtonState);
      this.ButtonTime.set(source.ButtonTime);
      this.ButtonStrength.set(source.ButtonStrength);
    }
  }

  class SInputKeyboard {
    constructor() {
      this.KeyState = new Uint8Array(256);
      this.KeyTime = new Float64Array(256);
      this.KeyStrength = new Float64Array(256);
    }
    copyFrom(source) {
      this.KeyState.set(source.KeyState);
      this.KeyTime.set(source.KeyTime);
      this.KeyStrength.set(source.KeyStrength);
    }
  }

  class SInput {
    constructor() {
      this.StateKeyboard = [];
      this.StateMouse = [];
      this.TimeStampKeyboard = [];
      this.TimeStampMouse = [];
      this.KeyboardPrevious = new SInputKeyboard();
      this.KeyboardCurrent = new SInputKeyboard();
      this.MousePrevious = new SInputMouse();
      this.MouseCurrent = new SInputMouse();
    }
    ButtonUp(index) { return this.MouseCurrent.ButtonState[index] === 0 && this.MousePrevious.ButtonState[index] !== 0; }
    ButtonDown(index) { return this.MouseCurrent.ButtonState[index] !== 0 && this.MousePrevious.ButtonState[index] === 0; }
    KeyUp(index) { return this.KeyboardCurrent.KeyState[index] === 0 && this.KeyboardPrevious.KeyState[index] !== 0; }
    KeyDown(index) { return this.KeyboardCurrent.KeyState[index] !== 0 && this.KeyboardPrevious.KeyState[index] === 0; }
    advance() {
      this.MousePrevious.copyFrom(this.MouseCurrent);
      this.KeyboardPrevious.copyFrom(this.KeyboardCurrent);
      this.MouseCurrent.Deltas = {x: 0, y: 0, z: 0};
    }
  }

  gpk.SInputMouse = SInputMouse;
  gpk.SInputKeyboard = SInputKeyboard;
  gpk.SInput = SInput;
})(globalThis.gpk);
