#include "gpk_gui_desktop.h"
#include "gpk_label.h"

#define NEW_OR_UNUSED(ctoken, utoken)							\
	uint32_t					i##ctoken				= 0;	\
	while(i##ctoken < desktop.Items.ctoken##s.size()) {			\
		if(desktop.Items.ctoken##s.Unused[i##ctoken]) 			\
			break;												\
		++i##ctoken;											\
	}															\
	if(i##ctoken >= desktop.Items.ctoken##s.size())				\
		gpk_necs(i##ctoken = desktop.Items.ctoken##s.push_back({}));						\
	desktop.Items.ctoken##s.Unused[i##ctoken]	= false;									\
	::gpk::S##ctoken	& elementToSetUp	= desktop.Items.ctoken##s[i##ctoken]	= {};	\
	gpk_necs(gpk::utoken##Initialize(gui, elementToSetUp));								\
	gpk_necall(gpk::controlSetParent(gui, elementToSetUp.IdControl, desktop.IdControl), "%I - %I", elementToSetUp.IdControl, desktop.IdControl);

::gpk::error_t			gpk::desktopDeletePaletteGrid	(::gpk::SGUI & gui, ::gpk::SDesktop & desktop, ::gpk::cid_t iElement) { 
	desktop.Items.PaletteGrids.Unused[iElement] = true; 
	::gpk::controlDelete(gui, desktop.Items.PaletteGrids[iElement].IdControl); 
	desktop.Items.PaletteGrids[iElement] = {}; 
	return 0; 
}

::gpk::error_t			gpk::desktopCreateViewport		(::gpk::SGUI & gui, ::gpk::SDesktop & desktop)	{ NEW_OR_UNUSED(Viewport	, viewport		); return iViewport		; }
::gpk::error_t			gpk::desktopCreatePaletteGrid	(::gpk::SGUI & gui, ::gpk::SDesktop & desktop)	{ NEW_OR_UNUSED(PaletteGrid	, paletteGrid	); return iPaletteGrid	; }
::gpk::error_t			gpk::desktopCreateControlList	(::gpk::SGUI & gui, ::gpk::SDesktop & desktop)	{ NEW_OR_UNUSED(ControlList	, controlList	);
	if(iControlList >= desktop.Children.size())
		gpk_necs(desktop.Children.resize(iControlList + 1));

	desktop.Children[iControlList].clear();
	return iControlList;
}

::gpk::error_t			gpk::desktopDeleteControlList	(::gpk::SGUI & gui, ::gpk::SDesktop & desktop, ::gpk::cid_t iElement)	{
	gpk_necall(desktop.Items.ControlLists.size() <= (uint32_t)iElement, "%s", "Invalid control list.");
	::gpk::SControlList			& controlListToDelete			= desktop.Items.ControlLists[iElement];
	::gpk::acid					childLists						= desktop.Children[iElement];
	for(uint32_t iOption = 0, countOptions = childLists.size(); iOption < countOptions; ++iOption) {
		::gpk::cid_t				childControlListIndex			= childLists[iOption];
		if(desktop.Items.ControlLists.size() > (uint32_t)childControlListIndex && false == desktop.Items.ControlLists.Unused[childControlListIndex])
			gpk_necall(gpk::desktopDeleteControlList(gui, desktop, childControlListIndex), "%s", "??");
		else
			ef_if(childControlListIndex != -1, "%s", "");
	}
	if(desktop.Items.ControlLists.size() > (uint32_t)controlListToDelete.IndexParentList && false == desktop.Items.ControlLists.Unused[controlListToDelete.IndexParentList]) {
		if (desktop.Children[controlListToDelete.IndexParentList].size() > (uint32_t)controlListToDelete.IndexParentItem)
			desktop.Children[controlListToDelete.IndexParentList][controlListToDelete.IndexParentItem]	= -1;
	}
	desktop.Items.ControlLists.Unused[iElement]	= true;
	gpk_necall(gpk::controlDelete(gui, controlListToDelete.IdControl, true), "%s", "Failed to delete control! Deleted already?");
	controlListToDelete		= {};
	return 0;
}

::gpk::error_t			gpk::desktopDeleteViewport	(::gpk::SGUI & gui, ::gpk::SDesktop & desktop, ::gpk::cid_t iElement)						{
	desktop.Items.Viewports.Unused[iElement]	= true;
	const ::gpk::SViewport		& viewport						= desktop.Items.Viewports[iElement];
	switch(viewport.ControlType) {
	case ::gpk::GUI_CONTROL_TYPE_PaletteGrid	: desktop.Items.PaletteGrids	.Unused[viewport.IdDesktopElement] = true; info_printf("Deleting viewport's palette grid: %i. Control Id: %i."	, viewport.IdDesktopElement, desktop.Items.PaletteGrids	[viewport.IdDesktopElement].IdControl);break;
	case ::gpk::GUI_CONTROL_TYPE_Viewport		: desktop.Items.Viewports		.Unused[viewport.IdDesktopElement] = true; info_printf("Deleting viewport's vp: %i. Control Id: %i."			, viewport.IdDesktopElement, desktop.Items.Viewports	[viewport.IdDesktopElement].IdControl);break;
	case ::gpk::GUI_CONTROL_TYPE_ControlList	: ::gpk::desktopDeleteControlList(gui, desktop, viewport.IdDesktopElement); info_printf("Deleting viewport's vp: %i. Control Id: %i."			, viewport.IdDesktopElement, desktop.Items.Viewports	[viewport.IdDesktopElement].IdControl);break;
	case ::gpk::GUI_CONTROL_TYPE_COUNT			:
		error_printf("%s", "Some team of lammer idiots who can't code decided that not casing every enum value would be an error.");
		break;
	}
	info_printf("Deleting control for viewport: %i. Control Id: %i.", iElement, viewport.IdControl);
	::gpk::controlDelete(gui, viewport.IdControl);
	desktop.Items.Viewports[iElement]										= {};
	return 0;
}

static	::gpk::error_t	displace					(::gpk::SGUI & gui, ::gpk::cid_t iControl, const ::gpk::n2<double> & mouseDeltas)	{
	const ::gpk::n2<double>		currentScale									= gui.Zoom.DPI * gui.Zoom.ZoomLevel;
	::gpk::n2<double>			deltasScaled									= mouseDeltas.f64().GetScaled(1.0 / currentScale.x, 1.0 / currentScale.y);
	gui.Controls.Placement[iControl].Area.Offset								+= deltasScaled.i16();
	return ::gpk::controlMetricsInvalidate(gui, iControl);
}

static	::gpk::error_t	pushToFrontAndDisplace		(::gpk::SGUI & gui, ::gpk::cid_t iControl, ::gpk::SInput & input)							{
	const ::gpk::cid_t			parentControlId				= gui.Controls.States[iControl].Parent;
	for(uint32_t iChild = 0, childCount = gui.Controls.Children[parentControlId].size(); iChild < childCount; ++iChild)
		if(gui.Controls.Children[parentControlId][iChild] == iControl) {
			gpk_necs(gui.Controls.Children[parentControlId].remove(iChild));
			gpk_necs(gui.Controls.Children[parentControlId].push_back(iControl));
			break;
		}
	if(input.MouseCurrent.Deltas.x || input.MouseCurrent.Deltas.y)
		gpk_necall(::displace(gui, iControl, {(double)input.MouseCurrent.Deltas.x, (double)input.MouseCurrent.Deltas.y}), "%s", "Unknown issue!");
	return 0;
}

static	::gpk::error_t	unhideMenuHierarchy			(::gpk::SGUI & gui, ::gpk::SDesktop& desktop, ::gpk::SControlList& menu)	{
	if(menu.IndexParentList != -1 && false == desktop.Items.ControlLists.Unused[menu.IndexParentList])
		::unhideMenuHierarchy(gui, desktop, desktop.Items.ControlLists[menu.IndexParentList]);

	gui.Controls.SetHidden(menu.IdControl, false);
	const ::gpk::cid_t			parentControlIndex			= gui.Controls.States[menu.IdControl].Parent;
	if(-1 != parentControlIndex) {
		::gpk::acid & parentChildren = gui.Controls.Children[parentControlIndex];
		for(uint32_t iChild = 0, childCount = parentChildren.size(); iChild < childCount; ++iChild) {
			if(parentChildren[iChild] == menu.IdControl) {
				parentChildren.remove(iChild);
				parentChildren.push_back(menu.IdControl);
				::gpk::controlMetricsInvalidate(gui, parentControlIndex);
				break;
			}
		}
	}
	return 0;
}

static	::gpk::error_t	selectMenuHierarchy				(::gpk::SGUI & gui, ::gpk::SDesktop& desktop, ::gpk::SControlList& menu)	{
	if(menu.IndexParentList != -1 && false == desktop.Items.ControlLists.Unused[menu.IndexParentList]) {
		::selectMenuHierarchy(gui, desktop, desktop.Items.ControlLists[menu.IndexParentList]);
		desktop.Items.ControlLists[menu.IndexParentList].IdSelected	= menu.IndexParentItem;
	}

	gui.Controls.SetHidden(menu.IdControl, false);
	return 0;
}

static	::gpk::error_t	unhoverMenuHierarchy						(::gpk::SGUI & gui, ::gpk::SDesktop & desktop, ::gpk::SControlList & menu)	{
	if(desktop.Items.ControlLists.size() > (uint32_t)menu.IndexParentList)
		::unhoverMenuHierarchy(gui, desktop, desktop.Items.ControlLists[menu.IndexParentList]);
	
	gui.Controls.SetHovered(menu.IdControl, false);

	for(uint32_t iItem = 0; iItem < menu.IdControls.size(); ++iItem)
		gui.Controls.SetHovered(menu.IdControls[iItem], false);

	return 0;
}

static	::gpk::error_t	clearMenuHierarchy	(::gpk::SGUI & gui, ::gpk::SDesktop & desktop)	{
	::gpk::SRecyclableElementContainer<::gpk::SControlList>	& menus	= desktop.Items.ControlLists;
	for(uint32_t iMenu = 0, countMenus = menus.size(); iMenu < countMenus; ++iMenu) {
		::gpk::SControlList			& menu				= menus[iMenu];
		menu.IdSelected			= -1;
		if(desktop.Children.size() <= (uint32_t)menu.IndexParentList || desktop.Children[menu.IndexParentList].size() <= (uint32_t)menu.IndexParentItem) // skip root menus
			continue;

		gui.Controls.SetHidden(menu.IdControl, true);
	}
	return 0;
}

static	int64_t			viewportUpdate		(::gpk::SGUI & gui, ::gpk::SDesktop & desktop, ::gpk::SInput & input, ::gpk::cid_t iViewport)							{
	::gpk::SViewport			& vp				= desktop.Items.Viewports[iViewport];
	if(gui.Controls.States[vp.IdControls[::gpk::VIEWPORT_CONTROL_CLOSE]].IsAction())
		::gpk::desktopDeleteViewport(gui, desktop, iViewport);
	else if(gui.Controls.IsPressed(vp.IdControls[::gpk::VIEWPORT_CONTROL_TITLE]))
		::pushToFrontAndDisplace(gui, vp.IdControl, input);
	else if(gui.Controls.IsPressed(vp.IdControls[::gpk::VIEWPORT_CONTROL_RESIZE_BOTTOM_RIGHT])) {
		if(input.MouseCurrent.Deltas.x || input.MouseCurrent.Deltas.y) {
			//::gpk::n2<int32_t> nextSize = gui.Controls.Controls[(uint32_t)vp.IdControl].Area.Size + ::gpk::n2<int32_t>{input.MouseCurrent.Deltas.x, input.MouseCurrent.Deltas.y};
			//if(::gpk::in_range(nextSize, {{}, gui.LastSize.i32()})) {
				::gpk::n2i32			mouseDeltas			= {input.MouseCurrent.Deltas.x, input.MouseCurrent.Deltas.y};
				const ::gpk::n2f64		currentScale		= gui.Zoom.DPI * gui.Zoom.ZoomLevel;
				::gpk::n2i16			deltasScaled		= mouseDeltas.GetScaled(1.0 / currentScale.x, 1.0 / currentScale.y).i16();
				gui.Controls.Placement[vp.IdControl].Area.Size									+= deltasScaled;
				gui.Controls.Placement[vp.IdControls[::gpk::VIEWPORT_CONTROL_TARGET]].Area.Size	+= deltasScaled;
				::gpk::controlMetricsInvalidate(gui, vp.IdControl);
			//}
		}
	}
	return 0;
}
int64_t					gpk::desktopUpdate						(::gpk::SGUI & gui, ::gpk::SDesktop & desktop, ::gpk::SInput & input)							{
	for(uint32_t iViewport = 0; iViewport < desktop.Items.Viewports.size(); ++iViewport) {
		if(desktop.Items.Viewports.Unused[iViewport])
			continue;
		::viewportUpdate(gui, desktop, input, (::gpk::cid_t)iViewport);
	}

	::gpk::acid					controlsToProcess						= {};
	::gpk::guiGetProcessableControls(gui, controlsToProcess);

	bool						inControlArea						= false;
	//bool						anyControlPressed					= false;
	for(uint32_t iControlToProcess = 0, countControls = controlsToProcess.size(); iControlToProcess < countControls; ++iControlToProcess) {
		const uint32_t				iControl							= controlsToProcess[iControlToProcess];
		const ::gpk::SControlArea	& controlMetrics					= gui.Controls.Area[iControl];
		const ::gpk::SControlState	& controlState						= gui.Controls.States[iControl];
		if(iControl != (uint32_t)desktop.IdControl)
			inControlArea			= inControlArea || ::gpk::in_range(gui.CursorPos.i16(), controlMetrics.Total.Global);
		//if(controlState.Pressed && (int32_t)iControl != app.Desktop.IdControl)
		//	anyControlPressed															= true;

		if(controlState.IsAction()) {
			info_printf("Executed %u.", iControl);
			::clearMenuHierarchy(gui, desktop);
		}
	}

	bool						selectedMenu					= false;
	::gpk::SRecyclableElementContainer<::gpk::SControlList>	& menus			= desktop.Items.ControlLists;
	for(uint32_t iMenu = 0, countMenus = menus.size(); iMenu < countMenus; ++iMenu) {
		::gpk::SControlList			& menu				= menus[iMenu];
		if(desktop.Children.size() <= (uint32_t)menu.IndexParentList)
			continue;
		else if(desktop.Children[menu.IndexParentList].size() <= (uint32_t)menu.IndexParentItem) // parent
			continue;

		::gpk::SControlList					& parentMenu		= menus[menu.IndexParentList];
		const ::gpk::SControlState			& parentItemState	= gui.Controls.States[parentMenu.IdControls[menu.IndexParentItem]];
		if(parentItemState.IsHovered()) {
			::unhideMenuHierarchy(gui, desktop, menu);
			if(parentItemState.IsAction()) {
				::clearMenuHierarchy(gui, desktop);
				::selectMenuHierarchy(gui, desktop, menu);
				selectedMenu																= true;
			}
		}
		else {
			const ::gpk::SControlArea		& controlListMetrics				= gui.Controls.Area[menu.IdControl];
			if(::gpk::in_range(gui.CursorPos.i16(), controlListMetrics.Total.Global) && ::gpk::controlHidden(gui, menu.IdControl))
				::unhideMenuHierarchy(gui, desktop, menu);

			else if(parentMenu.IdSelected != menu.IndexParentItem)
				gui.Controls.SetHidden(menu.IdControl, true);
		}
	}

	if(input.ButtonDown(1) || input.ButtonDown(2)) {
		::clearMenuHierarchy(gui, desktop);
		for(uint32_t iMenu = 0, countMenus = menus.size(); iMenu < countMenus; ++iMenu) {
			::gpk::SControlList																& menu								= menus[iMenu];
			if(desktop.Children.size() <= (uint32_t)menu.IndexParentList || desktop.Children[menu.IndexParentList].size() <= (uint32_t)menu.IndexParentItem) // parent
				continue;
			::unhoverMenuHierarchy(gui, desktop, menu);
		}
	}
	if(input.ButtonUp(0) && false == selectedMenu)
		::clearMenuHierarchy(gui, desktop);

	int64_t																	resultEvent							= 0;
	for(uint32_t iMenu = 0, countMenus = menus.size(); iMenu < countMenus; ++iMenu) {
		::gpk::SControlList																& menu								= menus[iMenu];
		bool																			eventFound							= false;
		for(uint32_t iItem = 0, countItems = menu.IdControls.size(); iItem < countItems; ++iItem) {
			if(false == gui.Controls.States[menu.IdControls[iItem]].IsAction())
				continue;
			if(desktop.Children[iMenu].size() < menu.IdControls.size())
				desktop.Children[iMenu].resize(menu.IdControls.size(), -1);
			if(desktop.Children[iMenu][iItem] != -1)
				continue;
			resultEvent			= menu.Events[iItem];
			eventFound			= true;
		}
		if(eventFound)
			break;
	}
	return resultEvent;
}

::gpk::error_t			gpk::desktopControlListSetParent(::gpk::SGUI & gui, ::gpk::SDesktop & desktop, ::gpk::cid_t iControlList, ::gpk::cid_t iParentControlList, ::gpk::cid_t iParentListItem)			{
	ree_if((desktop.Items.ControlLists.size() <= (uint32_t)iControlList) || desktop.Items.ControlLists.Unused[iControlList], "Invalid control list: %u.", iControlList);
	::gpk::SControlList			& controlList				= desktop.Items.ControlLists[iControlList];
	const int32_t				oldParentList				= controlList.IndexParentList;
	const int32_t				oldParentItem				= controlList.IndexParentItem;
	if(oldParentItem == iParentListItem && oldParentList == iParentControlList)	// Exit early if there is nothing to do here.
		return 0;
	controlList.IndexParentList = iParentControlList;
	controlList.IndexParentItem	= iParentListItem;
	
	if((desktop.Items.ControlLists.size() <= (uint32_t)iParentControlList) || desktop.Items.ControlLists.Unused[iParentControlList]) {
		ree_if(-1 != iParentControlList, "Invalid parent control list: %i. List item: %i.", iParentControlList, iParentListItem);
		gui.Controls.SetHidden(controlList.IdControl, false);
	}
	else { // update tree
		::gpk::SControlList			& parentControlList			= desktop.Items.ControlLists[iParentControlList];
		if(desktop.Children.size() > (uint32_t)oldParentList && desktop.Children[oldParentList].size() > (uint32_t)oldParentItem)
			desktop.Children[oldParentList][oldParentItem]							= -1;

		::gpk::acid					& parentChildren			= desktop.Children[iParentControlList];
		if(parentChildren.size() < parentControlList.IdControls.size())
			parentChildren.resize(parentControlList.IdControls.size(), -1);

		ree_if(parentChildren.size() <= ((uint32_t)iParentListItem) && iParentListItem != -1, "Invalid parent item: %i.", iParentListItem);
		::gpk::SControlConstraints	& controlListConstraints	= gui.Controls.Constraints[controlList.IdControl];
		if(iParentListItem == -1) 
			gui.Controls.SetHidden(controlList.IdControl, false);
		else {
			controlList.Orientation	= ::gpk::DIRECTION_DOWN;
			parentChildren[iParentListItem]	= iControlList;
			gui.Controls.SetHidden(controlList.IdControl, true);
			::gpk::controlListArrange(gui, parentControlList);
			if(controlList.Orientation != ::gpk::DIRECTION_RIGHT)
				controlListConstraints.DockToControl.Left	= parentControlList.IdControls[iParentListItem];
			if(parentControlList.IndexParentItem != -1 && parentControlList.IndexParentList != -1) {
				controlListConstraints.DockToControl		= {-1, -1, -1, -1};
				controlListConstraints.DockToControl.Right	= parentControlList.IdControls[iParentListItem];
				::gpk::SControlPlacement	& controlListControl				= gui.Controls.Placement[controlList.IdControl];
				::gpk::SControlConstraints	& parentControlListConstraints		= gui.Controls.Constraints[parentControlList.IdControl];
				if(gui.Controls.Constraints[parentControlList.IdControl].DockToControl.Bottom != -1) {
					int32_t offsety = gui.Controls.Placement[parentControlListConstraints.DockToControl.Bottom].Area.Offset.y + gui.Controls.Placement[parentControlListConstraints.DockToControl.Bottom].Area.Size.y + gui.Controls.Placement[parentControlList.IdControls[iParentListItem]].Area.Size.y * iParentListItem;
					controlListControl.Area.Offset.y	= (int16_t)offsety;
				}
				else
					controlListControl.Area.Offset.y	= gui.Controls.Placement[parentControlList.IdControl].Area.Offset.y + gui.Controls.Placement[parentControlList.IdControls[iParentListItem]].Area.Offset.y;
			}
			else
				controlListConstraints.DockToControl.Bottom	= parentControlList.IdControl;
		}
	}
	return ::gpk::controlListArrange(gui, controlList);
}
