#include "gpk_array.h"
#include "gpk_color.h"
#include "gpk_input.h"
#include "gpk_view_grid.h"
#include "gpk_image.h"
#include "gpk_array_static.h"
#include "gpk_ascii_color.h"

#ifndef GPK_GUI_H_2903874982374
#define GPK_GUI_H_2903874982374

namespace gpk
{
#pragma pack(push, 1)
	enum GUI_COLOR_MODE : uint8_t
		{ GUI_COLOR_MODE_DEFAULT				= 0
		, GUI_COLOR_MODE_3D
		, GUI_COLOR_MODE_FLAT
		, GUI_COLOR_MODE_COUNT
		};

	struct SControlMode {
		uint8_t									ColorMode								: 2;
		uint8_t									UseNewPalettes							: 1;
		uint8_t									NoHoverEffect							: 1;
		uint8_t									FrameOut								: 1;
		uint8_t									Design									: 1;
		uint8_t									NoBackgroundRect						: 1;
	};

	struct SControlRectangle {
		::gpk::SRectangle2<int16_t>				Local;
		::gpk::SRectangle2<int16_t>				Global;
	};

	enum GUI_CONTROL_AREA : uint8_t
		{ GUI_CONTROL_AREA_BACKGROUND			= 0
		, GUI_CONTROL_AREA_CLIENT
		, GUI_CONTROL_AREA_BORDER_LEFT
		, GUI_CONTROL_AREA_BORDER_TOP
		, GUI_CONTROL_AREA_BORDER_RIGHT
		, GUI_CONTROL_AREA_BORDER_BOTTOM
		, GUI_CONTROL_AREA_COUNT
		};

	struct SControlMetrics {
		::gpk::SControlRectangle				Total;
		::gpk::SControlRectangle				Client;
		::gpk::SRectangle2<int16_t>				Text;
		::gpk::SRectangle2<int16_t>				Rectangles	[::gpk::GUI_CONTROL_AREA_COUNT]	= {};
		::gpk::STriangle2<int16_t>				Triangles	[8]								= {};
	};

	struct SControlAttachId {
		int32_t									IdControl								: 31;
		int32_t									Total									: 1;
	};

	struct SControlConstraints {
		::gpk::SCoord2<int32_t>					AttachSizeToControl						;
		::gpk::SCoord2<bool>					AttachSizeToText						;
		::gpk::SRectLimits<int32_t>				DockToControl							;
		::gpk::SMinMax<::gpk::SCoord2<int16_t>>	SizeMinMax								= {{}, {0x7FFF, 0x7FFF}};
	};

	struct SControlState {
		bool									Disabled								: 1;
		bool									Hover									: 1;
		bool									UnHover									: 1;
		bool									Pressed									: 1;
		bool									Released								: 1;
		bool									Selected								: 1;
		bool									Execute									: 1;
		bool									Updated									: 1;

		bool									Hidden									: 1;
		bool									ImageInvertY							: 1;
		bool									Unused									: 1;
	};

	enum GUI_CONTROL_COLOR : uint8_t
		{ GUI_CONTROL_COLOR_BACKGROUND			= 0
		, GUI_CONTROL_COLOR_BORDER_LEFT
		, GUI_CONTROL_COLOR_BORDER_TOP
		, GUI_CONTROL_COLOR_BORDER_RIGHT
		, GUI_CONTROL_COLOR_BORDER_BOTTOM
		, GUI_CONTROL_COLOR_TEXT_BACKGROUND
		, GUI_CONTROL_COLOR_TEXT_FACE
		, GUI_CONTROL_COLOR_MARGIN_LEFT
		, GUI_CONTROL_COLOR_MARGIN_TOP
		, GUI_CONTROL_COLOR_MARGIN_RIGHT
		, GUI_CONTROL_COLOR_MARGIN_BOTTOM
		, GUI_CONTROL_COLOR_CORNER_LEFT_TOP_TOP
		, GUI_CONTROL_COLOR_CORNER_LEFT_TOP_LEFT
		, GUI_CONTROL_COLOR_CORNER_RIGHT_TOP_TOP
		, GUI_CONTROL_COLOR_CORNER_RIGHT_TOP_RIGHT
		, GUI_CONTROL_COLOR_CORNER_LEFT_BOTTOM_LEFT
		, GUI_CONTROL_COLOR_CORNER_LEFT_BOTTOM_BOTTOM
		, GUI_CONTROL_COLOR_CORNER_RIGHT_BOTTOM_RIGHT
		, GUI_CONTROL_COLOR_CORNER_RIGHT_BOTTOM_BOTTOM
		, GUI_CONTROL_COLOR_CLIENT
		, GUI_CONTROL_COLOR_COUNT
		};

