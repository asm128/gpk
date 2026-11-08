#include "gpk_array_pod.h"
#include "gpk_string.h"
#include "test_llc_suites.h"


GDEFINE_ENUM_TYPE(ARRAY_POD_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, OK						, 0, "All array_pod<> tests passed.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, DEFAULT_STATE			, 1, "A default array_pod<> was not an empty null range.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, INITIALIZER_CONSTRUCTION	, 2, "array_pod<> did not copy its initializer list.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, ARRAY_CONSTRUCTION		, 3, "array_pod<> did not copy its source array.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, VIEW_CONSTRUCTION		, 4, "array_pod<> did not copy its source view.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, COPY_CONSTRUCTION		, 5, "array_pod<> copy construction did not produce independent equal storage.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, MOVE_CONSTRUCTION		, 6, "array_pod<> move construction did not transfer its allocation.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, MOVE_SOURCE				, 7, "A moved-from array_pod<> did not become an empty null range.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, CONST_VIEW				, 8, "array_pod<> did not expose its counted const view.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, COPY_ASSIGNMENT			, 9, "array_pod<> copy assignment did not produce independent equal storage.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, VIEW_ASSIGNMENT			, 10, "array_pod<> did not copy its assigned view.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, ARRAY_ASSIGNMENT			, 11, "array_pod<> did not copy its assigned array.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, SELF_ASSIGNMENT			, 12, "array_pod<> changed during self-assignment.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, RESERVE_GROW				, 13, "array_pod<>::reserve() did not provide the requested capacity.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, RESERVE_PRESERVE			, 14, "array_pod<>::reserve() did not preserve its elements.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, RESERVE_STABLE			, 15, "array_pod<>::reserve() reallocated for an already satisfied request.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, RESIZE_COUNT				, 16, "array_pod<>::resize() returned or stored an incorrect count.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, RESIZE_FILL				, 17, "array_pod<>::resize(count, value) did not initialize new elements.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, RESIZE_PRESERVE			, 18, "array_pod<>::resize() did not preserve retained elements.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, RESIZE_BITS				, 19, "array_pod<>::resize_bits() did not round to the required element count.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, PUSH_BACK					, 20, "array_pod<>::push_back() did not append and report the new element index.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, APPEND_POINTER			, 21, "array_pod<>::append(pointer, count) did not append its source range.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, APPEND_ARRAY				, 22, "array_pod<>::append(array) did not append its source range.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, APPEND_VIEW				, 23, "array_pod<>::append(view) did not append its source range.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, APPEND_EMPTY				, 24, "array_pod<>::append() changed state for an empty source range.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, CLEAR						, 25, "array_pod<>::clear() did not retain allocation as an empty range.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, CLEAR_POINTER			, 26, "array_pod<>::clear_pointer() did not release and reset its storage.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, TERMINATOR				, 27, "array_pod<> did not preserve its zero-value terminator.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, POP_BACK					, 28, "array_pod<>::pop_back() did not remove its final element.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, POP_BACK_VALUE			, 29, "array_pod<>::pop_back(value) did not return and remove its final element.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, INSERT_VALUE_REALLOCATE	, 31, "array_pod<>::insert(value) failed while growing its allocation.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, INSERT_POINTER			, 32, "array_pod<>::insert(pointer, count) did not insert its source range.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, INSERT_ARRAY				, 33, "array_pod<>::insert(array) did not insert its source range.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, INSERT_VIEW				, 34, "array_pod<>::insert(view) did not insert its source range.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, INSERT_CHAIN_REALLOCATE	, 35, "array_pod<>::insert(pointer, count) failed while growing its allocation.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, ERASE						, 38, "array_pod<>::erase() did not erase the addressed element.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, APPEND_CHAINS				, 39, "array_pod<>::append(view of views) did not append every source range and report the written count.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, APPEND_STRING_ARRAY		, 40, "array_pod<>::append_string(array) did not append text and report the written count.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, APPEND_STRING_VIEW		, 41, "array_pod<>::append_string(view) did not append text and report the written count.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, APPEND_STRING_ELEMENT		, 42, "array_pod<>::append_string(element) did not append one character and report one written element.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, APPEND_STRING_FUNCTION	, 43, "array_pod<>::append_string(function) did not invoke the formatter and forward its result.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, APPEND_STRING_EMPTY_FUNCTION, 44, "array_pod<>::append_string(function) changed output for an empty formatter.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, APPEND_STRINGS				, 45, "array_pod<>::append_strings() did not append every string and report the written count.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, POP_BACK_EMPTY				, 46, "An empty array_pod<>::pop_back() did not fail without modifying its output.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, INSERT_INVALID_INDEX		, 47, "array_pod<>::insert() accepted an index beyond its logical range or changed state on failure.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, REMOVE_INVALID_INDEX		, 48, "array_pod<>::remove() accepted an index beyond its logical range or changed state on failure.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, REMOVE_UNORDERED_INVALID	, 49, "array_pod<>::remove_unordered() accepted an index beyond its logical range or changed state on failure.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, RESERVE_INVALID_COUNT		, 51, "array_pod<>::reserve() accepted a count beyond its supported range or changed state on failure.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, RESIZE_INVALID_COUNT		, 52, "array_pod<>::resize() accepted a count beyond its supported range or changed state on failure.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, APPEND_NULL_SOURCE			, 53, "array_pod<>::append() accepted a null non-empty source or changed state on failure.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, INSERT_NULL_SOURCE			, 54, "array_pod<>::insert() accepted a null non-empty source or changed state on failure.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, INSERT_EMPTY_SOURCE		, 55, "array_pod<>::insert() changed state for an empty source range.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, ALIAS_PUSH_BACK				, 56, "array_pod<>::push_back() did not preserve a source value from its own reallocated storage.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, ALIAS_RESIZE					, 57, "array_pod<>::resize(count, value) did not preserve a source value from its own reallocated storage.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, ALIAS_APPEND					, 58, "array_pod<>::append() did not preserve a source view of its own reallocated storage.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, ALIAS_INSERT_VALUE			, 59, "array_pod<>::insert(value) did not preserve a source value from its own shifted storage.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, ALIAS_INSERT_CHAIN			, 60, "array_pod<>::insert(chain) did not preserve a source view of its own shifted storage.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, ALIAS_ASSIGNMENT			, 61, "array_pod<> assignment did not safely copy an overlapping subview of itself.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, JOIN_EMPTY					, 62, "join() changed its output or reported characters for an empty field list.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, JOIN_SINGLE					, 63, "join() added a separator around a single field or reported the wrong character count.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, JOIN_MULTIPLE				, 64, "join() did not preserve empty fields, separator placement or its appended character count.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, FIND_SEQUENCE				, 65, "array_pod<>::find() did not find the requested element sequence or honor its offset.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, RFIND_SEQUENCE				, 66, "array_pod<>::rfind() did not find the last requested element sequence or honor its offset.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, JOIN_GENERIC_MULTIPLE		, 67, "Generic join() did not preserve both POD fields and their separator.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, INSERT_FRONT_VALUE			, 68, "array_pod<> did not insert a value at the front.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, INSERT_MIDDLE_VALUE		, 69, "array_pod<> did not insert a value in the middle.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, INSERT_END_VALUE			, 70, "array_pod<> did not insert a value at the end.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, REMOVE_FRONT				, 71, "array_pod<> did not remove the front element in order.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, REMOVE_MIDDLE			, 72, "array_pod<> did not remove the middle element in order.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, REMOVE_END				, 73, "array_pod<> did not remove the final element in order.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, REMOVE_UNORDERED_MIDDLE	, 74, "array_pod<> did not remove a middle element without preserving order.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, REMOVE_UNORDERED_END		, 75, "array_pod<> did not remove the final element without preserving order.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, POP_BACK_EMPTY_VALUE		, 76, "Empty array_pod<>::pop_back(value) did not fail and preserve output.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, ERASE_ONE_PAST			, 77, "array_pod<>::erase() accepted the one-past-end address.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, ERASE_NULL				, 78, "array_pod<>::erase() accepted a null address.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, ERASE_UNRELATED			, 79, "array_pod<>::erase() accepted unrelated storage.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, ERASE_MISALIGNED			, 80, "array_pod<>::erase() accepted a misaligned element address.");
GDEFINE_ENUM_VALUED(ARRAY_POD_TEST_RESULT, RESIZE_INVALID_FILLED_COUNT, 81, "array_pod<>::resize(count, value) accepted an unsupported count.");

