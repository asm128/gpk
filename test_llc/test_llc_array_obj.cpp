#include "gpk_array_obj.h"

#include "test_llc_suites.h"

#include <utility>

GDEFINE_ENUM_TYPE(ARRAY_OBJ_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, EMPLACE_INDEX	, 0, "array_obj<>::emplace_back() did not return the appended element index.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, EMPLACE_VALUE	, 1, "array_obj<>::emplace_back() did not construct the requested value in place.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, EMPLACE_DIRECT	, 2, "array_obj<>::emplace_back() copied a temporary instead of constructing from its arguments.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, EMPLACE_GROW		, 3, "array_obj<>::emplace_back() did not preserve values while growing its allocation.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, EMPLACE_LIFETIME	, 4, "array_obj<>::emplace_back() did not preserve object lifetime balance.");

GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, DEFAULT_STATE		, 5, "A default array_obj<> was not an empty null range.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, COPY_CONSTRUCTION	, 7, "array_obj<> copy construction did not produce independent equal storage.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, MOVE_CONSTRUCTION	, 8, "array_obj<> move construction did not transfer its allocation and clear the source.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, COPY_ASSIGNMENT	, 9, "array_obj<> copy assignment did not produce independent equal storage.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, RESERVE			, 10, "array_obj<>::reserve() did not preserve its elements or stable allocation.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, RESIZE			, 11, "array_obj<>::resize() did not preserve, construct or destroy the expected elements.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, PUSH_BACK			, 12, "array_obj<>::push_back() did not append and return the inserted index.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, APPEND			, 13, "array_obj<>::append() did not append its source range and return the first inserted index.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, INSERT			, 14, "array_obj<>::insert() did not preserve order while inserting values or ranges.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, POP_BACK			, 15, "array_obj<>::pop_back() did not remove or return the last element.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, REMOVE			, 16, "array_obj<>::remove() did not erase an element while preserving order.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, REMOVE_UNORDERED	, 17, "array_obj<>::remove_unordered() did not replace the removed element with the last one.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, ERASE				, 18, "array_obj<>::erase() did not remove the addressed element.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, CLEAR				, 19, "array_obj<>::clear() did not destroy its elements while retaining its allocation.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, CLEAR_POINTER		, 20, "array_obj<>::clear_pointer() did not destroy its elements and release its allocation.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, FIND_SEQUENCE		, 21, "array_obj<>::find() did not compare sequence elements by value.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, RFIND_SEQUENCE		, 22, "array_obj<>::rfind() did not compare sequence elements by value.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, ARRAY_CONSTRUCTION	, 23, "array_obj<> did not copy its source array.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, VIEW_CONSTRUCTION	, 24, "array_obj<> did not copy its source view.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, SELF_ASSIGNMENT		, 25, "array_obj<> changed during self-assignment.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, FILLED_RESIZE		, 26, "array_obj<> did not construct the expected fill values.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, VARIADIC_RESIZE	, 27, "array_obj<> did not construct the expected variadic values.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, SHRINK_RESIZE		, 28, "array_obj<> did not preserve expected values while shrinking.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, DEFAULT_RESIZE		, 29, "array_obj<> did not default-construct the expected values.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, ARRAY_APPEND		, 30, "array_obj<> did not append its source array.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, VIEW_APPEND		, 31, "array_obj<> did not append its source view.");
GDEFINE_ENUM_VALUED(ARRAY_OBJ_TEST_RESULT, EMPTY_APPEND		, 32, "array_obj<> changed while appending an empty range.");

stct SArrayObjTestValue {
	sttc ::gpk::u2_t	LiveCount;
	sttc ::gpk::u2_t	ConstructorCount;
	sttc ::gpk::u2_t	CopyCount;
	sttc ::gpk::u2_t	DestructorCount;

	::gpk::u2_t			Index	= {};
	::gpk::u3_t			Value	= {};

	SArrayObjTestValue() { ++LiveCount; ++ConstructorCount; }
	SArrayObjTestValue(::gpk::u2_t index) : SArrayObjTestValue(index, (::gpk::u3_t)index * 10) {}
	SArrayObjTestValue(::gpk::u2_t index, ::gpk::u3_t value) : Index(index), Value(value) { ++LiveCount; ++ConstructorCount; }
	SArrayObjTestValue(cnst SArrayObjTestValue & other) : Index(other.Index), Value(other.Value) { ++LiveCount; ++CopyCount; }
	~SArrayObjTestValue() { --LiveCount; ++DestructorCount; }
	SArrayObjTestValue & operator=(cnst SArrayObjTestValue & other) { Index = other.Index; Value = other.Value; rtrn *this; }
};

