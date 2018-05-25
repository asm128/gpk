#include "gpk_array.h"
#include "gpk_color.h"
#include "gpk_input.h"
#include "gpk_grid_view.h"
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
	};
	struct SControlState {
		bool												Hover									: 1;
		bool												Pressed									: 1;
		bool												Selected								: 1;
		bool												Execute									: 1;
		bool												Disabled								: 1;
		bool												Unused									: 1;
		bool												Outdated								: 1;
	};

	struct SControl {
		::gpk::SRectangle2D<int32_t>						Area								;
		::gpk::SRectLimits<int32_t>							Border								;
		::gpk::SRectLimits<int32_t>							Margin								;
		int32_t												IndexParent							;
	};

	struct SGUIZoom {
		::gpk::SCoord2<double>								DPI									= {1.0, 1.0};
		::gpk::SMinMax<double>								ZoomLimits							= {0.1, 10.0};
		float												ZoomLevel							= 1.0;
	};

	struct SGUI {
		::gpk::array_pod<SControl>							Controls							= {};
		::gpk::array_pod<SControlState>						ControlStates						= {};
		::gpk::array_pod<SControlMetrics>					ControlMetrics						= {};
		::gpk::array_obj<::gpk::array_pod<int32_t>>			ControlChildren						= {};
		::gpk::SGUIZoom										Zoom								= {};
	};

	::gpk::error_t										controlCreate						(::gpk::SGUI& gui);
	::gpk::error_t										controlDelete						(::gpk::SGUI& gui, int32_t iControl);

	::gpk::error_t										controlSetParent					(::gpk::SGUI& gui, int32_t iControl, int32_t iParent);
	::gpk::error_t										controlPaintHierarchy				(::gpk::SGUI& gui, int32_t iControl, ::gpk::grid_view<::gpk::SColorBGRA>& target);

#pragma pack(pop)
} // namespace

#endif // GPK_GUI_H_2903874982374

