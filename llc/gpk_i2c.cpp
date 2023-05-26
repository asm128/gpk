#include "gpk_i2c.h"
#include "gpk_platform_error.h"

::gpk::error_t gpk::i2cInit(SI2CDevice & device) {
	(void)device;
	return 0;
}

::gpk::error_t gpk::i2cRead(SI2CDevice & device, uint16_t address, uint16_t count, ::gpk::au8 & data, ::gpk::I2C_MODE mode, uint32_t timeout) {
#if defined(GPK_ST)
	HAL_StatusTypeDef result = HAL_ERROR;
	switch(mode) {
	case ::gpk::I2C_MODE_TASK: result = HAL_I2C_Mem_Read    (device.Handle, device.Address, memAddress, memAddSize, data.begin(), data.size(), timeout);
	case ::gpk::I2C_MODE_IT  : result = HAL_I2C_Mem_Read_IT (device.Handle, device.Address, memAddress, memAddSize, data.begin(), data.size(), timeout);
	case ::gpk::I2C_MODE_DMA : result = HAL_I2C_Mem_Read_DMA(device.Handle, device.Address, memAddress, memAddSize, data.begin(), data.size(), timeout);
	default:
		error_printf("Unsupported operation mode: %i", (int32_t)mode);
	}
	return ::gpk::error_t_from_hal_status(result);
#else
	(void)device, (void)address, (void)count, (void)data, (void)mode, (void)timeout;
	return -1;
#endif
}

::gpk::error_t gpk::i2cWrite(SI2CDevice & device, uint16_t address, uint16_t count, const ::gpk::vcu8 & data, ::gpk::I2C_MODE mode, uint32_t timeout) {
#if defined(GPK_ST)
	HAL_StatusTypeDef result = HAL_ERROR;
	switch(mode) {
	case ::gpk::I2C_MODE_TASK: result = HAL_I2C_Mem_Write    (device.Handle, device.Address, memAddress, memAddSize, (uint8_t*)data.begin(), data.size(), timeout);
	case ::gpk::I2C_MODE_IT  : result = HAL_I2C_Mem_Write_IT (device.Handle, device.Address, memAddress, memAddSize, (uint8_t*)data.begin(), data.size(), timeout);
	case ::gpk::I2C_MODE_DMA : result = HAL_I2C_Mem_Write_DMA(device.Handle, device.Address, memAddress, memAddSize, (uint8_t*)data.begin(), data.size(), timeout);
	default:
	  error_printf("Unsupported operation mode: %i", (int32_t)mode);
	}
	return ::gpk::error_t_from_hal_status(result);
#else
	(void)device, (void)address, (void)count, (void)data, (void)mode, (void)timeout;
	return -1;
#endif
}
