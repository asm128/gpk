#include "gpk_window.h"
#include "gpk_gui.h"
#include "gpk_gui_control_list.h"
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

stct SSampleGUI {
	::gpk::SWindow		& Window;
	::gpk::pobj<::gpk::SGUI>	GUIHandle	= {};
	::gpk::SGUI			& GUI;

	SSampleGUI(::gpk::SWindow & window, ::gpk::pobj<::gpk::SGUI> & gui) : Window(window), GUIHandle(gui), GUI(*gui) {}
	::gpk::cid_t		ControlMode				= ::gpk::CID_INVALID;
	::gpk::pobj<::gpk::SDialog>	Dialog			= {};
	::gpk::cid_t		ViewportPalette			= ::gpk::CID_INVALID;
	::gpk::SControlGrid	GridPaletteTones			= {};
	::gpk::SControlGrid	GridPaletteShades		= {};
	::gpk::cid_t		ViewportStates			= ::gpk::CID_INVALID;
	::gpk::SControlGrid	GridStates				= {};
	::gpk::cid_t		ControlLive				= ::gpk::CID_INVALID;
	SSampleGUIInspector	InspectorLive			= {};
	SSampleGUIInspector	InspectorHovered		= {};
	SSampleGUIInspector	InspectorFocused		= {};
	::gpk::cid_t		ControlHelp				= ::gpk::CID_INVALID;
	::gpk::cid_t		ControlExit				= ::gpk::CID_INVALID;
	::gpk::cid_t		ControlAlignmentTitle	= ::gpk::CID_INVALID;
	::gpk::cid_t		ControlAlignmentRoot	= ::gpk::CID_INVALID;
	::gpk::astcid<9>	ControlsAlignment		= {};
	::gpk::astcid<3>	ControlsKeyboard		= {};
	::gpk::u2_t			PaletteTone				= 0;
	::gpk::u2_t			PaletteShade			= 0;
};

::gpk::err_t		sampleGUISetup				(SSampleGUI & app);
::gpk::err_t		sampleGUIUpdate				(SSampleGUI & app, bool systemRequestedExit);
::gpk::err_t		sampleGUIDraw				(SSampleGUI & app, ::gpk::g8bgra target);

#endif // TEST_GPK_GUI_H_23627
