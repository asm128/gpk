#include "gpk_event_system.h"
#include "gpk_bit.h"

#ifndef GPK_GUI_CORE_H_261120
#define GPK_GUI_CORE_H_261120

#define gpk_gui_printf verbose_printf

namespace gpk
{
	#pragma pack(push, 1)
	tydf s1_t cid_t;
	stxp cid_t CID_INVALID = cid_t(-1);
	tydf cnst cid_t ccid_t;
	tydf apod<cid_t> acid;
	tydf view<cid_t> vcid;
	tydf view<ccid_t> vccid;
	#pragma pack(pop)
}

#endif // GPK_GUI_CORE_H_261120
