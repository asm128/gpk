#include "gpk_event.h"

#include "gpk_enum.h"
#include "gpk_ctti.h"

#ifndef GPK_SYSTEM_EVENT_H_23627
#define GPK_SYSTEM_EVENT_H_23627

nmsp gpk
{
	GDEFINE_ENUM_TYPE(SYSTEM_EVENT, u0_t);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Runtime	, 0);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Screen		, 1);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Keyboard	, 2);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Mouse		, 3);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Touch		, 4);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Joypad		, 5);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Text		, 6);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, GUI		, 7);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Audio		, 8);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Camera		, 9);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Raster		,10);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Device		,11);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Wire		,12);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Bus		,13);

	tydf SEvent            <SYSTEM_EVENT>	SEventSystem;
	tydf SEView            <SYSTEM_EVENT>	SEViewSystem;
	tydf FEventHandler     <SYSTEM_EVENT>	FEventSystem;
	tydf FEventHandlerConst<SYSTEM_EVENT>	FEventSystemConst;
	tydf FEViewHandler     <SYSTEM_EVENT>	FEViewSystem;
	tydf FEViewHandlerConst<SYSTEM_EVENT>	FEViewSystemConst;
	tydf pobj <SEventSystem>				PEventSystem;
	tydf apobj<SEventSystem>				TQueueSystemEvent;
	
	tplt<tpnm _t>
	stin	gpk::err_t eventEnqueueChild	(apobj<SEventSystem> & queue, RESULT eventType, _t & eventData)	{ 
		return eventEnqueueChild(queue, SYSTEM_EVENT_Device, eventType, eventData); 
	}

	//GDEFINE_ENUM_TYPE (EVENT_RUNTIME, u0_t);
	//GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Init			, 0);
	//GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Exit			, 1);
	//GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Process_queue	, 2);
	//GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Process_event	, 3);
} // nmsp

#endif // GPK_SYSTEM_EVENT_H_23627
