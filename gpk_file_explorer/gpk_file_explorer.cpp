#include "gpk_file_explorer.h"
#include "gpk_string_compose.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gfe::SApplication, "GPK File Explorer");

sttc ::gpk::cid_t demoControlCreate(::gpk::SGUI & gui, ::gpk::cid_t idParent, ::gpk::vcst_t identifier, ::gpk::vcst_t text, cnst ::gpk::rect2s1_t & area, ::gpk::ALIGN textAlign = ::gpk::ALIGN_CENTER_LEFT) {
	::gpk::cid_t idControl = -1;
	if_fail_fe(idControl = ::gpk::controlCreateChild(gui, idParent));
	gui.Controls.Identifier[idControl] = ::gpk::label(identifier);
	gui.Controls.Placement[idControl].Area = area;
	gui.Controls.Draw[idControl].ColorMode = ::gpk::GUI_COLOR_MODE_Flat;
	gui.Controls.Text[idControl].Text = text;
	gui.Controls.Text[idControl].Align = textAlign;
	rtrn idControl;
}

sttc ::gpk::err_t demoConfigLoad(::gfe::SApplication & app) {
	cnst ::gpk::SJSONReader & config = app.Framework.JSONConfig.Reader;
	if_true_fe(0 == config.Token.size());
	cnst ::gpk::err_t iFiles = ::gpk::jsonExpressionResolve("Files", config, 0);
	if_fail_fe(iFiles);
	cnst ::gpk::err_t fileCount = ::gpk::jsonArraySize(config, (::gpk::u2_t)iFiles);
	if_fail_fe(fileCount);
	for(::gpk::u2_t iFile = 0; iFile < (::gpk::u2_t)fileCount; ++iFile) {
		cnst ::gpk::err_t iFileNode = ::gpk::jsonArrayValueGet(config, (::gpk::u2_t)iFiles, iFile);
		if_fail_fe(iFileNode);
		::gpk::vcst_t label = {};
		::gpk::vcst_t path = {};
		if_fail_fe(::gpk::jsonObjectGetAsString(config, (::gpk::u2_t)iFileNode, "Label", label));
		if_fail_fe(::gpk::jsonObjectGetAsString(config, (::gpk::u2_t)iFileNode, "Path", path));
		if_fail_fe(app.SourceLabels.push_back(label));
		if_fail_fe(app.SourcePaths.push_back(path));
	}
	rtrn app.SourceLabels.size();
}

sttc ::gpk::err_t demoRowsBuild(cnst ::gpk::SJSONFile & data, ::gpk::aobj<::gfe::SJSONRow> & rows, ::gpk::u2_t iToken, ::gpk::vcst_t path) {
	cnst ::gpk::SJSONReader & reader = data.Reader;
	if_true_fe(iToken >= reader.Token.size());
	cnst ::gpk::JSON_TYPE type = reader.Token[iToken].Type;
	if(::gpk::JSON_TYPE_OBJECT == type)
		rtrn ::gpk::jsonObjectForEach(reader.Token, iToken, [&data, &rows, path, &reader](::gpk::u2_t iKey, ::gpk::u2_t iValue) -> ::gpk::err_t {
			::gpk::string childPath = path;
			if(childPath.size())
				if_fail_fe(childPath.push_back('.'));
			if_fail_fe(childPath.append(reader.View[iKey]));
			rtrn ::demoRowsBuild(data, rows, iValue, childPath);
		});
	if(::gpk::JSON_TYPE_ARRAY == type) {
		cnst ::gpk::err_t count = ::gpk::jsonArraySize(reader, iToken);
		if_fail_fe(count);
		for(::gpk::u2_t iElement = 0; iElement < (::gpk::u2_t)count; ++iElement) {
			cnst ::gpk::err_t iValue = ::gpk::jsonArrayValueGet(reader, iToken, iElement);
			if_fail_fe(iValue);
			::gpk::string childPath = path;
			if_fail_fe(::gpk::append_strings(childPath, "['", ::gpk::str(iElement), "']"));
			if_fail_fe(::demoRowsBuild(data, rows, (::gpk::u2_t)iValue, childPath));
		}
		rtrn count;
	}
	::gfe::SJSONRow row = {};
	row.Type = ::gpk::get_value_label(type);
	row.Name = path.size() ? path : ::gpk::vcst_t{"$"};
	if_fail_fe(::gpk::jsonWrite(reader.Token, data.Bytes, row.Value, iToken));
	rtrn rows.push_back(row);
}

