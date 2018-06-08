#include "gpk_gui_control.h"


			::gpk::error_t												gpk::controlListInitialize	(::gpk::SGUI& gui, ::gpk::SControlList& menu)													{ gpk_necall(menu.IdControl = ::gpk::controlCreate(gui), "Failed to create menu control!"); return 0; }
			::gpk::error_t												gpk::controlListPush		(::gpk::SGUI& gui, ::gpk::SControlList& menu, const ::gpk::view_const_string& text)				{
	if(menu.IdControl == -1) 
		gpk_necall(::gpk::controlListInitialize(gui, menu), "");

	const int32_t																idControl					= ::gpk::controlCreate(gui);
	gpk_necall(idControl, "Failed to create control! Out of memory?");
	::gpk::controlSetParent(gui, idControl, menu.IdControl);
	::gpk::SControl																& control					= gui.Controls.Controls		[idControl];
	::gpk::SControlText															& controlText				= gui.Controls.Text			[idControl];
	::gpk::SControlConstraints													& controlConstraints		= gui.Controls.Constraints	[idControl];
	//control				.Align												= ::gpk::ALIGN_TOP_RIGHT;
	control				.Margin												= {4, 4, 4, 4};
	controlText			.Text												= {text.begin(), text.size()};
	controlText			.Align												= ::gpk::ALIGN_CENTER_LEFT;
	controlConstraints	.AttachSizeToText.y									= true; //menu.Orientation == ::gme::MENU_ORIENTATION_VERTICAL	;
	controlConstraints	.AttachSizeToText.x									= true; //menu.Orientation == ::gme::MENU_ORIENTATION_HORIZONTAL;
	switch(menu.Orientation) {	
	default:
	case ::gpk::CONTROL_LIST_DIRECTION_HORIZONTAL	: controlConstraints.AttachSizeToControl.y = menu.IdControl; if(menu.IdControls.size()) controlConstraints.DockToControl.x = menu.IdControls[menu.IdControls.size() - 1]; break;
	case ::gpk::CONTROL_LIST_DIRECTION_VERTICAL		: controlConstraints.AttachSizeToControl.x = menu.IdControl; if(menu.IdControls.size()) controlConstraints.DockToControl.y = menu.IdControls[menu.IdControls.size() - 1]; break;
	}
	::gpk::controlUpdateMetrics(gui, idControl, gui.LastSize);

	switch(menu.Orientation) {	
	default:
	case ::gpk::CONTROL_LIST_DIRECTION_HORIZONTAL	: gui.Controls.Controls[menu.IdControl].Area.Size.x					+= gui.Controls.Metrics[idControl].Total.Global.Size.x; break;
	case ::gpk::CONTROL_LIST_DIRECTION_VERTICAL		: gui.Controls.Controls[menu.IdControl].Area.Size.y					+= gui.Controls.Metrics[idControl].Total.Global.Size.y; break;
	}
	

	return menu.IdControls.push_back(idControl);			
}