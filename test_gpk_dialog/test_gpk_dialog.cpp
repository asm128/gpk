#include "test_gpk_dialog.h"

#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Indexed File Explorer");

using namespace gpk;
using namespace gme;

stxp u2_t INVALID_FOLDER = (u2_t)-1;

sttc err_t controlUsePalette(SApplication & app, cid_t idControl, cnst asts1_t<GUI_CONTROL_PALETTE_COUNT> & palette) {
	SControlDraw & draw = app.Framework.GUI->Controls.Draw[idControl];
	draw.UseNewPalettes = true;
	draw.ColorMode = GUI_COLOR_MODE_Flat;
	draw.Palettes = palette;
	rtrn 0;
}

sttc err_t listCreate(SApplication & app, SControlList & list) {
	SGUI & gui = *app.Framework.GUI;
	if_fail_fe(controlListInitialize(gui, list));
	list.Orientation = DIRECTION_DOWN;
	pobj<SDialogViewport> viewport = {};
	if_null_fe(app.DialogMain.Controls[app.Viewport].as(viewport));
	if_fail_fe(controlSetParent(gui, list.IdControl, viewport->IdClient));
	SControlPlacement & placement = gui.Controls.Placement[list.IdControl];
	placement.Area = {};
	placement.Align = ALIGN_TOP_LEFT;
	gui.Controls.Draw[list.IdControl].NoBorder = true;
	gui.Controls.Draw[list.IdControl].NoClient = true;
	gui.Controls.States[list.IdControl].Mask &= ~(GUI_CONTROL_FLAG_Hovered | GUI_CONTROL_FLAG_Pressed | GUI_CONTROL_FLAG_Action);
	rtrn 0;
}

sttc err_t folderCreate(SApplication & app, vcst_t path, u2_t depth, u2_t & index) {
	pobj<::gme::SExplorerFolder> folder = {};
	if_null_fe(folder.create());
	folder->Path = path;
	folder->Depth = depth;
	if_fail_fe(::listCreate(app, folder->Controls));
	if_fail_fe(index = app.Explorer.Folder.push_back(folder));
	rtrn 0;
}

sttc err_t folderScanStart(SApplication & app, u2_t iFolder) {
	if_true_fef(iFolder >= app.Explorer.Folder.size(), "Invalid folder: %u.", iFolder);
	::gme::SExplorerFolder & folder = *app.Explorer.Folder[iFolder];
	if(folder.Loaded || folder.Loading)
		rtrn 0;
	folder.Loading = true;
	folder.Worker = ::std::thread([&folder]() {
		folder.ScanResult.store(pathList(folder.Tree, folder.Path, [](bool isFolder, vcst_c &) -> err_t { rtrn isFolder ? 2 : 0; }));
	});
	rtrn 0;
}

