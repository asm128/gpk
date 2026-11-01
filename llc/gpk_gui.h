#include "gpk_gui_control.h"
#include "gpk_input.h"
#include "gpk_font.h"

#include "gpk_ascii_color.h"

#ifndef GPK_GUI_H_23627
#define GPK_GUI_H_23627

namespace gpk
{
	struct SGUIZoom {
		::gpk::n2f3_t		DPI					= {1.0, 1.0};
		::gpk::minmaxf3_t	ZoomLimits			= {0.1, 10.0};
		double				ZoomLevel			= 1.0;

		bool				operator==			(const ::gpk::SGUIZoom & other)		const	noexcept	{
			return DPI			== other.DPI
				&& ZoomLevel	== other.ZoomLevel
				;
		}
		inline	bool		operator!=			(const ::gpk::SGUIZoom & other)		const	noexcept	{ return !operator==(other); }
	};

	struct SGUIColors {
		::gpk::papod<::gpk::bgra>			Palette					= {};
		::gpk::papod<::gpk::SControlTheme>	ControlThemes			= {};

		::gpk::asts1_t<::gpk::GUI_CONTROL_PALETTE_COUNT>	
											DefaultColors			= {};

		::gpk::apod<::gpk::astatic<::gpk::bgra, ::gpk::UI_CONTROL_AREA_COUNT>>
											Palettes;
	};

	struct SGUI {
		::gpk::n2u1_t						LastSize			= {};
		::gpk::n2f2_t						CursorPos			= {};
		::gpk::SControlTable				Controls			= {};
		::gpk::pobj<::gpk::SGUIColors>		Colors;
		// Font
		::gpk::apobj<::gpk::SRasterFont>	Fonts				= {};

		::gpk::SGUIZoom						LastZoom			= {};
		::gpk::SGUIZoom						Zoom				= {};
		u1_t							SelectedFont		= 7;
		u1_t							ThemeDefault		= ::gpk::ASCII_COLOR_CYAN * 16 + 8;
		::gpk::GUI_COLOR_MODE				ColorModeDefault	= ::gpk::GUI_COLOR_MODE_3D;//::gpk::GUI_COLOR_MODE_DEFAULT;
		bool								InputModeTouch		= true;
	};

	::gpk::err_t			guiProcessInput					(::gpk::SGUI & gui, const ::gpk::SInput & input, ::gpk::vpobj<::gpk::SEventSystem> in_Events = {});
	::gpk::err_t			guiUpdateMetrics				(::gpk::SGUI & gui, const ::gpk::n2u1_t & targetSize, bool forceUpdate);
	::gpk::err_t			guiDraw							(::gpk::SGUI & gui, ::gpk::g8bgra target);
	::gpk::err_t			guiGetProcessableControls		(const ::gpk::SGUI & gui, ::gpk::acid & controlIndices);
	::gpk::err_t			guiDeselect						(::gpk::SGUI & gui);
	::gpk::err_t			guiSelect						(::gpk::SGUI & gui, cid_t controlToSelect);

	::gpk::err_t			controlDelete					(::gpk::SGUI & gui, cid_t iControl, bool recursive = true);
	::gpk::cid_t 			controlCreate					(::gpk::SGUI & gui);
	::gpk::cid_t			controlCreateChild				(::gpk::SGUI & gui, cid_t iParent);
	::gpk::err_t			controlSetParent				(::gpk::SGUI & gui, cid_t iControl, cid_t iParent);
	::gpk::err_t			controlDrawHierarchy			(::gpk::SGUI & gui, cid_t iControl, ::gpk::g8bgra target);
	::gpk::err_t			controlTextSet					(::gpk::SGUI & gui, cid_t iControl, const ::gpk::vcst_t & text);
	::gpk::err_t			controlFontSet					(::gpk::SGUI & gui, cid_t iControl, s1_t iFont);
	//::gpk::err_t			controlUpdateMetrics			(::gpk::SGUI & gui, s2_t iControl, const ::gpk::n2<u2_t> & targetSize);
	::gpk::err_t			controlUpdateMetricsTopToDown	(::gpk::SGUI & gui, cid_t iControl, const ::gpk::n2u1_t & targetSize, bool forceUpdate);
	::gpk::err_t			controlMetricsInvalidate		(::gpk::SGUI & gui, cid_t iControl);
	::gpk::err_t			controlHidden					(const ::gpk::SGUI & gui, cid_t iControl);
	::gpk::err_t			controlDisabled					(const ::gpk::SGUI & gui, cid_t iControl);
	::gpk::err_t			controlInvalid					(const ::gpk::SGUI & gui, cid_t iControl);


	::gpk::err_t			guiProcessControls				(const ::gpk::SGUI & gui, const ::gpk::function<::gpk::err_t(::gpk::cid_t iControl)> & funcOnExecute);
	::gpk::err_t			guiProcessControls				(const ::gpk::SGUI & gui, ::gpk::vcid controlsToProcess, const ::gpk::function<::gpk::err_t(::gpk::cid_t iControl)> & funcOnExecute);
} // namespace

#endif // GPK_GUI_H_23627
