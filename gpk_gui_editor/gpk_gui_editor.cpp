#include "gpk_gui_editor.h"
#include "gpk_app_impl.h"
#include "gpk_cstring.h"
#include "gpk_framework.h"
#include "gpk_gui_theme.h"
#include "gpk_dialog_controls.h"
#include "gpk_file.h"
#include "gpk_label.h"
#include "gpk_png.h"
#include "gpk_string_compose.h"

usns gpk;

nmsp sample_gui {
	GDEFINE_ENUM_CTTI(INSPECTOR_gpk_ctti);
	GDEFINE_ENUM_CTTI(WORKSPACE_ACTION_gpk_ctti);
	GDEFINE_ENUM_CTTI(EDITOR_ACTION_gpk_ctti);

	CTTI_ENUM_COMPILED(INSPECTOR_FIELD, u0_t, ID, STATE, PALETTE, ALIGN, TEXT);
	GDEFINE_ENUM_CTTI(INSPECTOR_FIELD_gpk_ctti);

	CTTI_ENUM_COMPILED(ACTION, u0_t, SCREENSHOT, EXIT);
	GDEFINE_ENUM_CTTI(ACTION_gpk_ctti);
}

sttc err_t sampleGUIControlCreate(SSampleGUI & app, cid_t & idControl, vcst_t text, cnst rect2s1_t & area, ALIGN align = ALIGN_TOP_LEFT) {
	if_fail_fe(idControl = controlCreate(*app.Dialog->GUI, [=]
		( SControlState &, SControlState &, SControlPlacement & placement, SControlArea &
		, SControlConstraints &, SControlDraw & draw, SControlText & controlText, SControlImage &
		) -> err_t {
			placement.Area		= area;
			placement.Align		= align;
			placement.Margin	= {2, 2, 2, 2};
			placement.Border	= {3, 3, 3, 3};
			draw.ColorMode		= GUI_COLOR_MODE_Default;
			controlText.Text	= text;
			rtrn 0;
		}));
	rtrn 0;
}

sttc err_t sampleGUIGeometryModeApply(SSampleGUI & app) {
	for(u2_t iControl = 0; iControl < app.Dialog->GUI->Controls.Draw.size(); ++iControl)
		app.Dialog->GUI->Controls.Draw[iControl].UseNewPalettes = app.UseNewPalettes;
	rtrn controlTextSet(*app.Dialog->GUI, app.ControlGeometry, app.UseNewPalettes ? vcst_t{"Areas"} : vcst_t{"Indexed"});
}

sttc u2_t sampleGUIPaletteToneCount(cnst SSampleGUI & app) nxpt {
	if(!app.Dialog->GUI->Colors || !app.Dialog->GUI->Colors->ControlThemes)
		rtrn 0;
	rtrn app.Dialog->GUI->Colors->ControlThemes->size() / ASCII_COLOR_COUNT;
}

sttc u2_t sampleGUIPaletteToneRowCount(cnst SSampleGUI & app, cnst u2_t toneCount, cnst u2_t iColumn) nxpt {
	cnst u2_t columnCount = size(app.PalettePicker.IndexGridTones);
	cnst u2_t columnSize = ASCII_COLOR_COUNT / columnCount;
	cnst u2_t completeGroupRows = toneCount / ASCII_COLOR_COUNT * columnSize;
	cnst u2_t remainingTones = toneCount % ASCII_COLOR_COUNT;
	rtrn completeGroupRows + (remainingTones + columnCount - 1 - iColumn) / columnCount;
}

sttc u2_t sampleGUIPaletteToneFromCell(cnst SSampleGUI & app, cnst n2u2_t & cell, cnst u2_t toneCount) nxpt {
	cnst u2_t columnCount = size(app.PalettePicker.IndexGridTones);
	cnst u2_t columnSize = ASCII_COLOR_COUNT / columnCount;
	cnst u2_t completeGroups = toneCount / ASCII_COLOR_COUNT;
	cnst u2_t completeGroupRows = completeGroups * columnSize;
	if(cell.y >= completeGroupRows)
		rtrn completeGroups * ASCII_COLOR_COUNT + (cell.y - completeGroupRows) * columnCount + cell.x;
	cnst u2_t toneGroup = cell.y / columnSize;
	cnst u2_t toneRow = cell.y % columnSize;
	rtrn toneGroup * ASCII_COLOR_COUNT + toneRow + cell.x * columnSize;
}

sttc void sampleGUIPaletteToneSelect(SSampleGUI & app, cnst cid_t idSelected) nxpt {
	cnst u2_t toneCount = ::sampleGUIPaletteToneCount(app);
	for(u2_t iColumn = 0; iColumn < size(app.PalettePicker.IndexGridTones); ++iColumn)
	for(u2_t iRow = 0; iRow < ::sampleGUIPaletteToneRowCount(app, toneCount, iColumn); ++iRow)
	for(u2_t iShade = 0; iShade < ASCII_COLOR_COUNT; ++iShade) {
		cnst cid_t idControl = guiControlGridControl(*app.Dialog->GUI, app.System[GUI_TYPE_Control_grid][app.PalettePicker.IndexGridTones[iColumn]], DIRECTION_RIGHT, {iShade, iRow});
		app.Dialog->GUI->Controls.SetSelected(idControl, idControl == idSelected);
	}
}

sttc u2_t sampleGUIPaletteToneSelected(cnst SSampleGUI & app, cnst u2_t toneCount) nxpt {
	for(u2_t iColumn = 0; iColumn < size(app.PalettePicker.IndexGridTones); ++iColumn)
	for(u2_t iRow = 0; iRow < ::sampleGUIPaletteToneRowCount(app, toneCount, iColumn); ++iRow)
	for(u2_t iShade = 0; iShade < ASCII_COLOR_COUNT; ++iShade)
		if(app.Dialog->GUI->Controls.IsSelected(guiControlGridControl(*app.Dialog->GUI, app.System[GUI_TYPE_Control_grid][app.PalettePicker.IndexGridTones[iColumn]], DIRECTION_RIGHT, {iShade, iRow})))
			rtrn ::sampleGUIPaletteToneFromCell(app, {iColumn, iRow}, toneCount);
	rtrn app.Dialog->GUI->ThemeDefault / ASCII_COLOR_COUNT % toneCount;
}

sttc void sampleGUIPaletteShadeSelect(SSampleGUI & app, cnst u2_t selected) nxpt {
	for(u2_t iShade = 0; iShade < ASCII_COLOR_COUNT; ++iShade)
		app.Dialog->GUI->Controls.SetSelected(guiControlGridControl(*app.Dialog->GUI, app.System[GUI_TYPE_Control_grid][app.PalettePicker.IndexGridShades], DIRECTION_RIGHT, iShade), iShade == selected);
}