sttc err_t folderPublish(SApplication & app, u2_t iFolder) {
	if_true_fef(iFolder >= app.Explorer.Folder.size(), "Invalid folder: %u.", iFolder);
	::gme::SExplorerFolder & folder = *app.Explorer.Folder[iFolder];
	if(folder.Loaded || false == folder.Loading || INT_MAX == folder.ScanResult.load())
		rtrn 0;
	if(folder.Worker.joinable())
		folder.Worker.join();
	cnst err_t scanResult = folder.ScanResult.load();
	folder.Loaded = true;
	folder.Loading = false;
	if_fail_fe(folder.Labels.resize(folder.Tree.Name.size()));
	if_fail_fe(folder.ChildFolder.resize(folder.Tree.Name.size()));
	if_fail_fe(folder.ChildFolder.fill(INVALID_FOLDER));
	SGUI & gui = *app.Framework.GUI;
	for(u2_t iEntry = 0; iEntry < folder.Tree.Name.size(); ++iEntry) {
		cnst pobj<SPathNode> & node = folder.Tree.Tree[iEntry];
		if_true_fef(!node || !node->Info, "Invalid path node: %u.", iEntry);
		string & label = folder.Labels[iEntry];
		if_fail_fe(label.append(node->Info->Folder ? "[+] " : "    "));
		if_fail_fe(label.append(folder.Tree.Name[iEntry]));

		if(node->Info->Folder) {
			string childPath = {};
			if_fail_fe(pathNodePath(childPath, folder.Tree, *node));
			if_fail_fe(::folderCreate(app, childPath, folder.Depth + 1, folder.ChildFolder[iEntry]));
		}

		cnst cid_t idControl = controlCreate(gui);
		if_fail_fe(idControl);
		if_fail_fe(controlSetParent(gui, idControl, folder.Controls.IdControl));
		SControlPlacement & placement = gui.Controls.Placement[idControl];
		placement.Area.Size = {640, 20};
		placement.Border = {1, 1, 1, 1};
		placement.Margin = {2, 1, 2, 1};
		SControlText & text = gui.Controls.Text[idControl];
		text.Text = label;
		text.Align = ALIGN_CENTER_LEFT;
		if_fail_fe(::controlUsePalette(app, idControl, app.DialogMain.Colors->Button));
		if_fail_fe(folder.Controls.IdControls.push_back(idControl));
		if_fail_fe(folder.Controls.Events.push_back((s3_t)iEntry));
		if_fail_fe(app.Explorer.RowControl.push_back(idControl));
		if_fail_fe(app.Explorer.RowFolder.push_back(iFolder));
		if_fail_fe(app.Explorer.RowEntry.push_back(iEntry));
	}
	if_fail_fe(controlListArrange(gui, folder.Controls));
	if(::gpk::failed(scanResult))
		warning_printf("Folder scan retained %u entries before failing: '%.*s'.", folder.Tree.Name.size(), GPK_VIEW_ARGS(folder.Path));
	rtrn 0;
}

sttc err_t foldersPublish(SApplication & app) {
	for(u2_t iFolder = 0; iFolder < app.Explorer.Folder.size(); ++iFolder)
		if_fail_fe(::folderPublish(app, iFolder));
	rtrn 0;
}

sttc err_t visibleAppend(SApplication & app, u2_t iFolder) {
	if_true_fef(iFolder >= app.Explorer.Folder.size(), "Invalid folder: %u.", iFolder);
	cnst ::gme::SExplorerFolder & folder = *app.Explorer.Folder[iFolder];
	if(false == folder.Expanded)
		rtrn 0;
	for(u2_t iEntry = 0; iEntry < folder.Controls.IdControls.size(); ++iEntry) {
		if_fail_fe(app.Explorer.VisibleFolder.push_back(iFolder));
		if_fail_fe(app.Explorer.VisibleEntry.push_back(iEntry));
		cnst u2_t iChild = folder.ChildFolder[iEntry];
		if(iChild != INVALID_FOLDER)
			if_fail_fe(::visibleAppend(app, iChild));
	}
	rtrn 0;
}

sttc bool indexPathExcluded(vcst_t path) {
	vcst_t name = {};
	if(failed(pathFilename(path, name)))
		rtrn true;
	stxp vcst_c excluded[] = {".git", ".vs", "node_modules", "obj", "__pycache__", ".venv", "venv", "env"};
	for(u2_t iName = 0; iName < size(excluded); ++iName)
		if(name == excluded[iName])
			rtrn true;
	rtrn false;
}

sttc bool indexFileAccepted(vcst_t path) {
	vcst_t name = {};
	if(failed(pathFilename(path, name)))
		rtrn false;
	vcst_t extension = {};
	if(name.size() > 4) {
		name.slice(extension, name.size() - 4);
		if(extension == ".cpp")
			rtrn true;
	}
	if(name.size() > 2) {
		name.slice(extension, name.size() - 2);
		if(extension == ".h")
			rtrn true;
	}
	rtrn false;
}

