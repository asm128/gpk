#include "gpk_log.h"

#include <functional>

#if defined(GPK_ATMEL)
#	include <string.h>
#else
#	include <cstring>
#endif

#ifndef GPK_ARRAY_VIEW_H_2398472395543
#define GPK_ARRAY_VIEW_H_2398472395543

namespace gpk
{
#pragma pack(push, 1)
	template <typename _tCell>
	class view {
	protected:
		// Properties / Member Variables
		_tCell						* Data					= 0;
		uint32_t					Count					= 0;
	public:
		typedef	_tCell				TElement;
		typedef	view<const _tCell>	TConstView;
		typedef	view<_tCell>		TView;

		// Constructors
		inlcxpr						view					()												noexcept	= default;

		template <size_t _elementCount>
		inlcxpr						view					(_tCell (&elements)[_elementCount])				noexcept	: Data(elements), Count(_elementCount)										{}

		template <size_t _elementCount>
		inlcxpr						view					(uint32_t elementCount, _tCell (&elements)[_elementCount])	: Data(elements), Count(::gpk::min((uint32_t)_elementCount, elementCount))	{}

		inline						view					(_tCell * elements, uint32_t elementCount)					: Data(elements), Count(elementCount)										{
			gthrow_if(0 == elements && 0 != elementCount, "Invalid parameters: %p, %u.", elements, elementCount);	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}

		template <size_t _elementCount>
		inline						view					(_tCell (&elements)[_elementCount], uint32_t elementCount)	: Data(elements), Count(::gpk::min((uint32_t)_elementCount, elementCount))	{
			gthrow_if(elementCount > _elementCount, "Element count out of range. Max count: %u. Requested: %u.", (uint32_t)_elementCount, elementCount);
		}

		// Operators
		inlcxpr	operator			view<const _tCell>		()								const	noexcept	{ return {Data, Count}; }

		_tCell&						operator[]				(uint32_t index)									{
			gthrow_if(0 == Data, "%s", "Uninitialized array pointer.");
			gthrow_if(index >= Count, "Invalid index: %i.", index);
			return Data[index];
		}
		const _tCell&				operator[]				(uint32_t index)				const				{
			gthrow_if(0 == Data, "%s", "Uninitialized array pointer.");
			gthrow_if(index >= Count, "Invalid index: %i.", index);
			return Data[index];
		}
		inline	bool				operator				!=(const TConstView & other)	const				{ return !operator==(other); }
		bool						operator				==(const TConstView & other)	const				{
			if(this->size() != other.size())
				return false;
			if(this->begin() == other.begin())
				return true;
			return ::gpk::equal(other.begin(), this->begin(), this->size());
		}

		// Methods

		inlcxpr	const _tCell*		begin					()								const	noexcept	{ return Data;			}
		inlcxpr	const _tCell*		end						()								const	noexcept	{ return Data + Count;	}
		inlcxpr	const uint32_t&		size					()								const	noexcept	{ return Count;			}
		inlcxpr	const uint32_t		byte_count				()								const	noexcept	{ return (uint32_t)(Count * sizeof(_tCell));	}

		inline	_tCell*				begin					()										noexcept	{ return Data;			}
		inline	_tCell*				end						()										noexcept	{ return Data + Count;	}

		inline	::gpk::error_t		fill					(const _tCell & value, uint32_t offset = 0, uint32_t stop = 0xFFFFFFFFU) {
			for(uint32_t i = offset; i < ::gpk::min(Count, stop); ++i)
				Data[i]						= value;
			return Count;
		}

		typedef std::function<::gpk::error_t(_tCell&)>							TFuncForEach;
		typedef std::function<::gpk::error_t(const _tCell&)>					TFuncForEachConst;
		typedef std::function<::gpk::error_t(uint32_t & index, _tCell&)>		TFuncForEnumerate;
		typedef std::function<::gpk::error_t(uint32_t & index, const _tCell&)>	TFuncForEnumerateConst;