tplt<tpnm TCall>
sttc ::gpk::err_t podExpectedFailure(TCall call) {
	cnst ::gpk::err_t result = call();
	rtrn result;
}

tplt<tpnm T>
sttc ::gpk::err_t podCheck(::gpk::ATestError & errors, ARRAY_POD_TEST_RESULT result, cnst ::gpk::apod<T> & actual, ::gpk::view<cnst T> expected) {
	GPK_TEST_CHECKF(errors, result, actual.size() != expected.size() , "Count:%u, expected:%u." , actual.size(), expected.size() );
	for(::gpk::u2_t iValue = 0; iValue < actual.size() && iValue < expected.size(); ++iValue) {
		GPK_TEST_CHECKF(errors, result, actual[iValue] != expected[iValue] , "Element:%u value:%" GPK_FMT_S3 ", expected:%" GPK_FMT_S3 "." , iValue, (::gpk::s3_t)actual[iValue], (::gpk::s3_t)expected[iValue] );
	}
	rtrn 0;
}

tplt<tpnm T>
sttc ::gpk::err_t podFailureCheck(::gpk::ATestError & errors, ARRAY_POD_TEST_RESULT result, ::gpk::err_t callResult, cnst ::gpk::apod<T> & actual, ::gpk::view<cnst T> expected, ::gpk::view<cnst T> originalStorage) {
	GPK_TEST_CHECKF(errors, result, false == ::gpk::failed(callResult) , "Result:%i, expected failure." , callResult );
	GPK_TEST_CHECKF(errors, result, actual.begin() != originalStorage.begin() , "Moved storage. begin:%p, expected:%p." , actual.begin(), originalStorage.begin() );
	rtrn podCheck(errors, result, actual, expected);
}

tplt<tpnm T>
sttc bool podTerminatorMismatch(cnst ::gpk::apod<T> & value) {
	rtrn value.begin() && value.begin()[value.size()] != T{};
}

