#include "gpk_engine.h"
#include "gpk_window.h"
#include "gpk_gui.h"

#ifndef TEST_GPK_GUI_H_23627
#define TEST_GPK_GUI_H_23627

stct SSampleGUI {
	::gpk::SEngine		Engine			= {};
	::gpk::SWindow		& Window;
	::gpk::SGUI			& GUI;

	SSampleGUI(::gpk::SWindow & window, ::gpk::SGUI & gui) : Window(window), GUI(gui) {}
	::gpk::cid_t		ControlTitle			= ::gpk::CID_INVALID;
	::gpk::cid_t		ControlMode				= ::gpk::CID_INVALID;
	::gpk::astcid<3>	ControlsTheme			= {};
	::gpk::astcid<8>	ControlsState			= {};
	::gpk::cid_t		ControlLive				= ::gpk::CID_INVALID;
	::gpk::cid_t		ControlInspector		= ::gpk::CID_INVALID;
	::gpk::cid_t		ControlHelp				= ::gpk::CID_INVALID;
	::gpk::cid_t		ControlExit				= ::gpk::CID_INVALID;
	::gpk::cid_t		ControlAlignmentTitle	= ::gpk::CID_INVALID;
	::gpk::cid_t		ControlAlignmentRoot	= ::gpk::CID_INVALID;
	::gpk::astcid<9>	ControlsAlignment		= {};
	::gpk::astcid<6>	ControlsKeyboard		= {};
	::gpk::astsc_t<512>	TextLive				= {};
	::gpk::astsc_t<512>	TextInspector			= {};
	::gpk::u0_t			Theme					= 0;
};

::gpk::err_t		sampleGUISetup				(SSampleGUI & app);
::gpk::err_t		sampleGUIUpdate				(SSampleGUI & app, bool systemRequestedExit);
::gpk::err_t		sampleGUIDraw				(SSampleGUI & app, ::gpk::g8bgra target);

#endif // TEST_GPK_GUI_H_23627
