#include "gpk_array_ptr.h"
#include "gpk_enum.h"

#include "gpk_apod_serialize.h"

#ifndef GPK_EVENT_H_23627
#define GPK_EVENT_H_23627

#define gpk_event_printf verbose_printf

namespace gpk
{
	tplt<tpnm _tEventType>
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
	tplt<tpnm T> using SEView 					= ::gpk::SEventView<T>;
	tplt<tpnm T> using TEViewQueue 				= ::gpk::aobj<::gpk::SEView<T>>;

	tplt<tpnm T> using FEViewHandler			= std::function<::gpk::error_t(      ::gpk::SEView<T>&)>;
	tplt<tpnm T> using FEViewHandlerConst		= std::function<::gpk::error_t(const ::gpk::SEView<T>&)>;
	tplt<tpnm T> using FEventViewHandler		= std::function<::gpk::error_t(      ::gpk::SEView<T>&)>;
	tplt<tpnm T> using FEventViewHandlerConst	= std::function<::gpk::error_t(const ::gpk::SEView<T>&)>;

	tplt<tpnm _tEventType>
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

		tplt<tpnm _tEventTypeOther>
		inline	::gpk::error_t	ExtractChild		(::gpk::SEvent<_tEventTypeOther> & outputEvent)		const	{
			::gpk::vcu8					input				= Data;
			gpk_necs(outputEvent.Load(input));
			gpk_event_printf("%s", ::gpk::get_value_namep(outputEvent.Type)); 
			return 0; 
		}

		tplt<tpnm _tEventTypeOther>
		inline	::gpk::error_t	ExtractChild		(::gpk::SEventView<_tEventTypeOther> & outputEvent)	const	{
			::gpk::vcu8					input				= Data;
			gpk_necs(outputEvent.Load(input));
			gpk_event_printf("%s", ::gpk::get_value_namep(outputEvent.Type)); 
			return 0; 
		}
	};

	tplt<tpnm T> using FEventHandler		= std::function<::gpk::error_t(      ::gpk::SEvent<T>&)>;
	tplt<tpnm T> using FEventHandlerConst	= std::function<::gpk::error_t(const ::gpk::SEvent<T>&)>;

	tplt<tpnm T> using PEvent 				= ::gpk::pobj <::gpk::SEvent<T>>;
	tplt<tpnm T> using TEventQueue 			= ::gpk::apobj<::gpk::SEvent<T>>;

	tplt <tpnm _tEvntParent, tpnm _tEvntChild>
	static	::gpk::error_t	eventWrapChild		(::gpk::SEvent<_tEvntParent> & parentEvent, _tEvntChild childEventType, ::gpk::vcu8 eventData) {
		::gpk::SEView<_tEvntChild>	childEvent			= {childEventType, eventData};
		return childEvent.Save(parentEvent.Data);
	}

	tplt <tpnm _tEvntParent, tpnm _tEvntChild>
	static	::gpk::error_t	eventEnqueueChild	(::gpk::TEventQueue<_tEvntParent> & eventQueue, _tEvntParent parentEventType, _tEvntChild childEventType, ::gpk::vcu8 eventData) {
		::gpk::PEvent<_tEvntParent>	parentEvent			= {};
		parentEvent->Type = parentEventType;
		gpk_necs(::gpk::eventWrapChild(*parentEvent, childEventType, eventData));
		return eventQueue.push_back(parentEvent);
	}

	tplt <tpnm _tEvntParent, tpnm _tEvntChild, tpnm _tPOD>
	static	::gpk::error_t	eventEnqueueChild	(::gpk::TEventQueue<_tEvntParent> & eventQueue, _tEvntParent parentEventType, _tEvntChild childEventType, ::gpk::view<const _tPOD> eventData) {
		return ::gpk::eventEnqueueChild(eventQueue, parentEventType, childEventType, ::gpk::vcu8{(const uint8_t*)eventData.begin(), eventData.byte_count()});
	}

	tplt <tpnm _tEvntParent, tpnm _tEvntChild, tpnm _tPOD>
	static	::gpk::error_t	eventEnqueueChild	(::gpk::TEventQueue<_tEvntParent> & eventQueue, _tEvntParent parentEventType, _tEvntChild childEventType, const _tPOD & childEventDataType) {
		return ::gpk::eventEnqueueChild(eventQueue, parentEventType, childEventType, ::gpk::vcu8{(const uint8_t*)&childEventDataType, sizeof(_tPOD)});
	}

	tplt<tpnm _tChildEvent, tpnm _tParentEvent>
	static	::gpk::error_t	eventExtractAndHandle	(const ::gpk::SEvent<_tParentEvent> & parentEvent, const ::std::function<::gpk::error_t (const ::gpk::SEventView<_tChildEvent> &)> & funcHandleChild) {
		::gpk::SEView<_tChildEvent>	childEvent; 
		gpk_necs(parentEvent.ExtractChild(childEvent)); 
		return funcHandleChild(childEvent);
	}

}

#define gpk_warning_unhandled_event(eventUnhandled)	warning_printf("Unhandled '%s' event: '%s' (0x%llX)(%lli)(%c)"	, ::gpk::get_enum_namep((eventUnhandled).Type), ::gpk::get_value_namep((eventUnhandled).Type), (uint64_t)(eventUnhandled).Type, (int64_t)(eventUnhandled).Type, char((eventUnhandled).Type ? (eventUnhandled).Type : ' '))
#define gpk_warning_not_implemented(eventUnhandled) warning_printf("Implement for '%s'! '%s' (0x%llX)(%lli)(%c)"	, ::gpk::get_enum_namep((eventUnhandled).Type), ::gpk::get_value_namep((eventUnhandled).Type), (uint64_t)(eventUnhandled).Type, (int64_t)(eventUnhandled).Type, char((eventUnhandled).Type ? (eventUnhandled).Type : ' '))

#endif // GPK_EVENT_H_23627