sttc u2_t sampleGUIPaletteShadeSelected(cnst SSampleGUI & app) nxpt {
	for(u2_t iShade = 0; iShade < ASCII_COLOR_COUNT; ++iShade)
		if(app.Dialog->GUI->Controls.IsSelected(guiControlGridControl(*app.Dialog->GUI, app.System[GUI_TYPE_Control_grid][app.PalettePicker.IndexGridShades], DIRECTION_RIGHT, iShade)))
			rtrn iShade;
	rtrn app.Dialog->GUI->ThemeDefault % ASCII_COLOR_COUNT;
}

sttc err_t sampleGUIPalettePickerApply(SSampleGUI & app) {
	cnst u2_t toneCount = ::sampleGUIPaletteToneCount(app);
	if(0 == toneCount)
		rtrn 0;
	cnst u2_t tone = ::sampleGUIPaletteToneSelected(app, toneCount);
	cnst u2_t shade = ::sampleGUIPaletteShadeSelected(app);
	app.Dialog->GUI->ThemeDefault = (u1_t)(tone * ASCII_COLOR_COUNT + shade);
	for(u2_t iColumn = 0; iColumn < size(app.PalettePicker.IndexGridTones); ++iColumn)
	for(u2_t iRow = 0; iRow < ::sampleGUIPaletteToneRowCount(app, toneCount, iColumn); ++iRow)
	for(u2_t iShade = 0; iShade < ASCII_COLOR_COUNT; ++iShade) {
		cnst u2_t iTone = ::sampleGUIPaletteToneFromCell(app, {iColumn, iRow}, toneCount);
		cnst cid_t idControl = guiControlGridControl(*app.Dialog->GUI, app.System[GUI_TYPE_Control_grid][app.PalettePicker.IndexGridTones[iColumn]], DIRECTION_RIGHT, {iShade, iRow});
		app.Dialog->GUI->Controls.Draw[idControl].ColorTheme = (s1_t)(iTone * ASCII_COLOR_COUNT + iShade + 1);
	}
	for(u2_t iShade = 0; iShade < ASCII_COLOR_COUNT; ++iShade) {
		cnst cid_t idControl = guiControlGridControl(*app.Dialog->GUI, app.System[GUI_TYPE_Control_grid][app.PalettePicker.IndexGridShades], DIRECTION_RIGHT, iShade);
		app.Dialog->GUI->Controls.Draw[idControl].ColorTheme = (s1_t)(tone * ASCII_COLOR_COUNT + iShade + 1);
	}
	rtrn 0;
}

sttc void sampleGUIFontSelect(SSampleGUI & app, cnst u2_t selected) nxpt {
	for(u2_t iFont = 0; iFont < app.Dialog->GUI->Fonts.size(); ++iFont)
		app.Dialog->GUI->Controls.SetSelected(guiControlGridControl(*app.Dialog->GUI, app.System[GUI_TYPE_Control_grid][app.FontPicker.IndexGrid], DIRECTION_RIGHT, iFont), iFont == selected);
}

sttc u2_t sampleGUIFontSelected(cnst SSampleGUI & app) nxpt {
	for(u2_t iFont = 0; iFont < app.Dialog->GUI->Fonts.size(); ++iFont)
		if(app.Dialog->GUI->Controls.IsSelected(guiControlGridControl(*app.Dialog->GUI, app.System[GUI_TYPE_Control_grid][app.FontPicker.IndexGrid], DIRECTION_RIGHT, iFont)))
			rtrn iFont;
	rtrn app.Dialog->GUI->SelectedFont % app.Dialog->GUI->Fonts.size();
}

sttc void sampleGUIFontPickerApply(SSampleGUI & app) nxpt {
	if(0 == app.Dialog->GUI->Fonts.size())
		rtrn;
	app.Dialog->GUI->SelectedFont = (u1_t)::sampleGUIFontSelected(app);
}

sttc err_t sampleGUIStateFlagAppend(string & text, vcst_t flag) {
	if(text.size())
		if_fail_fe(text.push_back('+'));
	rtrn append_string(text, flag);
}

sttc err_t sampleGUIStateDescribe(cnst SControlState & state, b8_t disabled, string & text) {
	text.clear();
	cnst enum_definition<GUI_CONTROL_FLAG> & stateDefinition = get_enum<GUI_CONTROL_FLAG>();
	for(u2_t iState = 0; iState < stateDefinition.Values.size(); ++iState)
		if(bit_test_masked(state.Flag, state.Mask, stateDefinition.Values[iState]))
			if_fail_fe(::sampleGUIStateFlagAppend(text, stateDefinition.Names[iState]));
	if(disabled && false == state.IsDisabled())
		if_fail_fe(::sampleGUIStateFlagAppend(text, get_value_namev(GUI_CONTROL_FLAG_Disabled)));
	if(0 == text.size())
		if_fail_fe(append_string(text, "-"));
	rtrn text.size();
}

sttc err_t sampleGUIControlTextSetOwned(SSampleGUI & app, cid_t idControl, astsc_t<512> & storage, vcst_t text) {
	if(app.Dialog->GUI->Controls.Text[idControl].Text == text)
		rtrn 0;
	if_true_fe(text.size() >= storage.size());
	for(u2_t iCharacter = 0; iCharacter < text.size(); ++iCharacter)
		storage.Storage[iCharacter] = text[iCharacter];
	storage.Storage[text.size()] = 0;
	rtrn controlTextSet(*app.Dialog->GUI, idControl, {storage.Storage, text.size()});
}

sttc cid_t sampleGUIHoveredControl(cnst SSampleGUI & app) nxpt {
	for(u2_t iControl = 0; iControl < app.Dialog->GUI->Controls.States.size(); ++iControl)
		if(false == controlInvalid(*app.Dialog->GUI, (cid_t)iControl) && app.Dialog->GUI->Controls.IsHovered((cid_t)iControl))
			rtrn (cid_t)iControl;
	rtrn CID_INVALID;
}

sttc cid_t sampleGUIFocusedControl(cnst SSampleGUI & app) nxpt {
	for(u2_t iControl = 0; iControl < app.Dialog->GUI->Controls.States.size(); ++iControl)
		if(false == controlInvalid(*app.Dialog->GUI, (cid_t)iControl) && app.Dialog->GUI->Controls.IsFocused((cid_t)iControl))
			rtrn (cid_t)iControl;
	rtrn CID_INVALID;
}

