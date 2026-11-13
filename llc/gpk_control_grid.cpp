#include "gpk_control_grid.h"

static ::gpk::n2u2_t controlGridMetrics(cnst ::gpk::SControlGrid & grid) {
	if(0 == grid.Lists.size())
		rtrn {};
	::gpk::u2_t listCapacity = 0;
	for(::gpk::u2_t iList = 0; iList < grid.Lists.size(); ++iList)
		listCapacity = ::gpk::max(listCapacity, grid.Lists[iList].IdControls.size());
	cnst bool horizontal = grid.Lists[0].Orientation == ::gpk::DIRECTION_RIGHT || grid.Lists[0].Orientation == ::gpk::DIRECTION_LEFT;
	rtrn horizontal
		? ::gpk::n2u2_t{listCapacity, grid.Lists.size()}
		: ::gpk::n2u2_t{grid.Lists.size(), listCapacity}
		;
}

gpk::err_t gpk::controlGridInitialize(SGUI & gui, SControlGrid & grid, cid_t parent) {
	grid = {};
	if_fail_fe(grid.IdControl = controlCreate(gui));
	SControlPlacement & placement = gui.Controls.Placement[grid.IdControl];
	placement.Border = {};
	placement.Margin = {};
	gui.Controls.Draw[grid.IdControl].NoClient = true;
	gui.Controls.States[grid.IdControl].Mask &= ~GUI_CONTROL_FLAG_Hovered;
	if(parent != CID_INVALID)
		if_fail_fe(controlSetParent(gui, grid.IdControl, parent));
	rtrn 0;
}

gpk::cid_t gpk::controlGridControl(cnst SControlGrid & grid, cnst n2u2_t & cell) {
	cnst n2u2_t metrics = ::controlGridMetrics(grid);
	if(cell.x >= metrics.x || cell.y >= metrics.y)
		rtrn CID_INVALID;
	cnst DIRECTION direction = grid.Lists[0].Orientation;
	cnst bool horizontal = direction == DIRECTION_RIGHT || direction == DIRECTION_LEFT;
	cnst u2_t iList = horizontal ? cell.y : cell.x;
	u2_t iControl = horizontal ? cell.x : cell.y;
	if(direction == DIRECTION_LEFT)
		iControl = metrics.x - 1 - cell.x;
	else if(direction == DIRECTION_UP)
		iControl = metrics.y - 1 - cell.y;
	if(iList >= grid.Lists.size() || iControl >= grid.Lists[iList].IdControls.size())
		rtrn CID_INVALID;
	rtrn grid.Lists[iList].IdControls[iControl];
}

gpk::cid_t gpk::controlGridControl(cnst SControlGrid & grid, u2_t index) {
	for(u2_t iList = 0; iList < grid.Lists.size(); ++iList) {
		if(index < grid.Lists[iList].IdControls.size())
			rtrn grid.Lists[iList].IdControls[index];
		index -= grid.Lists[iList].IdControls.size();
	}
	rtrn CID_INVALID;
}

gpk::err_t gpk::controlGridArrange
	( SGUI & gui
	, SControlGrid & grid
	, cnst n2s1_t & cellSize
	, cnst n2s1_t & spacing
	, cnst n2s1_t & offset
	, ALIGN align
	) {
	cnst n2u2_t metrics = ::controlGridMetrics(grid);
	cnst n2s1_t gridSize =
		{ (s1_t)(metrics.x * cellSize.x + (metrics.x ? metrics.x - 1 : 0) * spacing.x)
		, (s1_t)(metrics.y * cellSize.y + (metrics.y ? metrics.y - 1 : 0) * spacing.y)
		};
	SControlPlacement & gridPlacement = gui.Controls.Placement[grid.IdControl];
	gridPlacement.Area = {offset, gridSize};
	gridPlacement.Align = align;
	if(0 == grid.Lists.size())
		rtrn controlMetricsInvalidate(gui, grid.IdControl);
	cnst DIRECTION direction = grid.Lists[0].Orientation;
	cnst bool horizontal = direction == DIRECTION_RIGHT || direction == DIRECTION_LEFT;
	for(u2_t iList = 0; iList < grid.Lists.size(); ++iList) {
		SControlList & list = grid.Lists[iList];
		SControlPlacement & listPlacement = gui.Controls.Placement[list.IdControl];
		listPlacement.Area = horizontal
			? rect2s1_t{{0, (s1_t)(iList * (cellSize.y + spacing.y))}, {gridSize.x, cellSize.y}}
			: rect2s1_t{{(s1_t)(iList * (cellSize.x + spacing.x)), 0}, {cellSize.x, gridSize.y}}
			;
		listPlacement.Align = ALIGN_TOP_LEFT;
		gui.Controls.Draw[list.IdControl].NoClient = true;
		gui.Controls.States[list.IdControl].Mask &= ~GUI_CONTROL_FLAG_Hovered;
		for(u2_t iControl = 0; iControl < list.IdControls.size(); ++iControl) {
			cnst cid_t idControl = list.IdControls[iControl];
			SControlPlacement & placement = gui.Controls.Placement[idControl];
			SControlConstraints & constraints = gui.Controls.Constraints[idControl];
			n2s1_t cell = horizontal ? n2s1_t{(s1_t)iControl, 0} : n2s1_t{0, (s1_t)iControl};
			if(direction == DIRECTION_LEFT)
				cell.x = (s1_t)(metrics.x - 1 - iControl);
			else if(direction == DIRECTION_UP)
				cell.y = (s1_t)(metrics.y - 1 - iControl);
			placement.Area =
				{ { (s1_t)(cell.x * (cellSize.x + spacing.x)), (s1_t)(cell.y * (cellSize.y + spacing.y)) }
				, cellSize
				};
			placement.Align = ALIGN_TOP_LEFT;
			constraints.AttachSizeToControl = {CID_INVALID, CID_INVALID};
			constraints.DockToControl = {CID_INVALID, CID_INVALID, CID_INVALID, CID_INVALID};
		}
	}
	rtrn controlMetricsInvalidate(gui, grid.IdControl);
}
