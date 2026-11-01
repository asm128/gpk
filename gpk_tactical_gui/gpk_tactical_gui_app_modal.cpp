#include "gpk_tactical_gui_app.h"

::gpk::error_t						dry::handleResizeScreenModal	(::dry::SApplication & app, ::gpk::n2s1_t newSize)	{
	(void)newSize;
	::gpk::SGUI								& gui							= *app.Framework.GUI;
	const ::gpk::cid_t						indexDialog						= app.Screens.Modal.ControlIds[::dry::SCREEN_CONTROL_MODAL_DIALOG];
	gui.Controls.Placement[indexDialog].Area.Size	= (::dry::MODAL_SCAN_SPOOL == app.Screens.ModalSelected)
		? ::gpk::n2s1_t{448, 288} // {int16_t(newSize.x / 5.0f * 3), int16_t(newSize.y / 2.0f * 1)};
		: ::gpk::n2s1_t{384, 200} // {int16_t(newSize.x / 5.0f * 3), int16_t(newSize.y / 2.0f * 1)};
		;

	gui.Controls.SetUpdated(indexDialog, false);
	return 0;
}

static	::gpk::error_t				modalDisplayScanSpool			(::dry::SApplication & app, ::gpk::view_array<const ::dry::SSpoolData> scanned) {
	::gpk::SGUI								& gui							= *app.Framework.GUI;
	const ::dry::SScreenModal				& screen						= app.Screens.Modal;
	gui.Controls.SetUpdated(screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_DIALOG], false);
	gui.Controls.SetHidden (screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_DIALOG], false);
	gui.Controls.SetUpdated(app.Screens.SpoolInfo.ControlIds[::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG], false);
	gui.Controls.SetHidden (app.Screens.SpoolInfo.ControlIds[::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG], false);
	const bool								isLoaded						= app.Catalog.Exists(scanned[0].UniqueId);
	gui.Controls.SetUpdated(screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_LOAD  ], isLoaded);
	gui.Controls.SetHidden (screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_LOAD  ], isLoaded);
	gui.Controls.SetUpdated(screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_CANCEL], false);
	gui.Controls.SetHidden (screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_CANCEL], false);
	gui.Controls.SetUpdated(screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_UNLOAD], false == isLoaded);
	gui.Controls.SetHidden (screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_UNLOAD], false == isLoaded);
	if(0 == scanned.size())
		return 0;
	const ::dry::SSpoolData					& lastScanned					= scanned[scanned.size() - 1];
	::gpk::controlTextSet(gui, app.Screens.SpoolInfo.ControlIds[::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_BRAND	], lastScanned.Brand		);
	::gpk::controlTextSet(gui, app.Screens.SpoolInfo.ControlIds[::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_MATERIAL	], lastScanned.Material	);
	::gpk::controlTextSet(gui, app.Screens.SpoolInfo.ControlIds[::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_COLOR	], lastScanned.Color		);
	::gpk::controlTextSet(gui, app.Screens.SpoolInfo.ControlIds[::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_NOTES	], lastScanned.Notes		);
	char									density[64]						= {};
	snprintf(density, ::gpk::size(density), "%.2fg/cm\xfd"	, lastScanned.Density	);	::gpk::controlTextSet(gui, app.Screens.SpoolInfo.ControlIds[::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_DENSITY		], ::gpk::label(density));
	snprintf(density, ::gpk::size(density), "%.2fmm"		, lastScanned.Diameter	);	::gpk::controlTextSet(gui, app.Screens.SpoolInfo.ControlIds[::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_DIAMETER		], ::gpk::label(density));
	snprintf(density, ::gpk::size(density), "%ig"			, lastScanned.SpoolSize	);	::gpk::controlTextSet(gui, app.Screens.SpoolInfo.ControlIds[::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_SPOOL_SIZE	], ::gpk::label(density));
	return 0;
}

