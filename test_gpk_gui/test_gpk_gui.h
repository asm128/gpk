#include "gpk_gui.h"
#include "gpk_ctti_stct.h"

#ifndef TEST_GPK_GUI_H_23627
#define TEST_GPK_GUI_H_23627

usns gpk;

	CTTI_STCT(SGUIEditor,
	stct SGUIEditor {
		cid_t							IdRoot					= CID_INVALID;
		SGUI							GUI						= {};
		apobj<string>					Text					= {};
		apobj<img8bgra>					Images					= {};
		img8bgra						Target					= {};
		err_t							Save					(string & jsonOutput)	{ (void)jsonOutput; rtrn 0; }
		err_t							Load					(vcst_t & jsonInput)	{
			u2_t readBytes = 0;
			if_fail_fe(jsonInput.slice(jsonInput, readBytes)); rtrn 0;
		}
	};
	);

CTTI_STCT(SGUIWorkspace,
	stct SGUIWorkspace {
	apobj<SGUIEditor>	Editors			= {};
	};
	);

stct SSampleGUI {
	SGUIWorkspace					Workspace				= {};
	apobj<string>					Text					= {};
};

err_t		sampleGUISetup				(SSampleGUI & app, SGUI & gui);
err_t		sampleGUIUpdate				(SSampleGUI & app, SGUI & gui, gpk::b8_t systemRequestedExit);
err_t		sampleGUIDraw				(SSampleGUI & app, SGUI & gui, g8bgra target);

#endif // TEST_GPK_GUI_H_23627
