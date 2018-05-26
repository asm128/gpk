#include "gpk_gui.h"
#include "gpk_bitmap_target.h"

			::gpk::error_t								gpk::controlCreate							(::gpk::SGUI& gui)										{
	::gpk::error_t												iControl									= -1;
	gpk_necall(iControl = ::gpk::resize( gui.Controls.size() + 1
		, gui.Controls
		, gui.ControlStates
		, gui.ControlMetrics
		, gui.ControlText
		, gui.ControlChildren
		) - 1, "Failed to resize! Out of memory?");
	gui.ControlStates	[iControl]							= {};
	gui.ControlStates	[iControl].Outdated					= true;
	gui.Controls		[iControl].IndexParent				= -1;
	gui.Controls		[iControl].Align					= ::gpk::ALIGN_TOP_LEFT;
	gui.Controls		[iControl].Border					= 
	gui.Controls		[iControl].Margin					= {};
	gui.Controls		[iControl].Area						= {{0, 0}, {16, 16}};
	return iControl; 
}

			::gpk::error_t								gpk::controlDelete							(::gpk::SGUI& gui, int32_t iControl)					{ 
	ree_if((gui.Controls.size() <= uint32_t(iControl)), "Invalid control id: %u.", iControl);
	::gpk::SControlState										& controlState								= gui.ControlStates[iControl];
	ree_if(controlState.Unused, "Invalid control id: %u.", iControl);
	controlState.Unused										= true;
	const uint32_t												indexParent									= (uint32_t)gui.Controls[iControl].IndexParent;
	if(indexParent < gui.Controls.size() && false == gui.ControlStates[indexParent].Unused) {
		::gpk::array_pod<int32_t>									& children									= gui.ControlChildren[indexParent];
		for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
			if(children[iChild] == iControl) {
				gpk_necall(children.remove(iChild), "Failed to remove child at index: %u.", iChild);
				break;
			}
	}
	::gpk::array_view<int32_t>									& children									= gui.ControlChildren[iControl];
	for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
		gui.ControlStates[children[iChild]].Unused				= true;
	return 0; 
}

	::gpk::error_t										gpk::controlSetParent						(::gpk::SGUI& gui, int32_t iControl, int32_t iParent)	{
	ree_if((gui.Controls.size() <= uint32_t(iControl)) || gui.ControlStates[iControl].Unused, "Invalid control id: %u.", iControl);
	::gpk::SControl												& control									= gui.Controls[iControl];
	if(control.IndexParent == iParent)	// Exit early if there is nothing to do here.
		return 0;

	control.IndexParent										= iParent;
	if(gui.Controls.size() <= ((uint32_t)iParent) || gui.ControlStates[iParent].Unused)
		return 0;

	// Set iControl to parent's children array.
	::gpk::array_pod<int32_t>									& children									= gui.ControlChildren[iParent];
	for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
		if(children[iChild] == iControl)
			return 0;

	gpk_necall(children.push_back(iControl), "Out of memory?");
	return 0;
}
	
static		::gpk::error_t								controlUpdateMetrics						(::gpk::SGUI& gui, int32_t iControl, ::gpk::grid_view<::gpk::SColorBGRA>& target)					{
	ree_if((gui.Controls.size() <= uint32_t(iControl)), "Invalid control id: %u.", iControl);
	::gpk::SControlState										& controlState								= gui.ControlStates[iControl];
	ree_if(controlState.Unused, "Invalid control id: %u.", iControl);
	const ::gpk::SControl										& control									= gui.Controls[iControl];
	::gpk::SCoord2<int32_t>										targetSize									= target.metrics().Cast<int32_t>();
	::gpk::SCoord2<double>										scale										= gui.Zoom.DPI * gui.Zoom.ZoomLevel;
	const bool													isValidParent								= gui.Controls.size() > (uint32_t)control.IndexParent && false == gui.ControlStates[control.IndexParent].Unused;
	if(isValidParent) {
		::controlUpdateMetrics(gui, control.IndexParent, target);
		targetSize												= gui.ControlMetrics[control.IndexParent].Client.Global.Size;
	}
	if(false == controlState.Outdated) 
		return 0;
	::gpk::SControlMetrics										& controlMetrics							= gui.ControlMetrics[iControl];
	controlMetrics.Client.Local								= 
		{	{ control.Margin.Left + control.Border.Left, control.Margin.Top + control.Border.Top }
		,	{ control.Area.Size.x - (control.Margin.Left	+ control.Border.Left	+ control.Margin.Right	+ control.Border.Right)
			, control.Area.Size.y - (control.Margin.Top		+ control.Border.Top	+ control.Margin.Bottom	+ control.Border.Bottom)
			}
		};
	::gpk::SCoord2<int32_t>										finalPosition	= control.Area.Offset;
	::gpk::SCoord2<int32_t>										finalSize		= control.Area.Size;

	controlMetrics.Total	.Local							= {finalPosition, finalSize};
		 if(::gpk::bit_true(control.Align, ::gpk::ALIGN_HCENTER	))	{ controlMetrics.Total.Local.Offset.x = targetSize.x / 2 - finalSize.x / 2 + finalPosition.x; }
	else if(::gpk::bit_true(control.Align, ::gpk::ALIGN_RIGHT	))	{ controlMetrics.Total.Local.Offset.x = targetSize.x - (finalSize.x + finalPosition.x); }
	else															{}

		 if(::gpk::bit_true(control.Align, ::gpk::ALIGN_VCENTER	))	{ controlMetrics.Total.Local.Offset.y = targetSize.y / 2 - finalSize.y / 2 + finalPosition.y; }
	else if(::gpk::bit_true(control.Align, ::gpk::ALIGN_BOTTOM	))	{ controlMetrics.Total.Local.Offset.y = targetSize.y - (finalSize.y + finalPosition.y); }
	else															{}

	controlMetrics.Total	.Global							= controlMetrics.Total	.Local;
	controlMetrics.Client	.Global							= controlMetrics.Client	.Local;
	controlMetrics.Client	.Global.Offset					+= controlMetrics.Total.Local.Offset;
	if(isValidParent) {
		::gpk::SControlMetrics										& parentMetrics								= gui.ControlMetrics[control.IndexParent];
		controlMetrics.Client	.Global.Offset					+= parentMetrics.Client.Global.Offset;
		controlMetrics.Total	.Global.Offset					+= parentMetrics.Client.Global.Offset;
		return 0;
	}

	return 0;
}

