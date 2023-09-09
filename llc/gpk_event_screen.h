#include "gpk_system_event.h"
#include "gpk_n2.h"

#ifndef GPK_EVENT_SCREEN_H_23627
#define GPK_EVENT_SCREEN_H_23627

namespace gpk
{
	GDEFINE_ENUM_TYPE (EVENT_SCREEN, uint8_t);
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

	typedef ::gpk::SEvent            <EVENT_SCREEN>	SScreenEvent;
	typedef ::gpk::SEventView        <EVENT_SCREEN>	SScreenEventView;
	typedef ::gpk::FEventHandler     <EVENT_SCREEN>	FScreenEvent;
	typedef ::gpk::FEventHandlerConst<EVENT_SCREEN>	FScreenEventConst;

	struct SEventArgsScreenCreate {
		::gpk::n2u16		Size;
#ifdef GPK_WINDOWS
		CREATESTRUCT		PlatformDetail;
#endif
	};

	stainli	::gpk::error_t	eventEnqueueScreenDestroy		(::gpk::apobj<::gpk::SEventSystem> & queue)						{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Destroy, {}); }
	stainli	::gpk::error_t	eventEnqueueScreenClose			(::gpk::apobj<::gpk::SEventSystem> & queue)						{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Close  , {}); }
	stainli	::gpk::error_t	eventEnqueueScreenCreate		(::gpk::apobj<::gpk::SEventSystem> & queue, const SEventArgsScreenCreate & key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Create, key); }
	stainli	::gpk::error_t	eventEnqueueScreenActivate		(::gpk::apobj<::gpk::SEventSystem> & queue, uint8_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Activate	, key); }
	stainli	::gpk::error_t	eventEnqueueScreenDeactivate	(::gpk::apobj<::gpk::SEventSystem> & queue, uint8_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Deactivate	, key); }
	stainli	::gpk::error_t	eventEnqueueScreenRedraw		(::gpk::apobj<::gpk::SEventSystem> & queue, uint8_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Redraw		, key); }
	stainli	::gpk::error_t	eventEnqueueScreenShow			(::gpk::apobj<::gpk::SEventSystem> & queue, uint8_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Show		, key); }
	stainli	::gpk::error_t	eventEnqueueScreenHide			(::gpk::apobj<::gpk::SEventSystem> & queue, uint8_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Hide		, key); }
	stainli	::gpk::error_t	eventEnqueueScreenConstraints	(::gpk::apobj<::gpk::SEventSystem> & queue, uint8_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Constraints, key); }
	stainli	::gpk::error_t	eventEnqueueScreenMove			(::gpk::apobj<::gpk::SEventSystem> & queue, ::gpk::n2u16 coord)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Move		, coord); }
	stainli	::gpk::error_t	eventEnqueueScreenResize		(::gpk::apobj<::gpk::SEventSystem> & queue, ::gpk::n2u16 size)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Resize		, size); }
} // namespace 

#endif // GPK_EVENT_SCREEN_H_23627
