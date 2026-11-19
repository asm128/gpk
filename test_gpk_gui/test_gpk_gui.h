#include "gpk_gui.h"
#include "gpk_gui_control_json.h"

#ifndef TEST_GPK_GUI_H_23627
#define TEST_GPK_GUI_H_23627

usns gpk;

stct SSampleGUI {
	SJSONFile		GUIJSON		= {};
};

err_t		sampleGUISetup		(SSampleGUI & app, SGUI & gui);
err_t		sampleGUIUpdate		(SSampleGUI & app, SGUI & gui, b8_t systemRequestedExit);
err_t		sampleGUIDraw		(SSampleGUI & app, SGUI & gui, g8bgra target);

#endif // TEST_GPK_GUI_H_23627