tplt<tpnm T>
sttc ::gpk::err_t testPodConstruction(::gpk::ATestError & errors) {
	::gpk::apod<T> empty;
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_DEFAULT_STATE, empty.size() , "Default count:%u, expected:0.", empty.size() );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_DEFAULT_STATE, empty.begin() , "Default begin:%p, expected:null.", empty.begin() );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_DEFAULT_STATE, empty.end() , "Default end:%p, expected:null.", empty.end() );

	T expected[] = {T(1), T(2), T(3)};
	::gpk::apod<T> initialized = {T(1), T(2), T(3)};
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_INITIALIZER_CONSTRUCTION, initialized, {expected}));
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(initialized) , "initializer terminator mismatch. size:%u, terminator:%" GPK_FMT_S3 "." , initialized.size(), (::gpk::s3_t)initialized.begin()[initialized.size()] );

	::gpk::apod<T> fromArray{expected};
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_ARRAY_CONSTRUCTION, fromArray, {expected}));
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_ARRAY_CONSTRUCTION, fromArray.begin() == expected , "Array construction aliases source. source:%p, copy:%p.", expected, fromArray.begin() );
	::gpk::view<cnst T> sourceView{expected};
	::gpk::apod<T> fromView{sourceView};
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_VIEW_CONSTRUCTION, fromView, {expected}));
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_VIEW_CONSTRUCTION, fromView.begin() == expected , "View construction aliases source. source:%p, copy:%p.", sourceView.begin(), fromView.begin() );

	::gpk::apod<T> copied{initialized};
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_COPY_CONSTRUCTION, copied, {expected}));
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_COPY_CONSTRUCTION, copied.begin() == initialized.begin() , "Copy construction aliases source. source:%p, copy:%p.", initialized.begin(), copied.begin() );
	if(copied.size() && initialized.size()) {
		copied[0] = T(9);
		GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_COPY_CONSTRUCTION, initialized[0] != T(1) , "Copied storage was not independent. source[0]:%" GPK_FMT_S3 ", expected:1." , (::gpk::s3_t)initialized[0] );
	}

	cnst ::gpk::view<T> movedStorage = initialized;
	::gpk::apod<T> moved{::std::move(initialized)};
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_MOVE_CONSTRUCTION, moved, {expected}));
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_MOVE_CONSTRUCTION, moved.begin() != movedStorage.begin() , "Move did not transfer storage. old:%p, moved:%p.", movedStorage.begin(), moved.begin() );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_MOVE_SOURCE, initialized.size() , "Moved source count:%u, expected:0.", initialized.size() );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_MOVE_SOURCE, initialized.begin() , "Moved source begin:%p, expected:null.", initialized.begin() );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_MOVE_SOURCE, initialized.end() , "Moved source end:%p, expected:null.", initialized.end() );

	cnst ::gpk::apod<T> & constMoved = moved;
	::gpk::view<cnst T> constView = constMoved;
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_CONST_VIEW, constView.begin() != moved.begin() , "Const view begin:%p, expected:%p.", constView.begin(), moved.begin() );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_CONST_VIEW, constView.size() != moved.size() , "Const view count:%u, expected:%u.", constView.size(), moved.size() );
	rtrn 0;
}

tplt<tpnm T>
sttc ::gpk::err_t testPodAssignment(::gpk::ATestError & errors) {
	T firstExpected[]	= {T(1), T(2), T(3)};
	T secondExpected[]	= {T(4), T(5)};
	::gpk::apod<T> source{firstExpected};
	::gpk::apod<T> target{T(9)};
	target = source;
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_COPY_ASSIGNMENT, target, {firstExpected}));
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_COPY_ASSIGNMENT, target.begin() == source.begin() , "Copy assignment aliases source. source:%p, target:%p.", source.begin(), target.begin() );
	if(target.size() && source.size()) {
		target[0] = T(8);
		GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_COPY_ASSIGNMENT, source[0] != T(1) , "Assigned storage was not independent. source[0]:%" GPK_FMT_S3 ", expected:1." , (::gpk::s3_t)source[0] );
	}

	::gpk::view<cnst T> secondView{secondExpected};
	target = secondView;
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_VIEW_ASSIGNMENT, target, {secondExpected}));
	target = firstExpected;
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_ARRAY_ASSIGNMENT, target, {firstExpected}));
	target = target;
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_SELF_ASSIGNMENT, target, {firstExpected}));
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(target) , "assignment terminator mismatch. size:%u, terminator:%" GPK_FMT_S3 "." , target.size(), (::gpk::s3_t)target.begin()[target.size()] );
	rtrn 0;
}

tplt<tpnm T>
sttc ::gpk::err_t testPodReserveResize(::gpk::ATestError & errors) {
	T expected[] = {T(1), T(2), T(3)};
	::gpk::apod<T> values{expected};
	cnst ::gpk::err_t capacity = values.reserve(32);
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_RESERVE_GROW, capacity < 32 , "reserve capacity mismatch. actual:%i, requested:32." , capacity );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_RESERVE_PRESERVE, values, {expected}));
	cnst ::gpk::view<T> reservedStorage = values;
	cnst ::gpk::err_t stableCapacity = values.reserve(16);
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_RESERVE_STABLE, stableCapacity != capacity , "Satisfied reserve capacity:%i, expected:%i.", stableCapacity, capacity );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_RESERVE_STABLE, values.begin() != reservedStorage.begin() , "Satisfied reserve moved storage. address:%p, expected:%p.", values.begin(), reservedStorage.begin() );

	::gpk::err_t result = values.resize(5, T(9));
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_RESIZE_COUNT, result != 5 , "Filled resize result:%i, expected:5.", result );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_RESIZE_COUNT, values.size() != 5 , "Filled resize count:%u, expected:5.", values.size() );
	for(::gpk::u2_t iValue = 0; iValue < values.size() && iValue < 3; ++iValue) {
		GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_RESIZE_PRESERVE, values[iValue] != expected[iValue] , "Growth changed element:%u value:%" GPK_FMT_S3 ", expected:%" GPK_FMT_S3 "." , iValue, (::gpk::s3_t)values[iValue], (::gpk::s3_t)expected[iValue] );
	}
	for(::gpk::u2_t iValue = 3; iValue < values.size() && iValue < 5; ++iValue) {
		GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_RESIZE_FILL, values[iValue] != T(9) , "Growth fill element:%u value:%" GPK_FMT_S3 ", expected:9." , iValue, (::gpk::s3_t)values[iValue] );
	}
	result = values.resize(2);
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_RESIZE_COUNT, result != 2 , "Shrinking resize result:%i, expected:2.", result );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_RESIZE_COUNT, values.size() != 2 , "Shrinking resize count:%u, expected:2.", values.size() );
	T expectedShrink[] = {T(1), T(2)};
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_RESIZE_PRESERVE, values, {expectedShrink}));
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(values) , "resize terminator mismatch. size:%u, terminator:%" GPK_FMT_S3 "." , values.size(), (::gpk::s3_t)values.begin()[values.size()] );

	::gpk::apod<T> bits;
	stxp ::gpk::u2_t elementBits = szof(T) * 8U;
	result = bits.resize_bits(0);
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_RESIZE_BITS, result , "Zero-bit resize result:%i, expected:0.", result );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_RESIZE_BITS, bits.size() , "Zero-bit resize count:%u, expected:0.", bits.size() );
	result = bits.resize_bits(1);
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_RESIZE_BITS, result != 1 , "One-bit resize result:%i, expected:1.", result );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_RESIZE_BITS, bits.size() != 1 , "One-bit resize count:%u, expected:1.", bits.size() );
	result = bits.resize_bits(elementBits);
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_RESIZE_BITS, result != 1 , "Exact-element resize result:%i, expected:1.", result );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_RESIZE_BITS, bits.size() != 1 , "Exact-element resize count:%u, expected:1.", bits.size() );
	result = bits.resize_bits(elementBits + 1);
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_RESIZE_BITS, result != 2 , "Rounded-bit resize result:%i, expected:2.", result );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_RESIZE_BITS, bits.size() != 2 , "Rounded-bit resize count:%u, expected:2.", bits.size() );
	rtrn 0;
}

