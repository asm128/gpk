#include "application.h"

#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

enum GUI_CONTROL_COLOR 
	{ GUI_CONTROL_COLOR_BACKGROUND		= 0
	, GUI_CONTROL_COLOR_CLIENT
	, GUI_CONTROL_COLOR_BORDER_LEFT
	, GUI_CONTROL_COLOR_BORDER_TOP
	, GUI_CONTROL_COLOR_BORDER_RIGHT
	, GUI_CONTROL_COLOR_BORDER_BOTTOM
	, GUI_CONTROL_COLOR_COUNT
	};

::gpk::error_t									setup					(::gme::SApplication & app)						{ 
	::gpk::SFramework									& framework				= app.Framework;
	::gpk::SDisplay										& mainWindow			= framework.MainDisplay;
	framework.Input.create();
	error_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?????!?!?!?!?");
	app.GUI.Colors.resize(GUI_CONTROL_COLOR_COUNT);
	app.GUI.Colors[GUI_CONTROL_COLOR_BACKGROUND		] = {0xFF, 0xFF, 0xFF, 0xFF};
	app.GUI.Colors[GUI_CONTROL_COLOR_CLIENT			] = {0x7F, 0x7F, 0x7F, 0xFF};
	app.GUI.Colors[GUI_CONTROL_COLOR_BORDER_LEFT	] = {0x00, 0x00, 0xFF, 0xFF};
	app.GUI.Colors[GUI_CONTROL_COLOR_BORDER_TOP		] = {0x00, 0xFF, 0x00, 0xFF};
	app.GUI.Colors[GUI_CONTROL_COLOR_BORDER_RIGHT	] = {0xFF, 0x00, 0x00, 0xFF};
	app.GUI.Colors[GUI_CONTROL_COLOR_BORDER_BOTTOM	] = {0xFF, 0xFF, 0x00, 0xFF};
	int32_t												controlTestRoot			= ::gpk::controlCreate(app.GUI);
	app.GUI.Controls[controlTestRoot].Area			= {{0, 0}, {320, 240}};
	app.GUI.Controls[controlTestRoot].Border		= {0, 0, 0, 0};
	app.GUI.Controls[controlTestRoot].Margin		= {0, 0, 0, 0};
	app.GUI.Controls[controlTestRoot].Align			= ::gpk::ALIGN_CENTER;
	app.GUI.Controls[controlTestRoot].ColorBack						= GUI_CONTROL_COLOR_BACKGROUND		;
	app.GUI.Controls[controlTestRoot].ColorClient					= GUI_CONTROL_COLOR_CLIENT			;
	app.GUI.Controls[controlTestRoot].ColorBorder.Left				= GUI_CONTROL_COLOR_BORDER_LEFT		;
	app.GUI.Controls[controlTestRoot].ColorBorder.Top				= GUI_CONTROL_COLOR_BORDER_TOP		;
	app.GUI.Controls[controlTestRoot].ColorBorder.Right				= GUI_CONTROL_COLOR_BORDER_RIGHT	;
	app.GUI.Controls[controlTestRoot].ColorBorder.Bottom			= GUI_CONTROL_COLOR_BORDER_BOTTOM	;
	::gpk::controlSetParent(app.GUI, controlTestRoot, -1);

	for(uint32_t iChild = 0; iChild < 720; ++iChild) {
		int32_t												controlTestChild0		= ::gpk::controlCreate(app.GUI);
		char												buffer [16]				= {};
		sprintf_s(buffer, "(%u)", iChild);
		::gpk::SControl										& control				= app.GUI.Controls		[controlTestChild0];
		::gpk::SControlText									& controlText			= app.GUI.ControlText	[controlTestChild0];
		control		.Area								= {{0, 0}, {(int32_t)(640 / 3 / (1 + iChild / 9)), (int32_t)(480 / 3 / (1 + iChild / 9))}}; // {32, 32}};//
		control		.Border								= {1, 1, 1, 1};
		control		.Margin								= {2, 2, 2, 2};
		controlText	.Text								= buffer; 
		control.ColorBack								= GUI_CONTROL_COLOR_BACKGROUND		;
		control.ColorClient								= GUI_CONTROL_COLOR_CLIENT			;
		control.ColorBorder.Left						= GUI_CONTROL_COLOR_BORDER_LEFT	;
		control.ColorBorder.Top							= GUI_CONTROL_COLOR_BORDER_TOP		;
		control.ColorBorder.Right						= GUI_CONTROL_COLOR_BORDER_RIGHT	;
		control.ColorBorder.Bottom						= GUI_CONTROL_COLOR_BORDER_BOTTOM	;
		switch(iChild % 9) {							  
		case 0: control.Align = ::gpk::ALIGN_TOP_LEFT			; controlText.Align = ::gpk::ALIGN_BOTTOM_RIGHT		; ;break;
		case 1: control.Align = ::gpk::ALIGN_CENTER_TOP			; controlText.Align = ::gpk::ALIGN_CENTER_BOTTOM	; ;break;
		case 2: control.Align = ::gpk::ALIGN_TOP_RIGHT			; controlText.Align = ::gpk::ALIGN_BOTTOM_LEFT		; ;break;
		case 3: control.Align = ::gpk::ALIGN_CENTER_LEFT		; controlText.Align = ::gpk::ALIGN_CENTER_RIGHT		; ;break;
		case 4: control.Align = ::gpk::ALIGN_CENTER				; controlText.Align = ::gpk::ALIGN_CENTER			; ;break;
		case 5: control.Align = ::gpk::ALIGN_CENTER_RIGHT		; controlText.Align = ::gpk::ALIGN_CENTER_LEFT		; ;break;
		case 6: control.Align = ::gpk::ALIGN_BOTTOM_LEFT		; controlText.Align = ::gpk::ALIGN_TOP_RIGHT		; ;break;
		case 7: control.Align = ::gpk::ALIGN_CENTER_BOTTOM		; controlText.Align = ::gpk::ALIGN_CENTER_TOP		; ;break;
		case 8: control.Align = ::gpk::ALIGN_BOTTOM_RIGHT		; controlText.Align = ::gpk::ALIGN_TOP_LEFT			; ;break;
		default:
			control		.Align								=  
			controlText	.Align								= {};
		}
		::gpk::controlSetParent(app.GUI, controlTestChild0, iChild / 9);
	}
	return 0; 
}

