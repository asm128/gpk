#include "gpk_file_explorer.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

using namespace gpk;

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gfe::SApplication, "GPK File Explorer");

sttc ::gpk::s2_t explorerExpanded(const ::gfe::SApplication & app, ::gpk::vcst_t path) {
	for(::gpk::u2_t iPath = 0; iPath < app.ExpandedPaths.size(); ++iPath)
		if(app.ExpandedPaths[iPath] == path)
			return (::gpk::s2_t)iPath;
	return -1;
}

sttc ::gpk::err_t explorerScroll(::gfe::SApplication & app) {
	::gpk::SGUI					& gui					= *app.Framework.GUI;
	::gpk::pobj<::gpk::SDialogViewport>	viewport				= {};
	app.DialogMain.Controls[app.Viewport].as(viewport);
	if_null_fe(viewport);
	const ::gpk::s3_t			rowHeight				= app.ExplorerControls.IdControls.size() ? gui.Controls.Placement[app.ExplorerControls.IdControls[0]].Area.Size.y : 1;
	const ::gpk::s3_t			clientHeight			= gui.Controls.Placement[viewport->IdClient].Area.Size.y;
	const ::gpk::u2_t			pageSize				= (::gpk::u2_t)::gpk::max((::gpk::s3_t)1, clientHeight / ::gpk::max((::gpk::s3_t)1, rowHeight));
	const ::gpk::s3_t			scrollMax				= ::gpk::max((::gpk::s3_t)0, (::gpk::s3_t)app.ExplorerControls.IdControls.size() - pageSize);
	gui.Controls.SetHidden(app.ScrollY->IdGUIControl, 0 == scrollMax);
	if(scrollMax) {
		app.ScrollY->ValueLimits						= {0, scrollMax};
		if_fail_fe(::gpk::sliderSetValue(*app.ScrollY, app.ScrollY->ValueCurrent));
	}
	else
		app.ScrollY->ValueCurrent					= 0;
	if(app.ExplorerOffset != app.ScrollY->ValueCurrent || app.ExplorerPageSize != pageSize || app.ExplorerScrollMax != scrollMax) {
		app.ExplorerOffset							= app.ScrollY->ValueCurrent;
		app.ExplorerPageSize						= pageSize;
		app.ExplorerScrollMax						= scrollMax;
		gui.Controls.Placement[app.ExplorerControls.IdControl].Area.Offset.y = (::gpk::s1_t)(-app.ExplorerOffset * rowHeight);
		for(::gpk::u2_t iControl = 0; iControl < app.ExplorerControls.IdControls.size(); ++iControl)
			gui.Controls.SetHidden(app.ExplorerControls.IdControls[iControl], iControl < (::gpk::u2_t)app.ExplorerOffset || iControl >= (::gpk::u2_t)app.ExplorerOffset + pageSize);
		if_fail_fe(::gpk::controlMetricsInvalidate(gui, app.ExplorerControls.IdControl));
		if_fail_fe(::gpk::controlMetricsInvalidate(gui, app.ScrollY->IdGUIControl));
	}
	return 0;
}

sttc ::gpk::err_t explorerRefresh(::gfe::SApplication & app) {
	::gpk::SGUI					& gui					= *app.Framework.GUI;
	if(-1 != app.ExplorerControls.IdControl)
		if_fail_fe(::gpk::controlDelete(gui, app.ExplorerControls.IdControl));
	app.ExplorerControls									= {};
	if_fail_fe(app.ExplorerPaths.clear());
	if_fail_fe(app.ExplorerLabels.clear());
	if(app.RootExpanded) {
		if_fail_fe(::gpk::pathList(app.ExplorerTree, app.ExplorerPath, [&app](bool folder, ::gpk::vcst_c & path) -> ::gpk::err_t {
			if_fail_fe(app.ExplorerPaths.push_back(path));
			return folder && -1 == ::explorerExpanded(app, path) ? 2 : 0;
		}));
	}
	else {
		if_fail_fe(app.ExplorerTree.Reset());
	}

	if_fail_fe(app.ExplorerLabels.resize(app.ExplorerTree.Name.size() + 1));
	if_fail_fe(app.ExplorerLabels[0].append(app.RootExpanded ? "[-] " : "[+] "));
	if_fail_fe(app.ExplorerLabels[0].append(app.ExplorerPath));
	for(::gpk::u2_t iEntry = 0; iEntry < app.ExplorerTree.Name.size(); ++iEntry) {
		::gpk::string				& label					= app.ExplorerLabels[iEntry + 1];
		for(::gpk::u2_t iLevel = 0; iLevel <= app.ExplorerTree.Info[iEntry].NestLevel; ++iLevel)
			if_fail_fe(label.append("  "));
		if(app.ExplorerTree.Info[iEntry].Folder) {
			if_fail_fe(label.append(-1 != ::explorerExpanded(app, app.ExplorerPaths[iEntry]) ? "[-] " : "[+] "));
		}
		else {
			if_fail_fe(label.append("    "));
		}
		if_fail_fe(label.append(app.ExplorerTree.Name[iEntry]));
	}
	if_fail_fe(::gpk::controlListInitialize(gui, app.ExplorerControls));
	app.ExplorerControls.Orientation					= ::gpk::DIRECTION_DOWN;
	app.ExplorerPageSize							= 0;
	app.ExplorerOffset								= -1;
	app.ExplorerScrollMax							= -1;

	::gpk::pobj<::gpk::SDialogViewport>	viewport				= {};
	if_null_fe(app.DialogMain.Controls[app.Viewport].as(viewport));
	if_fail_fe(::gpk::controlSetParent(gui, app.ExplorerControls.IdControl, viewport->IdClient));
	gui.Controls.Placement[app.ExplorerControls.IdControl].Align = ::gpk::ALIGN_TOP_LEFT;

	for(::gpk::u2_t iLabel = 0; iLabel < app.ExplorerLabels.size(); ++iLabel)
		if_fail_fe(::gpk::controlListPush(gui, app.ExplorerControls, app.ExplorerLabels[iLabel], (::gpk::s3_t)iLabel - 1));
	if_fail_fe(::gpk::controlTextSet(gui, viewport->IdTitle, app.ExplorerPath));
	return 0;
}

