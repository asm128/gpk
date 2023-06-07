#define GPK_INFO_PRINTF_ENABLED

#include "gpk_virtual_keyboard.h"

::gpk::error_t			gpk::virtualKeyboardHandleEvent	(::gpk::SVirtualKeyboard & vk, int32_t iControl) {
	const int32_t					idKeyFirst		= vk.IdRoot + 1;
	const int32_t					idKeyStop		= idKeyFirst + vk.Keys.size();
	if(::gpk::in_range<int32_t>(iControl, idKeyFirst, idKeyStop)) {
		const int32_t					iKey	= iControl - idKeyFirst;
		const uint16_t					key		= vk.Keys[iKey];
		info_printf("Virtual key %i: %c (%i)", iKey, key, key);
		return 1 + vk.Events.push_back({::gpk::VK_EVENT_RELEASE, VK_SCANCODE(key)});
	}
	else if(::gpk::in_range<int32_t>(iControl, idKeyStop, idKeyStop + ::gpk::get_value_count<::gpk::VK_SCANCODE>())) {
		const VK_SCANCODE				key		= VK_SCANCODE(iControl - idKeyStop);
		info_printf("Virtual scancode: %s (%i)", ::gpk::get_value_label(key).begin(), key);
		return 1 + vk.Events.push_back({::gpk::VK_EVENT_EDIT, key});
	}
	return 0;
}