::gpk::u2_t SArrayObjTestValue::LiveCount			= {};
::gpk::u2_t SArrayObjTestValue::ConstructorCount	= {};
::gpk::u2_t SArrayObjTestValue::CopyCount			= {};
::gpk::u2_t SArrayObjTestValue::DestructorCount		= {};

sttc ::gpk::err_t arrayObjCheck(::gpk::ATestError & errors, ARRAY_OBJ_TEST_RESULT result, cnst ::gpk::aobj<SArrayObjTestValue> & actual, ::gpk::view<cnst ::gpk::u2_t> expected) {
	GPK_TEST_CHECKF(errors, result, actual.size() != expected.size() , "Count:%u, expected:%u." , actual.size(), expected.size() );
	for(::gpk::u2_t iValue = 0; iValue < actual.size() && iValue < expected.size(); ++iValue) {
		GPK_TEST_CHECKF(errors, result, actual[iValue].Index != expected[iValue] , "Element:%u index:%u, expected:%u." , iValue, actual[iValue].Index, expected[iValue] );
		GPK_TEST_CHECKF(errors, result, actual[iValue].Value != (::gpk::u3_t)expected[iValue] * 10 , "Element:%u value:%" GPK_FMT_U3 ", expected:%" GPK_FMT_U3 "." , iValue, actual[iValue].Value, (::gpk::u3_t)expected[iValue] * 10 );
	}
	rtrn 0;
}

sttc ::gpk::err_t testArrayObjEmplace(::gpk::ATestError & errors) {
	cnst ::gpk::u2_t constructorsBefore	= SArrayObjTestValue::ConstructorCount;
	cnst ::gpk::u2_t copiesBefore		= SArrayObjTestValue::CopyCount;
	cnst ::gpk::u2_t liveBefore			= SArrayObjTestValue::LiveCount;
	{
		::gpk::aobj<SArrayObjTestValue> values = {};
		if_fail_fe(values.reserve(16));
		cnst ::gpk::err_t firstIndex	= values.emplace_back(::gpk::u2_t{0}, ::gpk::u3_t{0});
		cnst ::gpk::err_t secondIndex	= values.emplace_back(::gpk::u2_t{1}, ::gpk::u3_t{10});
		GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_EMPLACE_INDEX, firstIndex , "First index:%i, expected:0." , firstIndex );
		GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_EMPLACE_INDEX, secondIndex != 1 , "Second index:%i, expected:1." , secondIndex );
		GPK_TEST_REQUIREF(errors, ARRAY_OBJ_TEST_RESULT_EMPLACE_INDEX, values.size() != 2 , "Count:%u, expected:2." , values.size() );
		GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_EMPLACE_DIRECT, SArrayObjTestValue::ConstructorCount - constructorsBefore != 2 , "Direct constructions:%u, expected:2." , SArrayObjTestValue::ConstructorCount - constructorsBefore );
		GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_EMPLACE_DIRECT, SArrayObjTestValue::CopyCount != copiesBefore , "Copies:%u, expected:0." , SArrayObjTestValue::CopyCount - copiesBefore );
		GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_EMPLACE_DIRECT, SArrayObjTestValue::LiveCount - liveBefore != 2 , "Live objects added:%u, expected:2." , SArrayObjTestValue::LiveCount - liveBefore );

		for(::gpk::u2_t iValue = 2; iValue < 64; ++iValue) {
			cnst ::gpk::err_t index = values.emplace_back(iValue, (::gpk::u3_t)iValue * 10);
			GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_EMPLACE_INDEX, index != (::gpk::err_t)iValue , "Value:%u returned index:%i, expected:%u." , iValue, index, iValue );
			GPK_TEST_REQUIREF(errors, ARRAY_OBJ_TEST_RESULT_EMPLACE_INDEX, values.size() != iValue + 1 , "Value:%u left count:%u, expected:%u." , iValue, values.size(), iValue + 1 );
		}
		for(::gpk::u2_t iValue = 0; iValue < values.size(); ++iValue) {
			GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_EMPLACE_VALUE, values[iValue].Index != iValue , "Element:%u index:%u, expected:%u." , iValue, values[iValue].Index, iValue );
			GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_EMPLACE_VALUE, values[iValue].Value != (::gpk::u3_t)iValue * 10 , "Element:%u value:%" GPK_FMT_U3 ", expected:%" GPK_FMT_U3 "." , iValue, values[iValue].Value, (::gpk::u3_t)iValue * 10 );
		}
		GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_EMPLACE_GROW, SArrayObjTestValue::CopyCount == copiesBefore , "Growth made %u copies; expected at least one." , SArrayObjTestValue::CopyCount - copiesBefore );
		GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_EMPLACE_GROW, SArrayObjTestValue::LiveCount - liveBefore != values.size() , "Live objects added:%u, elements:%u." , SArrayObjTestValue::LiveCount - liveBefore, values.size() );
	}
	rtrn 0;
}

