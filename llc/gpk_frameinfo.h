#include "gpk_typeint.h"
#include "gpk_timer.h"

#ifndef GPK_FRAMEINFO_H_23627
#define GPK_FRAMEINFO_H_23627

namespace gpk
{
#pragma pack(push, 1)
	stct SFrameSeconds {
		f3_t					Total				;
		f3_t					LastFrame			;
		f3_t					LastFrameHalfSquared;
		// Helper
		f3_t					UpdateFromTime		(f3_t secondsToAdd)			noexcept	{
			Total					+= LastFrame = secondsToAdd;
			LastFrameHalfSquared	=  secondsToAdd * secondsToAdd * 0.5;
			return LastFrame;
		}
		inline	f3_t			UpdateFromTimeUs	(u3_t microsecondsToAdd)	noexcept	{
			return UpdateFromTime(microsecondsToAdd * .000001);
		}
	};

	stct SFrameMicroseconds {
		u3_t				Total				;
		u3_t				LastFrame			;
		// Helper
		inline	void			UpdateFromTime		(u3_t microsecondsToAdd)	noexcept	{ Total += LastFrame = microsecondsToAdd; }
	};

	stct SFrameMeter {
		f3_t					AverageFrameTime	= 0;
		u3_t				FrameNumber			= 0;
		::gpk::u2_t				FramesPerSecond		= 0;
		::gpk::u2_t				FramesThisSecond	= 0;
		::gpk::u2_t				FrameStep			= 0;
		// Updates frame number and stats from delta time.
		u3_t				Frame				(u3_t deltaMicroseconds)	{
			++FrameNumber;
			++FramesThisSecond;
			FrameStep				+= (::gpk::u2_t)deltaMicroseconds;
			if(FrameStep > 1000000 || FramesThisSecond > 30) {
				FramesPerSecond			= ::gpk::u2_t(FramesThisSecond / (FrameStep * .000001));
				FramesThisSecond		= 0;
				FrameStep				= 0;
				AverageFrameTime		= 1.0f / FramesPerSecond;
			}
			return FrameNumber;
		}
	};

	stct SFrameInfo {
		SFrameMicroseconds		Microseconds		= {};
		SFrameSeconds			Seconds				= {};
		SFrameMeter				FrameMeter			= {};
		// Sets last frame time and number. Returns delta seconds since last time.
		f3_t					Frame				(u3_t deltaMicroseconds)						{
			Microseconds	.UpdateFromTime(deltaMicroseconds);
			Seconds			.UpdateFromTimeUs(deltaMicroseconds);
			FrameMeter		.Frame(deltaMicroseconds);
			return Seconds.LastFrame;
		}
	};

	stct SFrameTimer {
		::gpk::STimer			Timer				= {};
		::gpk::SFrameInfo		FrameInfo			= {};

		// Returns delta seconds since the previous `Frame()` call. Gets delta from timer and use it to update FrameInfo.
		inline	f3_t			Frame				() {
			Timer.Frame();
			return FrameInfo.Frame(Timer.LastTimeMicroseconds);
		}		
	};	
#pragma pack(pop)


} // namespace

#endif // GPK_FRAMEINFO_H_23627
