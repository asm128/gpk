#include "gpk_gui_control.h"
#include "gpk_input.h"
#include "gpk_font.h"

#include "gpk_ascii_color.h"

#ifndef GPK_GUI_H_23627
#define GPK_GUI_H_23627

namespace gpk
{
	struct SGUIZoom {
		::gpk::n2f64		DPI					= {1.0, 1.0};
		::gpk::minmaxf64	ZoomLimits			= {0.1, 10.0};
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

		::gpk::asti16<::gpk::GUI_CONTROL_PALETTE_COUNT>	
											DefaultColors			= {};

		::gpk::apod<::gpk::astatic<::gpk::bgra, ::gpk::UI_CONTROL_AREA_COUNT>>
											Palettes;
	};

	struct SGUI {
		::gpk::n2u16						LastSize			= {};
		::gpk::n2f32						CursorPos			= {};
		::gpk::SControlTable				Controls			= {};
		::gpk::pobj<::gpk::SGUIColors>		Colors;
		// Font
		::gpk::apobj<::gpk::SRasterFont>	Fonts				= {};

		::gpk::SGUIZoom						LastZoom			= {};
		::gpk::SGUIZoom						Zoom				= {};
		uint16_t							SelectedFont		= 7;
		uint16_t							ThemeDefault		= ::gpk::ASCII_COLOR_CYAN * 16 + 8;
		::gpk::GUI_COLOR_MODE				ColorModeDefault	= ::gpk::GUI_COLOR_MODE_3D;//::gpk::GUI_COLOR_MODE_DEFAULT;
		bool								InputModeTouch		= true;
	};

	::gpk::error_t			guiProcessInput					(::gpk::SGUI & gui, const ::gpk::SInput & input, ::gpk::vpobj<::gpk::SSystemEvent> in_Events = {});
	::gpk::error_t			guiUpdateMetrics				(::gpk::SGUI & gui, const ::gpk::n2u16 & targetSize, bool forceUpdate);
	::gpk::error_t			guiDraw							(::gpk::SGUI & gui, ::gpk::g8bgra target);
	::gpk::error_t			guiGetProcessableControls		(const ::gpk::SGUI & gui, ::gpk::acid & controlIndices);
	::gpk::error_t			guiDeselect						(::gpk::SGUI & gui);
	::gpk::error_t			guiSelect						(::gpk::SGUI & gui, cid_t controlToSelect);

	::gpk::error_t			controlDelete					(::gpk::SGUI & gui, cid_t iControl, bool recursive = true);
	::gpk::cid_t 			controlCreate					(::gpk::SGUI & gui);
	::gpk::cid_t			controlCreateChild				(::gpk::SGUI & gui, cid_t iParent);
	::gpk::error_t			controlSetParent				(::gpk::SGUI & gui, cid_t iControl, cid_t iParent);
	::gpk::error_t			controlDrawHierarchy			(::gpk::SGUI & gui, cid_t iControl, ::gpk::g8bgra target);
	::gpk::error_t			controlTextSet					(::gpk::SGUI & gui, cid_t iControl, const ::gpk::vcs & text);
	::gpk::error_t			controlFontSet					(::gpk::SGUI & gui, cid_t iControl, int16_t iFont);
	//::gpk::error_t			controlUpdateMetrics			(::gpk::SGUI & gui, int32_t iControl, const ::gpk::n2<uint32_t> & targetSize);
	::gpk::error_t			controlUpdateMetricsTopToDown	(::gpk::SGUI & gui, cid_t iControl, const ::gpk::n2u16 & targetSize, bool forceUpdate);
	::gpk::error_t			controlMetricsInvalidate		(::gpk::SGUI & gui, cid_t iControl);
	::gpk::error_t			controlHidden					(const ::gpk::SGUI & gui, cid_t iControl);
	::gpk::error_t			controlDisabled					(const ::gpk::SGUI & gui, cid_t iControl);
	::gpk::error_t			controlInvalid					(const ::gpk::SGUI & gui, cid_t iControl);


	::gpk::error_t			guiProcessControls				(const ::gpk::SGUI & gui, const ::std::function<::gpk::error_t(::gpk::cid_t iControl)> & funcOnExecute);
	::gpk::error_t			guiProcessControls				(const ::gpk::SGUI & gui, ::gpk::vcid controlsToProcess, const ::std::function<::gpk::error_t(::gpk::cid_t iControl)> & funcOnExecute);


} // namespace

#endif // GPK_GUI_H_23627
