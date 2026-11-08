#include "gpk_array_ptr.h"

#ifndef GPK_PACKETBROKER_H
#define GPK_PACKETBROKER_H

nmsp gpk
{
	stct SPacketQueue { apobj<asc_t> Items;};
	stct SPacketBroker {
		::gpk::apobj<SPacketQueue>	Readers;
		::gpk::apobj<SPacketQueue>	Senders;
	};
} // nmsp

#endif // GPK_PACKETBROKER_H
