#include "application.h"
#include "gpk_bitmap_file.h"
#include "gpk_encoding.h"
#include "gpk_label.h"
#include "gpk_grid_copy.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");


::gpk::error_t													setup					(::gme::SApplication & app)						{ 
	::gpk::SFramework													& framework				= app.Framework;
	::gpk::SDisplay														& mainWindow			= framework.MainDisplay;
	error_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?????!?!?!?!?");
	::gpk::SGUI															& gui					= framework.GUI;
	const int32_t iShades = 16;
	gui.ThemeDefault												= 18 * iShades + 14;
	gui.ColorModeDefault											= ::gpk::GUI_COLOR_MODE_3D;
	int32_t																controlTestRoot			= ::gpk::controlCreate(gui);
	::gpk::SControl														& controlRoot			= gui.Controls.Controls[controlTestRoot];
	controlRoot.Area												= {{0, 0}, {320, 240}};
	controlRoot.Border												= {4, 4, 4, 4};
	controlRoot.Margin												= {20, 20, 20, 10};
	controlRoot.Align												= ::gpk::ALIGN_CENTER					;
	gui.Controls.Constraints[controlTestRoot].AttachSizeToControl	= {controlTestRoot, controlTestRoot};
	//gui.Controls.Modes	[controlTestRoot].Design				= true;
	::gpk::controlSetParent(gui, controlTestRoot, -1);

	for(uint32_t iChild = 0; iChild < 90; ++iChild) {
		int32_t															controlTestChild0		= ::gpk::controlCreate(gui);
		::gpk::SControl													& control				= gui.Controls.Controls	[controlTestChild0];
		::gpk::SControlText												& controlText			= gui.Controls.Text		[controlTestChild0];
		control		.Area											= {{0, 0}, {(int16_t)(800 / 3 / (1 + iChild / 9)), (int16_t)(600 / 3 / (1 + iChild / 9))}}; // {32, 32}};//
		//control		.Border											= {iChild % 5, iChild % 7, iChild % 11, iChild % 13};
		control		.Border											= {2, 2, 2, 2};
		control		.Margin											= {1, 1, 1, 1};

		char															buffer [1024]				= {};
		const int32_t													lenText						= (int32_t)sprintf_s(buffer, "(%u)", controlTestChild0);
		controlText	.Text											= {::gpk::label(buffer).begin(), (uint32_t)lenText}; 
		//= {0 == (iChild % 4), 0 == (iChild % 5)};
		gui.Controls.States	[controlTestChild0].Disabled			= 0 == (iChild % 9);
		gui.Controls.States	[controlTestChild0].Design				= iChild % 2;
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
		::gpk::controlSetParent(gui, controlTestChild0, iChild / 9);
		if((iChild / 9) == 5)
			gui.Controls.Constraints[controlTestChild0].AttachSizeToText	= {true, true};
	}
	{
		app.IdExit													= ::gpk::controlCreate(gui);
		::gpk::SControl													& controlExit			= gui.Controls.Controls[app.IdExit];
		controlExit.Align											= ::gpk::ALIGN_BOTTOM_RIGHT				;
		::gpk::SControlText												& controlText			= gui.Controls.Text[app.IdExit];
		controlText.Text											= "Exit";
		::gpk::controlSetParent(gui, app.IdExit, 9);
	}
	{
		app.IdTheme													= ::gpk::controlCreate(gui);
		::gpk::SControl													& controlExit			= gui.Controls.Controls[app.IdTheme];
		controlExit.Align											= ::gpk::ALIGN_CENTER_BOTTOM			;
		::gpk::SControlText												& controlText			= gui.Controls.Text[app.IdTheme];
		controlText.Text											= "Theme";
		::gpk::controlSetParent(gui, app.IdTheme, 8);
	}
	{
		app.IdMode													= ::gpk::controlCreate(gui);
		::gpk::SControl													& controlExit			= gui.Controls.Controls[app.IdMode];
		controlExit.Align											= ::gpk::ALIGN_BOTTOM_LEFT				;
		::gpk::SControlText												& controlText			= gui.Controls.Text[app.IdMode];
		controlText.Text											= "Mode";
		::gpk::controlSetParent(gui, app.IdMode, 7);
	}
	for(uint32_t iButton = app.IdExit; iButton < gui.Controls.Controls.size(); ++iButton) {
		::gpk::SControl													& control				= gui.Controls.Controls[iButton];
		::gpk::SControlText												& controlText			= gui.Controls.Text[iButton];
		control.Area												= {{0, 0}, {64, 20}};
		control.Border												= {1, 1, 1, 1};
		control.Margin												= {1, 1, 1, 1};
		controlText.Align											= ::gpk::ALIGN_CENTER;
	}
	
	const int32_t iShadesHalf	= iShades / 2;
	for(uint32_t iColor = 0; iColor < gui.Palette.size(); ++iColor) {
		const int32_t													idPaletteItem			= ::gpk::controlCreate(gui);
		::gpk::SControl													& control				= gui.Controls.Controls	[idPaletteItem];
		control.Align												= ::gpk::ALIGN_TOP_RIGHT;
		control.Area												= {{(int16_t)(256 - (iColor % iShades) * 16), (int16_t)((iColor / iShades) * 16)}, {16, 16}};
		//control.Area.Offset											+= {mainWi, 0};
		//control.Area.Offset											-= {iShades * iShadesHalf - 8, iShades * iShadesHalf - 8};
		control.Border												= 
		control.Margin												= {1, 1, 1, 1};
		control.ColorTheme											= iColor + 1;
		::gpk::controlSetParent(gui, idPaletteItem, 0);
	}

	//char															bmpFileName2	[]							= "Codepage-437-24.bmp";
	//error_if(errored(::gpk::bmpOrBmgLoad(bmpFileName2, app.TextureFont)), "");
	//::gpk::SImage<::gpk::SColorBGRA>								& verticalAtlas								= app.VerticalAtlas;
	//const ::gpk::SCoord2<uint32_t>									fontCharSize								= {9, 16};
	//verticalAtlas.resize(fontCharSize.x, fontCharSize.y * 256);
	//for(uint32_t iChar = 0; iChar < 256; ++iChar) {
	//	const uint32_t													srcOffsetY									= iChar / 32 * fontCharSize.y;
	//	const uint32_t													dstOffsetY									= iChar * fontCharSize.y;
	//	for(uint32_t y = 0; y < fontCharSize.y; ++y) {
	//		for(uint32_t x = 0; x < fontCharSize.x; ++x) {
	//			const uint32_t												srcOffsetX									= iChar % 32 * fontCharSize.x;
	//			const uint32_t												dstOffsetX									= 0;
	//			const ::gpk::SColorBGRA										& srcColor									= app.TextureFont.View[srcOffsetY + y][srcOffsetX + x];
	//			verticalAtlas.View[dstOffsetY + y][dstOffsetX + x]		= srcColor;
	//		}
	//	}
	//}
	//const ::gpk::SCoord2<uint32_t>									& textureFontMetrics						= verticalAtlas.View.metrics();// app.TextureFont.View.metrics();
	//gpk_necall(gui.FontTexture.resize(textureFontMetrics), "Whou would we failt ro resize=");
	//for(uint32_t y = 0, yMax = textureFontMetrics.y; y < yMax; ++y)
	//for(uint32_t x = 0, xMax = textureFontMetrics.x; x < xMax; ++x) {
	//	const ::gpk::SColorBGRA											& srcColor									= verticalAtlas.View[y][x];//app.TextureFont.View[y][x];
	//	gui.FontTexture.View[y * textureFontMetrics.x + x]	
	//		=	0 != srcColor.r
	//		||	0 != srcColor.g
	//		||	0 != srcColor.b
	//		;
	//}

	//::gpk::array_pod<char_t>	encoded;
	//::gpk::base64Encode({(const ubyte_t*)gui.FontTexture.Texels.begin(), gui.FontTexture.Texels.size() * 4}, encoded);
	//FILE						* fp = 0;
	//const int nul = 0;
	//fopen_s(&fp, "codepage_437_encoded.txt", "wb");
	//fwrite(encoded.begin(), 1, encoded.size(), fp);
	//fwrite(&nul, 1, 1, fp);
	//fclose(fp);


	return 0; 
}

