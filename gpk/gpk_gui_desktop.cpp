#include "gpk_gui_desktop.h"
#include "gpk_label.h"

#define NEW_OR_UNUSED(ctoken, utoken)																																			\
	uint32_t																	i##ctoken##									= 0;												\
	while(i##ctoken## < desktop.Items.##ctoken##s.size()) {																														\
		if(desktop.Items.##ctoken##s.Unused[i##ctoken##]) 																														\
			break;																																								\
		++i##ctoken##;																																							\
	}																																											\
	if(i##ctoken## >= desktop.Items.##ctoken##s.size())																															\
		gpk_necall(i##ctoken## = desktop.Items.##ctoken##s.push_back({}), "Out of memory?");																					\
	desktop.Items.##ctoken##s.Unused[i##ctoken##]	= false;																													\
	::gpk::S##ctoken##															& elementToSetUp							= desktop.Items.##ctoken##s[i##ctoken##]	= {};	\
	gpk_necall(::gpk::##utoken##Initialize(gui, elementToSetUp), "Unknown.");																									\
	gpk_necall(::gpk::controlSetParent(gui, elementToSetUp.IdControl, desktop.IdControl), "Invalid desktop id?");

			::gpk::error_t												gpk::desktopDeletePaletteGrid					(::gpk::SGUI& gui, ::gpk::SDesktop& desktop, int32_t iElement)						{ desktop.Items.PaletteGrids.Unused[iElement] = true; ::gpk::controlDelete(gui, desktop.Items.PaletteGrids[iElement].IdControl); desktop.Items.PaletteGrids[iElement] = {}; return 0; }
			::gpk::error_t												gpk::desktopCreateViewport						(::gpk::SGUI& gui, ::gpk::SDesktop& desktop)										{ NEW_OR_UNUSED(Viewport	, viewport		); return iViewport		; }
			::gpk::error_t												gpk::desktopCreatePaletteGrid					(::gpk::SGUI& gui, ::gpk::SDesktop& desktop)										{ NEW_OR_UNUSED(PaletteGrid	, paletteGrid	); return iPaletteGrid	; }
			::gpk::error_t												gpk::desktopCreateControlList					(::gpk::SGUI& gui, ::gpk::SDesktop& desktop)										{ NEW_OR_UNUSED(ControlList	, controlList	); 
	if(iControlList >= desktop.Children.size())
		gpk_necall(desktop.Children.resize(iControlList + 1), "Out of memory?");
	return iControlList; 
}

			::gpk::error_t												gpk::desktopDeleteControlList					(::gpk::SGUI& gui, ::gpk::SDesktop& desktop, int32_t iElement)						{ 
	gpk_necall(desktop.Items.ControlLists.size() <= (uint32_t)iElement, "Invalid control list.");
	::gpk::SControlList															controlListToDelete								= desktop.Items.ControlLists[iElement];
	::gpk::array_pod<int32_t>													childLists										= desktop.Children[iElement];
	for(uint32_t iOption = 0, countOptions = childLists.size(); iOption < countOptions; ++iOption) {
		int32_t																		childControlListIndex							= desktop.Children[iElement][iOption];
		if(desktop.Items.ControlLists.size() > (uint32_t)childControlListIndex && false == desktop.Items.ControlLists.Unused[childControlListIndex]) 
			gpk::desktopDeleteControlList(gui, desktop, desktop.Children[iElement][iOption]);
		else 
			error_if(childControlListIndex != -1, "");
	}
	if(desktop.Items.ControlLists.size() > (uint32_t)controlListToDelete.IndexParentList && false == desktop.Items.ControlLists.Unused[controlListToDelete.IndexParentList]) {
		if (desktop.Children[controlListToDelete.IndexParentList].size() > (uint32_t)controlListToDelete.IndexParentItem)
			desktop.Children[controlListToDelete.IndexParentList][controlListToDelete.IndexParentItem]				= -1;
	}
	desktop.Items.ControlLists.Unused[iElement]								= true; 
	gpk_necall(::gpk::controlDelete(gui, controlListToDelete.IdControl), "Failed to delete control! Deleted already?");
	controlListToDelete														= {};
	return 0;
}

			::gpk::error_t												gpk::desktopDeleteViewport						(::gpk::SGUI& gui, ::gpk::SDesktop& desktop, int32_t iElement)						{ 
	desktop.Items.Viewports.Unused[iElement]								= true; 
	const ::gpk::SViewport														& viewport										= desktop.Items.Viewports[iElement];
	switch(viewport.ControlType) {
	case ::gpk::GUI_CONTROL_TYPE_PaletteGrid	: desktop.Items.PaletteGrids	.Unused[viewport.IdDesktopElement] = true; info_printf("Deleting viewport's palette grid: %i. Control Id: %i."	, viewport.IdDesktopElement, desktop.Items.PaletteGrids	[viewport.IdDesktopElement].IdControl);break;
	case ::gpk::GUI_CONTROL_TYPE_Viewport		: desktop.Items.Viewports		.Unused[viewport.IdDesktopElement] = true; info_printf("Deleting viewport's vp: %i. Control Id: %i."			, viewport.IdDesktopElement, desktop.Items.Viewports	[viewport.IdDesktopElement].IdControl);break;
	case ::gpk::GUI_CONTROL_TYPE_ControlList	: ::gpk::desktopDeleteControlList(gui, desktop, viewport.IdDesktopElement); info_printf("Deleting viewport's vp: %i. Control Id: %i."			, viewport.IdDesktopElement, desktop.Items.Viewports	[viewport.IdDesktopElement].IdControl);break;
	}
	info_printf("Deleting control for viewport: %i. Control Id: %i.", iElement, viewport.IdControl);
	::gpk::controlDelete(gui, viewport.IdControl); 
	desktop.Items.Viewports[iElement]										= {};
	return 0;
}

static		::gpk::error_t												displace										(::gpk::SGUI& gui, int32_t iControl, const ::gpk::SCoord2<double> & mouseDeltas)	{
	const ::gpk::SCoord2<double>												currentScale									= gui.Zoom.DPI * gui.Zoom.ZoomLevel;
	::gpk::SCoord2<double>														deltasScaled									= mouseDeltas.Cast<double>().GetScaled(1.0 / currentScale.x, 1.0 / currentScale.y);
	gui.Controls.Controls[iControl].Area.Offset								+= deltasScaled.Cast<int32_t>();
	return ::gpk::controlMetricsInvalidate(gui, iControl);
}

static		::gpk::error_t												pushToFrontAndDisplace							(::gpk::SGUI& gui, int32_t iControl, ::gpk::SInput& input)							{
	const int32_t																parentControlId									= gui.Controls.Controls[iControl].IndexParent;
	for(uint32_t iChild = 0, childCount = gui.Controls.Children[parentControlId].size(); iChild < childCount; ++iChild)
		if(gui.Controls.Children[parentControlId][iChild] == iControl) {
			gpk_necall(gui.Controls.Children[parentControlId].remove(iChild)		, "Unknown issue!");
			gpk_necall(gui.Controls.Children[parentControlId].push_back(iControl)	, "Unknown issue!");
			break;
		}
	if(input.MouseCurrent.Deltas.x || input.MouseCurrent.Deltas.y) 
		gpk_necall(::displace(gui, iControl, {(double)input.MouseCurrent.Deltas.x, (double)input.MouseCurrent.Deltas.y}), "Unknown issue!");
	return 0;
}

static		::gpk::error_t												unhideMenuHierarchy						(::gpk::SGUI& gui, ::gpk::SDesktop& desktop, ::gpk::SControlList& menu)	{
	if(menu.IndexParentList != -1 && false == desktop.Items.ControlLists.Unused[menu.IndexParentList]) 
		::unhideMenuHierarchy(gui, desktop, desktop.Items.ControlLists[menu.IndexParentList]);

	gui.Controls.States[menu.IdControl].Hidden								= false;
	const int32_t																parentControlIndex						= gui.Controls.Controls[menu.IdControl].IndexParent;
	if(-1 != parentControlIndex) {
		::gpk::array_pod<int32_t> & parentChildren = gui.Controls.Children[parentControlIndex];
		for(uint32_t iChild = 0; iChild < parentChildren.size(); ++iChild) {
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


static		::gpk::error_t												selectMenuHierarchy						(::gpk::SGUI& gui, ::gpk::SDesktop& desktop, ::gpk::SControlList& menu)	{
	if(menu.IndexParentList != -1 && false == desktop.Items.ControlLists.Unused[menu.IndexParentList]) {
		::selectMenuHierarchy(gui, desktop, desktop.Items.ControlLists[menu.IndexParentList]);
		desktop.Items.ControlLists[menu.IndexParentList].IdSelected				= menu.IndexParentItem;
	}
	gui.Controls.States[menu.IdControl].Hidden = false;
	return 0;
}

static		::gpk::error_t												unhoverMenuHierarchy						(::gpk::SGUI& gui, ::gpk::SDesktop& desktop, ::gpk::SControlList& menu)	{
	if(desktop.Items.ControlLists.size() > (uint32_t)menu.IndexParentList)
		::unhoverMenuHierarchy(gui, desktop, desktop.Items.ControlLists[menu.IndexParentList]);
	gui.Controls.States[menu.IdControl].Hover									= false;
	for(uint32_t iItem = 0; iItem < menu.IdControls.size(); ++iItem)
		gui.Controls.States[menu.IdControls[iItem]].Hover							= false;
	return 0;
}

static		::gpk::error_t												clearMenuHierarchy						(::gpk::SGUI& gui, ::gpk::SDesktop& desktop)	{
	::gpk::SRecyclableElementContainer<::gpk::SControlList>							& menus								= desktop.Items.ControlLists;
	for(uint32_t iMenu = 0, countMenus = menus.size(); iMenu < countMenus; ++iMenu) {
		::gpk::SControlList																& menu								= menus[iMenu];
		menu.IdSelected																= -1;
		if(desktop.Children.size() <= (uint32_t)menu.IndexParentList || desktop.Children[menu.IndexParentList].size() <= (uint32_t)menu.IndexParentItem) // skip root menus
			continue;
		gui.Controls.States[menu.IdControl].Hidden									= true;
	}
	return 0;
}

			int64_t														gpk::desktopUpdate						(::gpk::SGUI& gui, ::gpk::SDesktop& desktop, ::gpk::SInput& input)							{
	for(uint32_t iViewport = 0; iViewport < desktop.Items.Viewports.size(); ++iViewport) {
		if(desktop.Items.Viewports.Unused[iViewport])
			continue;

		::gpk::SViewport															& vp									= desktop.Items.Viewports[iViewport];
		if(gui.Controls.States[(uint32_t)vp.IdControls[::gpk::VIEWPORT_CONTROL_CLOSE]].Execute) 
			::gpk::desktopDeleteViewport(gui, desktop, iViewport);
		else if(gui.Controls.States[(uint32_t)vp.IdControls[::gpk::VIEWPORT_CONTROL_TITLE]].Pressed) 
			::pushToFrontAndDisplace(gui, vp.IdControl, input);
		else if(gui.Controls.States[(uint32_t)vp.IdControls[::gpk::VIEWPORT_CONTROL_RESIZE_BOTTOM_RIGHT]].Pressed) {
			if(input.MouseCurrent.Deltas.x || input.MouseCurrent.Deltas.y) {
				//::gpk::SCoord2<int32_t> nextSize = gui.Controls.Controls[(uint32_t)vp.IdControl].Area.Size + ::gpk::SCoord2<int32_t>{input.MouseCurrent.Deltas.x, input.MouseCurrent.Deltas.y};
				//if(::gpk::in_range(nextSize, {{}, gui.LastSize.Cast<int32_t>()})) {
					::gpk::SCoord2<int32_t>														mouseDeltas										= {input.MouseCurrent.Deltas.x, input.MouseCurrent.Deltas.y};
					const ::gpk::SCoord2<double>												currentScale									= gui.Zoom.DPI * gui.Zoom.ZoomLevel;
					::gpk::SCoord2<double>														deltasScaled									= mouseDeltas.Cast<double>().GetScaled(1.0 / currentScale.x, 1.0 / currentScale.y);
					gui.Controls.Controls[(uint32_t)vp.IdControl].Area.Size										+= deltasScaled.Cast<int32_t>();
					gui.Controls.Controls[(uint32_t)vp.IdControls[::gpk::VIEWPORT_CONTROL_TARGET]].Area.Size	+= deltasScaled.Cast<int32_t>();
					::gpk::controlMetricsInvalidate(gui, vp.IdControl);
				//}
			}
		}
	}

	::gpk::array_pod<uint32_t>													controlsToProcess						= {};
	::gpk::guiGetProcessableControls(gui, controlsToProcess);

	bool																			inControlArea						= false;
	//bool																			anyControlPressed					= false;
	for(uint32_t iControlToProcess = 0, countControls = controlsToProcess.size(); iControlToProcess < countControls; ++iControlToProcess) {
		const uint32_t																	iControl							= controlsToProcess[iControlToProcess];
		const ::gpk::SControlState														& controlState						= gui.Controls.States[iControl];
		const ::gpk::SControlMetrics													& controlMetrics					= gui.Controls.Metrics[iControl];
		if(iControl != (uint32_t)desktop.IdControl)
			inControlArea																= inControlArea || ::gpk::in_range(gui.CursorPos.Cast<int32_t>(), controlMetrics.Total.Global);
		//if(controlState.Pressed && (int32_t)iControl != app.Desktop.IdControl)
		//	anyControlPressed															= true;

		if(controlState.Execute) {
			info_printf("Executed %u.", iControl);
			::clearMenuHierarchy(gui, desktop);
		}
	}

	bool selectedMenu = false;
	::gpk::SRecyclableElementContainer<::gpk::SControlList>							& menus								= desktop.Items.ControlLists;
	for(uint32_t iMenu = 0, countMenus = menus.size(); iMenu < countMenus; ++iMenu) {
		::gpk::SControlList																& menu								= menus[iMenu];
		if(desktop.Children.size() <= (uint32_t)menu.IndexParentList)
			continue;
		else if(desktop.Children[menu.IndexParentList].size() <= (uint32_t)menu.IndexParentItem) // parent 
			continue;

		::gpk::SControlList																& parentMenu						= menus[menu.IndexParentList];
		const ::gpk::SControlState														& parentItemState					= gui.Controls.States[parentMenu.IdControls[menu.IndexParentItem]]; 
		::gpk::SControlState															& controlStateMenu					= gui.Controls.States[menu.IdControl];
		if(parentItemState.Hover) {
			::unhideMenuHierarchy(gui, desktop, menu);
			if(parentItemState.Execute) {
				::clearMenuHierarchy(gui, desktop);
				::selectMenuHierarchy(gui, desktop, menu);
				selectedMenu																= true;
			}
		}
		else {
			const ::gpk::SControlMetrics													& controlListMetrics				= gui.Controls.Metrics[menu.IdControl];
			if(::gpk::in_range(gui.CursorPos.Cast<int32_t>(), controlListMetrics.Total.Global) && controlStateMenu.Hidden == false) 
				::unhideMenuHierarchy(gui, desktop, menu);

			else if(parentMenu.IdSelected != menu.IndexParentItem)
				controlStateMenu.Hidden												= true;
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
			if(false == gui.Controls.States[menu.IdControls[iItem]].Execute) 
				continue;
			if(desktop.Children[iMenu].size() < menu.IdControls.size())
				desktop.Children[iMenu].resize(menu.IdControls.size(), -1);
			if(desktop.Children[iMenu][iItem] != -1)
				continue;
			resultEvent																	= menu.Events[iItem];
			eventFound																	= true;
		}
		if(eventFound)
			break;
	}
	return resultEvent;
}

			::gpk::error_t												gpk::desktopControlListSetParent					(::gpk::SGUI& gui, ::gpk::SDesktop& desktop, int32_t iControlList, int32_t iParentControlList, int32_t iParentListItem)			{
	ree_if((desktop.Items.ControlLists.size() <= (uint32_t)iControlList) || desktop.Items.ControlLists.Unused[iControlList], "Invalid control list: %u.", iControlList);
	::gpk::SControlList															& controlList											= desktop.Items.ControlLists[iControlList];
	const int32_t																oldParentList											= controlList.IndexParentList;
	const int32_t																oldParentItem											= controlList.IndexParentItem;
	if(oldParentItem == iParentListItem && oldParentList == iParentControlList)	// Exit early if there is nothing to do here.
		return 0;
	controlList.IndexParentList 											= iParentControlList;
	controlList.IndexParentItem												= iParentListItem;
	::gpk::SControlState														& controlListStates										= gui.Controls.States[controlList.IdControl];
	if((desktop.Items.ControlLists.size() <= (uint32_t)iParentControlList) || desktop.Items.ControlLists.Unused[iParentControlList]) {
		ree_if(-1 != iParentControlList, "Invalid parent control list: %i. List item: %i.", iParentControlList, iParentListItem);
		controlListStates.Hidden												= false;
	}
	else { // update tree
		::gpk::SControlList															& parentControlList										= desktop.Items.ControlLists[iParentControlList];
		if(desktop.Children.size() > (uint32_t)oldParentList && desktop.Children[oldParentList].size() > (uint32_t)oldParentItem)
			desktop.Children[oldParentList][oldParentItem]							= -1;

		::gpk::array_pod<int32_t>													& parentChildren										= desktop.Children[iParentControlList];
		if(parentChildren.size() < parentControlList.IdControls.size())
			parentChildren.resize(parentControlList.IdControls.size(), -1);

		ree_if(parentChildren.size() <= ((uint32_t)iParentListItem) && iParentListItem != -1, "Invalid parent item!");
		::gpk::SControlConstraints													& controlListConstraints								= gui.Controls.Constraints[controlList.IdControl];
		if(iParentListItem != -1) {
			controlList.Orientation													= ::gpk::CONTROL_LIST_DIRECTION_VERTICAL;
			parentChildren[iParentListItem]											= iControlList;
			controlListStates.Hidden												= true;
			::gpk::controlListArrange(gui, parentControlList);
			if(controlList.Orientation != ::gpk::CONTROL_LIST_DIRECTION_HORIZONTAL)
				controlListConstraints.DockToControl.Left		= parentControlList.IdControls[iParentListItem];
			if(parentControlList.IndexParentItem != -1 && parentControlList.IndexParentList != -1) {
				controlListConstraints.DockToControl			= {-1, -1, -1, -1};
				controlListConstraints.DockToControl.Right		= parentControlList.IdControls[iParentListItem];
				::gpk::SControl														& controlListControl									= gui.Controls.Controls[controlList.IdControl];
				::gpk::SControlConstraints											& parentControlListConstraints							= gui.Controls.Constraints[parentControlList.IdControl];
				if(gui.Controls.Constraints[parentControlList.IdControl].DockToControl.Bottom != -1) {
					int32_t offsety = gui.Controls.Controls	[parentControlListConstraints.DockToControl.Bottom].Area.Offset.y + gui.Controls.Controls[parentControlListConstraints.DockToControl.Bottom].Area.Size.y + gui.Controls.Controls[parentControlList.IdControls[iParentListItem]].Area.Size.y * iParentListItem;
					controlListControl.Area.Offset.y			= offsety;
				}
				else
					controlListControl.Area.Offset.y			= gui.Controls.Controls[parentControlList.IdControl].Area.Offset.y + gui.Controls.Controls[parentControlList.IdControls[iParentListItem]].Area.Offset.y;
			}
			else
				controlListConstraints.DockToControl.Bottom	= parentControlList.IdControl;
		}
		else
			controlListStates.Hidden						= false;
	}
	return ::gpk::controlListArrange(gui, controlList);
}
