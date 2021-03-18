#include "gpk_array.h"

#ifndef GPK_SPI_H_02394293
#define GPK_SPI_H_02394293

namespace gpk
{
	struct SSPIDevice {
		uint16_t			Address		= 0;
#if defined(GPK_CMSIS)
		SPI_HandleTypeDef	Handle	= 0;
#else
		void				* Handle	= 0;
#endif
	};

	enum SPI_MODE : int8_t
		{ SPI_MODE_TASK
		, SPI_MODE_IT
		, SPI_MODE_DMA
		};

	::gpk::error_t spiInit (SSPIDevice& device);
	::gpk::error_t spiRead (SSPIDevice& device, uint16_t address, uint16_t count, ::gpk::array_pod<ubyte_t> & data, ::gpk::SPI_MODE mode, uint32_t timeout);
	::gpk::error_t spiWrite(SSPIDevice& device, uint16_t address, uint16_t count, const ::gpk::view_const_ubyte & data, ::gpk::SPI_MODE mode, uint32_t timeout);
} // namespace


#endif // GPK_SPI_H_02394293
