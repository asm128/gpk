#include "gpk_gui_control_list.h"

#ifndef GPK_CONTROL_GRID_H_23627
#define GPK_CONTROL_GRID_H_23627

nmsp gpk
{
	struct SControlGrid {
		cid_t			IdControl	= CID_INVALID;
		aobj<SControlList>	Lists		= {};

		inln	SControlList		& operator[]	(u2_t index)		nxpt	{ rtrn Lists[index]; }
		inln	cnst SControlList	& operator[]	(u2_t index) cnst	nxpt	{ rtrn Lists[index]; }
	};

	cid_t			controlGridControl		(cnst SControlGrid & grid, cnst n2u2_t & cell);
	cid_t			controlGridControl		(cnst SControlGrid & grid, u2_t index);
	err_t			controlGridInitialize	(SGUI & gui, SControlGrid & grid, cid_t parent = CID_INVALID);
	err_t			controlGridArrange
		( SGUI & gui
		, SControlGrid & grid
		, cnst n2s1_t & cellSize
		, cnst n2s1_t & spacing = {}
		, cnst n2s1_t & offset = {}
		, ALIGN align = ALIGN_TOP_LEFT
		);

	tplT	err_t	controlGridCreate
		( SGUI & gui
		, SControlGrid & grid
		, view<T> stringifiable
		, cid_t parent
		, DIRECTION direction
		, u2_t columnCount
		, cnst n2s1_t & cellSize
		, cnst n2s1_t & spacing = {}
		, cnst n2s1_t & offset = {}
		, ALIGN align = ALIGN_TOP_LEFT
		, ALIGN textAlign = ALIGN_CENTER
		) {
		if_true_fe(0 == columnCount);
		if_true_fe(0 >= cellSize.x || 0 >= cellSize.y);
		if_true_fe(0 > spacing.x || 0 > spacing.y);
		if_true_fe(direction != DIRECTION_RIGHT && direction != DIRECTION_LEFT && direction != DIRECTION_DOWN && direction != DIRECTION_UP);
		if_fail_fe(controlGridInitialize(gui, grid, parent));
		cnst n2u2_t metrics = {columnCount, (u2_t)((stringifiable.size() + columnCount - 1) / columnCount)};
		cnst bool horizontal = direction == DIRECTION_RIGHT || direction == DIRECTION_LEFT;
		cnst u2_t listCount = horizontal ? metrics.y : metrics.x;
		cnst u2_t listCapacity = horizontal ? metrics.x : metrics.y;
		if_fail_fe(grid.Lists.resize(listCount));
		for(u2_t iList = 0; iList < listCount; ++iList) {
			cnst u2_t listBegin = iList * listCapacity;
			cnst u2_t listSize = ::gpk::min(listCapacity, stringifiable.size() - listBegin);
			if_fail_fe(controlListCreate(gui, grid.Lists[iList], view<cnst T>{stringifiable.begin() + listBegin, listSize}, grid.IdControl, direction));
			for(u2_t iControl = 0; iControl < listSize; ++iControl)
				gui.Controls.Text[grid.Lists[iList].IdControls[iControl]].Align = textAlign;
		}
		rtrn controlGridArrange(gui, grid, cellSize, spacing, offset, align);
	}
}

#endif // GPK_CONTROL_GRID_H_23627
