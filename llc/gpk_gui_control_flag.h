#include "gpk_gui_core.h"
#include "gpk_enum.h"

#ifndef GPK_GUI_CONTROL_FLAG_H_23627
#define GPK_GUI_CONTROL_FLAG_H_23627

nmsp gpk
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE(GUI_CONTROL_FLAG, u1_t);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_FLAG, Hidden, 0x01U);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_FLAG, Disabled, 0x02U);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_FLAG, Hovered, 0x04U);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_FLAG, Focused, 0x08U);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_FLAG, Pressed, 0x10U);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_FLAG, Dragging, 0x20U);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_FLAG, Selected, 0x40U);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_FLAG, Updated, 0x80U);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_FLAG, Action, 0x100U);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_FLAG, Unused, 0xFFFFU);
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

	GDEFINE_ENUM_TYPE(EVENT_GUI_CONTROL, u0_t);
	GDEFINE_ENUM_VALUE(EVENT_GUI_CONTROL, StateChange, 2);

	struct SChangeControlState { 
		cid_t				Id		= -1; 
		GUI_CONTROL_FLAG	Clear	= {}; 
		GUI_CONTROL_FLAG	Set		= {};
	};

	stin	gpk::err_t eventEnqueueGuiControlStateChange(apobj<SEventSystem> & queue, cid_t iControl, GUI_CONTROL_FLAG oldState, GUI_CONTROL_FLAG newState) {
		SChangeControlState			stateChange				= {iControl};
		stateChange.Clear		= bit_clear(oldState, newState);
		stateChange.Set			= bit_clear(newState, oldState);
		gpk_gui_printf("Control state change: %i", iControl);
		for(u2_t i = 0; i < 8 * sizeof(GUI_CONTROL_FLAG); ++i) {
			cnst GUI_CONTROL_FLAG bit = bit_make<GUI_CONTROL_FLAG>((u0_t)i);
			if(bit & stateChange.Set  ) { gpk_gui_printf("State set  : (0x%X) %s", (u2_t)bit, get_value_namep(bit)); }
			if(bit & stateChange.Clear) { gpk_gui_printf("State clear: (0x%X) %s", (u2_t)bit, get_value_namep(bit)); }
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
	stxp	size_t SIZEOF_SCONTROL	= sizeof(SControlState);
#pragma pack(pop)

} // nmsp

#endif // GPK_GUI_CONTROL_FLAG_H_23627
