#include "gpk_chrono.h"

#ifdef GPK_ATMEL
#	include <time.h>
#else
#	include <chrono>
#	include <ctime>
#endif

#ifdef GPK_ATMEL
gpk::s3_t	gpk::timeCurrent		()	{ return time(0); }
gpk::s3_t	gpk::timeCurrentInMs	()	{ return time(0) * 1000; }
gpk::s3_t	gpk::timeCurrentInUs	()	{ return time(0) * 1000000; }
#else
usng		::std::chrono			::time_point;
usng		::std::chrono			::system_clock;
usng		::std::chrono			::microseconds;
usng		::std::chrono			::milliseconds;
usng		::std::chrono			::duration_cast;

gpk::s3_t	gpk::timeCurrent		()	{ time_point<system_clock> nowclock = system_clock::now(); return system_clock::to_time_t(nowclock); }
gpk::s3_t	gpk::timeCurrentInUs	()	{ time_point<system_clock> nowclock = system_clock::now(); return duration_cast<microseconds>(nowclock.time_since_epoch()).count(); }
gpk::s3_t	gpk::timeCurrentInMs	()	{ time_point<system_clock> nowclock = system_clock::now(); return duration_cast<milliseconds>(nowclock.time_since_epoch()).count(); }
// auto t0 = std::chrono::high_resolution_clock::now();
// auto nanosec = t0.time_since_epoch();
// 
// std::cout << nanosec.count() << " nanoseconds since epoch" "\n";
// std::cout << nanosec.count()/(1000000000.0 *60.0 *60.0) << " hours since epoch" "\n";
#endif
