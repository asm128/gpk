#include "gpk_engine_explorer.h"
#include "gpk_gui_text.h"
#include "gpk_gui_control.h"

static	::gpk::error_t	menuInit		(::gpk::SGUI & gui, ::gpk::ai32 & idButtons) {
	int32_t						idMenu			= ::gpk::controlCreate(gui);
	::gpk::SControl				& controlMenu	= gui.Controls.Controls[idMenu];
	controlMenu.Area		= {0, 0, 0, 20};
	controlMenu.Area.Size.y	= 20;
	controlMenu.Margin		= {};
	controlMenu.Border		= {};

	::gpk::SControlConstraints	& controlMenuState	= gui.Controls.Constraints[idMenu];
	controlMenuState.AttachSizeToControl.x	= (uint32_t)0;
	controlMenuState.AttachSizeToControl.y	= -1;

	::gpk::SControl				controlItem		= {};
	controlItem.Margin		= {4, 2, 4, 2};
	controlItem.Border		= {1, 1, 1, 1};
	gpk_necs(::gpk::guiCreateControlList<::gpk::EDITOR_APP_MENU>(gui, ::gpk::DIRECTION_RIGHT, idMenu, controlItem, ::gpk::ALIGN_CENTER_LEFT, idButtons));
	return idMenu;
}

::gpk::error_t			gpk::editorCreate	(::gpk::SEditor & editor) {
	::gpk::SGUI					& gui				= editor.GUI.GUI;
	gpk_necs(::gpk::inputBoxCreate(editor.InputBox, gui));

	//gui.Controls.States	[editor.InputBox.IdRoot].Hidden			= true;
	gui.Controls.Controls	[editor.InputBox.IdRoot].Align			= ::gpk::ALIGN_CENTER;
	//gui.Controls.Controls	[editor.InputBox.IdRoot].Area.Offset.x = 240;

	//const ::gpk::eid_t			iEntityTopLeft		= editor.GUIEngine.CreateSquare(false);
	//const ::gpk::eid_t			iEntityTopRight		= editor.GUIEngine.CreateSquare(true);

	gpk_necs(editor.Dialogs.resize(::gpk::get_value_count<::gpk::EDITOR_APP_DIALOG>()));
	editor.Dialogs[::gpk::EDITOR_APP_DIALOG_Menu] = ::menuInit(gui, editor.Buttons);
	return 0;
}

::gpk::error_t			gpk::editorUpdate	(::gpk::SEditor & editor, ::gpk::SInput & input, ::gpk::view<::gpk::SSysEvent> queuedEvents) {
	::gpk::au32					controlsToProcess	= {};
	::gpk::SGUI					& gui				= editor.GUI.GUI;
	::gpk::guiProcessInput(gui, input, queuedEvents);

	::gpk::guiGetProcessableControls(gui, controlsToProcess);
	if(int32_t result = editor.InputBox.Update(gui, editor.InputBox.VirtualKeyboard, queuedEvents, controlsToProcess)) {
		if(result == INT_MAX) { // enter key pressed
			::gpk::vcc					trimmed				= {};
			gpk_necs(editor.InputBox.Text.slice(trimmed, 0, ::gpk::min(editor.InputBox.Text.size(), 128U)));
			gpk_necs(::gpk::trim(trimmed));
			if(trimmed.size()) {
				::gpk::pobj<::gpk::ac>	textInput;
				textInput.create();
				*textInput	= ::gpk::label(trimmed);
				editor.InputHistory.push_back(textInput);
			}
			editor.InputBox.Edit(gui, false);
		}
	}
	else {
		::gpk::guiProcessControls(gui, controlsToProcess, [&](uint32_t iControl) {
			//uint32_t					offsetControl				= editor.Dialogs[::gpk::EDITOR_APP_DIALOG_Menu];
			editor.InputBox.SetText(gui, gui.Controls.Text[iControl].Text);
			editor.InputBox.Edit(gui, gui.Controls.Events[iControl].Execute);
			return 0;
		});
	}
	return 0;
}
