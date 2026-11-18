#include "gpk_event.h"

#include "gpk_enum.h"

#ifndef GPK_SYSTEM_EVENT_H_23627
#define GPK_SYSTEM_EVENT_H_23627

nmsp gpk
{
	GDEFINE_ENUM_TYPE(SYSTEM_EVENT, u0_t);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Runtime	, 0);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Framework	, 1);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Screen		, 2);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Keyboard	, 3);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Mouse		, 4);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Touch		, 5);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Joypad		, 6);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Text		, 7);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, GUI		, 8);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Audio		, 9);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Camera		,10);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Raster		,11);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Device		,12);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Wire		,13);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Bus		,14);

	tydf SEvent            <SYSTEM_EVENT>	SEventSystem;
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
