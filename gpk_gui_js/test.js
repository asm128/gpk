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
  test("gui.Controls binary state round-trips", () => {
    const sourceGUI = new gpk.SGUI(), source = sourceGUI.Controls;
    for (let id = 0; id < 64; ++id) gpk.controlCreate(sourceGUI);
    gpk.controlSetParent(source, 1, 0);
    gpk.controlSetParent(source, 2, 0);
    source.SetUnused(10, true);
    source.States[0].Flag = gpk.GUI_CONTROL_FLAG.Selected | gpk.GUI_CONTROL_FLAG.Hovered | gpk.GUI_CONTROL_FLAG.Updated;
    source.States[0].Mask = gpk.GUI_CONTROL_FLAG.Hidden | gpk.GUI_CONTROL_FLAG.Disabled | gpk.GUI_CONTROL_FLAG.Hovered | gpk.GUI_CONTROL_FLAG.Selected | gpk.GUI_CONTROL_FLAG.Updated;
    source.TextState[0] = [
      new gpk.STextState(-1, gpk.GUI_TEXT_FLAG.Bold, gpk.GUI_TEXT_FLAG.All),
      new gpk.STextState(0, gpk.GUI_TEXT_FLAG.Selected | gpk.GUI_TEXT_FLAG.Italic, gpk.GUI_TEXT_FLAG.All)
    ];
    source.Identifier[0] = "root_control";
    source.Script[0] = "{\"root_control\":{\"Text\":{\"Text\":\"restored\"}}}";
    source.Constraints[0].AttachSizeToControl = {x: 1, y: 2};
    source.Constraints[0].AttachSizeToText = {x: true, y: false};
    source.Constraints[0].DockToControl = {Left: 3, Top: 4, Right: 5, Bottom: 6};
    source.Constraints[0].SizeMinMax = {Min: {x: 7, y: 8}, Max: {x: 900, y: 700}};
    source.Placement[0].Area = gpk.rect2(11, 12, 320, 180);
    source.Placement[0].Margin = gpk.rect4(13, 14, 15, 16);
    source.Placement[0].Border = gpk.rect4(17, 18, 19, 20);
    source.Placement[0].Align = gpk.ALIGN.BOTTOM_RIGHT;
    source.Placement[0].RelativeBorderSize = gpk.GUI_CONTROL_LIMIT.Right;
    source.Placement[0].RelativeMarginSize = gpk.GUI_CONTROL_LIMIT.Bottom;
    source.Placement[0].RelativeMetrics = 0x0081;
    source.Area[0].Total.Local = gpk.rect2(23, 24, 25, 26);
    source.Area[0].Client.Global = gpk.rect2(27, 28, 29, 30);
    source.Area[0].Text = gpk.rect2(31, 32, 33, 34);
    source.Area[0].Rectangles[0] = gpk.rect2(35, 36, 37, 38);
    source.Area[0].Triangles[0] = [{x: 39, y: 40}, {x: 41, y: 42}, {x: 43, y: 44}];
    source.Draw[0].ColorMode = gpk.GUI_COLOR_MODE.Flat;
    source.Draw[0].FrameOut = 1;
    source.Draw[0].NoBorder = 1;
    source.Draw[0].UseNewPalettes = 1;
    source.Draw[0].ColorTheme = 21;
    source.Draw[0].Palettes = Array.from({length: gpk.GUI_CONTROL_PALETTE.COUNT}, (_, index) => 100 + index);
    source.Text[0].Text = "Serialized GUI text";
    source.Text[0].Align = gpk.ALIGN.CENTER_LEFT;
    source.Text[0].Selection = {Min: 3, Max: 9};
    source.Text[0].FontSelected = 4;
    source.Images[0].ImageOffset = {x: 5, y: 6};
    source.Images[0].ImageAlign = gpk.ALIGN.TOP_RIGHT;
    source.Images[0].ImageInvertX = 1;
    source.Images[0].Rotation = 3;
    source.Images[0].Image = {width: 2, height: 1, data: new Uint8ClampedArray([3, 2, 1, 4, 7, 6, 5, 8])};
    source.EventQueue.push(null);

    const bytes = sourceGUI.Controls.Save();
    gpk.assert(bytes[0] === 1 && bytes[1] === 64, "64-control count was not encoded as packed bytes 01 40");
    const sourceImage = source.Images[0].Image;
    source.Images[0].Image = null;
    const bytesWithoutImage = sourceGUI.Controls.Save();
    source.Images[0].Image = sourceImage;
    let imageMetricByteDifferences = 0;
    for (let index = 0; index < bytes.length; ++index) imageMetricByteDifferences += bytes[index] !== bytesWithoutImage[index];
    gpk.assert(bytes.length === bytesWithoutImage.length && imageMetricByteDifferences === 2, "image stream contains more than the W/H POD bytes");
    const restoredGUI = new gpk.SGUI(), restored = restoredGUI.Controls;
    for (let id = 0; id < 64; ++id) gpk.controlCreate(restoredGUI);
    const separatelyLoadedImage = {width: 1, height: 1, data: new Uint8ClampedArray([11, 10, 9, 12])};
    restored.Images[0].Image = separatelyLoadedImage;
    restored.Images[0].ImageOffset = {x: 45, y: 46};
    restored.Images[0].ImageAlign = gpk.ALIGN.BOTTOM_LEFT;
    restored.Images[0].ImageInvertY = 1;
    restored.Images[0].Rotation = 2;
    const remaining = restoredGUI.Controls.Load(bytes);
    gpk.assert(remaining.length === 0, "snapshot left " + remaining.length + " unread byte(s)");
    gpk.assert(restored.States.length === 64 && restored.Children[0].join(",") === "1,2", "slot count or child order changed");
    gpk.assert(restored.States[0].IsSelected() && restored.States[0].IsHovered() && restored.States[0].IsUpdated(), "control flags changed");
    gpk.assert(restored.States[10].IsUnused(), "unused slot was not preserved");
    gpk.assert(restored.TextState[0].length === 2 && restored.TextState[0][1].IsSelected() && restored.TextState[0][1].IsItalic(), "text state changed");
    gpk.assert(restored.Identifier[0] === source.Identifier[0] && restored.Script[0] === source.Script[0] && restored.Text[0].Text === source.Text[0].Text, "strings changed");
    gpk.assert(restored.Constraints[0].DockToControl.Right === 5 && restored.Placement[0].Area.Size.x === 320 && restored.Draw[0].Palettes[12] === 112, "component state changed");
    gpk.assert(source.Images[0].Image.width === 2 && source.Images[0].Image.height === 1, "source image metrics changed");
    gpk.assert(restored.Images[0].Image === separatelyLoadedImage && restored.Images[0].ImageOffset.x === 45 && restored.Images[0].ImageOffset.y === 46 && restored.Images[0].ImageAlign === gpk.ALIGN.BOTTOM_LEFT && restored.Images[0].ImageInvertY === 1 && restored.Images[0].Rotation === 2, "table load replaced separately owned image state");
    gpk.assert(restored.Area[0].Total.Local.Size.x === 25 && restored.Area[0].Triangles[0][2].y === 44, "control area changed");
    gpk.assert(restored.EventQueue.length === source.EventQueue.length && restored.EventQueue[0].Id === source.EventQueue[0].Id, "event queue changed");
    gpk.assert(restored.EventQueue[1].System === 0 && restored.EventQueue[1].Data.length === 0, "null event did not load as the default event");
  });
  test("gui.Controls binary load rejects truncated input", () => {
    const sourceGUI = new gpk.SGUI(); gpk.controlCreate(sourceGUI); sourceGUI.Controls.Identifier[0] = "source";
    const bytes = sourceGUI.Controls.Save(), destinationGUI = new gpk.SGUI();
    let rejectedTruncation = false;
    try { destinationGUI.Controls.Load(bytes.subarray(0, bytes.length - 1)); } catch (_) { rejectedTruncation = true; }
    gpk.assert(rejectedTruncation, "truncated gui.Controls input was accepted");
    try { new gpk.SGUI().Controls.Load(new Uint8Array([1])); }
    catch (error) { gpk.assert(/Truncated/.test(String(error)), "truncated packed count reported the wrong failure: " + error); return; }
    throw new Error("truncated packed count was accepted");
  });

  const passed = results.filter(result => result.pass).length;
  output.innerHTML = "<h1>gpk_gui_js tests: " + passed + "/" + results.length + "</h1>" + results.map(result => "<div class=\"" + (result.pass ? "pass" : "fail") + "\">" + (result.pass ? "PASS" : "FAIL") + " — " + result.name + (result.error ? "<pre>" + result.error.stack + "</pre>" : "") + "</div>").join("");
  document.title = passed === results.length ? "PASS - gpk_gui_js" : "FAIL - gpk_gui_js";
  globalThis.gpkTestResults = results;
})(globalThis.gpk);
