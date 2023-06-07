#include "gpk_input.h"
#include "gpk_array_static.h"
#include "gpk_ascii_color.h"
#include "gpk_font.h"
#include "gpk_tri2.h"
#include "gpk_rect.h"
#include "gpk_align.h"

#include "gpk_sysevent.h"

#ifndef GPK_GUI_H_2903874982374
#define GPK_GUI_H_2903874982374

namespace gpk
{
#pragma pack(push, 1)
	enum GUI_COLOR_MODE : uint8_t
		{ GUI_COLOR_MODE_DEFAULT	= 0
		, GUI_COLOR_MODE_3D
		, GUI_COLOR_MODE_FLAT
		, GUI_COLOR_MODE_COUNT
		};

	struct SControlMode {
		uint8_t					ColorMode			: 2;
		uint8_t					UseNewPalettes		: 1;
		uint8_t					NoHoverEffect		: 1;
		uint8_t					FrameOut			: 1;
		uint8_t					Design				: 1;
		uint8_t					NoBackgroundRect	: 1;
		uint8_t					PercentMetrics		: 1; // 8th bit
	};

	struct SControlRectangle {
		::gpk::rect2i16			Local;
		::gpk::rect2i16			Global;
	};

	enum GUI_CONTROL_AREA_OLD : uint8_t
		{ GUI_CONTROL_AREA_OLD_BACKGROUND	= 0
		, GUI_CONTROL_AREA_OLD_CLIENT
		, GUI_CONTROL_AREA_OLD_BORDER_LEFT
		, GUI_CONTROL_AREA_OLD_BORDER_TOP
		, GUI_CONTROL_AREA_OLD_BORDER_RIGHT
		, GUI_CONTROL_AREA_OLD_BORDER_BOTTOM
		, GUI_CONTROL_AREA_OLD_COUNT
		};

	struct SControlMetrics {
		::gpk::SControlRectangle	Total		= {};
		::gpk::SControlRectangle	Client		= {};
		::gpk::rect2i16				Text		= {};
		::gpk::rect2i16				Rectangles	[::gpk::GUI_CONTROL_AREA_OLD_COUNT]	= {};
		::gpk::tri2i16				Triangles	[8]		= {};
	};

	struct SControlAttachId {
		int32_t		IdControl		: 31;
		int32_t		Total			: 1;
	};

	struct SControlConstraints {
		::gpk::n2i32					AttachSizeToControl	;
		::gpk::n2<bool>					AttachSizeToText	;
		::gpk::rect<int32_t>			DockToControl		;
		::gpk::minmax<::gpk::n2i16>	SizeMinMax			= {{}, {0x7FFF, 0x7FFF}};
	};

	struct SControlState {
		bool		Disabled		: 1;
		bool		Hover			: 1;
		bool		UnHover			: 1;
		bool		Pressed			: 1;
		bool		Released		: 1;
		bool		Selected		: 1;
		bool		Execute			: 1;
		bool		Updated			: 1;

		bool		Hidden			: 1;
		bool		ImageInvertY	: 1;
		bool		Unused			: 1;
	};

	enum UI_CONTROL_AREA : uint8_t
		{ UI_CONTROL_AREA_BACKGROUND	= 0
		, UI_CONTROL_AREA_CLIENT
		, UI_CONTROL_AREA_BORDER_LEFT
		, UI_CONTROL_AREA_BORDER_TOP
		, UI_CONTROL_AREA_BORDER_RIGHT
		, UI_CONTROL_AREA_BORDER_BOTTOM
		, UI_CONTROL_AREA_TEXT_BACKGROUND
		, UI_CONTROL_AREA_TEXT_FACE
		, UI_CONTROL_AREA_MARGIN_LEFT
		, UI_CONTROL_AREA_MARGIN_TOP
		, UI_CONTROL_AREA_MARGIN_RIGHT
		, UI_CONTROL_AREA_MARGIN_BOTTOM
		, UI_CONTROL_AREA_CORNER_LEFT_TOP_TOP
		, UI_CONTROL_AREA_CORNER_LEFT_TOP_LEFT
		, UI_CONTROL_AREA_CORNER_RIGHT_TOP_TOP
		, UI_CONTROL_AREA_CORNER_RIGHT_TOP_RIGHT
		, UI_CONTROL_AREA_CORNER_LEFT_BOTTOM_LEFT
		, UI_CONTROL_AREA_CORNER_LEFT_BOTTOM_BOTTOM
		, UI_CONTROL_AREA_CORNER_RIGHT_BOTTOM_RIGHT
		, UI_CONTROL_AREA_CORNER_RIGHT_BOTTOM_BOTTOM
		, UI_CONTROL_AREA_COUNT
		};

