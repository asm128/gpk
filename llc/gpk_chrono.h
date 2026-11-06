#include "gpk_typeint.h"

#ifndef GPK_CHRONO_H_23627
#define GPK_CHRONO_H_23627

namespace gpk
{
	s3_t	timeCurrent		();
	s3_t	timeCurrentInMs	();
	s3_t	timeCurrentInUs	();
#pragma pack(push, 1)
	stct STimestamps {	
		u3_t	Make			= 0;
		u3_t	Load			= 0;
		u3_t	Save			= 0;
		u3_t	Send			= 0;
		u3_t	Read			= 0;
		u3_t	Sort			= 0;
		u3_t	Wipe			= 0;
		u3_t	Quit			= 0;
	};
	stct SDaylight {	
		u3_t	TimeOffset		= {};
		u2_t	OffsetMinutes	= 90;
		f2_t	ExtraRatio		= .5;
	};
#pragma pack(pop)

} // namespace

#endif // GPK_CHRONO_H_23627
