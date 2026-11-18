#include "gpk_gui.h"
#include "gpk_apod_serialize.h"
#include "test_llc_gui_control_corpus.h"
#include "test_llc_suites.h"

static_assert(sizeof(::gpk::SControlState) == 6, "The shared wire format requires the packed SControlState layout.");
static_assert(sizeof(::gpk::STextState) == 6, "The shared wire format requires the STextState layout.");
static_assert(sizeof(::gpk::SControlConstraints) == 22, "The shared wire format requires the packed SControlConstraints layout.");
static_assert(sizeof(::gpk::SControlPlacement) == 28, "The shared wire format requires the packed SControlPlacement layout.");
static_assert(sizeof(::gpk::SControlArea) == 176, "The shared wire format requires the packed SControlArea layout.");
static_assert(sizeof(::gpk::SControlDraw) == 29, "The shared wire format requires the packed SControlDraw layout.");

GDEFINE_ENUM_TYPE(GUI_CONTROL_SERIALIZATION_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, SAVE				, 0, "SControlTable::Save() rejected a valid table.");
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, PACKED_COUNT		, 1, "The 64-control count did not use the expected packed-u32 boundary representation.");
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, LOAD				, 2, "SControlTable::Load() rejected its own serialized table.");
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, CONSUMPTION		, 3, "SControlTable::Load() did not consume exactly one table.");
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, PARALLEL_ARRAYS	, 4, "The loaded control-table arrays do not share the serialized slot count.");
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, STATE				, 5, "Control state did not round-trip or computed state remained marked current.");
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, TEXT_STATE		, 6, "Per-character text state did not round-trip.");
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, STRINGS			, 7, "Identifier, script or text did not round-trip.");
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, COMPONENTS		, 8, "Constraints, placement, drawing or text metadata did not round-trip.");
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, IMAGE				, 9, "Control image data entered the table stream or its dimensions were not represented.");
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, HIERARCHY			, 10, "Control hierarchy or child order did not round-trip.");
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, TABLE_MEMBERS		, 11, "Area, event queue or exact control flags did not round-trip.");
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, TRUNCATED_REJECT	, 12, "The loader accepted truncated input.");
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, PACKED_BOUNDARIES	, 13, "A packed-u32 boundary value did not round-trip with the expected byte width.");
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, PACKED_TRUNCATED	, 14, "loadPacked() accepted a count with a missing tail byte.");
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, CORPUS_SETUP		, 15, "The complete GUI corpus could not be constructed.");
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, CORPUS_STRUCTURE	, 16, "The complete GUI corpus did not preserve its authored structure.");
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, CORPUS_STRINGS		, 17, "A loaded corpus text view does not borrow the retained serialized bytes.");
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, CORPUS_IMAGES		, 18, "A separately loaded corpus image did not survive table loading.");
GDEFINE_ENUM_VALUED(GUI_CONTROL_SERIALIZATION_TEST_RESULT, CORPUS_CONSUMPTION	, 19, "A corpus round trip left unread bytes or changed its byte stream.");

sttc ::gpk::err_t testPackedCounts(::gpk::ATestError & errors) {
	cnst ::gpk::u2_t values[] = {0, 0x3F, 0x40, 0x3FFF, 0x4000, 0x3FFFFF, 0x400000, 0x3FFFFFFF};
	cnst ::gpk::u0_t widths[] = {1, 1, 2, 2, 3, 3, 4, 4};
	for(::gpk::u2_t iValue = 0; iValue < ::gpk::size(values); ++iValue) {
		::gpk::au0_t encoded = {};
		cnst ::gpk::packedu2_t packed = values[iValue];
		cnst ::gpk::err_t saveResult = ::gpk::savePacked(encoded, packed);
		GPK_TEST_CHECKF(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_PACKED_BOUNDARIES
			, ::gpk::failed(saveResult) || encoded.size() != widths[iValue]
			, "Value: %u, save result: %i, encoded width: %u, expected width: %u."
			, values[iValue], saveResult, encoded.size(), widths[iValue]
			);
		if(::gpk::failed(saveResult) || encoded.size() != widths[iValue])
			continue;
		::gpk::vcu0_t input = encoded;
		::gpk::u2_t decoded = {};
		cnst ::gpk::err_t loadResult = ::gpk::loadPacked(input, decoded);
		GPK_TEST_CHECKF(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_PACKED_BOUNDARIES
			, ::gpk::failed(loadResult) || decoded != values[iValue] || input.size()
			, "Value: %u, load result: %i, decoded: %u, remaining bytes: %u."
			, values[iValue], loadResult, decoded, input.size()
			);
		if(widths[iValue] > 1) {
			::gpk::vcu0_t truncated = {encoded.begin(), encoded.size() - 1};
			decoded = {};
			GPK_TEST_CHECKF(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_PACKED_TRUNCATED
				, false == ::gpk::failed(::gpk::loadPacked(truncated, decoded))
				, "Value: %u, encoded width: %u."
				, values[iValue], encoded.size()
				);
		}
	}
	return 0;
}

