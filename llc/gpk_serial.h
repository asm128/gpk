#include "gpk_array_pod.h"

#ifdef GPK_WINDOWS
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#endif

#ifndef GPK_SERIAL_H
#define GPK_SERIAL_H

namespace gpk
{
	enum SERIAL_PARITY : uint8_t
		{ SERIAL_PARITY_NONE	= 0
		, SERIAL_PARITY_ODD		= 1
		, SERIAL_PARITY_EVEN	= 2
		, SERIAL_PARITY_MARK	= 3
		, SERIAL_PARITY_SPACE	= 4
		};

	enum SERIAL_STOP_BITS : uint8_t
		{ SERIAL_STOP_BITS_ONE	= 0
		, SERIAL_STOP_BITS_ONE5	= 1
		, SERIAL_STOP_BITS_TWO	= 2
		};

	struct SSerialPlaftormDetail {
#ifdef GPK_WINDOWS
		HANDLE					Handle			= INVALID_HANDLE_VALUE;
		DCB						Params			= {sizeof(DCB)};
		SERIAL_PARITY			Parity			= {};
		COMMTIMEOUTS			Timeouts		= {};
#endif
	};

	struct SSerial {
		SSerialPlaftormDetail	PlatformDetail	= {};
		::gpk::vcc				Port			= "";

#ifdef GPK_WINDOWS
								~SSerial		() { gpk_safe_closehandle(PlatformDetail.Handle); }
#endif

		::gpk::error_t			Write			(::gpk::vcu8 dataToSend)	const {
			uint32_t					sizeSent		= (uint32_t)-1;
#ifdef GPK_WINDOWS
			ree_if(0 == WriteFile(PlatformDetail.Handle, (const void*)dataToSend.begin(), (uint32_t)dataToSend.size(), (DWORD*)&sizeSent, 0) , "Failed to write to serial port %s." , ::gpk::toString(Port).begin());
#endif
			return sizeSent;
		}

		::gpk::error_t			Read			(::gpk::au8 & dataReceived)	const {
			uint32_t					sizeRead		= (uint32_t)-1;
			gpk_necs(dataReceived.resize(1024));
			memset(&dataReceived[0], 0, dataReceived.size());
#ifdef GPK_WINDOWS
			ree_if(0 == ReadFile(PlatformDetail.Handle, &dataReceived[0], (uint32_t)dataReceived.size(), (DWORD*)&sizeRead, 0), "Failed to read from serial port %s.", ::gpk::toString(Port).begin());
#endif
			return sizeRead; 
		}

		::gpk::error_t			Open			(::gpk::vcc port, uint32_t baudRate, ::gpk::SERIAL_PARITY parity = ::gpk::SERIAL_PARITY_NONE, ::gpk::SERIAL_STOP_BITS stopBitsZeroToTwo = ::gpk::SERIAL_STOP_BITS_ONE) {
			Port					= port;
#ifdef GPK_WINDOWS
			ree_if(INVALID_HANDLE_VALUE == (PlatformDetail.Handle = CreateFile(port.begin(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0)), "Failed to open serial on %s.", ::gpk::toString(Port).begin()); 

			// Set serial port parameters
			PlatformDetail.Params	= {sizeof(PlatformDetail.Params)};
			ree_if(0 == GetCommState(PlatformDetail.Handle, &PlatformDetail.Params), "Failed to get serial port state for %s.", ::gpk::toString(Port).begin()); 

			PlatformDetail.Params.BaudRate	= baudRate;
			PlatformDetail.Params.ByteSize	= 8;
			PlatformDetail.Params.StopBits	= (BYTE)stopBitsZeroToTwo;
			PlatformDetail.Params.Parity	= (BYTE)parity;
			ree_if(0 == SetCommState(PlatformDetail.Handle, &PlatformDetail.Params), "Failed to set serial port state for %s.", ::gpk::toString(Port).begin()); 

			// Set serial port timeouts
			PlatformDetail.Timeouts.ReadIntervalTimeout			= 50;
			PlatformDetail.Timeouts.ReadTotalTimeoutConstant	= 50;
			PlatformDetail.Timeouts.ReadTotalTimeoutMultiplier	= 2;
			PlatformDetail.Timeouts.WriteTotalTimeoutConstant	= 50;
			PlatformDetail.Timeouts.WriteTotalTimeoutMultiplier	= 2;
			ree_if(0 == SetCommTimeouts(PlatformDetail.Handle, &PlatformDetail.Timeouts), "Failed to set serial port timeouts for %s.", ::gpk::toString(Port).begin()); 
#endif
			return 0;
		}
	};
} // namespace

#endif // GPK_SERIAL_H