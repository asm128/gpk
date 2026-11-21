#include "gpk_gui_core.h"

#ifndef GPK_GUI_TEXT_FLAG_H_23627
#define GPK_GUI_TEXT_FLAG_H_23627

nmsp gpk
{
	GDEFINE_ENUM_TYPE(GUI_TEXT_FLAG, u1_t);
	GDEFINE_ENUM_VALUE(GUI_TEXT_FLAG, Selected, 0x01U);
	GDEFINE_ENUM_VALUE(GUI_TEXT_FLAG, Cursor, 0x02U);
	GDEFINE_ENUM_VALUE(GUI_TEXT_FLAG, Swap_colors, 0x04U);
	GDEFINE_ENUM_VALUE(GUI_TEXT_FLAG, Bold, 0x08U);
	GDEFINE_ENUM_VALUE(GUI_TEXT_FLAG, Underline, 0x10U);
	GDEFINE_ENUM_VALUE(GUI_TEXT_FLAG, Strikethrough, 0x20U);
	GDEFINE_ENUM_VALUE(GUI_TEXT_FLAG, Italic, 0x40U);
	stxp GUI_TEXT_FLAG GUI_TEXT_FLAG_All = (GUI_TEXT_FLAG)-1;

	inxp GUI_TEXT_FLAG isSelected			(cnst GUI_TEXT_FLAG state, cnst GUI_TEXT_FLAG mask) nxpt { return bit_test_masked(state, mask, GUI_TEXT_FLAG_Selected			); }
	inxp GUI_TEXT_FLAG isCursor				(cnst GUI_TEXT_FLAG state, cnst GUI_TEXT_FLAG mask) nxpt { return bit_test_masked(state, mask, GUI_TEXT_FLAG_Cursor			); }
	inxp GUI_TEXT_FLAG isSwapColors			(cnst GUI_TEXT_FLAG state, cnst GUI_TEXT_FLAG mask) nxpt { return bit_test_masked(state, mask, GUI_TEXT_FLAG_Swap_colors			); }
	inxp GUI_TEXT_FLAG isBold				(cnst GUI_TEXT_FLAG state, cnst GUI_TEXT_FLAG mask) nxpt { return bit_test_masked(state, mask, GUI_TEXT_FLAG_Bold				); }
	inxp GUI_TEXT_FLAG isUnderline			(cnst GUI_TEXT_FLAG state, cnst GUI_TEXT_FLAG mask) nxpt { return bit_test_masked(state, mask, GUI_TEXT_FLAG_Underline			); }
	inxp GUI_TEXT_FLAG isStrikethrough		(cnst GUI_TEXT_FLAG state, cnst GUI_TEXT_FLAG mask) nxpt { return bit_test_masked(state, mask, GUI_TEXT_FLAG_Strikethrough		); }
	inxp GUI_TEXT_FLAG isItalic				(cnst GUI_TEXT_FLAG state, cnst GUI_TEXT_FLAG mask) nxpt { return bit_test_masked(state, mask, GUI_TEXT_FLAG_Italic				); }

	inxp GUI_TEXT_FLAG setSelected			(GUI_TEXT_FLAG & state, cnst GUI_TEXT_FLAG mask, b8_t value) nxpt { return state = bit_set_masked(state, mask, GUI_TEXT_FLAG_Selected			, value); }
	inxp GUI_TEXT_FLAG setCursor			(GUI_TEXT_FLAG & state, cnst GUI_TEXT_FLAG mask, b8_t value) nxpt { return state = bit_set_masked(state, mask, GUI_TEXT_FLAG_Cursor			, value); }
	inxp GUI_TEXT_FLAG setSwapColors		(GUI_TEXT_FLAG & state, cnst GUI_TEXT_FLAG mask, b8_t value) nxpt { return state = bit_set_masked(state, mask, GUI_TEXT_FLAG_Swap_colors			, value); }
	inxp GUI_TEXT_FLAG setBold				(GUI_TEXT_FLAG & state, cnst GUI_TEXT_FLAG mask, b8_t value) nxpt { return state = bit_set_masked(state, mask, GUI_TEXT_FLAG_Bold				, value); }
	inxp GUI_TEXT_FLAG setUnderline			(GUI_TEXT_FLAG & state, cnst GUI_TEXT_FLAG mask, b8_t value) nxpt { return state = bit_set_masked(state, mask, GUI_TEXT_FLAG_Underline			, value); }
	inxp GUI_TEXT_FLAG setStrikethrough		(GUI_TEXT_FLAG & state, cnst GUI_TEXT_FLAG mask, b8_t value) nxpt { return state = bit_set_masked(state, mask, GUI_TEXT_FLAG_Strikethrough		, value); }
	inxp GUI_TEXT_FLAG setItalic			(GUI_TEXT_FLAG & state, cnst GUI_TEXT_FLAG mask, b8_t value) nxpt { return state = bit_set_masked(state, mask, GUI_TEXT_FLAG_Italic				, value); }

	GDEFINE_ENUM_TYPE(EVENT_GUI_TEXT, u0_t);
	GDEFINE_ENUM_VALUE(EVENT_GUI_TEXT, StateChange, 3);

	struct SChangeTextState {
		cid_t			IdControl	= -1;
		u2_t			IdCharacter	= (u2_t)-1;
		GUI_TEXT_FLAG	Clear		= {};
		GUI_TEXT_FLAG	Set			= {};
	};

	stin err_t eventEnqueueGuiTextStateChange(apobj<SEventSystem> & queue, cid_t iControl, u2_t iCharacter, GUI_TEXT_FLAG oldState, GUI_TEXT_FLAG newState) {
		SChangeTextState stateChange = {iControl, iCharacter};
		stateChange.Clear = bit_clear(oldState, newState);
		stateChange.Set = bit_clear(newState, oldState);
		return eventEnqueueChild(queue, SYSTEM_EVENT_GUI, EVENT_GUI_TEXT_StateChange, stateChange);
	}

	struct STextState {
		cid_t			Parent	= -1;	// Logical text-state parent; unrelated to the spatial control hierarchy.
		GUI_TEXT_FLAG	Flag	= {};
		GUI_TEXT_FLAG	Mask	= GUI_TEXT_FLAG_All;

		inxp b8_t IsSelected			() csnx { return isSelected			(Flag, Mask); }
		inxp b8_t IsCursor				() csnx { return isCursor			(Flag, Mask); }
		inxp b8_t IsSwapColors			() csnx { return isSwapColors		(Flag, Mask); }
		inxp b8_t IsBold				() csnx { return isBold				(Flag, Mask); }
		inxp b8_t IsUnderline			() csnx { return isUnderline		(Flag, Mask); }
		inxp b8_t IsStrikethrough		() csnx { return isStrikethrough	(Flag, Mask); }
		inxp b8_t IsItalic				() csnx { return isItalic			(Flag, Mask); }
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
	};
} // nmsp

#endif // GPK_GUI_TEXT_FLAG_H_23627
