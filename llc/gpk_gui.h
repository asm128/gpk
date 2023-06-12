#include "gpk_input.h"
#include "gpk_array_static.h"
#include "gpk_ascii_color.h"
#include "gpk_font.h"
#include "gpk_tri2.h"
#include "gpk_rect.h"
#include "gpk_align.h"

#include "gpk_sysevent.h"

#include "gpk_image_color.h"


#ifndef GPK_GUI_H_2903874982374
#define GPK_GUI_H_2903874982374

namespace gpk
{
#pragma pack(push, 1)
	typedef	int32_t		cid_t;

	typedef	n2<cid_t>			n2cid;
	typedef	rect<cid_t>			rectcid;
	typedef	apod<cid_t>			acid;
	typedef	view<cid_t>			vcid;
	typedef	view<const cid_t>	vccid;

	enum GUI_COLOR_MODE : uint8_t
		{ GUI_COLOR_MODE_Default	= 0
		, GUI_COLOR_MODE_3D
		, GUI_COLOR_MODE_Flat
		, GUI_COLOR_MODE_COUNT
		};

	// Runtime internal states
	struct SControlState {
		bool				Updated			: 1;
		bool				Unused			: 1;
	};

	// Holds the state changes after an update call. 
	struct SControlEvent {
		bool				Hover				: 1;
		bool				UnHover				: 1;
		bool				Pressed				: 1;
		bool				Released			: 1;
		bool				Focused				: 1;
		bool				DeFocused			: 1;
		bool				Selected			: 1;
		bool				Execute				: 1;
	};

	struct SControlMode {
		uint8_t				NoHover				: 1;
		uint8_t				NoFocus				: 1;
		uint8_t				NoExecute			: 1;
		uint8_t				NoSelect			: 1;
		bool				Disabled			: 1;
		bool				Hidden				: 1;
	};

	struct SControlDraw {
		uint8_t				ColorMode			: 2;
		uint8_t				FrameOut			: 1;
		uint8_t				NoBorder			: 1;	// skip drawing border area
		uint8_t				NoClient			: 1;	// skip drawing client area
		uint8_t				StatePalette		: 3;
		
		uint8_t				UseNewPalettes		: 1;	// -- Legacy property (don't use on the new implementation!)
		uint8_t				Design				: 1;	// -- Legacy property (don't use on the new implementation!)
		uint8_t				NoBackgroundRect	: 1;	// -- Legacy property (don't use on the new implementation!)
	};

	struct SRelativeBit {
		uint8_t				XOffset			: 1; //
		uint8_t				YOffset			: 1; //
		uint8_t				XSize			: 1; //
		uint8_t				YSize			: 1; //
		uint8_t				BorderLeft		: 1; //
		uint8_t				BorderTop		: 1; //
		uint8_t				BorderRight		: 1; //
		uint8_t				BorderBottom	: 1; //
	};

	struct SControlRectangle {
		::gpk::rect2i16		Local;
		::gpk::rect2i16		Global;
	};

	enum CONTROL_AREA_BASE : uint8_t 
		{ CONTROL_AREA_BASE_BACKGROUND				= 0
		, CONTROL_AREA_BASE_CONTENT
		, CONTROL_AREA_BASE_BORDER_LEFT
		, CONTROL_AREA_BASE_BORDER_TOP
		, CONTROL_AREA_BASE_BORDER_RIGHT
		, CONTROL_AREA_BASE_BORDER_BOTTOM
		, CONTROL_AREA_BASE_CORNER_TOP_LEFT
		, CONTROL_AREA_BASE_CORNER_RIGHT_TOP
		, CONTROL_AREA_BASE_CORNER_BOTTOM_RIGHT
		, CONTROL_AREA_BASE_CORNER_LEFT_BOTTOM
		, CONTROL_AREA_BASE_COUNT
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

	struct SControlMetrics {
		::gpk::SControlRectangle	Total		= {};
		::gpk::SControlRectangle	Client		= {};
		::gpk::rect2i16				Text		= {};
		::gpk::rect2i16				Rectangles	[::gpk::GUI_CONTROL_AREA_OLD_COUNT]	= {};
		::gpk::tri2i16				Triangles	[8]		= {};
	};

