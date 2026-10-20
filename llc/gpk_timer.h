#include "gpk_typeint.h"	// for int64_t

#if !(defined(GPK_WINDOWS) || defined(GPK_CMSIS) || defined(GPK_ARDUINO))
#	include <chrono>
#endif

#ifndef GPK_TIMER_H_23627
#define GPK_TIMER_H_23627

namespace gpk	//
{
	stct STimer {
		double			LastTimeSeconds			= 0;	// This variable is set by Frame() or Reset() and holds the time difference in seconds between a Frame() call and the next.
		uint64_t		LastTimeMicroseconds	= 0;	// This variable is set by Frame() or Reset() and holds the time difference in seconds between a Frame() call and the next.

		inline			STimer					()		noexcept	{ Reset(); }	// STimer constructor. This function/method executes when we create an instance of STimer without us requiring to call the function directly. It's used to set the initial values of the struct.
		// --
		void			Reset					()		noexcept;	// Reset timer
		double			Frame					()		noexcept;	// Calculate time elapsed since the last Frame() or Reset() call.

#if !(defined(GPK_WINDOWS) || defined(GPK_ARDUINO) || defined(GPK_CMSIS))
		::std::chrono::high_resolution_clock::time_point	PrevTimeStamp		= {};
#else // (GPK_WINDOWS || GPK_ARDUINO || GPK_CMSIS)
		int64_t			PrevTimeStamp			= 0;
#	ifndef GPK_ATMEL
		int64_t			CountsPerSecond			= 0;
		double			SecondsPerCount			= 0;
		double			MicrosecondsPerCount	= 0;
#	endif // GPK_ATMEL
#endif // !(GPK_WINDOWS || GPK_ARDUINO || GPK_CMSIS)
	}; // STimer
} // namespace

#endif // GPK_TIMER_H_23627


