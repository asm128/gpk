#include "gpk_event_system.h"
#include "gpk_n2.h"

#ifndef GPK_EVENT_SCREEN_H_23627
#define GPK_EVENT_SCREEN_H_23627

nmsp gpk
{
	GDEFINE_ENUM_TYPE (EVENT_SCREEN, u0_t);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Create			,  0);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Destroy		,  1);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Close			,  2);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Move			,  3);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Activate		,  4);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Deactivate		,  5);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Resize			,  6);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Redraw			,  7);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Show			,  8);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Hide			,  9);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Constraints	, 10);

	tydf SEvent            <EVENT_SCREEN>	SScreenEvent;
	tydf SEventView        <EVENT_SCREEN>	SScreenEventView;
	tydf FEventHandler     <EVENT_SCREEN>	FScreenEvent;
	tydf FEventHandlerConst<EVENT_SCREEN>	FScreenEventConst;

	stct SEventArgsScreenCreate {
		n2u1_t		Size;
#ifdef GPK_WINDOWS
		CREATESTRUCT		PlatformDetail;
#endif
	};

	stin	err_t eventEnqueueScreenDestroy		(apobj<SEventSystem> & queue)										{ return eventEnqueueChild(queue, SYSTEM_EVENT_Screen, EVENT_SCREEN_Destroy, {}); }
	stin	err_t eventEnqueueScreenClose		(apobj<SEventSystem> & queue)										{ return eventEnqueueChild(queue, SYSTEM_EVENT_Screen, EVENT_SCREEN_Close  , {}); }
	stin	err_t eventEnqueueScreenCreate		(apobj<SEventSystem> & queue, const SEventArgsScreenCreate & key)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Screen, EVENT_SCREEN_Create, key); }
	stin	err_t eventEnqueueScreenActivate	(apobj<SEventSystem> & queue, u0_t key)		{ return eventEnqueueChild(queue, SYSTEM_EVENT_Screen, EVENT_SCREEN_Activate	, key); }
	stin	err_t eventEnqueueScreenDeactivate	(apobj<SEventSystem> & queue, u0_t key)		{ return eventEnqueueChild(queue, SYSTEM_EVENT_Screen, EVENT_SCREEN_Deactivate	, key); }
	stin	err_t eventEnqueueScreenRedraw		(apobj<SEventSystem> & queue, u0_t key)		{ return eventEnqueueChild(queue, SYSTEM_EVENT_Screen, EVENT_SCREEN_Redraw		, key); }
	stin	err_t eventEnqueueScreenShow		(apobj<SEventSystem> & queue, u0_t key)		{ return eventEnqueueChild(queue, SYSTEM_EVENT_Screen, EVENT_SCREEN_Show		, key); }
	stin	err_t eventEnqueueScreenHide		(apobj<SEventSystem> & queue, u0_t key)		{ return eventEnqueueChild(queue, SYSTEM_EVENT_Screen, EVENT_SCREEN_Hide		, key); }
	stin	err_t eventEnqueueScreenConstraints	(apobj<SEventSystem> & queue, u0_t key)		{ return eventEnqueueChild(queue, SYSTEM_EVENT_Screen, EVENT_SCREEN_Constraints, key); }
	stin	err_t eventEnqueueScreenMove		(apobj<SEventSystem> & queue, n2u1_t coord)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Screen, EVENT_SCREEN_Move		, coord); }
	stin	err_t eventEnqueueScreenResize		(apobj<SEventSystem> & queue, n2u1_t size)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Screen, EVENT_SCREEN_Resize		, size); }
} // nmsp 

#endif // GPK_EVENT_SCREEN_H_23627
