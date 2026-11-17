#ifndef SIDE_VIEWPORT_H_23627
#define SIDE_VIEWPORT_H_23627

#include "gpk_gui_viewport.h"

nmsp gpk {
	cid_t		viewportControl		(cnst SGUI & gui, cid_t idViewport, VIEWPORT_CONTROL control);
	cid_t		viewportCreate		(SGUI & gui, cid_t idParent = CID_INVALID);
	err_t		viewportFold		(SGUI & gui, cid_t idViewport, b8_t fold);
	err_t		viewportUpdate		(SGUI & gui, cnst SInput & input, cid_t idViewport);
}

#endif // SIDE_VIEWPORT_H_23627
