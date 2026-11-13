#include "../test_gpk_file_viewer_common/test_gpk_file_viewer_common.h"
#include "gpk_app_impl.h"

stct SApplication {
	::gpk::SFramework	Framework;
	SFileViewer			Viewer;

	SApplication(::gpk::SRuntimeValues & runtimeValues) : Framework(runtimeValues, {}), Viewer(Framework.RootWindow, Framework.GUI) {}
};

::gpk::err_t setup(SApplication & app) {
	app.Framework.RootWindow.Size = {1280, 720};
	if_null_ve(::gpk::OS_NO_MEMORY, app.Framework.RootWindow.BackBuffer.create());
	if_fail_fe(app.Framework.RootWindow.BackBuffer->resize(app.Framework.RootWindow.Size.u2()));
	if_fail_fe(::gpk::mainWindowCreate(app.Framework.RootWindow, app.Framework.RuntimeValues.PlatformDetail, app.Framework.RootWindow.Input));
	rtrn ::fileViewerSetup(app.Viewer, FILE_VIEWER_LAYOUT_SPLIT);
}

::gpk::err_t cleanup(SApplication & app) {
	rtrn ::gpk::mainWindowDestroy(app.Framework.RootWindow);
}

::gpk::err_t update(SApplication & app, bool systemRequestedExit) {
	if(systemRequestedExit)
		rtrn ::gpk::APPLICATION_STATE_EXIT;
	cnst ::gpk::err_t result = ::gpk::updateFramework(app.Framework);
	if(result)
		rtrn result;
	if(app.Framework.RootWindow.Closed || app.Framework.RootWindow.Input->KeyDown(::gpk::SYSTEM_KEY_Escape))
		rtrn ::gpk::APPLICATION_STATE_EXIT;
	rtrn ::fileViewerUpdate(app.Viewer, FILE_VIEWER_LAYOUT_SPLIT);
}

::gpk::err_t draw(SApplication & app) {
	if(app.Framework.RootWindow.NoDraw)
		rtrn 0;
	if_fail_fe(app.Framework.RootWindow.BackBuffer->resize(app.Framework.RootWindow.Size.u2()));
	rtrn ::fileViewerDraw(app.Viewer, app.Framework.RootWindow.BackBuffer->Color.View);
}

GPK_DEFINE_APPLICATION_ENTRY_POINT(SApplication, "GPK split file viewer")
