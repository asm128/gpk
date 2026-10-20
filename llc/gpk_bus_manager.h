#include "gpk_enum.h"
#include "gpk_aobj_pobj.h"

#ifndef GPK_BUS_MANAGER_H
#define GPK_BUS_MANAGER_H

namespace gpk
{
	GDEFINE_ENUM_TYPE (BUS_MODE, u0_t);
	GDEFINE_ENUM_VALUE(BUS_MODE, Task		, 0);
	GDEFINE_ENUM_VALUE(BUS_MODE, Interrupt	, 1);
	GDEFINE_ENUM_VALUE(BUS_MODE, DMA		, 2);

	tpl_t	stct	SBusManager	{ tdfTTCnst(_t); apobj<T> Bus; };
} // namespace

#endif // GPK_BUS_MANAGER_H


