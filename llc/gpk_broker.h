#include "gpk_array_ptr.h"

#ifndef GPK_PACKETBROKER_H
#define GPK_PACKETBROKER_H

namespace gpk
{
	stct SPacketQueue { apobj<ac> Items;};
	stct SPacketBroker {
		::gpk::apobj<SPacketQueue>	Readers;
		::gpk::apobj<SPacketQueue>	Senders;
	};
} // namespace

#endif // GPK_PACKETBROKER_H
