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
	template <typename Val>
	class view {
	protected:
		// Properties / Member Variables
		Val						* Data			= 0;
		uint32_t				Count			= 0;
	public:
		typedef	Val				TElement;
		typedef	view<const Val>	TConstView;
		typedef	view<Val>		TView;

		// Constructors
		inlcxpr					view			()									noexcept	= default;

		template<size_t Len>
		inlcxpr					view			(Val (&elements)[Len])				noexcept	: Data(elements), Count(Len)										{}

		template<size_t Len>
		inlcxpr					view			(uint32_t elementCount, Val (&elements)[Len])	: Data(elements), Count(::gpk::min((uint32_t)Len, elementCount))	{}
		inline					view			(Val * elements, uint32_t elementCount)			: Data(elements), Count(elementCount)										{
			gthrow_if(0 == elements && 0 != elementCount, "Invalid parameters: %p, %u.", elements, elementCount);	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}

		template <size_t Len>
		inline					view			(Val (&elements)[Len], uint32_t elementCount)	: Data(elements), Count(::gpk::min((uint32_t)Len, elementCount))	{
			gthrow_if(elementCount > Len, "Element count out of range. Max count: %u. Requested: %u.", (uint32_t)Len, elementCount);
		}

		// Operators
		inlcxpr	operator		view<const Val>	()							const	noexcept	{ return {Data, Count}; }

		Val&					operator[]		(uint32_t index)								{
			gthrow_if(0 == Data, "%s", "Uninitialized array pointer.");
			gthrow_if(index >= Count, "Invalid index: %i.", index);
			return Data[index];
		}
		const Val&				operator[]		(uint32_t index)			const				{
			gthrow_if(0 == Data, "%s", "Uninitialized array pointer.");
			gthrow_if(index >= Count, "Invalid index: %i.", index);
			return Data[index];
		}
		inline	bool			operator!=		(const TConstView & other)	const				{ return !operator==(other); }
		bool					operator==		(const TConstView & other)	const				{
			if(this->size() != other.size())
				return false;
			if(this->begin() == other.begin())
				return true;
			return ::gpk::equal(other.begin(), this->begin(), this->size());
		}

		// Methods

		inlcxpr	const Val*		begin			()							const	noexcept	{ return Data;			}
		inlcxpr	const Val*		end				()							const	noexcept	{ return Data + Count;	}
		inlcxpr	const uint32_t&	size			()							const	noexcept	{ return Count;			}
		inlcxpr	const uint32_t	byte_count		()							const	noexcept	{ return (uint32_t)(Count * sizeof(Val));	}

		inline	Val*			begin			()									noexcept	{ return Data;			}
		inline	Val*			end				()									noexcept	{ return Data + Count;	}

		inline	::gpk::error_t	fill			(const Val & value, uint32_t offset = 0, uint32_t stop = 0xFFFFFFFFU)		{
			for(uint32_t i = offset; i < ::gpk::min(Count, stop); ++i)
				Data[i]					= value;
			return Count;
		}
		::gpk::error_t			slice			(TView & out, uint32_t offset, uint32_t count = (uint32_t)-1)				{
			ree_if(offset > Count, "Out of range. Max offset: %u. Requested: %u.", (uint32_t)Count, offset);
			const uint32_t				newSize			= Count - offset;
			if(count != (uint32_t)-1)
				ree_if(count > newSize, "Out of range. Max size: %u. Requested: %u.", (uint32_t)newSize, count);
			out						= {&Data[offset], ::gpk::min(newSize, count)};
			return out.size();
		}
		::gpk::error_t			slice			(TConstView & out, uint32_t offset, uint32_t count = (uint32_t)-1)	const	{
			ree_if(offset > Count, "Out of range. Max offset: %u. Requested: %u.", (uint32_t)Count, offset);
			const uint32_t				newSize			= Count - offset;
			if(count != (uint32_t)-1)
				ree_if(count > newSize, "Out of range. Max size: %u. Requested: %u.", (uint32_t)newSize, count);
			out						= {&Data[offset], ::gpk::min(newSize, count)};
			return out.size();
		}
		::gpk::error_t			revert			()																			{
			const uint32_t				lastElement		= Count - 1;
			for(uint32_t i = 0, swapCount = Count / 2; i < swapCount; ++i) {
				Val							old				= Data[i];
				Data[i]					= Data[lastElement - i];
				Data[lastElement - i]	= old;
			}
			return 0;
		}

		typedef std::function<void(Val&)>							TFuncForEach;
		typedef std::function<void(const Val&)>						TFuncForEachConst;
		typedef std::function<void(uint32_t & index, Val&)>			TFuncForEnumerate;
		typedef std::function<void(uint32_t & index, const Val&)>	TFuncForEnumerateConst;

		::gpk::error_t			for_each		(const TFuncForEach & funcForEach, uint32_t offset = 0)										{ 
			for(; offset < Count; ++offset)
				funcForEach(Data[offset]);
			return offset;
		}
		::gpk::error_t			for_each		(const TFuncForEachConst & funcForEach, uint32_t offset = 0)					const		{ 
			for(; offset < Count; ++offset)
				funcForEach(Data[offset]);
			return offset;
		}
		::gpk::error_t			enumerate		(const TFuncForEnumerate & funcForEach, uint32_t offset = 0)								{ 
			for(; offset < Count; ++offset)
				funcForEach(offset, Data[offset]);
			return offset;
		}
		::gpk::error_t			enumerate		(const TFuncForEnumerateConst & funcForEach, uint32_t offset = 0)				const		{ 
			for(; offset < Count; ++offset)
				funcForEach(offset, Data[offset]);
			return offset;
		}
		// 
		::gpk::error_t			for_each		(const TFuncForEach & funcForEach, uint32_t offset, uint32_t stop)							{ 
			for(stop = ::gpk::min(stop, Count); offset < stop; ++offset)
				funcForEach(Data[offset]);
			return offset;
		}
		::gpk::error_t			for_each		(const TFuncForEachConst & funcForEach, uint32_t offset, uint32_t stop)			const		{ 
			for(stop = ::gpk::min(stop, Count); offset < stop; ++offset)
				funcForEach(Data[offset]);
			return offset;
		}
		::gpk::error_t			enumerate		(const TFuncForEnumerate & funcForEach, uint32_t offset, uint32_t stop)						{ 
			for(stop = ::gpk::min(stop, Count); offset < stop; ++offset)
				funcForEach(offset, Data[offset]);
			return offset;
		}
		::gpk::error_t			enumerate		(const TFuncForEnumerateConst & funcForEach, uint32_t offset, uint32_t stop)	const		{ 
			for(stop = ::gpk::min(stop, Count); offset < stop; ++offset)
				funcForEach(offset, Data[offset]);
			return offset;
		}
	}; // view<>

	template<typename Val>	using	view_array	= ::gpk::view<Val>;
	template<typename Val>	using	view1d		= ::gpk::view<Val>;
	template<typename Val>	using	v1			= ::gpk::view<Val>;

	template<typename Val>	inlcxpr	uint32_t	size		(const ::gpk::view<Val> & viewToTest)	noexcept	{ return viewToTest.size();			}
	template<typename Val>	inlcxpr	uint32_t	byte_count	(const ::gpk::view<Val> & viewToTest)	noexcept	{ return viewToTest.byte_count();	}