tplt<tpnm T>
sttc ::gpk::err_t testPodAppend(::gpk::ATestError & errors) {
	::gpk::apod<T> values = {T(1), T(2)};
	::gpk::err_t result = values.push_back(T(3));
	T expectedPush[] = {T(1), T(2), T(3)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_PUSH_BACK, result != 2 , "Push-back index:%i, expected:2.", result );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_PUSH_BACK, values, {expectedPush}));

	T pointerTail[] = {T(4), T(5)};
	result = values.append(pointerTail, 2);
	T expectedPointer[] = {T(1), T(2), T(3), T(4), T(5)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_APPEND_POINTER, result != 3 , "Pointer append index:%i, expected:3.", result );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_APPEND_POINTER, values, {expectedPointer}));

	T arrayTail[] = {T(6), T(7)};
	result = values.append(arrayTail);
	T expectedArray[] = {T(1), T(2), T(3), T(4), T(5), T(6), T(7)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_APPEND_ARRAY, result != 5 , "Array append index:%i, expected:5.", result );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_APPEND_ARRAY, values, {expectedArray}));

	T viewTail[] = {T(8), T(9)};
	::gpk::view<cnst T> tailView{viewTail};
	result = values.append(tailView);
	T expectedView[] = {T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_APPEND_VIEW, result != 7 , "View append index:%i, expected:7.", result );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_APPEND_VIEW, values, {expectedView}));
	cnst ::gpk::view<T> storageBeforeEmpty = values;
	result = values.append(nullptr, 0);
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_APPEND_EMPTY, result != 9 , "Empty append index:%i, expected:9.", result );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_APPEND_EMPTY, values.size() != 9 , "Empty append count:%u, expected:9.", values.size() );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_APPEND_EMPTY, values.begin() != storageBeforeEmpty.begin() , "Empty append moved storage. address:%p, expected:%p.", values.begin(), storageBeforeEmpty.begin() );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(values) , "append terminator mismatch. size:%u, terminator:%" GPK_FMT_S3 "." , values.size(), (::gpk::s3_t)values.begin()[values.size()] );

	T firstChain[] = {T(1), T(2)};
	T secondChain[] = {T(3), T(4), T(5)};
	::gpk::view<cnst T> chains[] = {{firstChain}, {}, {secondChain}};
	::gpk::apod<T> chained = {T(9)};
	result = chained.append(::gpk::view<cnst ::gpk::view<cnst T>>{chains});
	T expectedChains[] = {T(9), T(1), T(2), T(3), T(4), T(5)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_APPEND_CHAINS, result != 5 , "Chained append written:%i, expected:5.", result );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_APPEND_CHAINS, chained, {expectedChains}));
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(chained) , "chained append terminator mismatch. size:%u, terminator:%" GPK_FMT_S3 "." , chained.size(), (::gpk::s3_t)chained.begin()[chained.size()] );

		T joinLeft[] = {T(1), T(2)};
		T joinRight[] = {T(3)};
		cnst ::gpk::view<cnst T> joinFields[] = {{joinLeft}, {joinRight}};
		::gpk::apod<T> joined;
		result = ::gpk::join(joined, T(0), ::gpk::view<cnst ::gpk::view<cnst T>>{joinFields});
		T expectedJoin[] = {T(1), T(2), T(0), T(3)};
		GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_JOIN_GENERIC_MULTIPLE, result != 4 , "Generic join written:%i, expected:4.", result );
		if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_JOIN_GENERIC_MULTIPLE, joined, {expectedJoin}));
	rtrn 0;
}

tplt<tpnm T>
sttc ::gpk::err_t testPodPopBack(::gpk::ATestError & errors) {
	::gpk::apod<T> values = {T(1), T(2), T(3)};
	T removed = {};
	::gpk::err_t result = values.pop_back(removed);
	T expectedValue[] = {T(1), T(2)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_POP_BACK_VALUE, result != 2 , "Pop-back-with-value result:%i, expected:2.", result );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_POP_BACK_VALUE, removed != T(3) , "Pop-back removed:%" GPK_FMT_S3 ", expected:3.", (::gpk::s3_t)removed );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_POP_BACK_VALUE, values, {expectedValue}));
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(values) , "pop_back(value) terminator mismatch. size:%u, terminator:%" GPK_FMT_S3 "." , values.size(), (::gpk::s3_t)values.begin()[values.size()] );

	result = values.pop_back();
	T expected[] = {T(1)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_POP_BACK, result != 1 , "Pop-back result:%i, expected:1.", result );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_POP_BACK, values, {expected}));
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(values) , "pop_back terminator mismatch. size:%u, terminator:%" GPK_FMT_S3 "." , values.size(), (::gpk::s3_t)values.begin()[values.size()] );
	rtrn 0;
}