	struct SControlAttachId {
		cid_t						IdControl		: 31;
		cid_t						Total			: 1;
	};

	struct SControlConstraints {
		::gpk::n2cid				AttachSizeToControl	= {-1, -1};
		::gpk::n2<bool>				AttachSizeToText	= {};
		::gpk::rectcid				DockToControl		= {-1, -1, -1, -1};
		::gpk::minmax<::gpk::n2i16>	SizeMinMax			= {{}, {0x7FFF, 0x7FFF}};
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
		::gpk::img8bgra			Temp			= {};
	};

	struct SControl {
		::gpk::rect2i16			Area			= {{}, {16, 16}};
		::gpk::rectu16			Margin			= {1, 1, 1, 1};
		::gpk::rectu8			Border			= {1, 1, 1, 1};
		::gpk::v2bgra			Image			= {};
		::gpk::n2i16			ImageOffset		= {};
		::gpk::ALIGN			ImageAlign		= ::gpk::ALIGN_CENTER;
		bool					ImageInvertY	= false;

		::gpk::ALIGN			Align			= ::gpk::ALIGN_TOP_LEFT;
		int16_t					ColorTheme		= 0;
		::gpk::asti32<::gpk::GUI_CONTROL_PALETTE_COUNT>	
								Palettes		= {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9,}};
		cid_t					Parent			= -1;
	};

	// The large amoutn of pointless casts written in this function is because idiots can't handle C types so some other retards decided to add this stupid rule into the standard .
	stincxp	void			controlNCSpacing	(const ::gpk::SControl & ctl, ::gpk::recti16 & ncSpacing)	noexcept	{ ncSpacing = {(int16_t)(ctl.Border.Left + ctl.Margin.Left), (int16_t)(ctl.Border.Top + ctl.Margin.Top), (int16_t)(ctl.Border.Right + ctl.Margin.Right), (int16_t)(ctl.Border.Bottom + ctl.Margin.Bottom)};	 }
	stincxp	void			controlNCSpacing	(const ::gpk::SControl & ctl, ::gpk::n2i16 & ncSpacing)		noexcept	{ ncSpacing = {(int16_t)(ctl.Border.Left + ctl.Margin.Left + ctl.Border.Right + ctl.Margin.Right), (int16_t)(ctl.Border.Top + ctl.Margin.Top + ctl.Border.Bottom + ctl.Margin.Bottom)}; }
	stincxp	::gpk::recti16	controlNCRect		(const ::gpk::SControl & ctl)	noexcept	{ return {(int16_t)(ctl.Border.Left + ctl.Margin.Left), (int16_t)(ctl.Border.Top + ctl.Margin.Top), (int16_t)(ctl.Border.Right + ctl.Margin.Right), (int16_t)(ctl.Border.Bottom + ctl.Margin.Bottom)};	 }
	stincxp	::gpk::n2i16	controlNCSpacing	(const ::gpk::SControl & ctl)	noexcept	{ return {(int16_t)(ctl.Border.Left + ctl.Margin.Left + ctl.Border.Right + ctl.Margin.Right), (int16_t)(ctl.Border.Top + ctl.Margin.Top + ctl.Border.Bottom + ctl.Margin.Bottom)}; }

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

	struct SControlText {
		::gpk::vcs			Text				= {};
		::gpk::ALIGN		Align				= ::gpk::ALIGN_CENTER;
		::gpk::minmaxu16	Selection			= {0, 1};
		int16_t				FontSelected		= -1;
	};

	struct SControlTheme {
		::gpk::astatic<::gpk::astu32<::gpk::UI_CONTROL_AREA_COUNT>, ::gpk::GUI_CONTROL_PALETTE_COUNT>							
							ColorCombos			= {};
	};
#pragma pack(pop)
	struct SGUIControlTable {
		::gpk::apod<::gpk::SControl				>	Controls		= {};
		::gpk::apod<::gpk::SControlMetrics		>	Metrics			= {};
		::gpk::apod<::gpk::SControlConstraints	>	Constraints		= {};
		::gpk::apod<::gpk::SControlState		>	States			= {};
		::gpk::apod<::gpk::SControlMode			>	Modes			= {};
		::gpk::apod<::gpk::SControlDraw			>	Draw			= {};
		::gpk::apod<::gpk::SControlEvent		>	Events			= {};
		::gpk::apod<::gpk::SRelativeBit			>	RelativeBit		= {};

