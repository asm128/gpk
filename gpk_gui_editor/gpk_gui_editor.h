#include "gpk_gui.h"
#include "gpk_gui_control_list.h"
#include "gpk_control_grid.h"
#include "gpk_dialog.h"

#ifndef TEST_GPK_GUI_H_23627
#define TEST_GPK_GUI_H_23627

stct SSampleGUIInspector {
	::gpk::cid_t		IdRoot			= ::gpk::CID_INVALID;
	::gpk::cid_t		IdTitle			= ::gpk::CID_INVALID;
	::gpk::SControlList	Names			= {};
	::gpk::SControlList	Values			= {};
	::gpk::astsc_t<512>	TextValues[5]	= {};
};

stct SFontPicker {
	::gpk::cid_t		IdViewport		= ::gpk::CID_INVALID;
	::gpk::SControlGrid	Grid			= {};
	::gpk::aobj<::gpk::string>	Labels	= {};
};

stct SPalettePicker {
	::gpk::cid_t		IdViewport		= ::gpk::CID_INVALID;
	::gpk::SControlGrid	GridTones[2]	= {};
	::gpk::SControlGrid	GridShades		= {};
};

stct SSampleGUIEdited {
	::gpk::pobj<::gpk::SDialog>	Dialog			= {};
	::gpk::cid_t			Control			= ::gpk::CID_INVALID;
};

stct SSampleGUI {
	::gpk::pobj<::gpk::SDialog>	Dialog			= {};
	SSampleGUIEdited			Edited			= {};
	bool				UseNewPalettes			= false;
	::gpk::cid_t		ControlMode				= ::gpk::CID_INVALID;
	::gpk::cid_t		ControlGeometry			= ::gpk::CID_INVALID;
	::gpk::acid			ViewportIds			= {};
	::gpk::SControlList	ListViewports			= {};
	::gpk::SControlList	ListExit			= {};
	SPalettePicker		PalettePicker			= {};
	SFontPicker			FontPicker				= {};
	::gpk::cid_t		ViewportStates			= ::gpk::CID_INVALID;
	::gpk::SControlGrid	GridStates				= {};
	::gpk::cid_t		ControlLive				= ::gpk::CID_INVALID;
	::gpk::cid_t		ViewportInspectors		= ::gpk::CID_INVALID;
	SSampleGUIInspector	InspectorLive			= {};
	SSampleGUIInspector	InspectorHovered		= {};
	SSampleGUIInspector	InspectorFocused		= {};
	::gpk::cid_t		ControlScreenshot		= ::gpk::CID_INVALID;
	::gpk::cid_t		ControlExit				= ::gpk::CID_INVALID;
	bool				ScreenshotRequested		= false;
	::gpk::cid_t		ViewportAlignments		= ::gpk::CID_INVALID;
	::gpk::astcid<9>	ControlsAlignment		= {};
	::gpk::astcid<4>	ControlsKeyboard		= {};
};

::gpk::err_t		sampleGUISetup				(SSampleGUI & app, ::gpk::pobj<::gpk::SGUI> & gui, ::gpk::pobj<::gpk::SInput> & input);
::gpk::err_t		sampleGUIUpdate				(SSampleGUI & app, bool systemRequestedExit);
::gpk::err_t		sampleGUIDraw				(SSampleGUI & app, ::gpk::g8bgra target);

#endif // TEST_GPK_GUI_H_23627