	enum GUI_CONTROL_PALETTE
		{ GUI_CONTROL_PALETTE_NORMAL		= 0
		, GUI_CONTROL_PALETTE_DISABLED
		, GUI_CONTROL_PALETTE_HOVER
		, GUI_CONTROL_PALETTE_PRESSED
		, GUI_CONTROL_PALETTE_SELECTED
		, GUI_CONTROL_PALETTE_SELECTED_DISABLED
		, GUI_CONTROL_PALETTE_SELECTED_HOVER
		, GUI_CONTROL_PALETTE_SELECTED_PRESSED
		, GUI_CONTROL_PALETTE_EXECUTE
		, GUI_CONTROL_PALETTE_OUTDATED
		, GUI_CONTROL_PALETTE_COUNT
		};

	struct SControlImage {
		::gpk::img<::gpk::bgra>			Temp			= {};
	};

	struct SControl {
		::gpk::rect2i16					Area			= {{}, {16, 16}};
		::gpk::rect<uint16_t>	Margin			= {1, 1, 1, 1};
		::gpk::rect<uint8_t>		Border			= {1, 1, 1, 1};
		::gpk::view2d<::gpk::bgra>		Image			= {};
		::gpk::n2i16					ImageOffset		= {};
		::gpk::ALIGN					ImageAlign		= ::gpk::ALIGN_CENTER;

		::gpk::ALIGN					Align			= ::gpk::ALIGN_TOP_LEFT;
		int16_t							ColorTheme		= 0;
		::gpk::asti32<::gpk::GUI_CONTROL_PALETTE_COUNT>	
										Palettes		= {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9,}};
		int32_t							Parent			= -1;
	};

	// The large amoutn of pointless casts written in this function is because idiots can't handle C types so some other retards decided to add this stupid rule into the standard .
	cnstxpr	void				controlNCSpacing	(const ::gpk::SControl & ctl, ::gpk::recti16 & ncSpacing)	noexcept	{ ncSpacing = {(int16_t)(ctl.Border.Left + ctl.Margin.Left), (int16_t)(ctl.Border.Top + ctl.Margin.Top), (int16_t)(ctl.Border.Right + ctl.Margin.Right), (int16_t)(ctl.Border.Bottom + ctl.Margin.Bottom)};	 }
	cnstxpr	void				controlNCSpacing	(const ::gpk::SControl & ctl, ::gpk::n2i16 & ncSpacing)		noexcept	{ ncSpacing = {(int16_t)(ctl.Border.Left + ctl.Margin.Left + ctl.Border.Right + ctl.Margin.Right), (int16_t)(ctl.Border.Top + ctl.Margin.Top + ctl.Border.Bottom + ctl.Margin.Bottom)}; }
	cnstxpr	::gpk::recti16		controlNCRect		(const ::gpk::SControl & ctl)	noexcept	{ return {(int16_t)(ctl.Border.Left + ctl.Margin.Left), (int16_t)(ctl.Border.Top + ctl.Margin.Top), (int16_t)(ctl.Border.Right + ctl.Margin.Right), (int16_t)(ctl.Border.Bottom + ctl.Margin.Bottom)};	 }
	cnstxpr	::gpk::n2i16		controlNCSpacing	(const ::gpk::SControl & ctl)	noexcept	{ return {(int16_t)(ctl.Border.Left + ctl.Margin.Left + ctl.Border.Right + ctl.Margin.Right), (int16_t)(ctl.Border.Top + ctl.Margin.Top + ctl.Border.Bottom + ctl.Margin.Bottom)}; }

	struct SGUIZoom {
		::gpk::n2f64			DPI					= {1.0, 1.0};
		::gpk::minmax<double>	ZoomLimits			= {0.1, 10.0};
		double					ZoomLevel			= 1.0;

		bool					operator==			(const ::gpk::SGUIZoom & other)		const	noexcept	{
			return DPI			== other.DPI
				&& ZoomLevel	== other.ZoomLevel
				;
		}
		inline	bool			operator!=			(const ::gpk::SGUIZoom & other)		const	noexcept	{ return !operator==(other); }
	};

	struct SControlText {
		::gpk::vcs				Text				= {};
		::gpk::ALIGN			Align				= ::gpk::ALIGN_CENTER;
		::gpk::minmax<uint32_t>	Selection			= {0, 1};
		int16_t					FontSelected		= -1;
	};

	struct SControlTheme {
		::gpk::astatic<::gpk::astu32<::gpk::UI_CONTROL_AREA_COUNT>, ::gpk::GUI_CONTROL_PALETTE_COUNT>							
								ColorCombos			= {};
	};
