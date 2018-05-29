#include "application.h"
#include "gpk_bitmap_file.h"

#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");


::gpk::error_t												setup					(::gme::SApplication & app)						{ 
	::gpk::SFramework												& framework				= app.Framework;
	::gpk::SDisplay													& mainWindow			= framework.MainDisplay;
	framework.Input.create();
	error_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?????!?!?!?!?");
	app.GUI.Colors.resize(::gpk::GUI_CONTROL_AREA_COUNT);
	app.GUI.Colors[::gpk::GUI_CONTROL_AREA_BACKGROUND		]	= {0x3F, 0x3F, 0x3F, 0xFF};
	app.GUI.Colors[::gpk::GUI_CONTROL_AREA_CLIENT			]	= {0x00, 0x7F, 0x7F, 0xFF};
	app.GUI.Colors[::gpk::GUI_CONTROL_AREA_BORDER_LEFT		]	= {0x00, 0x00, 0xFF, 0xFF};
	app.GUI.Colors[::gpk::GUI_CONTROL_AREA_BORDER_TOP		]	= {0x00, 0xFF, 0x00, 0xFF};
	app.GUI.Colors[::gpk::GUI_CONTROL_AREA_BORDER_RIGHT		]	= {0xFF, 0x00, 0x00, 0xFF};
	app.GUI.Colors[::gpk::GUI_CONTROL_AREA_BORDER_BOTTOM	]	= {0xFF, 0xFF, 0x00, 0xFF};
	int32_t															controlTestRoot			= ::gpk::controlCreate(app.GUI);
	::gpk::SControl													& controlRoot			= app.GUI.Controls[controlTestRoot];
	controlRoot.Area											= {{0, 0}, {320, 240}};
	controlRoot.Border											= {4, 4, 4, 4};
	controlRoot.Margin											= {20, 20, 20, 10};
	controlRoot.Align											= ::gpk::ALIGN_CENTER					;
	controlRoot.ColorBack										= ::gpk::GUI_CONTROL_AREA_BACKGROUND	;
	controlRoot.ColorClient										= ::gpk::GUI_CONTROL_AREA_CLIENT		;
	controlRoot.ColorBorder.Left								= ::gpk::GUI_CONTROL_AREA_BORDER_LEFT	;
	controlRoot.ColorBorder.Top									= ::gpk::GUI_CONTROL_AREA_BORDER_TOP	;
	controlRoot.ColorBorder.Right								= ::gpk::GUI_CONTROL_AREA_BORDER_RIGHT	;
	controlRoot.ColorBorder.Bottom								= ::gpk::GUI_CONTROL_AREA_BORDER_BOTTOM	;
	app.GUI.ControlConstraints[controlTestRoot].IndexControlToAttachHeightTo	= 0;
	app.GUI.ControlConstraints[controlTestRoot].IndexControlToAttachWidthTo		= 0;
	::gpk::controlSetParent(app.GUI, controlTestRoot, -1);

	for(uint32_t iChild = 0; iChild < 90; ++iChild) {
		int32_t															controlTestChild0		= ::gpk::controlCreate(app.GUI);
		char															buffer [16]				= {};
		sprintf_s(buffer, "(%u)", controlTestChild0);
		::gpk::SControl													& control				= app.GUI.Controls		[controlTestChild0];
		::gpk::SControlText												& controlText			= app.GUI.ControlText	[controlTestChild0];
		control		.Area											= {{0, 0}, {(int32_t)(640 / 3 / (1 + iChild / 9)), (int32_t)(320 / 3 / (1 + iChild / 9))}}; // {32, 32}};//
		control		.Border											= {2, 2, 2, 2};
		control		.Margin											= {1, 1, 1, 1};
		controlText	.Text											= buffer; 
		control.ColorBack											= ::gpk::GUI_CONTROL_AREA_BACKGROUND	;
		control.ColorClient											= ::gpk::GUI_CONTROL_AREA_CLIENT		;
		control.ColorBorder.Left									= ::gpk::GUI_CONTROL_AREA_BORDER_LEFT	;
		control.ColorBorder.Top										= ::gpk::GUI_CONTROL_AREA_BORDER_TOP	;
		control.ColorBorder.Right									= ::gpk::GUI_CONTROL_AREA_BORDER_RIGHT	;
		control.ColorBorder.Bottom									= ::gpk::GUI_CONTROL_AREA_BORDER_BOTTOM	;
		switch(iChild % 9) {										  
		case 0: control.Align = ::gpk::ALIGN_TOP_LEFT		; controlText.Align = ::gpk::ALIGN_BOTTOM_RIGHT		; ;break;
		case 1: control.Align = ::gpk::ALIGN_CENTER_TOP		; controlText.Align = ::gpk::ALIGN_CENTER_BOTTOM	; ;break;
		case 2: control.Align = ::gpk::ALIGN_TOP_RIGHT		; controlText.Align = ::gpk::ALIGN_BOTTOM_LEFT		; ;break;
		case 3: control.Align = ::gpk::ALIGN_CENTER_LEFT	; controlText.Align = ::gpk::ALIGN_CENTER_RIGHT		; ;break;
		case 4: control.Align = ::gpk::ALIGN_CENTER			; controlText.Align = ::gpk::ALIGN_CENTER			; ;break;
		case 5: control.Align = ::gpk::ALIGN_CENTER_RIGHT	; controlText.Align = ::gpk::ALIGN_CENTER_LEFT		; ;break;
		case 6: control.Align = ::gpk::ALIGN_BOTTOM_LEFT	; controlText.Align = ::gpk::ALIGN_TOP_RIGHT		; ;break;
		case 7: control.Align = ::gpk::ALIGN_CENTER_BOTTOM	; controlText.Align = ::gpk::ALIGN_CENTER_TOP		; ;break;
		case 8: control.Align = ::gpk::ALIGN_BOTTOM_RIGHT	; controlText.Align = ::gpk::ALIGN_TOP_LEFT			; ;break;
		default:
			control		.Align											=  
			controlText	.Align											= {};
		}
		::gpk::controlSetParent(app.GUI, controlTestChild0, iChild / 9);
	}

	char															bmpFileName2	[]							= "Codepage-437-24.bmp";
	error_if(errored(::gpk::bmpOrBmgLoad(bmpFileName2, app.TextureFont)), "");
	const ::gpk::SCoord2<uint32_t>									& textureFontMetrics						= app.TextureFont.View.metrics();
	gpk_necall(app.GUI.FontTexture.resize(textureFontMetrics), "Whou would we failt ro resize=");
	for(uint32_t y = 0, yMax = textureFontMetrics.y; y < yMax; ++y)
	for(uint32_t x = 0, xMax = textureFontMetrics.x; x < xMax; ++x) {
		const ::gpk::SColorBGRA											& srcColor									= app.TextureFont.View[y][x];
		app.GUI.FontTexture.View[y * textureFontMetrics.x + x]	
			=	0 != srcColor.r
			||	0 != srcColor.g
			||	0 != srcColor.b
			;
	}
	return 0; 
}