		::gpk::aobj<::gpk::SControlText			>	Text			= {};
		::gpk::aobj<::gpk::acid					>	Children		= {};
		::gpk::aobj<::gpk::SControlImage		>	Images			= {};

		::gpk::apod<::gpk::SSysEvent			>	EventQueue		= {};
	};
	::gpk::error_t			controlCreate					(::gpk::SGUIControlTable & gui, ::gpk::vi32 defaultColors);

	struct SGUIColors {
		::gpk::papod<::gpk::bgra>			Palette					= {};
		::gpk::papod<::gpk::SControlTheme>	ControlThemes			= {};

		::gpk::asti32<::gpk::GUI_CONTROL_PALETTE_COUNT>	
											DefaultColors			= {};

		::gpk::apod<::gpk::astatic<::gpk::bgra, ::gpk::UI_CONTROL_AREA_COUNT>>
											Palettes;
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

	::gpk::error_t			guiProcessInput					(::gpk::SGUI & gui, const ::gpk::SInput & input, ::gpk::view<const ::gpk::SSysEvent> in_Events = {});
	::gpk::error_t			guiUpdateMetrics				(::gpk::SGUI & gui, const ::gpk::n2u16 & targetSize, bool forceUpdate);
	::gpk::error_t			guiDraw							(::gpk::SGUI & gui, ::gpk::v2bgra target);
	::gpk::error_t			guiGetProcessableControls		(const ::gpk::SGUI & gui, ::gpk::au32 & controlIndices);
	::gpk::error_t			guiDeselect						(::gpk::SGUI & gui);
	::gpk::error_t			guiSelect						(::gpk::SGUI & gui, int32_t controlToSelect);

	::gpk::error_t			controlDelete					(::gpk::SGUI & gui, int32_t iControl, bool recursive = true);
	::gpk::error_t			controlCreate					(::gpk::SGUI & gui);
	::gpk::error_t			controlCreateChild				(::gpk::SGUI & gui, int32_t iParent);
	::gpk::error_t			controlSetParent				(::gpk::SGUI & gui, int32_t iControl, int32_t iParent);
	::gpk::error_t			controlDrawHierarchy			(::gpk::SGUI & gui, int32_t iControl, ::gpk::v2bgra target);
	::gpk::error_t			controlTextSet					(::gpk::SGUI & gui, int32_t iControl, const ::gpk::vcs & text);
	::gpk::error_t			controlFontSet					(::gpk::SGUI & gui, int32_t iControl, int16_t iFont);
	//::gpk::error_t			controlUpdateMetrics			(::gpk::SGUI & gui, int32_t iControl, const ::gpk::n2<uint32_t> & targetSize);
	::gpk::error_t			controlUpdateMetricsTopToDown	(::gpk::SGUI & gui, int32_t iControl, const ::gpk::n2u16 & targetSize, bool forceUpdate);
	::gpk::error_t			controlMetricsInvalidate		(::gpk::SGUI & gui, int32_t iControl);
	::gpk::error_t			controlHidden					(const ::gpk::SGUI & gui, int32_t iControl);
	::gpk::error_t			controlDisabled					(const ::gpk::SGUI & gui, int32_t iControl);
	::gpk::error_t			controlInvalid					(const ::gpk::SGUI & gui, int32_t iControl);


	::gpk::error_t			guiProcessControls				(const ::gpk::SGUI & gui, const ::std::function<::gpk::error_t(uint32_t iControl)> & funcOnExecute);
	::gpk::error_t			guiProcessControls				(const ::gpk::SGUI & gui, ::gpk::vcu32 controlsToProcess, const ::std::function<::gpk::error_t(uint32_t iControl)> & funcOnExecute);


} // namespace

#endif // GPK_GUI_H_2903874982374
