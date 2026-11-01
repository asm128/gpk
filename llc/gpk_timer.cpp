#include "gpk_timer.h"

#ifdef GPK_WINDOWS
#	include <Windows.h>
#elif defined(GPK_ARDUINO)
#	include <Arduino.h>
#elif defined(GPK_CMSIS)
#endif

GPK_USING_TYPEINT();

stxp	f3_t			MICROSECOND_SCALE		= 0.000001;

void					gpk::STimer::Reset		()				noexcept				{
	LastTimeSeconds			= f3_t(LastTimeMicroseconds = 0);
#if !(defined(GPK_WINDOWS) || defined(GPK_CMSIS))
#	ifdef GPK_ARDUINO
	PrevTimeStamp			= micros();
#	else // !GPK_ARDUINO
	PrevTimeStamp			= ::std::chrono::high_resolution_clock::now();
#	endif // GPK_ARDUINO
#else // (GPK_WINDOWS || GPK_CMSIS)
#	ifdef GPK_WINDOWS
	QueryPerformanceFrequency((::LARGE_INTEGER*)&CountsPerSecond);
	QueryPerformanceCounter((::LARGE_INTEGER*)&PrevTimeStamp);
#	elif defined(GPK_CMSIS)
	CountsPerSecond			= osKernelGetTickFreq();
	PrevTimeStamp			= osKernelGetTickCount();
#	endif // GPK_WINDOWS
	SecondsPerCount			= 1.0 / CountsPerSecond;
	MicrosecondsPerCount	= 1.0 / (CountsPerSecond * MICROSECOND_SCALE);
#endif // !(GPK_WINDOWS || GPK_CMSIS)
}

f3_t			gpk::STimer::Frame		()				noexcept				{
#if defined(GPK_WINDOWS) || defined(GPK_CMSIS) 
#	ifdef GPK_WINDOWS
	s3_t				currentTimeStamp		= 0;
	QueryPerformanceCounter((::LARGE_INTEGER*)&currentTimeStamp);
#	elif defined(GPK_CMSIS)
	s3_t				currentTimeStamp		= osKernelGetTickCount();
#	endif // GPK_WINDOWS
	LastTimeSeconds			= (currentTimeStamp - PrevTimeStamp) * SecondsPerCount;
	LastTimeMicroseconds	= u3_t((currentTimeStamp - PrevTimeStamp) * MicrosecondsPerCount);
	PrevTimeStamp			= currentTimeStamp;
	return LastTimeSeconds;
#else // !(GPK_WINDOWS || GPK_CMSIS) 
#	ifdef GPK_ARDUINO
	s3_t		currentTimeStamp	= (s3_t)micros();
	LastTimeMicroseconds	= currentTimeStamp - PrevTimeStamp;
#	else // !GPK_ARDUINO
	auto		currentTimeStamp		= ::std::chrono::high_resolution_clock::now();
	LastTimeMicroseconds	= (s3_t)::std::chrono::duration_cast<std::chrono::microseconds>(currentTimeStamp - PrevTimeStamp).count();
#	endif // GPK_ARDUINO
	PrevTimeStamp			= currentTimeStamp;
	return LastTimeSeconds	= LastTimeMicroseconds * MICROSECOND_SCALE;
#endif // GPK_WINDOWS || GPK_CMSIS
}