	enum GUI_CONTROL_PALETTE
		{ GUI_CONTROL_PALETTE_NORMAL			= 0
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
		::gpk::SImage<::gpk::SColorBGRA>								Temp								= {};
	};

	struct SControl {
		::gpk::SRectangle2<int16_t>										Area								= {{}, {16, 16}};
		::gpk::SRectLimits<uint16_t>									Margin								= {1, 1, 1, 1};
		::gpk::SRectLimits<uint8_t>										Border								= {1, 1, 1, 1};
		::gpk::view_grid<::gpk::SColorBGRA>								Image								= {};
		::gpk::SCoord2<int16_t>											ImageOffset							= {};
		::gpk::ALIGN													ImageAlign							= ::gpk::ALIGN_CENTER;

		::gpk::ALIGN													Align								= ::gpk::ALIGN_TOP_LEFT;
		int16_t															ColorTheme							= 0;
		::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_PALETTE_COUNT>	Palettes							= {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9,}};
		int32_t															IndexParent							= -1;
	};

	// The large amoutn of pointless casts written in this function is because idiots can't handle C types so some other retards decided to add this stupid rule into the standard .
	constexpr	void												controlNCSpacing					(const ::gpk::SControl& ctl, ::gpk::SRectLimits	<int16_t> & ncSpacing)	noexcept	{ ncSpacing = {(int16_t)(ctl.Border.Left + ctl.Margin.Left), (int16_t)(ctl.Border.Top + ctl.Margin.Top), (int16_t)(ctl.Border.Right + ctl.Margin.Right), (int16_t)(ctl.Border.Bottom + ctl.Margin.Bottom)};	 }
	constexpr	void												controlNCSpacing					(const ::gpk::SControl& ctl, ::gpk::SCoord2		<int16_t> & ncSpacing)	noexcept	{ ncSpacing = {(int16_t)(ctl.Border.Left + ctl.Margin.Left + ctl.Border.Right + ctl.Margin.Right), (int16_t)(ctl.Border.Top + ctl.Margin.Top + ctl.Border.Bottom + ctl.Margin.Bottom)}; }
	constexpr	::gpk::SRectLimits	<int16_t>						controlNCRect						(const ::gpk::SControl& ctl)											noexcept	{ return {(int16_t)(ctl.Border.Left + ctl.Margin.Left), (int16_t)(ctl.Border.Top + ctl.Margin.Top), (int16_t)(ctl.Border.Right + ctl.Margin.Right), (int16_t)(ctl.Border.Bottom + ctl.Margin.Bottom)};	 }
	constexpr	::gpk::SCoord2		<int16_t>						controlNCSpacing					(const ::gpk::SControl& ctl)											noexcept	{ return {(int16_t)(ctl.Border.Left + ctl.Margin.Left + ctl.Border.Right + ctl.Margin.Right), (int16_t)(ctl.Border.Top + ctl.Margin.Top + ctl.Border.Bottom + ctl.Margin.Bottom)}; }

	struct SGUIZoom {
		::gpk::SCoord2<double>											DPI									= {1.0, 1.0};
		::gpk::SMinMax<double>											ZoomLimits							= {0.1, 10.0};
		double															ZoomLevel							= 1.0;

		bool															operator ==							(const ::gpk::SGUIZoom& other)		const	noexcept	{
			return DPI			== other.DPI
				&& ZoomLevel	== other.ZoomLevel
				;
		}
		inline	bool													operator!=							(const ::gpk::SGUIZoom & other)		const	noexcept	{ return !operator==(other); }
	};

	struct SControlText {
		::gpk::view_const_string										Text								= {};
		::gpk::ALIGN													Align								= ::gpk::ALIGN_CENTER;
		::gpk::SMinMax<uint32_t>										Selection							= {0, 1};
	};

	struct SControlTheme {
		::gpk::array_static
			<::gpk::array_static
				<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>
			, ::gpk::GUI_CONTROL_PALETTE_COUNT>							ColorCombos							= {};
	};
	struct SGUIControlTable {
		::gpk::array_pod<::gpk::SControl			>					Controls							= {};
		::gpk::array_pod<::gpk::SControlState		>					States								= {};
		::gpk::array_pod<::gpk::SControlMetrics		>					Metrics								= {};
		::gpk::array_obj<::gpk::SControlText		>					Text								= {};
		::gpk::array_pod<::gpk::SControlConstraints	>					Constraints							= {};
		::gpk::array_obj<::gpk::SControlMode		>					Modes								= {};
		::gpk::array_obj<::gpk::array_pod<int32_t>	>					Children							= {};
		::gpk::array_obj<::gpk::SControlImage		>					Images								= {};
	};
