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
	error_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window. %s", " why?????!?!?!?!?");
	::gpk::SGUI															& gui					= *framework.GUI;

	const int32_t														iShades					= 16;
	gui.ThemeDefault												= app.PaletteColumn * iShades + app.PaletteRow;
	gui.ColorModeDefault											= ::gpk::GUI_COLOR_MODE_3D;
	int32_t																controlTestRoot			= ::gpk::controlCreate(gui);
	const ::gpk::SColorBGRA												colorBase				= gui.Palette[gui.ThemeDefault];
	::gpk::memcpy_s(app.Palettes, gui.DefaultColors);
	app.Palettes[::gpk::GUI_CONTROL_PALETTE_NORMAL				]	= gui.Palettes.push_back({colorBase, {}, {}, {}, {}, {}, ::gpk::RED, {}, {}, {}, });// gui.DefaultColors.CONTROL_NORMAL				;
	app.Palettes[::gpk::GUI_CONTROL_PALETTE_HOVER				]	= gui.Palettes.push_back({colorBase, {}, {}, {}, {}, {}, ::gpk::RED, {}, {}, {}, });// gui.DefaultColors.CONTROL_HOVER				;
	app.Palettes[::gpk::GUI_CONTROL_PALETTE_PRESSED				]	= gui.Palettes.push_back({colorBase, {}, {}, {}, {}, {}, ::gpk::RED, {}, {}, {}, });// gui.DefaultColors.CONTROL_PRESSED			;
	app.Palettes[::gpk::GUI_CONTROL_PALETTE_SELECTED			]	= gui.Palettes.push_back({colorBase, {}, {}, {}, {}, {}, ::gpk::RED, {}, {}, {}, });// gui.DefaultColors.CONTROL_SELECTED			;
	app.Palettes[::gpk::GUI_CONTROL_PALETTE_SELECTED_HOVER		]	= gui.Palettes.push_back({colorBase, {}, {}, {}, {}, {}, ::gpk::RED, {}, {}, {}, });// gui.DefaultColors.CONTROL_SELECTED_HOVER		;
	app.Palettes[::gpk::GUI_CONTROL_PALETTE_SELECTED_PRESSED	]	= gui.Palettes.push_back({colorBase, {}, {}, {}, {}, {}, ::gpk::RED, {}, {}, {}, });// gui.DefaultColors.CONTROL_SELECTED_PRESSED	;
	app.Palettes[::gpk::GUI_CONTROL_PALETTE_OUTDATED			]	= gui.Palettes.push_back({colorBase, {}, {}, {}, {}, {}, ::gpk::RED, {}, {}, {}, });// gui.DefaultColors.CONTROL_OUTDATED			;

	::gpk::SControl														& controlRoot			= gui.Controls.Controls[controlTestRoot];
	controlRoot.Area												= {{0, 0}, {320, 240}};
	controlRoot.Border												= {4, 4, 4, 4};
	controlRoot.Margin												= {20, 20, 20, 10};
	controlRoot.Align												= ::gpk::ALIGN_CENTER					;
	//gui.Controls.Modes[controlTestRoot].UseNewPalettes				= 1;
	memcpy(controlRoot.Palettes, app.Palettes, sizeof(app.Palettes));

	gui.Controls.Constraints[controlTestRoot].AttachSizeToControl	= {controlTestRoot, controlTestRoot};
	//gui.Controls.Modes	[controlTestRoot].Design				= true;
	::gpk::controlSetParent(gui, controlTestRoot, -1);

	for(uint32_t iChild = 0; iChild < 90; ++iChild) {
		int32_t															controlTestChild0		= ::gpk::controlCreate(gui);
		::gpk::SControl													& control				= gui.Controls.Controls	[controlTestChild0];
		::gpk::SControlText												& controlText			= gui.Controls.Text		[controlTestChild0];
		//gui.Controls.Modes[controlTestChild0].UseNewPalettes				= 1;
		memcpy(control.Palettes, app.Palettes, sizeof(app.Palettes));


		control		.Area											= {{0, 0}, {(int16_t)(800 / 3 / (1 + iChild / 9)), (int16_t)(600 / 3 / (1 + iChild / 9))}}; // {32, 32}};//
		//control		.Border											= {iChild % 5, iChild % 7, iChild % 11, iChild % 13};
		control		.Border											= {2, 2, 2, 2};
		control		.Margin											= {1, 1, 1, 1};

		char															buffer [1024]				= {};
		const int32_t													lenText						= (int32_t)sprintf_s(buffer, "(%u)", controlTestChild0);
		controlText	.Text											= {::gpk::label(buffer).begin(), (uint32_t)lenText};
		//= {0 == (iChild % 4), 0 == (iChild % 5)};
		gui.Controls.States	[controlTestChild0].Disabled			= 0 == (iChild % 9);
		gui.Controls.Modes	[controlTestChild0].Design				= iChild % 2;
		switch(iChild % 9) {
		case 0: control.Align = ::gpk::ALIGN_TOP_LEFT		; controlText.Align = ::gpk::ALIGN_BOTTOM_RIGHT		; break;
		case 1: control.Align = ::gpk::ALIGN_CENTER_TOP		; controlText.Align = ::gpk::ALIGN_CENTER_BOTTOM	; break;
		case 2: control.Align = ::gpk::ALIGN_TOP_RIGHT		; controlText.Align = ::gpk::ALIGN_BOTTOM_LEFT		; break;
		case 3: control.Align = ::gpk::ALIGN_CENTER_LEFT	; controlText.Align = ::gpk::ALIGN_CENTER_RIGHT		; break;
		case 4: control.Align = ::gpk::ALIGN_CENTER			; controlText.Align = ::gpk::ALIGN_CENTER			; break;
		case 5: control.Align = ::gpk::ALIGN_CENTER_RIGHT	; controlText.Align = ::gpk::ALIGN_CENTER_LEFT		; break;
		case 6: control.Align = ::gpk::ALIGN_BOTTOM_LEFT	; controlText.Align = ::gpk::ALIGN_TOP_RIGHT		; break;
		case 7: control.Align = ::gpk::ALIGN_CENTER_BOTTOM	; controlText.Align = ::gpk::ALIGN_CENTER_TOP		; break;
		case 8: control.Align = ::gpk::ALIGN_BOTTOM_RIGHT	; controlText.Align = ::gpk::ALIGN_TOP_LEFT			; break;
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
		controlText.Text											= "3D Mode";
		::gpk::controlSetParent(gui, app.IdMode, 7);
	}
	{
		app.IdNewPalette											= ::gpk::controlCreate(gui);
		::gpk::SControl													& controlExit			= gui.Controls.Controls[app.IdNewPalette];
		controlExit.Align											= ::gpk::ALIGN_CENTER_BOTTOM;
		::gpk::SControlText												& controlText			= gui.Controls.Text[app.IdNewPalette];
		controlText.Text											= "Palette Mode";
		::gpk::controlSetParent(gui, app.IdNewPalette, 2);
	}
	for(uint32_t iButton = app.IdExit; iButton < gui.Controls.Controls.size(); ++iButton) {
		::gpk::SControl													& control				= gui.Controls.Controls[iButton];
		::gpk::SControlText												& controlText			= gui.Controls.Text[iButton];
		control.Area												= {{0, 0}, {64, 20}};
		control.Border												= {1, 1, 1, 1};
		control.Margin												= {1, 1, 1, 1};
		controlText.Align											= ::gpk::ALIGN_CENTER;
		memcpy(control.Palettes, app.Palettes, sizeof(app.Palettes));
	}

	gui.Controls.Constraints[app.IdMode			].AttachSizeToText.x	= true;
	gui.Controls.Constraints[app.IdNewPalette	].AttachSizeToText.x	= true;

	const int32_t													iShadesHalf				= iShades / 2;
	for(uint32_t iTone = 0, countTones = gui.Palette.size() / iShades; iTone < countTones; ++iTone) {
		for(uint32_t iShade = 0; iShade < iShades; ++iShade) {
			const int32_t													idPaletteItem			= ::gpk::controlCreate(gui);
			::gpk::SControl													& control				= gui.Controls.Controls	[idPaletteItem];
			::gpk::SControlText												& controlText			= gui.Controls.Text[idPaletteItem];
			control.Align												= ::gpk::ALIGN_TOP_RIGHT;
			control.Area												= {{(int16_t)(256 - iShade * 16), (int16_t)(iTone * 16)}, {16, 16}};
			//control.Area.Offset											+= {mainWi, 0};
			//control.Area.Offset											-= {iShades * iShadesHalf - 8, iShades * iShadesHalf - 8};
			control.Border												=
			control.Margin												= {1, 1, 1, 1};
			control.ColorTheme											= iTone * iShades + iShade + 1;
			controlText.Text											= ".";
			::gpk::controlSetParent(gui, idPaletteItem, 0);
		}
	}
	for(uint32_t iShade = 0; iShade < iShades; ++iShade) {
		const int32_t													idPaletteRow		= ::gpk::controlCreate(gui);
		::gpk::SControl													& controlRow		= gui.Controls.Controls	[idPaletteRow];
		::gpk::SControlText												& controlRowText	= gui.Controls.Text[idPaletteRow];
		controlRow.Align											= ::gpk::ALIGN_TOP_RIGHT;
		controlRow.Area												= {{(int16_t)(256 - iShade * 16), -16}, {16, 16}};
		controlRow.Border											=
		controlRow.Margin											= {1, 1, 1, 1};
		controlRow.ColorTheme										= iShade + 1;
		controlRowText.Text											= app.RowText[iShade];
		//gui.Controls.Constraints[idPaletteRow].AttachSizeToText.x	= true;
		::gpk::controlSetParent(gui, idPaletteRow, 0);
	}
	for(uint32_t iTone = 0, countTones = gui.Palette.size() / iShades; iTone < countTones; ++iTone) {
		const int32_t													idPaletteRow		= ::gpk::controlCreate(gui);
		::gpk::SControl													& controlRow		= gui.Controls.Controls	[idPaletteRow];
		::gpk::SControlText												& controlRowText	= gui.Controls.Text[idPaletteRow];
		controlRow.Align												= ::gpk::ALIGN_TOP_RIGHT;
		controlRow.Area												= {{(int16_t)256+16, (int16_t)(iTone * 16)}, {16, 16}};
		//control.Area.Offset										+= {mainWi, 0};
		//control.Area.Offset										-= {iShades * iShadesHalf - 8, iShades * iShadesHalf - 8};
		controlRow.Border											=
		controlRow.Margin											= {1, 1, 1, 1};
		controlRow.ColorTheme										= iTone * iShades + 0 + 1;
		controlRowText.Text											= app.RowText[iTone];
		gui.Controls.Constraints[idPaletteRow].AttachSizeToText.x	= true;
		::gpk::controlSetParent(gui, idPaletteRow, 0);
	}

	//for(uint32_t iColor = 0; iColor < gui.Palette.size(); ++iColor) {
	//	const int32_t													idPaletteItem			= ::gpk::controlCreate(gui);
	//	::gpk::SControl													& control				= gui.Controls.Controls	[idPaletteItem];
	//	::gpk::SControlText												& controlText			= gui.Controls.Text[idPaletteItem];
	//	control.Align												= ::gpk::ALIGN_TOP_RIGHT;
	//	control.Area												= {{(int16_t)(256 - (iColor % iShades) * 16), (int16_t)((iColor / iShades) * 16)}, {16, 16}};
	//	//control.Area.Offset											+= {mainWi, 0};
	//	//control.Area.Offset											-= {iShades * iShadesHalf - 8, iShades * iShadesHalf - 8};
	//	control.Border												=
	//	control.Margin												= {1, 1, 1, 1};
	//	control.ColorTheme											= iColor + 1;
	//	controlText.Text											= ".";
	//	::gpk::controlSetParent(gui, idPaletteItem, 0);
	//}

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

::gpk::error_t														update					(::gme::SApplication & app, bool exitSignal)	{
	//::gpk::STimer															timer;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "%s", "Exit requested by runtime.");
	{
		::gpk::mutex_guard														lock					(app.LockRender);
		app.Framework.MainDisplayOffscreen									= app.Offscreen;
	}
	::gpk::SFramework														& framework				= app.Framework;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(framework), "%s", "Exit requested by framework update.");
	{
		::gpk::SGUI																& gui					= *framework.GUI;
		::gpk::mutex_guard														lock					(app.LockGUI);
		::gpk::array_pod<uint32_t>												controlsToProcess			= {};
		::gpk::guiGetProcessableControls(gui, controlsToProcess);
		for(uint32_t iProcessable = 0, countControls = controlsToProcess.size(); iProcessable < countControls; ++iProcessable) {
			uint32_t																iControl					= controlsToProcess[iProcessable];
			if(gui.Controls.States[iControl].Execute) {
				info_printf("Executed %u.", iControl);
				if(iControl == (uint32_t)app.IdExit)
					return 1;
				else if(iControl == (uint32_t)app.IdMode) {
					for(uint32_t iChild = 0; iChild < gui.Controls.Controls.size(); ++iChild)
						gui.Controls.Modes[iChild].ColorMode							= gui.Controls.Modes[iChild].ColorMode == ::gpk::GUI_COLOR_MODE_FLAT ? ::gpk::GUI_COLOR_MODE_3D : ::gpk::GUI_COLOR_MODE_FLAT;
				}
				else if(iControl == (uint32_t)app.IdTheme) {
					++gui.ThemeDefault;
					if(gui.ThemeDefault >= gui.ControlThemes.size())
						gui.ThemeDefault												= 0;
				}
				else if(iControl == (uint32_t)app.IdNewPalette) {
					for(uint32_t iChild = 0; iChild < gui.Controls.Controls.size(); ++iChild)
						gui.Controls.Modes[iChild].UseNewPalettes						= gui.Controls.Modes[iChild].UseNewPalettes ? 0 : 1;
				}
				else if(iControl > (uint32_t)app.IdMode) {
					gui.Controls.Controls[5].ColorTheme								= iControl - app.IdNewPalette;
					if(gui.Controls.Controls[5].ColorTheme >= (int32_t)gui.Palette.size())
						gui.Controls.Controls[5].ColorTheme								= 10;
					for(uint32_t iChild = 0; iChild < gui.Controls.Children[5].size(); ++iChild)
						gui.Controls.Controls[gui.Controls.Children[5][iChild]].ColorTheme = gui.Controls.Controls[5].ColorTheme;
				}
			}
		}
	}
	::gpk::sleep(1);
	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}

::gpk::error_t														cleanup					(::gme::SApplication & app)						{ return ::gpk::mainWindowDestroy(app.Framework.MainDisplay); }
::gpk::error_t														draw					(::gme::SApplication & app)						{
	::gpk::STimer															timer;
	app;
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>		target;
	target.create();
	target->resize(app.Framework.MainDisplay.Size, {0xFF, 0x40, 0x7F, 0xFF}, (uint32_t)-1);
	//::gpk::clearTarget(*target);
	{
		::gpk::mutex_guard													lock					(app.LockGUI);
		::gpk::controlDrawHierarchy(*app.Framework.GUI, 0, target->Color.View);
		::gpk::grid_copy(target->Color.View, app.VerticalAtlas.View);
	}
	{
		::gpk::mutex_guard													lock					(app.LockRender);
		app.Offscreen													= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}
