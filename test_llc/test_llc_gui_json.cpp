#include "gpk_ctti_stct_json.h"
#include "gpk_gui_control.h"
#include "gpk_gui_control_json.h"
#include "gpk_gui_control_json_expression.h"

#include "test_llc_suites.h"

usns gpk;

GDEFINE_ENUM_TYPE(GUI_JSON_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, CONTROL_CREATE	, 0, "The test could not create the source or destination control.");
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, TARGET_DEFAULT	, 1, "The destination control was not blank before loading.");
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, STATE_SAVE		, 2, "The SControlState table entry could not be saved to JSON.");
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, STATE_LOAD		, 3, "The SControlState JSON could not populate the destination table entry.");
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, STATE_VALUE		, 4, "The SControlState destination differs from the source.");
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, TEXT_SAVE		, 5, "The SControlText table entry could not be saved to JSON.");
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, TEXT_LOAD		, 6, "The SControlText JSON could not populate the destination table entry.");
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, TEXT_VALUE		, 7, "The SControlText destination differs from the source.");
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, INPUT_CURSOR	, 8, "The JSON input cursor was not fully consumed.");
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, TABLE_SAVE		, 9, "The control table could not be saved through its JSON layer.");
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, TABLE_PARSE	, 10, "The generated control-table JSON could not be parsed.");
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, TABLE_LOAD		, 11, "The control table could not be loaded through its JSON layer.");
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, BINARY_PARITY	, 12, "The JSON round trip does not match the established binary Save() contract.");
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, IMAGE_PRESERVE	, 13, "JSON loading changed separately owned control-image data.");
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, SCRIPT_RUN		, 14, "The control-table JSON script could not be executed.");
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, SCRIPT_LITERAL	, 15, "A literal JSON script did not update the target control.");
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, SCRIPT_EXPRESSION, 16, "A JSON expression did not copy the live control value.");
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, SCRIPT_STORED	, 17, "The script stored by control id did not execute.");
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, SCRIPT_ACTION	, 18, "Action-driven script execution did not update the target control.");
GDEFINE_ENUM_VALUED(GUI_JSON_TEST_RESULT, SCRIPT_EVENT	, 19, "The framework event binding did not enqueue the requested child event.");

sttc ::gpk::b8_t bytesEqual(::gpk::vcu0_t left, ::gpk::vcu0_t right) {
	if(left.size() != right.size())
		rtrn false;
	for(::gpk::u2_t iByte = 0; iByte < left.size(); ++iByte)
		if(left[iByte] != right[iByte])
			rtrn false;
	rtrn true;
}

