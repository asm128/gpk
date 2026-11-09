#include "gpk_array.h"
#include "gpk_stdsocket.h"

#if defined(GPK_WINDOWS)
#	include <windows.h>
#else
#	include <unistd.h>
#endif

#ifndef S3D_SERIAL_H_239846293487
#define S3D_SERIAL_H_239846293487

usns gpk;

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
			gpk::safe_closehandle(Handle);
#else
			gpk::safe_closesocket(Handle);
#endif
		}
						SSerial			() = default;
						SSerial			(const SSerial&) = delete;
		SSerial&		operator =		(const SSerial&) = delete;

	};

	::gpk::err_t	serialOpen			(::s3d::SSerial & serial, ::gpk::vcst_t portName, int baud);
	::gpk::err_t	serialClose			(::s3d::SSerial & serial);
	::gpk::err_t	serialWrite			(::s3d::SSerial & serial, ::gpk::vcu0_t data);
	::gpk::err_t	serialWrite			(::s3d::SSerial & serial, ::gpk::vcst_t data);
	::gpk::err_t	serialRead			(::s3d::SSerial & serial, ::gpk::au0_t & data, u2_t  count, u2_t  timeout);
	::gpk::err_t	serialReadUntil		(::s3d::SSerial & serial, ::gpk::au0_t & data, u0_t stopByte, u2_t  maxCount, u2_t  timeout);
	::gpk::err_t	serialRead			(::s3d::SSerial & serial, ::gpk::asc_t & data, u2_t  count, u2_t  timeout);
	::gpk::err_t	serialReadUntil		(::s3d::SSerial & serial, ::gpk::asc_t & data, sc_t stopByte, u2_t  maxCount, u2_t  timeout);
} // namespace

#endif // S3D_SERIAL_H_239846293487