sttc ::gpk::err_t explorerActivate(::gfe::SApplication & app, ::gpk::cid_t idControl) {
	::gpk::u2_t					iItem					= 0;
	for(; iItem < app.ExplorerControls.IdControls.size(); ++iItem)
		if(idControl == app.ExplorerControls.IdControls[iItem])
			break;
	if(iItem >= app.ExplorerControls.IdControls.size())
		return 0;

	const ::gpk::s3_t				iEntry					= app.ExplorerControls.Events[iItem];
	if(0 > iEntry) {
		app.RootExpanded									= !app.RootExpanded;
	}
	else {
		::gpk::s2_t					iExpansionAdded			= -1;
		if((::gpk::u2_t)iEntry >= app.ExplorerPaths.size())
			return 0;
		::gpk::vcst_t				entryPath				= app.ExplorerPaths[(::gpk::u2_t)iEntry];
		if(app.ExplorerTree.Info[(::gpk::u2_t)iEntry].Folder) {
			const ::gpk::s2_t			iExpanded				= ::explorerExpanded(app, entryPath);
			if(0 <= iExpanded) {
				if_fail_fe(app.ExpandedPaths[(::gpk::u2_t)iExpanded].clear());
			}
			else {
				::gpk::u2_t				iEmpty					= 0;
				for(; iEmpty < app.ExpandedPaths.size() && app.ExpandedPaths[iEmpty].size(); ++iEmpty) {}
				if(iEmpty < app.ExpandedPaths.size()) {
					app.ExpandedPaths[iEmpty]				= entryPath;
					iExpansionAdded							= (::gpk::s2_t)iEmpty;
				}
				else {
					if_fail_fe(iExpansionAdded = (::gpk::s2_t)app.ExpandedPaths.push_back(entryPath));
				}
			}
			const ::gpk::err_t		refreshResult				= ::explorerRefresh(app);
			if(::gpk::failed(refreshResult) && 0 <= iExpansionAdded) {
				if_fail_fe(app.ExpandedPaths[(::gpk::u2_t)iExpansionAdded].clear());
				return ::explorerRefresh(app);
			}
			return refreshResult;
		}
		else {
			::gpk::pobj<::gpk::SDialogViewport>	viewport		= {};
			app.DialogMain.Controls[app.Viewport].as(viewport);
			if_null_fe(viewport);
			return ::gpk::controlTextSet(*app.Framework.GUI, viewport->IdTitle, entryPath);
		}
	}
	return ::explorerRefresh(app);
}

::gpk::err_t cleanup(::gfe::SApplication & app) {
	return ::gpk::mainWindowDestroy(app.Framework.RootWindow);
}

