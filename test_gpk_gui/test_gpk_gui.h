#include "gpk_gui.h"
#include "gpk_gui_control_list.h"
#include "gpk_control_grid.h"
#include "gpk_dialog.h"

#ifndef TEST_GPK_GUI_H_23627
#define TEST_GPK_GUI_H_23627

usns gpk;

stct SSampleGUIInspector {
	cid_t			IdRoot			= CID_INVALID;
	cid_t			IdTitle			= CID_INVALID;
	SControlList	Names			= {};
	SControlList	Values			= {};
	astsc_t<512>	TextValues[5]	= {};
};

stct SFontPicker {
	cid_t			IdViewport		= CID_INVALID;
	SControlGrid	Grid			= {};
	aobj<string>	Labels	= {};
};


stct SSampleGUI {
	pobj<SGUI>		GUIHandle	= {};

	SSampleGUI(pobj<SGUI> & gui) : GUIHandle(gui) {}
	b8_t			UseNewPalettes			= false;
	cid_t			ControlMode				= CID_INVALID;
	cid_t			ControlGeometry			= CID_INVALID;
	pobj<SDialog>	Dialog			= {};
	acid			ViewportIds			= {};
	SControlList	ListViewports			= {};
	SControlList	ListExit			= {};
	SPalettePicker		PalettePicker			= {};
	SFontPicker			FontPicker				= {};
	cid_t		ViewportStates			= CID_INVALID;
	SControlGrid	GridStates				= {};
	cid_t		ControlLive				= CID_INVALID;
	cid_t		ViewportInspectors		= CID_INVALID;
	SSampleGUIInspector	InspectorLive			= {};
	SSampleGUIInspector	InspectorHovered		= {};
	SSampleGUIInspector	InspectorFocused		= {};
	cid_t		ControlScreenshot		= CID_INVALID;
	cid_t		ControlExit				= CID_INVALID;
	b8_t				ScreenshotRequested		= false;
	cid_t		ViewportAlignments		= CID_INVALID;
	astcid<9>	ControlsAlignment		= {};
	astcid<4>	ControlsKeyboard		= {};
};

err_t		sampleGUISetup				(SSampleGUI & app, pobj<SInput> & input);
err_t		sampleGUIUpdate				(SSampleGUI & app, gpk::b8_t systemRequestedExit);
err_t		sampleGUIDraw				(SSampleGUI & app, g8bgra target);

#endif // TEST_GPK_GUI_H_23627
