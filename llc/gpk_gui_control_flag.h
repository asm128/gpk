#include "gpk_event_system.h"

#include "gpk_bit.h"
#include "gpk_ctti_enum.h"
#include "gpk_ctti_stct.h"

#define gpk_gui_printf verbose_printf

#ifndef GPK_GUI_CONTROL_FLAG_H_23627
#define GPK_GUI_CONTROL_FLAG_H_23627

nmsp gpk
{
#pragma pack(push, 1)
	tydf	s1_t			cid_t;
	stxp	cid_t			CID_INVALID		= cid_t(-1);

	tydf	cnst cid_t		ccid_t;
	tydf	apod<cid_t>		acid;
	tydf	view<cid_t>		vcid;
	tydf	view<ccid_t>	vccid;

	CTTI_ENUM(GUI_CONTROL_FLAG,
	enum GUI_CONTROL_FLAG : u1_t
		{ GUI_CONTROL_FLAG_Hidden	= 0x01U	// Prevents from drawing the control and its children
		, GUI_CONTROL_FLAG_Disabled	= 0x02U	// Prevents the control from responding to user interaction
		, GUI_CONTROL_FLAG_Hovered	= 0x04U	// Modifies behavior depending on whether the mouse pointer top is inside the control area.
		, GUI_CONTROL_FLAG_Focused	= 0x08U	// Modified behavior depending on wether the control is selected for receiving user input.
		, GUI_CONTROL_FLAG_Pressed	= 0x10U	// Modifies behavior depending on whether the control is being pressed by holding the mouse button, a key or a touch event.
		, GUI_CONTROL_FLAG_Dragging	= 0x20U
		, GUI_CONTROL_FLAG_Selected	= 0x40U	//
		, GUI_CONTROL_FLAG_Updated	= 0x80U
		, GUI_CONTROL_FLAG_Action	= 0x100U
		, GUI_CONTROL_FLAG_Unused	= 0xFFFFU
		};
	);
	stxp	GUI_CONTROL_FLAG	GUI_CONTROL_FLAG_All	= GUI_CONTROL_FLAG_Unused;
	inxp	GUI_CONTROL_FLAG	setFlag			(GUI_CONTROL_FLAG & state, cnst GUI_CONTROL_FLAG mask, b8_t value)	nxpt	{ return state = bit_set_masked(state, mask, GUI_CONTROL_FLAG_Hidden	, value); }

	inxp	GUI_CONTROL_FLAG	isUnused		(cnst GUI_CONTROL_FLAG state)										nxpt	{ return bit_test(state, GUI_CONTROL_FLAG_Unused); }
	inxp	GUI_CONTROL_FLAG	isHidden		(cnst GUI_CONTROL_FLAG state, cnst GUI_CONTROL_FLAG mask)			nxpt	{ return bit_test_masked(state, mask, GUI_CONTROL_FLAG_Hidden	); }
	inxp	GUI_CONTROL_FLAG	isDisabled		(cnst GUI_CONTROL_FLAG state, cnst GUI_CONTROL_FLAG mask)			nxpt	{ return bit_test_masked(state, mask, GUI_CONTROL_FLAG_Disabled	); }
	inxp	GUI_CONTROL_FLAG	isHovered		(cnst GUI_CONTROL_FLAG state, cnst GUI_CONTROL_FLAG mask)			nxpt	{ return bit_test_masked(state, mask, GUI_CONTROL_FLAG_Hovered	); }
	inxp	GUI_CONTROL_FLAG	isFocused		(cnst GUI_CONTROL_FLAG state, cnst GUI_CONTROL_FLAG mask)			nxpt	{ return bit_test_masked(state, mask, GUI_CONTROL_FLAG_Focused	); }
	inxp	GUI_CONTROL_FLAG	isPressed		(cnst GUI_CONTROL_FLAG state, cnst GUI_CONTROL_FLAG mask)			nxpt	{ return bit_test_masked(state, mask, GUI_CONTROL_FLAG_Pressed	); }
	inxp	GUI_CONTROL_FLAG	isDragging		(cnst GUI_CONTROL_FLAG state, cnst GUI_CONTROL_FLAG mask)			nxpt	{ return bit_test_masked(state, mask, GUI_CONTROL_FLAG_Dragging	); }
	inxp	GUI_CONTROL_FLAG	isSelected		(cnst GUI_CONTROL_FLAG state, cnst GUI_CONTROL_FLAG mask)			nxpt	{ return bit_test_masked(state, mask, GUI_CONTROL_FLAG_Selected	); }
	inxp	GUI_CONTROL_FLAG	isUpdated		(cnst GUI_CONTROL_FLAG state, cnst GUI_CONTROL_FLAG mask)			nxpt	{ return bit_test_masked(state, mask, GUI_CONTROL_FLAG_Updated	); }
	inxp	GUI_CONTROL_FLAG	isAction		(cnst GUI_CONTROL_FLAG state, cnst GUI_CONTROL_FLAG mask)			nxpt	{ return bit_test_masked(state, mask, GUI_CONTROL_FLAG_Action	); }

	inxp	GUI_CONTROL_FLAG	setUnused		(GUI_CONTROL_FLAG & state, b8_t value)								nxpt	{ return state = bit_set(state, GUI_CONTROL_FLAG_Unused, value); }
	inxp	GUI_CONTROL_FLAG	setHidden		(GUI_CONTROL_FLAG & state, cnst GUI_CONTROL_FLAG mask, b8_t value)	nxpt	{ return state = bit_set_masked(state, mask, GUI_CONTROL_FLAG_Hidden	, value); }
	inxp	GUI_CONTROL_FLAG	setDisabled		(GUI_CONTROL_FLAG & state, cnst GUI_CONTROL_FLAG mask, b8_t value)	nxpt	{ return state = bit_set_masked(state, mask, GUI_CONTROL_FLAG_Disabled	, value); }
	inxp	GUI_CONTROL_FLAG	setHovered		(GUI_CONTROL_FLAG & state, cnst GUI_CONTROL_FLAG mask, b8_t value)	nxpt	{ return state = bit_set_masked(state, mask, GUI_CONTROL_FLAG_Hovered	, value); }
	inxp	GUI_CONTROL_FLAG	setFocused		(GUI_CONTROL_FLAG & state, cnst GUI_CONTROL_FLAG mask, b8_t value)	nxpt	{ return state = bit_set_masked(state, mask, GUI_CONTROL_FLAG_Focused	, value); }
	inxp	GUI_CONTROL_FLAG	setPressed		(GUI_CONTROL_FLAG & state, cnst GUI_CONTROL_FLAG mask, b8_t value)	nxpt	{ 
		return (value == false && bit_test_masked(state, mask, GUI_CONTROL_FLAG_Pressed) && bit_test_masked(state, mask, GUI_CONTROL_FLAG_Hovered) && false == bit_test_masked(state, mask, GUI_CONTROL_FLAG_Dragging))
			? state = bit_set_masked(bit_set_masked(state, mask, GUI_CONTROL_FLAG_Pressed, value), mask, GUI_CONTROL_FLAG_Action, true)
			: state = bit_set_masked(state, mask, GUI_CONTROL_FLAG_Pressed, value)
			; 
	}
	inxp	GUI_CONTROL_FLAG	setDragging		(GUI_CONTROL_FLAG & state, cnst GUI_CONTROL_FLAG mask, b8_t value)	nxpt	{ return state = bit_set_masked(state, mask, GUI_CONTROL_FLAG_Dragging	, value); }
	inxp	GUI_CONTROL_FLAG	setSelected		(GUI_CONTROL_FLAG & state, cnst GUI_CONTROL_FLAG mask, b8_t value)	nxpt	{ return state = bit_set_masked(state, mask, GUI_CONTROL_FLAG_Selected	, value); }
	inxp	GUI_CONTROL_FLAG	setUpdated		(GUI_CONTROL_FLAG & state, cnst GUI_CONTROL_FLAG mask, b8_t value)	nxpt	{ return state = bit_set_masked(state, mask, GUI_CONTROL_FLAG_Updated	, value); }
	inxp	GUI_CONTROL_FLAG	setAction		(GUI_CONTROL_FLAG & state, cnst GUI_CONTROL_FLAG mask, b8_t value)	nxpt	{ return state = bit_set_masked(state, mask, GUI_CONTROL_FLAG_Action	, value); }

	CTTI_ENUM(EVENT_GUI_CONTROL,
	enum EVENT_GUI_CONTROL : u0_t
		{ EVENT_GUI_CONTROL_StateChange = 2
		};
	);

	CTTI_STCT(SChangeControlState,
	struct SChangeControlState { 
		cid_t				Id		= -1; 
		GUI_CONTROL_FLAG	Clear	= {}; 
		GUI_CONTROL_FLAG	Set		= {};
	};
	);

	stin	gpk::err_t eventEnqueueGuiControlStateChange(apobj<SEventSystem> & queue, cid_t iControl, GUI_CONTROL_FLAG oldState, GUI_CONTROL_FLAG newState) {
		SChangeControlState			stateChange				= {iControl};
		stateChange.Clear		= bit_clear(oldState, newState);
		stateChange.Set			= bit_clear(newState, oldState);
		gpk_gui_printf("Control state change: %i", iControl);
		for(u2_t i = 0; i < 8 * sizeof(GUI_CONTROL_FLAG); ++i) {
			cnst GUI_CONTROL_FLAG bit = bit_make<GUI_CONTROL_FLAG>((u0_t)i);
			if(bit & stateChange.Set  ) gpk_gui_printf("State set  : (0x%X) %s", (u2_t)bit, get_value_namep(bit));
			if(bit & stateChange.Clear) gpk_gui_printf("State clear: (0x%X) %s", (u2_t)bit, get_value_namep(bit));
		}
		return eventEnqueueChild(queue, SYSTEM_EVENT_GUI, EVENT_GUI_CONTROL_StateChange, stateChange); 
	}

	tplt<tpnm _tState>
	usng TFuncSetMaskedValue = function<_tState(void)>; 

	tplt<tpnm _tState>
	_tState				setValue		(cid_t iControl, apobj<SEventSystem> & eventQueue, _tState & state, TFuncSetMaskedValue<_tState> funcSetValue) {
		cnst _tState			oldState		= state;
		state				= funcSetValue();
		if(oldState != state)
			if_fail_e(eventEnqueueGuiControlStateChange(eventQueue, iControl, oldState, state));
		return state;
	}

	CTTI_STCT(SControlState,
	struct SControlState {
		cid_t				Parent			= -1;
		GUI_CONTROL_FLAG	Flag			= {};
		GUI_CONTROL_FLAG	Mask			= (GUI_CONTROL_FLAG)-1;

		b8_t				SetHovered		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setValue<GUI_CONTROL_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setHovered (Flag, Mask, value); })); }
		b8_t				SetPressed		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setValue<GUI_CONTROL_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setPressed (Flag, Mask, value); })); }
		b8_t				SetHidden		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setValue<GUI_CONTROL_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setHidden  (Flag, Mask, value); })); }
		b8_t				SetFocused		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setValue<GUI_CONTROL_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setFocused (Flag, Mask, value); })); }
		b8_t				SetSelected		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setValue<GUI_CONTROL_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setSelected(Flag, Mask, value); })); }
		b8_t				SetDisabled		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setValue<GUI_CONTROL_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setDisabled(Flag, Mask, value); })); }
		b8_t				SetUpdated		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setValue<GUI_CONTROL_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setUpdated (Flag, Mask, value); })); }
		b8_t				SetDragging		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setValue<GUI_CONTROL_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setDragging(Flag, Mask, value); })); }
		b8_t				SetAction		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setValue<GUI_CONTROL_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setAction  (Flag, Mask, value); })); }
		b8_t				SetUnused		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setValue<GUI_CONTROL_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setUnused  (Flag, value);       })); }
		inxp	b8_t		IsHovered		()	csnx	{ return isHovered  (Flag, Mask); }
		inxp	b8_t		IsPressed		()	csnx	{ return isPressed  (Flag, Mask); }
		inxp	b8_t		IsHidden		()	csnx	{ return isHidden   (Flag, Mask); }
		inxp	b8_t		IsFocused		()	csnx	{ return isFocused  (Flag, Mask); }
		inxp	b8_t		IsSelected		()	csnx	{ return isSelected (Flag, Mask); }
		inxp	b8_t		IsDisabled		()	csnx	{ return isDisabled (Flag, Mask); }
		inxp	b8_t		IsUpdated		()	csnx	{ return isUpdated  (Flag, Mask); }
		inxp	b8_t		IsDragging		()	csnx	{ return isDragging (Flag, Mask); }
		inxp	b8_t		IsAction		()	csnx	{ return isAction   (Flag, Mask); }
		inxp	b8_t		IsUnused		()	csnx	{ return isUnused   (Flag); }
	};
	);
	stxp	size_t SIZEOF_SCONTROL	= sizeof(SControlState);
#pragma pack(pop)

} // nmsp

#endif // GPK_GUI_CONTROL_FLAG_H_23627