tplt<tpnm T>
sttc ::gpk::err_t testPodInsert(::gpk::ATestError & errors) {
	::gpk::apod<T> values = {T(2), T(4)};
	values.reserve(16);
	cnst ::gpk::view<T> reservedStorage = values;
	::gpk::err_t result = values.insert(0, T(1));
	T expectedFront[] = {T(1), T(2), T(4)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_INSERT_FRONT_VALUE, result != 3 , "Front insertion result:%i, expected:3.", result );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_INSERT_FRONT_VALUE, values.begin() != reservedStorage.begin() , "Front insertion moved storage. begin:%p, expected:%p.", values.begin(), reservedStorage.begin() );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_INSERT_FRONT_VALUE, values, {expectedFront}));
	result = values.insert(2, T(3));
	T expectedMiddle[] = {T(1), T(2), T(3), T(4)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_INSERT_MIDDLE_VALUE, result != 4 , "Middle insertion result:%i, expected:4.", result );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_INSERT_MIDDLE_VALUE, values.begin() != reservedStorage.begin() , "Middle insertion moved storage. begin:%p, expected:%p.", values.begin(), reservedStorage.begin() );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_INSERT_MIDDLE_VALUE, values, {expectedMiddle}));
	result = values.insert(values.size(), T(5));
	T expectedEnd[] = {T(1), T(2), T(3), T(4), T(5)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_INSERT_END_VALUE, result != 5 , "End insertion result:%i, expected:5.", result );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_INSERT_END_VALUE, values.begin() != reservedStorage.begin() , "End insertion moved storage. begin:%p, expected:%p.", values.begin(), reservedStorage.begin() );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_INSERT_END_VALUE, values, {expectedEnd}));

	::gpk::apod<T> reallocated = {T(1), T(3)};
	cnst ::gpk::u2_t capacity = reallocated.reserve(reallocated.size());
	if_fail_fe(reallocated.resize(capacity, T(8)));
	cnst ::gpk::view<T> previousStorage = reallocated;
	result = reallocated.insert(1, T(2));
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_INSERT_VALUE_REALLOCATE, result != (::gpk::err_t)capacity + 1 , "Reallocating value insertion result:%i, expected:%u.", result, capacity + 1 );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_INSERT_VALUE_REALLOCATE, reallocated.size() != capacity + 1 , "Reallocating value insertion count:%u, expected:%u.", reallocated.size(), capacity + 1 );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_INSERT_VALUE_REALLOCATE, reallocated.begin() == previousStorage.begin() , "Reallocating value insertion retained address:%p.", reallocated.begin() );
	T expectedReallocated[] = {T(1), T(2), T(3)};
	for(::gpk::u2_t iValue = 0; iValue < reallocated.size() && iValue < ::gpk::size(expectedReallocated); ++iValue) {
		GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_INSERT_VALUE_REALLOCATE, reallocated[iValue] != expectedReallocated[iValue] , "Reallocating value insertion element:%u value:%" GPK_FMT_S3 ", expected:%" GPK_FMT_S3 "." , iValue, (::gpk::s3_t)reallocated[iValue], (::gpk::s3_t)expectedReallocated[iValue] );
	}
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(reallocated) , "reallocating value insertion terminator mismatch. size:%u, terminator:%" GPK_FMT_S3 "." , reallocated.size(), (::gpk::s3_t)reallocated.begin()[reallocated.size()] );

	::gpk::apod<T> chains = {T(4), T(8)};
	chains.reserve(32);
	T pointerValues[] = {T(1), T(2), T(3)};
	result = chains.insert(0, pointerValues, 3);
	T expectedPointer[] = {T(1), T(2), T(3), T(4), T(8)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_INSERT_POINTER, result != 5 , "Pointer insertion result:%i, expected:5.", result );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_INSERT_POINTER, chains, {expectedPointer}));
	T arrayValues[] = {T(5), T(6)};
	result = chains.insert(4, arrayValues);
	T expectedArray[] = {T(1), T(2), T(3), T(4), T(5), T(6), T(8)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_INSERT_ARRAY, result != 7 , "Array insertion result:%i, expected:7.", result );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_INSERT_ARRAY, chains, {expectedArray}));
	T viewValues[] = {T(7)};
	result = chains.insert(chains.size() - 1, ::gpk::view<cnst T>{viewValues});
	T expectedView[] = {T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_INSERT_VIEW, result != 8 , "View insertion result:%i, expected:8.", result );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_INSERT_VIEW, chains, {expectedView}));
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(chains) , "chain insertion terminator mismatch. size:%u, terminator:%" GPK_FMT_S3 "." , chains.size(), (::gpk::s3_t)chains.begin()[chains.size()] );

	::gpk::apod<T> chainReallocated = {T(1), T(4)};
	cnst ::gpk::u2_t chainCapacity = chainReallocated.reserve(chainReallocated.size());
	if_fail_fe(chainReallocated.resize(chainCapacity, T(8)));
	cnst ::gpk::view<T> previousChainStorage = chainReallocated;
	T inserted[] = {T(2), T(3)};
	result = chainReallocated.insert(1, inserted, 2);
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_INSERT_CHAIN_REALLOCATE, result != (::gpk::err_t)chainCapacity + 2 , "Reallocating chain insertion result:%i, expected:%u.", result, chainCapacity + 2 );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_INSERT_CHAIN_REALLOCATE, chainReallocated.size() != chainCapacity + 2 , "Reallocating chain insertion count:%u, expected:%u.", chainReallocated.size(), chainCapacity + 2 );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_INSERT_CHAIN_REALLOCATE, chainReallocated.begin() == previousChainStorage.begin() , "Reallocating chain insertion retained address:%p.", chainReallocated.begin() );
	T expectedChainReallocated[] = {T(1), T(2), T(3), T(4)};
	for(::gpk::u2_t iValue = 0; iValue < chainReallocated.size() && iValue < ::gpk::size(expectedChainReallocated); ++iValue) {
		GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_INSERT_CHAIN_REALLOCATE, chainReallocated[iValue] != expectedChainReallocated[iValue] , "Reallocating chain insertion element:%u value:%" GPK_FMT_S3 ", expected:%" GPK_FMT_S3 "." , iValue, (::gpk::s3_t)chainReallocated[iValue], (::gpk::s3_t)expectedChainReallocated[iValue] );
	}
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(chainReallocated) , "reallocating chain insertion terminator mismatch. size:%u, terminator:%" GPK_FMT_S3 "." , chainReallocated.size(), (::gpk::s3_t)chainReallocated.begin()[chainReallocated.size()] );
	rtrn 0;
}

