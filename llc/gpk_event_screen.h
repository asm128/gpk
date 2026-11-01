#include "gpk_system_event.h"
#include "gpk_n2.h"

#ifndef GPK_EVENT_SCREEN_H_23627
#define GPK_EVENT_SCREEN_H_23627

namespace gpk
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

	tydf ::gpk::SEvent            <EVENT_SCREEN>	SScreenEvent;
	tydf ::gpk::SEventView        <EVENT_SCREEN>	SScreenEventView;
	tydf ::gpk::FEventHandler     <EVENT_SCREEN>	FScreenEvent;
	tydf ::gpk::FEventHandlerConst<EVENT_SCREEN>	FScreenEventConst;

	stct SEventArgsScreenCreate {
		::gpk::n2u1_t		Size;
#ifdef GPK_WINDOWS
		CREATESTRUCT		PlatformDetail;
#endif
	};

	stin	::gpk::err_t	eventEnqueueScreenDestroy		(::gpk::apobj<::gpk::SEventSystem> & queue)						{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Destroy, {}); }
	stin	::gpk::err_t	eventEnqueueScreenClose			(::gpk::apobj<::gpk::SEventSystem> & queue)						{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Close  , {}); }
	stin	::gpk::err_t	eventEnqueueScreenCreate		(::gpk::apobj<::gpk::SEventSystem> & queue, const SEventArgsScreenCreate & key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Create, key); }
	stin	::gpk::err_t	eventEnqueueScreenActivate		(::gpk::apobj<::gpk::SEventSystem> & queue, u0_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Activate	, key); }
	stin	::gpk::err_t	eventEnqueueScreenDeactivate	(::gpk::apobj<::gpk::SEventSystem> & queue, u0_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Deactivate	, key); }
	stin	::gpk::err_t	eventEnqueueScreenRedraw		(::gpk::apobj<::gpk::SEventSystem> & queue, u0_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Redraw		, key); }
	stin	::gpk::err_t	eventEnqueueScreenShow			(::gpk::apobj<::gpk::SEventSystem> & queue, u0_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Show		, key); }
	stin	::gpk::err_t	eventEnqueueScreenHide			(::gpk::apobj<::gpk::SEventSystem> & queue, u0_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Hide		, key); }
	stin	::gpk::err_t	eventEnqueueScreenConstraints	(::gpk::apobj<::gpk::SEventSystem> & queue, u0_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Constraints, key); }
	stin	::gpk::err_t	eventEnqueueScreenMove			(::gpk::apobj<::gpk::SEventSystem> & queue, ::gpk::n2u1_t coord)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Move		, coord); }
	stin	::gpk::err_t	eventEnqueueScreenResize		(::gpk::apobj<::gpk::SEventSystem> & queue, ::gpk::n2u1_t size)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Resize		, size); }
} // namespace 

#endif // GPK_EVENT_SCREEN_H_23627
