#include "gpk_windows.h"
#include "gpk_string_compose.h"

#if defined(GPK_WINDOWS)
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	include <Windows.h>

gpk::string	gpk::getWindowsErrorAsString(s2_t lastError) {	// Get the error message, if any.
	if(0 == lastError)
		return {};
	char			* messageBuffer				= {};
	u2_c			size						= FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
	string			message;
	if_null_ve(message, messageBuffer);
	append_strings(message, vcst_t{messageBuffer, size >= 2 ? size - 2 : size});
	LocalFree(messageBuffer);
	return message;
}

gpk::err_t	gpk::wcstombs				(string & output, view<const wchar_t> input)	{
	if(0 == input.size())
		return 0;

	int				sizeNeededForMultiByte		= WideCharToMultiByte(CP_UTF8, 0, input.begin(), int(input.size()), nullptr, 0, nullptr, nullptr);
	if(0 == sizeNeededForMultiByte)
		return 0;

	gpk::string			converted;
	if_fail_fe(converted.resize(sizeNeededForMultiByte));
	WideCharToMultiByte(CP_UTF8, 0, input.begin(), input.size(), &converted[0], int(converted.size()), nullptr, nullptr);
	if_fail_fe(output.append(converted));
	return sizeNeededForMultiByte;
}

gpk::err_t	gpk::mbstowcs				(gpk::apod<wchar_t> & output, vcst_t input)	{
	if(0 == input.size())
		return 0;

	apod<wchar_t>		converted;
	converted.resize(input.size());
	gpk_hrcall(MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, &input[0], input.size(), &converted[0], converted.size()));
	converted.resize((u2_t)wcslen(converted.begin()));
	output.append(converted);
	return converted.size();
}

#endif