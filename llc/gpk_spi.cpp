#include "gpk_spi.h"
#include "gpk_platform_error.h"

::gpk::err_t	gpk::spiInit	(SSPIDevice & device) {
	(void)device;
	return 0;
}
::gpk::err_t	gpk::spiLoad	(SSPIDevice & host, u0_t pin, au0_t  & data, u2_t registerIndex, u2_t registerCount, u2_t timeout, BUS_MODE mode) {
	if_fail_fe(data.resize(registerCount));
	if_fail_fe(spiLoad(host, pin, *(vu0_t*)&data, registerIndex, registerCount, timeout, mode));
	return 0;
}
::gpk::err_t	gpk::spiLoad	(SSPIDevice & host, u0_t pin, vu0_t & data, u2_t registerIndex, u2_t registerCount, u2_t timeout, BUS_MODE mode) {
#ifdef GPK_ST
	HAL_StatusTypeDef result = HAL_ERROR;
	switch(mode) {
	case SPI_MODE_TASK: result = HAL_SPI_Receive    (host.PlatformHandle, pin, registerIndex, registerCount, data.begin(), data.size(), timeout);
	case SPI_MODE_IT  : result = HAL_SPI_Receive_IT (host.PlatformHandle, pin, registerIndex, registerCount, data.begin(), data.size(), timeout);
	case SPI_MODE_DMA : result = HAL_SPI_Receive_DMA(host.PlatformHandle, pin, registerIndex, registerCount, data.begin(), data.size(), timeout);
	default:
		error_printf("Unsupported operation mode: %i", (int32_t)mode);
	}
	return ::gpk::error_t_from_hal_status(result);
#else
	(void)host, (void)pin, (void)registerIndex, (void)registerCount, (void)data, (void)mode, (void)timeout;
	return -1;
#endif
}
::gpk::err_t	gpk::spiSave		(SSPIDevice & host, u0_t pin, vcu0_c & data, u2_t registerIndex, u2_t timeout, BUS_MODE mode) {
#if defined(GPK_ST)
	HAL_StatusTypeDef result = HAL_ERROR;
	switch(mode) {
	case SPI_MODE_TASK: result = HAL_SPI_Transmit    (host.PlatformHandle, pin, registerIndex, registerCount, data.begin(), data.size(), timeout);
	case SPI_MODE_IT  : result = HAL_SPI_Transmit_IT (host.PlatformHandle, pin, registerIndex, registerCount, data.begin(), data.size(), timeout);
	case SPI_MODE_DMA : result = HAL_SPI_Transmit_DMA(host.PlatformHandle, pin, registerIndex, registerCount, data.begin(), data.size(), timeout);
	default:
	  error_printf("Unsupported operation mode: %i", (int32_t)mode);
	}
	return ::gpk::error_t_from_hal_status(result);
#else
	(void)host, (void)pin, (void)registerIndex, (void)data, (void)mode, (void)timeout;
	return -1;
#endif
}


