#include "gpk_array.h"

#ifndef GPK_I2C_H_02394293
#define GPK_I2C_H_02394293

namespace gpk
{
	struct SI2CDevice {
		uint16_t			Address;
#if defined(GPK_CMSIS)
		I2C_HandleTypeDef	Handle;
#else
		void				* Handle;
#endif
	};

	enum I2C_MODE : int8_t
		{ I2C_MODE_TASK
		, I2C_MODE_IT
		, I2C_MODE_DMA
		};

	::gpk::error_t i2cInit (SI2CDevice& device);
	::gpk::error_t i2cRead (SI2CDevice& device, uint16_t address, uint16_t count, ::gpk::array_pod<ubyte_t> & data, ::gpk::I2C_MODE mode, uint32_t timeout);
	::gpk::error_t i2cWrite(SI2CDevice& device, uint16_t address, uint16_t count, const ::gpk::view_const_ubyte & data, ::gpk::I2C_MODE mode, uint32_t timeout);
} // namespace


#endif // GPK_I2C_H_02394293
