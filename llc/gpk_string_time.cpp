#include "gpk_string_time.h"
#include "gpk_expression.h"

gpk::err_t	gpk::timeStringFormat	(vcst_t format, s3_t timeInMs, asc_t & output) {
	apod<SStripLiteralToken> tokens;
	SStripLiteralState		state;
	if_fail_fe(stripLiteralParse(state, tokens, format));
	rtrn timeStringFormat(format, tokens, timeInMs, output);
}

gpk::err_t	gpk::timeStringFormat	(vcst_t format, view<cnst SStripLiteralToken> tokens, s3_t timeInMs, asc_t & output) {
	s3_t			seconds					= timeInMs / 1000
		,			milliseconds			= timeInMs % 1000
		;
	if(milliseconds < 0) { --seconds; milliseconds += 1000; }
	cnst ::time_t	time					= (::time_t)seconds;
	if_true_fef((s3_t)time != seconds, "Time is outside the platform range: %" GPK_FMT_S3 ".", seconds);
	::tm			local					= {};
#if defined(GPK_WINDOWS)
	if_true_fe(localtime_s(&local, &time));
#elif defined(GPK_ATMEL)
	::tm			* converted;
	if_null_fe(converted = ::localtime(&time));
	local			= *converted;
#else
	if_null_fe(::localtime_r(&time, &local));
#endif
	cnst u2_t values[] = {
		(u2_t)((local.tm_year + 1900) % 100), (u2_t)(local.tm_mon + 1), (u2_t)local.tm_mday,
		(u2_t)local.tm_hour, (u2_t)local.tm_min, (u2_t)local.tm_sec, (u2_t)(milliseconds / 10)
	};
	sc_t digits[7][2] = {};
	for(u2_t i = 0; i < size(values); ++i) {
		digits[i][0] = (sc_t)('0' + values[i] / 10);
		digits[i][1] = (sc_t)('0' + values[i] % 10);
	}
	cnst keyval<vcst_t> bindings[] = {
		{"yy", {digits[0], 2}}, {"MM", {digits[1], 2}}, {"dd", {digits[2], 2}}, {"hh", {digits[3], 2}},
		{"mm", {digits[4], 2}}, {"ss", {digits[5], 2}}, {"ff", {digits[6], 2}}
	};
	rtrn expressionStringFormat(format, tokens, [&](vcst_c & key, vcst_t & resolved) {
		rtrn find(key, view<cnst keyval<vcst_t>>{bindings}, resolved);
	}, output);
}
