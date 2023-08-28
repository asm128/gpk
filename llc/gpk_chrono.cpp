#include "gpk_chrono.h"

#ifdef GPK_ATMEL
#	include <time.h>
#else
#	include <chrono>
#endif

//    auto t0 = std::chrono::high_resolution_clock::now();
//    auto nanosec = t0.time_since_epoch();
//
//    std::cout << nanosec.count() << " nanoseconds since epoch" "\n";
//    std::cout << nanosec.count()/(1000000000.0 *60.0 *60.0) << " hours since epoch" "\n";
namespace gpk 
{
#if defined(GPK_ATMEL)
	int64_t		timeCurrent		()	{ return time(0); }
#else
	using ::std::chrono::time_point;
	using ::std::chrono::system_clock;
	using ::std::chrono::microseconds;
	using ::std::chrono::milliseconds;
	using ::std::chrono::duration_cast;

#	if defined(GPK_ANDROID) || defined(GPK_LINUX)
	long long	timeCurrent		()	{ time_point<system_clock> nowclock = system_clock::now(); return system_clock::to_time_t(nowclock); }
	long long	timeCurrentInUs	()	{ time_point<system_clock> nowclock = system_clock::now(); return duration_cast<microseconds>(nowclock.time_since_epoch()).count(); }
	long long	timeCurrentInMs	()	{ time_point<system_clock> nowclock = system_clock::now(); return duration_cast<milliseconds>(nowclock.time_since_epoch()).count(); }
#	else
	int64_t		timeCurrent		()	{ time_point<system_clock> nowclock = system_clock::now(); return system_clock::to_time_t(nowclock); }
	int64_t		timeCurrentInUs	()	{ time_point<system_clock> nowclock = system_clock::now(); return duration_cast<microseconds>(nowclock.time_since_epoch()).count(); }
	int64_t		timeCurrentInMs	()	{ time_point<system_clock> nowclock = system_clock::now(); return duration_cast<milliseconds>(nowclock.time_since_epoch()).count(); }
#	endif
#endif
} // namespace