sttc ::gpk::err_t testControlTableJSONBinaryParity(::gpk::ATestError & errors) {
	::gpk::SControlTable source = {};
	cnst ::gpk::cid_t sourceRoot = ::gpk::controlCreate(source, {});
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_CONTROL_CREATE, ::gpk::failed(sourceRoot));
	cnst ::gpk::cid_t sourceChild = ::gpk::controlCreate(source, {});
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_CONTROL_CREATE, ::gpk::failed(sourceChild));
	cnst ::gpk::cid_t sourceUnused = ::gpk::controlCreate(source, {});
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_CONTROL_CREATE, ::gpk::failed(sourceUnused));
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_TABLE_LOAD, ::gpk::failed(::gpk::controlSetParent(source, sourceChild, sourceRoot)));
	source.Identifier[sourceRoot] = "root";
	source.Identifier[sourceChild] = "child";
	source.Identifier[sourceUnused] = "unused";
	source.SetUnused(sourceUnused, true);
	source.Text[sourceUnused].Text = "unused retained";
	source.States[sourceRoot].Flag = (::gpk::GUI_CONTROL_FLAG)(::gpk::GUI_CONTROL_FLAG_Hidden | ::gpk::GUI_CONTROL_FLAG_Focused | ::gpk::GUI_CONTROL_FLAG_Selected);
	source.Text[sourceRoot].Text = "root text";
	source.Text[sourceRoot].Align = ::gpk::ALIGN_BOTTOM_RIGHT;
	source.Text[sourceRoot].Selection = {2, 7};
	source.Text[sourceRoot].FontSelected = 3;
	source.Script[sourceChild] = "{\"child\":{\"State\":{\"Hidden\":true}}}";
	source.TextState[sourceRoot].push_back({-1, ::gpk::GUI_TEXT_Bold, ::gpk::GUI_TEXT_FLAG_All});
	source.TextState[sourceRoot].push_back({0, (::gpk::GUI_TEXT_FLAG)(::gpk::GUI_TEXT_Selected | ::gpk::GUI_TEXT_Italic), ::gpk::GUI_TEXT_FLAG_All});
	source.Constraints[sourceRoot].AttachSizeToControl = {sourceChild, sourceChild};
	source.Constraints[sourceRoot].AttachSizeToText = {true, false};
	source.Constraints[sourceRoot].DockToControl = {sourceChild, sourceChild, sourceChild, sourceChild};
	source.Constraints[sourceRoot].SizeMinMax = {{7, 8}, {900, 700}};
	source.Placement[sourceRoot].Area = {{11, 12}, {320, 180}};
	source.Placement[sourceRoot].Margin = {13, 14, 15, 16};
	source.Placement[sourceRoot].Border = {17, 18, 19, 20};
	source.Placement[sourceRoot].Align = ::gpk::ALIGN_BOTTOM_RIGHT;
	source.Placement[sourceRoot].RelativeBorderSize = ::gpk::GUI_CONTROL_LIMIT_Right;
	source.Placement[sourceRoot].RelativeMarginSize = ::gpk::GUI_CONTROL_LIMIT_Bottom;
	source.Placement[sourceRoot].RelativeMetrics = (::gpk::GUI_CONTROL_METRICS_FLAG)(::gpk::GUI_CONTROL_METRICS_FLAG_XSize | ::gpk::GUI_CONTROL_METRICS_FLAG_YOffset);
	source.Draw[sourceRoot].ColorMode = ::gpk::GUI_COLOR_MODE_Flat;
	source.Draw[sourceRoot].FrameOut = true;
	source.Draw[sourceRoot].NoBorder = true;
	source.Draw[sourceRoot].UseNewPalettes = true;
	source.Draw[sourceRoot].ColorTheme = 21;
	for(::gpk::u2_t iPalette = 0; iPalette < source.Draw[sourceRoot].Palettes.size(); ++iPalette)
		source.Draw[sourceRoot].Palettes[iPalette] = (::gpk::s1_t)(100 + iPalette);
	source.Area[sourceRoot].Total.Local = {{23, 24}, {25, 26}};
	source.Area[sourceRoot].Client.Global = {{27, 28}, {29, 30}};
	source.Area[sourceRoot].Text = {{31, 32}, {33, 34}};
	source.Area[sourceRoot].Rectangles[0] = {{35, 36}, {37, 38}};
	source.Area[sourceRoot].Triangles[0] = {{39, 40}, {41, 42}, {43, 44}};
	if_fail_fe(source.Images[sourceRoot].Temp.Color.resize(::gpk::n2u2_t{2, 1}));
	source.Images[sourceRoot].Temp.Color.Texels[0] = {1, 2, 3, 4};
	source.Images[sourceRoot].Temp.Color.Texels[1] = {5, 6, 7, 8};
	source.Images[sourceRoot].Image = source.Images[sourceRoot].Temp.Color.View;
	if_fail_fe(source.EventQueue.resize(2));
	source.EventQueue[0]->Type = ::gpk::SYSTEM_EVENT_GUI;
	if_fail_fe(source.EventQueue[0]->Data.push_back(1));
	if_fail_fe(source.EventQueue[0]->Data.push_back(2));
	if_fail_fe(source.EventQueue[0]->Data.push_back(3));
	if_fail_fe(source.EventQueue[0]->Data.push_back(4));

	::gpk::SControlTable target = {};
	cnst ::gpk::cid_t targetRoot = ::gpk::controlCreate(target, {});
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_CONTROL_CREATE, ::gpk::failed(targetRoot));
	cnst ::gpk::cid_t targetChild = ::gpk::controlCreate(target, {});
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_CONTROL_CREATE, ::gpk::failed(targetChild));
	cnst ::gpk::cid_t targetUnused = ::gpk::controlCreate(target, {});
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_CONTROL_CREATE, ::gpk::failed(targetUnused));
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_TABLE_LOAD, ::gpk::failed(::gpk::controlSetParent(target, targetChild, targetRoot)));
	target.Identifier[targetRoot] = "root";
	target.Identifier[targetChild] = "child";
	target.Identifier[targetUnused] = "unused";
	target.SetUnused(targetUnused, true);
	if_fail_fe(target.Images[targetRoot].Temp.Color.resize(::gpk::n2u2_t{2, 1}));
	target.Images[targetRoot].Temp.Color.Texels[0] = {9, 10, 11, 12};
	target.Images[targetRoot].Temp.Color.Texels[1] = {13, 14, 15, 16};
	target.Images[targetRoot].Image = target.Images[targetRoot].Temp.Color.View;
	target.Images[targetRoot].ImageOffset = {45, 46};
	target.Images[targetRoot].ImageAlign = ::gpk::ALIGN_BOTTOM_LEFT;
	target.Images[targetRoot].ImageInvertY = true;
	target.Images[targetRoot].Rotation = 2;
	cnst ::gpk::n2u2_t targetImageMetrics = target.Images[targetRoot].Image.metrics();
	cnst ::gpk::bgra targetImagePixel = target.Images[targetRoot].Image[0][0];
	cnst ::gpk::n2s1_t targetImageOffset = target.Images[targetRoot].ImageOffset;
	cnst ::gpk::u0_t targetImageAlign = target.Images[targetRoot].ImageAlign;
	cnst ::gpk::u0_t targetImageInvertX = target.Images[targetRoot].ImageInvertX;
	cnst ::gpk::u0_t targetImageInvertY = target.Images[targetRoot].ImageInvertY;
	cnst ::gpk::u0_t targetImageRotation = target.Images[targetRoot].Rotation;

	::gpk::string json = {};
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_TABLE_SAVE, ::gpk::failed(::gpk::controlTableJSONSave(source, json)));
	::gpk::SJSONReader reader = {};
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_TABLE_PARSE, ::gpk::failed(::gpk::jsonParse(reader, json)));
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_TABLE_LOAD, ::gpk::failed(::gpk::controlTableJSONLoad(target, reader)));
	GPK_TEST_CHECK(errors, GUI_JSON_TEST_RESULT_IMAGE_PRESERVE
		, target.Images[targetRoot].Image.metrics() != targetImageMetrics
		|| target.Images[targetRoot].Image[0][0] != targetImagePixel
		|| target.Images[targetRoot].ImageOffset != targetImageOffset
		|| target.Images[targetRoot].ImageAlign != targetImageAlign
		|| target.Images[targetRoot].ImageInvertX != targetImageInvertX
		|| target.Images[targetRoot].ImageInvertY != targetImageInvertY
		|| target.Images[targetRoot].Rotation != targetImageRotation
		);

	::gpk::au0_t sourceBinary = {}, targetBinary = {};
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_BINARY_PARITY, ::gpk::failed(source.Save(sourceBinary)));
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_BINARY_PARITY, ::gpk::failed(target.Save(targetBinary)));
	GPK_TEST_CHECKF(errors, GUI_JSON_TEST_RESULT_BINARY_PARITY
		, false == ::bytesEqual(sourceBinary, targetBinary)
		, "JSON:'%.*s'; source bytes:%u; target bytes:%u."
		, GPK_VIEW_ARGS(json), sourceBinary.size(), targetBinary.size()
		);
	rtrn 0;
}