sttc ::gpk::err_t demoRowsRefresh(::gfe::SApplication & app) {
	::gpk::SGUI & gui = *app.Framework.GUI;
	for(::gpk::u2_t iRow = 0; iRow < ::gfe::JSON_ROW_COUNT; ++iRow) {
		cnst ::gpk::u2_t iData = app.RowOffset + iRow;
		cnst bool visible = iData < app.Rows.size();
		gui.Controls.SetHidden(app.IdTypes [iRow], false == visible);
		gui.Controls.SetHidden(app.IdNames [iRow], false == visible);
		gui.Controls.SetHidden(app.IdValues[iRow], false == visible);
		if(false == visible)
			continue;
		if_fail_fe(::gpk::controlTextSet(gui, app.IdTypes [iRow], app.Rows[iData].Type));
		if_fail_fe(::gpk::controlTextSet(gui, app.IdNames [iRow], app.Rows[iData].Name));
		if_fail_fe(::gpk::controlTextSet(gui, app.IdValues[iRow], app.Rows[iData].Value));
	}
	for(::gpk::u2_t iSource = 0; iSource < app.IdSources.size(); ++iSource)
		gui.Controls.SetSelected(app.IdSources[iSource], iSource == app.SelectedSource);
	cnst ::gpk::u2_t first = app.Rows.size() ? app.RowOffset + 1 : 0;
	cnst ::gpk::u2_t last = ::gpk::min(app.RowOffset + ::gfe::JSON_ROW_COUNT, app.Rows.size());
	app.Status.clear();
	if_fail_fe(::gpk::append_strings(app.Status, app.SourceLabels[app.SelectedSource], " | rows ", ::gpk::str(first), "-", ::gpk::str(last), " of ", ::gpk::str(app.Rows.size())));
	rtrn ::gpk::controlTextSet(gui, app.IdStatus, app.Status);
}

sttc ::gpk::err_t demoSourceLoad(::gfe::SApplication & app, ::gpk::u2_t iSource) {
	if_true_fe(iSource >= app.SourcePaths.size());
	::gpk::SJSONFile data = {};
	::gpk::aobj<::gfe::SJSONRow> rows = {};
	if_fail_fe(::gpk::jsonFileRead(data, app.SourcePaths[iSource]));
	if_fail_fe(::demoRowsBuild(data, rows, 0, {}));
	app.Rows = rows;
	app.SelectedSource = iSource;
	app.RowOffset = 0;
	rtrn ::demoRowsRefresh(app);
}

sttc ::gpk::err_t demoControlListCreate(::gfe::SApplication & app, ::gpk::cid_t idParent, ::gpk::vcst_t identifierPrefix, ::gpk::acid & ids, ::gpk::s1_t rowHeight) {
	::gpk::aobj<::gpk::vcst_t> labels = {};
	if_fail_fe(labels.resize(::gfe::JSON_ROW_COUNT, " "));
	::gpk::SControlPlacement placement = {};
	placement.Area = {{0, 32}, {16, rowHeight}};
	placement.Margin = {1, 1, 1, 1};
	placement.Border = {1, 1, 1, 1};
	if_fail_fe(::gpk::guiCreateControlList(*app.Framework.GUI, ::gpk::DIRECTION_DOWN, {labels.begin(), labels.size()}, idParent, placement, ::gpk::ALIGN_CENTER_LEFT, ids));
	for(::gpk::u2_t iControl = 0; iControl < ids.size(); ++iControl) {
		::gpk::string identifier = {};
		if_fail_fe(::gpk::append_strings(identifier, identifierPrefix, "_", ::gpk::str(iControl)));
		app.Framework.GUI->Controls.Identifier[ids[iControl]] = ::gpk::label(identifier);
		app.Framework.GUI->Controls.Draw[ids[iControl]].ColorMode = ::gpk::GUI_COLOR_MODE_Flat;
	}
	rtrn ids.size();
}

