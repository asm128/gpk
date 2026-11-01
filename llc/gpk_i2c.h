#include "gpk_bus_manager.h"
#ifdef GPK_CMSIS
#elif defined(GPK_ARDUINO)
#	include "Wire.h"
#endif // GPK_CMSIS

#ifndef GPK_I2C_H
#define GPK_I2C_H

namespace gpk
{
	// GDEFINE_ENUM_TYPE (I2C_ERROR, s0_t);
	// GDEFINE_ENUM_VALUE(I2C_ERROR, Ok               , 0); // Success.
	// GDEFINE_ENUM_VALUE(I2C_ERROR, Too_large        , -1); // Data too long to fit in transmit buffer.
	// GDEFINE_ENUM_VALUE(I2C_ERROR, Fail_addr        , -2); // Received NACK on transmit of address.
	// GDEFINE_ENUM_VALUE(I2C_ERROR, Fail_data        , -3); // Received NACK on transmit of data.
	// GDEFINE_ENUM_VALUE(I2C_ERROR, Error            , -4); // Other error.
	// GDEFINE_ENUM_VALUE(I2C_ERROR, Timeout          , -5); // Timeout
#pragma pack(push, 1)
	stct SI2CSlave {
		u0_t				Address			= (u0_t)-1;
	};										  
	stct SI2CPinout {						  
		u0_t				Clock			= (u0_t)-1;
		u0_t				Datum			= (u0_t)-1;
	};
	stct SI2CDevice {
#ifdef GPK_CMSIS
		I2C_HandleTypeDef	PlatformHandle	= 0;
#elif defined(GPK_ARDUINO)
#	ifndef GPK_ESP32
		TwoWire				PlatformHandle	= {};
#	else
		TwoWire				PlatformHandle	= {0};
#	endif // GPK_ESP32
#endif // GPK_CMSIS
		apobj<SI2CSlave>	Slaves			= {};
		SI2CPinout			Pinout			= {};
		u2_t				Frequency		= (u2_t)-1;
	};
#pragma pack(pop)
	err_t	i2cInit	(SI2CDevice & host);	// Set clock frequency, pin modes and pin states
	// Read
	err_t	i2cLoad	(SI2CDevice & host, u0_t address, au0_t  & data, u1_t byteCountToLoad, u2_t timeout, BUS_MODE mode = BUS_MODE_DMA);
	err_t	i2cLoad	(SI2CDevice & host, u0_t address, vu0_t  & data, u1_t byteCountToLoad, u2_t timeout, BUS_MODE mode = BUS_MODE_DMA);
	// Send
	err_t	i2cSave	(SI2CDevice & host, u0_t address, vcu0_c & data, u2_t timeout, BUS_MODE mode = BUS_MODE_DMA);
} // namespace

#endif // GPK_I2C_H
