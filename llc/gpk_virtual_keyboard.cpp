#define GPK_INFO_PRINTF_ENABLED

#include "gpk_virtual_keyboard.h"

::gpk::error_t			gpk::virtualKeyboardHandleEvent	(const ::gpk::SVirtualKeyboard & vk, ::gpk::cid_t iControl, ::gpk::apod<::gpk::SVirtualKeyboardEvent> & events) {
	const ::gpk::cid_t			idKeyFirst			= vk.IdRoot + 1;
	const ::gpk::cid_t			idKeyStop			= ::gpk::cid_t(idKeyFirst + vk.Keys.size());
	if(::gpk::in_range<::gpk::cid_t>(iControl, idKeyFirst, idKeyStop)) {
		const int32_t				iKey				= iControl - idKeyFirst;
		const uint16_t				key					= vk.Keys[iKey];
		info_printf("Virtual key %i: %c (%i)", iKey, key, key);
		return 1 + events.push_back({::gpk::VK_EVENT_RELEASE, ::gpk::VK_SCANCODE(key)});
	}
	else if(::gpk::in_range<int32_t>(iControl, idKeyStop, idKeyStop + ::gpk::get_value_count<::gpk::VK_SCANCODE>())) {
		const ::gpk::VK_SCANCODE	key					= ::gpk::VK_SCANCODE(iControl - idKeyStop);
		info_printf("Virtual scancode: %s (%i)", ::gpk::get_value_label(key).begin(), key);
		return 1 + events.push_back({::gpk::VK_EVENT_EDIT, key});
	}
	return 0;
}
