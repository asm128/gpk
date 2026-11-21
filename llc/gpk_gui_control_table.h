#include "gpk_gui_control.h"

#ifndef GPK_GUI_CONTROL_TABLE_H_23627
#define GPK_GUI_CONTROL_TABLE_H_23627

namespace gpk
{
	struct SControlTable {
		::gpk::apod<::gpk::SControlState		>	States			= {};
		::gpk::apod<::gpk::SControlPlacement	>	Placement		= {};
		::gpk::apod<::gpk::SControlArea			>	Area			= {};
		::gpk::apod<::gpk::SControlConstraints	>	Constraints		= {};
		::gpk::apod<::gpk::SControlDraw			>	Draw			= {};
		::gpk::aobj<::gpk::SControlText			>	Text			= {};
		::gpk::aobj<::gpk::SControlImage		>	Images			= {};
		::gpk::aobj<::gpk::g8bgra>	ImageViews = {};
		::gpk::aobj<::gpk::rtbgra8d32>	ImageTemps = {};
		::gpk::aobj<::gpk::acid					>	Children		= {};
		//
		::gpk::apobj<::gpk::SEventSystem		>	EventQueue		= {};

		inline	bool		SetHovered		(cid_t iControl, bool value)	noexcept	{ return States[iControl].SetHovered (iControl, EventQueue, value); }
		inline	bool		SetPressed		(cid_t iControl, bool value)	noexcept	{ return States[iControl].SetPressed (iControl, EventQueue, value); }
		inline	bool		SetHidden		(cid_t iControl, bool value)	noexcept	{ return States[iControl].SetHidden  (iControl, EventQueue, value); }
		inline	bool		SetFocused		(cid_t iControl, bool value)	noexcept	{ return States[iControl].SetFocused (iControl, EventQueue, value); }
		inline	bool		SetSelected		(cid_t iControl, bool value)	noexcept	{ return States[iControl].SetSelected(iControl, EventQueue, value); }
		inline	bool		SetDisabled		(cid_t iControl, bool value)	noexcept	{ return States[iControl].SetDisabled(iControl, EventQueue, value); }
		inline	bool		SetUpdated		(cid_t iControl, bool value)	noexcept	{ return States[iControl].SetUpdated (iControl, EventQueue, value); }
		inline	bool		SetAction		(cid_t iControl, bool value)	noexcept	{ return States[iControl].SetAction  (iControl, EventQueue, value); }
		inline	bool		SetUnused		(cid_t iControl, bool value)	noexcept	{ return States[iControl].SetUnused  (iControl, EventQueue, value); }

		inline	bool		IsHovered		(cid_t iControl)		const	noexcept	{ return States[iControl].IsHovered  (); }
		inline	bool		IsPressed		(cid_t iControl)		const	noexcept	{ return States[iControl].IsPressed  (); }
		inline	bool		IsHidden		(cid_t iControl)		const	noexcept	{ return States[iControl].IsHidden   (); }
		inline	bool		IsFocused		(cid_t iControl)		const	noexcept	{ return States[iControl].IsFocused  (); }
		inline	bool		IsSelected		(cid_t iControl)		const	noexcept	{ return States[iControl].IsSelected (); }
		inline	bool		IsDisabled		(cid_t iControl)		const	noexcept	{ return States[iControl].IsDisabled (); }
		inline	bool		IsUpdated		(cid_t iControl)		const	noexcept	{ return States[iControl].IsUpdated  (); }
		inline	bool		IsAction		(cid_t iControl)		const	noexcept	{ return States[iControl].IsAction   (); }
		inline	bool		IsUnused		(cid_t iControl)		const	noexcept	{ return States[iControl].IsUnused   (); }
	};

	::gpk::cid_t		controlCreate					(::gpk::SControlTable & gui, ::gpk::vs1_t defaultColors);
} // namespace

#endif // GPK_GUI_CONTROL_TABLE_H_23627
