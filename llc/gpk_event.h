#include "gpk_array_ptr.h"
#include "gpk_enum.h"

#include "gpk_apod_serialize.h"

#ifndef GPK_EVENT_H_23627
#define GPK_EVENT_H_23627

#define gpk_event_printf verbose_printf

namespace gpk
{
	tpl_t struct SEventView {
		tdfT(_t);
		tydf	SEventView<T>	TEView;
		T		Type		= {};
		vcu0_t	Data		= {};

		err_t	Save		(au0_t & output)	const	{
			if_fail_fe(savePOD (output, Type));
			if_fail_fe(saveView(output, Data));
			rtrn 0;
		}
		err_t	Load		(vcu0_t & input)			{
			if_fail_fe(loadPOD (input, Type));
			if_fail_fe(loadView(input, Data));
			rtrn 0;
		}
	};
	tplt<tpnm T> using SEView 					= SEventView<T>;
	tplt<tpnm T> using TEViewQueue 				= aobj<SEView<T>>;

	tplt<tpnm T> using FEViewHandler			= function<err_t(      SEView<T>&)>;
	tplt<tpnm T> using FEViewHandlerConst		= function<err_t(const SEView<T>&)>;
	tplt<tpnm T> using FEventViewHandler		= function<err_t(      SEView<T>&)>;
	tplt<tpnm T> using FEventViewHandlerConst	= function<err_t(const SEView<T>&)>;

	tplt<tpnm _tEventType>
	stct SEvent {
		tydf _tEventType			T;
		tydf SEView<T>	TEView;
		tydf SEvent<T>	TEvent;

		T				Type		= {};
		au0_t			Data		= {};

						SEvent		(const TEvent &)					= default;
		constexpr		SEvent		(T type = {})						: Type(type) {}
						SEvent		(T type, const vcu0_t data)			: Type(type), Data(data) {}
						SEvent		(const TEView & eventView)			: Type(eventView.Type), Data(eventView.Data) {}

		TEvent&			oper= 		(const TEvent &)					= default;
		TEvent&			oper= 		(const TEView & eventView)			{ Type = eventView.Type; Data = eventView.Data; rtrn *this; }

		oper			TEView		()							const	{ rtrn {Type, Data.cu8()}; }

		err_t			Save		(au0_t & output)			const	{
			if_fail_fe(savePOD(output, Type));
			if_fail_fe(saveView(output, Data));
			rtrn 0;
		}

		err_t			Load		(vcu0_t & input)			{
			if_fail_fe(loadPOD(input, Type));
			if_fail_fe(loadView(input, Data));
			rtrn 0;
		}

		tplt<tpnm _tETypeOther>
		err_t			ExtractChild(SEvent<_tETypeOther> & outputEvent)	const	{
			vcu0_t			input				= Data;
			if_fail_fe(outputEvent.Load(input));
			gpk_event_printf("%s", get_value_namep(outputEvent.Type)); 
			rtrn 0; 
		}

		tplt<tpnm _tETypeOther>
		err_t	ExtractChild(SEView<_tETypeOther> & outputEvent)	const	{
			vcu0_t			input				= Data;
			if_fail_fe(outputEvent.Load(input));
			gpk_event_printf("%s", get_value_namep(outputEvent.Type)); 
			rtrn 0; 
		}
	};

	tplt<tpnm T> using FEventHandler		= function<err_t(      SEvent<T>&)>;
	tplt<tpnm T> using FEventHandlerConst	= function<err_t(const SEvent<T>&)>;

	tplt<tpnm T> using PEvent 				= pobj <SEvent<T>>;
	tplt<tpnm T> using TEventQueue 			= apobj<SEvent<T>>;

	tplt <tpnm _tEvntParent, tpnm _tEvntChild>
	sttc	err_t	eventWrapChild		(SEvent<_tEvntParent> & parentEvent, _tEvntChild childEventType, vcu0_t eventData) {
		SEView<_tEvntChild>	childEvent			= {childEventType, eventData};
		rtrn childEvent.Save(parentEvent.Data);
	}
	tplt <tpnm _tEvntParent, tpnm _tEvntChild>
	sttc	err_t	eventEnqueueChild	(TEventQueue<_tEvntParent> & eventQueue, _tEvntParent parentEventType, _tEvntChild childEventType, vcu0_t eventData) {
		PEvent<_tEvntParent>	parentEvent			= {};
		parentEvent->Type = parentEventType;
		if_fail_fe(eventWrapChild(*parentEvent, childEventType, eventData));
		rtrn eventQueue.push_back(parentEvent);
	}
	tplt <tpnm _tEvntParent, tpnm _tEvntChild, tpnm _tPOD>
	sttc	err_t	eventEnqueueChild	(TEventQueue<_tEvntParent> & eventQueue, _tEvntParent parentEventType, _tEvntChild childEventType, view<const _tPOD> eventData) {
		rtrn eventEnqueueChild(eventQueue, parentEventType, childEventType, vcu0_t{(const u0_t*)eventData.begin(), eventData.byte_count()});
	}
	tplt <tpnm _tEvntParent, tpnm _tEvntChild, tpnm _tPOD>
	sttc	err_t	eventEnqueueChild	(TEventQueue<_tEvntParent> & eventQueue, _tEvntParent parentEventType, _tEvntChild childEventType, const _tPOD & childEventDataType) {
		rtrn eventEnqueueChild(eventQueue, parentEventType, childEventType, vcu0_t{(const u0_t*)&childEventDataType, szof(_tPOD)});
	}
	tplt<tpnm _tChildEvent, tpnm _tParentEvent>
	sttc	err_t	eventExtractAndHandle	(const SEvent<_tParentEvent> & parentEvent, const function<err_t (const SEventView<_tChildEvent> &)> & funcHandleChild) {
		SEView<_tChildEvent>	childEvent; 
		if_fail_fe(parentEvent.ExtractChild(childEvent)); 
		rtrn funcHandleChild(childEvent);
	}
	tydf SEvent            <RESULT>	SEventResult;
	tydf SEventView        <RESULT>	SEViewResult;
	tydf FEventHandler     <RESULT>	FEventResult;
	tydf FEventHandlerConst<RESULT>	FEventResultConst;
	tydf SEvent            <COMMAND>	SEventCommand;
	tydf SEventView        <COMMAND>	SEViewCommand;
	tydf FEventHandler     <COMMAND>	FEventCommand;
	tydf FEventHandlerConst<COMMAND>	FEventCommandConst;
}

#define gpk_warning_unhandled_event(eventUnhandled)	warning_printf("Unhandled '%s' event: '%s' (0x%llX)(%lli)(%c)"	, get_enum_namep((eventUnhandled).Type), get_value_namep((eventUnhandled).Type), (u3_t)(eventUnhandled).Type, (s3_t)(eventUnhandled).Type, char((eventUnhandled).Type ? (eventUnhandled).Type : ' '))
#define gpk_warning_not_implemented(eventUnhandled) warning_printf("Implement for '%s'! '%s' (0x%llX)(%lli)(%c)"	, get_enum_namep((eventUnhandled).Type), get_value_namep((eventUnhandled).Type), (u3_t)(eventUnhandled).Type, (s3_t)(eventUnhandled).Type, char((eventUnhandled).Type ? (eventUnhandled).Type : ' '))

#endif // GPK_EVENT_H_23627