sttc ::gpk::err_t testArrayObjConstruction(::gpk::ATestError & errors) {
	::gpk::aobj<SArrayObjTestValue> empty = {};
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_DEFAULT_STATE, empty.size() , "Default count:%u, expected:0." , empty.size() );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_DEFAULT_STATE, empty.begin() , "Default begin:%p, expected:null." , empty.begin() );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_DEFAULT_STATE, empty.end() , "Default end:%p, expected:null." , empty.end() );

	SArrayObjTestValue source[] = {SArrayObjTestValue{1}, SArrayObjTestValue{2}, SArrayObjTestValue{3}};
	cnst ::gpk::u2_t expected[] = {1, 2, 3};
	::gpk::aobj<SArrayObjTestValue> fromArray{source};
	::gpk::view<cnst SArrayObjTestValue> sourceView = {source};
	::gpk::aobj<SArrayObjTestValue> fromView{sourceView};
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_ARRAY_CONSTRUCTION, fromArray, {expected}));
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_VIEW_CONSTRUCTION, fromView, {expected}));

	::gpk::aobj<SArrayObjTestValue> copied{fromArray};
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_COPY_CONSTRUCTION, copied, {expected}));
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_COPY_CONSTRUCTION, copied.begin() == fromArray.begin() , "Copy aliases source. source:%p, copy:%p." , fromArray.begin(), copied.begin() );
	cnst ::gpk::view<SArrayObjTestValue> movedStorage = copied;
	::gpk::aobj<SArrayObjTestValue> moved{::std::move(copied)};
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_MOVE_CONSTRUCTION, moved, {expected}));
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_MOVE_CONSTRUCTION, moved.begin() != movedStorage.begin() , "Move did not transfer storage. moved:%p, source storage:%p." , moved.begin(), movedStorage.begin() );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_MOVE_CONSTRUCTION, copied.size() , "Move source count:%u, expected:0." , copied.size() );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_MOVE_CONSTRUCTION, copied.begin() , "Move source begin:%p, expected:null." , copied.begin() );

	::gpk::aobj<SArrayObjTestValue> assigned = {SArrayObjTestValue{9}};
	assigned = fromArray;
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_COPY_ASSIGNMENT, assigned, {expected}));
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_COPY_ASSIGNMENT, assigned.begin() == fromArray.begin() , "Assignment aliases source. source:%p, target:%p." , fromArray.begin(), assigned.begin() );
	assigned = assigned;
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_SELF_ASSIGNMENT, assigned, {expected}));
	rtrn 0;
}