		inline	::gpk::error_t		for_each				(const TFuncForEach & funcForEach, uint32_t offset = 0, uint32_t stop = 0xFFFFFFFFU)						{ 
			return enumerate([&funcForEach](uint32_t, _tCell & value) { return funcForEach(value); }, offset, stop);
		}
		inline	::gpk::error_t		for_each				(const TFuncForEachConst & funcForEach, uint32_t offset = 0, uint32_t stop = 0xFFFFFFFFU)		const		{ 
			return enumerate([&funcForEach](uint32_t, const _tCell & value) { return funcForEach(value); }, offset, stop);
		}
		::gpk::error_t				enumerate				(const TFuncForEnumerate & funcForEach, uint32_t offset = 0, uint32_t stop = 0xFFFFFFFFU)					{ 
			for(uint32_t iCell = offset, cellCount = ::gpk::min(stop, Count); iCell < cellCount; ++iCell) {
				::gpk::error_t					funcResult				= funcForEach(iCell, Data[iCell]);
				if(errored(funcResult))
					return -(::gpk::error_t)iCell;
				else if(funcResult)
					return iCell;
			}
			return Count;
		}
		::gpk::error_t				enumerate				(const TFuncForEnumerateConst & funcForEach, uint32_t offset = 0, uint32_t stop = 0xFFFFFFFFU)	const		{ 
			for(uint32_t iCell = offset, cellCount = ::gpk::min(stop, Count); iCell < cellCount; ++iCell) {
				::gpk::error_t					funcResult				= funcForEach(iCell, Data[iCell]);
				if(errored(funcResult))
					return -(::gpk::error_t)iCell;
				else if(funcResult)
					return iCell;
			}
			return Count;
		}
		::gpk::error_t				slice					(TView & out, uint32_t offset, uint32_t count = (uint32_t)-1)				{
			ree_if(offset > Count, "Out of range. Max offset: %u. Requested: %u.", (uint32_t)Count, offset);
			const uint32_t					newSize					= Count - offset;
			if(count != (uint32_t)-1)
				ree_if(count > newSize, "Out of range. Max size: %u. Requested: %u.", (uint32_t)newSize, count);
			out							= {&Data[offset], ::gpk::min(newSize, count)};
			return out.size();
		}
		::gpk::error_t				slice					(TConstView & out, uint32_t offset, uint32_t count = (uint32_t)-1)	const	{
			ree_if(offset > Count, "Out of range. Max offset: %u. Requested: %u.", (uint32_t)Count, offset);
			const uint32_t					newSize					= Count - offset;
			if(count != (uint32_t)-1)
				ree_if(count > newSize, "Out of range. Max size: %u. Requested: %u.", (uint32_t)newSize, count);
			out							= {&Data[offset], ::gpk::min(newSize, count)};
			return out.size();
		}
		::gpk::error_t				reverse					()																			{
			const uint32_t					lastElement				= Count - 1;
			for(uint32_t i = 0, swapCount = Count / 2; i < swapCount; ++i) {
				_tCell							old						= Data[i];
				Data[i]						= Data[lastElement - i];
				Data[lastElement - i]		= old;
			}
			return 0;
		}
	}; // view<>

	template<typename _tCell>	using view_array		= ::gpk::view<_tCell>;
	template<typename _tCell>	using view1d			= ::gpk::view<_tCell>;
	template<typename _tCell>	using v1				= ::gpk::view<_tCell>;

	template <typename _tCell>	inlcxpr	uint32_t	size		(const ::gpk::view<_tCell> & viewToTest)	noexcept	{ return viewToTest.size();			}
	template <typename _tCell>	inlcxpr	uint32_t	byte_count	(const ::gpk::view<_tCell> & viewToTest)	noexcept	{ return viewToTest.byte_count();	}

#pragma pack(pop)