sttc err_t indexBuild(SApplication & app) {
	::gme::SSearch & search = app.Search;
	cnst err_t scanResult = pathList(search.Tree, app.RootPath, [](bool folder, vcst_c & path) -> err_t {
		if(folder)
			rtrn ::indexPathExcluded(path) ? 1 : 0;
		rtrn ::indexFileAccepted(path) ? 0 : 1;
	});
	if_fail_fe(scanResult);
	search.Data.IndexPath = true;
	if_fail_fe(getWordDict(search.Data, search.Word, search.Reverse, search.Tree));
	rtrn 0;
}

sttc err_t searchControlsEnsure(SApplication & app, u2_t count) {
	::gme::SSearch & search = app.Search;
	SGUI & gui = *app.Framework.GUI;
	while(search.Controls.IdControls.size() < count) {
		pobj<string> label = {};
		if_null_fe(label.create());
		cnst cid_t idControl = controlCreate(gui);
		if_fail_fe(idControl);
		if_fail_fe(controlSetParent(gui, idControl, search.Controls.IdControl));
		gui.Controls.Placement[idControl].Area.Size = {640, 20};
		gui.Controls.Placement[idControl].Border = {1, 1, 1, 1};
		gui.Controls.Placement[idControl].Margin = {2, 1, 2, 1};
		gui.Controls.Text[idControl].Align = ALIGN_CENTER_LEFT;
		if_fail_fe(::controlUsePalette(app, idControl, app.DialogMain.Colors->Button));
		if_fail_fe(search.Labels.push_back(label));
		if_fail_fe(search.Controls.IdControls.push_back(idControl));
		if_fail_fe(search.Controls.Events.push_back((s3_t)search.Controls.Events.size()));
	}
	rtrn 0;
}

sttc err_t searchUpdate(SApplication & app) {
	::gme::SSearch & search = app.Search;
	search.Query = app.InputBox.Text;
	trim(search.Query);
	if_fail_fe(search.Result.clear());
	if(search.Query.size() && search.Ready)
		if_fail_fe(wordDictResolve(search.Word, search.Query, search.Tree.Name.size(), search.Result));
	if_fail_fe(::searchControlsEnsure(app, search.Result.size()));
	SGUI & gui = *app.Framework.GUI;
	for(u2_t iResult = 0; iResult < search.Controls.IdControls.size(); ++iResult) {
		cnst bool visible = iResult < search.Result.size();
		gui.Controls.SetHidden(search.Controls.IdControls[iResult], false == visible);
		if(false == visible)
			continue;
		cnst u2_t iItem = search.Result[iResult];
		if_true_fef(iItem >= search.Tree.Tree.size(), "Invalid search result: %u.", iItem);
		string & label = *search.Labels[iResult];
		if_fail_fe(pathNodePath(label, search.Tree, *search.Tree.Tree[iItem]));
		gui.Controls.Text[search.Controls.IdControls[iResult]].Text = label;
	}
	app.ScrollOffset = 0;
	if_fail_fe(sliderSetValue(*app.ScrollY, 0));
	rtrn 0;
}

sttc err_t titleSet(SApplication & app, vcst_t text) {
	app.ViewportTitle = text;
	pobj<SDialogViewport> viewport = {};
	if_null_fe(app.DialogMain.Controls[app.Viewport].as(viewport));
	rtrn controlTextSet(*app.Framework.GUI, viewport->IdTitle, app.ViewportTitle);
}

sttc err_t rowActivate(SApplication & app, cid_t idControl) {
	for(u2_t iRow = 0; iRow < app.Explorer.RowControl.size(); ++iRow) {
		if(app.Explorer.RowControl[iRow] != idControl)
			continue;
		cnst u2_t iFolder = app.Explorer.RowFolder[iRow];
		cnst u2_t iEntry = app.Explorer.RowEntry[iRow];
		::gme::SExplorerFolder & folder = *app.Explorer.Folder[iFolder];
		cnst u2_t iChild = folder.ChildFolder[iEntry];
		if(iChild != INVALID_FOLDER) {
			::gme::SExplorerFolder & child = *app.Explorer.Folder[iChild];
			if(false == child.Loaded && false == child.Loading)
				if_fail_fe(::folderScanStart(app, iChild));
			child.Expanded = !child.Expanded;
			folder.Labels[iEntry][1] = child.Expanded ? '-' : '+';
			app.Framework.GUI->Controls.Text[idControl].Text = folder.Labels[iEntry];
			if_fail_fe(::titleSet(app, child.Path));
		}
		else {
			string path = {};
			if_fail_fe(pathNodePath(path, folder.Tree, *folder.Tree.Tree[iEntry]));
			if_fail_fe(::titleSet(app, path));
		}
		rtrn 1;
	}
	rtrn 0;
}