static	::gpk::error_t				modalDisplayDryingStop			(::dry::SApplication & app) {
	::gpk::SGUI								& gui							= *app.Framework.GUI;
	const ::dry::SScreenModal				& screen						= app.Screens.Modal;
	::gpk::controlTextSet(gui, screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_DIALOG], "Stop Drying?");
	const ::gpk::cid_t controls[] =
		{ screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_DIALOG]
		, screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_YES]
		, screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_NO]
		, screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_ICON_STOP]
		};
	for(const ::gpk::cid_t idControl : controls) {
		gui.Controls.SetHidden(idControl, false);
		gui.Controls.SetUpdated(idControl, false);
	}
	return 0;
}
static	::gpk::error_t				modalDisplayDryingCompleted		(::dry::SApplication & app) {
	::gpk::SGUI								& gui							= *app.Framework.GUI;
	const ::dry::SScreenModal				& screen						= app.Screens.Modal;
	::gpk::controlTextSet(gui, screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_DIALOG], "Drying completed successfully!");
	const ::gpk::cid_t controls[] =
		{ screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_DIALOG]
		, screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_CLOSE]
		};
	for(const ::gpk::cid_t idControl : controls) {
		gui.Controls.SetHidden(idControl, false);
		gui.Controls.SetUpdated(idControl, false);
	}
	return 0;
}
static	::gpk::error_t				modalDisplayCloseDoor			(::dry::SApplication & app) {
	::gpk::SGUI								& gui							= *app.Framework.GUI;
	const ::dry::SScreenModal				& screen						= app.Screens.Modal;
	::gpk::controlTextSet(gui, screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_DIALOG], "Close the door correctly to start drying");
	const ::gpk::cid_t controls[] =
		{ screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_DIALOG]
		, screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_CLOSE]
		, screen.ControlIds[::dry::SCREEN_CONTROL_MODAL_ICON_START]
		};
	for(const ::gpk::cid_t idControl : controls) {
		gui.Controls.SetHidden(idControl, false);
		gui.Controls.SetUpdated(idControl, false);
	}
	return 0;
}
::gpk::error_t						dry::modalDisplay				(::dry::SApplication & app, ::dry::MODAL modal) {
	::dry::modalClose(app);
	::gpk::SGUI								& gui							= *app.Framework.GUI;
	::gpk::vcid							children						= gui.Controls.Children[app.Screens.Modal.ControlIds[::dry::SCREEN_CONTROL_MODAL_DIALOG]];
	for(uint32_t iChild = 0; iChild < children.size(); ++iChild) {
		gui.Controls.SetHidden(children[iChild], true);
	}
	switch(modal) {
	case ::dry::MODAL_CLOSE_DOOR		: ::modalDisplayCloseDoor		(app); app.Screens.ModalEnabled = true; app.Screens.ModalSelected = modal; ::dry::handleResizeScreenModal(app, gui.LastSize.s1_t()); break;
	case ::dry::MODAL_DRYING_COMPLETE	: ::modalDisplayDryingCompleted	(app); app.Screens.ModalEnabled = true; app.Screens.ModalSelected = modal; ::dry::handleResizeScreenModal(app, gui.LastSize.s1_t()); break;
	case ::dry::MODAL_DRYING_STOP		: ::modalDisplayDryingStop		(app); app.Screens.ModalEnabled = true; app.Screens.ModalSelected = modal; ::dry::handleResizeScreenModal(app, gui.LastSize.s1_t()); break;
	case ::dry::MODAL_SCAN_SPOOL		:
		::modalDisplayScanSpool(app, app.Scanned);
		app.Screens.ModalEnabled			= true;
		app.Screens.ModalSelected			= modal;
		::dry::handleResizeScreenModal(app, gui.LastSize.s1_t());
		::dry::handleResizeScreenSpoolInfo(app, gui.LastSize.s1_t());
		break;
	}
	::gpk::guiUpdateMetrics(gui, gui.LastSize, false);
	return 0;
}
