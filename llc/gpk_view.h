#include "gpk_log.h"

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
	template <typename _tElement>
	class view {
	protected:
		// Properties / Member Variables
							_tElement				* Data						= 0;
							uint32_t				Count						= 0;
	public:
		typedef				_tElement				TElement;
		typedef				view<const _tElement>	TConstView;
		typedef				view<_tElement>			TView;

		// Constructors
		inline constexpr							view					()																		noexcept	= default;
		inline										view					(_tElement* elements, uint32_t elementCount)										: Data(elements), Count(elementCount)										{
			gthrow_if(0 == elements && 0 != elementCount, "Invalid parameters: %p, %u.", elements, elementCount);	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}

		template <size_t _elementCount>
		inline constexpr							view					(_tElement (&_elements)[_elementCount])									noexcept	: Data(_elements), Count(_elementCount)										{}

		template <size_t _elementCount>
		inline										view					(_tElement (&_elements)[_elementCount], uint32_t elementCount)						: Data(_elements), Count(::gpk::min((uint32_t)_elementCount, elementCount))	{
			gthrow_if(elementCount > _elementCount, "Element count out of range. Max count: %u. Requested: %u.", (uint32_t)_elementCount, elementCount);
		}

		template <size_t _elementCount>
		inline constexpr							view					(uint32_t elementCount, _tElement (&_elements)[_elementCount])						: Data(_elements), Count(::gpk::min((uint32_t)_elementCount, elementCount))	{}

		// Operators
		inline constexpr	operator				view<const _tElement>	()																const	noexcept	{ return {Data, Count}; }
							_tElement&				operator[]				(uint32_t index)																	{
			gthrow_if(0 == Data, "%s", "Uninitialized array pointer.");
			gthrow_if(index >= Count, "Invalid index: %i.", index);
			return Data[index];
		}
							const _tElement&		operator[]				(uint32_t index)												const				{
			gthrow_if(0 == Data, "%s", "Uninitialized array pointer.");
			gthrow_if(index >= Count, "Invalid index: %i.", index);
			return Data[index];
		}
							bool					operator				!=(const TConstView& other)										const				{ return !operator==(other); }
							bool					operator				==(const TConstView& other)										const				{
			if(this->size() != other.size())
				return false;
			if(this->begin() == other.begin())
				return true;
			return ::gpk::equal(other.begin(), this->begin(), this->size());
		}

		// Methods
		inline				_tElement*				begin					()																		noexcept	{ return Data;			}
		inline				_tElement*				end						()																		noexcept	{ return Data + Count;	}

		inline constexpr	const _tElement*		begin					()																const	noexcept	{ return Data;			}
		inline constexpr	const _tElement*		end						()																const	noexcept	{ return Data + Count;	}
		inline constexpr	const uint32_t&			size					()																const	noexcept	{ return Count;			}
		inline constexpr	const uint32_t			byte_count				()																const	noexcept	{ return (uint32_t)(Count * sizeof(_tElement));	}

		inline				::gpk::error_t			fill					(const _tElement& value, uint32_t offset = 0, uint32_t count = 0xFFFFFFFFU) {
			for(uint32_t i = offset; i < ::gpk::min(Count, count); ++i)
				Data[i]									= value;
			return 0;
		}

							gpk::error_t			slice					(TView & out, uint32_t offset, uint32_t count = (uint32_t)-1)	const				{
			ree_if(offset > Count, "Out of range. Max offset: %u. Requested: %u.", (uint32_t)Count, offset);
			const uint32_t									newSize						= Count - offset;
			if(count != (uint32_t)-1)
				ree_if(count > newSize, "Out of range. Max size: %u. Requested: %u.", (uint32_t)newSize, count);
			out = {&Data[offset], ::gpk::min(newSize, count)};
			return out.size();
		}
	};

	template<typename _tCell>	using view_array		= ::gpk::view<_tCell>;
	template<typename _tCell>	using view1d			= ::gpk::view<_tCell>;
	template<typename _tCell>	using v1				= ::gpk::view<_tCell>;

