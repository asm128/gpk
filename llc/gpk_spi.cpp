#include "gpk_spi.h"
#include "gpk_platform_error.h"

::gpk::error_t gpk::spiInit(SSPIDevice & device) {
	(void)device;
	return 0;
}

::gpk::error_t gpk::spiRead(SSPIDevice & device, uint16_t address, uint16_t count, ::gpk::au8 & data, ::gpk::SPI_MODE mode, uint32_t timeout) {
#if defined(GPK_ST)
	HAL_StatusTypeDef result = HAL_ERROR;
	switch(mode) {
	case SPI_MODE_TASK: result = HAL_SPI_Receive    (device.Handle, device.Address, memAddress, memAddSize, data.begin(), data.size(), timeout);
	case SPI_MODE_IT  : result = HAL_SPI_Receive_IT (device.Handle, device.Address, memAddress, memAddSize, data.begin(), data.size(), timeout);
	case SPI_MODE_DMA : result = HAL_SPI_Receive_DMA(device.Handle, device.Address, memAddress, memAddSize, data.begin(), data.size(), timeout);
	default:
		error_printf("Unsupported operation mode: %i", (int32_t)mode);
	}
	return ::gpk::error_t_from_hal_status(result);
#else
	(void)device, (void)address, (void)count, (void)data, (void)mode, (void)timeout;
	return -1;
#endif
}

::gpk::error_t gpk::spiWrite(SSPIDevice & device, uint16_t address, uint16_t count, const ::gpk::vcu8 & data, ::gpk::SPI_MODE mode, uint32_t timeout) {
#if defined(GPK_ST)
	HAL_StatusTypeDef result = HAL_ERROR;
	switch(mode) {
	case SPI_MODE_TASK: result = HAL_SPI_Transmit    (device.Handle, device.Address, memAddress, memAddSize, data.begin(), data.size(), timeout);
	case SPI_MODE_IT  : result = HAL_SPI_Transmit_IT (device.Handle, device.Address, memAddress, memAddSize, data.begin(), data.size(), timeout);
	case SPI_MODE_DMA : result = HAL_SPI_Transmit_DMA(device.Handle, device.Address, memAddress, memAddSize, data.begin(), data.size(), timeout);
	default:
	  error_printf("Unsupported operation mode: %i", (int32_t)mode);
	}
	return ::gpk::error_t_from_hal_status(result);
#else
	(void)device, (void)address, (void)count, (void)data, (void)mode, (void)timeout;
	return -1;
#endif
}
