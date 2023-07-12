#include "gpk_typeint.h"

#ifndef GPK_CHRONO_H_23627
#define GPK_CHRONO_H_23627

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

	struct SDaylight {
		uint64_t				TimeOffset		= {};
		uint32_t				OffsetMinutes	= 90;
		float					ExtraRatio		= .5;
	};
#pragma pack(pop)

} // namespace

#endif // GPK_CHRONO_H_23627
