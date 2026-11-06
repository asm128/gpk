#include "s3d_serial.h"
#include "gpk_sync.h"
#include "gpk_timer.h"

#include <fcntl.h>		/* File control definitions */

#if defined(GPK_WINDOWS)
#else
#	include <termios.h>	/* POSIX terminal control definitions */
#endif

#ifndef INVALID_HANDLE_VALUE
#	define INVALID_HANDLE_VALUE -1
#endif

usns gpk;

::gpk::err_t			s3d::serialClose		(::s3d::SSerial & serial) {
#if defined(GPK_WINDOWS)
	gpk::safe_closehandle(serial.Handle);
#else
	gpk::safe_closesocket(serial.Handle);
#endif
	serial.Connected		= false;
	return 0;
}
::gpk::err_t			s3d::serialWrite		(::s3d::SSerial & serial, ::gpk::vcst_t data) { return serialWrite(serial, ::gpk::vcu0_t((u0_c*)data.begin(), data.size())); }
::gpk::err_t			s3d::serialWrite		(::s3d::SSerial & serial, ::gpk::vcu0_t data) {
	if_zero_vw(0, data.size());
	if_true_fe(false == serial.Connected);
	if_true_fe(INVALID_HANDLE_VALUE == serial.Handle);
#if !defined(GPK_WINDOWS)
	int							sentBytes				= write(serial.Handle, data.begin(), data.size());
#else
    DWORD						sentBytes				= 0;
    if (0 == WriteFile(serial.Handle, data.begin(), data.size(), &sentBytes, 0)) {
        ClearCommError(serial.Handle, &serial.Errors, &serial.Status);
		error_printf("Failed to write to serial port. Error code. 0x%x."
			"\nStatus: "
			"\nfCtsHold   : %s"
			"\nfDsrHold   : %s"
			"\nfRlsdHold  : %s"
			"\nfXoffHold  : %s"
			"\nfXoffSent  : %s"
			"\nfEof       : %s"
			"\nfTxim      : %s"
			"\ncbInQue    : %i"
			"\ncbOutQue   : %i"
			, serial.Errors
			, serial.Status.fCtsHold		? "true" : "false"
			, serial.Status.fDsrHold		? "true" : "false"
			, serial.Status.fRlsdHold		? "true" : "false"
			, serial.Status.fXoffHold		? "true" : "false"
			, serial.Status.fXoffSent		? "true" : "false"
			, serial.Status.fEof			? "true" : "false"
			, serial.Status.fTxim			? "true" : "false"
			, serial.Status.cbInQue
			, serial.Status.cbOutQue
			);
        return -1;
    }
#endif
	if_true_fef(((u2_t )sentBytes) != data.size(), "sentBytes:%i bytes written. Expected data.size():%u", sentBytes, data.size());
	return sentBytes;
}
::gpk::err_t			s3d::serialRead			(::s3d::SSerial & serial, ::gpk::asc_t & data, u2_t  count, u2_t  timeout) { return serialRead(serial, *(::gpk::au0_t*)&data, count, timeout); }
::gpk::err_t			s3d::serialRead			(::s3d::SSerial & serial, ::gpk::au0_t & data, u2_t  count, u2_t  timeout) {
	if_zero_vw(0, count);
	if_true_fe(false == serial.Connected);
	if_true_fe(INVALID_HANDLE_VALUE == serial.Handle);
	u2_t 					totalBytesRead		= 0;
	::gpk::STimer				timerTimeout;
	u3_t 					timeElapsed				= 0;
	const u3_t 				timeoutInMicroseconds	= timeout * 1000;
	do {
		timerTimeout.Frame();
		timeElapsed				+= timerTimeout.LastTimeMicroseconds;
		if(timeElapsed >= timeoutInMicroseconds)
			return totalBytesRead;
		char						byte					= {};
#if !defined(GPK_WINDOWS)
		int							rtcd					= tcdrain(serial.Handle);
		printf("Waiting until UART buffer clears: %i\n", rtcd);
		int							readBytes;
		// read a char at a time
		if_fail_fe(readBytes = read(serial.Handle, &byte, 1));
#else
		DWORD						readBytes				= (DWORD)0;
		//unsigned int				toRead					= 0;

		ClearCommError(serial.Handle, &serial.Errors, &serial.Status);
		if(0 == serial.Status.cbInQue) {
			::gpk::sleep(5);
			continue;
		}
		if_zero_fe(ReadFile(serial.Handle, &byte, 1, &readBytes, NULL));
#endif
		if(0 == readBytes) {
			::gpk::sleep(5);
			continue;
		}
		data.push_back(byte);
		++totalBytesRead;
	} while(totalBytesRead < count);
	return totalBytesRead;
}

