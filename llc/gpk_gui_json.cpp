#include "gpk_gui_json.h"
#include "gpk_gui_control_json.h"

gpk::err_t gpk::guiLoad(SGUI & gui, cnst SJSONReader & input, u2_t iNode) {
	if_fail_fe(guiSetupDefaults(gui));
	rtrn controlTableJSONLoad(gui.Controls, gui.Colors->DefaultColors, input, iNode);
}
