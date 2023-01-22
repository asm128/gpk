#include "gpk_typeint.h"

#ifndef GPK_FRAMEINFO_H_928374654
#define GPK_FRAMEINFO_H_928374654

namespace gpk
{
#pragma pack(push, 1)
	struct SFrameSeconds {
				double															Total												;
				double															LastFrame											;
				double															LastFrameHalfSquared								;
		// Helper
		inline	void															UpdateFromTime										(double secondsToAdd)						noexcept	{
			Total																	+= LastFrame											= secondsToAdd;
			LastFrameHalfSquared													=  secondsToAdd * secondsToAdd * 0.5;
		}
	};

	struct SFrameMicroseconds {
				uint64_t														Total												;
				uint64_t														LastFrame											;
		// Helper
		inline	void															UpdateFromTime										(uint64_t microsecondsToAdd)				noexcept	{ Total += LastFrame = microsecondsToAdd; }
	};

	struct SFrameInfo {
				uint64_t														FrameNumber											= 0;
				uint64_t														FramesPerSecond										= 0;
				uint64_t														FramesThisSecond									= 0;
				uint64_t														FrameStep											= 0;
				double															AverageFrameTime									= 0;
				SFrameMicroseconds												Microseconds										= {};
				SFrameSeconds													Seconds												= {};

				void															Frame												(uint64_t timeElapsedMicroseconds)						{	// Set last frame time and number.
					++FrameNumber;
					++FramesThisSecond;
					Microseconds	.UpdateFromTime(timeElapsedMicroseconds);
					Seconds			.UpdateFromTime(timeElapsedMicroseconds / 1000000.0);
					FrameStep														+= timeElapsedMicroseconds;
					if(FramesThisSecond > 30) {
						FramesPerSecond													= uint64_t(FramesThisSecond / (FrameStep * .000001));
						FramesThisSecond												= 0;
						FrameStep														= 0;
					}
					//AverageFrameTime												= FramesThisSecond ? 1.0 / FramesThisSecond : 0;
					//while(FrameStep >= 1000000) {
					//	FrameStep														-= 1000000;
					//}
				}
	};
#pragma pack(pop)


} // namespace

#endif // GPK_FRAMEINFO_H_928374654