sttc err_t resultActivate(SApplication & app, cid_t idControl) {
	for(u2_t iResult = 0; iResult < app.Search.Result.size(); ++iResult) {
		if(app.Search.Controls.IdControls[iResult] != idControl)
			continue;
		cnst u2_t iItem = app.Search.Result[iResult];
		string path = {};
		if_fail_fe(pathNodePath(path, app.Search.Tree, *app.Search.Tree.Tree[iItem]));
		if_fail_fe(::titleSet(app, path));
		rtrn 1;
	}
	rtrn 0;
}

sttc err_t rowsLayout(SApplication & app) {
	SGUI & gui = *app.Framework.GUI;
	pobj<SDialogViewport> viewport = {};
	if_null_fe(app.DialogMain.Controls[app.Viewport].as(viewport));
	cnst n2s1_t clientSize = gui.Controls.Placement[viewport->IdClient].Area.Size;
	cnst s3_t rowHeight = gui.Fonts[gui.SelectedFont]->CharSize.y + 6;
	app.PageSize = (u2_t)max((s3_t)1, clientSize.y / max((s3_t)1, rowHeight));

	cnst bool searching = app.Search.Query.size();
	if_fail_fe(app.Explorer.VisibleFolder.clear());
	if_fail_fe(app.Explorer.VisibleEntry.clear());
	if(false == searching)
		if_fail_fe(::visibleAppend(app, 0));
	cnst u2_t rowCount = searching ? app.Search.Result.size() : app.Explorer.VisibleFolder.size();
	cnst s3_t scrollMax = max((s3_t)0, (s3_t)rowCount - app.PageSize);
	app.ScrollY->ValueLimits = {0, scrollMax};
	if_fail_fe(sliderSetValue(*app.ScrollY, app.ScrollY->ValueCurrent));
	app.ScrollOffset = app.ScrollY->ValueCurrent;

	for(u2_t iFolder = 0; iFolder < app.Explorer.Folder.size(); ++iFolder) {
		::gme::SExplorerFolder & folder = *app.Explorer.Folder[iFolder];
		gui.Controls.SetHidden(folder.Controls.IdControl, true);
		gui.Controls.Placement[folder.Controls.IdControl].Area.Size = clientSize;
		for(u2_t iEntry = 0; iEntry < folder.Controls.IdControls.size(); ++iEntry)
			gui.Controls.SetHidden(folder.Controls.IdControls[iEntry], true);
	}
	gui.Controls.SetHidden(app.Search.Controls.IdControl, false == searching);
	gui.Controls.Placement[app.Search.Controls.IdControl].Area.Size = clientSize;
	for(u2_t iResult = 0; iResult < app.Search.Controls.IdControls.size(); ++iResult)
		gui.Controls.SetHidden(app.Search.Controls.IdControls[iResult], true);

	cnst u2_t pageEnd = min(rowCount, (u2_t)app.ScrollOffset + app.PageSize);
	for(u2_t iVisible = (u2_t)app.ScrollOffset; iVisible < pageEnd; ++iVisible) {
		cid_t idControl = -1;
		u2_t depth = 0;
		if(searching) {
			idControl = app.Search.Controls.IdControls[iVisible];
		}
		else {
			cnst u2_t iFolder = app.Explorer.VisibleFolder[iVisible];
			cnst u2_t iEntry = app.Explorer.VisibleEntry[iVisible];
			::gme::SExplorerFolder & folder = *app.Explorer.Folder[iFolder];
			gui.Controls.SetHidden(folder.Controls.IdControl, false);
			idControl = folder.Controls.IdControls[iEntry];
			depth = folder.Depth;
		}
		gui.Controls.SetHidden(idControl, false);
		SControlPlacement & placement = gui.Controls.Placement[idControl];
		placement.Area.Offset = {(s1_t)(8 + depth * 16), (s1_t)((iVisible - app.ScrollOffset) * rowHeight)};
		placement.Area.Size = {(s1_t)max((s3_t)64, (s3_t)clientSize.x - placement.Area.Offset.x - 14), (s1_t)rowHeight};
		gui.Controls.Constraints[idControl].DockToControl = {-1, -1, -1, -1};
		gui.Controls.Constraints[idControl].AttachSizeToControl = {-1, -1};
	}
	viewport->Settings.ScrollBarY = 0 != scrollMax;
	gui.Controls.SetHidden(app.ScrollY->IdGUIControl, 0 == scrollMax);
	if_fail_fe(controlMetricsInvalidate(gui, viewport->IdClient));
	rtrn 0;
}

