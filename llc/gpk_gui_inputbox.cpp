#include "gpk_gui_inputbox.h"
#include "gpk_gui_control_list.h"
#include "gpk_label.h"

#ifndef GPK_WINDOWS
#	include "gpk_string.h"
#endif

::gpk::error_t			gpk::virtualKeyboardSetup437(::gpk::SGUI & gui, ::gpk::SVirtualKeyboard & vk)	{
	::gpk::au1_t					keys;
	for(uint16_t i = 1; i < 255; ++i) {
		keys.push_back(i);
	}
	keys.push_back('!');
	keys.push_back('?');
	gpk_necs(gpk::virtualKeyboardSetup(gui, vk, 32, ::gpk::vcu1_t{keys}));
	gui.Controls.SetHidden(vk.IdRoot, true);
	return vk.IdRoot;
}

::gpk::error_t			gpk::virtualKeyboardSetup	(::gpk::SGUI & gui, ::gpk::SVirtualKeyboard & vk, uint8_t rowWidth, ::gpk::vcu1_c & keys) {
	vk.Keys					= keys;

	vk.IdRoot				= ::gpk::controlCreate(gui);
	const ::gpk::n2u0_t			charSize					= gui.Fonts[gui.SelectedFont]->CharSize;
	const ::gpk::n2s1_t			sizeKey						= (charSize + ::gpk::n2u0_t{6, 6}).s1_t();
	stacxpr	uint16_t			SIZE_BUTTON					= 88;
	const ::gpk::n2s1_t			sizeKeypad					= {int16_t(sizeKey.x * rowWidth + 4 + SIZE_BUTTON), int16_t(sizeKey.y * (keys.size() / rowWidth) + 4)};

	{
		::gpk::SControlPlacement	& controlRoot				= gui.Controls.Placement[vk.IdRoot];
		controlRoot.Area.Size	= sizeKeypad;
		controlRoot.Align		= ::gpk::ALIGN_CENTER_BOTTOM;
	}
	for(uint32_t iKey = 0; iKey < vk.Keys.size(); ++iKey) {
		sc_t						text[8]		= {};
		sprintf_s(text, "%c", vk.Keys[iKey]);

		::gpk::cid_t				idKey			= ::gpk::controlCreate(gui);
		::gpk::SControlPlacement	& controlKey	= gui.Controls.Placement[idKey];
		controlKey.Area.Size	= sizeKey;
		controlKey.Area.Offset.x= int16_t((iKey % rowWidth) * sizeKey.x);
		controlKey.Area.Offset.y= int16_t((iKey / rowWidth) * sizeKey.y);
		controlKey.Border		= {1, 1, 1, 1};
		//controlKey.ColorTheme			= ::gpk::ASCII_COLOR_DARKGREY * 16 + 8;
		gui.Controls.Text[idKey].Align	= ::gpk::ALIGN_CENTER;

		gpk_necs(gpk::controlSetParent(gui, idKey, vk.IdRoot));
		gpk_necs(gpk::controlTextSet(gui, idKey, ::gpk::label(text)));
	}

	gpk_necs(gpk::guiCreateControlList<::gpk::VK_SCANCODE>(gui, vk.IdRoot, SIZE_BUTTON, 0, ::gpk::ALIGN_TOP_RIGHT, ::gpk::ALIGN_CENTER, vk.IdKeys));
	return 0;
}

::gpk::error_t			gpk::inputBoxCreate		(::gpk::SInputBox & inputBox, ::gpk::SGUI & gui, ::gpk::cid_t iParent)	{
	gpk_necs(inputBox.IdRoot = ::gpk::controlCreate(gui));
	gui.Controls.States		[inputBox.IdRoot].Mask		&= ~::gpk::GUI_CONTROL_FLAG_Hovered;
	gui.Controls.Placement	[inputBox.IdRoot].Border	= {};
	gui.Controls.Placement	[inputBox.IdRoot].Margin	= {};
	//gui.Controls.Draw		[inputBox.IdRoot].NoBorder	= true;
	gui.Controls.Draw		[inputBox.IdRoot].NoClient	= true;

	gpk_necs(inputBox.IdText = ::gpk::controlCreate(gui));
	gui.Controls.Placement	[inputBox.IdText].Border	= {2, 2, 2, 2};
	gui.Controls.Placement	[inputBox.IdText].Margin	= {1, 1, 1, 1};
	gui.Controls.Placement	[inputBox.IdText].Area.Size	= {0, 22};
	gui.Controls.Text		[inputBox.IdText].Align		= ::gpk::ALIGN_CENTER;

	gpk_necs(gpk::virtualKeyboardSetup437(gui, inputBox.VirtualKeyboard));
	gui.Controls.SetHidden(inputBox.VirtualKeyboard.IdRoot, false);
	gui.Controls.Placement	[inputBox.VirtualKeyboard.IdRoot].Align	= gui.Controls.Placement[inputBox.IdText].Align;
	gui.Controls.Placement	[inputBox.VirtualKeyboard.IdRoot].Area.Offset.y
		= gui.Controls.Placement	[inputBox.IdText].Area.Offset.y
		+ gui.Controls.Placement	[inputBox.IdText].Area.Size.y
		;
	gui.Controls.Placement[inputBox.IdText].Area.Size.x	= gui.Controls.Placement[inputBox.VirtualKeyboard.IdRoot].Area.Size.x;

	gui.Controls.Placement[inputBox.IdRoot].Area.Size.x	= ::gpk::max(gui.Controls.Placement[inputBox.IdText].Area.Size.x, gui.Controls.Placement[inputBox.VirtualKeyboard.IdRoot].Area.Size.x);
	gui.Controls.Placement[inputBox.IdRoot].Area.Size.y	+= gui.Controls.Placement[inputBox.VirtualKeyboard.IdRoot].Area.Size.y;

	gpk_necs(gpk::controlSetParent(gui, inputBox.IdText					, inputBox.IdRoot));
	gpk_necs(gpk::controlSetParent(gui, inputBox.VirtualKeyboard.IdRoot	, inputBox.IdRoot));
	if(iParent >= 0)
		gpk_necall(gpk::controlSetParent(gui, inputBox.IdRoot, iParent), "iParent: %i", iParent);

	return inputBox.IdRoot;
}