::gpk::err_t			s3d::serialReadUntil	(::s3d::SSerial & serial, ::gpk::au0_t & data, u0_t stopByte, u2_t  maxCount, u2_t  timeout) {
	if_zero_vw(0, maxCount);
	if_true_fe(false == serial.Connected);
	if_true_fe(INVALID_HANDLE_VALUE == serial.Handle);
	u2_t 					totalBytesRead			= 0;
	::gpk::STimer				timerTimeout;
	u3_t 					timeElapsed				= 0;
	const u3_t 				timeoutInMicroseconds	= timeout * 1000;
	do {
		timerTimeout.Frame();
		timeElapsed				+= timerTimeout.LastTimeMicroseconds;
		if(timeElapsed >= timeoutInMicroseconds)
			return totalBytesRead;
		char						byte					= {};
#if !defined(GPK_WINDOWS)
		int							readBytes;
		if_fail_fe(readBytes = read(serial.Handle, &byte, 1));  // read a char at a time
#else
		DWORD						readBytes				= (DWORD)0;
		ClearCommError(serial.Handle, &serial.Errors, &serial.Status);
		if(0 == serial.Status.cbInQue) {
			::gpk::sleep(5);
			continue;
		}
		if_zero_fe(ReadFile(serial.Handle, &byte, 1, &readBytes, NULL));
#endif
		if(0 == readBytes) {
			::gpk::sleep(5);
			continue;
		}
		data.push_back(byte);
		++totalBytesRead;
	} while(totalBytesRead < maxCount && (data.size() && data[data.size() - 1] != stopByte));
	return totalBytesRead;
}

::gpk::err_t			s3d::serialOpen			(::s3d::SSerial & serial, ::gpk::vcst_t portName, int baud) {
	info_printf("Opening port '%s' at %d bps\n", portName.begin(), baud);
	serial.Connected		= false;
#ifndef GPK_WINDOWS
	gpk::safe_closesocket(serial.Handle);
	termios						toptions				= {};
	//int							fd						= open(GPK_VIEW_ARGS(portName).begin(), O_RDWR | O_NOCTTY | O_NDELAY);
	int							fd;
	if_fail_fef(fd = open(::gpk::string(portName).begin(), O_RDWR | O_NOCTTY), "Unable to open port '%.*s'", GPK_VIEW_ARGS(portName));
	if (tcgetattr(fd, &toptions) < 0) {
		 error_printf("%s", "Couldn't get term attributes.");
		 close(fd);
		 return -1;
	}
	speed_t						brate					= baud; // let you override switch below if needed
	switch(baud) {
	case 4800:	brate = B4800;	break;
	case 9600:	brate = B9600;	break;
	case 19200:  brate = B19200;  break;
	case 38400:  brate = B38400;  break;
	case 57600:  brate = B57600;  break;
	case 115200: brate = B115200; break;
	}
	cfsetispeed(&toptions, brate);
	cfsetospeed(&toptions, brate);

	// 8N1
	toptions.c_cflag		&= ~PARENB;
	toptions.c_cflag		&= ~CSTOPB;
	toptions.c_cflag		&= ~CSIZE;
	toptions.c_cflag		|= CS8;
	// no flow control
	toptions.c_cflag		&= ~CRTSCTS;

	toptions.c_cflag		|= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
	toptions.c_iflag		&= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

	toptions.c_lflag		&= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
	toptions.c_oflag		&= ~OPOST; // make raw

	// see: http://unixwiz.net/techtips/termios-vmin-vtime.html
	toptions.c_cc[VMIN]		= 1;
	toptions.c_cc[VTIME]	= 20;

	if_fail_fef(tcsetattr(fd, TCSANOW, &toptions), "'%.*s'", GPK_VIEW_ARGS(portName));
	serial.Handle			= fd;
#else
	gpk::safe_closehandle(serial.Handle);
	DWORD								dwDesiredAccess			= GENERIC_READ | GENERIC_WRITE;
	serial.Handle					= CreateFileA(portName.begin(), dwDesiredAccess, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (serial.Handle == INVALID_HANDLE_VALUE) {
		if_true_fef(GetLastError() == ERROR_FILE_NOT_FOUND, "%.*s not available.", GPK_VIEW_ARGS(portName))
		else
			error_printf("Failed to open serial port: %.*s.", GPK_VIEW_ARGS(portName));
		return -1;
	}

	DCB									dcb					= {0};

	if_zero_fef(GetCommState(serial.Handle, &dcb), "%.*s.", GPK_VIEW_ARGS(portName));
	dcb.BaudRate					= baud;
	dcb.ByteSize					= 8;
	dcb.StopBits					= ONESTOPBIT;
	dcb.Parity						= NOPARITY;
	dcb.fDtrControl					= DTR_CONTROL_ENABLE; //Setting the DTR to Control_Enable ensures that the Arduino is properly
	if_zero_fef(SetCommState(serial.Handle, &dcb), "%.*s.", GPK_VIEW_ARGS(portName));

	// Set up timeouts
	COMMTIMEOUTS						commTimeouts		= {};
	if_zero_fef(GetCommTimeouts(serial.Handle, &commTimeouts), "%.*s.", GPK_VIEW_ARGS(portName));
	commTimeouts.ReadIntervalTimeout			= 100;
	commTimeouts.ReadTotalTimeoutMultiplier		= 1;
	commTimeouts.ReadTotalTimeoutConstant		= 100;
	commTimeouts.WriteTotalTimeoutMultiplier	= 1;
	commTimeouts.WriteTotalTimeoutConstant		= 100;
	if_zero_fef(SetCommTimeouts(serial.Handle, &commTimeouts), "%.*s.", GPK_VIEW_ARGS(portName));
	PurgeComm(serial.Handle, PURGE_RXCLEAR | PURGE_TXCLEAR);
#endif
	serial.Connected				= true;
	return 0;
}