sttc ::gpk::err_t demoStyleChildren(::gpk::SGUI & gui, ::gpk::cid_t idParent, ::gpk::vcid ids) {
	for(::gpk::u2_t iControl = 0; iControl < ids.size(); ++iControl) {
		gui.Controls.Draw[ids[iControl]] = gui.Controls.Draw[idParent];
		gui.Controls.Text[ids[iControl]].FontSelected = gui.Controls.Text[idParent].FontSelected;
	}
	rtrn ids.size();
}

sttc ::gpk::err_t demoGUISetup(::gfe::SApplication & app) {
	::gpk::SGUI & gui = *app.Framework.GUI;
	if_fail_fe(app.IdRoot = ::demoControlCreate(gui, app.DialogMain.Root, "file_explorer", "", {{0, 0}, {1280, 720}}));
	if_fail_fe(app.IdTitle = ::demoControlCreate(gui, app.IdRoot, "title", "CTTI JSON WORKBENCH", {{18, 16}, {1244, 44}}, ::gpk::ALIGN_CENTER));
	if_fail_fe(app.IdSourcesPanel = ::demoControlCreate(gui, app.IdRoot, "sources", "DATASETS", {{18, 76}, {222, 562}}, ::gpk::ALIGN_TOP_LEFT));
	if_fail_fe(app.IdTypePanel = ::demoControlCreate(gui, app.IdRoot, "types", "TYPE", {{256, 76}, {150, 562}}, ::gpk::ALIGN_TOP_LEFT));
	if_fail_fe(app.IdNamePanel = ::demoControlCreate(gui, app.IdRoot, "names", "MEMBER", {{414, 76}, {310, 562}}, ::gpk::ALIGN_TOP_LEFT));
	if_fail_fe(app.IdValuePanel = ::demoControlCreate(gui, app.IdRoot, "values", "VALUE", {{732, 76}, {530, 562}}, ::gpk::ALIGN_TOP_LEFT));
	if_fail_fe(app.IdStatus = ::demoControlCreate(gui, app.IdRoot, "status", "", {{18, 652}, {900, 38}}));
	if_fail_fe(app.IdPrevious = ::demoControlCreate(gui, app.IdRoot, "previous", "PREVIOUS", {{936, 652}, {96, 38}}, ::gpk::ALIGN_CENTER));
	if_fail_fe(app.IdNext = ::demoControlCreate(gui, app.IdRoot, "next", "NEXT", {{1038, 652}, {96, 38}}, ::gpk::ALIGN_CENTER));
	if_fail_fe(app.IdExit = ::demoControlCreate(gui, app.IdRoot, "exit", "EXIT", {{1140, 652}, {122, 38}}, ::gpk::ALIGN_CENTER));

	::gpk::SControlPlacement sourcePlacement = {};
	sourcePlacement.Area = {{0, 32}, {16, 30}};
	sourcePlacement.Margin = {1, 1, 1, 1};
	sourcePlacement.Border = {1, 1, 1, 1};
	if_fail_fe(::gpk::guiCreateControlList(gui, ::gpk::DIRECTION_DOWN, {app.SourceLabels.begin(), app.SourceLabels.size()}, app.IdSourcesPanel, sourcePlacement, ::gpk::ALIGN_CENTER_LEFT, app.IdSources));
	for(::gpk::u2_t iSource = 0; iSource < app.IdSources.size(); ++iSource) {
		::gpk::string identifier = {};
		if_fail_fe(::gpk::append_strings(identifier, "source_", ::gpk::str(iSource)));
		gui.Controls.Identifier[app.IdSources[iSource]] = ::gpk::label(identifier);
	}
	if_fail_fe(::demoControlListCreate(app, app.IdTypePanel , "type" , app.IdTypes , 28));
	if_fail_fe(::demoControlListCreate(app, app.IdNamePanel , "name" , app.IdNames , 28));
	if_fail_fe(::demoControlListCreate(app, app.IdValuePanel, "value", app.IdValues, 28));

	cnst ::gpk::SJSONReader & config = app.Framework.JSONConfig.Reader;
	if_fail_fe(::gpk::guiJSONLoad(gui, config, 0));
	if_fail_fe(::demoStyleChildren(gui, app.IdSourcesPanel, app.IdSources));
	if_fail_fe(::demoStyleChildren(gui, app.IdTypePanel, app.IdTypes));
	if_fail_fe(::demoStyleChildren(gui, app.IdNamePanel, app.IdNames));
	if_fail_fe(::demoStyleChildren(gui, app.IdValuePanel, app.IdValues));
	rtrn 0;
}