::gpk::error_t												update					(::gme::SApplication & app, bool exitSignal)	{ 
	::gpk::STimer													timer;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "Exit requested by runtime.");
	{
		::gme::mutex_guard												lock					(app.LockRender);
		app.Framework.MainDisplayOffscreen							= app.Offscreen;
	}
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "Exit requested by framework update.");

	app.GUI.Controls[0].Area									= {{0, 0}, app.Framework.MainDisplay.Size.Cast<int32_t>()};
	{
		::gme::mutex_guard												lock					(app.LockGUI);
		::gpk::guiProcessInput(app.GUI, *app.Framework.Input);
	}
	if(app.Framework.Input->MouseCurrent.Deltas.z)
		app.GUI.Zoom.ZoomLevel										+= app.Framework.Input->MouseCurrent.Deltas.z * (1.0f / (120 * 4));
 
	for(uint32_t iControl = 0, countControls = app.GUI.Controls.size(); iControl < countControls; ++iControl) {
		if(app.GUI.ControlStates[iControl].Unused || app.GUI.ControlStates[iControl].Disabled)
			continue;
		if(app.GUI.ControlStates[iControl].Execute)
			info_printf("Executed %u.", iControl);
	}
	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0; 
}

::gpk::error_t												cleanup					(::gme::SApplication & app)						{ 
	::gpk::mainWindowDestroy(app.Framework.MainDisplay);
	return 0; 
}

::gpk::error_t												draw					(::gme::SApplication & app)						{ 
	::gpk::STimer													timer;
	app;
	::gpk::ptr_obj<::gpk::SRenderTarget>							target;
	target.create();
	target->Color		.resize(app.Framework.MainDisplay.Size);
	target->DepthStencil.resize(app.Framework.MainDisplay.Size);
	::gpk::clearTarget(*target);
	{
		::gme::mutex_guard												lock					(app.LockGUI);
		::gpk::controlDrawHierarchy(app.GUI, 0, target->Color.View);
	}
	{
		::gme::mutex_guard												lock					(app.LockRender);
		app.Offscreen												= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0; 
}