	// view_array common typedefs
	typedef				::gpk::view<ubyte_t				>	view_ubyte			, v1ub, vub;
	typedef				::gpk::view<byte_t				>	view_byte			, v1b, vb;
	typedef				::gpk::view<uchar_t				>	view_uchar			, v1uc, vuc;
	typedef				::gpk::view<char_t				>	view_char			, v1c, vc;
	typedef				::gpk::view<float				>	view_float32		, v1f32, v1f, vf32, vf;
	typedef				::gpk::view<double				>	view_float64		, v1f64, v1d, vf64, vd;
	typedef				::gpk::view<uint8_t				>	view_uint8			, v1u8, vu8;
	typedef				::gpk::view<uint16_t			>	view_uint16			, v1u16, vu16;
	typedef				::gpk::view<uint32_t			>	view_uint32			, v1u32, vu32;
	typedef				::gpk::view<uint64_t			>	view_uint64			, v1u64, vu64;
	typedef				::gpk::view<int8_t				>	view_int8			, v1i8, vi8;
	typedef				::gpk::view<int16_t				>	view_int16			, v1i16, vi16;
	typedef				::gpk::view<int32_t				>	view_int32			, v1i32, vi32;
	typedef				::gpk::view<int64_t				>	view_int64			, v1i64, vi64;

	// view_array<const> common typedefs
	typedef				::gpk::view<const ubyte_t		>	view_const_ubyte	, v1cub, vcub;
	typedef				::gpk::view<const byte_t		>	view_const_byte		, v1cb, vcb;
	typedef				::gpk::view<const uchar_t		>	view_const_uchar	, v1cuc, vcuc;
	typedef				::gpk::view<const char_t		>	view_const_char		, v1cc, vcc;
	typedef				::gpk::view<const float			>	view_const_float32	, v1cf32, v1cf, vcf32, vcf;
	typedef				::gpk::view<const double		>	view_const_float64	, v1cf64, v1cd, vcf64, vcd;
	typedef				::gpk::view<const uint8_t		>	view_const_uint8	, v1cu8, vcu8;
	typedef				::gpk::view<const uint16_t		>	view_const_uint16	, v1cu16, vcu16;
	typedef				::gpk::view<const uint32_t		>	view_const_uint32	, v1cu32, vcu32;
	typedef				::gpk::view<const uint64_t		>	view_const_uint64	, v1cu64, vcu64;
	typedef				::gpk::view<const int8_t		>	view_const_int8		, v1ci8, vci8;
	typedef				::gpk::view<const int16_t		>	view_const_int16	, v1ci16, vci16;
	typedef				::gpk::view<const int32_t		>	view_const_int32	, v1ci32, vci32;
	typedef				::gpk::view<const int64_t		>	view_const_int64	, v1ci64, vci64;

	typedef				::gpk::view<::gpk::vub	>	vvub;
	typedef				::gpk::view<::gpk::vb	>	vvb;
	typedef				::gpk::view<::gpk::vuc	>	vvuc;
	typedef				::gpk::view<::gpk::vc	>	vvc;
	typedef				::gpk::view<::gpk::v1f	>	vv1f;
	typedef				::gpk::view<::gpk::v1d	>	vv1d;
	typedef				::gpk::view<::gpk::vu8	>	vvu8;
	typedef				::gpk::view<::gpk::vu16	>	vvu16;
	typedef				::gpk::view<::gpk::vu32	>	vvu32;
	typedef				::gpk::view<::gpk::vu64	>	vvu64;
	typedef				::gpk::view<::gpk::vi8	>	vvi8;
	typedef				::gpk::view<::gpk::vi16	>	vvi16;
	typedef				::gpk::view<::gpk::vi32	>	vvi32;
	typedef				::gpk::view<::gpk::vi64	>	vvi64;

