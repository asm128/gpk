#include "gpk_event_system.h"

#ifndef GPK_EVENT_DEVICE_H_23627
#define GPK_EVENT_DEVICE_H_23627

nmsp  gpk
{
#pragma pack(push, 1)
	stct SDeviceResult {
		const SYSTEM_EVENT	SystemEvent 	= {};
		const u0_t			TailWidth		: 2;
		const u0_t			Multiplier		: 6;
		RESULT				DeviceEvent		= {};

		cxpr						SDeviceResult	(RESULT deviceEvent = RESULT_Error)
			: SystemEvent {SYSTEM_EVENT_Device}
			, TailWidth   {uint_tail_width<u2_t>((u2_t)szof(RESULT))}
			, Multiplier  {uint_tail_multiplier<u2_t>((u2_t)szof(RESULT))}
			, DeviceEvent {deviceEvent}
			{}
	};
	stct SDeviceCommand {
		const SYSTEM_EVENT	SystemEvent 	= {};
		u0_c				TailWidth		: 2;
		u0_c				Multiplier		: 6;
		COMMAND				DeviceEvent		= {};

		cxpr						SDeviceCommand	(COMMAND deviceEvent = COMMAND_Ok)
			: SystemEvent {SYSTEM_EVENT_Device}
			, TailWidth   {uint_tail_width<u2_t>((u2_t)szof(COMMAND))}
			, Multiplier  {uint_tail_multiplier<u2_t>((u2_t)szof(COMMAND))}
			, DeviceEvent {deviceEvent}
			{}
	};
#pragma pack(pop)
	tydf SEvent            <RESULT>		SEventResult;
	tydf SEventView        <RESULT>		SEViewResult;
	tydf FEventHandler     <RESULT>		FEventResult;
	tydf FEventHandlerConst<RESULT>		FEventResultConst;
	tydf SEvent            <COMMAND>	SEventCommand;
	tydf SEventView        <COMMAND>	SEViewCommand;
	tydf FEventHandler     <COMMAND>	FEventCommand;
	tydf FEventHandlerConst<COMMAND>	FEventCommandConst;

	//stin	gpk::err_t eventEnqueueRuntimeInit			(TQueueSystemEvent & queue)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Runtime, EVENT_RUNTIME_Init			, EVENT_RUNTIME_Init			); }
	//stin	gpk::err_t eventEnqueueRuntimeExit			(TQueueSystemEvent & queue)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Runtime, EVENT_RUNTIME_Exit			, EVENT_RUNTIME_Exit			); }
	//stin	gpk::err_t eventEnqueueRuntimeProcessQueue	(TQueueSystemEvent & queue)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Runtime, EVENT_RUNTIME_Process_queue, EVENT_RUNTIME_Process_queue); }
	//stin	gpk::err_t eventEnqueueRuntimeProcessEvent	(TQueueSystemEvent & queue)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Runtime, EVENT_RUNTIME_Process_event, EVENT_RUNTIME_Process_event); }
} // namespace

#endif