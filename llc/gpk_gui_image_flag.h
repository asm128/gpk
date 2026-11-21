#include "gpk_gui_core.h"

#define gpk_gui_printf verbose_printf

#ifndef GPK_GUI_IMAGE_FLAG_H_23627
#define GPK_GUI_IMAGE_FLAG_H_23627

nmsp gpk
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE(GUI_IMAGE_FLAG, u1_t);
	GDEFINE_ENUM_VALUE(GUI_IMAGE_FLAG, Hidden, 0x01U); // Prevents from drawing the control and its children
	GDEFINE_ENUM_VALUE(GUI_IMAGE_FLAG, Disabled, 0x02U); // Prevents the control from responding to user interaction
	GDEFINE_ENUM_VALUE(GUI_IMAGE_FLAG, Hovered, 0x04U); // Modifies behavior depending on whether the mouse pointer top is inside the control area.
	GDEFINE_ENUM_VALUE(GUI_IMAGE_FLAG, Focused, 0x08U); // Modified behavior depending on wether the control is selected for receiving user input.
	GDEFINE_ENUM_VALUE(GUI_IMAGE_FLAG, Pressed, 0x10U); // Modifies behavior depending on whether the control is being pressed by holding the mouse button, a key or a touch event.
	GDEFINE_ENUM_VALUE(GUI_IMAGE_FLAG, Dragging, 0x20U);
	GDEFINE_ENUM_VALUE(GUI_IMAGE_FLAG, Selected, 0x40U);
	GDEFINE_ENUM_VALUE(GUI_IMAGE_FLAG, Updated, 0x80U);
	GDEFINE_ENUM_VALUE(GUI_IMAGE_FLAG, Action, 0x100U);
	GDEFINE_ENUM_VALUE(GUI_IMAGE_FLAG, Unused, 0xFFFFU);
	stxp	GUI_IMAGE_FLAG	GUI_IMAGE_FLAG_All	= GUI_IMAGE_FLAG_Unused;
	inxp	GUI_IMAGE_FLAG	setFlag			(GUI_IMAGE_FLAG & state, cnst GUI_IMAGE_FLAG mask, b8_t value)	nxpt	{ return state = bit_set_masked(state, mask, GUI_IMAGE_FLAG_Hidden	, value); }

	inxp	GUI_IMAGE_FLAG	isUnused		(cnst GUI_IMAGE_FLAG state)										nxpt	{ return bit_test(state, GUI_IMAGE_FLAG_Unused); }
	inxp	GUI_IMAGE_FLAG	isHidden		(cnst GUI_IMAGE_FLAG state, cnst GUI_IMAGE_FLAG mask)			nxpt	{ return bit_test_masked(state, mask, GUI_IMAGE_FLAG_Hidden	); }
	inxp	GUI_IMAGE_FLAG	isDisabled		(cnst GUI_IMAGE_FLAG state, cnst GUI_IMAGE_FLAG mask)			nxpt	{ return bit_test_masked(state, mask, GUI_IMAGE_FLAG_Disabled	); }
	inxp	GUI_IMAGE_FLAG	isHovered		(cnst GUI_IMAGE_FLAG state, cnst GUI_IMAGE_FLAG mask)			nxpt	{ return bit_test_masked(state, mask, GUI_IMAGE_FLAG_Hovered	); }
	inxp	GUI_IMAGE_FLAG	isFocused		(cnst GUI_IMAGE_FLAG state, cnst GUI_IMAGE_FLAG mask)			nxpt	{ return bit_test_masked(state, mask, GUI_IMAGE_FLAG_Focused	); }
	inxp	GUI_IMAGE_FLAG	isPressed		(cnst GUI_IMAGE_FLAG state, cnst GUI_IMAGE_FLAG mask)			nxpt	{ return bit_test_masked(state, mask, GUI_IMAGE_FLAG_Pressed	); }
	inxp	GUI_IMAGE_FLAG	isDragging		(cnst GUI_IMAGE_FLAG state, cnst GUI_IMAGE_FLAG mask)			nxpt	{ return bit_test_masked(state, mask, GUI_IMAGE_FLAG_Dragging	); }
	inxp	GUI_IMAGE_FLAG	isSelected		(cnst GUI_IMAGE_FLAG state, cnst GUI_IMAGE_FLAG mask)			nxpt	{ return bit_test_masked(state, mask, GUI_IMAGE_FLAG_Selected	); }
	inxp	GUI_IMAGE_FLAG	isUpdated		(cnst GUI_IMAGE_FLAG state, cnst GUI_IMAGE_FLAG mask)			nxpt	{ return bit_test_masked(state, mask, GUI_IMAGE_FLAG_Updated	); }
	inxp	GUI_IMAGE_FLAG	isAction		(cnst GUI_IMAGE_FLAG state, cnst GUI_IMAGE_FLAG mask)			nxpt	{ return bit_test_masked(state, mask, GUI_IMAGE_FLAG_Action	); }

	inxp	GUI_IMAGE_FLAG	setUnused		(GUI_IMAGE_FLAG & state, b8_t value)								nxpt	{ return state = bit_set(state, GUI_IMAGE_FLAG_Unused, value); }
	inxp	GUI_IMAGE_FLAG	setHidden		(GUI_IMAGE_FLAG & state, cnst GUI_IMAGE_FLAG mask, b8_t value)	nxpt	{ return state = bit_set_masked(state, mask, GUI_IMAGE_FLAG_Hidden	, value); }
	inxp	GUI_IMAGE_FLAG	setDisabled		(GUI_IMAGE_FLAG & state, cnst GUI_IMAGE_FLAG mask, b8_t value)	nxpt	{ return state = bit_set_masked(state, mask, GUI_IMAGE_FLAG_Disabled	, value); }
	inxp	GUI_IMAGE_FLAG	setHovered		(GUI_IMAGE_FLAG & state, cnst GUI_IMAGE_FLAG mask, b8_t value)	nxpt	{ return state = bit_set_masked(state, mask, GUI_IMAGE_FLAG_Hovered	, value); }
	inxp	GUI_IMAGE_FLAG	setFocused		(GUI_IMAGE_FLAG & state, cnst GUI_IMAGE_FLAG mask, b8_t value)	nxpt	{ return state = bit_set_masked(state, mask, GUI_IMAGE_FLAG_Focused	, value); }
	inxp	GUI_IMAGE_FLAG	setPressed		(GUI_IMAGE_FLAG & state, cnst GUI_IMAGE_FLAG mask, b8_t value)	nxpt	{ 
		return (value == false && bit_test_masked(state, mask, GUI_IMAGE_FLAG_Pressed) && bit_test_masked(state, mask, GUI_IMAGE_FLAG_Hovered) && false == bit_test_masked(state, mask, GUI_IMAGE_FLAG_Dragging))
			? state = bit_set_masked(bit_set_masked(state, mask, GUI_IMAGE_FLAG_Pressed, value), mask, GUI_IMAGE_FLAG_Action, true)
			: state = bit_set_masked(state, mask, GUI_IMAGE_FLAG_Pressed, value)
			; 
	}
	inxp	GUI_IMAGE_FLAG	setDragging		(GUI_IMAGE_FLAG & state, cnst GUI_IMAGE_FLAG mask, b8_t value)	nxpt	{ return state = bit_set_masked(state, mask, GUI_IMAGE_FLAG_Dragging	, value); }
	inxp	GUI_IMAGE_FLAG	setSelected		(GUI_IMAGE_FLAG & state, cnst GUI_IMAGE_FLAG mask, b8_t value)	nxpt	{ return state = bit_set_masked(state, mask, GUI_IMAGE_FLAG_Selected	, value); }
	inxp	GUI_IMAGE_FLAG	setUpdated		(GUI_IMAGE_FLAG & state, cnst GUI_IMAGE_FLAG mask, b8_t value)	nxpt	{ return state = bit_set_masked(state, mask, GUI_IMAGE_FLAG_Updated	, value); }
	inxp	GUI_IMAGE_FLAG	setAction		(GUI_IMAGE_FLAG & state, cnst GUI_IMAGE_FLAG mask, b8_t value)	nxpt	{ return state = bit_set_masked(state, mask, GUI_IMAGE_FLAG_Action	, value); }

	GDEFINE_ENUM_TYPE(EVENT_GUI_IMAGE, u0_t);
	GDEFINE_ENUM_VALUE(EVENT_GUI_IMAGE, StateChange, 2);

	struct SChangeImageState { 
		cid_t				Id		= -1; 
		GUI_IMAGE_FLAG		Clear	= {}; 
		GUI_IMAGE_FLAG		Set		= {};
	};

	stin	err_t eventEnqueueGuiImageStateChange(apobj<SEventSystem> & queue, cid_t iControl, GUI_IMAGE_FLAG oldState, GUI_IMAGE_FLAG newState) {
		SChangeImageState			stateChange				= {iControl};
		stateChange.Clear		= bit_clear(oldState, newState);
		stateChange.Set			= bit_clear(newState, oldState);
		gpk_gui_printf("Control state change: %i", iControl);
		for(u2_t i = 0; i < 8 * sizeof(GUI_IMAGE_FLAG); ++i) {
			cnst GUI_IMAGE_FLAG bit = bit_make<GUI_IMAGE_FLAG>((u0_t)i);
			if(bit & stateChange.Set  ) { cnst vcst_t name = ::gpk::get_value_namev(bit); gpk_gui_printf("State set  : (0x%X) %.*s", (u2_t)bit, GPK_VIEW_ARGS(name)); }
			if(bit & stateChange.Clear) { cnst vcst_t name = ::gpk::get_value_namev(bit); gpk_gui_printf("State clear: (0x%X) %.*s", (u2_t)bit, GPK_VIEW_ARGS(name)); }
		}
		return eventEnqueueChild(queue, SYSTEM_EVENT_GUI, EVENT_GUI_IMAGE_StateChange, stateChange); 
	}

	tplt<tpnm _tState>
	usng TFuncSetImageMaskedValue = function<_tState(void)>; 

	tplt<tpnm _tState>
	_tState				setImageValue		(cid_t iControl, apobj<SEventSystem> & eventQueue, _tState & state, TFuncSetImageMaskedValue<_tState> funcSetValue) {
		cnst _tState			oldState		= state;
		state				= funcSetValue();
		if(oldState != state)
			if_fail_e(eventEnqueueGuiImageStateChange(eventQueue, iControl, oldState, state));
		return state;
	}

	struct SImageState {
		cid_t				Parent			= -1;
		GUI_IMAGE_FLAG		Flag			= {};
		GUI_IMAGE_FLAG		Mask			= (GUI_IMAGE_FLAG)-1;

		b8_t				SetHovered		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setImageValue<GUI_IMAGE_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setHovered (Flag, Mask, value); })); }
		b8_t				SetPressed		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setImageValue<GUI_IMAGE_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setPressed (Flag, Mask, value); })); }
		b8_t				SetHidden		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setImageValue<GUI_IMAGE_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setHidden  (Flag, Mask, value); })); }
		b8_t				SetFocused		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setImageValue<GUI_IMAGE_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setFocused (Flag, Mask, value); })); }
		b8_t				SetSelected		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setImageValue<GUI_IMAGE_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setSelected(Flag, Mask, value); })); }
		b8_t				SetDisabled		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setImageValue<GUI_IMAGE_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setDisabled(Flag, Mask, value); })); }
		b8_t				SetUpdated		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setImageValue<GUI_IMAGE_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setUpdated (Flag, Mask, value); })); }
		b8_t				SetDragging		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setImageValue<GUI_IMAGE_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setDragging(Flag, Mask, value); })); }
		b8_t				SetAction		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setImageValue<GUI_IMAGE_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setAction  (Flag, Mask, value); })); }
		b8_t				SetUnused		(cid_t iControl, apobj<SEventSystem> & eventQueue, b8_t value)	nxpt	{ return 0 != (Flag = setImageValue<GUI_IMAGE_FLAG>(iControl, eventQueue, Flag, [this, &value]() { return setUnused  (Flag, value);       })); }
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
	stxp	size_t SIZEOF_SIMAGE	= sizeof(SImageState);
#pragma pack(pop)

} // nmsp

#endif // GPK_GUI_IMAGE_FLAG_H_23627