::gpk::err_t setup(::gfe::SApplication & app) {
	::gpk::SFramework				& framework				= app.Framework;
	::gpk::SWindow					& mainWindow				= framework.RootWindow;
	mainWindow.Size										= {1280, 720};
	if_fail_fe(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input));
	::gpk::SGUI					& gui					= *framework.GUI;
	app.DialogMain.GUI									= framework.GUI;
	app.DialogMain.Input								= mainWindow.Input;
	gui.ColorModeDefault								= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault									= ::gpk::ASCII_COLOR_DARKGREEN * 16 + 7;

	app.IdExit										= ::gpk::controlCreate(gui);
	::gpk::SControlPlacement		& controlExit				= gui.Controls.Placement[app.IdExit];
	controlExit.Area									= {{0, 0}, {64, 20}};
	controlExit.Border								= {10, 10, 10, 10};
	controlExit.Margin								= {1, 1, 1, 1};
	controlExit.Align									= ::gpk::ALIGN_BOTTOM_RIGHT;
	if_fail_fe(::gpk::controlTextSet(gui, app.IdExit, "Exit"));
	gui.Controls.Text[app.IdExit].Align				= ::gpk::ALIGN_CENTER;
	if_fail_fe(::gpk::controlSetParent(gui, app.IdExit, -1));

	::gpk::pobj<::gpk::SDialogViewport>	viewport				= {};
	if_fail_fe(app.Viewport = ::gpk::viewportCreate(app.DialogMain, viewport));
	gui.Controls.Placement[viewport->IdGUIControl].Area	= {{96, 64}, {1024, 576}};
	gui.Controls.Placement[viewport->IdGUIControl].Align	= ::gpk::ALIGN_CENTER;
	if_fail_fe(::gpk::sliderCreate(app.DialogMain, app.ScrollY));
	app.ScrollY->Vertical							= true;
	gui.Controls.Placement[app.ScrollY->IdGUIControl].Area.Offset = {0, 20};
	gui.Controls.Placement[app.ScrollY->IdGUIControl].Area.Size = {8, 556};
	gui.Controls.Constraints[app.ScrollY->IdGUIControl].DockToControl.Top = viewport->IdGUIControl;
	gui.Controls.Constraints[app.ScrollY->IdGUIControl].DockToControl.Right = viewport->IdGUIControl;
	viewport->Settings.ScrollBarY					= true;
	viewport->IdScrollBarY							= app.ScrollY->IdGUIControl;
	if_fail_fe(::gpk::sliderSetValue(*app.ScrollY, 0));

	::gpk::string					absolutePath			= {};
	::gpk::vcst_t					rootPath					= {};
	if_fail_fe(::gpk::pathAbsolute(".", absolutePath));
	if_fail_fe(::gpk::pathBegin(absolutePath, rootPath));
	app.ExplorerPath									= rootPath.size() ? rootPath : ::gpk::vcst_t{absolutePath};
	return ::explorerRefresh(app);
}

::gpk::err_t draw(::gfe::SApplication & app) {
	::gpk::pobj<::gpk::rtbgra8d32>	target					= {};
	target.create();
	target->resize(app.Framework.RootWindow.Size, ::gpk::SColorBGRA{}, 0xFFFFFFFF);
	{
		::std::lock_guard			lock					(app.LockGUI);
		::gpk::guiDraw(*app.Framework.GUI, target->Color.View);
	}
	{
		::std::lock_guard			lock					(app.LockRender);
		app.Offscreen									= target;
	}
	return 0;
}

::gpk::err_t update(::gfe::SApplication & app, bool exitSignal) {
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, exitSignal);
	{
		::std::lock_guard			lock					(app.LockRender);
		app.Framework.RootWindow.BackBuffer				= app.Offscreen;
	}
	::std::lock_guard			lockGUI					(app.LockGUI);
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework));

	::gpk::SGUI					& gui					= *app.Framework.GUI;
	::gpk::SInput					& input					= *app.Framework.RootWindow.Input;
	::gpk::s3_t					scrollValue				= app.ScrollY->ValueCurrent;
	if(input.MouseCurrent.Deltas.z)
		scrollValue								-= input.MouseCurrent.Deltas.z / WHEEL_DELTA;
	if(input.KeyDown(VK_DOWN))					++scrollValue;
	if(input.KeyDown(VK_UP))					--scrollValue;
	if(input.KeyDown(VK_NEXT))					scrollValue += app.ExplorerPageSize;
	if(input.KeyDown(VK_PRIOR))					scrollValue -= app.ExplorerPageSize;
	if_fail_fe(::gpk::sliderSetValue(*app.ScrollY, scrollValue));
	::gpk::cid_t					explorerControl			= -1;
	if_fail_fe(::gpk::guiProcessControls(gui, app.ExplorerControls.IdControls, [&explorerControl](::gpk::cid_t idControl) { explorerControl = idControl; return 0; }));
	if(-1 != explorerControl)
		if_fail_fe(::explorerActivate(app, explorerControl));
	if(::gpk::guiProcessControls(gui, [&app](::gpk::cid_t idControl) { return idControl == app.IdExit ? 1 : 0; }))
		return ::gpk::APPLICATION_STATE_EXIT;
	if_fail_fe(app.DialogMain.Update());
	if_fail_fe(::explorerScroll(app));
	return 0;
}
