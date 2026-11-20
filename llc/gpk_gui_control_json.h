#include "gpk_gui_control.h"
#include "gpk_json.h"

#ifndef GPK_GUI_CONTROL_JSON_H_261118
#define GPK_GUI_CONTROL_JSON_H_261118

nmsp gpk
{
	err_t	controlTableJSONLoad	(SControlTable & controls, cnst SJSONReader & input, u2_t iNode = 0);
	err_t	controlTableJSONLoad	(SControlTable & controls, vs1_t defaultColors, cnst SJSONReader & input, u2_t iNode = 0);
	err_t	controlTableJSONLoad	(SControlTable & controls, vcst_t & inputJsonString_NOT_FILENAME);
	err_t	controlTableJSONSave	(cnst SControlTable & controls, string & outputJsonString);
	err_t	controlTableJSONSave	(string & outputJsonString, cnst SControlTable & controls, vs1_t defaultColors);
	err_t	controlTableScriptRun	(SControlTable & controls, vcst_t jsonExpression);
	err_t	controlTableScriptRun	(SControlTable & controls, cid_t iControl);

} // nmsp

#endif // GPK_GUI_CONTROL_JSON_H_261118