tplt<tpnm T>
sttc ::gpk::err_t testPodRemove(::gpk::ATestError & errors) {
	::gpk::apod<T> ordered = {T(1), T(2), T(3), T(4), T(5)};
	::gpk::err_t result = ordered.remove(0);
	T expectedFront[] = {T(2), T(3), T(4), T(5)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_REMOVE_FRONT, result != 4 , "Ordered front removal result:%i, expected:4.", result );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_REMOVE_FRONT, ordered, {expectedFront}));
	result = ordered.remove(1);
	T expectedMiddle[] = {T(2), T(4), T(5)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_REMOVE_MIDDLE, result != 3 , "Ordered middle removal result:%i, expected:3.", result );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_REMOVE_MIDDLE, ordered, {expectedMiddle}));
	result = ordered.remove(ordered.size() - 1);
	T expectedEnd[] = {T(2), T(4)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_REMOVE_END, result != 2 , "Ordered end removal result:%i, expected:2.", result );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_REMOVE_END, ordered, {expectedEnd}));
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(ordered) , "ordered removal terminator mismatch. size:%u, terminator:%" GPK_FMT_S3 "." , ordered.size(), (::gpk::s3_t)ordered.begin()[ordered.size()] );

	::gpk::apod<T> unordered = {T(1), T(2), T(3), T(4)};
	result = unordered.remove_unordered(1);
	T expectedUnordered[] = {T(1), T(4), T(3)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_REMOVE_UNORDERED_MIDDLE, result != 3 , "Unordered middle removal result:%i, expected:3.", result );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_REMOVE_UNORDERED_MIDDLE, unordered, {expectedUnordered}));
	result = unordered.remove_unordered(unordered.size() - 1);
	T expectedUnorderedEnd[] = {T(1), T(4)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_REMOVE_UNORDERED_END, result != 2 , "Unordered end removal result:%i, expected:2.", result );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_REMOVE_UNORDERED_END, unordered, {expectedUnorderedEnd}));
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(unordered) , "unordered removal terminator mismatch. size:%u, terminator:%" GPK_FMT_S3 "." , unordered.size(), (::gpk::s3_t)unordered.begin()[unordered.size()] );

	::gpk::apod<T> erased = {T(1), T(2), T(3), T(4)};
	result = erased.erase(&erased[1]);
	T expectedErased[] = {T(1), T(3), T(4)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_ERASE, result != 3 , "Addressed removal result:%i, expected:3.", result );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_ERASE, erased, {expectedErased}));
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(erased) , "addressed removal terminator mismatch. size:%u, terminator:%" GPK_FMT_S3 "." , erased.size(), (::gpk::s3_t)erased.begin()[erased.size()] );
	rtrn 0;
}

tplt<tpnm T>
sttc ::gpk::err_t testPodFailures(::gpk::ATestError & errors) {
	::gpk::apod<T> empty;
	T removed = T(7);
	::gpk::err_t result = podExpectedFailure([&empty]() { rtrn empty.pop_back(); });
	if_fail_fe(podFailureCheck(errors, ARRAY_POD_TEST_RESULT_POP_BACK_EMPTY, result, empty, ::gpk::view<cnst T>{}, {}));
	result = podExpectedFailure([&empty, &removed]() { rtrn empty.pop_back(removed); });
	if_fail_fe(podFailureCheck(errors, ARRAY_POD_TEST_RESULT_POP_BACK_EMPTY_VALUE, result, empty, ::gpk::view<cnst T>{}, {}));
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_POP_BACK_EMPTY_VALUE, removed != T(7) , "Empty pop-back output:%" GPK_FMT_S3 ", expected:7.", (::gpk::s3_t)removed );

	T expected[] = {T(1), T(2), T(3)};
	::gpk::apod<T> values{expected};
	cnst ::gpk::view<cnst T> originalStorage = values;
	result = podExpectedFailure([&values]() { rtrn values.insert(values.size() + 1, T(9)); });
	if_fail_fe(podFailureCheck(errors, ARRAY_POD_TEST_RESULT_INSERT_INVALID_INDEX, result, values, {expected}, originalStorage));
	result = podExpectedFailure([&values]() { rtrn values.remove(values.size()); });
	if_fail_fe(podFailureCheck(errors, ARRAY_POD_TEST_RESULT_REMOVE_INVALID_INDEX, result, values, {expected}, originalStorage));
	result = podExpectedFailure([&values]() { rtrn values.remove_unordered(values.size()); });
	if_fail_fe(podFailureCheck(errors, ARRAY_POD_TEST_RESULT_REMOVE_UNORDERED_INVALID, result, values, {expected}, originalStorage));
	result = podExpectedFailure([&values]() { rtrn values.erase(values.end()); });
	if_fail_fe(podFailureCheck(errors, ARRAY_POD_TEST_RESULT_ERASE_ONE_PAST, result, values, {expected}, originalStorage));
	result = podExpectedFailure([&values]() { rtrn values.erase(0); });
	if_fail_fe(podFailureCheck(errors, ARRAY_POD_TEST_RESULT_ERASE_NULL, result, values, {expected}, originalStorage));
	T unrelated = T(9);
	result = podExpectedFailure([&values, &unrelated]() { rtrn values.erase(&unrelated); });
	if_fail_fe(podFailureCheck(errors, ARRAY_POD_TEST_RESULT_ERASE_UNRELATED, result, values, {expected}, originalStorage));
	if constexpr(szof(T) > 1) {
		// erase() accepts an element pointer; this deliberately malformed pointer proves its boundary validation.
		cnst T * misaligned = (cnst T*)((cnst ::gpk::u0_t*)values.begin() + 1);
		result = podExpectedFailure([&values, misaligned]() { rtrn values.erase(misaligned); });
	}
	else
		result = -1;
	if_fail_fe(podFailureCheck(errors, ARRAY_POD_TEST_RESULT_ERASE_MISALIGNED, result, values, {expected}, originalStorage));

	result = podExpectedFailure([&values]() { rtrn values.reserve(0x40000000U); });
	if_fail_fe(podFailureCheck(errors, ARRAY_POD_TEST_RESULT_RESERVE_INVALID_COUNT, result, values, {expected}, originalStorage));
	result = podExpectedFailure([&values]() { rtrn values.resize(0x40000000U); });
	if_fail_fe(podFailureCheck(errors, ARRAY_POD_TEST_RESULT_RESIZE_INVALID_COUNT, result, values, {expected}, originalStorage));
	result = podExpectedFailure([&values]() { rtrn values.resize(0x40000000U, T(9)); });
	if_fail_fe(podFailureCheck(errors, ARRAY_POD_TEST_RESULT_RESIZE_INVALID_FILLED_COUNT, result, values, {expected}, originalStorage));

	result = podExpectedFailure([&values]() { rtrn values.append(nullptr, 1); });
	if_fail_fe(podFailureCheck(errors, ARRAY_POD_TEST_RESULT_APPEND_NULL_SOURCE, result, values, {expected}, originalStorage));
	result = podExpectedFailure([&values]() { rtrn values.insert(1, nullptr, 1); });
	if_fail_fe(podFailureCheck(errors, ARRAY_POD_TEST_RESULT_INSERT_NULL_SOURCE, result, values, {expected}, originalStorage));
	result = values.insert(1, nullptr, 0);
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_INSERT_EMPTY_SOURCE, result != 3 , "Empty insertion result:%i, expected:3.", result );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_INSERT_EMPTY_SOURCE, values.begin() != originalStorage.begin() , "Empty insertion moved storage. begin:%p, expected:%p.", values.begin(), originalStorage.begin() );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_INSERT_EMPTY_SOURCE, values, {expected}));
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(values) , "failed-operation terminator mismatch. size:%u, terminator:%" GPK_FMT_S3 "." , values.size(), (::gpk::s3_t)values.begin()[values.size()] );
	rtrn 0;
}

