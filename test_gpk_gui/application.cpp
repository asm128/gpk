#include "application.h"
#include "gpk_bitmap_file.h"
#include "gpk_encoding.h"
#include "gpk_label.h"
#include "gpk_grid_copy.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");


::gpk::err_t			setup		(::gme::SApplication & app)						{
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::SWindow				& mainWindow			= framework.RootWindow;
	mainWindow.Size			= {1280, 720};
	if_true_e(::gpk::failed(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)));
	::gpk::SGUI					& gui					= *framework.GUI;

	const int32_t				iShades					= 16;
	gui.ThemeDefault		= app.PaletteColumn * iShades + app.PaletteRow;
	gui.ColorModeDefault	= ::gpk::GUI_COLOR_MODE_3D;
	::gpk::cid_t				controlTestRoot			= ::gpk::controlCreate(gui);
	const ::gpk::bgra			colorBase				= (*gui.Colors->Palette)[gui.ThemeDefault];
	::gpk::memcpy_s(app.Palettes, gui.Colors->DefaultColors.Storage);
	app.Palettes[::gpk::GUI_CONTROL_PALETTE_NORMAL				]	= (uint16_t)gui.Colors->Palettes.push_back({colorBase, {}, {}, {}, {}, {}, ::gpk::RED, {}, {}, {}, });// gui.DefaultColors.CONTROL_NORMAL				;
	app.Palettes[::gpk::GUI_CONTROL_PALETTE_HOVER				]	= (uint16_t)gui.Colors->Palettes.push_back({colorBase, {}, {}, {}, {}, {}, ::gpk::RED, {}, {}, {}, });// gui.DefaultColors.CONTROL_HOVER				;
	app.Palettes[::gpk::GUI_CONTROL_PALETTE_PRESSED				]	= (uint16_t)gui.Colors->Palettes.push_back({colorBase, {}, {}, {}, {}, {}, ::gpk::RED, {}, {}, {}, });// gui.DefaultColors.CONTROL_PRESSED			;
	app.Palettes[::gpk::GUI_CONTROL_PALETTE_SELECTED			]	= (uint16_t)gui.Colors->Palettes.push_back({colorBase, {}, {}, {}, {}, {}, ::gpk::RED, {}, {}, {}, });// gui.DefaultColors.CONTROL_SELECTED			;
	app.Palettes[::gpk::GUI_CONTROL_PALETTE_SELECTED_HOVER		]	= (uint16_t)gui.Colors->Palettes.push_back({colorBase, {}, {}, {}, {}, {}, ::gpk::RED, {}, {}, {}, });// gui.DefaultColors.CONTROL_SELECTED_HOVER		;
	app.Palettes[::gpk::GUI_CONTROL_PALETTE_SELECTED_PRESSED	]	= (uint16_t)gui.Colors->Palettes.push_back({colorBase, {}, {}, {}, {}, {}, ::gpk::RED, {}, {}, {}, });// gui.DefaultColors.CONTROL_SELECTED_PRESSED	;
	app.Palettes[::gpk::GUI_CONTROL_PALETTE_OUTDATED			]	= (uint16_t)gui.Colors->Palettes.push_back({colorBase, {}, {}, {}, {}, {}, ::gpk::RED, {}, {}, {}, });// gui.DefaultColors.CONTROL_OUTDATED			;

	::gpk::SControlPlacement	& controlRoot			= gui.Controls.Placement[controlTestRoot];
	controlRoot.Area		= {{0, 0}, {320, 240}};
	controlRoot.Border		= {4, 4, 4, 4};
	controlRoot.Margin		= {20, 20, 20, 10};
	controlRoot.Align		= ::gpk::ALIGN_CENTER					;
	//gui.Controls.Modes[controlTestRoot].UseNewPalettes	= 1;
	::gpk::memcpy_s(gui.Controls.Draw[controlTestRoot].Palettes.Storage, app.Palettes);

	gui.Controls.Constraints[controlTestRoot].AttachSizeToControl	= {controlTestRoot, controlTestRoot};
	//gui.Controls.Modes	[controlTestRoot].Design				= true;
	::gpk::controlSetParent(gui, controlTestRoot, -1);

	for(uint32_t iChild = 0; iChild < 90; ++iChild) {
		::gpk::cid_t					controlTestChild0		= ::gpk::controlCreate(gui);
		::gpk::SControlPlacement		& control				= gui.Controls.Placement	[controlTestChild0];
		::gpk::SControlText				& controlText			= gui.Controls.Text		[controlTestChild0];
		//gui.Controls.Modes[controlTestChild0].UseNewPalettes				= 1;
		::gpk::memcpy_s(gui.Controls.Draw[controlTestChild0].Palettes.Storage, app.Palettes);


		control		.Area			= {{0, 0}, {(int16_t)(800 / 3 / (1 + iChild / 9)), (int16_t)(600 / 3 / (1 + iChild / 9))}}; // {32, 32}};//
		//control		.Border		= {iChild % 5, iChild % 7, iChild % 11, iChild % 13};
		control		.Border			= {2, 2, 2, 2};
		control		.Margin			= {1, 1, 1, 1};

		char							buffer [1024]				= {};
		const int32_t					lenText						= (int32_t)sprintf_s(buffer, "(%u)", controlTestChild0);
		controlText	.Text			= {::gpk::label(buffer).begin(), (uint32_t)lenText};
		//= {0 == (iChild % 4), 0 == (iChild % 5)};
		gui.Controls.SetDisabled(controlTestChild0,	0 == (iChild % 9));
		//gui.Controls.States	[controlTestChild0].Mask		&= ~::gpk::GUI_CONTROL_FLAG_Hovered;
		gui.Controls.Draw	[controlTestChild0].NoClient	= 
		gui.Controls.Draw	[controlTestChild0].NoBorder	= iChild % 2;
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
		::gpk::controlSetParent(gui, controlTestChild0, ::gpk::cid_t(iChild / 9));
		if((iChild / 9) == 5)
			gui.Controls.Constraints[controlTestChild0].AttachSizeToText	= {true, true};
	}
	{
		app.IdExit													= ::gpk::controlCreate(gui);
		::gpk::SControlPlacement													& controlExit			= gui.Controls.Placement[app.IdExit];
		controlExit.Align											= ::gpk::ALIGN_BOTTOM_RIGHT				;
		::gpk::SControlText												& controlText			= gui.Controls.Text[app.IdExit];
		controlText.Text											= "Exit";
		::gpk::controlSetParent(gui, app.IdExit, 9);
	}
	{
		app.IdTheme													= ::gpk::controlCreate(gui);
		::gpk::SControlPlacement													& controlExit			= gui.Controls.Placement[app.IdTheme];
		controlExit.Align											= ::gpk::ALIGN_CENTER_BOTTOM			;
		::gpk::SControlText												& controlText			= gui.Controls.Text[app.IdTheme];
		controlText.Text											= "Theme";
		::gpk::controlSetParent(gui, app.IdTheme, 8);
	}
	{
		app.IdMode													= ::gpk::controlCreate(gui);
		::gpk::SControlPlacement													& controlExit			= gui.Controls.Placement[app.IdMode];
		controlExit.Align											= ::gpk::ALIGN_BOTTOM_LEFT				;
		::gpk::SControlText												& controlText			= gui.Controls.Text[app.IdMode];
		controlText.Text											= "3D Mode";
		::gpk::controlSetParent(gui, app.IdMode, 7);
	}
	{
		app.IdNewPalette											= ::gpk::controlCreate(gui);
		::gpk::SControlPlacement													& controlExit			= gui.Controls.Placement[app.IdNewPalette];
		controlExit.Align											= ::gpk::ALIGN_CENTER_BOTTOM;
		::gpk::SControlText												& controlText			= gui.Controls.Text[app.IdNewPalette];
		controlText.Text											= "Palette Mode";
		::gpk::controlSetParent(gui, app.IdNewPalette, 2);
	}
	for(uint32_t iButton = app.IdExit; iButton < gui.Controls.States.size(); ++iButton) {
		::gpk::SControlPlacement										& control				= gui.Controls.Placement[iButton];
		::gpk::SControlText												& controlText			= gui.Controls.Text[iButton];
		control.Area												= {{0, 0}, {64, 20}};
		control.Border												= {1, 1, 1, 1};
		control.Margin												= {1, 1, 1, 1};
		controlText.Align											= ::gpk::ALIGN_CENTER;
		::gpk::memcpy_s(gui.Controls.Draw[iButton].Palettes.Storage, app.Palettes);
	}

	gui.Controls.Constraints[app.IdMode			].AttachSizeToText.x	= true;
	gui.Controls.Constraints[app.IdNewPalette	].AttachSizeToText.x	= true;

	//const int32_t													iShadesHalf				= iShades / 2;
	for(uint32_t iTone = 0, countTones = gui.Colors->Palette->size() / iShades; iTone < countTones; ++iTone) {
		for(uint32_t iShade = 0; iShade < iShades; ++iShade) {
			const ::gpk::cid_t													idPaletteItem			= ::gpk::controlCreate(gui);
			::gpk::SControlPlacement													& control				= gui.Controls.Placement	[idPaletteItem];
			::gpk::SControlText												& controlText			= gui.Controls.Text[idPaletteItem];
			control.Align												= ::gpk::ALIGN_TOP_RIGHT;
			control.Area												= {{(int16_t)(256 - iShade * 12), (int16_t)(iTone * 12)}, {12, 12}};
			//control.Area.Offset											+= {mainWi, 0};
			//control.Area.Offset											-= {iShades * iShadesHalf - 8, iShades * iShadesHalf - 8};
			control.Border												= {1, 1, 1, 1};
			control.Margin												= {1, 1, 1, 1};
			gui.Controls.Draw[idPaletteItem].ColorTheme											= int16_t(iTone * iShades + iShade + 1);
			controlText.Text											= ".";
			::gpk::controlSetParent(gui, idPaletteItem, 0);
		}
	}
	for(uint32_t iShade = 0; iShade < iShades; ++iShade) {
		const ::gpk::cid_t													idPaletteRow		= ::gpk::controlCreate(gui);
		::gpk::SControlPlacement													& controlRow		= gui.Controls.Placement	[idPaletteRow];
		::gpk::SControlText												& controlRowText	= gui.Controls.Text[idPaletteRow];
		controlRow.Align											= ::gpk::ALIGN_TOP_RIGHT;
		controlRow.Area												= {{(int16_t)(256 - iShade * 12), -12}, {12, 12}};
		controlRow.Border											= {1, 1, 1, 1};
		controlRow.Margin											= {1, 1, 1, 1};
		gui.Controls.Draw[idPaletteRow].ColorTheme										= int16_t(iShade + 1);
		controlRowText.Text											= app.RowText[iShade];
		//gui.Controls.Constraints[idPaletteRow].AttachSizeToText.x	= true;
		::gpk::controlSetParent(gui, idPaletteRow, 0);
	}
	for(uint32_t iTone = 0, countTones = gui.Colors->Palette->size() / iShades; iTone < countTones; ++iTone) {
		const ::gpk::cid_t													idPaletteRow		= ::gpk::controlCreate(gui);
		::gpk::SControlPlacement													& controlRow		= gui.Controls.Placement	[idPaletteRow];
		::gpk::SControlText												& controlRowText	= gui.Controls.Text[idPaletteRow];
		controlRow.Align												= ::gpk::ALIGN_TOP_RIGHT;
		controlRow.Area												= {{(int16_t)256+12, (int16_t)(iTone * 12)}, {12, 12}};
		//control.Area.Offset										+= {mainWi, 0};
		//control.Area.Offset										-= {iShades * iShadesHalf - 8, iShades * iShadesHalf - 8};
		controlRow.Border											= {1, 1, 1, 1};
		controlRow.Margin											= {1, 1, 1, 1};
		gui.Controls.Draw[idPaletteRow].ColorTheme										= int16_t(iTone * iShades + 0 + 1);
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
	//if_true_e(::gpk::failed(::gpk::bmpOrBmgLoad(bmpFileName2, app.TextureFont)), "");
	//::gpk::SImage<::gpk::bgra>								& verticalAtlas								= app.VerticalAtlas;
	//const ::gpk::n2<uint32_t>									fontCharSize								= {9, 16};
	//verticalAtlas.resize(fontCharSize.x, fontCharSize.y * 256);
	//for(uint32_t iChar = 0; iChar < 256; ++iChar) {
	//	const uint32_t													srcOffsetY									= iChar / 32 * fontCharSize.y;
	//	const uint32_t													dstOffsetY									= iChar * fontCharSize.y;
	//	for(uint32_t y = 0; y < fontCharSize.y; ++y) {
	//		for(uint32_t x = 0; x < fontCharSize.x; ++x) {
	//			const uint32_t												srcOffsetX									= iChar % 32 * fontCharSize.x;
	//			const uint32_t												dstOffsetX									= 0;
	//			const ::gpk::bgra										& srcColor									= app.TextureFont.View[srcOffsetY + y][srcOffsetX + x];
	//			verticalAtlas.View[dstOffsetY + y][dstOffsetX + x]		= srcColor;
	//		}
	//	}
	//}
	//const ::gpk::n2<uint32_t>									& textureFontMetrics						= verticalAtlas.View.metrics();// app.TextureFont.View.metrics();
	//do { if_fail_fef(gui.FontTexture.resize(textureFontMetrics), "Whou would we failt ro resize=");
	//for(uint32_t y = 0, yMax = textureFontMetrics.y; y < yMax; ++y)
	//for(uint32_t x = 0, xMax = textureFontMetrics.x; x < xMax; ++x) {
	//	const ::gpk::bgra											& srcColor									= verticalAtlas.View[y][x];//app.TextureFont.View[y][x];
	//	gui.FontTexture.View[y * textureFontMetrics.x + x]
	//		=	0 != srcColor.r
	//		||	0 != srcColor.g
	//		||	0 != srcColor.b
	//		;
	//}

	//::gpk::apod<char>	encoded;
	//::gpk::base64Encode({(const ubyte_t*)gui.FontTexture.Texels.begin(), gui.FontTexture.Texels.size() * 4}, encoded);
	//FILE						* fp = 0;
	//const int nul = 0;
	//fopen_s(&fp, "codepage_437_encoded.txt", "wb");
	//fwrite(encoded.begin(), 1, encoded.size(), fp);
	//fwrite(&nul, 1, 1, fp);
	//fclose(fp);


	return 0;
}