sttc ::gpk::err_t controlTableCreate64(::gpk::SControlTable & table) {
	for(::gpk::u2_t iControl = 0; iControl < 64; ++iControl)
		if_fail_fef(::gpk::controlCreate(table, {}) , "iControl: %u", iControl);
	return 0;
}

sttc ::gpk::err_t controlTablePopulate(::gpk::SControlTable & table) {
	if_fail_fe(::controlTableCreate64(table));
	if_fail_fe(::gpk::controlSetParent(table, 1, 0));
	if_fail_fe(::gpk::controlSetParent(table, 2, 0));
	table.SetUnused(10, true);

	table.States[0].Flag = (::gpk::GUI_CONTROL_FLAG)(::gpk::GUI_CONTROL_FLAG_Selected | ::gpk::GUI_CONTROL_FLAG_Hovered | ::gpk::GUI_CONTROL_FLAG_Updated);
	table.States[0].Mask = (::gpk::GUI_CONTROL_FLAG)(::gpk::GUI_CONTROL_FLAG_Hidden | ::gpk::GUI_CONTROL_FLAG_Disabled | ::gpk::GUI_CONTROL_FLAG_Hovered | ::gpk::GUI_CONTROL_FLAG_Selected | ::gpk::GUI_CONTROL_FLAG_Updated);
	table.TextState[0].push_back({-1, ::gpk::GUI_TEXT_Bold, ::gpk::GUI_TEXT_FLAG_All});
	table.TextState[0].push_back({0, (::gpk::GUI_TEXT_FLAG)(::gpk::GUI_TEXT_Selected | ::gpk::GUI_TEXT_Italic), ::gpk::GUI_TEXT_FLAG_All});
	table.Identifier[0] = ::gpk::vcst_t{"root_control"};
	table.Script[0] = "{\"root_control\":{\"Text\":{\"Text\":\"restored\"}}}";

	::gpk::SControlConstraints & constraints = table.Constraints[0];
	constraints.AttachSizeToControl = {1, 2};
	constraints.AttachSizeToText = {true, false};
	constraints.DockToControl = {3, 4, 5, 6};
	constraints.SizeMinMax = {{7, 8}, {900, 700}};

	::gpk::SControlPlacement & placement = table.Placement[0];
	placement.Area = {{11, 12}, {320, 180}};
	placement.Margin = {13, 14, 15, 16};
	placement.Border = {17, 18, 19, 20};
	placement.Align = ::gpk::ALIGN_BOTTOM_RIGHT;
	placement.RelativeBorderSize = ::gpk::GUI_CONTROL_LIMIT_Right;
	placement.RelativeMarginSize = ::gpk::GUI_CONTROL_LIMIT_Bottom;
	placement.RelativeMetrics = (::gpk::GUI_CONTROL_METRICS_FLAG)(::gpk::GUI_CONTROL_METRICS_FLAG_XSize | ::gpk::GUI_CONTROL_METRICS_FLAG_YOffset);
	table.Area[0].Total.Local = {{23, 24}, {25, 26}};
	table.Area[0].Client.Global = {{27, 28}, {29, 30}};
	table.Area[0].Text = {{31, 32}, {33, 34}};
	table.Area[0].Rectangles[0] = {{35, 36}, {37, 38}};
	table.Area[0].Triangles[0] = {{39, 40}, {41, 42}, {43, 44}};

	::gpk::SControlDraw & draw = table.Draw[0];
	draw.ColorMode = ::gpk::GUI_COLOR_MODE_Flat;
	draw.FrameOut = true;
	draw.NoBorder = true;
	draw.NoClient = false;
	draw.UseNewPalettes = true;
	draw.ColorTheme = 21;
	for(::gpk::u2_t iPalette = 0; iPalette < draw.Palettes.size(); ++iPalette)
		draw.Palettes[iPalette] = (::gpk::s1_t)(100 + iPalette);

	::gpk::SControlText & text = table.Text[0];
	text.Text = ::gpk::vcst_t{"Serialized GUI text"};
	text.Align = ::gpk::ALIGN_CENTER_LEFT;
	text.Selection = {3, 9};
	text.FontSelected = 4;

	::gpk::SControlImage & image = table.Images[0];
	image.ImageOffset = {5, 6};
	image.ImageAlign = ::gpk::ALIGN_TOP_RIGHT;
	image.ImageInvertX = true;
	image.ImageInvertY = false;
	image.Rotation = 3;
	cnst ::gpk::n2u2_t imageMetrics = {2, 1};
	if_fail_fe(image.Temp.Color.resize(imageMetrics));
	image.Temp.Color.Texels[0] = ::gpk::bgra{1, 2, 3, 4};
	image.Temp.Color.Texels[1] = ::gpk::bgra{5, 6, 7, 8};
	image.Image = image.Temp.Color.View;
	if_fail_fe(table.EventQueue.resize(table.EventQueue.size() + 1));
	return 0;
}

