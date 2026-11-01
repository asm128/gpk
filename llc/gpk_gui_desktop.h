#include "gpk_gui_control_list.h"
#include "gpk_gui_viewport.h"

#ifndef GPK_GUI_DESKTOP_H_23627
#define GPK_GUI_DESKTOP_H_23627

namespace gpk
{

	tplt <tpnm _tElement>
	struct SRecyclableElementContainer {
		::gpk::view_bit<u3_t>	Unused;
		::gpk::aobj<_tElement>		Elements;
		::gpk::au3_t				UnusedData;

		inxp	const _tElement&	operator[]					(u2_t index)			const				{ return Elements[index]; }
		inxp	_tElement&			operator[]					(u2_t index)								{ return Elements[index]; }

		inxp	u2_t			size						()							const	noexcept	{ return Elements.size(); }
		::gpk::err_t				push_back					(const _tElement & toPush)						{
			const ::gpk::cid_t				elemIndex					= (::gpk::cid_t)Elements.push_back(toPush);
			if_true_vef(-1, failed(elemIndex), "%s", "Failed to add element to container! Out of memory?");
			stxp	u2_t				boolContainerElementSize	= sizeof(u3_t) * ::gpk::BYTE_SIZE;
			if_fail_fe(UnusedData.resize((elemIndex + 1) / boolContainerElementSize + 1));
			Unused						= {UnusedData.begin(), (u2_t)(elemIndex + 1)};
			Unused[elemIndex]			= false;
			return elemIndex;
		}
	};

#define GPK_DESKTOP_RECYCLABLE(elementType_token)	::gpk::SRecyclableElementContainer<::gpk::S##elementType_token>		elementType_token##s

	struct SDesktopItems {
		GPK_DESKTOP_RECYCLABLE(Viewport);
		GPK_DESKTOP_RECYCLABLE(PaletteGrid);
		GPK_DESKTOP_RECYCLABLE(ControlList);
	};

	struct SDesktop {
		::gpk::cid_t				IdControl					= -1;
		::gpk::SDesktopItems		Items						= {};
		::gpk::aobj<::gpk::acid>	Children					= {}; // Keep track of control list hierarchy.
	};

	::gpk::err_t				desktopCreatePaletteGrid	(::gpk::SGUI & gui, ::gpk::SDesktop & desktop);
	::gpk::err_t				desktopCreateControlList	(::gpk::SGUI & gui, ::gpk::SDesktop & desktop);
	::gpk::err_t				desktopCreateViewport		(::gpk::SGUI & gui, ::gpk::SDesktop & desktop);
	::gpk::err_t				desktopDeletePaletteGrid	(::gpk::SGUI & gui, ::gpk::SDesktop & desktop, ::gpk::cid_t iElement);
	::gpk::err_t				desktopDeleteControlList	(::gpk::SGUI & gui, ::gpk::SDesktop & desktop, ::gpk::cid_t iElement);
	::gpk::err_t				desktopDeleteViewport		(::gpk::SGUI & gui, ::gpk::SDesktop & desktop, ::gpk::cid_t iElement);
																							    
	::gpk::err_t				desktopControlListSetParent	(::gpk::SGUI & gui, ::gpk::SDesktop & desktop, ::gpk::cid_t iControlList, ::gpk::cid_t iParentControlList, ::gpk::cid_t iParentListItem);
	s3_t						desktopUpdate				(::gpk::SGUI & gui, ::gpk::SDesktop & desktop, ::gpk::SInput & input);

} // namespace

#endif // GPK_GUI_DESKTOP_H_23627
