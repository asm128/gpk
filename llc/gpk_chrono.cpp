#include "gpk_chrono.h"
#include "gpk_expression.h"
#include <time.h>

#ifndef GPK_ATMEL
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
	s3_t		timeCurrent		()	{ return time(0); }
	s3_t		timeCurrentInMs	()	{ return time(0) * 1000; }
	s3_t		timeCurrentInUs	()	{ return time(0) * 1000000; }
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
	s3_t		timeCurrent		()	{ time_point<system_clock> nowclock = system_clock::now(); return system_clock::to_time_t(nowclock); }
	s3_t		timeCurrentInUs	()	{ time_point<system_clock> nowclock = system_clock::now(); return duration_cast<microseconds>(nowclock.time_since_epoch()).count(); }
	s3_t		timeCurrentInMs	()	{ time_point<system_clock> nowclock = system_clock::now(); return duration_cast<milliseconds>(nowclock.time_since_epoch()).count(); }
#	endif
#endif
} // namespace

::gpk::err_t gpk::timeStringFormat(::gpk::vcst_t format, ::gpk::view<cnst ::gpk::SStripLiteralType> tokens, ::gpk::s3_t timeInMs, ::gpk::asc_t & output) {
	::gpk::s3_t seconds = timeInMs / 1000, milliseconds = timeInMs % 1000;
	if(milliseconds < 0) { --seconds; milliseconds += 1000; }
	cnst ::time_t time = (::time_t)seconds;
	if_true_fef((::gpk::s3_t)time != seconds, "Time is outside the platform range: %" GPK_FMT_S3 ".", seconds);
	::tm local = {};
#if defined(GPK_WINDOWS)
	if_true_fe(localtime_s(&local, &time));
#elif defined(GPK_ATMEL)
	cnst ::tm * converted = ::localtime(&time);
	if_null_fe(converted);
	local = *converted;
#else
	if_null_fe(::localtime_r(&time, &local));
#endif
	cnst ::gpk::u2_t values[] = {
		(::gpk::u2_t)((local.tm_year + 1900) % 100), (::gpk::u2_t)(local.tm_mon + 1), (::gpk::u2_t)local.tm_mday,
		(::gpk::u2_t)local.tm_hour, (::gpk::u2_t)local.tm_min, (::gpk::u2_t)local.tm_sec, (::gpk::u2_t)(milliseconds / 10)
	};
	::gpk::sc_t digits[7][2] = {};
	for(::gpk::u2_t i = 0; i < ::gpk::size(values); ++i) {
		digits[i][0] = (::gpk::sc_t)('0' + values[i] / 10);
		digits[i][1] = (::gpk::sc_t)('0' + values[i] % 10);
	}
	cnst ::gpk::keyval<::gpk::vcst_t> bindings[] = {
		{"yy", {digits[0], 2}}, {"MM", {digits[1], 2}}, {"dd", {digits[2], 2}}, {"hh", {digits[3], 2}},
		{"mm", {digits[4], 2}}, {"ss", {digits[5], 2}}, {"ff", {digits[6], 2}}
	};
	rtrn ::gpk::expressionStringFormat(format, tokens, [&](::gpk::vcst_c & key, ::gpk::vcst_t & resolved) {
		rtrn ::gpk::find(key, ::gpk::view<cnst ::gpk::keyval<::gpk::vcst_t>>{bindings}, resolved);
	}, output);
}

::gpk::err_t gpk::timeStringFormat(::gpk::vcst_t format, ::gpk::s3_t timeInMs, ::gpk::asc_t & output) {
	::gpk::apod<::gpk::SStripLiteralType> tokens;
	::gpk::SStripLiteralState state;
	if_fail_fe(::gpk::stripLiteralParse(state, tokens, format));
	rtrn ::gpk::timeStringFormat(format, tokens, timeInMs, output);
}
