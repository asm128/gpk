#include "gpk_ptr.h"
#include "gpk_enum.h"

#include "gpk_apod_serialize.h"

#ifndef GPK_EVENT_H_230511
#define GPK_EVENT_H_230511

namespace gpk
{
	template<typename _tEventType>
	struct SEventView {
		_tEventType				Type;
		::gpk::vcu8				Data;

		::gpk::error_t			Save				(::gpk::au8 & output)	const	{
			gpk_necs(::gpk::savePOD (output, Type));
			gpk_necs(::gpk::saveView(output, Data));
			return 0;
		}

		::gpk::error_t			Load				(::gpk::vcu8 & input)			{
			gpk_necs(::gpk::loadPOD (input, Type));
			gpk_necs(::gpk::loadView(input, Data));
			return 0;
		}
	};
	template<typename T> using FEventViewHandler		= std::function<::gpk::error_t(      ::gpk::SEventView<T>&)>;
	template<typename T> using FEventViewHandlerConst	= std::function<::gpk::error_t(const ::gpk::SEventView<T>&)>;

	template<typename _tEventType>
	struct SEvent {
		_tEventType				Type				= {};
		::gpk::au8				Data				= {};

		::gpk::error_t			Save				(::gpk::au8 & output)	const	{
			gpk_necs(::gpk::savePOD(output, Type));
			gpk_necs(::gpk::saveView(output, Data));
			return 0;
		}

		::gpk::error_t			Load				(::gpk::vcu8 & input)			{
			gpk_necs(::gpk::loadPOD(input, Type));
			gpk_necs(::gpk::loadView(input, Data));
			return 0;
		}

		template<typename _tEventTypeOther>
		inline	::gpk::error_t	ExtractChild		(::gpk::SEvent<_tEventTypeOther> & outputEvent)		const	{
			::gpk::vcu8					input				= Data;
			gpk_necs(outputEvent.Load(input));
			info_printf("%s", ::gpk::get_value_namep(outputEvent.Type)); 
			return 0; 
		}

		template<typename _tEventTypeOther>
		inline	::gpk::error_t	ExtractChild		(::gpk::SEventView<_tEventTypeOther> & outputEvent)	const	{
			::gpk::vcu8					input				= Data;
			gpk_necs(outputEvent.Load(input));
			info_printf("%s", ::gpk::get_value_namep(outputEvent.Type)); 
			return 0; 
		}
	};

	template<typename T> using FEventHandler		= std::function<::gpk::error_t(      ::gpk::SEvent<T>&)>;
	template<typename T> using FEventHandlerConst	= std::function<::gpk::error_t(const ::gpk::SEvent<T>&)>;

	template <typename _tEvntParent, typename _tEvntChild>
	static	::gpk::error_t	eventWrapChild		(::gpk::SEvent<_tEvntParent> & parentEvent, _tEvntChild childEventType, ::gpk::vcu8 eventData) {
		::gpk::SEvent<_tEvntChild>	childEvent			= {childEventType, };
		gpk_necs(childEvent.Data.append(eventData));
		return childEvent.Save(parentEvent.Data);
	}

	template <typename _tEvntParent, typename _tEvntChild>
	static	::gpk::error_t	eventEnqueueChild	(::gpk::apobj<::gpk::SEvent<_tEvntParent>> & eventQueue, _tEvntParent parentEventType, _tEvntChild childEventType, ::gpk::vcu8 eventData) {
		::gpk::pobj<::gpk::SEvent<_tEvntParent>>	parentEvent			= {};
		parentEvent->Type = parentEventType;
		gpk_necs(::gpk::eventWrapChild(*parentEvent, childEventType, eventData));
		return eventQueue.push_back(parentEvent);
	}

	template <typename _tEvntParent, typename _tEvntChild, typename _tPOD>
	static	::gpk::error_t	eventEnqueueChild	(::gpk::apobj<::gpk::SEvent<_tEvntParent>> & eventQueue, _tEvntParent parentEventType, _tEvntChild childEventType, ::gpk::view<const _tPOD> eventData) {
		return ::gpk::eventEnqueueChild(eventQueue, parentEventType, childEventType, ::gpk::vcu8{(const uint8_t*)eventData.begin(), eventData.byte_count()});
	}

	template <typename _tEvntParent, typename _tEvntChild, typename _tPOD>
	static	::gpk::error_t	eventEnqueueChild	(::gpk::apobj<::gpk::SEvent<_tEvntParent>> & eventQueue, _tEvntParent parentEventType, _tEvntChild childEventType, const _tPOD & childEventDataType) {
		return ::gpk::eventEnqueueChild(eventQueue, parentEventType, childEventType, ::gpk::vcu8{(const uint8_t*)&childEventDataType, sizeof(_tPOD)});
	}

	template<typename _tParentEvent, typename _tChildEvent>
	static	::gpk::error_t	extractAndHandle	(const ::gpk::SEvent<_tParentEvent> & parentEvent, const ::std::function<::gpk::error_t (const ::gpk::SEventView<_tChildEvent> &)> & funcHandleChild) {
		::gpk::SEventView<_tChildEvent>	childEvent; 
		gpk_necs(parentEvent.ExtractChild(childEvent)); 
		return funcHandleChild(childEvent);
	}
}

#define gpk_warning_unhandled_event(eventUnhandled)		warning_printf("Unhandled %s event: %s (0x%X)(%X)(%c)", ::gpk::get_enum_namep(eventUnhandled.Type), ::gpk::get_value_namep(eventUnhandled.Type), eventUnhandled.Type, eventUnhandled.Type, (char)eventUnhandled.Type); 

#endif // GPK_EVENT_H_230511