#pragma pack(pop)

	// view_array common typedefs
	//typedef				::gpk::view_array<char_t			>	view_string			;
	typedef				::gpk::view_array<ubyte_t			>	view_ubyte			, v1ub, vub;
	typedef				::gpk::view_array<byte_t			>	view_byte			, v1b, vb;
	typedef				::gpk::view_array<uchar_t			>	view_uchar			, v1uc, vuc;
	typedef				::gpk::view_array<char_t			>	view_char			, v1c, vc;
	typedef				::gpk::view_array<float				>	view_float32		, v1f32, v1f, vf32, vf;
	typedef				::gpk::view_array<double			>	view_float64		, v1f64, v1d, vf64, vd;
	typedef				::gpk::view_array<uint8_t			>	view_uint8			, v1u8, vu8;
	typedef				::gpk::view_array<uint16_t			>	view_uint16			, v1u16, vu16;
	typedef				::gpk::view_array<uint32_t			>	view_uint32			, v1u32, vu32;
	typedef				::gpk::view_array<uint64_t			>	view_uint64			, v1u64, vu64;
	typedef				::gpk::view_array<int8_t			>	view_int8			, v1i8, vi8;
	typedef				::gpk::view_array<int16_t			>	view_int16			, v1i16, vi16;
	typedef				::gpk::view_array<int32_t			>	view_int32			, v1i32, vi32;
	typedef				::gpk::view_array<int64_t			>	view_int64			, v1i64, vi64;

	// view_array<const> common typedefs
	//typedef				::gpk::view_array<const char_t		>	view_const_string	;
	typedef				::gpk::view_array<const ubyte_t		>	view_const_ubyte	, v1cub, vcub;
	typedef				::gpk::view_array<const byte_t		>	view_const_byte		, v1cb, vcb;
	typedef				::gpk::view_array<const uchar_t		>	view_const_uchar	, v1cuc, vcuc;
	typedef				::gpk::view_array<const char_t		>	view_const_char		, v1cc, vcc;
	typedef				::gpk::view_array<const float		>	view_const_float32	, v1cf32, v1cf, vcf32, vcf;
	typedef				::gpk::view_array<const double		>	view_const_float64	, v1cf64, v1cd, vcf64, vcd;
	typedef				::gpk::view_array<const uint8_t		>	view_const_uint8	, v1cu8, vcu8;
	typedef				::gpk::view_array<const uint16_t	>	view_const_uint16	, v1cu16, vcu16;
	typedef				::gpk::view_array<const uint32_t	>	view_const_uint32	, v1cu32, vcu32;
	typedef				::gpk::view_array<const uint64_t	>	view_const_uint64	, v1cu64, vcu64;
	typedef				::gpk::view_array<const int8_t		>	view_const_int8		, v1ci8, vci8;
	typedef				::gpk::view_array<const int16_t		>	view_const_int16	, v1ci16, vci16;
	typedef				::gpk::view_array<const int32_t		>	view_const_int32	, v1ci32, vci32;
	typedef				::gpk::view_array<const int64_t		>	view_const_int64	, v1ci64, vci64;

	struct view_const_string : public view<const char_t> {
		inline constexpr							view_const_string					()																= default;
		inline constexpr							view_const_string					(const view_const_char & other)									: view(other)							{}
													view_const_string					(const char* inputString, uint32_t length)						: view(inputString, length)			{ Count = (length == (uint32_t)-1) ? (uint32_t)strlen(inputString) : length;																}
		template<size_t _stringLength>				view_const_string					(const char (&inputString)[_stringLength], uint32_t length)		: view(inputString, length)			{ Count = (length == (uint32_t)-1) ? (uint32_t)strnlen(inputString, (uint32_t)_stringLength) : ::gpk::min(length, (uint32_t)_stringLength);	}
		template<size_t _stringLength>				view_const_string					(const char (&inputString)[_stringLength])						: view(inputString, _stringLength)	{ Count = (uint32_t)strnlen(inputString, (uint32_t)_stringLength);																			}
	};

	typedef view_const_string vcs;

	struct view_string : public view_const_string {
		inline constexpr							view_string							()																= default;
		inline constexpr							view_string							(const view_char & other)										: view_const_string(other)						{}
													view_string							(char* inputString, uint32_t length)							: view_const_string(inputString, length)		{}
		template<size_t _stringLength>				view_string							(char (&inputString)[_stringLength], uint32_t length)			: view_const_string(inputString, length)		{}
		template<size_t _stringLength>				view_string							(char (&inputString)[_stringLength])							: view_const_string(inputString)				{}
		//constexpr		operator					const view_const_char&			()															const	noexcept	{ return *(const view_const_string*)this; }
	};

	template <typename _tCell>
						::gpk::error_t			reverse								(::gpk::view<_tCell> elements)																			{
		for(uint32_t i = 0, swapCount = elements.size() / 2; i < swapCount; ++i) {
			_tCell											old									= elements[i];
			elements[i]									= elements[elements.size() - 1 - i];
			elements[elements.size() - 1 - i]			= old;
		}
		return 0;
	}

	template <typename _t>	static inline constexpr uint32_t	size		(::gpk::view<_t> viewToTest)	noexcept	{ return viewToTest.size();					}
	template <typename _t>	static inline constexpr uint32_t	byte_count	(::gpk::view<_t> viewToTest)	noexcept	{ return viewToTest.byte_count();	}

						::gpk::error_t			rtrim								(::gpk::vcc & trimmed, const ::gpk::vcc & original, const ::gpk::vcc & characters = " \t\b\n\r");
						::gpk::error_t			ltrim								(::gpk::vcc & trimmed, const ::gpk::vcc & original, const ::gpk::vcc & characters = " \t\b\n\r");
	static inline		::gpk::error_t			trim								(::gpk::vcc & trimmed, const ::gpk::vcc & original, const ::gpk::vcc & characters = " \t\b\n\r") 	{
		const uint32_t countChars = ::gpk::ltrim(trimmed, original, characters);
		return countChars + ::gpk::rtrim(trimmed, trimmed, characters);
	}
	static inline		::gpk::error_t			trim								(::gpk::vcc & trimmed) 	{
		return trim(trimmed, trimmed, " \t\b\n\r");
	}

	template<typename _tElement>
						::gpk::error_t			find								(const _tElement& element, const ::gpk::view<const _tElement>& target, uint32_t offset = 0)				{
		for(uint32_t iOffset = offset, offsetStop = target.size(); iOffset < offsetStop; ++iOffset)
			if(element == target[iOffset])
				return (int32_t)iOffset;
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t			rfind								(const _tElement& element, const ::gpk::view<const _tElement>& target, int32_t offset = 0)				{
		for(int32_t iOffset = target.size() - 1 - offset; iOffset >= 0; --iOffset)
			if(element == target[iOffset])
				return iOffset;
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t			find_sequence_obj					(const ::gpk::view<_tElement>& sequence, const ::gpk::view<_tElement>& target, uint32_t offset = 0)	{
		for(int32_t iOffset = (int32_t)offset, offsetStop = ((int32_t)target.size() - sequence.size()) + 1; iOffset < offsetStop; ++iOffset) {
			bool											equal								= true;
			for(uint32_t iSequenceElement = 0; iSequenceElement < sequence.size(); ++iSequenceElement) {
				if(sequence[iSequenceElement] != target[iOffset + iSequenceElement]) {
					equal									= false;
					break;
				}
			}
			if(equal)
				return iOffset;
		}
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t			rfind_sequence_obj					(const ::gpk::view<_tElement>& sequence, const ::gpk::view<_tElement>& target, uint32_t offset = 0)	{
		for(int32_t iOffset = (int32_t)(target.size() - sequence.size() - offset); iOffset >= 0; --iOffset) {
			bool											equal								= true;
			for(uint32_t iSequenceElement = 0; iSequenceElement < sequence.size(); ++iSequenceElement) {
				if(sequence[iSequenceElement] != target[iOffset + iSequenceElement]) {
					equal									= false;
					break;
				}
			}
			if(equal)
				return iOffset;
		}
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t			find_sequence_pod					(const ::gpk::view<_tElement>& sequence, const ::gpk::view<_tElement>& target, uint32_t offset = 0)	{
		for(int32_t iOffset = (int32_t)offset, offsetStop = ((int32_t)target.size() - sequence.size()) + 1; iOffset < offsetStop; ++iOffset)
			if(0 == memcmp(sequence.begin(), &target[iOffset], sequence.size() * sizeof(_tElement)))
				return iOffset;
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t			rfind_sequence_pod					(const ::gpk::view<_tElement>& sequence, const ::gpk::view<_tElement>& target, uint32_t offset = 0)	{
		for(int32_t iOffset = (int32_t)(target.size() - sequence.size() - offset); iOffset >= 0; --iOffset)
			if(0 == memcmp(sequence.begin(), &target[iOffset], sequence.size() * sizeof(_tElement)))
				return iOffset;
		return -1;
	}

	static inline		::gpk::error_t			find_string							(const ::gpk::vcs& sequence, const ::gpk::vcc & target, uint32_t offset = 0) { return ::gpk::find_sequence_pod(sequence, target, offset); }
	static inline		::gpk::error_t			rfind_string						(const ::gpk::vcs& sequence, const ::gpk::vcc & target, uint32_t offset = 0) { return ::gpk::rfind_sequence_pod(sequence, target, offset); }

	template<typename _tElement>
	static inline		::gpk::error_t			is_any_of							(const ::gpk::view<const _tElement>& valuesToFind, const _tElement & valueToTest)	{
		for(uint32_t iCharacter = 0; iCharacter < valuesToFind.size(); ++iCharacter)
			if(valueToTest == valuesToFind[iCharacter])
				return iCharacter;
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t			split								(const _tElement& valueToFind, const ::gpk::view<_tElement> & original, ::gpk::view<_tElement> & left, ::gpk::view<_tElement> & right) {
		const ::gpk::error_t							iValue								= ::gpk::find(valueToFind, original);
		if(0 > iValue) {
			left										= original;
			right										= {};
		}
		else {
			gpk_necall(original.slice(left, 0, iValue), "%s", "Invalid slice");
			const uint32_t									offsetRight							= iValue + 1;
			gpk_necall(original.slice(right, offsetRight, original.size() - offsetRight), "%s", "Invalid slice");
		}
		return iValue;
	}

	template<typename _tElement>
						::gpk::error_t			splitAt								(const _tElement& valueToFind, const ::gpk::view<_tElement> & original, ::gpk::view<_tElement> & left, ::gpk::view<_tElement> & right) {
		const ::gpk::error_t							iValue								= ::gpk::find(valueToFind, original);
		if(0 > iValue) { // Read until the end unless fragment is found.
			left										= original;
			right										= {};
		}
		else {
			gpk_necall(original.slice(left, 0, (uint32_t)iValue), "%s", "Invalid slice");
			gpk_necall(original.slice(right, iValue, (uint32_t)original.size() - iValue), "%s", "Invalid slice");
		}
		return iValue;
	}

	template<typename _tElement>
						::gpk::error_t			split								(const ::gpk::view<_tElement>& sequenceToFind, const ::gpk::view<_tElement> & original, ::gpk::view<_tElement> & left, ::gpk::view<_tElement> & right) {
		const ::gpk::error_t							iValue								= ::gpk::find_sequence_pod(sequenceToFind, original);
		if(0 > iValue) {
			left										= original;
			right										= {};
		}
		else {
			gpk_necall(original.slice(left, 0, iValue), "%s", "Invalid slice");
			const uint32_t													offsetRight							= iValue + sequenceToFind.size();
			gpk_necall(original.slice(right, offsetRight, original.size() - offsetRight), "%s", "Invalid slice");
		}
		return iValue;
	}

	template<typename _tElement>
						::gpk::error_t			splitAt								(const ::gpk::view<_tElement>& sequenceToFind, const ::gpk::view<_tElement> & original, ::gpk::view<_tElement> & left, ::gpk::view<_tElement> & right) {
		const ::gpk::error_t							iValue								= ::gpk::find_sequence_pod(sequenceToFind, original);
		if(0 > iValue) { // Read until the end unless fragment is found.
			left										= original;
			right										= {};
		}
		else {
			gpk_necall(original.slice(left, 0, (uint32_t)iValue), "%s", "Invalid slice");
			gpk_necall(original.slice(right, iValue, (uint32_t)original.size() - iValue), "%s", "Invalid slice");
		}
		return iValue;
	}

	template<typename _tElement>
						::gpk::error_t			max									(const ::gpk::view<const _tElement> & input, const _tElement ** result) {
		if(0 == input.size())
			return -1;
		*result										= &input[0];
		int32_t											iMax								= 0;
		for(uint32_t iElement = 1; iElement < input.size(); ++iElement) {
			const _tElement									& currentElement					= input[iElement];
			if(currentElement > **result) {
				*result										= &currentElement;
				iMax										= iElement;
			}
		}
		return iMax;
	}

	template<typename _tElement>
						::gpk::error_t			min									(const ::gpk::view<const _tElement> & input, const _tElement ** result) {
		if(0 == input.size())
			return -1;
		*result										= &input[0];
		int32_t											iMin								= 0;
		for(uint32_t iElement = 1; iElement < input.size(); ++iElement) {
			const _tElement									& currentElement					= input[iElement];
			if(currentElement < **result) {
				*result										= &currentElement;
				iMin										= iElement;
			}
		}
		return iMin;
	}

	template<typename _tElement>
						const _tElement&		max									(const ::gpk::view<const _tElement> & input) {
		gthrow_if(0 == input.size(), "%s", "Cannot get reference to max element of an empty array.");
		const _tElement									* result							= &input[0];
		for(uint32_t iElement = 1; iElement < input.size(); ++iElement) {
			const _tElement									& currentElement					= input[iElement];
			if(currentElement > *result)
				result										= &currentElement;
		}
		return *result;
	}

	template<typename _tElement>
						const _tElement&		min									(const ::gpk::view<const _tElement> & input) {
		gthrow_if(0 == input.size(), "%s", "Cannot get reference to max element of an empty array.");
		const _tElement									* result							= &input[0];
		for(uint32_t iElement = 1; iElement < input.size(); ++iElement) {
			const _tElement									& currentElement					= input[iElement];
			if(currentElement < *result)
				result										= &currentElement;
		}
		return *result;
	}

#define be2le_16(number) ::gpk::reverse<ubyte_t>({(ubyte_t*)&number, 2})
#define be2le_32(number) ::gpk::reverse<ubyte_t>({(ubyte_t*)&number, 4})
#define be2le_64(number) ::gpk::reverse<ubyte_t>({(ubyte_t*)&number, 8})

	template<typename _tElement>
	::gpk::error_t									viewRead							(::gpk::view<const _tElement> & headerToRead, const ::gpk::vcb & input)	{
		ree_if(input.size() < 4, "Invalid input size: %u", input.size());
		const uint32_t										elementCount						= *(uint32_t*)input.begin();
		ree_if((elementCount * sizeof(_tElement)) > (input.size() - sizeof(uint32_t)), "Invalid input size: %u. Expected: %u", input.size(), elementCount * sizeof(_tElement));
		headerToRead									= {(input.size() > sizeof(uint32_t)) ? (const _tElement*)&input[sizeof(uint32_t)] : 0, elementCount};
		return sizeof(uint32_t) + headerToRead.size() * sizeof(_tElement);
	}

	template<typename _tElement>
	::gpk::error_t									viewRead							(::gpk::view<_tElement> & headerToRead, const ::gpk::vb & input)	{
		ree_if(input.size() < 4, "Invalid input size: %u", input.size());
		const uint32_t										elementCount						= *(uint32_t*)input.begin();
		ree_if((elementCount * sizeof(_tElement)) > (input.size() - sizeof(uint32_t)), "Invalid input size: %u. Expected: %u", input.size(), elementCount * sizeof(_tElement));
		headerToRead									= {(input.size() > sizeof(uint32_t)) ? (_tElement*)&input[sizeof(uint32_t)] : 0, elementCount};
		return sizeof(uint32_t) + headerToRead.size() * sizeof(_tElement);
	}

	static inline	::gpk::error_t					viewRead							(::gpk::vcs & headerToRead, const ::gpk::vcb & input)	{
		ree_if(input.size() < 4, "Invalid input size: %u", input.size());
		const uint32_t										elementCount						= *(uint32_t*)input.begin();
		ree_if(elementCount > (input.size() - sizeof(uint32_t)), "Invalid input size: %u. Expected: %u", input.size(), elementCount);
		headerToRead									= {(input.size() > sizeof(uint32_t)) ? &input[sizeof(uint32_t)] : 0, elementCount};
		return sizeof(uint32_t) + headerToRead.size();
	}


	template<typename _tElement>
	::gpk::error_t									viewLoad							(::gpk::view<const _tElement> & headerToRead, const ::gpk::vcub & input)	{
		ree_if(input.size() < 4, "Invalid input size: %u", input.size());
		const uint32_t										elementCount						= *(uint32_t*)input.begin();
		ree_if((elementCount * sizeof(_tElement)) > (input.size() - sizeof(uint32_t)), "Invalid input size: %u. Expected: %u", input.size(), elementCount * sizeof(_tElement));
		headerToRead									= {(input.size() > sizeof(uint32_t)) ? (const _tElement*)&input[sizeof(uint32_t)] : 0, elementCount};
		return sizeof(uint32_t) + headerToRead.size() * sizeof(_tElement);
	}

	template<typename _tElement>
	::gpk::error_t									viewLoad							(::gpk::view<_tElement> & headerToRead, const ::gpk::vub & input)	{
		ree_if(input.size() < 4, "Invalid input size: %u", input.size());
		const uint32_t										elementCount						= *(uint32_t*)input.begin();
		ree_if((elementCount * sizeof(_tElement)) > (input.size() - sizeof(uint32_t)), "Invalid input size: %u. Expected: %u", input.size(), elementCount * sizeof(_tElement));
		headerToRead									= {(input.size() > sizeof(uint32_t)) ? (_tElement*)&input[sizeof(uint32_t)] : 0, elementCount};
		return sizeof(uint32_t) + headerToRead.size() * sizeof(_tElement);
	}

	static inline	::gpk::error_t					viewLoad							(::gpk::vcs & headerToRead, const ::gpk::vcub & input)	{
		ree_if(input.size() < 4, "Invalid input size: %u", input.size());
		const uint32_t										elementCount						= *(uint32_t*)input.begin();
		ree_if(elementCount > (input.size() - sizeof(uint32_t)), "Invalid input size: %u. Expected: %u", input.size(), elementCount);
		headerToRead									= vcc{(input.size() > sizeof(uint32_t)) ? (const char*)&input[sizeof(uint32_t)] : 0, elementCount};
		return sizeof(uint32_t) + headerToRead.size();
	}

} // namespace

#endif // GPK_ARRAY_VIEW_H_2398472395543
