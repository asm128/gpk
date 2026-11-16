#include "gpk_gui.h"
#include "gpk_gui_control_list.h"
#include "gpk_gui_system.h"
#include "gpk_control_grid.h"
#include "gpk_dialog.h"
#include "gpk_dialog_controls.h"
#include "gpk_ctti_enum.h"
#include "gpk_ctti_stct.h"

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
	u2_t			IndexGrid		= (u2_t)-1;
	aobj<string>	Labels	= {};
};

stct SSampleGUIPalettePicker {
	cid_t			IdViewport		= CID_INVALID;
	u2_t			IndexGridTones[2]	= {(u2_t)-1, (u2_t)-1};
	u2_t			IndexGridShades	= (u2_t)-1;
};

CTTI_STCT(SGUIEditor,
	struct SGUIEditor {
	pobj<SDialogViewport>
				Viewport		= {};
	pobj<SDialog>	Dialog			= {};
	u2_t			IndexActions	= (u2_t)-1;
	apobj<SDialogViewport>
				DataViews		= {};
	aobj<string>	DataValues		= {};
	img<bgra>		Target			= {};
	};
	);

CTTI_STCT(SGUIWorkspace,
	struct SGUIWorkspace {
	u2_t			IndexActions	= (u2_t)-1;
	u2_t			IndexTaskbar	= (u2_t)-1;
	apobj<SGUIEditor>
				Editors			= {};
	};
	);


stct SSampleGUI {
	b8_t			UseNewPalettes			= false;
	cid_t			ControlMode				= CID_INVALID;
	cid_t			ControlGeometry			= CID_INVALID;
	pobj<SDialog>	Dialog			= {};
	astatic<acid, GUI_TYPE_COUNT>
					System				= {};
	u2_t			IndexListViewports		= (u2_t)-1;
	u2_t			IndexListExit			= (u2_t)-1;
	SSampleGUIPalettePicker
					PalettePicker			= {};
	SFontPicker			FontPicker				= {};
	u2_t			IndexGridStates			= (u2_t)-1;
	SGUIWorkspace		Workspace				= {};
	SSampleGUIInspector	Inspectors[sample_gui::INSPECTOR_gpk_ctti.Count] = {};
	b8_t				ScreenshotRequested		= false;

	SSampleGUI(pobj<SGUI> & gui) { Dialog.create(gui); }
};

err_t		sampleGUISetup				(SSampleGUI & app, pobj<SInput> & input);
err_t		sampleGUIUpdate				(SSampleGUI & app, apobj<SEventSystem> & systemEvents, gpk::b8_t systemRequestedExit);
err_t		sampleGUIDraw				(SSampleGUI & app, g8bgra target);

#endif // TEST_GPK_GUI_H_23627
