#include "test_gpk_gui.h"
#include "gpk_app_impl.h"
#include "gpk_framework.h"
#include "gpk_gui_control_theme.h"
#include "gpk_gui_theme.h"
#include "gpk_system_key.h"

usns gpk;

err_t sampleGUISetup(SSampleGUI & app, SGUI & gui) {
	gui.ColorModeDefault = GUI_COLOR_MODE_3D;
	if_fail_fe(controlColorsDefault(gui.Colors));
	if_fail_fe(controlFontsDefault(gui.Fonts));
	if_fail_fef(jsonFileRead(app.GUIJSON, "../gpk/test_gpk_gui/gui_design.json"), "%s", "Failed to load the GUI JSON document.");
	if_fail_fe(controlTableJSONLoad(gui.Controls, gui.Colors->DefaultColors, app.GUIJSON.Reader));
	rtrn guiUpdateMetrics(gui, {1280, 720}, true);
}

err_t sampleGUIUpdate(SSampleGUI &, SGUI &, b8_t systemRequestedExit) {
	rtrn systemRequestedExit ? APPLICATION_STATE_EXIT : 0;
}

err_t sampleGUIDraw(SSampleGUI &, SGUI & gui, g8bgra target) {
	if_fail_fe(target.fill(SColorBGRA{16, 14, 12}));
	rtrn guiDraw(gui, target);
}

#define GPK_SAMPLE_GUI_TITLE "LLC GUI JSON expressions"

stct SApplication {
	SFramework		Framework;
	SSampleGUI		Sample;

	SApplication(SRuntimeValues & runtimeValues) : Framework(runtimeValues, {}) {}
};

err_t setup(SApplication & app) {
	SWindow & window = app.Framework.RootWindow;
	window.Size = {1280, 720};
	if_null_fe(window.BackBuffer.create());
	if_fail_fe(window.BackBuffer->resize(window.Size.u2()));
	if_fail_fe(mainWindowCreate(window, app.Framework.RuntimeValues.PlatformDetail, window.Input));
	rtrn sampleGUISetup(app.Sample, *app.Framework.GUI);
}

err_t cleanup(SApplication & app) {
	rtrn mainWindowDestroy(app.Framework.RootWindow);
}

err_t update(SApplication & app, b8_t systemRequestedExit) {
	if(systemRequestedExit)
		rtrn APPLICATION_STATE_EXIT;
	cnst err_t result = updateFramework(app.Framework);
	if(result)
		rtrn result;
	rtrn sampleGUIUpdate(app.Sample, *app.Framework.GUI, app.Framework.RootWindow.Input->KeyDown(SYSTEM_KEY_Escape));
}

err_t draw(SApplication & app) {
	SWindow & window = app.Framework.RootWindow;
	if(window.NoDraw)
		rtrn 0;
	if_fail_fe(window.BackBuffer->resize(window.Size.u2()));
	rtrn sampleGUIDraw(app.Sample, *app.Framework.GUI, window.BackBuffer->Color.View);
}

GPK_DEFINE_APPLICATION_ENTRY_POINT(SApplication, GPK_SAMPLE_GUI_TITLE)