sttc bool viewBorrowsBytes(::gpk::vcst_t text, ::gpk::vcu0_t bytes) {
	if(0 == text.size())
		return true;
	cnst ::gpk::u0_t * textBegin = (cnst ::gpk::u0_t*)text.begin();
	cnst ::gpk::u0_t * textEnd = (cnst ::gpk::u0_t*)text.end();
	return textBegin >= bytes.begin() && textEnd <= bytes.end();
}

struct SControlCorpus {
	::gpk::SGUI												GUI				= {};
	::gpk::cid_t												Root			= ::gpk::CID_INVALID;
	::dry::SScreen<::dry::SCREEN_CONTROL_HUD_COUNT>					Hud				= {};
	::dry::SScreen<::dry::SCREEN_CONTROL_MENU_COUNT>					Menu			= {};
	::dry::SScreen<::dry::SCREEN_CONTROL_MODAL_COUNT>					Modal			= {};
	::dry::SScreen<::dry::SCREEN_CONTROL_SPOOL_INFO_COUNT>			SpoolInfo		= {};
	::dry::SScreen<::dry::SCREEN_CONTROL_LOAD_FILAMENT_COUNT>		LoadFilament	= {};
	::dry::SScreen<::dry::SCREEN_CONTROL_INFO_COUNT>					Info			= {};
	::dry::SScreen<::dry::SCREEN_CONTROL_CONFIG_COUNT>				Config			= {};
};