tplt<tpnm T>
sttc ::gpk::err_t testPodAliasing(::gpk::ATestError & errors) {
	::gpk::apod<T> pushed = {T(1), T(2)};
	cnst ::gpk::u2_t pushCapacity = pushed.reserve(pushed.size());
	if_fail_fe(pushed.resize(pushCapacity, T(8)));
	::gpk::err_t result = pushed.push_back(pushed[1]);
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_ALIAS_PUSH_BACK, result != (::gpk::err_t)pushCapacity , "Aliased push-back index:%i, expected:%u.", result, pushCapacity );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_ALIAS_PUSH_BACK, pushed.size() != pushCapacity + 1 , "Aliased push-back count:%u, expected:%u.", pushed.size(), pushCapacity + 1 );
	if(pushCapacity < pushed.size()) {
		GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_ALIAS_PUSH_BACK, pushed[pushCapacity] != T(2) , "Aliased push-back element:%u value:%" GPK_FMT_S3 ", expected:2." , pushCapacity, (::gpk::s3_t)pushed[pushCapacity] );
	}

	::gpk::apod<T> resized = {T(1), T(2)};
	cnst ::gpk::u2_t resizeCapacity = resized.reserve(resized.size());
	if_fail_fe(resized.resize(resizeCapacity, T(8)));
	result = resized.resize(resizeCapacity + 2, resized[1]);
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_ALIAS_RESIZE, result != (::gpk::err_t)resizeCapacity + 2 , "Aliased filled resize result:%i, expected:%u.", result, resizeCapacity + 2 );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_ALIAS_RESIZE, resized.size() != resizeCapacity + 2 , "Aliased filled resize count:%u, expected:%u.", resized.size(), resizeCapacity + 2 );
	for(::gpk::u2_t iValue = resizeCapacity; iValue < resized.size() && iValue < resizeCapacity + 2; ++iValue) {
		GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_ALIAS_RESIZE, resized[iValue] != T(2) , "Aliased filled resize element:%u value:%" GPK_FMT_S3 ", expected:2." , iValue, (::gpk::s3_t)resized[iValue] );
	}

	::gpk::apod<T> appended = {T(1), T(2), T(3)};
	::gpk::view<cnst T> appendedSource{appended.begin(), appended.size()};
	result = appended.append(appendedSource);
	T expectedAppend[] = {T(1), T(2), T(3), T(1), T(2), T(3)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_ALIAS_APPEND, result != 3 , "Aliased append index:%i, expected:3.", result );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_ALIAS_APPEND, appended, {expectedAppend}));

	::gpk::apod<T> insertedValue = {T(1), T(2), T(3)};
	insertedValue.reserve(16);
	result = insertedValue.insert(0, insertedValue[1]);
	T expectedValue[] = {T(2), T(1), T(2), T(3)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_ALIAS_INSERT_VALUE, result != 4 , "Aliased value insertion result:%i, expected:4.", result );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_ALIAS_INSERT_VALUE, insertedValue, {expectedValue}));

	::gpk::apod<T> insertedChain = {T(1), T(2), T(3), T(4)};
	insertedChain.reserve(16);
	::gpk::view<cnst T> insertedSource{&insertedChain[1], 2};
	result = insertedChain.insert(0, insertedSource);
	T expectedChain[] = {T(2), T(3), T(1), T(2), T(3), T(4)};
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_ALIAS_INSERT_CHAIN, result != 6 , "Aliased chain insertion result:%i, expected:6.", result );
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_ALIAS_INSERT_CHAIN, insertedChain, {expectedChain}));

	::gpk::apod<T> assigned = {T(1), T(2), T(3), T(4)};
	::gpk::view<cnst T> assignedSource{&assigned[1], 3};
	assigned = assignedSource;
	T expectedAssignment[] = {T(2), T(3), T(4)};
	if_fail_fe(podCheck(errors, ARRAY_POD_TEST_RESULT_ALIAS_ASSIGNMENT, assigned, {expectedAssignment}));

	GPK_TEST_CHECK(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(pushed));
	GPK_TEST_CHECK(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(resized));
	GPK_TEST_CHECK(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(appended));
	GPK_TEST_CHECK(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(insertedValue));
	GPK_TEST_CHECK(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(insertedChain));
	GPK_TEST_CHECK(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(assigned));
	rtrn 0;
}

