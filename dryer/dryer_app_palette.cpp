#include "dryer_palette.h"

::gpk::error_t							dry::setupPalettes		(::gpk::SGUI & gui) {
	::gpk::view_array<::gpk::SColorBGRA>		paletteNormal			= gui.Palettes[gui.DefaultColors[::gpk::GUI_CONTROL_PALETTE_NORMAL	]];
	::gpk::view_array<::gpk::SColorBGRA>		paletteDisabled			= gui.Palettes[gui.DefaultColors[::gpk::GUI_CONTROL_PALETTE_DISABLED]];
	::gpk::view_array<::gpk::SColorBGRA>		palettePressed			= gui.Palettes[gui.DefaultColors[::gpk::GUI_CONTROL_PALETTE_PRESSED	]];
	::gpk::view_array<::gpk::SColorBGRA>		paletteHover			= gui.Palettes[gui.DefaultColors[::gpk::GUI_CONTROL_PALETTE_HOVER	]];
	::gpk::view_array<::gpk::SColorBGRA>		paletteSelected			= gui.Palettes[gui.DefaultColors[::gpk::GUI_CONTROL_PALETTE_SELECTED]];

	paletteNormal.fill(::gpk::WHITE);
	paletteNormal[::gpk::GUI_CONTROL_COLOR_BACKGROUND			] = ::gpk::BLACK;
	paletteNormal[::gpk::GUI_CONTROL_COLOR_CLIENT				] = ::gpk::BLACK;

	paletteDisabled.fill(::gpk::GRAY);
	paletteDisabled[::gpk::GUI_CONTROL_COLOR_BACKGROUND			] = ::gpk::DARKGRAY;
	paletteDisabled[::gpk::GUI_CONTROL_COLOR_CLIENT				] = ::gpk::DARKGRAY;

	palettePressed.fill(::gpk::ORANGE);
	palettePressed[::gpk::GUI_CONTROL_COLOR_BACKGROUND			] = ::gpk::DARKBLUE / 2.f;
	palettePressed[::gpk::GUI_CONTROL_COLOR_CLIENT				] = ::gpk::DARKBLUE / 2.f;

	paletteHover.fill(::gpk::ORANGE *.5 + ::gpk::WHITE * .5);
	paletteHover[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT			] =
	paletteHover[::gpk::GUI_CONTROL_COLOR_BORDER_TOP			] =
	paletteHover[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT			] =
	paletteHover[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM			] = ::gpk::WHITE;
	paletteHover[::gpk::GUI_CONTROL_COLOR_BACKGROUND			] = ::gpk::DARKBLUE;
	paletteHover[::gpk::GUI_CONTROL_COLOR_CLIENT				] = ::gpk::DARKBLUE;


	paletteSelected.fill(::gpk::ORANGE);
	paletteSelected[::gpk::GUI_CONTROL_COLOR_BACKGROUND			] = ::gpk::BLACK;
	paletteSelected[::gpk::GUI_CONTROL_COLOR_CLIENT				] = ::gpk::BLACK;

	return 0;
}