	// view_array<const> common typedefs
	typedef				::gpk::view<const vcub	>	vcvcub;
	typedef				::gpk::view<const vcb	>	vcvcb;
	typedef				::gpk::view<const vcuc	>	vcvcuc;
	typedef				::gpk::view<const vcc	>	vcvcc;
	typedef				::gpk::view<const v1cf	>	vcv1cf;
	typedef				::gpk::view<const v1cd	>	vcv1cd;
	typedef				::gpk::view<const vcu8	>	vcvcu8;
	typedef				::gpk::view<const vcu16	>	vcvcu16;
	typedef				::gpk::view<const vcu32	>	vcvcu32;
	typedef				::gpk::view<const vcu64	>	vcvcu64;
	typedef				::gpk::view<const vci8	>	vcvci8;
	typedef				::gpk::view<const vci16	>	vcvci16;
	typedef				::gpk::view<const vci32	>	vcvci32;
	typedef				::gpk::view<const vci64	>	vcvci64;

	static constexpr ::gpk::vcc		STR_NULL		= {4, "null"};
	static constexpr ::gpk::vcc		STR_TRUE		= {4, "true"};
	static constexpr ::gpk::vcc		STR_FALSE		= {5, "false"};

	struct view_string : public view<char_t> {
		inlcxpr							view_string				()															= default;
		inlcxpr							view_string				(const view_char & other)						noexcept	: view(other)				{}
										view_string				(char * inputString, uint32_t length)						: view(inputString, length)	{ Count = (length == (uint32_t)-1) ? (uint32_t)strlen(inputString) : length;					}
		template<size_t _stringLength>	view_string				(char (&inputString)[_stringLength])			noexcept	: view(inputString)			{ Count = (uint32_t)strnlen(inputString, (uint32_t)_stringLength);								}
		template<size_t _stringLength>	view_string				(char (&inputString)[_stringLength], uint32_t length)		: view(inputString, length)	{ if(length == (uint32_t)-1) Count = (uint32_t)strnlen(inputString, (uint32_t)_stringLength);	}
	};

	struct view_const_string : public view<const char_t> {
		inlcxpr							view_const_string		()															= default;
		inlcxpr							view_const_string		(const view_const_char & other)					noexcept	: view(other)				{}
										view_const_string		(const char* inputString, uint32_t length)					: view(inputString, length)	{ Count = (length == (uint32_t)-1) ? (uint32_t)strlen(inputString) : length;					}
		template<size_t _stringLength>	view_const_string		(const char (&inputString)[_stringLength])		noexcept	: view(inputString)			{ Count = (uint32_t)strnlen(inputString, (uint32_t)_stringLength);								}
		template<size_t _stringLength>	view_const_string		(const char (&inputString)[_stringLength], uint32_t length)	: view(inputString, length)	{ if(length == (uint32_t)-1) Count = (uint32_t)strnlen(inputString, (uint32_t)_stringLength);	}
	};

	typedef				::gpk::view_string					vs;
	typedef				::gpk::view_const_string			vcs;

	typedef				::gpk::view<::gpk::vs			>	vvs;
	typedef				::gpk::view<::gpk::vcs			>	vvcs;
	typedef				::gpk::view<const ::gpk::vs		>	vcvs;
	typedef				::gpk::view<const ::gpk::vcs	>	vcvcs;





	constexpr ::gpk::vcc			TRIM_CHARACTERS			= " \t\b\n\r";

	::gpk::error_t					viewRead				(::gpk::vcs & headerToRead, const ::gpk::vcb & input);
	::gpk::error_t					viewLoad				(::gpk::vcs & headerToRead, const ::gpk::vcub & input);
	::gpk::error_t					rtrim					(::gpk::vcc & trimmed, const ::gpk::vcc & original, const ::gpk::vcc & characters = TRIM_CHARACTERS);
	::gpk::error_t					ltrim					(::gpk::vcc & trimmed, const ::gpk::vcc & original, const ::gpk::vcc & characters = TRIM_CHARACTERS);
	::gpk::error_t					trim					(::gpk::vcc & trimmed, const ::gpk::vcc & original, const ::gpk::vcc & characters = TRIM_CHARACTERS);
	stainli	::gpk::error_t	rtrim					(::gpk::vcc & trimmed) 	{ return rtrim(trimmed, trimmed); }
	stainli	::gpk::error_t	ltrim					(::gpk::vcc & trimmed) 	{ return ltrim(trimmed, trimmed); }
	stainli	::gpk::error_t	trim					(::gpk::vcc & trimmed) 	{ return trim(trimmed, trimmed); }


