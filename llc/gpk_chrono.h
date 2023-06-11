#include "gpk_typeint.h"

#ifndef GPK_CHRONO_H_23611
#define GPK_CHRONO_H_23611

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

#endif // GPK_CHRONO_H_23611