tplt<tpnm T>
sttc ::gpk::err_t testPodClear(::gpk::ATestError & errors) {
	::gpk::apod<T> values = {T(1), T(2), T(3)};
	cnst ::gpk::view<T> allocatedStorage = values;
	cnst ::gpk::err_t result = values.clear();
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_CLEAR, result , "Clear result:%i, expected:0.", result );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_CLEAR, values.size() , "Clear count:%u, expected:0.", values.size() );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_CLEAR, values.begin() != allocatedStorage.begin() , "Clear moved storage. begin:%p, expected:%p.", values.begin(), allocatedStorage.begin() );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_CLEAR, values.end() != allocatedStorage.begin() , "Clear end:%p, expected:%p.", values.end(), allocatedStorage.begin() );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_TERMINATOR, podTerminatorMismatch(values) , "clear terminator mismatch. size:%u, terminator:%" GPK_FMT_S3 "." , values.size(), (::gpk::s3_t)values.begin()[values.size()] );
	cnst ::gpk::err_t clearResult = values.clear_pointer();
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_CLEAR_POINTER, clearResult , "Clear-pointer result:%i, expected:0.", clearResult );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_CLEAR_POINTER, values.size() , "Clear-pointer count:%u, expected:0.", values.size() );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_CLEAR_POINTER, values.begin() , "Clear-pointer begin:%p, expected:null.", values.begin() );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_CLEAR_POINTER, values.end() , "Clear-pointer end:%p, expected:null.", values.end() );
	rtrn 0;
}

tplt<tpnm T>
sttc ::gpk::err_t testPodFind(::gpk::ATestError & errors) {
	::gpk::apod<T> values = {T(1), T(2), T(1), T(2)};
	T sequenceData[] = {T(1), T(2)};
	cnst ::gpk::view<cnst T> sequence{sequenceData};
	cnst ::gpk::err_t firstFind = values.find(sequence);
	cnst ::gpk::err_t offsetFind = values.find(sequence, 1);
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_FIND_SEQUENCE, firstFind , "First sequence index:%i, expected:0.", firstFind );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_FIND_SEQUENCE, offsetFind != 2 , "Offset sequence index:%i, expected:2.", offsetFind );
	cnst ::gpk::err_t lastFind = values.rfind(sequence);
	cnst ::gpk::err_t reverseOffsetFind = values.rfind(sequence, 1);
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_RFIND_SEQUENCE, lastFind != 2 , "Last sequence index:%i, expected:2.", lastFind );
	GPK_TEST_CHECKF(errors, ARRAY_POD_TEST_RESULT_RFIND_SEQUENCE, reverseOffsetFind , "Reverse-offset sequence index:%i, expected:0.", reverseOffsetFind );
	rtrn 0;
}

tplt<tpnm T>
sttc ::gpk::err_t testPodType(::gpk::ATestError & errors) {
	if_fail_fe(testPodConstruction<T>(errors));
	if_fail_fe(testPodAssignment<T>(errors));
	if_fail_fe(testPodReserveResize<T>(errors));
	if_fail_fe(testPodAppend<T>(errors));
	if_fail_fe(testPodPopBack<T>(errors));
	if_fail_fe(testPodInsert<T>(errors));
	if_fail_fe(testPodRemove<T>(errors));
	if_fail_fe(testPodFailures<T>(errors));
	if_fail_fe(testPodAliasing<T>(errors));
	if_fail_fe(testPodFind<T>(errors));
	rtrn testPodClear<T>(errors);
}

tplt<tpnm T>
sttc ::gpk::err_t testPodTypeLogged(::gpk::ATestError & errors) {
	cnst ::gpk::u2_t checkCount = testCheckCount(errors);
	cnst ::gpk::u2_t failureCount = testErrorCount(errors);
	if_fail_fe(testPodType<T>(errors));
	cnst ::gpk::u2_t typeFailures = testErrorCount(errors) - failureCount;
	cnst ::gpk::u2_t typeChecks = testCheckCount(errors) - checkCount;
	if(typeFailures) error_printf("%s suite completed: %u/%u checks passed, %u failed.", ::gpk::get_type_namep<T>(), typeChecks - typeFailures, typeChecks, typeFailures);
	rtrn 0;
}

::gpk::err_t test_array_pod(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	cnst ::gpk::u2_t failureCount = testErrorCount(errors);
	if_fail_fe(::gpk::testTypePack(::gpk::type_pack_integers{}, [&](auto type) { rtrn testPodTypeLogged<typename decltype(type)::type>(errors); }));
	if(failureCount == testErrorCount(errors))
		always_printf("Types tested successfully:\n%s, %s, %s, %s, %s, %s, %s, %s."
			, ::gpk::get_type_namep<::gpk::u0_t>(), ::gpk::get_type_namep<::gpk::u1_t>(), ::gpk::get_type_namep<::gpk::u2_t>(), ::gpk::get_type_namep<::gpk::u3_t>()
			, ::gpk::get_type_namep<::gpk::s0_t>(), ::gpk::get_type_namep<::gpk::s1_t>(), ::gpk::get_type_namep<::gpk::s2_t>(), ::gpk::get_type_namep<::gpk::s3_t>()
			);
	rtrn 0;
}
