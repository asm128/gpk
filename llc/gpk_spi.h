#include "gpk_array.h"

#ifndef GPK_SPI_H_02394293
#define GPK_SPI_H_02394293

namespace gpk
{
#pragma pack(push, 1)
	struct SSPIDevice {
#if defined(GPK_CMSIS)
		SPI_HandleTypeDef	Handle;
#else
		void				* Handle;
#endif
		uint16_t			Address;
	};
#pragma pack(pop)

	enum SPI_MODE : int8_t
		{ SPI_MODE_TASK
		, SPI_MODE_IT
		, SPI_MODE_DMA
		};

	gpk::err_t spiInit (SSPIDevice& device);
	gpk::err_t spiRead (SSPIDevice& device, uint16_t address, uint16_t count, ::gpk::au0_t & data, ::gpk::SPI_MODE mode, uint32_t timeout);
	gpk::err_t spiWrite(SSPIDevice& device, uint16_t address, uint16_t count, ::gpk::vcu0_c & data, ::gpk::SPI_MODE mode, uint32_t timeout);
} // namespace

#endif // GPK_SPI_H_02394293
