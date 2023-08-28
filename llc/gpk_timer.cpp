#include "gpk_timer.h"

#ifdef GPK_WINDOWS
#	include <Windows.h>
#elif defined(GPK_ATMEL)
#	include <Arduino.h>
#endif

stacxpr	double			MICROSECOND_SCALE		= 0.000001;

void					gpk::STimer::Reset		()				noexcept				{
	LastTimeSeconds			= double(LastTimeMicroseconds = 0);
#if defined(GPK_WINDOWS) || defined(GPK_CMSIS) || defined(GPK_ESP32)
#	if defined(GPK_WINDOWS)
	QueryPerformanceFrequency((::LARGE_INTEGER*)&CountsPerSecond);
	QueryPerformanceCounter((::LARGE_INTEGER*)&PrevTimeStamp);
#	elif defined(GPK_CMSIS)
	CountsPerSecond			= osKernelGetTickFreq();
	PrevTimeStamp			= osKernelGetTickCount();
#	endif
	SecondsPerCount			= 1.0 / CountsPerSecond;
	MicrosecondsPerCount	= 1.0 / (CountsPerSecond * MICROSECOND_SCALE);
#else
#	if defined(GPK_ATMEL) || defined(GPK_ARDUINO)
	PrevTimeStamp			= micros();
#	else
	PrevTimeStamp			= ::std::chrono::high_resolution_clock::now();
#	endif
#endif
	CurrentTimeStamp		= PrevTimeStamp;
}

double					gpk::STimer::Frame		()				noexcept				{
#if defined(GPK_WINDOWS) || defined(GPK_CMSIS) || defined(GPK_ESP32)
#	if defined(GPK_WINDOWS)
	QueryPerformanceCounter((::LARGE_INTEGER*)&CurrentTimeStamp);
#	elif defined(GPK_CMSIS)
	CurrentTimeStamp		= osKernelGetTickCount();
#	endif
	LastTimeSeconds			= (CurrentTimeStamp - PrevTimeStamp) * SecondsPerCount;
	LastTimeMicroseconds	= uint64_t((CurrentTimeStamp - PrevTimeStamp) * MicrosecondsPerCount);
	PrevTimeStamp			= CurrentTimeStamp;
	return LastTimeSeconds;
#else
#	if defined(GPK_ATMEL) || defined(GPK_ARDUINO)
		CurrentTimeStamp		= micros();
		LastTimeMicroseconds	= CurrentTimeStamp - PrevTimeStamp;
#	else
		CurrentTimeStamp		= ::std::chrono::high_resolution_clock::now();
		auto						timeDifference			= CurrentTimeStamp - PrevTimeStamp;
		LastTimeMicroseconds	= (uint64_t)::std::chrono::duration_cast<std::chrono::microseconds>(timeDifference).count();
#	endif
		return LastTimeSeconds	= LastTimeMicroseconds * MICROSECOND_SCALE;
#endif
}
