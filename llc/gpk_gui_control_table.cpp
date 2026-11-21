#include "gpk_gui_control_table.h"
#include "gpk_array.h"

static	gpk::err_t	controlInstanceReset	(::gpk::SControlTable & controlTable, ::gpk::cid_t iControl, ::gpk::vs1_t defaultColors)										{
	controlTable.States		[iControl]	= {};
	controlTable.Placement	[iControl]	= {};
	controlTable.Area		[iControl]	= {};
	controlTable.Constraints[iControl]	= {};
	controlTable.Draw		[iControl]	= {};
	controlTable.Text		[iControl]	= {};
	controlTable.Children	[iControl]	= ::gpk::vcid{};
	controlTable.Images		[iControl]	= {};
	controlTable.ImageViews	[iControl]	= {};
	controlTable.ImageTemps	[iControl]	= {};
	
	::gpk::SControlDraw			& controlDraw			= controlTable.Draw[iControl];
	for(::gpk::u2_t iColor = 0, countColors = ::gpk::min(controlDraw.Palettes.size(), defaultColors.size()); iColor < countColors; ++iColor)
		controlDraw.Palettes[iColor] = defaultColors[iColor];

	return 0;
}


::gpk::cid_t			gpk::controlCreate		(::gpk::SControlTable & controlTable, ::gpk::vs1_t defaultColors)	{
	for(::gpk::u2_t iControl = 0; iControl < controlTable.States.size(); ++iControl) {
		if(controlTable.IsUnused((::gpk::cid_t)iControl)) {
			if_fail_fef(::controlInstanceReset(controlTable, (::gpk::cid_t)iControl, defaultColors), "iControl: %i", iControl);
			return (::gpk::cid_t)iControl;
		}
	}

	::gpk::cid_t				iControl				= -1;
	if_fail_fe(iControl = (::gpk::cid_t)::gpk::resize(controlTable.States.size() + 1
		, controlTable.States
		, controlTable.Placement
		, controlTable.Area
		, controlTable.Constraints
		, controlTable.Draw
		, controlTable.Text
		, controlTable.Images
		, controlTable.ImageViews
		, controlTable.ImageTemps
		, controlTable.Children
		) - 1);
	if_fail_fe(::controlInstanceReset(controlTable, iControl, defaultColors));
	return iControl;
}