sttc ::gpk::err_t testArrayObjReserveResize(::gpk::ATestError & errors) {
	SArrayObjTestValue source[] = {SArrayObjTestValue{1}, SArrayObjTestValue{2}};
	::gpk::aobj<SArrayObjTestValue> values{source};
	cnst ::gpk::u2_t expectedInitial[] = {1, 2};
	cnst ::gpk::err_t capacity = values.reserve(32);
	GPK_TEST_REQUIREF(errors, ARRAY_OBJ_TEST_RESULT_RESERVE, capacity < 32 , "Reserve capacity:%i, expected at least:32." , capacity );
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_RESERVE, values, {expectedInitial}));
	cnst ::gpk::view<SArrayObjTestValue> reservedStorage = values;
	cnst ::gpk::err_t stableCapacity = values.reserve(16);
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_RESERVE, stableCapacity != capacity , "Stable reserve capacity:%i, expected:%i." , stableCapacity, capacity );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_RESERVE, values.begin() != reservedStorage.begin() , "Stable reserve moved storage. begin:%p, expected:%p." , values.begin(), reservedStorage.begin() );

	SArrayObjTestValue fill{9};
	::gpk::err_t result = values.resize(4, fill);
	cnst ::gpk::u2_t expectedFill[] = {1, 2, 9, 9};
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_RESIZE, result != 4 , "Filled resize result:%i, expected:4." , result );
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_FILLED_RESIZE, values, {expectedFill}));
	result = values.resize(6, ::gpk::u2_t{7}, ::gpk::u3_t{70});
	cnst ::gpk::u2_t expectedVariadic[] = {1, 2, 9, 9, 7, 7};
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_RESIZE, result != 6 , "Variadic resize result:%i, expected:6." , result );
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_VARIADIC_RESIZE, values, {expectedVariadic}));
	result = values.resize(3);
	cnst ::gpk::u2_t expectedShrink[] = {1, 2, 9};
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_RESIZE, result != 3 , "Shrink result:%i, expected:3." , result );
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_SHRINK_RESIZE, values, {expectedShrink}));
	result = values.resize(5);
	cnst ::gpk::u2_t expectedDefault[] = {1, 2, 9, 0, 0};
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_RESIZE, result != 5 , "Default resize result:%i, expected:5." , result );
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_DEFAULT_RESIZE, values, {expectedDefault}));
	rtrn 0;
}

sttc ::gpk::err_t testArrayObjAppendInsert(::gpk::ATestError & errors) {
	::gpk::aobj<SArrayObjTestValue> values = {SArrayObjTestValue{1}, SArrayObjTestValue{2}};
	SArrayObjTestValue third{3};
	::gpk::err_t result = values.push_back(third);
	cnst ::gpk::u2_t expectedPush[] = {1, 2, 3};
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_PUSH_BACK, result != 2 , "Push result:%i, expected index:2." , result );
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_PUSH_BACK, values, {expectedPush}));

	SArrayObjTestValue arrayTail[] = {SArrayObjTestValue{4}, SArrayObjTestValue{5}};
	result = values.append(arrayTail);
	cnst ::gpk::u2_t expectedArray[] = {1, 2, 3, 4, 5};
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_APPEND, result != 3 , "Array append result:%i, expected index:3." , result );
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_ARRAY_APPEND, values, {expectedArray}));
	SArrayObjTestValue viewTail[] = {SArrayObjTestValue{6}, SArrayObjTestValue{7}};
	result = values.append(::gpk::view<cnst SArrayObjTestValue>{viewTail});
	cnst ::gpk::u2_t expectedView[] = {1, 2, 3, 4, 5, 6, 7};
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_APPEND, result != 5 , "View append result:%i, expected index:5." , result );
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_VIEW_APPEND, values, {expectedView}));
	result = values.append(::gpk::view<cnst SArrayObjTestValue>{});
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_APPEND, result != 7 , "Empty append result:%i, expected index:7." , result );
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_EMPTY_APPEND, values, {expectedView}));

	::gpk::aobj<SArrayObjTestValue> inserted = {SArrayObjTestValue{2}, SArrayObjTestValue{5}};
	if_fail_fe(inserted.reserve(32));
	SArrayObjTestValue one{1};
	cnst ::gpk::err_t insertFront = inserted.insert(0, one);
	SArrayObjTestValue middle[] = {SArrayObjTestValue{3}, SArrayObjTestValue{4}};
	cnst ::gpk::err_t insertMiddle = inserted.insert(2, ::gpk::view<cnst SArrayObjTestValue>{middle});
	SArrayObjTestValue six{6};
	cnst ::gpk::err_t insertEnd = inserted.insert(inserted.size(), six);
	cnst ::gpk::u2_t expectedInsert[] = {1, 2, 3, 4, 5, 6};
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_INSERT, insertFront != 3 , "Front insertion result:%i, expected:3." , insertFront );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_INSERT, insertMiddle != 5 , "Middle insertion result:%i, expected:5." , insertMiddle );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_INSERT, insertEnd != 6 , "End insertion result:%i, expected:6." , insertEnd );
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_INSERT, inserted, {expectedInsert}));

	::gpk::aobj<SArrayObjTestValue> reallocated = {SArrayObjTestValue{1}, SArrayObjTestValue{3}};
	cnst ::gpk::err_t reallocatedCapacity = reallocated.reserve(reallocated.size());
	GPK_TEST_REQUIREF(errors, ARRAY_OBJ_TEST_RESULT_RESERVE, reallocatedCapacity < 2 , "Reallocation setup capacity:%i, expected at least:2." , reallocatedCapacity );
	SArrayObjTestValue padding{8};
	cnst ::gpk::err_t fillResult = reallocated.resize(reallocatedCapacity, padding);
	GPK_TEST_REQUIREF(errors, ARRAY_OBJ_TEST_RESULT_RESIZE, fillResult != reallocatedCapacity , "Reallocation setup resize result:%i, expected:%i." , fillResult, reallocatedCapacity );
	GPK_TEST_REQUIREF(errors, ARRAY_OBJ_TEST_RESULT_RESIZE, reallocated.size() != (::gpk::u2_t)reallocatedCapacity , "Reallocation setup count:%u, expected:%i." , reallocated.size(), reallocatedCapacity );
	SArrayObjTestValue two{2};
	result = reallocated.insert(1, two);
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_INSERT, result != reallocatedCapacity + 1 , "Reallocating insertion result:%i, expected:%i." , result, reallocatedCapacity + 1 );
	GPK_TEST_REQUIREF(errors, ARRAY_OBJ_TEST_RESULT_INSERT, reallocated.size() != (::gpk::u2_t)reallocatedCapacity + 1 , "Reallocating insertion count:%u, expected:%u." , reallocated.size(), (::gpk::u2_t)reallocatedCapacity + 1 );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_INSERT, reallocated[0].Index != 1 , "Reallocating insertion first index:%u, expected:1." , reallocated[0].Index );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_INSERT, reallocated[1].Index != 2 , "Reallocating insertion second index:%u, expected:2." , reallocated[1].Index );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_INSERT, reallocated[2].Index != 3 , "Reallocating insertion third index:%u, expected:3." , reallocated[2].Index );
	rtrn 0;
}

