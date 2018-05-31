#include "gpk_array.h"
#include "gpk_color.h"
#include "gpk_input.h"
#include "gpk_grid_view.h"
#include "gpk_texture.h"
#include "gpk_array_static.h"
#include <string>

#ifndef GPK_GUI_H_2903874982374
#define GPK_GUI_H_2903874982374

namespace gpk
{
#pragma pack(push, 1)
	struct SControlRectangle {
		::gpk::SRectangle2D<int32_t>						Local;
		::gpk::SRectangle2D<int32_t>						Global;
	};

	struct SControlMetrics {
		::gpk::SControlRectangle							Total;
		::gpk::SControlRectangle							Client;
		::gpk::SControlRectangle							Text;
	};

	struct SControlConstraints {
		int32_t												IndexControlToAttachWidthTo				;
		int32_t												IndexControlToAttachHeightTo			;
		int32_t												AttachToTextWidth						;
		int32_t												AttachToTextHeight						;
	};


	struct SControlState {
		bool												Outdated								: 1;
		bool												Hover									: 1;
		bool												Pressed									: 1;
		bool												Selected								: 1;
		bool												Execute									: 1;
		bool												Disabled								: 1;
		bool												Unused									: 1;
	};

	enum GUI_CONTROL_AREA
		{ GUI_CONTROL_AREA_BACKGROUND		= 0
		, GUI_CONTROL_AREA_CLIENT			
		, GUI_CONTROL_AREA_BORDER_LEFT		
		, GUI_CONTROL_AREA_BORDER_TOP
		, GUI_CONTROL_AREA_BORDER_RIGHT
		, GUI_CONTROL_AREA_BORDER_BOTTOM	
		, GUI_CONTROL_AREA_COUNT
		};

	enum GUI_CONTROL_COLOR
		{ GUI_CONTROL_COLOR_BACKGROUND							= 0
		, GUI_CONTROL_COLOR_CLIENT			
		, GUI_CONTROL_COLOR_BORDER_LEFT		
		, GUI_CONTROL_COLOR_BORDER_TOP
		, GUI_CONTROL_COLOR_BORDER_RIGHT
		, GUI_CONTROL_COLOR_BORDER_BOTTOM	
		, GUI_CONTROL_COLOR_TEXT_BACKGROUND
		, GUI_CONTROL_COLOR_TEXT_FACE			
		, GUI_CONTROL_COLOR_CORNER_LEFT_TOP_TOP
		, GUI_CONTROL_COLOR_CORNER_LEFT_TOP_LEFT				
		, GUI_CONTROL_COLOR_CORNER_RIGHT_TOP_TOP
		, GUI_CONTROL_COLOR_CORNER_RIGHT_TOP_RIGHT
		, GUI_CONTROL_COLOR_CORNER_LEFT_BOTTOM_LEFT	
		, GUI_CONTROL_COLOR_CORNER_LEFT_BOTTOM_BOTTOM
		, GUI_CONTROL_COLOR_CORNER_RIGHT_BOTTOM_RIGHT
		, GUI_CONTROL_COLOR_CORNER_RIGHT_BOTTOM_BOTTOM
		, GUI_CONTROL_COLOR_COUNT
		};

	struct SControl {
		::gpk::SRectangle2D<int32_t>						Area								;
		::gpk::SRectLimits<uint16_t>						Border								;
		::gpk::SRectLimits<uint16_t>						Margin								;
		::gpk::SRectLimits<uint16_t>						ColorBorder							= 
			{ ::gpk::GUI_CONTROL_AREA_BORDER_LEFT	
			, ::gpk::GUI_CONTROL_AREA_BORDER_TOP	
			, ::gpk::GUI_CONTROL_AREA_BORDER_RIGHT	
			, ::gpk::GUI_CONTROL_AREA_BORDER_BOTTOM	
			};
		uint16_t											ColorBack							= ::gpk::GUI_CONTROL_AREA_BACKGROUND;
		uint16_t											ColorClient							= ::gpk::GUI_CONTROL_AREA_CLIENT	;
		int32_t												IndexParent							;
		::gpk::ALIGN										Align								;
	};

	struct SGUIZoom {
		::gpk::SCoord2<double>								DPI									= {1.0, 1.0};
		::gpk::SMinMax<double>								ZoomLimits							= {0.1, 10.0};
		float												ZoomLevel							= 1.0;
	};

	struct SControlText {
		::std::string										Text								;
		::gpk::ALIGN										Align								;
		int8_t												Padding								;
		int16_t												ColorIndex							;
	};

	struct SGUIControlTable {
		::gpk::array_pod<::gpk::SControl			>		Controls							= {};
		::gpk::array_pod<::gpk::SControlState		>		States								= {};
		::gpk::array_pod<::gpk::SControlMetrics		>		Metrics								= {};
		::gpk::array_obj<::gpk::SControlText		>		Text								= {};
		::gpk::array_pod<::gpk::SControlConstraints	>		Constraints							= {};
		::gpk::array_obj<::gpk::array_pod<int32_t>	>		Children							= {};
	};

	struct SGUI {
		::gpk::SCoord2<float>								CursorPos							= {};
		::gpk::SGUIControlTable								Controls							= {};
		::gpk::array_pod<::gpk::SColorBGRA>					Palette								= 
			{ {0x3F, 0x3F, 0x3F, 0xFF}
			, {0x00, 0x7F, 0x7F, 0xFF}
			, {0x00, 0x00, 0xFF, 0xFF}
			, {0x00, 0xFF, 0x00, 0xFF}
			, {0xFF, 0x00, 0x00, 0xFF}
			, {0xFF, 0xFF, 0x00, 0xFF}
			};

		::gpk::array_pod
			<::gpk::array_static
				<int32_t, GUI_CONTROL_COLOR_COUNT>>			ColorCombos							= 0;

		// Font
		::gpk::STextureMonochrome<uint32_t>					FontTexture							= {};
		::gpk::SCoord2<uint32_t>							FontCharSize						= {9, 16};

		::gpk::SGUIZoom										Zoom								= {};
	};

	::gpk::error_t										guiProcessInput						(::gpk::SGUI& gui, ::gpk::SInput& input);

	::gpk::error_t										controlCreate						(::gpk::SGUI& gui);
	::gpk::error_t										controlDelete						(::gpk::SGUI& gui, int32_t iControl);

	::gpk::error_t										controlSetParent					(::gpk::SGUI& gui, int32_t iControl, int32_t iParent);
	::gpk::error_t										controlDrawHierarchy				(::gpk::SGUI& gui, int32_t iControl, ::gpk::grid_view<::gpk::SColorBGRA>& target);

#pragma pack(pop)
} // namespace

#endif // GPK_GUI_H_2903874982374

