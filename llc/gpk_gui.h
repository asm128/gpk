#include "gpk_gui_control.h"
#include "gpk_input.h"
#include "gpk_font.h"

#include "gpk_ascii_color.h"

#ifndef GPK_GUI_H_23627
#define GPK_GUI_H_23627

namespace gpk
{
	struct SGUIZoom {
		n2f3_t				DPI					= {1.0, 1.0};
		minmaxf3_t			ZoomLimits			= {0.1, 10.0};
		f3_t				ZoomLevel			= 1.0;

		bool				operator==			(const SGUIZoom & other)		const	noexcept	{
			return DPI			== other.DPI
				&& ZoomLevel	== other.ZoomLevel
				;
		}
		inline	bool		operator!=			(const SGUIZoom & other)		const	noexcept	{ return !operator==(other); }
	};

	struct SGUIColors {
		papod<bgra>				Palette					= {};
		papod<SControlTheme>	ControlThemes			= {};

		asts1_t<GUI_CONTROL_PALETTE_COUNT>	DefaultColors			= {};

		apod<asttc<bgra, UI_CONTROL_AREA_COUNT>>
											Palettes;
	};

	struct SGUI {
		n2u1_t				LastSize			= {};
		n2f2_t				CursorPos			= {};
		SControlTable		Controls			= {};
		pobj<SGUIColors>	Colors;
		// Font
		apobj<SRasterFont>	Fonts				= {};

		SGUIZoom			LastZoom			= {};
		SGUIZoom			Zoom				= {};
		u1_t				SelectedFont		= 7;
		u1_t				ThemeDefault		= ASCII_COLOR_CYAN * 16 + 8;
		GUI_COLOR_MODE		ColorModeDefault	= GUI_COLOR_MODE_3D;//GUI_COLOR_MODE_DEFAULT;
		bool								InputModeTouch		= true;
	};

	err_t			guiProcessInput					(SGUI & gui, const SInput & input, vpobj<SEventSystem> in_Events = {});
	err_t			guiUpdateMetrics				(SGUI & gui, const n2u1_t & targetSize, bool forceUpdate);
	err_t			guiDraw							(SGUI & gui, g8bgra target);
	err_t			guiGetProcessableControls		(const SGUI & gui, acid & controlIndices);
	err_t			guiDeselect						(SGUI & gui);
	err_t			guiSelect						(SGUI & gui, cid_t controlToSelect);

	err_t			controlDelete					(SGUI & gui, cid_t iControl, bool recursive = true);
	cid_t 			controlCreate					(SGUI & gui);
	cid_t			controlCreateChild				(SGUI & gui, cid_t iParent);
	err_t			controlSetParent				(SGUI & gui, cid_t iControl, cid_t iParent);
	err_t			controlDrawHierarchy			(SGUI & gui, cid_t iControl, g8bgra target);
	err_t			controlTextSet					(SGUI & gui, cid_t iControl, vcst_t text);
	err_t			controlFontSet					(SGUI & gui, cid_t iControl, s1_t iFont);
	//err_t			controlUpdateMetrics			(SGUI & gui, s2_t iControl, const n2<u2_t> & targetSize);
	err_t			controlUpdateMetricsTopToDown	(SGUI & gui, cid_t iControl, const n2u1_t & targetSize, bool forceUpdate);
	err_t			controlMetricsInvalidate		(SGUI & gui, cid_t iControl);
	err_t			controlHidden					(const SGUI & gui, cid_t iControl);
	err_t			controlDisabled					(const SGUI & gui, cid_t iControl);
	err_t			controlInvalid					(const SGUI & gui, cid_t iControl);


	err_t			guiProcessControls				(const SGUI & gui, const function<err_t(cid_t iControl)> & funcOnExecute);
	err_t			guiProcessControls				(const SGUI & gui, vcid controlsToProcess, const function<err_t(cid_t iControl)> & funcOnExecute);
} // namespace

#endif // GPK_GUI_H_23627
