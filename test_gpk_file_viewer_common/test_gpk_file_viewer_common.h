#include "gpk_framework.h"
#include "gpk_dialog.h"
#include "gpk_dialog_controls.h"
#include "gpk_gui_control_list.h"

#ifndef TEST_GPK_FILE_VIEWER_COMMON_H_23627
#define TEST_GPK_FILE_VIEWER_COMMON_H_23627

GDEFINE_ENUM_TYPE (FILE_VIEWER_LAYOUT, ::gpk::u0_t);
GDEFINE_ENUM_VALUE(FILE_VIEWER_LAYOUT, FLOATING, 0);
GDEFINE_ENUM_VALUE(FILE_VIEWER_LAYOUT, SPLIT   , 1);

stct SFileViewer {
	::gpk::SWindow					& Window;
	::gpk::pobj<::gpk::SGUI>	GUIHandle	= {};
	::gpk::SGUI					& GUI;

	::gpk::pobj<::gpk::SDialog>	Dialog		= {};
	::gpk::astcid<2>			Viewports	= {};
	::gpk::astcid<2>			ScrollBars	= {};
	::gpk::cid_t				ScrollBarX	= -1;
	::gpk::SControlList			Files		= {};
	::gpk::SControlList			Lines		= {};
	::gpk::string				FileText	= {};
	::gpk::apod<::gpk::vcst_t>	FileLines	= {};
	::gpk::u2_t				FileLineWidthMax = 0;

	SFileViewer(::gpk::SWindow & window, ::gpk::pobj<::gpk::SGUI> & gui) : Window(window), GUIHandle(gui), GUI(*gui) {}
};

::gpk::err_t	fileViewerSetup		(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout, ::gpk::vcst_t rootPath = {});
::gpk::err_t	fileViewerUpdate	(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout);
::gpk::err_t	fileViewerDraw		(SFileViewer & viewer, ::gpk::g8bgra target);

#endif // TEST_GPK_FILE_VIEWER_COMMON_H_23627
