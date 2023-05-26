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

	::gpk::error_t spiInit (SSPIDevice& device);
	::gpk::error_t spiRead (SSPIDevice& device, uint16_t address, uint16_t count, ::gpk::au8 & data, ::gpk::SPI_MODE mode, uint32_t timeout);
	::gpk::error_t spiWrite(SSPIDevice& device, uint16_t address, uint16_t count, const ::gpk::vcu8 & data, ::gpk::SPI_MODE mode, uint32_t timeout);
} // namespace

#endif // GPK_SPI_H_02394293
