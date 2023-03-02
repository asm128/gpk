#include "gpk_platform.h"

#if defined(GPK_ATMEL) || defined(GPK_WINDOWS)
#	include <stdint.h>
#else
#	include <cstdint>
#endif

#ifndef GPK_CHRONO_H_2983742893
#define GPK_CHRONO_H_2983742893

namespace gpk
{
	int64_t					timeCurrent			();
	int64_t					timeCurrentInMs		();
	int64_t					timeCurrentInUs		();
#pragma pack(push, 1)
	struct STimestamps {	
		uint64_t				Created				= 0;
		uint64_t				Loaded				= 0;
		uint64_t				Modified			= 0;
		uint64_t				Saved				= 0;
	};
#pragma pack(pop)

} // namespace

#endif // GPK_CHRONO_H_2983742893