sttc ::gpk::err_t setupCorpus(SControlCorpus & corpus) {
	if_fail_fe(corpus.Root = ::gpk::controlCreate(corpus.GUI));
	corpus.GUI.Controls.Identifier[corpus.Root] = ::gpk::vcst_t{"corpus_root"};
	cnst ::gpk::vs1_t defaultColors = corpus.GUI.Colors->DefaultColors;
	if_fail_fe(::dry::setupScreenFromControlParams(corpus.GUI.Controls, corpus.Root, corpus.Hud         , ::dry::control_list_hud          , defaultColors, false));
	if_fail_fe(::dry::setupScreenFromControlParams(corpus.GUI.Controls, corpus.Root, corpus.Menu        , ::dry::control_list_menu         , defaultColors, false));
	if_fail_fe(::dry::setupScreenFromControlParams(corpus.GUI.Controls, corpus.Root, corpus.Modal       , ::dry::control_list_modal        , defaultColors, false));
	if_fail_fe(::dry::setupScreenFromControlParams(corpus.GUI.Controls, corpus.Root, corpus.SpoolInfo   , ::dry::control_list_spool_info   , defaultColors, false));
	if_fail_fe(::dry::setupScreenFromControlParams(corpus.GUI.Controls, corpus.Root, corpus.LoadFilament, ::dry::control_list_load_filament, defaultColors, false));
	if_fail_fe(::dry::setupScreenFromControlParams(corpus.GUI.Controls, corpus.Root, corpus.Info        , ::dry::control_list_info         , defaultColors, false));
	if_fail_fe(::dry::setupScreenFromControlParams(corpus.GUI.Controls, corpus.Root, corpus.Config      , ::dry::control_list_config       , defaultColors, false));
	if_fail_fe(::dry::setupPalettes(*corpus.GUI.Colors));
	return 0;
}

sttc bool bytesEqual(::gpk::vcu0_t left, ::gpk::vcu0_t right) {
	if(left.size() != right.size())
		return false;
	for(::gpk::u2_t iByte = 0; iByte < left.size(); ++iByte)
		if(left[iByte] != right[iByte])
			return false;
	return true;
}

