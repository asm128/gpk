#include "gpk_chrono.h"
#include <chrono>

int64_t											gpk::timeCurrent							()																													{
	::std::chrono::system_clock::time_point				nowclock									= std::chrono::system_clock::now();
	return std::chrono::system_clock::to_time_t(nowclock);
}

int64_t											gpk::timeCurrentInMs						()																													{
	::std::chrono::system_clock::time_point				nowclock									= std::chrono::system_clock::now();
	return ::std::chrono::duration_cast<std::chrono::milliseconds>(nowclock.time_since_epoch()).count();
}