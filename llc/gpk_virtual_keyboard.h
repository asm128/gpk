#include "gpk_event_input.h"
#include "gpk_gui_control_state.h"
#include "gpk_system_key.h"

#ifndef GPK_VIRTUAL_KEYBOARD_H_23627
#define GPK_VIRTUAL_KEYBOARD_H_23627

namespace gpk
{
	stct SVirtualKeyboardCharacter {
		::gpk::cid_t	IdControl	= ::gpk::CID_INVALID;
		::gpk::u2_t	Character	= {};
	};
	stct SVirtualKeyboardCommand {
		::gpk::cid_t		IdControl	= ::gpk::CID_INVALID;
		::gpk::SYSTEM_KEY	Key			= {};
	};

	stct SVirtualKeyboard {
		::gpk::cid_t						IdRoot		= ::gpk::CID_INVALID;
		::gpk::apod<::gpk::SVirtualKeyboardCharacter>	Characters	= {};
		::gpk::apod<::gpk::SVirtualKeyboardCommand>	Commands	= {};
	};

	::gpk::err_t	virtualKeyboardHandleEvent	(cnst ::gpk::SVirtualKeyboard & keyboard, ::gpk::cid_t iControl, ::gpk::TQueueSystemEvent & events);
} // namespace gpk

#endif // GPK_VIRTUAL_KEYBOARD_H_23627


