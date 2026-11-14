#define GPK_INFO_PRINTF_ENABLED

#include "gpk_virtual_keyboard.h"

gpk::err_t 		gpk::virtualKeyboardHandleEvent	(cnst ::gpk::SVirtualKeyboard & vk, ::gpk::cid_t iControl, ::gpk::apod<::gpk::SVirtualKeyboardEvent> & events) {
	cnst ::gpk::cid_t			idKeyFirst			= vk.IdRoot + 1;
	cnst ::gpk::cid_t			idKeyStop			= ::gpk::cid_t(idKeyFirst + vk.Keys.size());
	if(::gpk::in_range<::gpk::cid_t>(iControl, idKeyFirst, idKeyStop)) {
		cnst s2_t				iKey				= iControl - idKeyFirst;
		u1_c				key					= vk.Keys[iKey];
		info_printf("Virtual key %i: %c (%i)", iKey, key, key);
		return 1 + events.push_back({::gpk::VK_EVENT_Release, ::gpk::VK_SCANCODE(key)});
	}
	else if(::gpk::in_range<s2_t>(iControl, idKeyStop, idKeyStop + ::gpk::get_value_count<::gpk::VK_SCANCODE>())) {
		cnst ::gpk::VK_SCANCODE	key					= ::gpk::VK_SCANCODE(iControl - idKeyStop);
		info_printf("Virtual scancode: %s (%i)", ::gpk::get_value_namep(key), key);
		return 1 + events.push_back({::gpk::VK_EVENT_Edit, key});
	}
	return 0;
}
