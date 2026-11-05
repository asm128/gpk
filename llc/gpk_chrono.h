#include "gpk_typeint.h"
#include "gpk_parse.h"
#include "gpk_array_pod.h"

#ifndef GPK_CHRONO_H_23627
#define GPK_CHRONO_H_23627

namespace gpk
{
#if defined(GPK_ANDROID) || defined(GPK_LINUX)
	long long				timeCurrent			();
	long long				timeCurrentInMs		();
	long long				timeCurrentInUs		();
#else
	s3_t					timeCurrent			();
	s3_t					timeCurrentInMs		();
	s3_t					timeCurrentInUs		();
#endif
#pragma pack(push, 1)
	stct STimestamps {	
		u3_t				Created				= 0;
		u3_t				Loaded				= 0;
		u3_t				Modified			= 0;
		u3_t				Saved				= 0;
	};

	stct SDaylight {
		u3_t				TimeOffset		= {};
		::gpk::u2_t				OffsetMinutes	= 90;
		f2_t					ExtraRatio		= .5;
	};
#pragma pack(pop)

} // namespace

#endif // GPK_CHRONO_H_23627
