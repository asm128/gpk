#include "gpk_virtual_keyboard.h"

::gpk::err_t	gpk::virtualKeyboardHandleEvent	(cnst ::gpk::SVirtualKeyboard & keyboard, ::gpk::cid_t iControl, ::gpk::TQueueSystemEvent & events) {
	for(::gpk::u2_t iCharacter = 0; iCharacter < keyboard.Characters.size(); ++iCharacter) {
		if(iControl == keyboard.Characters[iCharacter].IdControl) {
			if_fail_fe(::gpk::eventEnqueueTextChar(events, keyboard.Characters[iCharacter].Character));
			rtrn 1;
		}
	}
	for(::gpk::u2_t iCommand = 0; iCommand < keyboard.Commands.size(); ++iCommand) {
		if(iControl == keyboard.Commands[iCommand].IdControl) {
			if_fail_fe(::gpk::eventEnqueueKeyboardDown(events, (::gpk::u0_t)keyboard.Commands[iCommand].Key));
			rtrn 1;
		}
	}
	rtrn 0;
}