#pragma pack(pop)

	typedef	::gpk::view<::gpk::uchar_t		>	view_uchar			, v1uc, vuc;
	typedef	::gpk::view<char				>	view_char			, v1c, vc;
	typedef	::gpk::view<float				>	view_float32		, v1f32, v1f, vf32, vf;
	typedef	::gpk::view<double				>	view_float64		, v1f64, v1d, vf64, vd;
	typedef	::gpk::view<uint8_t				>	view_uint8			, v1u8, vu8;
	typedef	::gpk::view<uint16_t			>	view_uint16			, v1u16, vu16;
	typedef	::gpk::view<uint32_t			>	view_uint32			, v1u32, vu32;
	typedef	::gpk::view<uint64_t			>	view_uint64			, v1u64, vu64;
	typedef	::gpk::view<int8_t				>	view_int8			, v1i8, vi8;
	typedef	::gpk::view<int16_t				>	view_int16			, v1i16, vi16;
	typedef	::gpk::view<int32_t				>	view_int32			, v1i32, vi32;
	typedef	::gpk::view<int64_t				>	view_int64			, v1i64, vi64;
	typedef	::gpk::view<const ::gpk::uchar_t>	view_const_uchar	, v1cuc, vcuc;
	typedef	::gpk::view<const char			>	view_const_char		, v1cc, vcc;
	typedef	::gpk::view<const float			>	view_const_float32	, v1cf32, v1cf, vcf32, vcf;
	typedef	::gpk::view<const double		>	view_const_float64	, v1cf64, v1cd, vcf64, vcd;
	typedef	::gpk::view<const uint8_t		>	view_const_uint8	, v1cu8, vcu8;
	typedef	::gpk::view<const uint16_t		>	view_const_uint16	, v1cu16, vcu16;
	typedef	::gpk::view<const uint32_t		>	view_const_uint32	, v1cu32, vcu32;
	typedef	::gpk::view<const uint64_t		>	view_const_uint64	, v1cu64, vcu64;
	typedef	::gpk::view<const int8_t		>	view_const_int8		, v1ci8, vci8;
	typedef	::gpk::view<const int16_t		>	view_const_int16	, v1ci16, vci16;
	typedef	::gpk::view<const int32_t		>	view_const_int32	, v1ci32, vci32;
	typedef	::gpk::view<const int64_t		>	view_const_int64	, v1ci64, vci64;

	typedef	::gpk::view<::gpk::vuc	>	vvuc;
	typedef	::gpk::view<::gpk::vc	>	vvc;
	typedef	::gpk::view<::gpk::v1f	>	vv1f;
	typedef	::gpk::view<::gpk::v1d	>	vv1d;
	typedef	::gpk::view<::gpk::vu8	>	vvu8;
	typedef	::gpk::view<::gpk::vu16	>	vvu16;
	typedef	::gpk::view<::gpk::vu32	>	vvu32;
	typedef	::gpk::view<::gpk::vu64	>	vvu64;
	typedef	::gpk::view<::gpk::vi8	>	vvi8;
	typedef	::gpk::view<::gpk::vi16	>	vvi16;
	typedef	::gpk::view<::gpk::vi32	>	vvi32;
	typedef	::gpk::view<::gpk::vi64	>	vvi64;
	typedef	::gpk::view<const vcuc	>	vcvcuc;
	typedef	::gpk::view<const vcc	>	vcvcc;
	typedef	::gpk::view<const v1cf	>	vcv1cf;
	typedef	::gpk::view<const v1cd	>	vcv1cd;
	typedef	::gpk::view<const vcu8	>	vcvcu8;
	typedef	::gpk::view<const vcu16	>	vcvcu16;
	typedef	::gpk::view<const vcu32	>	vcvcu32;
	typedef	::gpk::view<const vcu64	>	vcvcu64;
	typedef	::gpk::view<const vci8	>	vcvci8;
	typedef	::gpk::view<const vci16	>	vcvci16;
	typedef	::gpk::view<const vci32	>	vcvci32;
	typedef	::gpk::view<const vci64	>	vcvci64;

	stacxpr	::gpk::vcc		STR_NULL		= {4, "null"};
	stacxpr	::gpk::vcc		STR_TRUE		= {4, "true"};
	stacxpr	::gpk::vcc		STR_FALSE		= {5, "false"};

	struct view_string : public view<char> {
		inlcxpr				view_string				()														= default;
		inlcxpr				view_string				(const view_char & other)					noexcept	: view(other)				{}
							view_string				(char * inputString, uint32_t length)					: view(inputString, length)	{ Count = (length == (uint32_t)-1) ? (uint32_t)strlen(inputString) : length;					}
		template<u32 Len>	view_string				(char (&inputString)[Len])					noexcept	: view(inputString)			{ Count = (uint32_t)strnlen(inputString, (uint32_t)Len);								}
		template<u32 Len>	view_string				(char (&inputString)[Len], uint32_t length)				: view(inputString, length)	{ if(length == (uint32_t)-1) Count = (uint32_t)strnlen(inputString, (uint32_t)Len);	}
	};

	struct view_const_string : public view<const char> {
		inlcxpr				view_const_string		()														= default;
		inlcxpr				view_const_string		(const view_const_char & other)				noexcept	: view(other)				{}
							view_const_string		(const char* inputString, uint32_t length)				: view(inputString, length)	{ Count = (length == (uint32_t)-1) ? (uint32_t)strlen(inputString) : length;					}
		template<u32 Len>	view_const_string		(const char (&inputString)[Len])			noexcept	: view(inputString)			{ Count = (uint32_t)strnlen(inputString, (uint32_t)Len);								}
		template<u32 Len>	view_const_string		(const char (&inputString)[Len], uint32_t length)		: view(inputString, length)	{ if(length == (uint32_t)-1) Count = (uint32_t)strnlen(inputString, (uint32_t)Len);	}
	};

	typedef	::gpk::view_string					vs;
	typedef	::gpk::view_const_string			vcs;

	typedef	::gpk::view<::gpk::vs			>	vvs;
	typedef	::gpk::view<::gpk::vcs			>	vvcs;
	typedef	::gpk::view<const ::gpk::vs		>	vcvs;
	typedef	::gpk::view<const ::gpk::vcs	>	vcvcs;

	cnstxpr ::gpk::vcc		TRIM_CHARACTERS			= " \t\b\n\r";
							
	::gpk::error_t			rtrim					(::gpk::vcc & trimmed, const ::gpk::vcc & original, const ::gpk::vcc & characters = ::gpk::TRIM_CHARACTERS);
	::gpk::error_t			ltrim					(::gpk::vcc & trimmed, const ::gpk::vcc & original, const ::gpk::vcc & characters = ::gpk::TRIM_CHARACTERS);
	::gpk::error_t			trim					(::gpk::vcc & trimmed, const ::gpk::vcc & original, const ::gpk::vcc & characters = ::gpk::TRIM_CHARACTERS);
	stainli	::gpk::error_t	rtrim					(::gpk::vcc & trimmed) 	{ return rtrim(trimmed, trimmed); }
	stainli	::gpk::error_t	ltrim					(::gpk::vcc & trimmed) 	{ return ltrim(trimmed, trimmed); }
	stainli	::gpk::error_t	trim					(::gpk::vcc & trimmed) 	{ return trim(trimmed, trimmed); }


	template <typename Val>
	::gpk::error_t					reverse								(::gpk::view<Val> elements)																			{
		const uint32_t						lastElement							= elements.size() - 1;
		for(uint32_t i = 0, swapCount = elements.size() / 2; i < swapCount; ++i) {
			Val								old									= elements[i];
			elements[i]						= elements[lastElement - i];
			elements[lastElement - i]		= old;
		}
		return 0;
	}

	template<typename Val>
	::gpk::error_t					find					(const Val & valueToFind, const ::gpk::view<const Val> & target, uint32_t offset = 0)				{
		for(uint32_t iOffset = offset, offsetStop = target.size(); iOffset < offsetStop; ++iOffset)
			if(valueToFind == target[iOffset])
				return (int32_t)iOffset;
		return -1;
	}

	template<typename Val>
	::gpk::error_t					rfind					(const Val & valueToFind, const ::gpk::view<const Val> & target, int32_t offset = 0)				{
		for(uint32_t iOffset = target.size() - 1 - offset; iOffset < target.size(); --iOffset)
			if(valueToFind == target[iOffset])
				return iOffset;
		return -1;
	}

	template<typename Val>
	::gpk::error_t					find_sequence_obj		(const ::gpk::view<Val> & sequence, const ::gpk::view<Val> & target, uint32_t offset = 0)	{
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

	template<typename Val>
	::gpk::error_t					rfind_sequence_obj		(const ::gpk::view<Val> & sequence, const ::gpk::view<Val> & target, uint32_t offset = 0)	{
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

	template<typename Val>
	::gpk::error_t					find_sequence_pod		(const ::gpk::view<Val> & sequence, const ::gpk::view<Val> & target, uint32_t offset = 0)	{
		for(int32_t iOffset = (int32_t)offset, offsetStop = ((int32_t)target.size() - sequence.size()) + 1; iOffset < offsetStop; ++iOffset)
			if(0 == memcmp(sequence.begin(), &target[iOffset], sequence.size() * sizeof(Val)))
				return iOffset;
		return -1;
	}

	template<typename Val>
	::gpk::error_t					rfind_sequence_pod		(const ::gpk::view<Val> & sequence, const ::gpk::view<Val>& target, uint32_t offset = 0)	{
		for(int32_t iOffset = (int32_t)(target.size() - sequence.size() - offset); iOffset >= 0; --iOffset)
			if(0 == memcmp(sequence.begin(), &target[iOffset], sequence.size() * sizeof(Val)))
				return iOffset;
		return -1;
	}

	stainli	::gpk::error_t			find_string				(const ::gpk::vcs & toFind, const ::gpk::vcc & target, uint32_t offset = 0) { return ::gpk::find_sequence_pod (toFind, target, offset); }
	stainli	::gpk::error_t			rfind_string			(const ::gpk::vcs & toFind, const ::gpk::vcc & target, uint32_t offset = 0) { return ::gpk::rfind_sequence_pod(toFind, target, offset); }

	template<typename Val>
	::gpk::error_t					split					(const Val & valueToFind, const ::gpk::view<Val> & original, ::gpk::view<Val> & left, ::gpk::view<Val> & right) {
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

	template<typename Val>
	::gpk::error_t					splitAt					(const Val & valueToFind, const ::gpk::view<Val> & original, ::gpk::view<Val> & left, ::gpk::view<Val> & right) {
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

	template<typename Val>
	::gpk::error_t					split					(const ::gpk::view<Val> & sequenceToFind, const ::gpk::view<Val> & original, ::gpk::view<Val> & left, ::gpk::view<Val> & right) {
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

	template<typename Val>
	::gpk::error_t					splitAt					(const ::gpk::view<Val> & sequenceToFind, const ::gpk::view<Val> & original, ::gpk::view<Val> & left, ::gpk::view<Val> & right) {
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

	template<typename Val>
	::gpk::error_t					max						(::gpk::view<Val> input, Val ** result) {
		ree_if(0 == input.size(), "%s", "Cannot get reference to max element of an empty array.");
		*result							= &input[0];
		int32_t								iMax					= 0;
		for(uint32_t iElement = 1; iElement < input.size(); ++iElement) {
			const Val						& currentElement		= input[iElement];
			if(currentElement > **result) {
				*result							= &currentElement;
				iMax							= iElement;
			}
		}
		return iMax;
	}

	template<typename Val>
	::gpk::error_t					min						(::gpk::view<Val> input, Val ** result) {
		ree_if(0 == input.size(), "%s", "Cannot get reference to max element of an empty array.");
		*result							= &input[0];
		int32_t								iMin					= 0;
		for(uint32_t iElement = 1; iElement < input.size(); ++iElement) {
			const Val						& currentElement		= input[iElement];
			if(currentElement < **result) {
				*result							= &currentElement;
				iMin							= iElement;
			}
		}
		return iMin;
	}

	template<typename Val>
	Val&							max						(::gpk::view<Val> input) {
		Val								* result				= 0;
		gthrow_if(errored(::gpk::max(input, &result)), "%s", "");
		return *result;
	}

	template<typename Val>
	Val&							min						(::gpk::view<Val> input) {
		Val								* result				= 0;
		gthrow_if(errored(::gpk::min(input, &result)), "%s", "");
		return *result;
	}

#define be2le_16(number) ::gpk::reverse<uint8_t>({(uint8_t*)&number, 2})
#define be2le_32(number) ::gpk::reverse<uint8_t>({(uint8_t*)&number, 4})
#define be2le_64(number) ::gpk::reverse<uint8_t>({(uint8_t*)&number, 8})

	template<typename Val>
	::gpk::error_t					viewRead				(::gpk::view<const Val> & headerToRead, const ::gpk::vcu8 & input)	{
		ree_if(input.size() < 4, "Invalid input size: %u", input.size());
		const uint32_t						elementCount			= *(const uint32_t*)input.begin();
		ree_if((elementCount * sizeof(Val)) > (input.size() - sizeof(uint32_t)), "Invalid input size: %u. Expected: %u", input.size(), elementCount * sizeof(Val));
		headerToRead					= {(input.size() > sizeof(uint32_t)) ? (const Val*)&input[sizeof(uint32_t)] : 0, elementCount};
		return sizeof(uint32_t) + headerToRead.size() * sizeof(Val);
	}

	template<typename Val>
	::gpk::error_t					viewRead				(::gpk::view<Val> & headerToRead, const ::gpk::vu8 & input)	{
		ree_if(input.size() < 4, "Invalid input size: %u", input.size());
		const uint32_t						elementCount			= *(const uint32_t*)input.begin();
		ree_if((elementCount * sizeof(Val)) > (input.size() - sizeof(uint32_t)), "Invalid input size: %u. Expected: %u", input.size(), elementCount * sizeof(Val));
		headerToRead					= {(input.size() > sizeof(uint32_t)) ? (Val*)&input[sizeof(uint32_t)] : 0, elementCount};
		return sizeof(uint32_t) + headerToRead.size() * sizeof(Val);
	}

	template<typename Val>	::gpk::error_t	viewRead	(::gpk::view<const Val> & headerToRead, const ::gpk::vci8 & input)	{ return viewRead(headerToRead, *(const ::gpk::vcu8*)&input); }
	template<typename Val>	::gpk::error_t	viewRead	(::gpk::view<const Val> & headerToRead, const ::gpk::vcc  & input)	{ return viewRead(headerToRead, *(const ::gpk::vcu8*)&input); }
	template<typename Val>	::gpk::error_t	viewRead	(::gpk::view<Val> & headerToRead, const ::gpk::vi8 & input)			{ return viewRead(headerToRead, *(const ::gpk::vu8*)&input); }
	template<typename Val>	::gpk::error_t	viewRead	(::gpk::view<Val> & headerToRead, const ::gpk::vc  & input)			{ return viewRead(headerToRead, *(const ::gpk::vu8*)&input); }

	template<typename _tPOD> 
	::gpk::error_t					loadPOD				(::gpk::vcu8 & input, _tPOD & output) { 
		::gpk::view<const _tPOD>			readView			= {}; 
		uint32_t							bytesRead			= 0;
		gpk_necs(bytesRead = ::gpk::viewRead(readView, input)); 
		input							= {input.begin() + bytesRead, input.size() - bytesRead}; 
		output							= readView[0]; 
		return 0;
	}
	template<typename _tPOD> ::gpk::error_t	loadPOD		(::gpk::vci8 & input, _tPOD & output) { return loadPOD (*(::gpk::vcu8*)& input, output); }
	template<typename _tPOD> ::gpk::error_t	loadPOD		(::gpk::vcc  & input, _tPOD & output) { return loadPOD (*(::gpk::vcu8*)& input, output); }

	template<typename _tPOD> 
	::gpk::error_t					loadView			(::gpk::vcu8 & input, ::gpk::view<const _tPOD> & output) { 
		uint32_t							bytesRead				= 0;
		gpk_necs(bytesRead = ::gpk::viewRead(output, input)); 
		input							= {input.begin() + bytesRead, input.size() - bytesRead}; 
		return 0;
	}
	template<typename _tPOD> ::gpk::error_t	loadView	(::gpk::vci8 & input, ::gpk::view<_tPOD> & output) { return loadView(*(::gpk::vcu8*)& input, output); }
	template<typename _tPOD> ::gpk::error_t	loadView	(::gpk::vcc  & input, ::gpk::view<_tPOD> & output) { return loadView(*(::gpk::vcu8*)& input, output); }
} // namespace

#endif // GPK_ARRAY_VIEW_H_2398472395543
