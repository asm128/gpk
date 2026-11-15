#include "gpk_gui.h"
#include "gpk_gui_control_list.h"
#include "gpk_control_grid.h"
#include "gpk_dialog.h"
#include "gpk_dialog_controls.h"
#include "gpk_json.h"

#ifndef TEST_GPK_GUI_H_23627
#define TEST_GPK_GUI_H_23627

usns gpk;

nmsp sample_gui {
	CTTI_ENUM_COMPILED(INSPECTOR, u0_t, LIVE, HOVERED, FOCUSED);
	CTTI_ENUM_COMPILED(WORKSPACE_ACTION, u0_t, NEW_GUI);
	CTTI_ENUM_COMPILED(EDITOR_ACTION, u0_t, NEW_CONTROL);
}

stct SSampleGUIInspector {
	pobj<SDialogViewport>
					Viewport		= {};
 	astsc_t<512>	TextValues[5]	= {};
};

stct SFontPicker {
	SControlGrid	Grid			= {};
	aobj<string>	Labels	= {};
};

CTTI_STCT(SGUIEditor,
	struct SGUIEditor {
	pobj<SDialogViewport>
				Viewport		= {};
	pobj<SDialog>	Dialog			= {};
	SControlList	Actions			= {};
	apobj<SDialogViewport>
				DataViews		= {};
	SJSONFile		Data			= {};
	img<bgra>		Target			= {};
	};
	);

CTTI_STCT(SGUIWorkspace,
	struct SGUIWorkspace {
	SControlList	Actions			= {};
	SControlList	Taskbar			= {};
	apobj<SGUIEditor>
				Editors			= {};
	};
	);


stct SSampleGUI {
	b8_t			UseNewPalettes			= false;
	cid_t			ControlMode				= CID_INVALID;
	cid_t			ControlGeometry			= CID_INVALID;
	pobj<SDialog>	Dialog			= {};
	SControlList	ListViewports			= {};
	SControlList	ListExit			= {};
	SPalettePicker		PalettePicker			= {};
	SFontPicker			FontPicker				= {};
	SControlGrid		GridStates				= {};
	SGUIWorkspace		Workspace				= {};
	SSampleGUIInspector	Inspectors[sample_gui::INSPECTOR_gpk_ctti.Count] = {};
	b8_t				ScreenshotRequested		= false;

	SSampleGUI(pobj<SGUI> & gui) { Dialog.create(gui); }
};

err_t		sampleGUISetup				(SSampleGUI & app, pobj<SInput> & input);
err_t		sampleGUIUpdate				(SSampleGUI & app, gpk::b8_t systemRequestedExit);
err_t		sampleGUIDraw				(SSampleGUI & app, g8bgra target);

#endif // TEST_GPK_GUI_H_23627