sttc ::gpk::err_t testCorpusRoundTrip(::gpk::ATestError & errors) {
	SControlCorpus source = {};
	GPK_TEST_REQUIRE(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_CORPUS_SETUP, ::gpk::failed(::setupCorpus(source)));
	cnst ::gpk::cid_t sourceDoor = source.Hud.ControlIds[::dry::SCREEN_CONTROL_HUD_DOOR];
	::gpk::img8bgra & sourceDoorStorage = source.Hud.ControlIcons[::dry::SCREEN_CONTROL_HUD_DOOR].Off;
	GPK_TEST_REQUIRE(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_CORPUS_IMAGES, ::gpk::failed(sourceDoorStorage.resize(::gpk::n2u2_t{1, 1})));
	sourceDoorStorage[0][0] = ::gpk::bgra{1, 2, 3, 4};
	source.GUI.Controls.Images[sourceDoor].Image = sourceDoorStorage.View;
	cnst ::gpk::u2_t expectedControlCount
		= 1
		+ ::gpk::size(::dry::control_list_hud)
		+ ::gpk::size(::dry::control_list_menu)
		+ ::gpk::size(::dry::control_list_modal)
		+ ::gpk::size(::dry::control_list_spool_info)
		+ ::gpk::size(::dry::control_list_load_filament)
		+ ::gpk::size(::dry::control_list_info)
		+ ::gpk::size(::dry::control_list_config)
		;
	cnst ::gpk::cid_t sourceSpoolDialog = source.SpoolInfo.ControlIds[::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG];
	cnst ::gpk::cid_t sourceSpoolTitle = source.SpoolInfo.ControlIds[::dry::SCREEN_CONTROL_SPOOL_INFO_TITLE];
	GPK_TEST_CHECKF(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_CORPUS_STRUCTURE
		, source.GUI.Controls.States.size() != expectedControlCount
		|| source.GUI.Controls.Children[source.Root].size() != ::dry::SCREEN_COUNT - 1
		|| source.GUI.Controls.States[sourceSpoolDialog].Parent != source.Root
		|| source.GUI.Controls.States[sourceSpoolTitle].Parent != sourceSpoolDialog
		|| source.GUI.Controls.Text[sourceSpoolTitle].Text != ::gpk::vcst_t{"Spool Scanned!"}
		, "Controls: %u; expected: %u; root children: %u."
		, source.GUI.Controls.States.size(), expectedControlCount, source.GUI.Controls.Children[source.Root].size()
		);

	::gpk::au0_t serialized = {};
	GPK_TEST_REQUIRE(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_SAVE, ::gpk::failed(source.GUI.Controls.Save(serialized)));
	SControlCorpus loaded = {};
	GPK_TEST_REQUIRE(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_CORPUS_SETUP, ::gpk::failed(::setupCorpus(loaded)));
	cnst ::gpk::cid_t loadedDoor = loaded.Hud.ControlIds[::dry::SCREEN_CONTROL_HUD_DOOR];
	::gpk::img8bgra & loadedDoorStorage = loaded.Hud.ControlIcons[::dry::SCREEN_CONTROL_HUD_DOOR].Off;
	GPK_TEST_REQUIRE(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_CORPUS_IMAGES, ::gpk::failed(loadedDoorStorage.resize(::gpk::n2u2_t{1, 1})));
	::gpk::SControlImage & separatelyLoadedImage = loaded.GUI.Controls.Images[loadedDoor];
	cnst ::gpk::bgra separatelyLoadedMarker = {17, 34, 51, 68};
	loadedDoorStorage[0][0] = separatelyLoadedMarker;
	separatelyLoadedImage.Image = loadedDoorStorage.View;
	cnst ::gpk::n2u2_t separatelyLoadedMetrics = separatelyLoadedImage.Image.metrics();
	separatelyLoadedImage.ImageAlign = ::gpk::ALIGN_BOTTOM_RIGHT;

	::gpk::vcu0_t input = serialized;
	GPK_TEST_REQUIRE(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_LOAD, ::gpk::failed(loaded.GUI.Controls.Load(input)));
	GPK_TEST_CHECKF(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_CORPUS_CONSUMPTION, input.size(), "Corpus unread bytes: %u.", input.size());
	GPK_TEST_CHECK(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_CORPUS_IMAGES
		, loaded.GUI.Controls.Images[loadedDoor].Image.metrics() != separatelyLoadedMetrics
		|| loaded.GUI.Controls.Images[loadedDoor].Image[0][0] != separatelyLoadedMarker
		|| loaded.GUI.Controls.Images[loadedDoor].ImageAlign != ::gpk::ALIGN_BOTTOM_RIGHT
		);
	::gpk::au0_t serializedAgain = {};
	GPK_TEST_REQUIRE(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_SAVE, ::gpk::failed(loaded.GUI.Controls.Save(serializedAgain)));
	GPK_TEST_CHECKF(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_CORPUS_CONSUMPTION
		, false == ::bytesEqual(serialized, serializedAgain)
		, "Original bytes: %u; second save bytes: %u.", serialized.size(), serializedAgain.size()
		);
	cnst ::gpk::vcu0_t serializedBytes = serialized;
	for(::gpk::u2_t iControl = 0; iControl < loaded.GUI.Controls.Text.size(); ++iControl) {
		GPK_TEST_CHECKF(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_CORPUS_STRINGS
			, false == ::viewBorrowsBytes(loaded.GUI.Controls.Text[iControl].Text, serializedBytes)
			, "Corpus control: %u; text length: %u.", iControl, loaded.GUI.Controls.Text[iControl].Text.size()
			);
		GPK_TEST_CHECKF(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_CORPUS_STRINGS
			, false == ::viewBorrowsBytes(loaded.GUI.Controls.Identifier[iControl], serializedBytes)
			, "Corpus control: %u; identifier length: %u.", iControl, loaded.GUI.Controls.Identifier[iControl].size()
			);
	}
	return 0;
}

