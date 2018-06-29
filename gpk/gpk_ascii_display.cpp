#include "gpk_ascii_display.h"

#include <cstdio>
#include <Windows.h>

#include <vector>

					::gpk::error_t						gpk::asciiDisplayTitleSet						(const view_const_string& title)															{ return (0 == SetConsoleTitle(title.begin() ? title.begin() : "")) ? -1 : 0; }

struct SWindowsConsoleInfo {
						::CONSOLE_FONT_INFOEX				InfoFontOriginal								= {sizeof(::CONSOLE_FONT_INFOEX)};
						::CONSOLE_FONT_INFOEX				InfoFontCurrent									= {sizeof(::CONSOLE_FONT_INFOEX)};
						::CONSOLE_SCREEN_BUFFER_INFOEX		InfoScreenBufferOriginal						= {sizeof(::CONSOLE_SCREEN_BUFFER_INFOEX)};
						::CONSOLE_SCREEN_BUFFER_INFOEX		InfoScreenBufferCurrent							= {sizeof(::CONSOLE_SCREEN_BUFFER_INFOEX)};
						bool								Created											= false;
						bool								SystemOwned										= false;
};

static				::std::vector<uint8_t >				g_bufferClearCharacter							= {};
static				::std::vector<uint16_t>				g_bufferClearColors								= {};
static				::SWindowsConsoleInfo				g_ConsoleInfo									= {};
static constexpr	const ::gpk::SColorRGBA				g_DefaultPalette	[]							= 
	{ ::gpk::ASCII_COLOR_INDEX_0	
	, ::gpk::ASCII_COLOR_INDEX_1	
	, ::gpk::ASCII_COLOR_INDEX_2	
	, ::gpk::ASCII_COLOR_INDEX_3	
	, ::gpk::ASCII_COLOR_INDEX_4	
	, ::gpk::ASCII_COLOR_INDEX_5	
	, ::gpk::ASCII_COLOR_INDEX_6	
	, ::gpk::ASCII_COLOR_INDEX_7	
	, ::gpk::ASCII_COLOR_INDEX_8	
	, ::gpk::ASCII_COLOR_INDEX_9	
	, ::gpk::ASCII_COLOR_INDEX_10	
	, ::gpk::ASCII_COLOR_INDEX_11	
	, ::gpk::ASCII_COLOR_INDEX_12	
	, ::gpk::ASCII_COLOR_INDEX_13	
	, ::gpk::ASCII_COLOR_INDEX_14	
	, ::gpk::ASCII_COLOR_INDEX_15	
	};

