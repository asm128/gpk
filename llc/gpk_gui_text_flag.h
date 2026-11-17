#include "gpk_gui_control_flag.h"

#ifndef GPK_GUI_TEXT_FLAG_H_23627
#define GPK_GUI_TEXT_FLAG_H_23627

nmsp gpk
{
	CTTI_ENUM(GUI_TEXT_FLAG,
	enum GUI_TEXT_FLAG : u1_t
		{ GUI_TEXT_Selected			= 0x01U
		, GUI_TEXT_Cursor			= 0x02U
		, GUI_TEXT_Swap_colors		= 0x04U
		, GUI_TEXT_Bold				= 0x08U
		, GUI_TEXT_Underline			= 0x10U
		, GUI_TEXT_Strikethrough		= 0x20U
		, GUI_TEXT_Italic			= 0x40U
		};
	);
	stxp GUI_TEXT_FLAG GUI_TEXT_FLAG_All = (GUI_TEXT_FLAG)-1;

	inxp GUI_TEXT_FLAG isSelected			(cnst GUI_TEXT_FLAG state, cnst GUI_TEXT_FLAG mask) nxpt { return bit_test_masked(state, mask, GUI_TEXT_Selected			); }
	inxp GUI_TEXT_FLAG isCursor			(cnst GUI_TEXT_FLAG state, cnst GUI_TEXT_FLAG mask) nxpt { return bit_test_masked(state, mask, GUI_TEXT_Cursor			); }
	inxp GUI_TEXT_FLAG isSwapColors			(cnst GUI_TEXT_FLAG state, cnst GUI_TEXT_FLAG mask) nxpt { return bit_test_masked(state, mask, GUI_TEXT_Swap_colors			); }
	inxp GUI_TEXT_FLAG isBold				(cnst GUI_TEXT_FLAG state, cnst GUI_TEXT_FLAG mask) nxpt { return bit_test_masked(state, mask, GUI_TEXT_Bold				); }
	inxp GUI_TEXT_FLAG isUnderline			(cnst GUI_TEXT_FLAG state, cnst GUI_TEXT_FLAG mask) nxpt { return bit_test_masked(state, mask, GUI_TEXT_Underline			); }
	inxp GUI_TEXT_FLAG isStrikethrough		(cnst GUI_TEXT_FLAG state, cnst GUI_TEXT_FLAG mask) nxpt { return bit_test_masked(state, mask, GUI_TEXT_Strikethrough		); }
	inxp GUI_TEXT_FLAG isItalic				(cnst GUI_TEXT_FLAG state, cnst GUI_TEXT_FLAG mask) nxpt { return bit_test_masked(state, mask, GUI_TEXT_Italic				); }

	inxp GUI_TEXT_FLAG setSelected			(GUI_TEXT_FLAG & state, cnst GUI_TEXT_FLAG mask, b8_t value) nxpt { return state = bit_set_masked(state, mask, GUI_TEXT_Selected			, value); }
	inxp GUI_TEXT_FLAG setCursor			(GUI_TEXT_FLAG & state, cnst GUI_TEXT_FLAG mask, b8_t value) nxpt { return state = bit_set_masked(state, mask, GUI_TEXT_Cursor			, value); }
	inxp GUI_TEXT_FLAG setSwapColors			(GUI_TEXT_FLAG & state, cnst GUI_TEXT_FLAG mask, b8_t value) nxpt { return state = bit_set_masked(state, mask, GUI_TEXT_Swap_colors			, value); }
	inxp GUI_TEXT_FLAG setBold				(GUI_TEXT_FLAG & state, cnst GUI_TEXT_FLAG mask, b8_t value) nxpt { return state = bit_set_masked(state, mask, GUI_TEXT_Bold				, value); }
	inxp GUI_TEXT_FLAG setUnderline			(GUI_TEXT_FLAG & state, cnst GUI_TEXT_FLAG mask, b8_t value) nxpt { return state = bit_set_masked(state, mask, GUI_TEXT_Underline			, value); }
	inxp GUI_TEXT_FLAG setStrikethrough		(GUI_TEXT_FLAG & state, cnst GUI_TEXT_FLAG mask, b8_t value) nxpt { return state = bit_set_masked(state, mask, GUI_TEXT_Strikethrough		, value); }
	inxp GUI_TEXT_FLAG setItalic				(GUI_TEXT_FLAG & state, cnst GUI_TEXT_FLAG mask, b8_t value) nxpt { return state = bit_set_masked(state, mask, GUI_TEXT_Italic				, value); }

	CTTI_ENUM(EVENT_GUI_TEXT,
	enum EVENT_GUI_TEXT : u0_t
		{ EVENT_GUI_TEXT_StateChange = 3
		};
	);

	CTTI_STCT(SChangeTextState,
	struct SChangeTextState {
		cid_t			IdControl	= -1;
		u2_t			IdCharacter	= (u2_t)-1;
		GUI_TEXT_FLAG	Clear		= {};
		GUI_TEXT_FLAG	Set			= {};
	};
	);

	stin err_t eventEnqueueGuiTextStateChange(apobj<SEventSystem> & queue, cid_t iControl, u2_t iCharacter, GUI_TEXT_FLAG oldState, GUI_TEXT_FLAG newState) {
		SChangeTextState stateChange = {iControl, iCharacter};
		stateChange.Clear = bit_clear(oldState, newState);
		stateChange.Set = bit_clear(newState, oldState);
		return eventEnqueueChild(queue, SYSTEM_EVENT_GUI, EVENT_GUI_TEXT_StateChange, stateChange);
	}

	CTTI_STCT(STextState,
	struct STextState {
		cid_t			Parent	= -1;	// Logical text-state parent; unrelated to the spatial control hierarchy.
		GUI_TEXT_FLAG	Flag	= {};
		GUI_TEXT_FLAG	Mask	= GUI_TEXT_FLAG_All;

		b8_t SetSelected(cid_t iControl, u2_t iCharacter, apobj<SEventSystem> & eventQueue, b8_t value) nxpt {
			cnst GUI_TEXT_FLAG oldState = Flag;
			Flag = setSelected(Flag, Mask, value);
			if(oldState != Flag)
				if_fail_e(eventEnqueueGuiTextStateChange(eventQueue, iControl, iCharacter, oldState, Flag));
			return IsSelected();
		}
		b8_t SetCursor(cid_t iControl, u2_t iCharacter, apobj<SEventSystem> & eventQueue, b8_t value) nxpt {
			cnst GUI_TEXT_FLAG oldState = Flag;
			Flag = setCursor(Flag, Mask, value);
			if(oldState != Flag)
				if_fail_e(eventEnqueueGuiTextStateChange(eventQueue, iControl, iCharacter, oldState, Flag));
			return IsCursor();
		}
		b8_t SetSwapColors(cid_t iControl, u2_t iCharacter, apobj<SEventSystem> & eventQueue, b8_t value) nxpt {
			cnst GUI_TEXT_FLAG oldState = Flag;
			Flag = setSwapColors(Flag, Mask, value);
			if(oldState != Flag)
				if_fail_e(eventEnqueueGuiTextStateChange(eventQueue, iControl, iCharacter, oldState, Flag));
			return IsSwapColors();
		}
		b8_t SetBold(cid_t iControl, u2_t iCharacter, apobj<SEventSystem> & eventQueue, b8_t value) nxpt {
			cnst GUI_TEXT_FLAG oldState = Flag;
			Flag = setBold(Flag, Mask, value);
			if(oldState != Flag)
				if_fail_e(eventEnqueueGuiTextStateChange(eventQueue, iControl, iCharacter, oldState, Flag));
			return IsBold();
		}
		b8_t SetUnderline(cid_t iControl, u2_t iCharacter, apobj<SEventSystem> & eventQueue, b8_t value) nxpt {
			cnst GUI_TEXT_FLAG oldState = Flag;
			Flag = setUnderline(Flag, Mask, value);
			if(oldState != Flag)
				if_fail_e(eventEnqueueGuiTextStateChange(eventQueue, iControl, iCharacter, oldState, Flag));
			return IsUnderline();
		}
		b8_t SetStrikethrough(cid_t iControl, u2_t iCharacter, apobj<SEventSystem> & eventQueue, b8_t value) nxpt {
			cnst GUI_TEXT_FLAG oldState = Flag;
			Flag = setStrikethrough(Flag, Mask, value);
			if(oldState != Flag)
				if_fail_e(eventEnqueueGuiTextStateChange(eventQueue, iControl, iCharacter, oldState, Flag));
			return IsStrikethrough();
		}
		b8_t SetItalic(cid_t iControl, u2_t iCharacter, apobj<SEventSystem> & eventQueue, b8_t value) nxpt {
			cnst GUI_TEXT_FLAG oldState = Flag;
			Flag = setItalic(Flag, Mask, value);
			if(oldState != Flag)
				if_fail_e(eventEnqueueGuiTextStateChange(eventQueue, iControl, iCharacter, oldState, Flag));
			return IsItalic();
		}
		inxp b8_t IsSelected			() csnx { return isSelected			(Flag, Mask); }
		inxp b8_t IsCursor				() csnx { return isCursor			(Flag, Mask); }
		inxp b8_t IsSwapColors			() csnx { return isSwapColors			(Flag, Mask); }
		inxp b8_t IsBold					() csnx { return isBold				(Flag, Mask); }
		inxp b8_t IsUnderline				() csnx { return isUnderline			(Flag, Mask); }
		inxp b8_t IsStrikethrough			() csnx { return isStrikethrough		(Flag, Mask); }
		inxp b8_t IsItalic					() csnx { return isItalic				(Flag, Mask); }
	};
	);
} // nmsp

#endif // GPK_GUI_TEXT_FLAG_H_23627
