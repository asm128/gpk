#include "gpk_windows.h"

#if defined(GPK_WINDOWS)
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	include <Windows.h>

::gpk::apod<char_t>				gpk::getWindowsErrorAsString		(const int32_t lastError)					{	// Get the error message, if any.
	if(0 == lastError)
		return {};
	char_t										* messageBuffer						= nullptr;
	const uint32_t								size								= FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	retval_gerror_if({}, nullptr == messageBuffer, "FormatMessage() failed.")
	else {
		const ::gpk::apod<char_t>				message								= ::gpk::view<const char_t>{messageBuffer, size >= 2 ? size - 2 : size};
		LocalFree(messageBuffer);
		return message;
	}
}

::gpk::error_t							gpk::wcstombs						(::gpk::apod<char_t> & output, const ::gpk::view<wchar_t> input)	{
	if(0 == input.size())
		return 0;

	int											sizeNeededForMultiByte				= WideCharToMultiByte(CP_UTF8, 0, input.begin(), int(input.size()), nullptr, 0, nullptr, nullptr);
	if(0 == sizeNeededForMultiByte)
		return 0;

	::gpk::apod<char_t>					converted;
	converted.resize(sizeNeededForMultiByte);
	WideCharToMultiByte(CP_UTF8, 0, input.begin(), input.size(), &converted[0], int(converted.size()), nullptr, nullptr);
	output.append(converted);
	return 0;
}

::gpk::error_t							gpk::mbstowcs						(::gpk::apod<wchar_t> & output, const ::gpk::vcc input)	{
	if(0 == input.size())
		return 0;

	::gpk::apod<wchar_t>					converted;
	converted.resize(input.size());
	gpk_hrcall(MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, &input[0], input.size(), &converted[0], converted.size()));
	converted.resize((uint32_t)wcslen(converted.begin()));
	output.append(converted);
	return 0;
}

#endif