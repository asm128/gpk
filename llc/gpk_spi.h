#include "gpk_bus_manager.h"
#ifdef GPK_CMSIS
#elif defined(GPK_ARDUINO)
#	include "SPI.h"
#endif // GPK_CMSIS

#ifndef GPK_SPI_H
#define GPK_SPI_H

namespace gpk
{
#pragma pack(push, 1)
	stct SSPISlave {
		u0_t				Pin					= (u0_t)-1;
	};
	stct SSPIPinout {
		u0_t				Clock				= (u0_t)-1;
		u0_t				MasterOut			= (u0_t)-1;
		u0_t				RemoteOut			= (u0_t)-1;
	};
	stct SSPIDevice {
#ifdef GPK_CMSIS
		SPI_HandleTypeDef	PlatformHandle		= 0;
#elif defined(GPK_ARDUINO)
		SPIClass			PlatformHandle		= {};
#else
		void				* PlatformHandle	= 0;
#endif // GPK_CMSIS
		apobj<SSPISlave>	Slaves				= {};
		SSPIPinout			Pinout				= {};
		u2_t				Frequency			= (u2_t)-1;
	};
#pragma pack(pop)

	err_t	spiInit	(SSPIDevice & device);
	err_t	spiLoad	(SSPIDevice & device, u0_t pin, au0_t  & data, u2_t registerIndex, u2_t registerCount, u2_t timeout, BUS_MODE mode = BUS_MODE_DMA);
	err_t	spiLoad	(SSPIDevice & device, u0_t pin, vu0_t  & data, u2_t registerIndex, u2_t registerCount, u2_t timeout, BUS_MODE mode = BUS_MODE_DMA);
	err_t	spiSave	(SSPIDevice & device, u0_t pin, vcu0_c & data, u2_t registerIndex, u2_t timeout, BUS_MODE mode = BUS_MODE_DMA);
} // namespace

#endif // GPK_SPI_H


