#define GPK_INFO_PRINTF_ENABLED

#include "gpk_virtual_keyboard.h"

gpk::err_t 		gpk::virtualKeyboardHandleEvent	(cnst SVirtualKeyboard & vk, cid_t iControl, apod<SVirtualKeyboardEvent> & events) {
	cnst cid_t			idKeyFirst			= vk.IdRoot + 1;
	cnst cid_t			idKeyStop			= cid_t(idKeyFirst + vk.Keys.size());
	if(in_range<cid_t>(iControl, idKeyFirst, idKeyStop)) {
		s2_c				iKey				= iControl - idKeyFirst;
		u1_c				key					= vk.Keys[iKey];
		info_printf("Virtual key %i: %c (%i)", iKey, key, key);
		return 1 + events.push_back({VK_EVENT_Release, VK_SCANCODE(key)});
	}
	else if(in_range<s2_t>(iControl, idKeyStop, idKeyStop + get_value_count<VK_SCANCODE>())) {
		cnst VK_SCANCODE	key					= VK_SCANCODE(iControl - idKeyStop);
		info_printf("Virtual scancode: %s (%i)", get_value_namep(key), key);
		return 1 + events.push_back({VK_EVENT_Edit, key});
	}
	return 0;
}
