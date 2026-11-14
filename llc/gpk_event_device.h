#include "gpk_event_system.h"

#ifndef GPK_EVENT_DEVICE_H_23627
#define GPK_EVENT_DEVICE_H_23627

nmsp  gpk
{
	CTTI_ENUM(
	enum RESULT
		{ RESULT_Ok					= 0
		, RESULT_Error
		, RESULT_Busy              
		, RESULT_Timeout           
		, RESULT_Full              
		, RESULT_Empty             
		, RESULT_Overrun           
		, RESULT_Not_available     
		, RESULT_Not_found         
		, RESULT_Invalid_parameter 
		, RESULT_Forbidden         
		, RESULT_Restart           
		, RESULT_Wake_up           
		, RESULT_Sleep             
		, RESULT_Offline           
		, RESULT_Disconnected      
		, RESULT_Connecting        
		, RESULT_Connected         
		, RESULT_Missing_data      
		, RESULT_No_Memory         
		, RESULT_Read              
		, RESULT_Write             
		};
	);

 	GDEFINE_ENUM_TYPE (COMMAND, s0_t);
	GDEFINE_ENUM_VALUE(COMMAND, Ok                , RESULT_Ok                * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Error             , RESULT_Error             * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Busy              , RESULT_Busy              * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Timeout           , RESULT_Timeout           * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Full              , RESULT_Full              * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Empty             , RESULT_Empty             * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Overrun           , RESULT_Overrun           * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Not_available     , RESULT_Not_available     * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Not_found         , RESULT_Not_found         * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Invalid_parameter , RESULT_Invalid_parameter * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Forbidden         , RESULT_Forbidden         * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Restart           , RESULT_Restart           * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Wake_up           , RESULT_Wake_up           * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Sleep             , RESULT_Sleep             * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Offline           , RESULT_Offline           * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Disconnected      , RESULT_Disconnected      * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Connecting        , RESULT_Connecting        * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Connected         , RESULT_Connected         * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Missing_data      , RESULT_Missing_data      * -1);
	GDEFINE_ENUM_VALUE(COMMAND, No_Memory         , RESULT_No_Memory         * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Read              , RESULT_Read              * -1);
	GDEFINE_ENUM_VALUE(COMMAND, Write             , RESULT_Write             * -1);
	
	tplt<tpnm _t>
	stin	err_t eventEnqueueChild	(apobj<SEventSystem> & queue, RESULT eventType, _t & eventData)	{ 
		return eventEnqueueChild(queue, SYSTEM_EVENT_Device, eventType, eventData); 
	}
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

#endif // GPK_EVENT_DEVICE_H_23627
