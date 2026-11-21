#include "gpk_typeint.h"

#ifndef GPK_GPIO_H
#define GPK_GPIO_H

nmsp gpk
{
	struct SPinOut {
		u1_t	I	= {};	// Pin/channel number for RX (In)
		u1_t	O	= {};	// Pin/channel number for TX (Out)
	};
} // nmsp 

#endif // GPK_GPIO_H