//					::gpk::error_t						gpk::asciiDisplayInfo							(::gpk::SASCIIDisplayInfo& displayInfo)																	{ 
//	::CONSOLE_SCREEN_BUFFER_INFOEX								& csbiInfo										= g_ConsoleInfo.InfoScreenBufferCurrent;
//	displayInfo.DisplaySizeMax								= {0x7FFF, 0x7FFF};
//	displayInfo.DisplaySizeCurrent							= {(uint32_t)csbiInfo.dwSize.X, (uint32_t)csbiInfo.dwSize.Y};
//	displayInfo.ColorWidth									= 24;
//	displayInfo.PaletteSize									= 16;
//	return 0; 
//}

					::gpk::error_t						initWindowsConsoleProperties					(int width, int height, const ::gpk::SColorRGBA* palette)												{
	reterr_error_if(0 == palette, "Invalid parameter.");
	reterr_error_if(0 == width * height, "Invalid console size.");
	const ::HANDLE												handleConsoleOut								= ::GetStdHandle(STD_OUTPUT_HANDLE);	// Get console output handle

	::CONSOLE_SCREEN_BUFFER_INFOEX								& csbiInfo										= g_ConsoleInfo.InfoScreenBufferCurrent;
	csbiInfo.dwSize.X										= (::SHORT)width;
	csbiInfo.dwSize.Y										= (::SHORT)height;
	csbiInfo.dwMaximumWindowSize.X							= (::SHORT)width;
	csbiInfo.dwMaximumWindowSize.Y							= (::SHORT)height;

	// We're forced to set up the colors at this point because for some reason the GetConsoleScreenBufferInfoEx() function doesn't return the color table properly,
	// thus they're all 0 when we call SetConsoleScreenBufferInfoEx() causing all the colors to be reset to COLOR_BLACK.
	// Ideally we would have a setConsoleColors() function to do this separately.
	for(uint32_t iColor = 0; iColor < 16; ++iColor)
		csbiInfo.ColorTable[iColor]								= palette[iColor];

	csbiInfo.wAttributes									= ::gpk::ASCII_COLOR_WHITE;
	
	uint32_t													newClearSize									= width * height;
	if(newClearSize > g_bufferClearCharacter.size()) {
		g_bufferClearCharacter	.resize(newClearSize);
		g_bufferClearColors		.resize(newClearSize);
	}
	reterr_error_if(FALSE == ::SetConsoleScreenBufferInfoEx(handleConsoleOut, &csbiInfo), "Failed to set console properties.");
	return 0;
}

					::gpk::error_t						gpk::asciiDisplayResize							(uint32_t width, uint32_t height)																		{ return ::initWindowsConsoleProperties(width, height, (const ::gpk::SColorRGBA*)g_ConsoleInfo.InfoScreenBufferCurrent.ColorTable); }
					::gpk::error_t						gpk::asciiDisplayPaletteReset					()																										{ return ::initWindowsConsoleProperties(g_ConsoleInfo.InfoScreenBufferCurrent.dwSize.X, g_ConsoleInfo.InfoScreenBufferCurrent.dwSize.Y, g_DefaultPalette); }
					::gpk::error_t						gpk::asciiDisplayPaletteSet						(const ::gpk::view_array<::gpk::SColorRGBA>& palette)													{
	reterr_error_if(palette.size() < 16, "Palette too small. 16 colors are required for the console palette to be valid.");
	return ::initWindowsConsoleProperties(g_ConsoleInfo.InfoScreenBufferCurrent.dwSize.X, g_ConsoleInfo.InfoScreenBufferCurrent.dwSize.Y, palette.begin());
}

					::gpk::error_t						gpk::asciiDisplaySize							(::gpk::SCoord2<uint32_t>& size)																		{
	const ::CONSOLE_SCREEN_BUFFER_INFOEX						& csbiInfo										= g_ConsoleInfo.InfoScreenBufferCurrent;
	size													= {(uint32_t)csbiInfo.dwSize.X, (uint32_t)csbiInfo.dwSize.Y};
	return 0;
}

					::gpk::error_t						gpk::asciiDisplayClear							(uint8_t character, uint16_t colorRef)																	{
	reterr_error_if(false == ::g_ConsoleInfo.Created, "Cannot clear console if the console wasn't created!");
	reterr_error_if(0 == ::g_bufferClearCharacter.size(), "Cannot clear zero-sized console!");
	::memset(&g_bufferClearCharacter[0], character, g_bufferClearCharacter.size());
	const uint16_t												colors[4]										= {colorRef, colorRef, colorRef, colorRef};
	if(g_bufferClearColors[0] != colorRef) {
		 if(0 == (g_bufferClearColors.size() % 4)) {
			for(uint32_t iCell = 0, cellCount = (uint32_t)g_bufferClearColors.size() / 4; iCell < cellCount; ++iCell)
				*(uint64_t*)g_bufferClearColors[iCell]					= *(uint64_t*)colors;
		}
		else if(0 == (g_bufferClearColors.size() % 2)) {
			for(uint32_t iCell = 0, cellCount = (uint32_t)g_bufferClearColors.size() / 2; iCell < cellCount; ++iCell)
				*(uint32_t*)g_bufferClearColors[iCell]					= *(uint32_t*)colors;
		}
		else {
			for(uint32_t iCell = 0, cellCount = (uint32_t)g_bufferClearColors.size(); iCell < cellCount; ++iCell)
				g_bufferClearColors[iCell]								= colorRef;
		}
	}

	const ::CONSOLE_SCREEN_BUFFER_INFOEX						& csbiInfo										= g_ConsoleInfo.InfoScreenBufferCurrent;
	uint32_t													sizeFrontBuffer									= csbiInfo.dwSize.X * csbiInfo.dwSize.Y;
	::COORD														offset											= {0, 0};
	::DWORD														dummy											= 0;
	const ::HANDLE												handleConsoleOut								= ::GetStdHandle(STD_OUTPUT_HANDLE);	// Get console output handle
	reterr_error_if(0 == ::WriteConsoleOutputCharacterA	(handleConsoleOut, (const char*)	g_bufferClearCharacter	.data(), ::gpk::max(0U, ::gpk::min((uint32_t)g_bufferClearCharacter	.size(), (uint32_t)sizeFrontBuffer)), offset, &dummy ), "How did this happen?");
	reterr_error_if(0 == ::WriteConsoleOutputAttribute	(handleConsoleOut,					g_bufferClearColors		.data(), ::gpk::max(0U, ::gpk::min((uint32_t)g_bufferClearColors	.size(), (uint32_t)sizeFrontBuffer)), offset, &dummy ), "How did this happen?");
	return 0;
}

					::gpk::error_t						gpk::asciiDisplayPresent						(const ::gpk::view_array<const uint8_t>& characters, const ::gpk::view_array<const uint16_t>& colors)	{ 
	ree_if(false == ::g_ConsoleInfo.Created, "Cannot present console if the console wasn't created!");
	const ::HANDLE												handleConsoleOut								= ::GetStdHandle(STD_OUTPUT_HANDLE);	// Get console output handle	
	const ::CONSOLE_SCREEN_BUFFER_INFOEX						& csbiInfo										= g_ConsoleInfo.InfoScreenBufferCurrent;
	uint32_t													sizeFrontBuffer									= csbiInfo.dwSize.X * csbiInfo.dwSize.Y;
	::COORD														offset											= {0, 0};
	::DWORD														dummy											= 0;
	uint32_t													lengthWriteCharacters							= ::gpk::max(0U, ::gpk::min(characters	.size(), (uint32_t)sizeFrontBuffer));
	uint32_t													lengthWriteColors								= ::gpk::max(0U, ::gpk::min(colors		.size(), (uint32_t)sizeFrontBuffer));
	::WriteConsoleOutputCharacterA	(handleConsoleOut, (const char*)	characters	.begin(), lengthWriteCharacters	, offset, &dummy );
	::WriteConsoleOutputAttribute	(handleConsoleOut,					colors		.begin(), lengthWriteColors		, offset, &dummy );
	return 0; 
}

					BOOL WINAPI							handlerConsoleRoutine							(_In_ DWORD dwCtrlType)																					{
	switch(dwCtrlType) {
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	default:
		return TRUE;
	}
}

					::gpk::error_t						gpk::asciiDisplayDestroy						()																										{ 
	retwarn_warn_if(false == g_ConsoleInfo.Created, "Redundant destruction of system console.");
	::SetConsoleCtrlHandler(::handlerConsoleRoutine, FALSE);
	const ::HANDLE												hConsoleOut										= ::GetStdHandle( STD_OUTPUT_HANDLE );
	::SetCurrentConsoleFontEx		(hConsoleOut, FALSE, &g_ConsoleInfo.InfoFontOriginal	);
	::SetConsoleScreenBufferInfoEx	(hConsoleOut, &g_ConsoleInfo.InfoScreenBufferOriginal	);
	if(false == g_ConsoleInfo.SystemOwned) {
		::FreeConsole();
		::fclose(stdout);
		::FILE *										
		stream													= 0; ::freopen_s	(&stream, "CONIN$", "r+", stdin);
		stream													= 0; ::fopen_s		(&stream, "CONOUT$", "w+");
	}
	g_ConsoleInfo.Created									= false;
	return 0; 
}

					::gpk::error_t						gpk::asciiDisplayCreate							(uint32_t frontBufferWidth, uint32_t frontBufferHeight)													{
	bool														createdConsole										= (FALSE == ::AllocConsole()) ? false : true;
	if(createdConsole)
		::AttachConsole(::GetCurrentProcessId());

	::EnableMenuItem
		( ::GetSystemMenu(::GetConsoleWindow(), FALSE)
		, SC_CLOSE
		, MF_BYCOMMAND | MF_GRAYED 
		);

	if(createdConsole) {
		const HANDLE												hConsoleIn										= ::GetStdHandle(STD_INPUT_HANDLE);
		DWORD														mode											= 0;
		::GetConsoleMode(hConsoleIn, &mode);
		mode													&= ~(DWORD)ENABLE_QUICK_EDIT_MODE;
		::SetConsoleMode(hConsoleIn, mode);

		FILE*												
		stream													= 0; ::freopen_s(&stream, "CONOUT$", "w+", stdout);
		stream													= 0; ::freopen_s(&stream, "CONIN$", "r+", stdin);
	}

	SetConsoleTitle(TEXT("ASCII Console Sugar"));
	::SetConsoleCtrlHandler(::handlerConsoleRoutine, TRUE);
	const HANDLE												hConsoleOut										= ::GetStdHandle(STD_OUTPUT_HANDLE);
	::GetCurrentConsoleFontEx		( hConsoleOut, TRUE	, &g_ConsoleInfo.InfoFontOriginal			);
	::GetConsoleScreenBufferInfoEx	( hConsoleOut		, &g_ConsoleInfo.InfoScreenBufferOriginal	);
	g_ConsoleInfo.InfoFontCurrent							= g_ConsoleInfo.InfoFontOriginal			;
	g_ConsoleInfo.InfoScreenBufferCurrent					= g_ConsoleInfo.InfoScreenBufferOriginal	;

	::CONSOLE_FONT_INFOEX										infoFont										= {sizeof(::CONSOLE_FONT_INFOEX)};
	infoFont.dwFontSize.X									= 8;
	infoFont.dwFontSize.Y									= 12;
	::wcscpy_s(infoFont.FaceName, L"Terminal");
	::SetCurrentConsoleFontEx(hConsoleOut, FALSE, &infoFont);

	::CONSOLE_SCREEN_BUFFER_INFOEX								& csbiInfo										= g_ConsoleInfo.InfoScreenBufferCurrent;
	csbiInfo.srWindow.Top									= 10;
	csbiInfo.srWindow.Left									= 10;
	csbiInfo.srWindow.Right									= 800;
	csbiInfo.srWindow.Bottom								= 600;
	::initWindowsConsoleProperties(frontBufferWidth, frontBufferHeight, g_DefaultPalette);
	g_ConsoleInfo.Created									= true;
	g_ConsoleInfo.SystemOwned								= createdConsole ? false : true;
	return 0; 
}