static		::gpk::error_t								actualControlPaint							(::gpk::SGUI& gui, int32_t iControl, ::gpk::grid_view<::gpk::SColorBGRA>& target)					{
	ree_if((gui.Controls.size() <= uint32_t(iControl)), "Invalid control id: %u.", iControl);
	::controlUpdateMetrics(gui, iControl, target);
	::gpk::SControlMetrics										& controlMetrics							= gui.ControlMetrics[iControl];
	const ::gpk::SControl										& control									= gui.Controls[iControl];
	//::gpk::SRectangle2D<int32_t>
	//	{ controlMetrics.Total.Global.Offset	+ ::gpk::SCoord2<int32_t>{control.Border.Left, control.Border.Top}
	//	, controlMetrics.Total.Global.Size		- ::gpk::SCoord2<int32_t>{control.Border.Left + control.Border.Right, control.Border.Top + control.Border.Bottom}
	//	});
	::gpk::drawRectangle(target, {0xff, 0xff, 0xff, 0xff}, controlMetrics.Total.Global);
	::gpk::drawRectangle(target, {0x7f, 0x7f, 0x7f, 0xff}, controlMetrics.Client.Global);
	::gpk::drawRectangle(target, {0xff, 0x00, 0xFF, 0xff}, ::gpk::SRectangle2D<int32_t>
		{ controlMetrics.Total.Global.Offset	
		, ::gpk::SCoord2<int32_t>{controlMetrics.Total.Global.Size.x, control.Border.Top}
		});
	::gpk::drawRectangle(target, {0x00, 0xFF, 0x00, 0xff}, ::gpk::SRectangle2D<int32_t>
		{ controlMetrics.Total.Global.Offset + ::gpk::SCoord2<int32_t>{0, controlMetrics.Total.Global.Size.y - control.Border.Bottom}
		, ::gpk::SCoord2<int32_t>{controlMetrics.Total.Global.Size.x, control.Border.Bottom}
		});
	::gpk::drawRectangle(target, {0x00, 0x00, 0xFF, 0xff}, ::gpk::SRectangle2D<int32_t>
		{ controlMetrics.Total.Global.Offset	
		, ::gpk::SCoord2<int32_t>{control.Border.Left, controlMetrics.Total.Global.Size.y}
		});
	::gpk::drawRectangle(target, {0x00, 0x00, 0xff, 0xff}, ::gpk::SRectangle2D<int32_t>
		{ controlMetrics.Total.Global.Offset + ::gpk::SCoord2<int32_t>{controlMetrics.Total.Global.Size.x - control.Border.Right, 0}
		, ::gpk::SCoord2<int32_t>{control.Border.Bottom, controlMetrics.Total.Global.Size.y}
		});
	return 0;
}
			::gpk::error_t								gpk::controlPaintHierarchy					(::gpk::SGUI& gui, int32_t iControl, ::gpk::grid_view<::gpk::SColorBGRA>& target)					{
	ree_if((gui.Controls.size() <= uint32_t(iControl)), "Invalid control id: %u.", iControl);
	::gpk::SControlState										& controlState								= gui.ControlStates[iControl];
	ree_if(controlState.Unused, "Invalid control id: %u.", iControl);
	::actualControlPaint(gui, iControl, target);
	::gpk::array_view<int32_t>									& children									= gui.ControlChildren[iControl];
	for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild) {
		::gpk::controlPaintHierarchy(gui, children[iChild], target);
	}
	return 0;
}
