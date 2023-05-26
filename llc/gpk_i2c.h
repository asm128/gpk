#include "gpk_array.h"

#ifndef GPK_I2C_H_02394293
#define GPK_I2C_H_02394293

namespace gpk
{
#pragma pack(push, 1)
	struct SI2CDevice {
#if defined(GPK_CMSIS)
		I2C_HandleTypeDef	Handle;
#else
		void				* Handle;
#endif
		uint16_t			Address;
	};
#pragma pack(pop)

	enum I2C_MODE : int8_t
		{ I2C_MODE_TASK
		, I2C_MODE_IT
		, I2C_MODE_DMA
		};

	::gpk::error_t i2cInit (SI2CDevice & device);
	::gpk::error_t i2cRead (SI2CDevice & device, uint16_t address, uint16_t count, ::gpk::au8 & data, ::gpk::I2C_MODE mode, uint32_t timeout);
	::gpk::error_t i2cWrite(SI2CDevice & device, uint16_t address, uint16_t count, const ::gpk::vcu8 & data, ::gpk::I2C_MODE mode, uint32_t timeout);
} // namespace


#endif // GPK_I2C_H_02394293