::gpk::error_t									update					(::gme::SApplication & app, bool exitSignal)	{ 
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "Exit requested by runtime.");
	{
		::gme::mutex_guard									lock					(app.LockRender);
		app.Framework.MainDisplayOffscreen				= app.Offscreen;
	}
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "Exit requested by framework update.");

	app.GUI.Controls[0].Area			= {{0, 0}, app.Framework.MainDisplay.Size.Cast<int32_t>()};
	::gpk::guiProcessInput(app.GUI, *app.Framework.Input);
	for(uint32_t iControl = 0, countControls = app.GUI.Controls.size(); iControl < countControls; ++iControl) {
		if(app.GUI.ControlStates[iControl].Unused || app.GUI.ControlStates[iControl].Disabled)
			continue;
		if(app.GUI.ControlStates[iControl].Execute)
			info_printf("Executed %u.", iControl);
	}
	return 0; 
}

::gpk::error_t									cleanup					(::gme::SApplication & app)						{ 
	::gpk::mainWindowDestroy(app.Framework.MainDisplay);
	return 0; 
}

::gpk::error_t									draw					(::gme::SApplication & app)						{ 
	app;
	::gpk::ptr_obj<::gpk::SRenderTarget>				target;
	target.create();
	target->Color		.resize(app.Framework.MainDisplay.Size);
	target->DepthStencil.resize(app.Framework.MainDisplay.Size);
	::gpk::clearTarget(*target);
	::gpk::controlPaintHierarchy(app.GUI, 0, target->Color.View);

	{
		::gme::mutex_guard									lock					(app.LockRender);
		app.Offscreen									= target;
	}
	return 0; 
}