#include "gpk_timer.h"

#ifdef GPK_WINDOWS
#	include <Windows.h>
#elif defined(GPK_ARDUINO)
#	include <Arduino.h>
#elif defined(GPK_CMSIS)
#endif

stacxpr	double			MICROSECOND_SCALE		= 0.000001;

void					gpk::STimer::Reset		()				noexcept				{
	LastTimeSeconds			= double(LastTimeMicroseconds = 0);
#if defined(GPK_WINDOWS) || defined(GPK_CMSIS) 
#	ifdef GPK_WINDOWS
	QueryPerformanceFrequency((::LARGE_INTEGER*)&CountsPerSecond);
	QueryPerformanceCounter((::LARGE_INTEGER*)&PrevTimeStamp);
#	elif defined(GPK_CMSIS)
	CountsPerSecond			= osKernelGetTickFreq();
	PrevTimeStamp			= osKernelGetTickCount();
#	endif
	SecondsPerCount			= 1.0 / CountsPerSecond;
	MicrosecondsPerCount	= 1.0 / (CountsPerSecond * MICROSECOND_SCALE);
#else
#	ifdef GPK_ARDUINO
	PrevTimeStamp			= micros();
#	else
	PrevTimeStamp			= ::std::chrono::high_resolution_clock::now();
#	endif
#endif
}

double					gpk::STimer::Frame		()				noexcept				{
#if defined(GPK_WINDOWS) || defined(GPK_CMSIS) 
#	ifdef GPK_WINDOWS
	const int64_t	currentTimeStamp		= 0;
	QueryPerformanceCounter((::LARGE_INTEGER*)&currentTimeStamp);
#	elif defined(GPK_CMSIS)
	const int64_t	currentTimeStamp		= osKernelGetTickCount();
#	endif
	LastTimeSeconds			= (currentTimeStamp - PrevTimeStamp) * SecondsPerCount;
	LastTimeMicroseconds	= uint64_t((currentTimeStamp - PrevTimeStamp) * MicrosecondsPerCount);
	PrevTimeStamp			= currentTimeStamp;
	return LastTimeSeconds;
#else
#	ifdef GPK_ARDUINO
	const int64_t	currentTimeStamp	= (int64_t)micros();
	LastTimeMicroseconds	= currentTimeStamp - PrevTimeStamp;
#	else
	const auto		currentTimeStamp		= ::std::chrono::high_resolution_clock::now();
	LastTimeMicroseconds	= (int64_t)::std::chrono::duration_cast<std::chrono::microseconds>(currentTimeStamp - PrevTimeStamp).count();
#	endif
	PrevTimeStamp			= currentTimeStamp;
	return LastTimeSeconds	= LastTimeMicroseconds * MICROSECOND_SCALE;
#endif
}
