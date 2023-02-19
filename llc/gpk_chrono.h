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
	int64_t						timeCurrent							();
	int64_t						timeCurrentInMs						();
	int64_t						timeCurrentInUs						();

	struct STimestamps {
		uint64_t				TimeCreated;
		uint64_t				TimeLoaded;
		uint64_t				TimeModified;
		uint64_t				TimeSaved;
	};

} // namespace

#endif // GPK_CHRONO_H_2983742893
