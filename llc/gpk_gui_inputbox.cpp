#include "gpk_gui_inputbox.h"
#include "gpk_gui_control_list.h"
#include "gpk_label.h"
#include "gpk_string.h"

GPK_USING_TYPEINT();

::gpk::err_t			gpk::virtualKeyboardSetup437(::gpk::SGUI & gui, ::gpk::SVirtualKeyboard & vk)	{
	::gpk::au1_t					keys;
	for(u1_t i = 1; i < 255; ++i) {
		keys.push_back(i);
	}
	keys.push_back('!');
	keys.push_back('?');
	if_fail_fe(gpk::virtualKeyboardSetup(gui, vk, 32, ::gpk::vcu1_t{keys}));
	gui.Controls.SetHidden(vk.IdRoot, true);
	return vk.IdRoot;
}

::gpk::err_t			gpk::virtualKeyboardSetup	(::gpk::SGUI & gui, ::gpk::SVirtualKeyboard & vk, u0_t rowWidth, ::gpk::vcu1_c & keys) {
	vk.Keys					= keys;

	vk.IdRoot				= ::gpk::controlCreate(gui);
	const ::gpk::n2u0_t			charSize					= gui.Fonts[gui.SelectedFont]->CharSize;
	const ::gpk::n2s1_t			sizeKey						= (charSize + ::gpk::n2u0_t{6, 6}).s1();
	stxp	u1_t			SIZE_BUTTON					= 88;
	const ::gpk::n2s1_t			sizeKeypad					= {s1_t(sizeKey.x * rowWidth + 4 + SIZE_BUTTON), s1_t(sizeKey.y * (keys.size() / rowWidth) + 4)};

	{
		::gpk::SControlPlacement	& controlRoot				= gui.Controls.Placement[vk.IdRoot];
		controlRoot.Area.Size	= sizeKeypad;
		controlRoot.Align		= ::gpk::ALIGN_CENTER_BOTTOM;
	}
	for(u2_t iKey = 0; iKey < vk.Keys.size(); ++iKey) {
		sc_t						text[8]		= {};
		sprintf_s(text, "%c", vk.Keys[iKey]);

		::gpk::cid_t				idKey			= ::gpk::controlCreate(gui);
		::gpk::SControlPlacement	& controlKey	= gui.Controls.Placement[idKey];
		controlKey.Area.Size	= sizeKey;
		controlKey.Area.Offset.x= s1_t((iKey % rowWidth) * sizeKey.x);
		controlKey.Area.Offset.y= s1_t((iKey / rowWidth) * sizeKey.y);
		controlKey.Border		= {1, 1, 1, 1};
		//controlKey.ColorTheme			= ::gpk::ASCII_COLOR_DARKGREY * 16 + 8;
		gui.Controls.Text[idKey].Align	= ::gpk::ALIGN_CENTER;

		if_fail_fe(gpk::controlSetParent(gui, idKey, vk.IdRoot));
		if_fail_fe(gpk::controlTextSet(gui, idKey, label(text)));
	}

	if_fail_fe(gpk::guiCreateControlList<::gpk::VK_SCANCODE>(gui, vk.IdRoot, SIZE_BUTTON, 0, ::gpk::ALIGN_TOP_RIGHT, ::gpk::ALIGN_CENTER, vk.IdKeys));
	return 0;
}

::gpk::err_t			gpk::inputBoxCreate		(::gpk::SInputBox & inputBox, ::gpk::SGUI & gui, ::gpk::cid_t iParent)	{
	if_fail_fe(inputBox.IdRoot = ::gpk::controlCreate(gui));
	gui.Controls.States		[inputBox.IdRoot].Mask		&= ~::gpk::GUI_CONTROL_FLAG_Hovered;
	gui.Controls.Placement	[inputBox.IdRoot].Border	= {};
	gui.Controls.Placement	[inputBox.IdRoot].Margin	= {};
	//gui.Controls.Draw		[inputBox.IdRoot].NoBorder	= true;
	gui.Controls.Draw		[inputBox.IdRoot].NoClient	= true;

	if_fail_fe(inputBox.IdText = ::gpk::controlCreate(gui));
	gui.Controls.Placement	[inputBox.IdText].Border	= {2, 2, 2, 2};
	gui.Controls.Placement	[inputBox.IdText].Margin	= {1, 1, 1, 1};
	gui.Controls.Placement	[inputBox.IdText].Area.Size	= {0, 22};
	gui.Controls.Text		[inputBox.IdText].Align		= ::gpk::ALIGN_CENTER;

	if_fail_fe(gpk::virtualKeyboardSetup437(gui, inputBox.VirtualKeyboard));
	gui.Controls.SetHidden(inputBox.VirtualKeyboard.IdRoot, false);
	gui.Controls.Placement	[inputBox.VirtualKeyboard.IdRoot].Align	= gui.Controls.Placement[inputBox.IdText].Align;
	gui.Controls.Placement	[inputBox.VirtualKeyboard.IdRoot].Area.Offset.y
		= gui.Controls.Placement	[inputBox.IdText].Area.Offset.y
		+ gui.Controls.Placement	[inputBox.IdText].Area.Size.y
		;
	gui.Controls.Placement[inputBox.IdText].Area.Size.x	= gui.Controls.Placement[inputBox.VirtualKeyboard.IdRoot].Area.Size.x;

	gui.Controls.Placement[inputBox.IdRoot].Area.Size.x	= ::gpk::max(gui.Controls.Placement[inputBox.IdText].Area.Size.x, gui.Controls.Placement[inputBox.VirtualKeyboard.IdRoot].Area.Size.x);
	gui.Controls.Placement[inputBox.IdRoot].Area.Size.y	+= gui.Controls.Placement[inputBox.VirtualKeyboard.IdRoot].Area.Size.y;

	if_fail_fe(gpk::controlSetParent(gui, inputBox.IdText					, inputBox.IdRoot));
	if_fail_fe(gpk::controlSetParent(gui, inputBox.VirtualKeyboard.IdRoot	, inputBox.IdRoot));
	if(iParent >= 0)
		if_fail_fef(gpk::controlSetParent(gui, inputBox.IdRoot, iParent), "iParent: %i", iParent);

	return inputBox.IdRoot;
}