::gpk::error_t												update					(::gme::SApplication & app, bool exitSignal)	{ 
	//::gpk::STimer													timer;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "Exit requested by runtime.");
	{
		::gme::mutex_guard												lock					(app.LockRender);
		app.Framework.MainDisplayOffscreen							= app.Offscreen;
	}
	::gpk::SFramework												& framework				= app.Framework;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(framework), "Exit requested by framework update.");
	::gpk::SGUI														& gui					= framework.GUI;

	{
		::gme::mutex_guard												lock					(app.LockGUI);
		::gpk::guiProcessInput(gui, *app.Framework.Input);
		for(uint32_t iControl = 0, countControls = gui.Controls.Controls.size(); iControl < countControls; ++iControl) {
			if(gui.Controls.States[iControl].Unused || gui.Controls.States[iControl].Disabled)
				continue;
			if(gui.Controls.States[iControl].Execute) {
				info_printf("Executed %u.", iControl);
				if(iControl == (uint32_t)app.IdExit)
					return 1;
				else if(iControl == (uint32_t)app.IdMode) {
					gui.Controls.Modes[5		].ColorMode					= gui.Controls.Modes[5].ColorMode == ::gpk::GUI_COLOR_MODE_THEME ? ::gpk::GUI_COLOR_MODE_3D : ::gpk::GUI_COLOR_MODE_THEME; 
					gui.Controls.Modes[iControl	].ColorMode					= gui.Controls.Modes[5].ColorMode;
					for(uint32_t iChild = 0; iChild < gui.Controls.Children[5].size(); ++iChild) 
						gui.Controls.Modes[gui.Controls.Children[5][iChild]].ColorMode = gui.Controls.Modes[5].ColorMode;
				}
				else if(iControl == (uint32_t)app.IdTheme) {
					++gui.ThemeDefault; 
					if(gui.ThemeDefault >= gui.ControlThemes.size())
						gui.ThemeDefault										= 0;
				}
				else if(iControl > (uint32_t)app.IdMode) {
					gui.Controls.Controls[5].ColorTheme						= iControl - app.IdMode; 
					for(uint32_t iChild = 0; iChild < gui.Controls.Children[5].size(); ++iChild) 
						gui.Controls.Controls[gui.Controls.Children[5][iChild]].ColorTheme = gui.Controls.Controls[5].ColorTheme;
				}
			}
		}
		if(app.Framework.Input->MouseCurrent.Deltas.z) {
			gui.Zoom.ZoomLevel										+= app.Framework.Input->MouseCurrent.Deltas.z * (1.0 / (120 * 4ULL));
			::gpk::guiUpdateMetrics(gui, framework.MainDisplay.Size, true);
		}
	}
	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0; 
}

::gpk::error_t													cleanup					(::gme::SApplication & app)						{ return ::gpk::mainWindowDestroy(app.Framework.MainDisplay); }
::gpk::error_t													draw					(::gme::SApplication & app)						{ 
	::gpk::STimer														timer;
	app;
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	target;
	target.create();
	target->Color		.resize(app.Framework.MainDisplay.Size);
	target->DepthStencil.resize(app.Framework.MainDisplay.Size);
	//::gpk::clearTarget(*target);
	{
		::gme::mutex_guard												lock					(app.LockGUI);
		::gpk::controlDrawHierarchy(app.Framework.GUI, 0, target->Color.View);
		::gpk::grid_copy(target->Color.View, app.VerticalAtlas.View);
	}
	{
		::gme::mutex_guard												lock					(app.LockRender);
		app.Offscreen												= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0; 
}