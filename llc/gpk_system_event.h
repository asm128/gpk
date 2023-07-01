#include "gpk_event.h"

#ifndef GPK_SYSTEM_EVENT_H_23627
#define GPK_SYSTEM_EVENT_H_23627

namespace gpk
{
	GDEFINE_ENUM_TYPE(SYSTEM_EVENT, uint32_t);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Runtime	, 0);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Screen		, 1);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Keyboard	, 2);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Mouse		, 3);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Touch		, 4);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Joypad		, 5);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Text		, 6);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, GUI		, 7);

	typedef ::gpk::SEvent            <SYSTEM_EVENT> SSystemEvent;
	typedef ::gpk::FEventHandler     <SYSTEM_EVENT>	FSystemEvent;
	typedef ::gpk::FEventHandlerConst<SYSTEM_EVENT>	FSystemEventConst;
	typedef ::gpk::pobj <SSystemEvent>				PSystemEvent;
	typedef ::gpk::apobj<SSystemEvent>				TQueueSystemEvent;

	GDEFINE_ENUM_TYPE (EVENT_RUNTIME, uint8_t);
	GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Init			, 0);
	GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Exit			, 1);
	GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Process_queue	, 2);
	GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Process_event	, 3);
	stainli	::gpk::error_t	eventEnqueueRuntimeInit			(::gpk::TQueueSystemEvent & queue)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Runtime, ::gpk::EVENT_RUNTIME_Init			, ::gpk::EVENT_RUNTIME_Init			); }
	stainli	::gpk::error_t	eventEnqueueRuntimeExit			(::gpk::TQueueSystemEvent & queue)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Runtime, ::gpk::EVENT_RUNTIME_Exit			, ::gpk::EVENT_RUNTIME_Exit			); }
	stainli	::gpk::error_t	eventEnqueueRuntimeProcessQueue	(::gpk::TQueueSystemEvent & queue)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Runtime, ::gpk::EVENT_RUNTIME_Process_queue, ::gpk::EVENT_RUNTIME_Process_queue); }
	stainli	::gpk::error_t	eventEnqueueRuntimeProcessEvent	(::gpk::TQueueSystemEvent & queue)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Runtime, ::gpk::EVENT_RUNTIME_Process_event, ::gpk::EVENT_RUNTIME_Process_event); }
} // namespace 

#endif // GPK_SYSTEM_EVENT_H_23627