sttc vcst_t sampleGUIPaletteName(cnst GUI_CONTROL_PALETTE palette) nxpt {
	cnst SCTTIEnumDeclaration * definition = cttiEnum("GUI_CONTROL_PALETTE");
	if(definition)
		for(u2_t iValue = 0; iValue < definition->Count; ++iValue)
			if(definition->Values[iValue].Value == (s3_t)palette) {
				vcst_c name = definition->Names[iValue];
				cnst u2_t prefixLength = definition->Name.size() + 1;
				rtrn name.size() > prefixLength ? vcst_t{name.begin() + prefixLength, name.size() - prefixLength} : name;
			}
	rtrn "-";
}

sttc err_t sampleGUIViewportCreate(SSampleGUI & app, cid_t & idViewport, pobj<SDialogViewport> & viewport, vcst_t title, cnst rect2s1_t & area);

sttc err_t sampleGUIInspectorCreate(SSampleGUI & app, SSampleGUIInspector & inspector, vcst_t title, cnst rect2s1_t & area, cid_t idParent, cnst pobj<SDialogViewport> & previous) {
	cid_t idViewport = CID_INVALID;
	if_fail_fe(::sampleGUIViewportCreate(app, idViewport, inspector.Viewport, title, area));
	if_fail_fe(controlSetParent(*app.Dialog->GUI, inspector.Viewport->IdGUIControl, idParent));
	SControlConstraints & constraints = app.Dialog->GUI->Controls.Constraints[inspector.Viewport->IdGUIControl];
	constraints.AttachSizeToControl.x = idParent;
	if(previous)
		constraints.DockToControl.Bottom = previous->IdGUIControl;
	cnst cid_t idClient = inspector.Viewport->IdClient;
	cnst s1_t clientWidth = (s1_t)(app.Dialog->GUI->Controls.Placement[idClient].Area.Size.x - controlNCSpacing(app.Dialog->GUI->Controls.Placement[idClient]).x);
	sttc vcst_c emptyValues[] = {"NONE", "-", "-", "-", "-"};
	acid idNames = {}, idValues = {};
	if_fail_fe(guiCreateControlList<sample_gui::INSPECTOR_FIELD>(*app.Dialog->GUI, idClient, {88, 18}, {0, 4}, ALIGN_TOP_LEFT, ALIGN_CENTER_RIGHT, idNames));
	if_fail_fe(guiCreateControlList(*app.Dialog->GUI, {emptyValues}, idClient, {(u1_t)(clientWidth - 88), 18}, {88, 4}, ALIGN_TOP_LEFT, ALIGN_CENTER_LEFT, idValues));
	rtrn 0;
}

sttc err_t sampleGUIInspectorValueSet(SSampleGUI & app, SSampleGUIInspector & inspector, sample_gui::INSPECTOR_FIELD field, vcst_t value) {
	stxp u0_t fieldCount = sample_gui::TEXT + 1;
	cnst acid & children = app.Dialog->GUI->Controls.Children[inspector.Viewport->IdClient];
	rtrn ::sampleGUIControlTextSetOwned(app, children[fieldCount + field], inspector.TextValues[field], value);
}

sttc err_t sampleGUIInspectorUpdate(SSampleGUI & app, SSampleGUIInspector & inspector, cid_t idControl) {
	SGUI			& gui			= *app.Dialog->GUI;
	SControlTable	& controls		= gui.Controls;
	if(controlInvalid(gui, idControl)) {
		if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, sample_gui::ID, "NONE"));
		for(u0_t iField = sample_gui::STATE; iField <= sample_gui::TEXT; ++iField)
			if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, (sample_gui::INSPECTOR_FIELD)iField, "-"));
		rtrn 0;
	}
	cnst SControlState & state = controls.States[idControl];
	b8_c			disabled	= 0 != controlDisabled(gui, idControl);
	string			stateText	= {};
	if_fail_fe(::sampleGUIStateDescribe(state, disabled, stateText));
	cnst GUI_CONTROL_PALETTE palette = paletteIndexFromState(disabled, state);
	vcst_c				paletteName = ::sampleGUIPaletteName(palette);
	s1_c				paletteSlot	= controls.Draw[idControl].Palettes[palette];
	string			paletteText	= {};
	if_fail_fe(append_strings(paletteText, paletteName, " [", str(paletteSlot), "]"));
	if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, sample_gui::ID, str(idControl)));
	if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, sample_gui::STATE, stateText));
	if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, sample_gui::PALETTE, paletteText));
	if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, sample_gui::ALIGN, get_value_namev(controls.Placement[idControl].Align)));
	rtrn ::sampleGUIInspectorValueSet(app, inspector, sample_gui::TEXT, get_value_namev(controls.Text[idControl].Align));
}

sttc err_t sampleGUIViewportCreate(SSampleGUI & app, cid_t & idViewport, pobj<SDialogViewport> & viewport, vcst_t title, cnst rect2s1_t & area) {
	if_fail_fe(idViewport = viewportCreate(*app.Dialog, viewport));
	SControlTable & controls = app.Dialog->GUI->Controls;
	controls.Draw[viewport->IdGUIControl      ].UseNewPalettes = false;
	controls.Draw[viewport->IdClose           ].UseNewPalettes = false;
	controls.Draw[viewport->IdResizeBottomRight].UseNewPalettes = false;
	SControlPlacement & root = controls.Placement[viewport->IdGUIControl];
	root.Area = area;
	root.Border = {3, 3, 3, 3};
	root.Margin = {1, 1, 1, 1};
	if_fail_fe(controlTextSet(*app.Dialog->GUI, viewport->IdTitle, title));
	controls.Placement[viewport->IdTitle].Border = {2, 2, 2, 2};
	controls.Placement[viewport->IdClient].Area.Size = {(s1_t)(area.Size.x - 8), (s1_t)(area.Size.y - 32)};
	controls.Placement[viewport->IdClient].Border = {1, 1, 1, 1};
	viewport->SettingsOld = viewport->Settings;
	rtrn controlMetricsInvalidate(*app.Dialog->GUI, viewport->IdGUIControl);
}

sttc b8_t sampleGUICTTIGUIType(cnst SCTTIStructDeclaration & declaration) {
	u2_t iName = declaration.File.size();
	while(iName && declaration.File[iName - 1] != '/' && declaration.File[iName - 1] != '\\')
		--iName;
	cnst vcst_t fileName = {declaration.File.begin() + iName, declaration.File.size() - iName};
	rtrn fileName.size() >= 7 && vcst_t{fileName.begin(), 7} == "gpk_gui";
}

