#include "SIDE.h"
#include "gpk_app_impl.h"

stct SApplication {
	SFramework		Framework;
	SFileViewer		Viewer;

	SApplication(SRuntimeValues & runtimeValues) : Framework(runtimeValues, {}) {}
};

err_t setup(SApplication & app) {
	app.Framework.RootWindow.Size = {1280, 720};
	if_null_fe(app.Framework.RootWindow.BackBuffer.create());
	if_fail_fe(app.Framework.RootWindow.BackBuffer->resize(app.Framework.RootWindow.Size.u2()));
	if_fail_fe(mainWindowCreate(app.Framework.RootWindow, app.Framework.RuntimeValues.PlatformDetail, app.Framework.RootWindow.Input));
	if_null_fe(app.Viewer.Dialog.create(app.Framework.GUI));

	if_null_w((app.Viewer.Dialog->Input = app.Framework.RootWindow.Input).get_ref());
	if_fail_fe(guiUpdateMetrics(*app.Viewer.Dialog->GUI, app.Framework.RootWindow.Size, false));
	rtrn ::fileViewerSetup(app.Viewer, FILE_VIEWER_LAYOUT_FLOATING);
}

err_t cleanup(SApplication & app) {
	rtrn mainWindowDestroy(app.Framework.RootWindow);
}

err_t update(SApplication & app, gpk::b8_t systemRequestedExit) {
	if(systemRequestedExit)
		rtrn APPLICATION_STATE_EXIT;
	err_t result;
	if_fail_fef(result = updateFramework(app.Framework), "result=%" GPK_FMT_S2, result);
	if(app.Framework.RootWindow.Closed)
		rtrn APPLICATION_STATE_EXIT;
	rtrn ::fileViewerUpdate(app.Viewer, FILE_VIEWER_LAYOUT_FLOATING, app.Framework.RootWindow.EventQueue);
}

err_t draw(SApplication & app) {
	if(app.Framework.RootWindow.NoDraw)
		rtrn 0;
	if_fail_fe(app.Framework.RootWindow.BackBuffer->resize(app.Framework.RootWindow.Size.u2()));
	rtrn ::fileViewerDraw(app.Viewer, app.Framework.RootWindow.BackBuffer->Color.View);
}

GPK_DEFINE_APPLICATION_ENTRY_POINT(SApplication, "GPK pure SGUI file viewer")