err_t cleanup(::gme::SApplication & app) {
	if(app.Search.Worker.joinable())
		app.Search.Worker.join();
	for(u2_t iFolder = 0; iFolder < app.Explorer.Folder.size(); ++iFolder)
		if(app.Explorer.Folder[iFolder]->Worker.joinable())
			app.Explorer.Folder[iFolder]->Worker.join();
	rtrn mainWindowDestroy(app.Framework.RootWindow);
}

err_t setup(::gme::SApplication & app) {
	SFramework & framework = app.Framework;
	SWindow & mainWindow = framework.RootWindow;
	framework.GUI = app.DialogMain.GUI;
	mainWindow.Size = {1280, 800};
	if_fail_fe(mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input));
	app.DialogMain.Input = mainWindow.Input;
	SGUI & gui = *framework.GUI;
	gui.ColorModeDefault = GUI_COLOR_MODE_Flat;
	gui.ThemeDefault = ASCII_COLOR_DARKGREY * 16 + 7;

	app.IdExit = controlCreate(gui);
	if_fail_fe(app.IdExit);
	SControlPlacement & exitPlacement = gui.Controls.Placement[app.IdExit];
	exitPlacement.Area = {{-12, -12}, {88, 24}};
	exitPlacement.Align = ALIGN_BOTTOM_RIGHT;
	gui.Controls.Text[app.IdExit].Text = "Exit";
	gui.Controls.Text[app.IdExit].Align = ALIGN_CENTER;
	if_fail_fe(::controlUsePalette(app, app.IdExit, app.DialogMain.Colors->Button));

	pobj<SDialogViewport> viewport = {};
	if_fail_fe(app.Viewport = viewportCreate(app.DialogMain, viewport));
	gui.Controls.Placement[viewport->IdGUIControl].Area = {{0, 32}, {1180, 500}};
	gui.Controls.Placement[viewport->IdGUIControl].Align = ALIGN_CENTER_TOP;
	if_fail_fe(controlTextSet(gui, viewport->IdTitle, "Indexed File Explorer"));

	if_fail_fe(sliderCreate(app.DialogMain, app.ScrollY));
	app.ScrollY->Vertical = true;
	viewport->IdScrollBarY = app.ScrollY->IdGUIControl;
	viewport->Settings.ScrollBarY = true;
	if_fail_fe(sliderSetValue(*app.ScrollY, 0));

	if_fail_fe(inputBoxCreate(app.InputBox, gui));
	app.InputBox.MaxLength = 96;
	gui.Controls.Placement[app.InputBox.IdRoot].Align = ALIGN_CENTER_BOTTOM;
	gui.Controls.Placement[app.InputBox.IdRoot].Area.Offset.y = -16;
	if_fail_fe(app.InputBox.Edit(gui, true));

	if_fail_fe(pathAbsolute(".", app.RootPath));
	u2_t rootFolder = INVALID_FOLDER;
	if_fail_fe(::folderCreate(app, app.RootPath, 0, rootFolder));
	if_true_fef(0 != rootFolder, "Unexpected root folder index: %u.", rootFolder);
	app.Explorer.Folder[0]->Expanded = true;
	if_fail_fe(::folderScanStart(app, 0));
	if_fail_fe(::listCreate(app, app.Search.Controls));
	if_fail_fe(::titleSet(app, app.RootPath));
	app.Search.Worker = ::std::thread([&app]() { app.Search.BuildResult.store(::indexBuild(app)); });
	rtrn 0;
}