	template <typename _tCell>
	::gpk::error_t					reverse								(::gpk::view<_tCell> elements)																			{
		const uint32_t						lastElement							= elements.size() - 1;
		for(uint32_t i = 0, swapCount = elements.size() / 2; i < swapCount; ++i) {
			_tCell								old									= elements[i];
			elements[i]						= elements[lastElement - i];
			elements[lastElement - i]		= old;
		}
		return 0;
	}

	template<typename _tCell>
	::gpk::error_t					find					(const _tCell & valueToFind, const ::gpk::view<const _tCell> & target, uint32_t offset = 0)				{
		for(uint32_t iOffset = offset, offsetStop = target.size(); iOffset < offsetStop; ++iOffset)
			if(valueToFind == target[iOffset])
				return (int32_t)iOffset;
		return -1;
	}

	template<typename _tCell>
	::gpk::error_t					rfind					(const _tCell & valueToFind, const ::gpk::view<const _tCell> & target, int32_t offset = 0)				{
		for(uint32_t iOffset = target.size() - 1 - offset; iOffset < target.size(); --iOffset)
			if(valueToFind == target[iOffset])
				return iOffset;
		return -1;
	}

	template<typename _tCell>
	::gpk::error_t					find_sequence_obj		(const ::gpk::view<_tCell> & sequence, const ::gpk::view<_tCell> & target, uint32_t offset = 0)	{
		for(int32_t iOffset = (int32_t)offset, offsetStop = ((int32_t)target.size() - sequence.size()) + 1; iOffset < offsetStop; ++iOffset) {
			bool								equal					= true;
			for(uint32_t iSequenceElement = 0; iSequenceElement < sequence.size(); ++iSequenceElement) {
				if(sequence[iSequenceElement] != target[iOffset + iSequenceElement]) {
					equal							= false;
					break;
				}
			}
			if(equal)
				return iOffset;
		}
		return -1;
	}

	template<typename _tCell>
	::gpk::error_t					rfind_sequence_obj		(const ::gpk::view<_tCell> & sequence, const ::gpk::view<_tCell> & target, uint32_t offset = 0)	{
		for(int32_t iOffset = (int32_t)(target.size() - sequence.size() - offset); iOffset >= 0; --iOffset) {
			bool								equal					= true;
			for(uint32_t iSequenceElement = 0; iSequenceElement < sequence.size(); ++iSequenceElement) {
				if(sequence[iSequenceElement] != target[iOffset + iSequenceElement]) {
					equal							= false;
					break;
				}
			}
			if(equal)
				return iOffset;
		}
		return -1;
	}

	template<typename _tCell>
	::gpk::error_t					find_sequence_pod		(const ::gpk::view<_tCell> & sequence, const ::gpk::view<_tCell> & target, uint32_t offset = 0)	{
		for(int32_t iOffset = (int32_t)offset, offsetStop = ((int32_t)target.size() - sequence.size()) + 1; iOffset < offsetStop; ++iOffset)
			if(0 == memcmp(sequence.begin(), &target[iOffset], sequence.size() * sizeof(_tCell)))
				return iOffset;
		return -1;
	}

	template<typename _tCell>
	::gpk::error_t					rfind_sequence_pod		(const ::gpk::view<_tCell> & sequence, const ::gpk::view<_tCell>& target, uint32_t offset = 0)	{
		for(int32_t iOffset = (int32_t)(target.size() - sequence.size() - offset); iOffset >= 0; --iOffset)
			if(0 == memcmp(sequence.begin(), &target[iOffset], sequence.size() * sizeof(_tCell)))
				return iOffset;
		return -1;
	}

	stainli	::gpk::error_t			find_string				(const ::gpk::vcs & toFind, const ::gpk::vcc & target, uint32_t offset = 0) { return ::gpk::find_sequence_pod (toFind, target, offset); }
	stainli	::gpk::error_t			rfind_string			(const ::gpk::vcs & toFind, const ::gpk::vcc & target, uint32_t offset = 0) { return ::gpk::rfind_sequence_pod(toFind, target, offset); }

