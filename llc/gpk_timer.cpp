#include "gpk_timer.h"

#if defined(GPK_WINDOWS)
#include <Windows.h>

static constexpr double			MICROSECOND_SCALE											= 0.000001;


void							gpk::STimer::Reset											()				noexcept				{
	QueryPerformanceFrequency( ( ::LARGE_INTEGER* )&CountsPerSecond );
	SecondsPerCount					= (1.0 / (CountsPerSecond));
	MicrosecondsPerCount			= (1.0 / (CountsPerSecond * MICROSECOND_SCALE));
	QueryPerformanceCounter( ( ::LARGE_INTEGER* )&PrevTimeStamp );
	LastTimeSeconds					= 0;
	LastTimeMicroseconds			= 0;
}

void							gpk::STimer::Frame											()				noexcept				{
	QueryPerformanceCounter( ( ::LARGE_INTEGER* ) &CurrentTimeStamp );
	LastTimeSeconds					= (CurrentTimeStamp - PrevTimeStamp) * SecondsPerCount;
	LastTimeMicroseconds			= uint64_t((CurrentTimeStamp - PrevTimeStamp) * MicrosecondsPerCount);
	PrevTimeStamp					= CurrentTimeStamp;
}

#elif defined(GPK_CMSIS)
void							gpk::STimer::Reset											()				noexcept				{
	CountsPerSecond					= osKernelGetTickFreq();
	SecondsPerCount					= (1.0 / (CountsPerSecond));
	MicrosecondsPerCount			= (1.0 / (CountsPerSecond / 1000000.0));
	PrevTimeStamp					= osKernelGetTickCount();
	LastTimeSeconds					= 0;
	LastTimeMicroseconds			= 0;
}

void							gpk::STimer::Frame											()				noexcept				{
	CurrentTimeStamp				= osKernelGetTickCount();
	LastTimeSeconds					= double	(( CurrentTimeStamp - PrevTimeStamp ) * SecondsPerCount);
	LastTimeMicroseconds			= uint64_t	(( CurrentTimeStamp - PrevTimeStamp ) / (CountsPerSecond / 1000000.0));
	PrevTimeStamp					= CurrentTimeStamp;
}
#else
void							gpk::STimer::Reset											()				noexcept				{
	//SecondsPerCount					= (1.0 / (CountsPerSecond ? CountsPerSecond : 1));
	PrevTimeStamp					= ::std::chrono::high_resolution_clock::now();
	CurrentTimeStamp				= PrevTimeStamp;
	LastTimeMicroseconds			= 0;
	LastTimeSeconds					= 0;
}

void							gpk::STimer::Frame											()				noexcept				{
	CurrentTimeStamp				= ::std::chrono::high_resolution_clock::now();
	auto								timeDifference									= CurrentTimeStamp - PrevTimeStamp;
	LastTimeMicroseconds			= (uint64_t)::std::chrono::duration_cast<std::chrono::microseconds>(timeDifference).count();
	LastTimeSeconds					= LastTimeMicroseconds / 1000000.0;
	PrevTimeStamp					= CurrentTimeStamp;
}
#endif