sttc err_t sampleGUIJSONColumns(SJSONFile & document, aobj<vcst_t> & types, aobj<vcst_t> & members, aobj<vcst_t> & values) {
	document.Reader = {};
	if_fail_fe(jsonParse(document.Reader, document.Bytes));
	if_fail_fe(jsonObjectKeyList(document.Reader, 0, members));
	if_fail_fe(types.resize(members.size()));
	if_fail_fe(values.resize(members.size()));
	for(u2_t iMember = 0; iMember < members.size(); ++iMember) {
		err_t iMemberNode = jsonObjectValueGet(document.Reader, 0, members[iMember]);
		if_fail_fe(iMemberNode);
		if_fail_fe(jsonObjectGetAsString(document.Reader, (u2_t)iMemberNode, "type", types[iMember]));
		err_t iValueNode = jsonObjectValueGet(document.Reader, (u2_t)iMemberNode, "value");
		if_fail_fe(iValueNode);
		values[iMember] = document.Reader.View[iValueNode];
	}
	rtrn members.size();
}

sttc err_t sampleGUIDataViewportRefresh(SSampleGUI & app, SDialogViewport & viewport, SJSONFile & document) {
	aobj<vcst_t> types = {}, members = {}, values = {};
	if_fail_fe(::sampleGUIJSONColumns(document, types, members, values));
	SControlTable & controls = app.Dialog->GUI->Controls;
	if(0 == controls.Children[viewport.IdClient].size()) {
		acid fields = {};
		if_fail_fe(guiCreateControlList(*app.Dialog->GUI, view<vcst_c>{types  }, viewport.IdClient, {110, 18}, {  4, 4}, ALIGN_TOP_LEFT, ALIGN_CENTER_LEFT, fields));
		if_fail_fe(guiCreateControlList(*app.Dialog->GUI, view<vcst_c>{members}, viewport.IdClient, {110, 18}, {114, 4}, ALIGN_TOP_LEFT, ALIGN_CENTER_LEFT, fields));
		if_fail_fe(guiCreateControlList(*app.Dialog->GUI, view<vcst_c>{values }, viewport.IdClient, {130, 18}, {224, 4}, ALIGN_TOP_LEFT, ALIGN_CENTER_LEFT, fields));
		rtrn fields.size();
	}
	cnst acid & fields = controls.Children[viewport.IdClient];
	if_true_fef(fields.size() != members.size() * 3, "JSON viewport field count changed. Controls: %u. JSON columns: %u.", fields.size(), members.size() * 3);
	for(u2_t iMember = 0; iMember < members.size(); ++iMember) {
		if_fail_fe(controlTextSet(*app.Dialog->GUI, fields[iMember                     ], types  [iMember]));
		if_fail_fe(controlTextSet(*app.Dialog->GUI, fields[iMember + members.size()    ], members[iMember]));
		if_fail_fe(controlTextSet(*app.Dialog->GUI, fields[iMember + members.size() * 2], values [iMember]));
	}
	rtrn fields.size();
}

sttc err_t sampleGUIDataViewportCreate(SSampleGUI & app, SGUIEditor & editor, vcst_t title, SJSONFile & document) {
	aobj<vcst_t> members = {};
	if_fail_fe(jsonParse(document.Reader, document.Bytes));
	if_fail_fe(jsonObjectKeyList(document.Reader, 0, members));
	pobj<SDialogViewport> viewport = {};
	cid_t idViewport = CID_INVALID;
	if_fail_fe(::sampleGUIViewportCreate(app, idViewport, viewport, title, {{4, 4}, {362, (s1_t)(42 + members.size() * 18)}}));
	SControlTable & controls = app.Dialog->GUI->Controls;
	if_fail_fe(controlSetParent(*app.Dialog->GUI, viewport->IdGUIControl, editor.DataViewport->IdClient));
	SControlConstraints & constraints = controls.Constraints[viewport->IdGUIControl];
	constraints.AttachSizeToControl.x = editor.DataViewport->IdClient;
	if(editor.DataViews.size())
		constraints.DockToControl.Bottom = editor.DataViews[editor.DataViews.size() - 1]->IdGUIControl;
	if_fail_fe(::sampleGUIDataViewportRefresh(app, *viewport, document));
	if_fail_fe(editor.DataViews.push_back(viewport));
	rtrn 0;
}

sttc err_t sampleGUIDataViewportsCreate(SSampleGUI & app, SGUIEditor & editor) {
	cid_t idViewport = CID_INVALID;
	if_fail_fe(::sampleGUIViewportCreate(app, idViewport, editor.DataViewport, "STRUCTS", {{}, {380, 500}}));
	SControlTable & controls = app.Dialog->GUI->Controls;
	controls.Placement[editor.DataViewport->IdGUIControl].Align = ALIGN_TOP_RIGHT;
	controls.Constraints[editor.DataViewport->IdGUIControl].DockToControl.Right = editor.Viewport->IdGUIControl;
	editor.DataViewport->Settings.ScrollBarY = true;
	if_fail_fe(guiControlListPush(*app.Dialog->GUI, app.System[GUI_TYPE_Control_list][app.IndexListViewports], DIRECTION_RIGHT, app.Dialog->GUI->Controls.Text[editor.DataViewport->IdTitle].Text));
	cnst aobj<SCTTIStructDeclaration> & declarations = cttiStructDeclarations();
	for(u2_t iDeclaration = 0; iDeclaration < declarations.size(); ++iDeclaration) {
		cnst SCTTIStructDeclaration & metadata = declarations[iDeclaration];
		if(false == ::sampleGUICTTIGUIType(metadata))
			continue;
		pobj<SJSONFile> document = {};
		if_null_fe(document.create());
		if_fail_fe(cttiMemberJSON(metadata, document->Bytes));
		if_fail_fe(::sampleGUIDataViewportCreate(app, editor, metadata.Name, *document));
		if_fail_fe(editor.Data.push_back(document));
	}
	rtrn editor.DataViews.size();
}