sttc ::gpk::err_t testControlTableJSONScript(::gpk::ATestError & errors) {
	::gpk::SControlTable controls = {};
	cnst ::gpk::cid_t source = ::gpk::controlCreate(controls, {});
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_CONTROL_CREATE, ::gpk::failed(source));
	cnst ::gpk::cid_t target = ::gpk::controlCreate(controls, {});
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_CONTROL_CREATE, ::gpk::failed(target));
	cnst ::gpk::cid_t action = ::gpk::controlCreate(controls, {});
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_CONTROL_CREATE, ::gpk::failed(action));
	controls.Identifier[source] = "source";
	controls.Identifier[target] = "target";
	controls.Identifier[action] = "action";
	controls.Draw[source].ColorTheme = 27;

	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_SCRIPT_RUN
		, ::gpk::failed(::gpk::controlTableScriptRun(controls, R"({"target":{"Draw":{"ColorMode":2}}})"))
		);
	GPK_TEST_CHECK(errors, GUI_JSON_TEST_RESULT_SCRIPT_LITERAL, controls.Draw[target].ColorMode != ::gpk::GUI_COLOR_MODE_Flat);

	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_SCRIPT_RUN
		, ::gpk::failed(::gpk::controlTableScriptRun(controls, R"({"target":{"Draw":{"ColorTheme":"{source.Draw.ColorTheme}"}}})"))
		);
	GPK_TEST_CHECKF(errors, GUI_JSON_TEST_RESULT_SCRIPT_EXPRESSION, controls.Draw[target].ColorTheme != controls.Draw[source].ColorTheme
		, "Source:%i, target:%i.", controls.Draw[source].ColorTheme, controls.Draw[target].ColorTheme
		);

	controls.Script[action] = R"({"target":{"Placement":{"Align":"BOTTOM_RIGHT"}}})";
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_SCRIPT_RUN, ::gpk::failed(::gpk::controlTableScriptRun(controls, action)));
	GPK_TEST_CHECK(errors, GUI_JSON_TEST_RESULT_SCRIPT_STORED, controls.Placement[target].Align != ::gpk::ALIGN_BOTTOM_RIGHT);

	controls.Script[action] = R"({"target":{"State":{"Flag":1}}})";
	controls.SetAction(action, true);
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_SCRIPT_RUN, ::gpk::failed(::gpk::controlTableScriptUpdate(controls)));
	GPK_TEST_CHECK(errors, GUI_JSON_TEST_RESULT_SCRIPT_ACTION, false == controls.States[target].IsHidden());

	controls.EventQueue.clear();
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_SCRIPT_RUN
		, ::gpk::failed(::gpk::controlTableScriptRun(controls, R"({"framework":{"event":{"Type":5,"Data":[9,8,7]}}})"))
		);
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_SCRIPT_EVENT, 1 != controls.EventQueue.size());
	::gpk::SEvent<::gpk::u0_t> childEvent = {};
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_SCRIPT_EVENT, ::gpk::failed(controls.EventQueue[0]->ExtractChild(childEvent)));
	GPK_TEST_CHECK(errors, GUI_JSON_TEST_RESULT_SCRIPT_EVENT
		, 5 != childEvent.Type || 3 != childEvent.Data.size() || 9 != childEvent.Data[0] || 8 != childEvent.Data[1] || 7 != childEvent.Data[2]
		);
	rtrn 0;
}

