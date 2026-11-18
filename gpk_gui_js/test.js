(function (gpk) {
  "use strict";
  const output = document.getElementById("test-output"), results = [];
  function test(name, body) {
    try { body(); results.push({name, pass:true}); }
    catch (error) { results.push({name, pass:false, error}); }
  }
  function createGUI() { const gui = new gpk.SGUI(); gpk.controlCreate(gui); return gui; }

  test("control slots and reuse", () => {
    const gui = createGUI(); const first = 0, second = gpk.controlCreate(gui); gpk.controlDelete(gui.Controls, first); const reused = gpk.controlCreate(gui);
    gpk.assert(second === 1 && reused === 0, "controlCreate did not reuse the unused slot");
  });
  test("pressed release produces action", () => {
    const gui = createGUI(), state = gui.Controls.States[0]; state.SetHovered(0, gui.Controls.EventQueue, true); state.SetPressed(0, gui.Controls.EventQueue, true); state.SetPressed(0, gui.Controls.EventQueue, false);
    gpk.assert(state.IsAction(), "release did not set Action");
  });
  test("state mask blocks hover", () => {
    const gui = createGUI(), state = gui.Controls.States[0]; state.Mask &= ~gpk.GUI_CONTROL_FLAG.Hovered; state.SetHovered(0, gui.Controls.EventQueue, true);
    gpk.assert(!state.IsHovered(), "masked hover changed state");
  });
  test("masked bit updates change only enabled bits", () => {
    const changed = gpk.setBitMasked(0, 0x02, 0x03, true);
    gpk.assert(changed === 0x02, "masked update wrote disabled bits: " + changed);
  });
  test("parent ownership and inherited hidden", () => {
    const gui = createGUI(), child = gpk.controlCreateChild(gui, 0); gui.Controls.SetHidden(0, true);
    gpk.assert(gui.Controls.Children[0][0] === child && gpk.controlHidden(gui.Controls, child), "hierarchy did not propagate hidden state");
  });
  test("metrics preserve center alignment", () => {
    const gui = createGUI(), placement = gui.Controls.Placement[0]; placement.Area = gpk.rect2(0,0,100,40); placement.Align = gpk.ALIGN.CENTER;
    gpk.guiUpdate(gui, {x:300,y:200}); const area = gui.Controls.Area[0].Total.Global;
    gpk.assert(area.Offset.x === 100 && area.Offset.y === 80, "centered area is " + JSON.stringify(area));
  });
  test("edge alignment does not satisfy center masks", () => {
    gpk.assert(gpk.bitTrue(gpk.ALIGN.TOP_LEFT, gpk.ALIGN.LEFT), "TOP_LEFT lost LEFT");
    gpk.assert(gpk.bitFalse(gpk.ALIGN.TOP_LEFT, gpk.ALIGN.HCENTER), "TOP_LEFT incorrectly satisfies HCENTER");
    gpk.assert(gpk.bitFalse(gpk.ALIGN.TOP_LEFT, gpk.ALIGN.VCENTER), "TOP_LEFT incorrectly satisfies VCENTER");
    const left = gpk.realignRectangle({x:1280,y:720}, gpk.rect2(12,32,1256,676), gpk.ALIGN.TOP_LEFT);
    const right = gpk.realignRectangle({x:1280,y:720}, gpk.rect2(2,2,52,20), gpk.ALIGN.TOP_RIGHT);
    gpk.assert(left.Offset.x === 12 && left.Offset.y === 32, "TOP_LEFT moved to " + JSON.stringify(left.Offset));
    gpk.assert(right.Offset.x === 1226 && right.Offset.y === 2, "TOP_RIGHT moved to " + JSON.stringify(right.Offset));
  });
  test("supplied GUI document keeps its authored panel geometry", () => {
    const gui = new gpk.SGUI(), ids = gpk.guiDocumentCreate(gui, globalThis.GUI_TYPE_EXPLORER_DOCUMENT);
    gpk.guiUpdate(gui, {x:1280,y:720});
    const area = name => gui.Controls.Area[ids[name]].Total.Global;
    const menu = area("menu_bar"), workspace = area("workspace"), structures = area("structures_panel"), enums = area("enums_panel"), details = area("details_panel"), close = area("close_application");
    gpk.assert(menu.Offset.x === 0 && menu.Offset.y === 0 && menu.Size.x === 1280 && menu.Size.y === 24, "menu geometry changed: " + JSON.stringify(menu));
    gpk.assert(workspace.Offset.x === 12 && workspace.Offset.y === 32, "workspace moved: " + JSON.stringify(workspace));
    gpk.assert(structures.Offset.x === 12 && structures.Offset.y === 32 && structures.Size.x === 252, "structures panel moved: " + JSON.stringify(structures));
    gpk.assert(enums.Offset.x === 272 && enums.Offset.y === 32 && enums.Size.x === 252, "enums panel moved: " + JSON.stringify(enums));
    gpk.assert(details.Offset.x === 532 && details.Offset.y === 32 && details.Size.x === 736, "details panel moved: " + JSON.stringify(details));
    gpk.assert(close.Offset.x === 1226 && close.Offset.y === 2, "close button moved: " + JSON.stringify(close));
  });
  test("JSON patches existing identifiers only", () => {
    const gui = createGUI(); gui.Controls.Identifier[0] = "root"; gpk.guiJSONLoad(gui, {root:{Text:{Text:"patched"}}});
    gpk.assert(gui.Controls.Text[0].Text === "patched" && gui.Controls.States.length === 1, "JSON loader changed ownership semantics");
  });
  test("dynamic script resolves serialized values", () => {
    const gui = createGUI(); gui.Controls.Identifier[0] = "root"; gui.Controls.Text[0].Text = "before"; gui.Controls.Script[0] = JSON.stringify({root:{Text:{Text:"{root.Text.Text}"}}});
    gpk.guiScriptRun(gui, 0); gpk.assert(gui.Controls.Text[0].Text === "before", "script resolver changed the value");
  });
  test("JSON expressions preserve boolean and conditional operators", () => {
    const value = gpk.jsonExpressionResolve("root.Flag == 4 && !root.Hidden ? root.Values[1] : 0", {root:{Flag:4, Hidden:false, Values:[3,9]}});
    gpk.assert(value === 9, "expression result was " + value);
  });
  test("expression reader exposes token tree", () => {
    const reader = gpk.expressionParse("root.Values[root.Index]");
    gpk.assert(reader.Token.length === reader.View.length && reader.Tree[0].Children.length === 1, "reader arrays or root tree are inconsistent");
    gpk.assert(reader.Token.some(token => token.Type === gpk.EXPRESSION_READER_TYPE.TERM_INDEX), "index token is missing");
  });
  test("computed keys and indices resolve", () => {
    const input = {root:{Key:"Values", Index:1, Values:[3,9]}};
    gpk.assert(gpk.jsonExpressionResolve("root.{root.Key}[root.Index]", input) === 9, "computed access did not resolve");
  });
  test("conditional evaluation is lazy", () => {
    const value = gpk.jsonExpressionResolve("root.Flag ? root.Value : root.Missing", {root:{Flag:true, Value:7}});
    gpk.assert(value === 7, "unused conditional branch was evaluated");
  });
  test("JSON-aware equality compares arrays and objects", () => {
    const input = {root:{A:[1,{x:true}], B:[1,{x:true}], Empty:[], Disabled:false}};
    gpk.assert(gpk.jsonExpressionResolve("root.A == root.B", input) === true, "deep equality failed");
    gpk.assert(gpk.jsonExpressionResolve("root.Empty == root.Disabled", input) === true, "JSON truth equality failed");
  });
  test("format parser preserves nested computed keys", () => {
    const input = {root:{Key:"Name", Name:"LLC"}};
    gpk.assert(gpk.jsonStringFormat("Engine: {root.{root.Key}}", input) === "Engine: LLC", "nested format expression failed");
  });
  test("drawing writes pixels", () => {
    const gui = createGUI(); gui.Controls.Placement[0].Area = gpk.rect2(2,2,40,20); gui.Controls.Text[0].Text = "A"; gpk.guiUpdate(gui,{x:64,y:32});
    const target = new gpk.PixelTarget(64,32); target.clear(gpk.rgba(0,0,0,255)); gpk.guiDraw(gui,target);
    let changed = false; for (let index = 0; index < target.data.length; index += 4) if (target.data[index] || target.data[index+1] || target.data[index+2]) { changed = true; break; }
    gpk.assert(changed, "draw target remained blank");
  });
  test("viewport remains an ordinary hierarchy", () => {
    const gui = createGUI(), viewport = new gpk.SViewport(); gpk.viewportInitialize(gui, viewport);
    gpk.assert(gui.Controls.Children[viewport.IdControl].length === gpk.VIEWPORT_CONTROL.COUNT, "viewport children mismatch");
  });
  test("control grid maps cells", () => {
    const gui = createGUI(), grid = gpk.guiControlGridCreate(gui,["a","b","c","d"],0,gpk.DIRECTION.RIGHT,2,{x:30,y:20});
    gpk.assert(gpk.guiControlGridControl(gui,grid,gpk.DIRECTION.RIGHT,{x:1,y:1}) >= 0, "grid cell was not mapped");
  });

  const passed = results.filter(result => result.pass).length;
  output.innerHTML = "<h1>gpk_gui_js tests: " + passed + "/" + results.length + "</h1>" + results.map(result => "<div class=\"" + (result.pass ? "pass" : "fail") + "\">" + (result.pass ? "PASS" : "FAIL") + " — " + result.name + (result.error ? "<pre>" + result.error.stack + "</pre>" : "") + "</div>").join("");
  document.title = passed === results.length ? "PASS - gpk_gui_js" : "FAIL - gpk_gui_js";
  globalThis.gpkTestResults = results;
})(globalThis.gpk);