sttc ::gpk::err_t testArrayObjRemoveClear(::gpk::ATestError & errors) {
	::gpk::aobj<SArrayObjTestValue> values = {SArrayObjTestValue{1}, SArrayObjTestValue{2}, SArrayObjTestValue{3}, SArrayObjTestValue{4}, SArrayObjTestValue{5}};
	SArrayObjTestValue removed = {};
	::gpk::err_t result = values.pop_back(removed);
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_POP_BACK, result != 4 , "Value pop result:%i, expected:4." , result );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_POP_BACK, removed.Index != 5 , "Value pop removed index:%u, expected:5." , removed.Index );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_POP_BACK, removed.Value != 50 , "Value pop removed value:%" GPK_FMT_U3 ", expected:50." , removed.Value );
	result = values.pop_back();
	cnst ::gpk::u2_t expectedPop[] = {1, 2, 3};
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_POP_BACK, result != 3 , "Pop result:%i, expected:3." , result );
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_POP_BACK, values, {expectedPop}));
	result = values.remove(1);
	cnst ::gpk::u2_t expectedRemove[] = {1, 3};
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_REMOVE, result != 2 , "Ordered removal result:%i, expected:2." , result );
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_REMOVE, values, {expectedRemove}));

	::gpk::aobj<SArrayObjTestValue> unordered = {SArrayObjTestValue{1}, SArrayObjTestValue{2}, SArrayObjTestValue{3}, SArrayObjTestValue{4}};
	result = unordered.remove_unordered(1);
	cnst ::gpk::u2_t expectedUnordered[] = {1, 4, 3};
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_REMOVE_UNORDERED, result != 3 , "Unordered removal result:%i, expected:3." , result );
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_REMOVE_UNORDERED, unordered, {expectedUnordered}));

	::gpk::aobj<SArrayObjTestValue> erased = {SArrayObjTestValue{1}, SArrayObjTestValue{2}, SArrayObjTestValue{3}};
	result = erased.erase(&erased[1]);
	cnst ::gpk::u2_t expectedErase[] = {1, 3};
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_ERASE, result != 2 , "Erase result:%i, expected:2." , result );
	if_fail_fe(arrayObjCheck(errors, ARRAY_OBJ_TEST_RESULT_ERASE, erased, {expectedErase}));

	::gpk::aobj<SArrayObjTestValue> cleared = {SArrayObjTestValue{1}, SArrayObjTestValue{2}, SArrayObjTestValue{3}};
	cnst ::gpk::view<SArrayObjTestValue> allocatedStorage = cleared;
	result = cleared.clear();
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_CLEAR, result , "Clear result:%i, expected:0." , result );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_CLEAR, cleared.size() , "Clear count:%u, expected:0." , cleared.size() );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_CLEAR, cleared.begin() != allocatedStorage.begin() , "Clear moved storage. begin:%p, expected:%p." , cleared.begin(), allocatedStorage.begin() );
	if_fail_fe(cleared.emplace_back(::gpk::u2_t{9}));
	result = cleared.clear_pointer();
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_CLEAR_POINTER, result , "Clear-pointer result:%i, expected:0." , result );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_CLEAR_POINTER, cleared.size() , "Clear-pointer count:%u, expected:0." , cleared.size() );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_CLEAR_POINTER, cleared.begin() , "Clear-pointer begin:%p, expected:null." , cleared.begin() );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_CLEAR_POINTER, cleared.end() , "Clear-pointer end:%p, expected:null." , cleared.end() );
	rtrn 0;
}

