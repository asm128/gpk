#include "gpk_timer.h"
#include <windows.h>

void							gpk::STimer::Reset											()								{
	QueryPerformanceFrequency( ( ::LARGE_INTEGER* )&CountsPerSecond );
	SecondsPerCount					= (1.0 / (CountsPerSecond));
	MicrosecondsPerCount			= (1.0 / (CountsPerSecond / 1000000.0));
	QueryPerformanceCounter( ( ::LARGE_INTEGER* )&PrevTimeStamp );		
	LastTimeSeconds					= 0;
	LastTimeMicroseconds			= 0;
}

void							gpk::STimer::Frame											()								{
	QueryPerformanceCounter( ( ::LARGE_INTEGER* ) &CurrentTimeStamp );
	LastTimeSeconds					=  (float)	(( CurrentTimeStamp - PrevTimeStamp ) * SecondsPerCount);
	LastTimeMicroseconds			=  uint64_t	(( CurrentTimeStamp - PrevTimeStamp ) / (CountsPerSecond / 1000000.0));
	PrevTimeStamp					= CurrentTimeStamp;
}
