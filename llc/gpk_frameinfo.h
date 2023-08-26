#include "gpk_typeint.h"

#ifndef GPK_FRAMEINFO_H_23627
#define GPK_FRAMEINFO_H_23627

namespace gpk
{
#pragma pack(push, 1)
	struct SFrameSeconds {
		double					Total				;
		double					LastFrame			;
		double					LastFrameHalfSquared;
		// Helper
		void					UpdateFromTime		(double secondsToAdd)			noexcept	{
			Total					+= LastFrame = secondsToAdd;
			LastFrameHalfSquared	=  secondsToAdd * secondsToAdd * 0.5;
		}
		inline	void			UpdateFromTimeUs	(uint64_t microsecondsToAdd)	noexcept	{
			UpdateFromTime(microsecondsToAdd * .000001);
		}
	};

	struct SFrameMicroseconds {
		uint64_t				Total				;
		uint64_t				LastFrame			;
		// Helper
		inline	void			UpdateFromTime		(uint64_t microsecondsToAdd)	noexcept	{ Total += LastFrame = microsecondsToAdd; }
	};

	struct SFrameMeter {
		double					AverageFrameTime	= 0;
		uint64_t				FrameNumber			= 0;
		uint32_t				FramesPerSecond		= 0;
		uint32_t				FramesThisSecond	= 0;
		uint32_t				FrameStep			= 0;

		void					Frame				(uint64_t timeElapsedMicroseconds)	{	// Set last frame time and number.
			++FrameNumber;
			++FramesThisSecond;
			FrameStep				+= (uint32_t)timeElapsedMicroseconds;
			if(FrameStep > 1 || FramesThisSecond > 30) {
				FramesPerSecond			= uint32_t(FramesThisSecond / (FrameStep * .000001));
				FramesThisSecond		= 0;
				FrameStep				= 0;
				AverageFrameTime		= 1.0f / FramesPerSecond;
			}
		}
	};

	struct SFrameInfo {
		SFrameMicroseconds		Microseconds		= {};
		SFrameSeconds			Seconds				= {};
		SFrameMeter				FrameMeter			= {};

		void					Frame				(uint64_t timeElapsedMicroseconds)						{	// Set last frame time and number.
			Microseconds	.UpdateFromTime(timeElapsedMicroseconds);
			Seconds			.UpdateFromTimeUs(timeElapsedMicroseconds);
			FrameMeter		.Frame(timeElapsedMicroseconds);
		}
	};
#pragma pack(pop)


} // namespace

#endif // GPK_FRAMEINFO_H_23627