	template<typename _tCell>
	::gpk::error_t					split					(const _tCell & valueToFind, const ::gpk::view<_tCell> & original, ::gpk::view<_tCell> & left, ::gpk::view<_tCell> & right) {
		const ::gpk::error_t				iValue					= ::gpk::find(valueToFind, original);
		if(0 > iValue) {
			left							= original;
			right							= {};
		}
		else {
			gpk_necs(original.slice(left, 0, iValue));
			const uint32_t						offsetRight				= iValue + 1;
			gpk_necs(original.slice(right, offsetRight, original.size() - offsetRight));
		}
		return iValue;
	}

	template<typename _tCell>
	::gpk::error_t					splitAt					(const _tCell & valueToFind, const ::gpk::view<_tCell> & original, ::gpk::view<_tCell> & left, ::gpk::view<_tCell> & right) {
		const ::gpk::error_t				iValue					= ::gpk::find(valueToFind, original);
		if(0 > iValue) { // Read until the end unless fragment is found.
			left							= original;
			right							= {};
		}
		else {
			gpk_necall(original.slice(left, 0, (uint32_t)iValue), "%s", "Invalid slice");
			gpk_necall(original.slice(right, iValue, (uint32_t)original.size() - iValue), "%s", "Invalid slice");
		}
		return iValue;
	}

	template<typename _tCell>
	::gpk::error_t					split					(const ::gpk::view<_tCell> & sequenceToFind, const ::gpk::view<_tCell> & original, ::gpk::view<_tCell> & left, ::gpk::view<_tCell> & right) {
		const ::gpk::error_t				iValue					= ::gpk::find_sequence_pod(sequenceToFind, original);
		if(0 > iValue) {
			left							= original;
			right							= {};
		}
		else {
			gpk_necall(original.slice(left, 0, iValue), "%s", "Invalid slice");
			const uint32_t						offsetRight				= iValue + sequenceToFind.size();
			gpk_necall(original.slice(right, offsetRight, original.size() - offsetRight), "%s", "Invalid slice");
		}
		return iValue;
	}

	template<typename _tCell>
	::gpk::error_t					splitAt					(const ::gpk::view<_tCell> & sequenceToFind, const ::gpk::view<_tCell> & original, ::gpk::view<_tCell> & left, ::gpk::view<_tCell> & right) {
		const ::gpk::error_t				iValue					= ::gpk::find_sequence_pod(sequenceToFind, original);
		if(0 > iValue) { // Read until the end unless fragment is found.
			left							= original;
			right							= {};
		}
		else {
			gpk_necall(original.slice(left, 0, (uint32_t)iValue), "%s", "Invalid slice");
			gpk_necall(original.slice(right, iValue, (uint32_t)original.size() - iValue), "%s", "Invalid slice");
		}
		return iValue;
	}

	template<typename _tCell>
	::gpk::error_t					max						(::gpk::view<_tCell> input, _tCell ** result) {
		ree_if(0 == input.size(), "%s", "Cannot get reference to max element of an empty array.");
		*result							= &input[0];
		int32_t								iMax					= 0;
		for(uint32_t iElement = 1; iElement < input.size(); ++iElement) {
			const _tCell						& currentElement		= input[iElement];
			if(currentElement > **result) {
				*result							= &currentElement;
				iMax							= iElement;
			}
		}
		return iMax;
	}

	template<typename _tCell>
	::gpk::error_t					min						(::gpk::view<_tCell> input, _tCell ** result) {
		ree_if(0 == input.size(), "%s", "Cannot get reference to max element of an empty array.");
		*result							= &input[0];
		int32_t								iMin					= 0;
		for(uint32_t iElement = 1; iElement < input.size(); ++iElement) {
			const _tCell						& currentElement		= input[iElement];
			if(currentElement < **result) {
				*result							= &currentElement;
				iMin							= iElement;
			}
		}
		return iMin;
	}

