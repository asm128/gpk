#include "gpk_apod_serialize.h"
#include "gpk_array_ptr.h"
#include "gpk_enum.h"
#include "gpk_ctti_stct.h"

#ifndef GPK_EVENT_H_23627
#define GPK_EVENT_H_23627

#define gpk_event_printf verbose_printf

nmsp gpk
{
	tpl_t struct SEventView {
		tdfT(_t);
		tydf	SEventView<T>	TEView;
		T		Type		= {};
		vcu0_t	Data		= {};

		err_t	Save		(au0_t & output)	cnst	{
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
	tplt<tpnm T> usng SEView 					= SEventView<T>;
	tplt<tpnm T> usng TEViewQueue 				= aobj<SEView<T>>;

	tplt<tpnm T> usng FEViewHandler				= function<err_t(      SEView<T>&)>;
	tplt<tpnm T> usng FEViewHandlerConst		= function<err_t(cnst SEView<T>&)>;
	tplt<tpnm T> usng FEventViewHandler			= function<err_t(      SEView<T>&)>;
	tplt<tpnm T> usng FEventViewHandlerConst	= function<err_t(cnst SEView<T>&)>;

	tplt<tpnm _tEventType>
	stct SEvent {
		tydf _tEventType			T;
		tydf SEView<T>	TEView;
		tydf SEvent<T>	TEvent;

		T				Type		= {};
		au0_t			Data		= {};

						SEvent		(cnst TEvent &)					= default;
		cxpr			SEvent		(T type = {})					: Type(type) {}
						SEvent		(T type, vcu0_c data)			: Type(type), Data(data) {}
						SEvent		(cnst TEView & eventView)		: Type(eventView.Type), Data(eventView.Data) {}

		TEvent&			oper= 		(cnst TEvent &)					= default;
		TEvent&			oper= 		(cnst TEView & eventView)		{ Type = eventView.Type; Data = eventView.Data; rtrn *this; }

		oper			TEView		()						cnst	{ rtrn {Type, Data}; }

		err_t			Save		(au0_t & output)		cnst	{
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
		err_t			ExtractChild(SEvent<_tETypeOther> & outputEvent)	cnst	{
			vcu0_t			input				= Data;
			if_fail_fe(outputEvent.Load(input));
			gpk_event_printf("%s", get_value_namep(outputEvent.Type)); 
			rtrn 0; 
		}
		tplt<tpnm _tETypeOther>
		err_t			ExtractChild(SEView<_tETypeOther> & outputEvent)	cnst	{
			vcu0_t			input				= Data;
			if_fail_fe(outputEvent.Load(input));
			gpk_event_printf("%s", get_value_namep(outputEvent.Type)); 
			rtrn 0; 
		}
	};
	CTTI_STCT_TEMPLATE_EXISTING(SEvent, _tEventType, struct SEvent { _tEventType Type; au0_t Data; };, _tEventType, au0_t);
	static_assert(ctti_struct_member_types<SEvent<u0_t>>);
	static_assert(ctti_same<tpnm SCTTIMemberTypes<SEvent<u0_t>, ctti_index_pack<0, 1>>::Type, type_pack<u0_t, au0_t>>::Value);

	tplt<tpnm T> usng FEventHandler			= function<err_t(      SEvent<T>&)>;
	tplt<tpnm T> usng FEventHandlerConst	= function<err_t(cnst SEvent<T>&)>;

	tplt<tpnm T> usng PEvent 				= pobj <SEvent<T>>;
	tplt<tpnm T> usng TEventQueue 			= apobj<SEvent<T>>;

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
	sttc	err_t	eventEnqueueChild	(TEventQueue<_tEvntParent> & eventQueue, _tEvntParent parentEventType, _tEvntChild childEventType, view<cnst _tPOD> eventData) {
		rtrn eventEnqueueChild(eventQueue, parentEventType, childEventType, vcu0_t{(cnst u0_t*)eventData.begin(), eventData.byte_count()});
	}
	tplt <tpnm _tEvntParent, tpnm _tEvntChild, tpnm _tPOD>
	sttc	err_t	eventEnqueueChild	(TEventQueue<_tEvntParent> & eventQueue, _tEvntParent parentEventType, _tEvntChild childEventType, cnst _tPOD & childEventDataType) {
		rtrn eventEnqueueChild(eventQueue, parentEventType, childEventType, vcu0_t{(cnst u0_t*)&childEventDataType, szof(_tPOD)});
	}
	tplt<tpnm _tChildEvent, tpnm _tParentEvent>
	sttc	err_t	eventExtractAndHandle	(cnst SEvent<_tParentEvent> & parentEvent, cnst FError<cnst SEventView<_tChildEvent> &> & funcHandleChild) {
		SEView<_tChildEvent>	childEvent; 
		if_fail_fe(parentEvent.ExtractChild(childEvent)); 
		rtrn funcHandleChild(childEvent);
	}
}

#define gpk_warning_unhandled_event(eventUnhandled)	warning_printf("Unhandled '%s' event: '%s'	(0x%" GPK_FMT_X3  ")(%" GPK_FMT_S3 ")(%c)"	, get_enum_namep((eventUnhandled).Type), get_value_namep((eventUnhandled).Type), (u3_t)(eventUnhandled).Type, (s3_t)(eventUnhandled).Type, sc_t((eventUnhandled).Type ? (eventUnhandled).Type : ' '))
#define gpk_warning_not_implemented(eventUnhandled) warning_printf("Implement for '%s'! '%s'	(0x%" GPK_FMT_X3  ")(%" GPK_FMT_S3 ")(%c)"	, get_enum_namep((eventUnhandled).Type), get_value_namep((eventUnhandled).Type), (u3_t)(eventUnhandled).Type, (s3_t)(eventUnhandled).Type, sc_t((eventUnhandled).Type ? (eventUnhandled).Type : ' '))

#endif // GPK_EVENT_H_23627