::gpk::err_t test_gui_control_serialization(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	if_fail_fe(::testPackedCounts(errors));
	if_fail_fe(::testCorpusRoundTrip(errors));
	::gpk::SGUI sourceGUI = {};
	::gpk::SControlTable & source = sourceGUI.Controls;
	if_fail_fe(::controlTablePopulate(source));
	::gpk::au0_t serialized = {};
	GPK_TEST_REQUIRE(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_SAVE, ::gpk::failed(sourceGUI.Controls.Save(serialized)));
	GPK_TEST_REQUIRE(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_PACKED_COUNT, serialized.size() < 2);
	GPK_TEST_CHECKF(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_PACKED_COUNT, serialized[0] != 1 || serialized[1] != 64, "Packed count bytes: 0x%02X 0x%02X; expected: 0x01 0x40.", serialized[0], serialized[1]);
	::gpk::au0_t serializedWithoutImage = {};
	source.Images[0].Image = {};
	GPK_TEST_REQUIRE(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_SAVE, ::gpk::failed(sourceGUI.Controls.Save(serializedWithoutImage)));
	source.Images[0].Image = source.Images[0].Temp.Color.View;
	::gpk::u2_t imageMetricByteDifferences = 0;
	for(::gpk::u2_t iByte = 0, byteCount = ::gpk::min(serialized.size(), serializedWithoutImage.size()); iByte < byteCount; ++iByte)
		imageMetricByteDifferences += serialized[iByte] != serializedWithoutImage[iByte];
	GPK_TEST_CHECKF(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_IMAGE
		, serialized.size() != serializedWithoutImage.size() || imageMetricByteDifferences != 2
		, "Serialized bytes with image: %u; without image: %u; differing W/H bytes: %u."
		, serialized.size(), serializedWithoutImage.size(), imageMetricByteDifferences
		);

	::gpk::SGUI restoredGUI = {};
	::gpk::SControlTable & restored = restoredGUI.Controls;
	if_fail_fe(::controlTableCreate64(restored));
	cnst ::gpk::n2u2_t separatelyLoadedImageMetrics = {1, 1};
	cnst ::gpk::n2s1_t separatelyLoadedImageOffset = {45, 46};
	cnst ::gpk::bgra separatelyLoadedPixel = {9, 10, 11, 12};
	if_fail_fe(restored.Images[0].Temp.Color.resize(separatelyLoadedImageMetrics));
	restored.Images[0].Temp.Color.Texels[0] = separatelyLoadedPixel;
	restored.Images[0].Image = restored.Images[0].Temp.Color.View;
	restored.Images[0].ImageOffset = separatelyLoadedImageOffset;
	restored.Images[0].ImageAlign = ::gpk::ALIGN_BOTTOM_LEFT;
	restored.Images[0].ImageInvertY = true;
	restored.Images[0].Rotation = 2;
	::gpk::vcu0_t input = serialized;
	GPK_TEST_REQUIRE(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_LOAD, ::gpk::failed(restoredGUI.Controls.Load(input)));
	GPK_TEST_CHECKF(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_CONSUMPTION, input.size(), "Remaining bytes: %u.", input.size());
	GPK_TEST_REQUIRE(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_PARALLEL_ARRAYS, restored.States.size() != 64);
	GPK_TEST_CHECK(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_PARALLEL_ARRAYS
		, restored.TextState.size() != 64 || restored.Identifier.size() != 64 || restored.Script.size() != 64
		|| restored.Constraints.size() != 64 || restored.Placement.size() != 64 || restored.Area.size() != 64
		|| restored.Draw.size() != 64 || restored.Text.size() != 64 || restored.Images.size() != 64 || restored.Children.size() != 64
		);
	GPK_TEST_CHECK(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_STATE
		, restored.States[0].Parent != source.States[0].Parent
		|| restored.States[0].Mask != source.States[0].Mask
		|| false == restored.States[0].IsSelected() || false == restored.States[0].IsHovered() || false == restored.States[0].IsUpdated()
		|| false == restored.States[10].IsUnused()
		);
	GPK_TEST_CHECK(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_TEXT_STATE
		, restored.TextState[0].size() != 2
		|| restored.TextState[0][0].Parent != -1 || false == restored.TextState[0][0].IsBold()
		|| restored.TextState[0][1].Parent != 0 || false == restored.TextState[0][1].IsSelected() || false == restored.TextState[0][1].IsItalic()
		);
	GPK_TEST_CHECK(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_STRINGS
		, restored.Identifier[0] != source.Identifier[0] || restored.Script[0] != source.Script[0] || restored.Text[0].Text != source.Text[0].Text
		|| (cnst ::gpk::u0_t*)restored.Identifier[0].begin() < serialized.begin() || (cnst ::gpk::u0_t*)restored.Identifier[0].end() > serialized.end()
		|| (cnst ::gpk::u0_t*)restored.Text[0].Text.begin() < serialized.begin() || (cnst ::gpk::u0_t*)restored.Text[0].Text.end() > serialized.end()
		);
	GPK_TEST_CHECK(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_COMPONENTS
		, restored.Constraints[0].AttachSizeToControl != source.Constraints[0].AttachSizeToControl
		|| restored.Constraints[0].DockToControl != source.Constraints[0].DockToControl
		|| restored.Placement[0].Area != source.Placement[0].Area || restored.Placement[0].Align != source.Placement[0].Align
		|| restored.Draw[0].ColorTheme != source.Draw[0].ColorTheme || restored.Draw[0].Palettes != source.Draw[0].Palettes
		|| restored.Text[0].Selection != source.Text[0].Selection || restored.Text[0].FontSelected != source.Text[0].FontSelected
		);
	cnst ::gpk::n2u2_t sourceImageMetrics = source.Images[0].Image.metrics();
	cnst ::gpk::n2u2_t expectedImageMetrics = {2, 1};
	GPK_TEST_CHECKF(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_IMAGE
		, sourceImageMetrics != expectedImageMetrics || restored.Images[0].Image.metrics() != separatelyLoadedImageMetrics
		|| restored.Images[0].Image[0][0] != separatelyLoadedPixel
		|| restored.Images[0].ImageOffset != separatelyLoadedImageOffset || restored.Images[0].ImageAlign != ::gpk::ALIGN_BOTTOM_LEFT
		|| restored.Images[0].ImageInvertX || false == restored.Images[0].ImageInvertY || restored.Images[0].Rotation != 2
		, "Source metrics: %u x %u; restored texel count: %u; restored offset: %i, %i; align: %u; invert: %u, %u; rotation: %u."
		, sourceImageMetrics.x, sourceImageMetrics.y, restored.Images[0].Image.size()
		, restored.Images[0].ImageOffset.x, restored.Images[0].ImageOffset.y, restored.Images[0].ImageAlign
		, (::gpk::u2_t)restored.Images[0].ImageInvertX, (::gpk::u2_t)restored.Images[0].ImageInvertY, (::gpk::u2_t)restored.Images[0].Rotation
		);
	GPK_TEST_CHECK(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_HIERARCHY
		, restored.States[1].Parent != 0 || restored.States[2].Parent != 0
		|| restored.Children[0].size() != 2 || restored.Children[0][0] != 1 || restored.Children[0][1] != 2
		);
	GPK_TEST_CHECK(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_TABLE_MEMBERS
		, restored.Area[0].Total.Local != source.Area[0].Total.Local
		|| restored.Area[0].Client.Global != source.Area[0].Client.Global
		|| restored.Area[0].Text != source.Area[0].Text
		|| restored.Area[0].Rectangles[0] != source.Area[0].Rectangles[0]
		|| restored.Area[0].Triangles[0] != source.Area[0].Triangles[0]
		|| restored.EventQueue.size() != source.EventQueue.size()
		|| restored.EventQueue.size() && (restored.EventQueue[0]->Type != source.EventQueue[0]->Type || restored.EventQueue[0]->Data != source.EventQueue[0]->Data)
		|| restored.EventQueue.size() < 2 || 0 == (cnst ::gpk::SEventSystem*)restored.EventQueue[1]
		|| restored.EventQueue[1]->Type != ::gpk::SYSTEM_EVENT_Runtime || restored.EventQueue[1]->Data.size()
		);

	::gpk::SGUI truncatedGUI = {};
	::gpk::vcu0_t truncatedInput = {serialized.begin(), serialized.size() - 1};
	GPK_TEST_CHECK(errors, GUI_CONTROL_SERIALIZATION_TEST_RESULT_TRUNCATED_REJECT
		, ::gpk::succeeded(truncatedGUI.Controls.Load(truncatedInput))
		);
	return 0;
}