	template<typename _tCell>
	_tCell&							max						(::gpk::view<_tCell> input) {
		_tCell								* result				= 0;
		gthrow_if(errored(::gpk::max(input, &result)), "%s", "");
		return *result;
	}

	template<typename _tCell>
	_tCell&							min						(::gpk::view<_tCell> input) {
		_tCell								* result				= 0;
		gthrow_if(errored(::gpk::min(input, &result)), "%s", "");
		return *result;
	}

#define be2le_16(number) ::gpk::reverse<ubyte_t>({(ubyte_t*)&number, 2})
#define be2le_32(number) ::gpk::reverse<ubyte_t>({(ubyte_t*)&number, 4})
#define be2le_64(number) ::gpk::reverse<ubyte_t>({(ubyte_t*)&number, 8})

	template<typename _tCell>
	::gpk::error_t					viewRead				(::gpk::view<const _tCell> & headerToRead, const ::gpk::vcb & input)	{
		ree_if(input.size() < 4, "Invalid input size: %u", input.size());
		const uint32_t						elementCount			= *(uint32_t*)input.begin();
		ree_if((elementCount * sizeof(_tCell)) > (input.size() - sizeof(uint32_t)), "Invalid input size: %u. Expected: %u", input.size(), elementCount * sizeof(_tCell));
		headerToRead					= {(input.size() > sizeof(uint32_t)) ? (const _tCell*)&input[sizeof(uint32_t)] : 0, elementCount};
		return sizeof(uint32_t) + headerToRead.size() * sizeof(_tCell);
	}

	template<typename _tCell>
	::gpk::error_t					viewRead				(::gpk::view<_tCell> & headerToRead, const ::gpk::vb & input)	{
		ree_if(input.size() < 4, "Invalid input size: %u", input.size());
		const uint32_t						elementCount			= *(uint32_t*)input.begin();
		ree_if((elementCount * sizeof(_tCell)) > (input.size() - sizeof(uint32_t)), "Invalid input size: %u. Expected: %u", input.size(), elementCount * sizeof(_tCell));
		headerToRead					= {(input.size() > sizeof(uint32_t)) ? (_tCell*)&input[sizeof(uint32_t)] : 0, elementCount};
		return sizeof(uint32_t) + headerToRead.size() * sizeof(_tCell);
	}

	template<typename _tCell>
	::gpk::error_t					viewLoad				(::gpk::view<const _tCell> & headerToRead, const ::gpk::vcub & input)	{
		ree_if(input.size() < 4, "Invalid input size: %u", input.size());
		const uint32_t						elementCount			= *(uint32_t*)input.begin();
		ree_if((elementCount * sizeof(_tCell)) > (input.size() - sizeof(uint32_t)), "Invalid input size: %u. Expected: %u", input.size(), elementCount * sizeof(_tCell));
		headerToRead					= {(input.size() > sizeof(uint32_t)) ? (const _tCell*)&input[sizeof(uint32_t)] : 0, elementCount};
		return sizeof(uint32_t) + headerToRead.size() * sizeof(_tCell);
	}

	template<typename _tCell>
	::gpk::error_t					viewLoad				(::gpk::view<_tCell> & headerToRead, const ::gpk::vub & input)	{
		ree_if(input.size() < 4, "Invalid input size: %u", input.size());
		const uint32_t						elementCount			= *(uint32_t*)input.begin();
		ree_if((elementCount * sizeof(_tCell)) > (input.size() - sizeof(uint32_t)), "Invalid input size: %u. Expected: %u", input.size(), elementCount * sizeof(_tCell));
		headerToRead					= {(input.size() > sizeof(uint32_t)) ? (_tCell*)&input[sizeof(uint32_t)] : 0, elementCount};
		return sizeof(uint32_t) + headerToRead.size() * sizeof(_tCell);
	}

} // namespace

#endif // GPK_ARRAY_VIEW_H_2398472395543
