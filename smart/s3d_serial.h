#include "gpk_array.h"

#if defined(GPK_WINDOWS)
#	include <windows.h>
#else
#	include <unistd.h>
#endif

#ifndef S3D_SERIAL_H_239846293487
#define S3D_SERIAL_H_239846293487

namespace s3d
{
	struct SSerial {
#if defined(GPK_WINDOWS)
		HANDLE			Handle			= INVALID_HANDLE_VALUE;
		COMSTAT			Status			= {};
		DWORD			Errors			= {};
#else
		int				Handle			= -1;
#endif
		bool			Connected		= false;

						~SSerial		() {
#if defined(GPK_WINDOWS)
			gpk_safe_closehandle(Handle);
#else
			gpk_safe_closesocket(Handle);
#endif
		}
						SSerial			() = default;
						SSerial			(const SSerial&) = delete;
		SSerial&		operator =		(const SSerial&) = delete;

	};

	::gpk::error_t	serialOpen			(::s3d::SSerial & serial, ::gpk::vcst_t portName, int baud);
	::gpk::error_t	serialClose			(::s3d::SSerial & serial);
	::gpk::error_t	serialWrite			(::s3d::SSerial & serial, ::gpk::vcu0_t data);
	::gpk::error_t	serialRead			(::s3d::SSerial & serial, ::gpk::au0_t & data, uint32_t count, uint32_t timeout);
	::gpk::error_t	serialReadUntil		(::s3d::SSerial & serial, ::gpk::au0_t & data, char stopByte, uint32_t maxCount, uint32_t timeout);
} // namespace

#endif // S3D_SERIAL_H_239846293487
