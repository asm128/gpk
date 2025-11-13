#include "gpk_event.h"

#ifndef GPK_SYSTEM_EVENT_H_23627
#define GPK_SYSTEM_EVENT_H_23627

namespace gpk
{
	GDEFINE_ENUM_TYPE(SYSTEM_EVENT, uint8_t);
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

	tydf ::gpk::SEvent            <::gpk::SYSTEM_EVENT>	SEventSystem;
	tydf ::gpk::SEView            <::gpk::SYSTEM_EVENT>	SEViewSystem;
	tydf ::gpk::FEventHandler     <::gpk::SYSTEM_EVENT>	FEventSystem;
	tydf ::gpk::FEventHandlerConst<::gpk::SYSTEM_EVENT>	FEventSystemConst;
	tydf ::gpk::FEViewHandler     <::gpk::SYSTEM_EVENT>	FEViewSystem;
	tydf ::gpk::FEViewHandlerConst<::gpk::SYSTEM_EVENT>	FEViewSystemConst;
	tydf ::gpk::pobj <::gpk::SEventSystem>				PEventSystem;
	tydf ::gpk::apobj<::gpk::SEventSystem>				TQueueSystemEvent;
	
	template<typename _t>
	stin	::gpk::error_t	eventEnqueueChild	(::gpk::apobj<::gpk::SEventSystem> & queue, ::gpk::RESULT eventType, _t & eventData)	{ 
		return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Device, eventType, eventData); 
	}

	//GDEFINE_ENUM_TYPE (EVENT_RUNTIME, uint8_t);
	//GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Init			, 0);
	//GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Exit			, 1);
	//GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Process_queue	, 2);
	//GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Process_event	, 3);

#pragma pack(push, 1)
	stct SDeviceResult {
		const ::gpk::SYSTEM_EVENT	SystemEvent 	= {};
		const uint8_t				TailWidth		: 2;
		const uint8_t				Multiplier		: 6;
		::gpk::RESULT				DeviceEvent		= {};

		cxpr						SDeviceResult	(::gpk::RESULT deviceEvent = ::gpk::RESULT_Error)
			: SystemEvent {::gpk::SYSTEM_EVENT_Device}
			, TailWidth   {::gpk::uint_tail_width<uint32_t>((uint32_t)szof(::gpk::RESULT))}
			, Multiplier  {::gpk::uint_tail_multiplier<uint32_t>((uint32_t)szof(::gpk::RESULT))}
			, DeviceEvent {deviceEvent}
			{}
	};
	stct SDeviceCommand {
		const ::gpk::SYSTEM_EVENT	SystemEvent 	= {};
		const uint8_t				TailWidth		: 2;
		const uint8_t				Multiplier		: 6;
		::gpk::COMMAND				DeviceEvent		= {};

		cxpr						SDeviceCommand	(::gpk::COMMAND deviceEvent = ::gpk::COMMAND_Ok)
			: SystemEvent {::gpk::SYSTEM_EVENT_Device}
			, TailWidth   {::gpk::uint_tail_width<uint32_t>((uint32_t)szof(::gpk::COMMAND))}
			, Multiplier  {::gpk::uint_tail_multiplier<uint32_t>((uint32_t)szof(::gpk::COMMAND))}
			, DeviceEvent {deviceEvent}
			{}
	};
#pragma pack(pop)

	//stin	::gpk::error_t	eventEnqueueRuntimeInit			(::gpk::TQueueSystemEvent & queue)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Runtime, ::gpk::EVENT_RUNTIME_Init			, ::gpk::EVENT_RUNTIME_Init			); }
	//stin	::gpk::error_t	eventEnqueueRuntimeExit			(::gpk::TQueueSystemEvent & queue)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Runtime, ::gpk::EVENT_RUNTIME_Exit			, ::gpk::EVENT_RUNTIME_Exit			); }
	//stin	::gpk::error_t	eventEnqueueRuntimeProcessQueue	(::gpk::TQueueSystemEvent & queue)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Runtime, ::gpk::EVENT_RUNTIME_Process_queue, ::gpk::EVENT_RUNTIME_Process_queue); }
	//stin	::gpk::error_t	eventEnqueueRuntimeProcessEvent	(::gpk::TQueueSystemEvent & queue)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Runtime, ::gpk::EVENT_RUNTIME_Process_event, ::gpk::EVENT_RUNTIME_Process_event); }
} // namespace

#endif // GPK_SYSTEM_EVENT_H_23627
