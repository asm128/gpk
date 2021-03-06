#ifdef GPK_ATMEL
#	include <stdint.h>
#else
#	include <cstdint>
#endif

#ifndef GPK_CHRONO_H_2983742893
#define GPK_CHRONO_H_2983742893

namespace gpk
{
	int64_t											timeCurrent							();
	int64_t											timeCurrentInMs						();
	int64_t											timeCurrentInUs						();
} // namespace

#endif // GPK_CHRONO_H_2983742893
