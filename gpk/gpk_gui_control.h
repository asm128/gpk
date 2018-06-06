#include "gpk_gui.h"

#ifndef GPK_GUI_CONTROL_H_2903749802374
#define GPK_GUI_CONTROL_H_2903749802374

namespace gpk
{
	enum CONTROL_LIST_DIRECTION : int32_t 
		{	CONTROL_LIST_DIRECTION_HORIZONTAL	= 1
		,	CONTROL_LIST_DIRECTION_VERTICAL		= 2
		};

	struct SControlList {
			int32_t											IdControl								= -1;
			CONTROL_LIST_DIRECTION							Orientation								= ::gpk::CONTROL_LIST_DIRECTION_HORIZONTAL;
			::gpk::array_pod<int32_t>						IdControls								= {};
			::gpk::array_pod<int32_t>						IdSelected								= {};
	};

		::gpk::error_t									controlListInitialize		(::gpk::SGUI& gui, ::gpk::SControlList& menu);
		::gpk::error_t									controlListPush				(::gpk::SGUI& gui, ::gpk::SControlList& menu, const ::gpk::view_const_string& text);
}

#endif