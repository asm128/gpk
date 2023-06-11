#include "gpk_enum.h"

#ifndef GPK_ENGINE_EXPLORER_MENU_H
#define GPK_ENGINE_EXPLORER_MENU_H

namespace gpk
{	
	GDEFINE_ENUM_TYPE(UI_FILE_OPTIONS, uint8_t);
	GDEFINE_ENUM_VALUE(UI_FILE_OPTIONS, New , 0);
	GDEFINE_ENUM_VALUE(UI_FILE_OPTIONS, Load, 1);
	GDEFINE_ENUM_VALUE(UI_FILE_OPTIONS, Save, 2);

	GDEFINE_ENUM_TYPE(EDITOR_APP_DIALOG, uint8_t);
	GDEFINE_ENUM_VALUE(EDITOR_APP_DIALOG, Menu		, 0);
	GDEFINE_ENUM_VALUE(EDITOR_APP_DIALOG, Palette	, 1);
	GDEFINE_ENUM_VALUE(EDITOR_APP_DIALOG, Layout	, 2);
	GDEFINE_ENUM_VALUE(EDITOR_APP_DIALOG, Project	, 3);
	GDEFINE_ENUM_VALUE(EDITOR_APP_DIALOG, Settings	, 4);

	GDEFINE_ENUM_TYPE(EDITOR_APP_MENU, uint8_t);
	GDEFINE_ENUM_VALUE(EDITOR_APP_MENU, File, 0);
	GDEFINE_ENUM_VALUE(EDITOR_APP_MENU, Edit, 1);
	GDEFINE_ENUM_VALUE(EDITOR_APP_MENU, View, 2);
	GDEFINE_ENUM_VALUE(EDITOR_APP_MENU, Help, 3);

	GDEFINE_ENUM_TYPE(EDITOR_APP_FILE, uint8_t);
	GDEFINE_ENUM_VALUE(EDITOR_APP_FILE, Project	, 0);
	GDEFINE_ENUM_VALUE(EDITOR_APP_FILE, Layout	, 1);
	GDEFINE_ENUM_VALUE(EDITOR_APP_FILE, Palette	, 2);

	GDEFINE_ENUM_TYPE(EDITOR_APP_EDIT, uint8_t);
	GDEFINE_ENUM_VALUE(EDITOR_APP_EDIT, Clear	, 0);
	GDEFINE_ENUM_VALUE(EDITOR_APP_EDIT, Select	, 1);

	GDEFINE_ENUM_TYPE(EDITOR_APP_VIEW, uint8_t);
	GDEFINE_ENUM_VALUE(EDITOR_APP_VIEW, Restore	, 0);
	GDEFINE_ENUM_VALUE(EDITOR_APP_VIEW, Minimize, 1);
	GDEFINE_ENUM_VALUE(EDITOR_APP_VIEW, Maximize, 2);

	GDEFINE_ENUM_TYPE(EDITOR_APP_HELP, uint8_t);
	GDEFINE_ENUM_VALUE(EDITOR_APP_HELP, About, 0);
} // namespace

#endif // GPK_ENGINE_EXPLORER_MENU_H
