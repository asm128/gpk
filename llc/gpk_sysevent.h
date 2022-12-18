#include "gpk_enum.h"

#ifndef GPK_SYSEVENT_H_2094098237
#define GPK_SYSEVENT_H_2094098237

namespace gpk
{
	GDEFINE_ENUM_TYPE(SYSEVENT, uint16_t);
	GDEFINE_ENUM_VALUE(SYSEVENT, WINDOW_DESTROY		,  0);
	GDEFINE_ENUM_VALUE(SYSEVENT, WINDOW_CLOSE		,  1);
	GDEFINE_ENUM_VALUE(SYSEVENT, WINDOW_MOVE		,  2);
	GDEFINE_ENUM_VALUE(SYSEVENT, WINDOW_ACTIVATE	,  3);
	GDEFINE_ENUM_VALUE(SYSEVENT, WINDOW_DEACTIVATE	,  4);
	GDEFINE_ENUM_VALUE(SYSEVENT, WINDOW_RESIZE		,  5);
	GDEFINE_ENUM_VALUE(SYSEVENT, WINDOW_REDRAW		,  6);
	GDEFINE_ENUM_VALUE(SYSEVENT, WINDOW_SHOW		,  7);
	GDEFINE_ENUM_VALUE(SYSEVENT, WINDOW_HIDE		,  8);
	GDEFINE_ENUM_VALUE(SYSEVENT, KEY_UP				,  9);
	GDEFINE_ENUM_VALUE(SYSEVENT, KEY_DOWN			, 10);
	GDEFINE_ENUM_VALUE(SYSEVENT, SYSKEY_DOWN		, 11);		
	GDEFINE_ENUM_VALUE(SYSEVENT, SYSKEY_UP			, 12);		
	GDEFINE_ENUM_VALUE(SYSEVENT, MOUSE_UP			, 13);
	GDEFINE_ENUM_VALUE(SYSEVENT, MOUSE_DOWN			, 14);
	GDEFINE_ENUM_VALUE(SYSEVENT, MOUSE_DBLCLK		, 15);	
	GDEFINE_ENUM_VALUE(SYSEVENT, MOUSE_WHEEL		, 16);	
	GDEFINE_ENUM_VALUE(SYSEVENT, MOUSE_POSITION		, 17);	
	GDEFINE_ENUM_VALUE(SYSEVENT, TOUCH_BEGIN		, 18);	
	GDEFINE_ENUM_VALUE(SYSEVENT, TOUCH_END			, 19);
	GDEFINE_ENUM_VALUE(SYSEVENT, TOUCH_UPDATE		, 20);	
	GDEFINE_ENUM_VALUE(SYSEVENT, TOUCH_OWNERSHIP	, 21);		
	GDEFINE_ENUM_VALUE(SYSEVENT, CHAR				, 22);
	GDEFINE_ENUM_VALUE(SYSEVENT, MINMAXINFO			, 23);
	GDEFINE_ENUM_VALUE(SYSEVENT, JOY_BUTTON_DOWN	, 24);
	GDEFINE_ENUM_VALUE(SYSEVENT, JOY_BUTTON_UP		, 25);
	GDEFINE_ENUM_VALUE(SYSEVENT, JOY_MOVE			, 26);

	struct SSysEvent {
		::gpk::SYSEVENT				Type;
		::gpk::array_pod<uint8_t>	Data;
	};

} // namespace 

#endif // GPK_SYSEVENT_H_2094098237