::gpk::err_t			update		(::gme::SApplication & app, bool exitSignal)	{
	//::gpk::STimer															timer;
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, exitSignal);
	{
		::std::lock_guard			lock					(app.LockRender);
		app.Framework.RootWindow.BackBuffer	= app.Offscreen;
	}
	::gpk::SFramework			& framework				= app.Framework;
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(framework));
	{
		::gpk::SGUI					& gui					= *framework.GUI;
		::std::lock_guard			lock					(app.LockGUI);
		if(1 == ::gpk::guiProcessControls(gui, [&gui, &app](::gpk::cid_t iControl) {
			info_printf("Executed %u.", iControl);
			if(iControl == app.IdExit)
				return 1;
			else if(iControl == app.IdMode) {
				for(uint32_t iChild = 0; iChild < gui.Controls.States.size(); ++iChild)
					gui.Controls.Draw[iChild].ColorMode							= gui.Controls.Draw[iChild].ColorMode == ::gpk::GUI_COLOR_MODE_Flat ? ::gpk::GUI_COLOR_MODE_3D : ::gpk::GUI_COLOR_MODE_Flat;
			}
			else if(iControl == app.IdTheme) {
				++gui.ThemeDefault;
				if(gui.ThemeDefault >= gui.Colors->ControlThemes->size())
					gui.ThemeDefault												= 0;
			}
			else if(iControl == app.IdNewPalette) {
				for(uint32_t iChild = 0; iChild < gui.Controls.States.size(); ++iChild)
					gui.Controls.Draw[iChild].UseNewPalettes						= gui.Controls.Draw[iChild].UseNewPalettes ? 0 : 1;
			}
			else if(iControl > app.IdMode) {
				gui.Controls.Draw[5].ColorTheme								= int16_t(iControl - app.IdNewPalette);
				if(gui.Controls.Draw[5].ColorTheme >= (int32_t)gui.Colors->Palette->size())
					gui.Controls.Draw[5].ColorTheme								= 10;
				for(uint32_t iChild = 0; iChild < gui.Controls.Children[5].size(); ++iChild)
					gui.Controls.Draw[gui.Controls.Children[5][iChild]].ColorTheme = gui.Controls.Draw[5].ColorTheme;
			}
			return 0;
		}))
			return 1;
	}
	::gpk::sleep(1);
	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}

::gpk::err_t			cleanup		(::gme::SApplication & app)						{ return ::gpk::mainWindowDestroy(app.Framework.RootWindow); }
::gpk::err_t			draw		(::gme::SApplication & app)						{
	::gpk::STimer					timer;
	::gpk::pobj<::gpk::rtbgra8d32>	target;
	target.create();
	target->resize(app.Framework.RootWindow.Size, {0xFF, 0x40, 0x7F, 0xFF}, (uint32_t)-1);
	//::gpk::clearTarget(*target);
	{
		::std::lock_guard				lock					(app.LockGUI);
		::gpk::guiDraw(*app.Framework.GUI, target->Color.View);
		::gpk::grid_copy(target->Color.View, app.VerticalAtlas.View);
	}
	{
		::std::lock_guard				lock					(app.LockRender);
		app.Offscreen				= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}