err_t sampleGUISetup(SSampleGUI & app, pobj<SInput> & input) {
	app.Dialog->Input = input;
	app.Dialog->GUI->ColorModeDefault = GUI_COLOR_MODE_3D;
	pobj<SDialogViewport> viewportPalette = {};
	cid_t idViewport = CID_INVALID;
	cid_t idComposite = CID_INVALID;
	if_fail_fe(::sampleGUIViewportCreate(app, idViewport, viewportPalette, "PALETTE", {{20, 18}, {300, 220}}));
	cnst u2_t toneCount = ::sampleGUIPaletteToneCount(app);
	if_true_fe(0 == toneCount);
	cnst u2_t toneDefault = toneCount >= 3 ? toneCount - 3 : 0;
	cnst u2_t shadeDefault = ASCII_COLOR_COUNT / 2;
	aobj<vcst_t> toneTexts = {};
	aobj<vcst_t> shadeTexts = {};
	if_fail_fe(shadeTexts.resize(ASCII_COLOR_COUNT));
	if_fail_fe(idComposite = guiControlGridCreate(*app.Dialog->GUI, view<vcst_c>{shadeTexts}, viewportPalette->IdClient, DIRECTION_RIGHT, ASCII_COLOR_COUNT / 2, {16, 16}, {1, 1}, {78, 4}));
	if_fail_fe(app.PalettePicker.IndexGridShades = app.System[GUI_TYPE_Control_grid].push_back(idComposite));
	for(u2_t iColumn = 0; iColumn < size(app.PalettePicker.IndexGridTones); ++iColumn) {
		if_fail_fe(toneTexts.resize(::sampleGUIPaletteToneRowCount(app, toneCount, iColumn) * ASCII_COLOR_COUNT));
		if_fail_fe(idComposite = guiControlGridCreate(*app.Dialog->GUI, view<vcst_c>{toneTexts}, viewportPalette->IdClient, DIRECTION_RIGHT, ASCII_COLOR_COUNT, {8, 5}, {}, {(s1_t)(16 + iColumn * 132), 40}));
		if_fail_fe(app.PalettePicker.IndexGridTones[iColumn] = app.System[GUI_TYPE_Control_grid].push_back(idComposite));
		for(u2_t iColor = 0; iColor < toneTexts.size(); ++iColor) {
			cnst cid_t idControl = guiControlGridControl(*app.Dialog->GUI, idComposite, DIRECTION_RIGHT, iColor);
			app.Dialog->GUI->Controls.Placement[idControl].Border = {};
			app.Dialog->GUI->Controls.Placement[idControl].Margin = {};
			app.Dialog->GUI->Controls.Draw[idControl].NoBorder = true;
		}
	}
	::sampleGUIPaletteShadeSelect(app, shadeDefault);
	for(u2_t iColumn = 0; iColumn < size(app.PalettePicker.IndexGridTones); ++iColumn)
	for(u2_t iRow = 0; iRow < ::sampleGUIPaletteToneRowCount(app, toneCount, iColumn); ++iRow)
		if(toneDefault == ::sampleGUIPaletteToneFromCell(app, {iColumn, iRow}, toneCount))
			::sampleGUIPaletteToneSelect(app, guiControlGridControl(*app.Dialog->GUI, app.System[GUI_TYPE_Control_grid][app.PalettePicker.IndexGridTones[iColumn]], DIRECTION_RIGHT, {shadeDefault, iRow}));
	pobj<SDialogViewport> viewportFonts = {};
	if_fail_fe(::sampleGUIViewportCreate(app, idViewport, viewportFonts, "FONTS", {{332, 18}, {252, 264}}));
	if_fail_fe(app.FontPicker.Labels.resize(app.Dialog->GUI->Fonts.size()));
	for(u2_t iFont = 0; iFont < app.Dialog->GUI->Fonts.size(); ++iFont) {
		cnst n2u0_t charSize = app.Dialog->GUI->Fonts[iFont]->CharSize;
		if_fail_fe(append_strings(app.FontPicker.Labels[iFont], str(charSize.x), "x", str(charSize.y)));
	}
	if_fail_fe(idComposite = guiControlGridCreate(*app.Dialog->GUI, view<string>{app.FontPicker.Labels}, viewportFonts->IdClient, DIRECTION_RIGHT, 2, {120, 28}, {4, 6}));
	if_fail_fe(app.FontPicker.IndexGrid = app.System[GUI_TYPE_Control_grid].push_back(idComposite));
	for(u2_t iFont = 0; iFont < app.Dialog->GUI->Fonts.size(); ++iFont)
		if_fail_fe(controlFontSet(*app.Dialog->GUI, guiControlGridControl(*app.Dialog->GUI, idComposite, DIRECTION_RIGHT, iFont), (s1_t)iFont));
	::sampleGUIFontSelect(app, app.Dialog->GUI->SelectedFont);
	cnst enum_definition<GUI_CONTROL_FLAG> & stateDefinition = get_enum<GUI_CONTROL_FLAG>();
	pobj<SDialogViewport> viewportStates = {};
	if_fail_fe(::sampleGUIViewportCreate(app, idViewport, viewportStates, "CONTROL STATES", {{20, 242}, {176, (s1_t)(42 + stateDefinition.Values.size() * 18)}}));

	if_fail_fe(::sampleGUIControlCreate(app, app.ControlMode, "Mode: 3D", {{4, 136}, {134, 42}}));
	if_fail_fe(controlSetParent(*app.Dialog->GUI, app.ControlMode, viewportPalette->IdClient));
	if_fail_fe(::sampleGUIControlCreate(app, app.ControlGeometry, "Geometry: Indexed", {{142, 136}, {146, 42}}));
	if_fail_fe(controlSetParent(*app.Dialog->GUI, app.ControlGeometry, viewportPalette->IdClient));
	aobj<vcst_t> stateTexts = {};
	if_fail_fe(stateTexts.resize(stateDefinition.Values.size() * 2));
	for(u2_t iState = 0; iState < stateDefinition.Values.size(); ++iState)
		stateTexts[iState * 2] = stateTexts[iState * 2 + 1] = stateDefinition.Names[iState];
	if_fail_fe(idComposite = guiControlGridCreate(*app.Dialog->GUI, view<vcst_c>{stateTexts}, viewportStates->IdClient, DIRECTION_RIGHT, 2, {76, 18}, {}, {8, 6}));
	if_fail_fe(app.IndexGridStates = app.System[GUI_TYPE_Control_grid].push_back(idComposite));
	for(u2_t iState = 0; iState < stateDefinition.Values.size(); ++iState) {
		cnst cid_t idName = guiControlGridControl(*app.Dialog->GUI, idComposite, DIRECTION_RIGHT, {0, iState});
		cnst cid_t idState = guiControlGridControl(*app.Dialog->GUI, idComposite, DIRECTION_RIGHT, {1, iState});
		app.Dialog->GUI->Controls.Text[idName].Align = ALIGN_CENTER_RIGHT;
		app.Dialog->GUI->Controls.Text[idState].Align = ALIGN_CENTER_LEFT;
		app.Dialog->GUI->Controls.Draw[idName].ColorMode = app.Dialog->GUI->Controls.Draw[idState].ColorMode = GUI_COLOR_MODE_Default;
		if(stateDefinition.Values[iState] < GUI_CONTROL_FLAG_Updated)
			app.Dialog->GUI->Controls.States[idState].Flag = bit_set(app.Dialog->GUI->Controls.States[idState].Flag, stateDefinition.Values[iState]);
	}
	pobj<SDialogViewport> viewportInspectors = {};
	if_fail_fe(::sampleGUIViewportCreate(app, idViewport, viewportInspectors, "INSPECTORS", {{384, 18}, {300, 456}}));
	app.Dialog->GUI->Controls.Placement[viewportInspectors->IdGUIControl].Align = ALIGN_TOP_RIGHT;
	for(u0_t iInspector = 0; iInspector < sample_gui::INSPECTOR_gpk_ctti.Count; ++iInspector)
		if_fail_fe(::sampleGUIInspectorCreate
			( app
			, app.Inspectors[iInspector]
			, get_value_labels<sample_gui::INSPECTOR>()[iInspector]
			, {{4, 4}, {286, 134}}
			, viewportInspectors->IdClient
			, iInspector ? app.Inspectors[iInspector - 1].Viewport : pobj<SDialogViewport>{}
			));
	pobj<SDialogViewport> viewportAlignments = {};
	if_fail_fe(::sampleGUIViewportCreate(app, idViewport, viewportAlignments, "ALIGN", {{20, 18}, {372, 187}}));
	app.Dialog->GUI->Controls.Placement[viewportAlignments->IdGUIControl].Align = ALIGN_TOP_RIGHT;
	sttc cnst ALIGN horizontal[] = {ALIGN_LEFT, ALIGN_HCENTER, ALIGN_RIGHT};
	sttc cnst ALIGN vertical  [] = {ALIGN_TOP , ALIGN_VCENTER, ALIGN_BOTTOM};
	for(u0_t y = 0; y < 3; ++y)
	for(u0_t x = 0; x < 3; ++x) {
		cnst ALIGN controlAlign = (ALIGN)(vertical[y] | horizontal[x]);
		cnst ALIGN textAlign = (ALIGN)(vertical[2 - y] | horizontal[2 - x]);
		cid_t idControl = CID_INVALID;
		if_fail_fe(::sampleGUIControlCreate(app, idControl, get_value_namev(controlAlign), {{2, 2}, {114, 42}}, controlAlign));
		app.Dialog->GUI->Controls.Text[idControl].Align = textAlign;
		if_fail_fe(controlSetParent(*app.Dialog->GUI, idControl, viewportAlignments->IdClient));
	}
	aobj<vcst_t> viewportTitles = {};
	for(u2_t iViewport = 0; iViewport < app.Dialog->Controls.size(); ++iViewport) {
		pobj<SDialogViewport> viewport = {};
		if_fail_fe(app.Dialog->Get((cid_t)iViewport, viewport));
		if(app.Dialog->GUI->Controls.States[viewport->IdGUIControl].Parent == app.Dialog->Root)
			if_fail_fe(viewportTitles.push_back(app.Dialog->GUI->Controls.Text[viewport->IdTitle].Text));
	}
	if_fail_fe(idComposite = guiControlListCreate(*app.Dialog->GUI, DIRECTION_RIGHT, view<vcst_c>{viewportTitles}));
	if_fail_fe(app.IndexListViewports = app.System[GUI_TYPE_Control_list].push_back(idComposite));
	app.Dialog->GUI->Controls.Placement[idComposite].Area.Offset = {0, 0};
	app.Dialog->GUI->Controls.Placement[idComposite].Align = ALIGN_TOP_LEFT;
	if_fail_fe(idComposite = guiControlListCreate(*app.Dialog->GUI, DIRECTION_RIGHT, get_value_labels<sample_gui::ACTION>()));
	if_fail_fe(app.IndexListExit = app.System[GUI_TYPE_Control_list].push_back(idComposite));
	app.Dialog->GUI->Controls.Placement[idComposite].Area.Offset = {0, 0};
	app.Dialog->GUI->Controls.Placement[idComposite].Align = ALIGN_TOP_RIGHT;
	if_fail_fe(idComposite = guiControlListCreate(*app.Dialog->GUI, DIRECTION_RIGHT, get_value_labels<sample_gui::WORKSPACE_ACTION>()));
	if_fail_fe(app.Workspace.IndexActions = app.System[GUI_TYPE_Control_list].push_back(idComposite));
	app.Dialog->GUI->Controls.Placement[idComposite].Area.Offset = {0, 0};
	app.Dialog->GUI->Controls.Placement[idComposite].Align = ALIGN_BOTTOM_LEFT;
	if_fail_fe(idComposite = guiControlListCreate(*app.Dialog->GUI, DIRECTION_RIGHT, view<vcst_c>{}));
	if_fail_fe(app.Workspace.IndexTaskbar = app.System[GUI_TYPE_Control_list].push_back(idComposite));
	app.Dialog->GUI->Controls.Placement[idComposite].Area.Offset = {0, 0};
	app.Dialog->GUI->Controls.Placement[idComposite].Align = ALIGN_BOTTOM_LEFT;
	app.Dialog->GUI->Controls.Constraints[idComposite].DockToControl.Right = app.System[GUI_TYPE_Control_list][app.Workspace.IndexActions];
	if_fail_fe(idComposite = guiCreateKeyboard(*app.Dialog->GUI, app.System));
	if_fail_fe(::sampleGUIGeometryModeApply(app));
	if_fail_fe(::sampleGUIPalettePickerApply(app));
	::sampleGUIFontPickerApply(app);
	cid_t inspectedControls[sample_gui::INSPECTOR_gpk_ctti.Count] = {};
	inspectedControls[sample_gui::LIVE] = app.Inspectors[sample_gui::LIVE].Viewport->IdTitle;
	inspectedControls[sample_gui::HOVERED] = inspectedControls[sample_gui::FOCUSED] = CID_INVALID;
	for(u0_t iInspector = 0; iInspector < sample_gui::INSPECTOR_gpk_ctti.Count; ++iInspector)
		if_fail_fe(::sampleGUIInspectorUpdate(app, app.Inspectors[iInspector], inspectedControls[iInspector]));
	rtrn 0;
}

