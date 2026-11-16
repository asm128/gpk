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

nmsp demo {
	CTTI_ENUM(INSPECTOR,
		enum INSPECTOR : u0_t
		{ INSPECTOR_Live
		, INSPECTOR_Hovered
		, INSPECTOR_Focused
		};
		);
	CTTI_ENUM(WORKSPACE_ACTION,
	enum WORKSPACE_ACTION : u0_t
		{ WORKSPACE_ACTION_New_GUI
		};
		);
	CTTI_ENUM(EDITOR_ACTION,
	enum EDITOR_ACTION : u0_t
		{ EDITOR_ACTION_New_control
		};
		);
}
	CTTI_STCT(SGUIEditor,
	stct SGUIEditor {
		cid_t							PalettePicker			= {};
		cid_t							FontPicker				= {};
		aobj<string>					FontLabels				= {};
		cid_t							Inspectors[demo::INSPECTOR_gpk_ctti.Count]	= {};
		SGUI							GUI						= {};
		apobj<string>					Text					= {};
		apobj<img8bgra>					Images					= {};
		img8bgra						Target					= {};
		err_t							Save					(string & jsonOutput)	{ rtrn 0; }
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
};

err_t		sampleGUISetup				(SSampleGUI & app, pobj<SInput> & input);
err_t		sampleGUIUpdate				(SSampleGUI & app, apobj<SEventSystem> & systemEvents, gpk::b8_t systemRequestedExit);
err_t		sampleGUIDraw				(SSampleGUI & app, g8bgra target);

#endif // TEST_GPK_GUI_H_23627
