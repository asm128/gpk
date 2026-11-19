#include "gpk_event.h"

#include "gpk_enum.h"

#ifndef GPK_SYSTEM_EVENT_H_23627
#define GPK_SYSTEM_EVENT_H_23627

nmsp gpk
{
	CTTI_ENUM(SYSTEM_EVENT,
	enum SYSTEM_EVENT : u0_t
		{ SYSTEM_EVENT_Runtime		=  0
		, SYSTEM_EVENT_Framework	=  1
		, SYSTEM_EVENT_Screen		=  2
		, SYSTEM_EVENT_Keyboard		=  3
		, SYSTEM_EVENT_Mouse		=  4
		, SYSTEM_EVENT_Touch		=  5
		, SYSTEM_EVENT_Joypad		=  6
		, SYSTEM_EVENT_Text			=  7
		, SYSTEM_EVENT_GUI			=  8
		, SYSTEM_EVENT_Audio		=  9
		, SYSTEM_EVENT_Camera		= 10
		, SYSTEM_EVENT_Raster		= 11
		, SYSTEM_EVENT_Device		= 12
		, SYSTEM_EVENT_Wire			= 13
		, SYSTEM_EVENT_Bus			= 14
		};
	);
	GDEFINE_ENUM_CTTI(SYSTEM_EVENT);

	tydf SEvent            <SYSTEM_EVENT>	SEventSystem;
	static_assert(ctti_same<tpnm SCTTIMemberTypes<SEventSystem, ctti_index_pack<0, 1>>::Type, type_pack<SYSTEM_EVENT, au0_t>>::Value);
	tydf SEView            <SYSTEM_EVENT>	SEViewSystem;
	tydf FEventHandler     <SYSTEM_EVENT>	FEventSystem;
	tydf FEventHandlerConst<SYSTEM_EVENT>	FEventSystemConst;
	tydf FEViewHandler     <SYSTEM_EVENT>	FEViewSystem;
	tydf FEViewHandlerConst<SYSTEM_EVENT>	FEViewSystemConst;
	tydf pobj <SEventSystem>				PEventSystem;
	tydf apobj<SEventSystem>				TQueueSystemEvent;

	//GDEFINE_ENUM_TYPE (EVENT_RUNTIME, u0_t);
	//GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Init			, 0);
	//GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Exit			, 1);
	//GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Process_queue	, 2);
	//GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Process_event	, 3);
} // nmsp

#endif // GPK_SYSTEM_EVENT_H_23627