err_t sampleGUIUpdate(SSampleGUI & app, gpk::b8_t systemRequestedExit) {
	if(systemRequestedExit)
		rtrn APPLICATION_STATE_EXIT;
	b8_t exitRequested = false;
	if_fail_fe(guiProcessControls(*app.Dialog->GUI, [&](cid_t idControl) -> err_t {
		if(idControl == app.Dialog->GUI->Controls.Children[app.System[GUI_TYPE_Control_list][app.Workspace.IndexActions]][sample_gui::NEW_GUI]) {
			pobj<SGUIEditor> editor = {};
			if_null_fe(editor.create());
			pobj<SGUI> guiEdited = {};
			if_null_fe(guiEdited.create());
			if_fail_fe(guiSetupDefaultColors(guiEdited->Colors));
			if_fail_fe(guiSetupDefaultFonts(*guiEdited));
			if_null_fe(editor->Dialog.create(guiEdited));
			cid_t idViewport = CID_INVALID;
			if_fail_fe(::sampleGUIViewportCreate(app, idViewport, editor->Viewport, "GUI", {{}, {320, 240}}));
			app.Dialog->GUI->Controls.Placement[editor->Viewport->IdGUIControl].Align = ALIGN_CENTER;
			cid_t idComposite = CID_INVALID;
			if_fail_fe(idComposite = guiControlListCreate(*app.Dialog->GUI, DIRECTION_RIGHT, get_value_labels<sample_gui::EDITOR_ACTION>(), editor->Viewport->IdClient));
			if_fail_fe(editor->IndexActions = app.System[GUI_TYPE_Control_list].push_back(idComposite));
			app.Dialog->GUI->Controls.Placement[idComposite].Area.Offset = {4, 4};
			app.Dialog->GUI->Controls.Placement[idComposite].Align = ALIGN_TOP_LEFT;
			app.Dialog->GUI->Controls.Images[editor->Viewport->IdClient].ImageAlign = ALIGN_BOTTOM | ALIGN_HCENTER;
			if_fail_fe(guiControlListPush(*app.Dialog->GUI, app.System[GUI_TYPE_Control_list][app.IndexListViewports], DIRECTION_RIGHT, app.Dialog->GUI->Controls.Text[editor->Viewport->IdTitle].Text));
			if_fail_fe(::sampleGUIDataViewportsCreate(app, *editor));
			if_fail_fe(app.Workspace.Editors.push_back(editor));
			if_fail_fe(guiControlListPush(*app.Dialog->GUI, app.System[GUI_TYPE_Control_list][app.Workspace.IndexTaskbar], DIRECTION_RIGHT, app.Dialog->GUI->Controls.Text[editor->Viewport->IdTitle].Text));
			rtrn 0;
		}
		for(u2_t iEditor = 0; iEditor < app.Workspace.Editors.size(); ++iEditor) {
			SGUIEditor & editor = *app.Workspace.Editors[iEditor];
			if(idControl == app.Dialog->GUI->Controls.Children[app.System[GUI_TYPE_Control_list][editor.IndexActions]][sample_gui::NEW_CONTROL]) {
				cid_t idNewControl = CID_INVALID;
				if_fail_fe(idNewControl = controlCreate(*editor.Dialog->GUI));
				editor.Dialog->GUI->Controls.Placement[idNewControl].Align = ALIGN_CENTER;
				if_fail_fe(guiSelect(*editor.Dialog->GUI, idNewControl));
				rtrn controlMetricsInvalidate(*editor.Dialog->GUI, idNewControl);
			}
			if(idControl == app.Dialog->GUI->Controls.Children[app.System[GUI_TYPE_Control_list][app.Workspace.IndexTaskbar]][iEditor]) {
				if_fail_fe(viewportFold(*editor.Viewport, false));
				app.Dialog->GUI->Controls.SetHidden(editor.Viewport->IdGUIControl, false);
				if_fail_fe(controlToFront(*app.Dialog->GUI, editor.Viewport->IdGUIControl));
				rtrn 0;
			}
		}
		if(idControl == app.Dialog->GUI->Controls.Children[app.System[GUI_TYPE_Control_list][app.IndexListExit]][sample_gui::SCREENSHOT]) {
			app.ScreenshotRequested = true;
			rtrn 0;
		}
		if(idControl == app.Dialog->GUI->Controls.Children[app.System[GUI_TYPE_Control_list][app.IndexListExit]][sample_gui::EXIT]) {
			exitRequested = true;
			rtrn 0;
		}
		if(idControl == app.ControlMode) {
			app.Dialog->GUI->ColorModeDefault = app.Dialog->GUI->ColorModeDefault == GUI_COLOR_MODE_3D ? GUI_COLOR_MODE_Flat : GUI_COLOR_MODE_3D;
			rtrn controlTextSet(*app.Dialog->GUI, app.ControlMode, app.Dialog->GUI->ColorModeDefault == GUI_COLOR_MODE_3D ? vcst_t{"Mode: 3D"} : vcst_t{"Mode: Flat"});
		}
		if(idControl == app.ControlGeometry) {
			app.UseNewPalettes = !app.UseNewPalettes;
			rtrn ::sampleGUIGeometryModeApply(app);
		}
		for(u2_t iViewport = 0; iViewport < app.Dialog->GUI->Controls.Children[app.System[GUI_TYPE_Control_list][app.IndexListViewports]].size(); ++iViewport)
			if(idControl == app.Dialog->GUI->Controls.Children[app.System[GUI_TYPE_Control_list][app.IndexListViewports]][iViewport]) {
				u2_t iTopLevel = 0;
				for(u2_t iDialogControl = 0; iDialogControl < app.Dialog->Controls.size(); ++iDialogControl) {
					pobj<SDialogViewport> viewport = {};
					if_fail_fe(app.Dialog->Get((cid_t)iDialogControl, viewport));
					if(app.Dialog->GUI->Controls.States[viewport->IdGUIControl].Parent != app.Dialog->Root)
						continue;
					if(iTopLevel++ != iViewport)
						continue;
					if_fail_fe(viewportFold(*viewport, false));
					app.Dialog->GUI->Controls.SetHidden(viewport->IdGUIControl, false);
					if_fail_fe(controlToFront(*app.Dialog->GUI, viewport->IdGUIControl));
					break;
				}
				rtrn 0;
			}
		cnst u2_t toneCount = ::sampleGUIPaletteToneCount(app);
		for(u2_t iColumn = 0; iColumn < size(app.PalettePicker.IndexGridTones); ++iColumn)
		for(u2_t iRow = 0; iRow < ::sampleGUIPaletteToneRowCount(app, toneCount, iColumn); ++iRow)
		for(u2_t iShade = 0; iShade < ASCII_COLOR_COUNT; ++iShade)
			if(idControl == guiControlGridControl(*app.Dialog->GUI, app.System[GUI_TYPE_Control_grid][app.PalettePicker.IndexGridTones[iColumn]], DIRECTION_RIGHT, {iShade, iRow})) {
				::sampleGUIPaletteToneSelect(app, idControl);
				::sampleGUIPaletteShadeSelect(app, iShade);
				rtrn ::sampleGUIPalettePickerApply(app);
			}
		for(u2_t iShade = 0; iShade < ASCII_COLOR_COUNT; ++iShade)
			if(idControl == guiControlGridControl(*app.Dialog->GUI, app.System[GUI_TYPE_Control_grid][app.PalettePicker.IndexGridShades], DIRECTION_RIGHT, iShade)) {
				::sampleGUIPaletteShadeSelect(app, iShade);
				rtrn ::sampleGUIPalettePickerApply(app);
			}
		for(u2_t iFont = 0; iFont < app.Dialog->GUI->Fonts.size(); ++iFont)
			if(idControl == guiControlGridControl(*app.Dialog->GUI, app.System[GUI_TYPE_Control_grid][app.FontPicker.IndexGrid], DIRECTION_RIGHT, iFont)) {
				::sampleGUIFontSelect(app, iFont);
				::sampleGUIFontPickerApply(app);
				rtrn 0;
			}
		rtrn 0;
	}));
	if(exitRequested)
		rtrn APPLICATION_STATE_EXIT;
	cnst enum_definition<GUI_CONTROL_FLAG> & stateDefinition = get_enum<GUI_CONTROL_FLAG>();
	for(u2_t iState = 0; iState < stateDefinition.Values.size(); ++iState)
		if(stateDefinition.Values[iState] < GUI_CONTROL_FLAG_Updated) {
			SControlState & state = app.Dialog->GUI->Controls.States[guiControlGridControl(*app.Dialog->GUI, app.System[GUI_TYPE_Control_grid][app.IndexGridStates], DIRECTION_RIGHT, {1, iState})];
			state.Flag = bit_set(state.Flag, stateDefinition.Values[iState]);
		}
	cnst cid_t idHovered = ::sampleGUIHoveredControl(app);
	cnst cid_t idFocused = ::sampleGUIFocusedControl(app);
	cnst cid_t inspectedControls[sample_gui::INSPECTOR_gpk_ctti.Count] = {
		app.Inspectors[sample_gui::LIVE].Viewport->IdTitle,
		idHovered,
		idFocused,
	};
	for(u0_t iInspector = 0; iInspector < sample_gui::INSPECTOR_gpk_ctti.Count; ++iInspector)
		if_fail_fe(::sampleGUIInspectorUpdate(app, app.Inspectors[iInspector], inspectedControls[iInspector]));
	if_fail_fe(app.Dialog->Update());
	for(u2_t iEditor = 0; iEditor < app.Workspace.Editors.size(); ++iEditor)
		if_fail_fe(app.Workspace.Editors[iEditor]->Dialog->Update());
	rtrn 0;
}

