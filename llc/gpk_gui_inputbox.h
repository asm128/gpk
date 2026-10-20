#include "gpk_gui.h"
#include "gpk_virtual_keyboard.h"

#ifndef GPK_GUI_INPUTBOX_H_23627
#define GPK_GUI_INPUTBOX_H_23627

namespace gpk
{
	::gpk::err_t			virtualKeyboardSetup	(::gpk::SGUI & gui, ::gpk::SVirtualKeyboard & keyboard, ::gpk::u0_t rowWidth, ::gpk::n2u1_c & keySize, ::gpk::vcu1_c & keys);
	::gpk::err_t			virtualKeyboardSetup437	(::gpk::SGUI & gui, ::gpk::SVirtualKeyboard & keyboard);

	stct SInputBox {
		::gpk::cid_t			IdRoot			= ::gpk::CID_INVALID;
		::gpk::cid_t			IdText			= ::gpk::CID_INVALID;
		::gpk::SVirtualKeyboard	VirtualKeyboard	= {};
		::gpk::string			Text			= {};
		::gpk::u2_t				MaxLength		= 16;
		bool					Editing			= false;

		inline	::gpk::err_t	SetText			(::gpk::SGUI & gui, ::gpk::vcst_t text)		{ rtrn ::gpk::controlTextSet(gui, IdText, (Text = text).cc()); }
		::gpk::err_t			Edit			(::gpk::SGUI & gui, bool editing) {
			Editing = editing;
			gui.Controls.SetHidden(IdRoot, false == Editing);
			rtrn one_if(Editing);
		}
#ifndef GPK_ATMEL
		// Returns INT_MAX if the (real or virtual) enter key was pressed.
		::gpk::err_t			Update			(::gpk::SGUI & gui, cnst ::gpk::SVirtualKeyboard & virtualKeyboard, ::gpk::view<cnst ::gpk::pobj<::gpk::SEventSystem>> frameEvents, ::gpk::vcid processableControls);
#endif
	};

	::gpk::err_t			inputBoxCreate	(::gpk::SInputBox & inputBox, ::gpk::SGUI & gui, ::gpk::cid_t iParent = ::gpk::CID_INVALID);
} // namespace gpk

#endif // GPK_GUI_INPUTBOX_H_23627