err_t update(::gme::SApplication & app, bool exitSignal) {
	if_true_vi(APPLICATION_STATE_EXIT, exitSignal);
	{
		lock_guard lock(app.LockRender);
		app.Framework.RootWindow.BackBuffer = app.Offscreen;
	}
	if_true_vi(APPLICATION_STATE_EXIT, APPLICATION_STATE_EXIT == updateFramework(app.Framework));
	{
		lock_guard lockGUI(app.Framework.LockGUI);
		SGUI & gui = *app.Framework.GUI;
		acid controlsToProcess = {};
		if_fail_fe(::foldersPublish(app));
		if_fail_fe(guiGetProcessableControls(gui, controlsToProcess));
		if(false == app.Search.Published && INT_MAX != app.Search.BuildResult.load()) {
			if(app.Search.Worker.joinable())
				app.Search.Worker.join();
			app.Search.Published = true;
			app.Search.Ready = succeeded(app.Search.BuildResult.load());
			if(app.Search.Query.size())
				if_fail_fe(::searchUpdate(app));
		}

		cnst string queryBefore = app.InputBox.Text;
		if_fail_fe(app.InputBox.Update(gui, app.InputBox.VirtualKeyboard, app.Framework.RootWindow.EventQueue, controlsToProcess));
		if(queryBefore != app.InputBox.Text)
			if_fail_fe(::searchUpdate(app));

		cid_t activated = -1;
		bool exitClicked = false;
		if_fail_fe(guiProcessControls(gui, controlsToProcess, [&activated, &exitClicked, &app](cid_t idControl) {
			if(idControl == app.IdExit) {
				exitClicked = true;
				rtrn 1;
			}
			if(0 <= find(idControl, {app.Explorer.RowControl.begin(), app.Explorer.RowControl.size()}) || 0 <= find(idControl, {app.Search.Controls.IdControls.begin(), app.Search.Controls.IdControls.size()}))
				activated = idControl;
			rtrn 0;
		}));
		if(exitClicked)
			rtrn APPLICATION_STATE_EXIT;
		if(-1 != activated) {
			if(app.Search.Query.size()) {
				if_fail_fe(::resultActivate(app, activated));
			}
			else {
				if_fail_fe(::rowActivate(app, activated));
			}
		}

		SInput & input = *app.Framework.RootWindow.Input;
		s3_t scrollValue = app.ScrollY->ValueCurrent;
		if(input.MouseCurrent.Deltas.z)
			scrollValue -= input.MouseCurrent.Deltas.z / WHEEL_DELTA;
		if(input.KeyDown(VK_DOWN)) ++scrollValue;
		if(input.KeyDown(VK_UP)) --scrollValue;
		if(input.KeyDown(VK_NEXT)) scrollValue += app.PageSize;
		if(input.KeyDown(VK_PRIOR)) scrollValue -= app.PageSize;
		if_fail_fe(sliderSetValue(*app.ScrollY, scrollValue));
		if_fail_fe(app.DialogMain.Update());
		if_fail_fe(::rowsLayout(app));
	}
	sleep(1);
	rtrn 0;
}

err_t draw(::gme::SApplication & app) {
	pobj<rtbgra8d32> target = {};
	if_null_fe(target.create());
	if_fail_fe(target->resize(app.Framework.RootWindow.Size, DARKGRAY, 0xFFFFFFFFU));
	{
		lock_guard lock(app.Framework.LockGUI);
		if_fail_fe(guiDraw(*app.Framework.GUI, target->Color.View));
	}
	{
		lock_guard lock(app.LockRender);
		app.Offscreen = target;
	}
	rtrn 0;
}
