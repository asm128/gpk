#include "gpk_gui_inputbox.h"
#include "gpk_gui_control.h"
#include "gpk_label.h"

::gpk::error_t			gpk::virtualKeyboardSetup437(::gpk::SGUI & gui, ::gpk::SVirtualKeyboard & vk)	{
	::gpk::au16					keys;
	for(uint16_t i = 1; i < 255; ++i) {
		keys.push_back(i);
	}
	keys.push_back('!');
	keys.push_back('?');
	gpk_necs(::gpk::virtualKeyboardSetup(gui, vk, 32, ::gpk::view_const_uint16{keys}));
	gui.Controls.States[vk.IdRoot].Hidden	= true;
	return vk.IdRoot;
}

::gpk::error_t			gpk::virtualKeyboardSetup	(::gpk::SGUI & gui, ::gpk::SVirtualKeyboard & vk, uint8_t rowWidth, const ::gpk::vcu16 & keys) {
	vk.Keys					= keys;

	vk.IdRoot				= ::gpk::controlCreate(gui);
	const ::gpk::n2u8			charSize					= gui.Fonts[gui.SelectedFont]->CharSize;
	const ::gpk::n2i16			sizeKey						= (charSize + ::gpk::n2u8{6, 6}).i16();
	stacxpr	uint16_t			SIZE_BUTTON					= 88;
	const ::gpk::n2i16			sizeKeypad					= {int16_t(sizeKey.x * rowWidth + 4 + SIZE_BUTTON), int16_t(sizeKey.y * (keys.size() / rowWidth) + 4)};

	{
		::gpk::SControl				& controlRoot				= gui.Controls.Controls[vk.IdRoot];
		controlRoot.Area.Size	= sizeKeypad;
		controlRoot.Align		= ::gpk::ALIGN_CENTER_BOTTOM;
	}
	for(uint32_t iKey = 0; iKey < vk.Keys.size(); ++iKey) {
		char						text[8]		= {};
		sprintf_s(text, "%c", vk.Keys[iKey]);

		int32_t						idKey			= ::gpk::controlCreate(gui);
		::gpk::SControl				& controlKey	= gui.Controls.Controls[idKey];
		controlKey.Area.Size	= sizeKey;
		controlKey.Area.Offset.x= int16_t((iKey % rowWidth) * sizeKey.x);
		controlKey.Area.Offset.y= int16_t((iKey / rowWidth) * sizeKey.y);
		controlKey.Border		= {1, 1, 1, 1};
		//controlKey.ColorTheme			= ::gpk::ASCII_COLOR_DARKGREY * 16 + 8;
		gui.Controls.Text[idKey].Align	= ::gpk::ALIGN_CENTER;

		gpk_necs(::gpk::controlSetParent(gui, idKey, vk.IdRoot));
		gpk_necs(::gpk::controlTextSet(gui, idKey, ::gpk::label(text)));
	}

	gpk_necs(::gpk::guiSetupControlList<::gpk::VK_SCANCODE>(gui, vk.IdRoot, SIZE_BUTTON, 0, ::gpk::ALIGN_TOP_RIGHT, ::gpk::ALIGN_CENTER, vk.IdKeys));
	return 0;
}

::gpk::error_t			gpk::inputBoxCreate		(::gpk::SUIInputBox & inputBox, ::gpk::SGUI & gui, int32_t iParent)	{
	gpk_necs(inputBox.IdRoot = ::gpk::controlCreate(gui));
	gui.Controls.Controls	[inputBox.IdRoot].Border			= {};
	gui.Controls.Controls	[inputBox.IdRoot].Margin			= {};
	gui.Controls.Modes		[inputBox.IdRoot].Design			= true;
	gui.Controls.Modes		[inputBox.IdRoot].NoBackgroundRect	= true;

	gpk_necs(inputBox.IdText = ::gpk::controlCreate(gui));
	gui.Controls.Controls	[inputBox.IdText].Border	= {2, 2, 2, 2};
	gui.Controls.Controls	[inputBox.IdText].Margin	= {1, 1, 1, 1};
	gui.Controls.Controls	[inputBox.IdText].Area.Size	= {0, 22};
	gui.Controls.Text		[inputBox.IdText].Align		= ::gpk::ALIGN_CENTER;

	gpk_necs(::gpk::virtualKeyboardSetup437(gui, inputBox.VirtualKeyboard));
	gui.Controls.States		[inputBox.VirtualKeyboard.IdRoot].Hidden		= false;
	gui.Controls.Controls	[inputBox.VirtualKeyboard.IdRoot].Align			= gui.Controls.Controls[inputBox.IdText].Align;
	gui.Controls.Controls	[inputBox.VirtualKeyboard.IdRoot].Area.Offset.y
		= gui.Controls.Controls	[inputBox.IdText].Area.Offset.y
		+ gui.Controls.Controls	[inputBox.IdText].Area.Size.y
		;
	gui.Controls.Controls	[inputBox.IdText].Area.Size.x	= gui.Controls.Controls	[inputBox.VirtualKeyboard.IdRoot].Area.Size.x;

	gui.Controls.Controls[inputBox.IdRoot].Area.Size.x	= ::gpk::max(gui.Controls.Controls[inputBox.IdText].Area.Size.x, gui.Controls.Controls[inputBox.VirtualKeyboard.IdRoot].Area.Size.x);
	gui.Controls.Controls[inputBox.IdRoot].Area.Size.y	+= gui.Controls.Controls[inputBox.VirtualKeyboard.IdRoot].Area.Size.y;

	gpk_necs(::gpk::controlSetParent(gui, inputBox.IdText					, inputBox.IdRoot));
	gpk_necs(::gpk::controlSetParent(gui, inputBox.VirtualKeyboard.IdRoot	, inputBox.IdRoot));
	if(iParent >= 0)
		gpk_necall(::gpk::controlSetParent(gui, inputBox.IdRoot, iParent), "iParent: %i", iParent);

	return inputBox.IdRoot;
}