#pragma pack(pop)

	struct SGUIFont {
		::gpk::SCoord2<uint8_t>											CharSize;
		::gpk::SImageMonochrome<uint32_t>								Texture	;
	};

	struct SGUI {
		::gpk::SCoord2<uint32_t>										LastSize							= {};
		::gpk::SCoord2<float>											CursorPos							= {};
		::gpk::SGUIControlTable											Controls							= {};
		::gpk::array_pod<::gpk::SColorBGRA>								Palette								= {};
		::gpk::array_pod<::gpk::SControlTheme>							ControlThemes						= {};

		::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_PALETTE_COUNT>	DefaultColors	= {};

		::gpk::array_pod<
			::gpk::array_static<::gpk::SColorBGRA, ::gpk::GUI_CONTROL_COLOR_COUNT>
		>																Palettes;

		// Font
		::gpk::array_obj<::gpk::SGUIFont>								Fonts								= {};
		::gpk::SImageMonochrome<uint32_t>								FontTexture							= {};
		::gpk::SCoord2<uint16_t>										FontCharSize						= {9, 16};

		::gpk::SGUIZoom													LastZoom							= {};
		::gpk::SGUIZoom													Zoom								= {};
		uint32_t														ThemeDefault						= 0;
		::gpk::GUI_COLOR_MODE											ColorModeDefault					= ::gpk::GUI_COLOR_MODE_FLAT;//::gpk::GUI_COLOR_MODE_DEFAULT;
		bool															InputModeTouch						= true;

																		SGUI								();
	};

	::gpk::error_t											guiProcessInput						(::gpk::SGUI& gui, ::gpk::SInput& input);
	::gpk::error_t											guiUpdateMetrics					(::gpk::SGUI& gui, const ::gpk::SCoord2<uint32_t> & targetSize, bool forceUpdate);
	::gpk::error_t											guiDraw								(::gpk::SGUI& gui, ::gpk::view_grid<::gpk::SColorBGRA>& target);
	::gpk::error_t											guiGetProcessableControls			(::gpk::SGUI& gui, ::gpk::array_pod<uint32_t>& controlIndices);
	::gpk::error_t											guiDeselect							(::gpk::SGUI& gui);
	::gpk::error_t											guiSelect							(::gpk::SGUI& gui, int32_t controlToSelect);

	::gpk::error_t											controlDelete						(::gpk::SGUI& gui, int32_t iControl, bool recursive = true);
	::gpk::error_t											controlCreate						(::gpk::SGUI& gui);
	::gpk::error_t											controlCreateChild					(::gpk::SGUI& gui, int32_t iParent);
	::gpk::error_t											controlSetParent					(::gpk::SGUI& gui, int32_t iControl, int32_t iParent);
	::gpk::error_t											controlDrawHierarchy				(::gpk::SGUI& gui, int32_t iControl, ::gpk::view_grid<::gpk::SColorBGRA>& target);
	::gpk::error_t											controlTextSet						(::gpk::SGUI& gui, int32_t iControl, const ::gpk::view_const_string& text);
	//::gpk::error_t											controlUpdateMetrics				(::gpk::SGUI& gui, int32_t iControl, const ::gpk::SCoord2<uint32_t> & targetSize);
	::gpk::error_t											controlUpdateMetricsTopToDown		(::gpk::SGUI& gui, int32_t iControl, const ::gpk::SCoord2<uint32_t> & targetSize, bool forceUpdate);
	::gpk::error_t											controlMetricsInvalidate			(::gpk::SGUI& gui, int32_t iControl);
	::gpk::error_t											controlHidden						(const ::gpk::SGUI& gui, int32_t iControl);
	::gpk::error_t											controlDisabled						(const ::gpk::SGUI& gui, int32_t iControl);
	::gpk::error_t											controlInvalid						(const ::gpk::SGUI& gui, int32_t iControl);

} // namespace

#endif // GPK_GUI_H_2903874982374
