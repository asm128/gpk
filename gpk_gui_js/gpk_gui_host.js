(function (gpk) {
  "use strict";

  class SGUIHost {
    constructor(canvas, gui, options = {}) {
      this.Canvas = canvas;
      this.Context = canvas.getContext("2d", {alpha: false});
      this.GUI = gui;
      this.Input = new gpk.SInput();
      this.Target = new gpk.PixelTarget(canvas.width, canvas.height);
      this.Background = options.background || gpk.rgba(13, 18, 24, 255);
      this.Events = [];
      this.Running = false;
      this.OnFrame = options.onFrame || null;
      this._frame = this._frame.bind(this);
      this._bind();
    }
    _position(event) {
      const bounds = this.Canvas.getBoundingClientRect();
      return {x: Math.floor((event.clientX - bounds.left) * this.Canvas.width / bounds.width), y: Math.floor((event.clientY - bounds.top) * this.Canvas.height / bounds.height)};
    }
    _bind() {
      this.Canvas.addEventListener("pointermove", event => {
        const position = this._position(event), previous = this.Input.MouseCurrent.Position;
        this.Input.MouseCurrent.Deltas.x += position.x - previous.x; this.Input.MouseCurrent.Deltas.y += position.y - previous.y;
        this.Input.MouseCurrent.Position = position;
      });
      this.Canvas.addEventListener("pointerdown", event => {
        const button = Math.max(0, event.button); this.Input.MouseCurrent.Position = this._position(event); this.Input.MouseCurrent.ButtonState[button] = 1;
        this.Canvas.setPointerCapture(event.pointerId); event.preventDefault();
      });
      this.Canvas.addEventListener("pointerup", event => {
        const button = Math.max(0, event.button); this.Input.MouseCurrent.Position = this._position(event); this.Input.MouseCurrent.ButtonState[button] = 0;
        if (this.Canvas.hasPointerCapture(event.pointerId)) this.Canvas.releasePointerCapture(event.pointerId); event.preventDefault();
      });
      this.Canvas.addEventListener("pointercancel", event => { this.Input.MouseCurrent.ButtonState.fill(0); if (this.Canvas.hasPointerCapture(event.pointerId)) this.Canvas.releasePointerCapture(event.pointerId); });
      this.Canvas.addEventListener("wheel", event => { this.Input.MouseCurrent.Deltas.z += Math.sign(event.deltaY); event.preventDefault(); }, {passive: false});
      this.Canvas.addEventListener("contextmenu", event => event.preventDefault());
      globalThis.addEventListener("keydown", event => {
        const code = event.keyCode & 0xFF; this.Input.KeyboardCurrent.KeyState[code] = 1; this.Events.push({Type: "KeyboardDown", Key: event.key});
        if (event.key.length === 1) this.Events.push({Type: "Text", Code: event.key.charCodeAt(0)});
      });
      globalThis.addEventListener("keyup", event => { this.Input.KeyboardCurrent.KeyState[event.keyCode & 0xFF] = 0; });
      globalThis.addEventListener("blur", () => { this.Input.MouseCurrent.ButtonState.fill(0); this.Input.KeyboardCurrent.KeyState.fill(0); });
    }
    present() { this.Context.putImageData(new ImageData(this.Target.data, this.Target.width, this.Target.height), 0, 0); }
    step(time = performance.now()) {
      gpk.guiProcessInput(this.GUI, this.Input);
      gpk.guiUpdate(this.GUI, {x: this.Target.width, y: this.Target.height});
      this.Target.clear(this.Background);
      gpk.guiDraw(this.GUI, this.Target);
      if (this.OnFrame) this.OnFrame(this, time);
      this.present();
      this.Input.advance(); this.Events.length = 0;
    }
    _frame(time) { if (!this.Running) return; this.step(time); requestAnimationFrame(this._frame); }
    start() { if (!this.Running) { this.Running = true; requestAnimationFrame(this._frame); } }
    stop() { this.Running = false; }
  }
  gpk.SGUIHost = SGUIHost;
})(globalThis.gpk);
