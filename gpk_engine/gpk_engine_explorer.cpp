#include "gpk_engine_explorer.h"
#include "gpk_gui_text.h"
#include "gpk_gui_control_list.h"

static	gpk::cid_t	menuInit		(gpk::SGUI & gui, gpk::acid & idButtons) {
	gpk::cid_t				idMenu			= controlCreate(gui);
	gpk::SControlPlacement	& controlMenu	= gui.Controls.Placement[idMenu];
	controlMenu.Area		= {0, 0, 0, 20};
	controlMenu.Area.Size.y	= 20;
	controlMenu.Margin		= {};
	controlMenu.Border		= {};

	gpk::SControlConstraints	& controlMenuState	= gui.Controls.Constraints[idMenu];
	controlMenuState.AttachSizeToControl.x	= (gpk::cid_t)0;
	controlMenuState.AttachSizeToControl.y	= -1;

	gpk::SControlPlacement	controlItem		= {};
	controlItem.Margin		= {4, 2, 4, 2};
	controlItem.Border		= {1, 1, 1, 1};
	if_fail_fe(gpk::guiCreateControlList<gpk::EDITOR_APP_MENU>(gui, gpk::DIRECTION_RIGHT, idMenu, controlItem, gpk::ALIGN_CENTER_LEFT, idButtons));
	return idMenu;
}

gpk::err_t 		gpk::editorCreate	(SEditor & editor) {
	if(!editor.UI.GUI)
		editor.UI.GUI.create();

	SGUI					& gui				= *editor.UI.GUI;
	if_fail_fe(gpk::inputBoxCreate(editor.UI.InputBox, gui));

	gui.Controls.Placement[editor.UI.InputBox.IdRoot].Align			= ALIGN_CENTER_BOTTOM;

	if_fail_fe(editor.Dialogs.resize(get_value_count<EDITOR_APP_DIALOG>()));
	editor.Dialogs[EDITOR_APP_DIALOG_Menu] = ::menuInit(gui, editor.Menu);
	return 0;
}

gpk::err_t 		gpk::editorUpdate	(SEditor & editor, SInput & input, vpobj<SEventSystem> queuedEvents) {
	acid					controlsToProcess	= {};

	SGUI					& gui				= *editor.UI.GUI;
	guiProcessInput(gui, input, queuedEvents);

	guiGetProcessableControls(gui, controlsToProcess);
	if(s2_t result = editor.UI.InputBox.Update(gui, editor.UI.InputBox.VirtualKeyboard, queuedEvents, controlsToProcess)) {
		if(result == INT_MAX) { // enter key pressed
			vcst_t					trimmed				= {};
			if_fail_fe(editor.UI.InputBox.Text.slice(trimmed, 0, min(editor.UI.InputBox.Text.size(), 128U)));
			if_fail_fe(gpk::trim(trimmed));
			if(trimmed.size()) {
				pobj<string>	textInput;
				textInput.create();
				*textInput	= label(trimmed);
				editor.UI.InputHistory.push_back(textInput);
			}
			editor.UI.InputBox.Edit(gui, false);
		}
	}
	else {
		guiProcessControls(gui, controlsToProcess, [&](cid_t iControl) {
			//u2_t					offsetControl				= editor.Dialogs[EDITOR_APP_DIALOG_Menu];
			editor.UI.InputBox.SetText(gui, gui.Controls.Text[iControl].Text);
			editor.UI.InputBox.Edit(gui, true);
			return 0;
		});
	}
	return 0;
}
