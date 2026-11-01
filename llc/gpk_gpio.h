#include "gpk_typeint.h"

#ifndef GPK_GPIO_H
#define GPK_GPIO_H

namespace gpk
{
	struct SPinOut {
		u1_t	I	= {};	// Pin/channel number for RX (In)
		u1_t	O	= {};	// Pin/channel number for TX (Out)
	};
} // namespace 

#endif // GPK_GPIO_H