sttc ::gpk::err_t testArrayObjFind(::gpk::ATestError & errors) {
	::gpk::sc_t targetA0[] = {'a'};
	::gpk::sc_t targetB0[] = {'b'};
	::gpk::sc_t targetA1[] = {'a'};
	::gpk::sc_t targetB1[] = {'b'};
	cnst ::gpk::vcst_t targetData[] = {{targetA0}, {targetB0}, {targetA1}, {targetB1}};
	cnst ::gpk::aobj<::gpk::vcst_t> target{targetData};

	::gpk::sc_t sequenceA[] = {'a'};
	::gpk::sc_t sequenceB[] = {'b'};
	cnst ::gpk::vcst_t sequenceData[] = {{sequenceA}, {sequenceB}};
	cnst ::gpk::view<cnst ::gpk::vcst_t> sequence{sequenceData};
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_FIND_SEQUENCE, target.find(sequence) , "Semantic sequence find result:%i, expected:0." , target.find(sequence) );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_FIND_SEQUENCE, target.find(sequence, 1) != 2 , "Semantic sequence find after offset 1:%i, expected:2." , target.find(sequence, 1) );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_RFIND_SEQUENCE, target.rfind(sequence) != 2 , "Semantic sequence rfind result:%i, expected:2." , target.rfind(sequence) );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_RFIND_SEQUENCE, target.rfind(sequence, 1) , "Semantic sequence rfind through offset 1:%i, expected:0." , target.rfind(sequence, 1) );
	rtrn 0;
}

::gpk::err_t test_array_obj(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	SArrayObjTestValue::LiveCount			= {};
	SArrayObjTestValue::ConstructorCount	= {};
	SArrayObjTestValue::CopyCount			= {};
	SArrayObjTestValue::DestructorCount		= {};
	if_fail_fe(::testArrayObjEmplace(errors));
	if_fail_fe(::testArrayObjConstruction(errors));
	if_fail_fe(::testArrayObjReserveResize(errors));
	if_fail_fe(::testArrayObjAppendInsert(errors));
	if_fail_fe(::testArrayObjRemoveClear(errors));
	if_fail_fe(::testArrayObjFind(errors));
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_EMPLACE_LIFETIME, SArrayObjTestValue::LiveCount , "Live objects after suite:%u, expected:0." , SArrayObjTestValue::LiveCount );
	GPK_TEST_CHECKF(errors, ARRAY_OBJ_TEST_RESULT_EMPLACE_LIFETIME, SArrayObjTestValue::DestructorCount != SArrayObjTestValue::ConstructorCount + SArrayObjTestValue::CopyCount , "Destructors:%u, constructors:%u, copies:%u." , SArrayObjTestValue::DestructorCount, SArrayObjTestValue::ConstructorCount, SArrayObjTestValue::CopyCount );
	rtrn 0;
}
