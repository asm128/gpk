#include "gpk_chrono.h"
#include <chrono>

//    auto t0 = std::chrono::high_resolution_clock::now();        
//    auto nanosec = t0.time_since_epoch();
//
//    std::cout << nanosec.count() << " nanoseconds since epoch" "\n";
//    std::cout << nanosec.count()/(1000000000.0 *60.0 *60.0) << " hours since epoch" "\n";


int64_t											gpk::timeCurrent							()																													{
	::std::chrono::system_clock::time_point				nowclock									= std::chrono::system_clock::now();
	return std::chrono::system_clock::to_time_t(nowclock);
}

int64_t											gpk::timeCurrentInUs						()																													{
	::std::chrono::high_resolution_clock::time_point	nowclock									= std::chrono::high_resolution_clock::now();
	return ::std::chrono::duration_cast<std::chrono::microseconds>(nowclock.time_since_epoch()).count();
}

int64_t											gpk::timeCurrentInMs						()																													{
	::std::chrono::high_resolution_clock::time_point				nowclock						= std::chrono::high_resolution_clock::now();
	return ::std::chrono::duration_cast<std::chrono::milliseconds>(nowclock.time_since_epoch()).count();
}