sttc ::gpk::err_t testControlStateJSONRoundTrip(::gpk::ATestError & errors) {
	::gpk::SControlTable controls = {};
	cnst ::gpk::cid_t idSource = ::gpk::controlCreate(controls, {});
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_CONTROL_CREATE, ::gpk::failed(idSource));
	cnst ::gpk::cid_t idTarget = ::gpk::controlCreate(controls, {});
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_CONTROL_CREATE, ::gpk::failed(idTarget));

	::gpk::SControlState & source = controls.States[idSource];
	source.Parent = 42;
	source.Flag = (::gpk::GUI_CONTROL_FLAG)(::gpk::GUI_CONTROL_FLAG_Hidden | ::gpk::GUI_CONTROL_FLAG_Focused | ::gpk::GUI_CONTROL_FLAG_Selected);
	source.Mask = (::gpk::GUI_CONTROL_FLAG)(::gpk::GUI_CONTROL_FLAG_Hidden | ::gpk::GUI_CONTROL_FLAG_Focused | ::gpk::GUI_CONTROL_FLAG_Selected | ::gpk::GUI_CONTROL_FLAG_Action);

	cnst ::gpk::SControlState & blank = controls.States[idTarget];
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_TARGET_DEFAULT
		, blank.Parent == source.Parent || blank.Flag == source.Flag || blank.Mask == source.Mask
		);

	::gpk::string json = {};
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_STATE_SAVE, ::gpk::failed(::gpk::saveCTTI(json, source)));
	::gpk::vcst_t jsonInput = json;
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_STATE_LOAD, ::gpk::failed(::gpk::loadCTTI(jsonInput, controls.States[idTarget])));
	GPK_TEST_CHECK(errors, GUI_JSON_TEST_RESULT_INPUT_CURSOR, jsonInput.size());

	cnst ::gpk::SControlState & target = controls.States[idTarget];
	GPK_TEST_CHECKF(errors, GUI_JSON_TEST_RESULT_STATE_VALUE
		, target.Parent != source.Parent || target.Flag != source.Flag || target.Mask != source.Mask
		, "JSON:'%.*s'; loaded:{Parent:%i,Flag:0x%X,Mask:0x%X}; expected:{Parent:%i,Flag:0x%X,Mask:0x%X}."
		, GPK_VIEW_ARGS(json)
		, target.Parent, (::gpk::u2_t)target.Flag, (::gpk::u2_t)target.Mask
		, source.Parent, (::gpk::u2_t)source.Flag, (::gpk::u2_t)source.Mask
		);
	rtrn 0;
}