err_t sampleGUIDraw(SSampleGUI & app, g8bgra target) {
	if_fail_fe(target.fill(SColorBGRA{16, 14, 12}));
	for(u2_t iEditor = 0; iEditor < app.Workspace.Editors.size(); ++iEditor) {
		SGUIEditor & editor = *app.Workspace.Editors[iEditor];
		if(app.Dialog->GUI->Controls.IsHidden(editor.Viewport->IdGUIControl))
			continue;
		SControlImage & editorImage = app.Dialog->GUI->Controls.Images[editor.Viewport->IdClient];
		cnst SControlPlacement & placementClient = app.Dialog->GUI->Controls.Placement[editor.Viewport->IdClient];
		cnst SControlPlacement & placementActions = app.Dialog->GUI->Controls.Placement[app.System[GUI_TYPE_Control_list][editor.IndexActions]];
		n2s1_t surfaceSize = placementClient.Area.Size - controlNCSpacing(placementClient).s1();
		surfaceSize.x -= placementActions.Area.Offset.x * 2;
		surfaceSize.y -= placementActions.Area.Offset.y * 2 + placementActions.Area.Size.y;
		if(surfaceSize.x > 0 && surfaceSize.y > 0) {
			if_fail_fe(editor.Target.resize(surfaceSize.u2()));
			if_fail_fe(guiDraw(*editor.Dialog->GUI, editor.Target.View));
			editorImage.Image = editor.Target.View;
		}
	}
	if_fail_fe(guiDraw(*app.Dialog->GUI, target));
	if(app.ScreenshotRequested) {
		app.ScreenshotRequested = false;
		au0_t pngBytes = {};
		if_fail_fe(pngFileWrite(target, pngBytes));
		if_fail_fe(fileFromMemory("test_gpk_gui.png", pngBytes));
	}
	rtrn 0;
}

