#include "test_llc_suites.h"
#include "gpk_view_serialize.h"
#include "gpk_noise.h"

usns gpk;

static_assert(1 == ::gpk::uint_width_field_size<::gpk::u0_t>(), "u0_t width field");
static_assert(1 == ::gpk::uint_width_field_size<::gpk::u1_t>(), "u1_t width field");
static_assert(2 == ::gpk::uint_width_field_size<::gpk::u2_t>(), "u2_t width field");
static_assert(3 == ::gpk::uint_width_field_size<::gpk::u3_t>(), "u3_t width field");

GDEFINE_ENUM_TYPE(PACKED_UINT_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(PACKED_UINT_TEST_RESULT, OK				, 0, "All packed-uint tests passed.");
GDEFINE_ENUM_VALUED(PACKED_UINT_TEST_RESULT, DEFAULT_VALUE		, 1, "A default packed integer did not represent zero in one byte.");
GDEFINE_ENUM_VALUED(PACKED_UINT_TEST_RESULT, TAIL_WIDTH		, 2, "The packed tail width did not match the value's width band.");
GDEFINE_ENUM_VALUED(PACKED_UINT_TEST_RESULT, VALUE_WIDTH		, 3, "The serialized width did not equal one byte plus the tail width.");
GDEFINE_ENUM_VALUED(PACKED_UINT_TEST_RESULT, MULTIPLIER		, 4, "The first-byte multiplier did not contain the value's high bits.");
GDEFINE_ENUM_VALUED(PACKED_UINT_TEST_RESULT, TAIL_BASE			, 5, "The tail helper did not contain the value's low bytes.");
GDEFINE_ENUM_VALUED(PACKED_UINT_TEST_RESULT, VALUE_ROUND_TRIP	, 6, "The packed integer did not reconstruct its source value.");
GDEFINE_ENUM_VALUED(PACKED_UINT_TEST_RESULT, BYTE_VIEW			, 7, "The serialization byte view reported the wrong address or width.");
GDEFINE_ENUM_VALUED(PACKED_UINT_TEST_RESULT, LOAD_RESULT		, 8, "loadUInt() did not report the packed byte width.");
GDEFINE_ENUM_VALUED(PACKED_UINT_TEST_RESULT, LOAD_VALUE			, 9, "loadUInt() did not reconstruct the packed source value.");
GDEFINE_ENUM_VALUED(PACKED_UINT_TEST_RESULT, LOAD_CONSUMPTION	, 10, "loadUInt() did not consume exactly one packed integer.");
GDEFINE_ENUM_VALUED(PACKED_UINT_TEST_RESULT, INVALID_VALUE		, 11, "packed_uint<> accepted a value outside its representable range.");

stxp ::gpk::u3_t PACKED_RANDOM_SEED	= 0x5041434B45445549ULL;
stxp ::gpk::u2_t PACKED_RANDOM_COUNT	= 256;

tplt<tpnm T>
sttc ::gpk::err_t testDefault(::gpk::ATestError & errors) {
	cnst ::gpk::packed_uint<T> packed{};
	GPK_TEST_CHECKF(errors, PACKED_UINT_TEST_RESULT_DEFAULT_VALUE, packed.TailWidth , "%u-bit default tail width:%u, expected:0.", bcof(T), ::gpk::u2_t(packed.TailWidth));
	GPK_TEST_CHECKF(errors, PACKED_UINT_TEST_RESULT_DEFAULT_VALUE, packed.Multiplier , "%u-bit default multiplier:%u, expected:0.", bcof(T), ::gpk::u2_t(packed.Multiplier));
	GPK_TEST_CHECKF(errors, PACKED_UINT_TEST_RESULT_DEFAULT_VALUE, packed.ValueWidth() != 1 , "%u-bit default value width:%u, expected:1.", bcof(T), ::gpk::u2_t(packed.ValueWidth()));
	GPK_TEST_CHECKF(errors, PACKED_UINT_TEST_RESULT_DEFAULT_VALUE, packed.Value() , "%u-bit default value:%" GPK_FMT_U3 ", expected:0.", bcof(T), ::gpk::u3_t(packed.Value()));
	return 0;
}

tplt<tpnm T>
sttc ::gpk::err_t testInvalidValue(::gpk::ATestError & errors) {
#ifdef GPK_WINDOWS
	stxp ::gpk::u0_t WIDTH_FIELD = ::gpk::uint_width_field_size<T>();
	cnst T maximum = T(-1) >> WIDTH_FIELD;
	cnst T invalid = maximum + 1;
	GPK_TEST_CHECKF(errors, PACKED_UINT_TEST_RESULT_INVALID_VALUE, !::gpk::testThrows([&]() { cnst ::gpk::packed_uint<T> packed{invalid}; (void)packed; }) , "%u-bit packed_uint<> accepted value:%" GPK_FMT_U3 " above maximum:%" GPK_FMT_U3 "." , ::gpk::u2_t(szof(T) * 8), ::gpk::u3_t(invalid), ::gpk::u3_t(maximum) );
#else
	(void)errors;
#endif
	return 0;
}

tplt<tpnm T>
sttc ::gpk::err_t testValue(::gpk::ATestError & errors, ::gpk::u3_c source, ::gpk::u0_c expectedTailWidth, bool trace = true) {
	cnst T value = (T)source;
	cnst ::gpk::packed_uint<T> packed{value};
	::gpk::u3_c expectedMultiplier = source >> (expectedTailWidth * 8);
	::gpk::u3_c expectedTail = expectedTailWidth ? source & ((::gpk::u3_t(1) << (expectedTailWidth * 8)) - 1) : source;
	::gpk::u0_c expectedValueWidth = 1 + expectedTailWidth;
	::gpk::u2_c typeBits = szof(T) * 8;

	GPK_TEST_CHECKF(errors, PACKED_UINT_TEST_RESULT_TAIL_WIDTH, packed.TailWidth != expectedTailWidth , "%u-bit packed tail width:%u, expected:%u for value:%" GPK_FMT_U3 "." , typeBits, ::gpk::u2_t(packed.TailWidth), ::gpk::u2_t(expectedTailWidth), source);
	GPK_TEST_CHECKF(errors, PACKED_UINT_TEST_RESULT_TAIL_WIDTH, ::gpk::uint_tail_width(value) != expectedTailWidth , "%u-bit helper tail width:%u, expected:%u for value:%" GPK_FMT_U3 "." , typeBits, ::gpk::u2_t(::gpk::uint_tail_width(value)), ::gpk::u2_t(expectedTailWidth), source);
	GPK_TEST_CHECKF(errors, PACKED_UINT_TEST_RESULT_VALUE_WIDTH, packed.ValueWidth() != expectedValueWidth , "%u-bit packed value width:%u, expected:%u for value:%" GPK_FMT_U3 "." , typeBits, ::gpk::u2_t(packed.ValueWidth()), ::gpk::u2_t(expectedValueWidth), source);
	GPK_TEST_CHECKF(errors, PACKED_UINT_TEST_RESULT_VALUE_WIDTH, ::gpk::uint_value_width(value) != expectedValueWidth , "%u-bit helper value width:%u, expected:%u for value:%" GPK_FMT_U3 "." , typeBits, ::gpk::u2_t(::gpk::uint_value_width(value)), ::gpk::u2_t(expectedValueWidth), source);
	GPK_TEST_CHECKF(errors, PACKED_UINT_TEST_RESULT_MULTIPLIER, ::gpk::u3_t(packed.Multiplier) != expectedMultiplier , "%u-bit packed multiplier:%" GPK_FMT_U3 ", expected:%" GPK_FMT_U3 " for value:%" GPK_FMT_U3 "." , typeBits, ::gpk::u3_t(packed.Multiplier), expectedMultiplier, source);
	GPK_TEST_CHECKF(errors, PACKED_UINT_TEST_RESULT_MULTIPLIER, ::gpk::u3_t(::gpk::uint_tail_multiplier(value)) != expectedMultiplier , "%u-bit helper multiplier:%" GPK_FMT_U3 ", expected:%" GPK_FMT_U3 " for value:%" GPK_FMT_U3 "." , typeBits, ::gpk::u3_t(::gpk::uint_tail_multiplier(value)), expectedMultiplier, source);
	GPK_TEST_CHECKF(errors, PACKED_UINT_TEST_RESULT_TAIL_BASE, ::gpk::u3_t(::gpk::uint_tail_base(value)) != expectedTail , "%u-bit tail mismatch for value:%" GPK_FMT_U3 ". helper:%" GPK_FMT_U3 ", expected:%" GPK_FMT_U3 "." , typeBits, source, ::gpk::u3_t(::gpk::uint_tail_base(value)), expectedTail );
	GPK_TEST_CHECKF(errors, PACKED_UINT_TEST_RESULT_VALUE_ROUND_TRIP, ::gpk::u3_t(packed.Value()) != source , "%u-bit round-trip mismatch. source:%" GPK_FMT_U3 ", unpacked:%" GPK_FMT_U3 "." , typeBits, source, ::gpk::u3_t(packed.Value()) );

	cnst ::gpk::vcu0_t bytes = packed.tplt cu8<::gpk::vcu0_t>();
	GPK_TEST_REQUIREF(errors, PACKED_UINT_TEST_RESULT_BYTE_VIEW, (::gpk::uP_t)bytes.begin() != (::gpk::uP_t)&packed , "%u-bit byte view began at:%p, expected:%p for value:%" GPK_FMT_U3 "." , typeBits, bytes.begin(), &packed, source);
	GPK_TEST_REQUIREF(errors, PACKED_UINT_TEST_RESULT_BYTE_VIEW, bytes.size() > szof(packed) , "%u-bit byte view size:%u exceeds packed storage:%u for value:%" GPK_FMT_U3 "." , typeBits, bytes.size(), ::gpk::u2_t(szof(packed)), source);
	GPK_TEST_CHECKF(errors, PACKED_UINT_TEST_RESULT_BYTE_VIEW, bytes.size() != expectedValueWidth , "%u-bit byte-view mismatch for value:%" GPK_FMT_U3 ". begin:%p, expected begin:%p, size:%u, expected size:%u." , typeBits, source, bytes.begin(), &packed, bytes.size(), ::gpk::u2_t(expectedValueWidth) );

	::gpk::vcu0_t input = bytes;
	T loaded = {};
	cnst ::gpk::err_t loadResult = ::gpk::loadUInt(input, loaded);
	if(trace) {
		always_printf("PACKED %u-bit input:%" GPK_FMT_U3 "; tail width actual:%u expected:%u; multiplier actual:%" GPK_FMT_U3 " expected:%" GPK_FMT_U3 "; bytes:%u expected:%u; restored:%" GPK_FMT_U3 "; load:%i; remaining:%u expected:0.", typeBits, source, (::gpk::u2_t)packed.TailWidth, (::gpk::u2_t)expectedTailWidth, (::gpk::u3_t)packed.Multiplier, expectedMultiplier, bytes.size(), (::gpk::u2_t)expectedValueWidth, (::gpk::u3_t)loaded, loadResult, input.size());
		::gpk::testTraceBytes("Packed representation", bytes);
	}
	GPK_TEST_CHECKF(errors, PACKED_UINT_TEST_RESULT_LOAD_RESULT, loadResult != expectedValueWidth , "%u-bit load result mismatch for value:%" GPK_FMT_U3 ". result:%i, expected:%u." , typeBits, source, loadResult, ::gpk::u2_t(expectedValueWidth) );
	GPK_TEST_CHECKF(errors, PACKED_UINT_TEST_RESULT_LOAD_VALUE, ::gpk::u3_t(loaded) != source , "%u-bit loaded-value mismatch. source:%" GPK_FMT_U3 ", loaded:%" GPK_FMT_U3 "." , typeBits, source, ::gpk::u3_t(loaded) );
	GPK_TEST_CHECKF(errors, PACKED_UINT_TEST_RESULT_LOAD_CONSUMPTION, input.size() , "%u-bit load left bytes behind for value:%" GPK_FMT_U3 ". remaining:%u, expected:0." , typeBits, source, input.size() );
	return 0;
}

tplt<tpnm T>
sttc ::gpk::err_t testType(STestRunnerState & runner, ::gpk::ATestError & errors) {
	if_fail_fe(testDefault<T>(errors));
	if_fail_fe(testInvalidValue<T>(errors));

	stxp ::gpk::u0_t MULTIPLIER_BITS = 8 - ::gpk::uint_width_field_size<T>();
	::gpk::u3_t previousMaximum = 0;
	for(::gpk::u0_t tailWidth = 0; tailWidth < szof(T); ++tailWidth) {
		::gpk::u0_c valueBits = MULTIPLIER_BITS + tailWidth * 8;
		::gpk::u3_c maximum = (::gpk::u3_t(1) << valueBits) - 1;
		::gpk::u3_c minimum = tailWidth ? previousMaximum + 1 : 0;
		::gpk::u3_c sample = minimum + (maximum - minimum) / 3;
		if_fail_fe(testValue<T>(errors, minimum, tailWidth));
		if(sample != minimum && sample != maximum)
			if_fail_fe(testValue<T>(errors, sample, tailWidth));
		if(maximum != minimum)
			if_fail_fe(testValue<T>(errors, maximum, tailWidth));
		previousMaximum = maximum;
	}
	cnst ::gpk::u3_t seed = runner.testSeed(PACKED_RANDOM_SEED);
	always_printf("PACKED generated coverage: %u-bit, seed:%" GPK_FMT_U3 ", cases:%u; individual failures retain counterexamples.", (::gpk::u2_t)bcof(T), seed, PACKED_RANDOM_COUNT);
	::gpk::SPRNG random = {seed};
	stxp ::gpk::u0_t VALUE_BITS = 8 - ::gpk::uint_width_field_size<T>() + (szof(T) - 1) * 8;
	stxp ::gpk::u3_t VALUE_MASK = (::gpk::u3_t(1) << VALUE_BITS) - 1;
	for(::gpk::u2_t iRandom = 0; iRandom < PACKED_RANDOM_COUNT; ++iRandom) {
		cnst T value = T(random.Next() & VALUE_MASK);
		cnst ::gpk::u2_t failureCount = testErrorCount(errors);
		if_fail_fe(testValue<T>(errors, value, ::gpk::uint_tail_width(value), false));
		if(failureCount != testErrorCount(errors))
			error_printf("%u-bit randomized packed_uint<> case failed. seed:%" GPK_FMT_U3 ", iteration:%u, value:%" GPK_FMT_U3 "."
				, ::gpk::u2_t(szof(T) * 8), seed, iRandom, ::gpk::u3_t(value)
				);
	}
	return 0;
}

tplt<tpnm T>
sttc ::gpk::err_t testTypeLogged(STestRunnerState runner, ::gpk::ATestError & errors) {
	cnst ::gpk::u2_t checkCount = testCheckCount(errors);
	cnst ::gpk::u2_t failureCount = testErrorCount(errors);
	if_fail_fe(testType<T>(runner, errors));
	cnst ::gpk::u2_t typeFailures = testErrorCount(errors) - failureCount;
	cnst ::gpk::u2_t typeChecks = testCheckCount(errors) - checkCount;
	if(typeFailures) error_printf("%2u-bit suite completed: %u/%u checks passed, %u failed.", ::gpk::u2_t(szof(T) * 8), typeChecks - typeFailures, typeChecks, typeFailures);
	return 0;
}

::gpk::err_t test_packed_uint(STestRunnerState & runner, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	cnst ::gpk::u2_t failureCount = testErrorCount(errors);
	if_fail_fe(::gpk::testTypePack(::gpk::type_pack_unsigned{}, [&](auto type) {
		rtrn testTypeLogged<typename decltype(type)::type>(runner, errors);
		}));
	if(failureCount == testErrorCount(errors))
		always_printf("Element widths tested successfully:\n%u, %u, %u and %u bits."
			, ::gpk::u2_t(szof(::gpk::u0_t) * 8), ::gpk::u2_t(szof(::gpk::u1_t) * 8), ::gpk::u2_t(szof(::gpk::u2_t) * 8), ::gpk::u2_t(szof(::gpk::u3_t) * 8)
			);
	return 0;
}