sttc ::gpk::err_t testControlTextJSONRoundTrip(::gpk::ATestError & errors) {
	::gpk::SControlTable controls = {};
	cnst ::gpk::cid_t idSource = ::gpk::controlCreate(controls, {});
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_CONTROL_CREATE, ::gpk::failed(idSource));
	cnst ::gpk::cid_t idTarget = ::gpk::controlCreate(controls, {});
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_CONTROL_CREATE, ::gpk::failed(idTarget));

	::gpk::SControlText & source = controls.Text[idSource];
	source.Text = "CTTI table round trip";
	source.Align = ::gpk::ALIGN_BOTTOM_RIGHT;
	source.Selection = {3, 11};
	source.FontSelected = 7;

	cnst ::gpk::SControlText & blank = controls.Text[idTarget];
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_TARGET_DEFAULT
		, blank.Text == source.Text || blank.Align == source.Align || blank.Selection.Min == source.Selection.Min || blank.Selection.Max == source.Selection.Max || blank.FontSelected == source.FontSelected
		);

	::gpk::string json = {};
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_TEXT_SAVE, ::gpk::failed(::gpk::saveCTTI(json, source)));
	::gpk::vcst_t jsonInput = json;
	GPK_TEST_REQUIRE(errors, GUI_JSON_TEST_RESULT_TEXT_LOAD, ::gpk::failed(::gpk::loadCTTI(jsonInput, controls.Text[idTarget])));
	GPK_TEST_CHECK(errors, GUI_JSON_TEST_RESULT_INPUT_CURSOR, jsonInput.size());

	cnst ::gpk::SControlText & target = controls.Text[idTarget];
	GPK_TEST_CHECKF(errors, GUI_JSON_TEST_RESULT_TEXT_VALUE
		, target.Text != source.Text || target.Align != source.Align || target.Selection.Min != source.Selection.Min || target.Selection.Max != source.Selection.Max || target.FontSelected != source.FontSelected
		, "JSON:'%.*s'; loaded:{Text:'%.*s',Align:0x%X,Selection:{%u,%u},Font:%i}; expected:{Text:'%.*s',Align:0x%X,Selection:{%u,%u},Font:%i}."
		, GPK_VIEW_ARGS(json)
		, GPK_VIEW_ARGS(target.Text), (::gpk::u2_t)target.Align, target.Selection.Min, target.Selection.Max, target.FontSelected
		, GPK_VIEW_ARGS(source.Text), (::gpk::u2_t)source.Align, source.Selection.Min, source.Selection.Max, source.FontSelected
		);
	rtrn 0;
}

::gpk::err_t test_gui_json(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t) {
	if_fail_fe(::testControlStateJSONRoundTrip(errors));
	if_fail_fe(::testControlTextJSONRoundTrip(errors));
	if_fail_fe(::testControlTableJSONBinaryParity(errors));
	if_fail_fe(::testControlTableJSONScript(errors));
	rtrn 0;
}
