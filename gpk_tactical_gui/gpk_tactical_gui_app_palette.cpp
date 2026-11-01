#include "gpk_tactical_gui_palette.h"

::gpk::error_t							dry::setupPalettes		(::gpk::SGUI & gui) {
	::gpk::SGUIColors						& colors				= *gui.Colors;
	auto									& paletteNormal			= colors.Palettes[colors.DefaultColors[::gpk::GUI_CONTROL_PALETTE_NORMAL	]];
	auto									& paletteDisabled		= colors.Palettes[colors.DefaultColors[::gpk::GUI_CONTROL_PALETTE_DISABLED]];
	auto									& palettePressed			= colors.Palettes[colors.DefaultColors[::gpk::GUI_CONTROL_PALETTE_PRESSED	]];
	auto									& paletteHover			= colors.Palettes[colors.DefaultColors[::gpk::GUI_CONTROL_PALETTE_HOVER	]];
	auto									& paletteSelected			= colors.Palettes[colors.DefaultColors[::gpk::GUI_CONTROL_PALETTE_SELECTED]];

	::gpk::view<::gpk::bgra>{paletteNormal}.fill(::gpk::WHITE);
	paletteNormal[::gpk::UI_CONTROL_AREA_BACKGROUND			] = ::gpk::BLACK;
	paletteNormal[::gpk::UI_CONTROL_AREA_CLIENT				] = ::gpk::BLACK;

	::gpk::view<::gpk::bgra>{paletteDisabled}.fill(::gpk::GRAY);
	paletteDisabled[::gpk::UI_CONTROL_AREA_BACKGROUND			] = ::gpk::DARKGRAY;
	paletteDisabled[::gpk::UI_CONTROL_AREA_CLIENT				] = ::gpk::DARKGRAY;

	::gpk::view<::gpk::bgra>{palettePressed}.fill(::gpk::ORANGE);
	palettePressed[::gpk::UI_CONTROL_AREA_BACKGROUND			] = ::gpk::DARKBLUE / 2.f;
	palettePressed[::gpk::UI_CONTROL_AREA_CLIENT				] = ::gpk::DARKBLUE / 2.f;

	::gpk::view<::gpk::bgra>{paletteHover}.fill(::gpk::ORANGE *.5 + ::gpk::WHITE * .5);
	paletteHover[::gpk::UI_CONTROL_AREA_BORDER_LEFT			] =
	paletteHover[::gpk::UI_CONTROL_AREA_BORDER_TOP			] =
	paletteHover[::gpk::UI_CONTROL_AREA_BORDER_RIGHT			] =
	paletteHover[::gpk::UI_CONTROL_AREA_BORDER_BOTTOM			] = ::gpk::WHITE;
	paletteHover[::gpk::UI_CONTROL_AREA_BACKGROUND			] = ::gpk::DARKBLUE;
	paletteHover[::gpk::UI_CONTROL_AREA_CLIENT				] = ::gpk::DARKBLUE;


	::gpk::view<::gpk::bgra>{paletteSelected}.fill(::gpk::ORANGE);
	paletteSelected[::gpk::UI_CONTROL_AREA_BACKGROUND			] = ::gpk::BLACK;
	paletteSelected[::gpk::UI_CONTROL_AREA_CLIENT				] = ::gpk::BLACK;

	return 0;
}
