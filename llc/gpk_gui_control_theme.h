#include "gpk_gui_control.h"
#include "gpk_font.h"

#ifndef GPK_GUI_CONTROL_THEME_H
#define GPK_GUI_CONTROL_THEME_H

nmsp gpk
{
	err_t	controlColorsDefault	(SControlColors & output);
	err_t	controlColorsDefault	(pobj<SControlColors> & output);
	err_t	controlFontsDefault		(apobj<SRasterFont> & output);
	err_t	controlColorsShade
		( asttc<bgra, UI_CONTROL_AREA_COUNT>	& output
		, view<cnst bgra>						base
		, u2_t shade
		, u2_t shades
		);
} // nmsp 

#endif //  GPK_GUI_CONTROL_THEME_H
