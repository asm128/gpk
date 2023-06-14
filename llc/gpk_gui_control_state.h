#include "gpk_sysevent.h"

#include "gpk_bit.h"


#ifndef GPK_GUI_CONTROL_STATE_H_29038749823613
#define GPK_GUI_CONTROL_STATE_H_29038749823613

namespace gpk
{
#pragma pack(push, 1)
	typedef	int32_t			cid_t;
	typedef	const cid_t		ccid_t;
	typedef	apod<cid_t>		acid;
	typedef	view<cid_t>		vcid;
	typedef	view<ccid_t>	vccid;

	GDEFINE_ENUM_TYPE(GUI_CONTROL_STATE, uint8_t)
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Hidden	, 0x01U);	// Prevents from drawing the control and its children
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Disabled	, 0x02U);	// Prevents the control from responding to user interaction
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Hovered	, 0x04U);	// Modifies behavior depending on whether the mouse pointer top is inside the control area.
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Pressed	, 0x08U);	// Modifies behavior depending on whether the control is being pressed by holding the mouse button, a key or a touch event.
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Focused	, 0x10U);	// Modified behavior depending on wether the control is selected for receiving user input.
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Selected	, 0x20U);	// 
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Updated	, 0x40U);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE, Unused	, 0xFFU);

	inlcxpr	bool	isHidden		(const GUI_CONTROL_STATE state, const GUI_CONTROL_STATE mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_Hidden		); }
	inlcxpr	bool	isDisabled		(const GUI_CONTROL_STATE state, const GUI_CONTROL_STATE mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_Disabled	); }
	inlcxpr	bool	isHovered		(const GUI_CONTROL_STATE state, const GUI_CONTROL_STATE mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_Hovered		); }
	inlcxpr	bool	isPressed		(const GUI_CONTROL_STATE state, const GUI_CONTROL_STATE mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_Pressed		); }
	inlcxpr	bool	isFocused		(const GUI_CONTROL_STATE state, const GUI_CONTROL_STATE mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_Focused		); }
	inlcxpr	bool	isSelected		(const GUI_CONTROL_STATE state, const GUI_CONTROL_STATE mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_Selected	); }
	inlcxpr	bool	isUpdated		(const GUI_CONTROL_STATE state, const GUI_CONTROL_STATE mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_Updated		); }
	inlcxpr	bool	isUnused		(const GUI_CONTROL_STATE state)											noexcept	{ return bit_test(state, GUI_CONTROL_STATE_Unused); }

	inlcxpr	bool	setHidden		(GUI_CONTROL_STATE & state, const GUI_CONTROL_STATE mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_Hidden	) && bit_set(state, GUI_CONTROL_STATE_Hidden	, value); }
	inlcxpr	bool	setDisabled		(GUI_CONTROL_STATE & state, const GUI_CONTROL_STATE mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_Disabled	) && bit_set(state, GUI_CONTROL_STATE_Disabled	, value); }
	inlcxpr	bool	setHovered		(GUI_CONTROL_STATE & state, const GUI_CONTROL_STATE mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_Hovered	) && bit_set(state, GUI_CONTROL_STATE_Hovered	, value); }
	inlcxpr	bool	setPressed		(GUI_CONTROL_STATE & state, const GUI_CONTROL_STATE mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_Pressed	) && bit_set(state, GUI_CONTROL_STATE_Pressed	, value); }
	inlcxpr	bool	setFocused		(GUI_CONTROL_STATE & state, const GUI_CONTROL_STATE mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_Focused	) && bit_set(state, GUI_CONTROL_STATE_Focused	, value); }
	inlcxpr	bool	setSelected		(GUI_CONTROL_STATE & state, const GUI_CONTROL_STATE mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_Selected	) && bit_set(state, GUI_CONTROL_STATE_Selected	, value); }
	inlcxpr	bool	setUpdated		(GUI_CONTROL_STATE & state, const GUI_CONTROL_STATE mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_Updated	) && bit_set(state, GUI_CONTROL_STATE_Updated	, value); }
	inlcxpr	bool	setUnused		(GUI_CONTROL_STATE & state, const GUI_CONTROL_STATE mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_Unused	) && bit_set(state, GUI_CONTROL_STATE_Unused	, value); }

	GDEFINE_ENUM_TYPE(GUI_CONTROL_STATE_EX, uint8_t)
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE_EX, Open		, 0x01U);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE_EX, Locked		, 0x02U);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE_EX, Expanded	, 0x04U);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE_EX, Minimized	, 0x08U);	// Prevents from drawing the control and its children, draws the icon form of the control if required
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE_EX, Maximized	, 0x10U);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE_EX, FullScreen	, 0x20U);
	GDEFINE_ENUM_VALUE(GUI_CONTROL_STATE_EX, Active		, 0x40U);
	inlcxpr	bool	isOpen				(const GUI_CONTROL_STATE_EX state, const GUI_CONTROL_STATE_EX mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_EX_Open			); }
	inlcxpr	bool	isLocked			(const GUI_CONTROL_STATE_EX state, const GUI_CONTROL_STATE_EX mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_EX_Locked		); }
	inlcxpr	bool	isExpanded			(const GUI_CONTROL_STATE_EX state, const GUI_CONTROL_STATE_EX mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_EX_Expanded		); }
	inlcxpr	bool	isMinimized			(const GUI_CONTROL_STATE_EX state, const GUI_CONTROL_STATE_EX mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_EX_Minimized	); }
	inlcxpr	bool	isMaximized			(const GUI_CONTROL_STATE_EX state, const GUI_CONTROL_STATE_EX mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_EX_Maximized	); }
	inlcxpr	bool	isFullScreen		(const GUI_CONTROL_STATE_EX state, const GUI_CONTROL_STATE_EX mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_EX_FullScreen	); }
	inlcxpr	bool	isActive			(const GUI_CONTROL_STATE_EX state, const GUI_CONTROL_STATE_EX mask)			noexcept	{ return bit_test_masked(state, mask, GUI_CONTROL_STATE_EX_Active		); }

	inlcxpr	bool	setOpen				(GUI_CONTROL_STATE_EX & state, const GUI_CONTROL_STATE_EX mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_EX_Open		) && bit_set(state, GUI_CONTROL_STATE_EX_Open		, value); }
	inlcxpr	bool	setLocked			(GUI_CONTROL_STATE_EX & state, const GUI_CONTROL_STATE_EX mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_EX_Locked		) && bit_set(state, GUI_CONTROL_STATE_EX_Locked		, value); }
	inlcxpr	bool	setExpanded			(GUI_CONTROL_STATE_EX & state, const GUI_CONTROL_STATE_EX mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_EX_Expanded	) && bit_set(state, GUI_CONTROL_STATE_EX_Expanded	, value); }
	inlcxpr	bool	setMinimized		(GUI_CONTROL_STATE_EX & state, const GUI_CONTROL_STATE_EX mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_EX_Minimized	) && bit_set(state, GUI_CONTROL_STATE_EX_Minimized	, value); }
	inlcxpr	bool	setMaximized		(GUI_CONTROL_STATE_EX & state, const GUI_CONTROL_STATE_EX mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_EX_Maximized	) && bit_set(state, GUI_CONTROL_STATE_EX_Maximized	, value); }
	inlcxpr	bool	setFullScreen		(GUI_CONTROL_STATE_EX & state, const GUI_CONTROL_STATE_EX mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_EX_FullScreen	) && bit_set(state, GUI_CONTROL_STATE_EX_FullScreen	, value); }
	inlcxpr	bool	setActive			(GUI_CONTROL_STATE_EX & state, const GUI_CONTROL_STATE_EX mask, bool value)	noexcept	{ return bit_test(mask, GUI_CONTROL_STATE_EX_Active		) && bit_set(state, GUI_CONTROL_STATE_EX_Active		, value); }

	GDEFINE_ENUM_TYPE (GUI_CONTROL_EVENT, uint16_t)
	GDEFINE_ENUM_VALUE(GUI_CONTROL_EVENT, StateChange, 0x000);


	// Holds the state changes after an update call. 
	struct SControlEvent {
		bool			MouseIn				: 1;
		bool			MouseOut			: 1;
		bool			Pushed				: 1;
		bool			Released			: 1;
		bool			Show				: 1;
		bool			Hide				: 1;
		bool			FocusIn				: 1;
		bool			FocusOut			: 1;

		bool			SelectIn			: 1;
		bool			SelectOut			: 1;
		bool			Open				: 1;
		bool			Close				: 1;
		bool			Expand				: 1;
		bool			Collapse			: 1;

		bool			Execute				: 1;
		bool			Resize				: 1;
		bool			Create				: 1;

		bool			HandleStateChange	(const GUI_CONTROL_STATE oldState, const GUI_CONTROL_STATE newState) {
			bool				handled				= false;
			if(bit_test(oldState, GUI_CONTROL_STATE_Hovered  ) != bit_test(newState, GUI_CONTROL_STATE_Hovered  ))	(bit_test(newState, GUI_CONTROL_STATE_Hovered) ? MouseIn : MouseOut ) = handled = true;
			if(bit_test(oldState, GUI_CONTROL_STATE_Pressed  ) != bit_test(newState, GUI_CONTROL_STATE_Pressed  ))	handled = bit_test(newState, GUI_CONTROL_STATE_Pressed) ? Pushed = true : Released = Execute = true;
			if(bit_test(oldState, GUI_CONTROL_STATE_Hidden   ) != bit_test(newState, GUI_CONTROL_STATE_Hidden   ))	(bit_test(newState, GUI_CONTROL_STATE_Hidden   )	 ? Hide    : Hide     ) = handled = true;
			if(bit_test(oldState, GUI_CONTROL_STATE_Focused  ) != bit_test(newState, GUI_CONTROL_STATE_Focused  ))	(bit_test(newState, GUI_CONTROL_STATE_Focused  )	 ? FocusIn : FocusOut ) = handled = true;
			if(bit_test(oldState, GUI_CONTROL_STATE_Selected ) != bit_test(newState, GUI_CONTROL_STATE_Selected ))	(bit_test(newState, GUI_CONTROL_STATE_Selected )	 ? SelectIn: SelectOut) = handled = true;
			if(bit_test(oldState, GUI_CONTROL_STATE_Disabled ) != bit_test(newState, GUI_CONTROL_STATE_Disabled ))	(bit_test(newState, GUI_CONTROL_STATE_Disabled )	 ? Expand  : Collapse ) = handled = true;
			if(bit_test(oldState, GUI_CONTROL_STATE_Updated  ) != bit_test(newState, GUI_CONTROL_STATE_Updated  ))	(bit_test(newState, GUI_CONTROL_STATE_Updated  )	 ? Resize  : Resize   ) = handled = true;
			if(bit_test(oldState, GUI_CONTROL_STATE_Unused   ) != bit_test(newState, GUI_CONTROL_STATE_Unused	) && false == bit_test(newState, GUI_CONTROL_STATE_Unused))
				Create = handled	= true;
			return handled;
		}
	};

	struct SControlState {
		cid_t				Parent			= -1;
		GUI_CONTROL_STATE	State			= {};
		GUI_CONTROL_STATE	Mask			= (GUI_CONTROL_STATE)-1;

#define CONTROL_STATE_SET_FUNCTION(MemberName)					\
	if(false == bit_test(Mask, GUI_CONTROL_STATE_##MemberName))	\
		return false;											\
	const GUI_CONTROL_STATE	oldState = State;					\
	set##MemberName(State, Mask, value);						\
	events.HandleStateChange(oldState, State);					\
	return bit_test(State, GUI_CONTROL_STATE_##MemberName)

		bool				SetHovered		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Hovered  ); }
		bool				SetPressed		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Pressed  ); }
		bool				SetHidden		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Hidden   ); }
		bool				SetFocused		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Focused  ); }
		bool				SetSelected		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Selected ); }
		bool				SetDisabled		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Disabled ); }
		bool				SetUpdated		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Updated  ); }
		bool				SetUnused		(SControlEvent & events, bool value)	noexcept	{ CONTROL_STATE_SET_FUNCTION(Unused   ); }

		inlcxpr	bool		IsHovered		()	const	noexcept	{ return isHovered  (State, Mask); }
		inlcxpr	bool		IsPressed		()	const	noexcept	{ return isPressed  (State, Mask); }
		inlcxpr	bool		IsHidden		()	const	noexcept	{ return isHidden   (State, Mask); }
		inlcxpr	bool		IsFocused		()	const	noexcept	{ return isFocused  (State, Mask); }
		inlcxpr	bool		IsSelected		()	const	noexcept	{ return isSelected (State, Mask); }
		inlcxpr	bool		IsDisabled		()	const	noexcept	{ return isDisabled (State, Mask); }
		inlcxpr	bool		IsUpdated		()	const	noexcept	{ return isUpdated  (State, Mask); }
		inlcxpr	bool		IsUnused		()	const	noexcept	{ return isUnused   (State); }

	};
	stacxpr	size_t SIZEOF_SCONTROL	= sizeof(SControlState);
#pragma pack(pop)

} // namespace

#endif // GPK_GUI_CONTROL_STATE_H_29038749823613
