#include "llc_event.h"

#ifndef LLC_EVENT_STORAGE_H
#define LLC_EVENT_STORAGE_H

namespace llc
{
	GDEFINE_ENUM_TYPE (EVENT_STORAGE, u0_t);
	GDEFINE_ENUM_VALUE(EVENT_STORAGE, State         , 0);
	GDEFINE_ENUM_VALUE(EVENT_STORAGE, Block_load    , 1);
	GDEFINE_ENUM_VALUE(EVENT_STORAGE, Record_get    , 2);
	GDEFINE_ENUM_VALUE(EVENT_STORAGE, Range_get     , 3);
	GDEFINE_ENUM_VALUE(EVENT_STORAGE, Record_append , 4);
	GDEFINE_ENUM_VALUE(EVENT_STORAGE, Flush         , 5);
	GDEFINE_ENUM_VALUE(EVENT_STORAGE, Reload        , 6);

	tydf SEvent            <EVENT_STORAGE>	SEventStorage;
	tydf SEventView        <EVENT_STORAGE>	SEViewStorage;
	tydf FEventHandler     <EVENT_STORAGE>	FStorageEvent;
	tydf FEventHandlerConst<EVENT_STORAGE>	FStorageEventConst;
}

#endif // LLC_EVENT_STORAGE_H