#pragma pack(pop)
	struct SGUIControlTable {
		::gpk::apod<::gpk::SControl				>	Controls		= {};
		::gpk::apod<::gpk::SControlState		>	States			= {};
		::gpk::apod<::gpk::SControlMetrics		>	Metrics			= {};
		::gpk::aobj<::gpk::SControlText			>	Text			= {};
		::gpk::apod<::gpk::SControlConstraints	>	Constraints		= {};
		::gpk::aobj<::gpk::SControlMode			>	Modes			= {};
		::gpk::aobj<::gpk::apod<int32_t>		>	Children		= {};
		::gpk::aobj<::gpk::SControlImage		>	Images			= {};
		::gpk::apod<::gpk::SSysEvent			>	Events			= {};
	};

	struct SGUIColors {
		::gpk::papod<::gpk::bgra>			Palette					= {};
		::gpk::papod<::gpk::SControlTheme>	ControlThemes			= {};

		::gpk::asti32<::gpk::GUI_CONTROL_PALETTE_COUNT>	
											DefaultColors			= {};

		::gpk::apod<::gpk::astatic<::gpk::bgra, ::gpk::UI_CONTROL_AREA_COUNT>
		>									Palettes;
	};

	struct SGUI {
		::gpk::n2u16						LastSize			= {};
		::gpk::n2f32						CursorPos			= {};
		::gpk::SGUIControlTable				Controls			= {};
		::gpk::pobj<::gpk::SGUIColors>		Colors;
		// Font
		::gpk::apobj<::gpk::SRasterFont>	Fonts				= {};

		::gpk::SGUIZoom						LastZoom			= {};
		::gpk::SGUIZoom						Zoom				= {};
		uint32_t							SelectedFont		= 7;
		uint32_t							ThemeDefault		= ::gpk::ASCII_COLOR_CYAN * 16 + 8;
		::gpk::GUI_COLOR_MODE				ColorModeDefault	= ::gpk::GUI_COLOR_MODE_3D;//::gpk::GUI_COLOR_MODE_DEFAULT;
		bool								InputModeTouch		= true;
	};

#pragma pack(push, 1)
		struct SControlParams {
			::gpk::GUI_COLOR_MODE			ColorMode		;
			int32_t							Id				;
			int32_t							IdParent		;
			bool							Design			;
			bool							NoBackground	;
			bool							NoHoverEffect	;
			bool							FrameOut		;
			::gpk::ALIGN					Align			;
			::gpk::rect2i16					Area			;
			::gpk::rect<uint8_t>		Border			;
			::gpk::rect<uint16_t>	Margin			;
			::gpk::vcs						Text			;
			::gpk::ALIGN					TextAlign		;
			::gpk::ALIGN					IconAlign		;
			::gpk::vcs						IconPathOff		;
			::gpk::vcs						IconPathOn		;
			::gpk::n2<bool>					AdjustToText	;
		};
#pragma pack(pop)

	::gpk::error_t			guiProcessInput					(::gpk::SGUI & gui, const ::gpk::SInput & input, ::gpk::view<const ::gpk::SSysEvent> in_Events = {});
	::gpk::error_t			guiUpdateMetrics				(::gpk::SGUI & gui, const ::gpk::n2u16 & targetSize, bool forceUpdate);
	::gpk::error_t			guiDraw							(::gpk::SGUI & gui, ::gpk::v2<::gpk::bgra> target);
	::gpk::error_t			guiGetProcessableControls		(const ::gpk::SGUI & gui, ::gpk::au32 & controlIndices);
	::gpk::error_t			guiDeselect						(::gpk::SGUI & gui);
	::gpk::error_t			guiSelect						(::gpk::SGUI & gui, int32_t controlToSelect);

	::gpk::error_t			controlDelete					(::gpk::SGUI & gui, int32_t iControl, bool recursive = true);
	::gpk::error_t			controlCreate					(::gpk::SGUI & gui);
	::gpk::error_t			controlCreateChild				(::gpk::SGUI & gui, int32_t iParent);
	::gpk::error_t			controlSetParent				(::gpk::SGUI & gui, int32_t iControl, int32_t iParent);
	::gpk::error_t			controlDrawHierarchy			(::gpk::SGUI & gui, int32_t iControl, ::gpk::v2<::gpk::bgra> target);
	::gpk::error_t			controlTextSet					(::gpk::SGUI & gui, int32_t iControl, const ::gpk::vcs & text);
	::gpk::error_t			controlFontSet					(::gpk::SGUI & gui, int32_t iControl, int16_t iFont);
	//::gpk::error_t			controlUpdateMetrics			(::gpk::SGUI & gui, int32_t iControl, const ::gpk::n2<uint32_t> & targetSize);
	::gpk::error_t			controlUpdateMetricsTopToDown	(::gpk::SGUI & gui, int32_t iControl, const ::gpk::n2u16 & targetSize, bool forceUpdate);
	::gpk::error_t			controlMetricsInvalidate		(::gpk::SGUI & gui, int32_t iControl);
	::gpk::error_t			controlHidden					(const ::gpk::SGUI & gui, int32_t iControl);
	::gpk::error_t			controlDisabled					(const ::gpk::SGUI & gui, int32_t iControl);
	::gpk::error_t			controlInvalid					(const ::gpk::SGUI & gui, int32_t iControl);


	::gpk::error_t			guiProcessControls				(const ::gpk::SGUI & gui, const ::std::function<::gpk::error_t(int32_t iControl)> & funcOnExecute);
	::gpk::error_t			guiProcessControls				(const ::gpk::SGUI & gui, ::gpk::vcu32 controlsToProcess, const ::std::function<::gpk::error_t(uint32_t iControl)> & funcOnExecute);
} // namespace

#endif // GPK_GUI_H_2903874982374