#define GPK_SAMPLE_GUI_TITLE "LLC GUI palettes, states and alignment"

stct SApplication {
	SFramework	Framework;
	SSampleGUI			Sample;

	SApplication(SRuntimeValues & runtimeValues)
		: Framework(runtimeValues, {}), Sample(Framework.GUI) {}
};

err_t setup(SApplication & app) {
	SWindow & window = app.Framework.RootWindow;
	window.Size = {1280, 540};
	if_null_fe(window.BackBuffer.create());
	if_fail_fe(window.BackBuffer->resize(window.Size.u2()));
	if_fail_fe(mainWindowCreate(window, app.Framework.RuntimeValues.PlatformDetail, window.Input));
	rtrn ::sampleGUISetup(app.Sample, window.Input);
}

err_t cleanup(SApplication & app) {
	rtrn mainWindowDestroy(app.Framework.RootWindow);
}

err_t update(SApplication & app, gpk::b8_t systemRequestedExit) {
	if(systemRequestedExit)
		rtrn APPLICATION_STATE_EXIT;
	cnst err_t result = updateFramework(app.Framework);
	if(result)
		rtrn result;
	cnst SWindow & window = app.Framework.RootWindow;
	cnst err_t resultSample = ::sampleGUIUpdate(app.Sample, window.Input->KeyDown(SYSTEM_KEY_Escape));
	sleep(1);
	rtrn resultSample;
}

err_t draw(SApplication & app) {
	SWindow & window = app.Framework.RootWindow;
	if(window.NoDraw)
		rtrn 0;
	if_fail_fe(window.BackBuffer->resize(window.Size.u2()));
	rtrn ::sampleGUIDraw(app.Sample, window.BackBuffer->Color.View);
}

GPK_DEFINE_APPLICATION_ENTRY_POINT(SApplication, GPK_SAMPLE_GUI_TITLE)