::gpk::err_t cleanup(::gfe::SApplication & app) { return ::gpk::mainWindowDestroy(app.Framework.RootWindow); }
::gpk::err_t setup	(::gfe::SApplication & app) {
	::gpk::SWindow				& window				= app.Framework.RootWindow;
	window.Size											= {1280, 720};
	if_null_fe(window.BackBuffer.create());
	if_fail_fe(window.BackBuffer->resize(window.Size.u2()));
	if_fail_fe(::gpk::mainWindowCreate(window, app.Framework.RuntimeValues.PlatformDetail, window.Input));
	app.DialogMain.Input								= window.Input;
	if_fail_fe(::demoConfigLoad(app));
	if_fail_fe(::demoGUISetup(app));
	if_fail_fe(::demoSourceLoad(app, 0));
	return 0;
}
::gpk::err_t draw	(::gfe::SApplication & app) {
	::gpk::pobj<::gpk::rtbgra8d32>	target					= {};
	target.create();
	target->resize(app.Framework.RootWindow.Size, ::gpk::SColorBGRA{}, 0xFFFFFFFF);
	{
		::gpk::lock_guard			lock					(app.LockGUI);
		::gpk::guiDraw(*app.Framework.GUI, target->Color.View);
	}
	{
		::gpk::lock_guard			lock					(app.LockRender);
		app.Offscreen									= target;
	}
	return 0;
}

::gpk::err_t update(::gfe::SApplication & app, gpk::b8_t exitSignal) {
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, exitSignal);
	{
		::gpk::lock_guard			lock					(app.LockRender);
		app.Framework.RootWindow.BackBuffer				= app.Offscreen;
	}
	::gpk::lock_guard			lockGUI					(app.LockGUI);
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework));

	::gpk::SGUI					& gui					= *app.Framework.GUI;
	::gpk::b8_t exitRequested = false;
	if_fail_fe(::gpk::guiProcessControls(gui, [&app, &gui, &exitRequested](::gpk::cid_t idControl) -> ::gpk::err_t {
		if(idControl == app.IdExit)
			exitRequested = true;
		else if(idControl == app.IdPrevious) {
			app.RowOffset = app.RowOffset > ::gfe::JSON_ROW_COUNT ? app.RowOffset - ::gfe::JSON_ROW_COUNT : 0;
			if_fail_fe(::demoRowsRefresh(app));
		}
		else if(idControl == app.IdNext) {
			if(app.RowOffset + ::gfe::JSON_ROW_COUNT < app.Rows.size())
				app.RowOffset += ::gfe::JSON_ROW_COUNT;
			if_fail_fe(::demoRowsRefresh(app));
		}
		else
			for(::gpk::u2_t iSource = 0; iSource < app.IdSources.size(); ++iSource)
				if(idControl == app.IdSources[iSource]) {
					if(::gpk::failed(::demoSourceLoad(app, iSource)))
						::gpk::controlTextSet(gui, app.IdStatus, "LOAD FAILED - PREVIOUS DATA PRESERVED");
					break;
				}
		rtrn 0;
	}));
	if(exitRequested)
		return ::gpk::APPLICATION_STATE_EXIT;

	if_fail_fe(app.DialogMain.Update());
	return 0;
}
