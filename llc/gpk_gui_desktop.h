#include "gpk_gui_control.h"
#include "gpk_gui_viewport.h"

#ifndef GPK_GUI_DESKTOP_H_9283492374
#define GPK_GUI_DESKTOP_H_9283492374

namespace gpk
{

	template <typename _tElement>
	struct SRecyclableElementContainer {
		::gpk::view_bit<uint64_t>	Unused;
		::gpk::aobj<_tElement>		Elements;
		::gpk::au64					UnusedData;

		inlcxpr	const _tElement&	operator[]					(uint32_t index)			const				{ return Elements[index]; }
		inlcxpr	_tElement&			operator[]					(uint32_t index)								{ return Elements[index]; }

		inlcxpr	uint32_t			size						()							const	noexcept	{ return Elements.size(); }
		::gpk::error_t				push_back					(const _tElement& toPush)						{
			const int32_t					elemIndex					= Elements.push_back(toPush);
			ree_if(errored(elemIndex), "%s", "Failed to add element to container! Out of memory?");
			stacxpr	uint32_t				boolContainerElementSize	= sizeof(uint64_t) * ::gpk::PLATFORM_BYTE_BIT_COUNT;
			gpk_necall(UnusedData.resize((elemIndex + 1) / boolContainerElementSize + 1), "%s", "Out of memory?");
			Unused						= {UnusedData.begin(), (uint32_t)(elemIndex + 1)};
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
		int32_t						IdControl					= -1;
		::gpk::SDesktopItems		Items						= {};
		::gpk::aobj<::gpk::ai32>	Children					= {}; // Keep track of control list hierarchy.
	};

	::gpk::error_t				desktopCreatePaletteGrid	(::gpk::SGUI & gui, ::gpk::SDesktop& desktop);
	::gpk::error_t				desktopCreateControlList	(::gpk::SGUI & gui, ::gpk::SDesktop& desktop);
	::gpk::error_t				desktopCreateViewport		(::gpk::SGUI & gui, ::gpk::SDesktop& desktop);
	::gpk::error_t				desktopDeletePaletteGrid	(::gpk::SGUI & gui, ::gpk::SDesktop& desktop, int32_t iElement);
	::gpk::error_t				desktopDeleteControlList	(::gpk::SGUI & gui, ::gpk::SDesktop& desktop, int32_t iElement);
	::gpk::error_t				desktopDeleteViewport		(::gpk::SGUI & gui, ::gpk::SDesktop& desktop, int32_t iElement);

	::gpk::error_t				desktopControlListSetParent	(::gpk::SGUI & gui, ::gpk::SDesktop& desktop, int32_t iControlList, int32_t iParentControlList, int32_t iParentListItem);
	int64_t						desktopUpdate				(::gpk::SGUI & gui, ::gpk::SDesktop& desktop, ::gpk::SInput& input);

} // namespace

#endif // GPK_GUI_DESKTOP_H_9283492374
