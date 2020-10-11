#include "gpk_log.h"
#include "gpk_eval.h"
#include <cstring>

#ifndef GPK_ARRAY_VIEW_H_2398472395543
#define GPK_ARRAY_VIEW_H_2398472395543

namespace gpk
{

	template <typename _tElement>
	class view_array {
	protected:
		// Properties / Member Variables
							_tElement				* Data						= 0;
							uint32_t				Count						= 0;
	public:
		typedef				_tElement					TElement;
		typedef				view_array<const _tElement>	TConstView;
		typedef				view_array<_tElement>		TView;

		// Constructors
		inline constexpr							view_array					()																		noexcept	= default;
		inline										view_array					(_tElement* elements, uint32_t elementCount)										: Data(elements), Count(elementCount)										{
			gthrow_if(0 == elements && 0 != elementCount, "Invalid parameters: %p, %u.", elements, elementCount);	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}

		template <size_t _elementCount>
		inline constexpr							view_array					(_tElement (&_elements)[_elementCount])									noexcept	: Data(_elements), Count(_elementCount)										{}

		template <size_t _elementCount>
		inline										view_array					(_tElement (&_elements)[_elementCount], uint32_t elementCount)						: Data(_elements), Count(::gpk::min((uint32_t)_elementCount, elementCount))	{
			gthrow_if(elementCount > _elementCount, "Element count out of range. Max count: %u. Requested: %u.", (uint32_t)_elementCount, elementCount);
		}

		// Operators
		inline constexpr	operator				view_array<const _tElement>	()																const	noexcept	{ return {Data, Count}; }
							_tElement&				operator[]					(uint32_t index)																	{
			gthrow_if(0 == Data, "%s", "Uninitialized array pointer.");
			gthrow_if(index >= Count, "Invalid index: %i.", index);
			return Data[index];
		}
							const _tElement&		operator[]					(uint32_t index)												const				{
			gthrow_if(0 == Data, "%s", "Uninitialized array pointer.");
			gthrow_if(index >= Count, "Invalid index: %i.", index);
			return Data[index];
		}
							bool					operator					!=(const TConstView& other)										const				{ return !operator==(other); }
							bool					operator					==(const TConstView& other)										const				{
			if(this->size() != other.size())
				return false;
			if(this->begin() == other.begin())
				return true;
			return ::gpk::equal(other.begin(), this->begin(), this->size());
		}

		// Methods
		inline				_tElement*				begin						()																		noexcept	{ return Data;			}
		inline				_tElement*				end							()																		noexcept	{ return Data + Count;	}

		inline constexpr	const _tElement*		begin						()																const	noexcept	{ return Data;			}
		inline constexpr	const _tElement*		end							()																const	noexcept	{ return Data + Count;	}

		inline constexpr	const uint32_t&			size						()																const	noexcept	{ return Count;			}

							gpk::error_t			slice						(TView & out, uint32_t offset, uint32_t count = (uint32_t)-1)	const				{
			ree_if(offset > Count, "Out of range. Max offset: %u. Requested: %u.", (uint32_t)Count, offset);
			const uint32_t									newSize						= Count - offset;
			if(count != (uint32_t)-1)
				ree_if(count > newSize, "Out of range. Max size: %u. Requested: %u.", (uint32_t)newSize, count);
			out = {&Data[offset], ::gpk::min(newSize, count)};
			return out.size();
		}
	};

	// view_array common typedefs
	//typedef				::gpk::view_array<char_t			>	view_string			;
	typedef				::gpk::view_array<ubyte_t			>	view_ubyte			;
	typedef				::gpk::view_array<byte_t			>	view_byte			;
	typedef				::gpk::view_array<uchar_t			>	view_uchar			, vuc;
	typedef				::gpk::view_array<char_t			>	view_char			, vc;
	typedef				::gpk::view_array<float				>	view_float32		, vf;
	typedef				::gpk::view_array<double			>	view_float64		, vd;
	typedef				::gpk::view_array<uint8_t			>	view_uint8			, vu8;
	typedef				::gpk::view_array<uint16_t			>	view_uint16			, vu16;
	typedef				::gpk::view_array<uint32_t			>	view_uint32			, vu32;
	typedef				::gpk::view_array<uint64_t			>	view_uint64			, vu64;
	typedef				::gpk::view_array<int8_t			>	view_int8			, vi8;
	typedef				::gpk::view_array<int16_t			>	view_int16			, vi16;
	typedef				::gpk::view_array<int32_t			>	view_int32			, vi32;
	typedef				::gpk::view_array<int64_t			>	view_int64			, vi64;

	// view_array<const> common typedefs
	//typedef				::gpk::view_array<const char_t		>	view_const_string	;
	typedef				::gpk::view_array<const ubyte_t		>	view_const_ubyte	;
	typedef				::gpk::view_array<const byte_t		>	view_const_byte		;
	typedef				::gpk::view_array<const uchar_t		>	view_const_uchar	, vcuc;
	typedef				::gpk::view_array<const char_t		>	view_const_char		, vcc;
	typedef				::gpk::view_array<const float		>	view_const_float32	, vcf;
	typedef				::gpk::view_array<const double		>	view_const_float64	, vcd;
	typedef				::gpk::view_array<const uint8_t		>	view_const_uint8	, vcu8;
	typedef				::gpk::view_array<const uint16_t	>	view_const_uint16	, vcu16;
	typedef				::gpk::view_array<const uint32_t	>	view_const_uint32	, vcu32;
	typedef				::gpk::view_array<const uint64_t	>	view_const_uint64	, vcu64;
	typedef				::gpk::view_array<const int8_t		>	view_const_int8		, vci8;
	typedef				::gpk::view_array<const int16_t		>	view_const_int16	, vci16;
	typedef				::gpk::view_array<const int32_t		>	view_const_int32	, vci32;
	typedef				::gpk::view_array<const int64_t		>	view_const_int64	, vci64;

	struct view_const_string : public view_array<const char_t> {
		inline constexpr											view_const_string					()																= default;
		inline constexpr											view_const_string					(const view_const_char & other)									: view_array(other)							{}
																	view_const_string					(const char* inputString, uint32_t length)						: view_array(inputString, length)			{ Count = (length == (uint32_t)-1) ? (uint32_t)strlen(inputString) : length;																}
		template<size_t _stringLength>								view_const_string					(const char (&inputString)[_stringLength], uint32_t length)		: view_array(inputString, length)			{ Count = (length == (uint32_t)-1) ? (uint32_t)strnlen(inputString, (uint32_t)_stringLength) : ::gpk::min(length, (uint32_t)_stringLength);	}
		template<size_t _stringLength>								view_const_string					(const char (&inputString)[_stringLength])						: view_array(inputString, _stringLength)	{ Count = (uint32_t)strnlen(inputString, (uint32_t)_stringLength);																			}
	};

	typedef view_const_string vcs;

	struct view_string : public view_const_string {
		inline constexpr											view_string							()																= default;
		inline constexpr											view_string							(const view_char & other)										: view_const_string(other)						{}
																	view_string							(char* inputString, uint32_t length)							: view_const_string(inputString, length)		{}
		template<size_t _stringLength>								view_string							(char (&inputString)[_stringLength], uint32_t length)			: view_const_string(inputString, length)		{}
		template<size_t _stringLength>								view_string							(char (&inputString)[_stringLength])							: view_const_string(inputString)				{}
		//constexpr		operator									const view_const_char&			()															const	noexcept	{ return *(const view_const_string*)this; }
	};

	template <typename _tCell>
						::gpk::error_t							reverse								(::gpk::view_array<_tCell> elements)																			{
		for(uint32_t i = 0, swapCount = elements.size() / 2; i < swapCount; ++i) {
			uint8_t															old									= elements[i];
			elements[i]													= elements[elements.size() - 1 - i];
			elements[elements.size() - 1 - i]							= old;
		}
		return 0;
	}

						::gpk::error_t							rtrim								(::gpk::view_const_char & trimmed, const ::gpk::view_const_char & original, const ::gpk::view_const_char & characters = " \t\b\n\r");
						::gpk::error_t							ltrim								(::gpk::view_const_char & trimmed, const ::gpk::view_const_char & original, const ::gpk::view_const_char & characters = " \t\b\n\r");
						::gpk::error_t							trim								(::gpk::view_const_char & trimmed, const ::gpk::view_const_char & original, const ::gpk::view_const_char & characters = " \t\b\n\r");

	template<typename _tElement>
						::gpk::error_t							max									(const ::gpk::view_array<const _tElement> & input, const _tElement ** result) {
		if(0 == input.size())
			return -1;
		*result														= &input[0];
		int32_t															iMax								= 0;
		for(uint32_t iElement = 1; iElement < input.size(); ++iElement) {
			const _tElement													& currentElement					= input[iElement];
			if(currentElement > **result) {
				*result														= &currentElement;
				iMax														= iElement;
			}
		}
		return iMax;
	}

	template<typename _tElement>
						::gpk::error_t							min									(const ::gpk::view_array<const _tElement> & input, const _tElement ** result) {
		if(0 == input.size())
			return -1;
		*result														= &input[0];
		int32_t															iMin								= 0;
		for(uint32_t iElement = 1; iElement < input.size(); ++iElement) {
			const _tElement													& currentElement					= input[iElement];
			if(currentElement < **result) {
				*result														= &currentElement;
				iMin														= iElement;
			}
		}
		return iMin;
	}

	template<typename _tElement>
						const _tElement&						max									(const ::gpk::view_array<const _tElement> & input) {
		gthrow_if(0 == input.size(), "%s", "Cannot get reference to max element of an empty array.");
		const _tElement													* result							= &input[0];
		for(uint32_t iElement = 1; iElement < input.size(); ++iElement) {
			const _tElement													& currentElement					= input[iElement];
			if(currentElement > *result)
				result														= &currentElement;
		}
		return *result;
	}

	template<typename _tElement>
						const _tElement&						min									(const ::gpk::view_array<const _tElement> & input) {
		gthrow_if(0 == input.size(), "%s", "Cannot get reference to max element of an empty array.");
		const _tElement													* result							= &input[0];
		for(uint32_t iElement = 1; iElement < input.size(); ++iElement) {
			const _tElement													& currentElement					= input[iElement];
			if(currentElement < *result)
				result														= &currentElement;
		}
		return *result;
	}

#define be2le_16(number) ::gpk::reverse<ubyte_t>({(ubyte_t*)&number, 2})
#define be2le_32(number) ::gpk::reverse<ubyte_t>({(ubyte_t*)&number, 4})
#define be2le_64(number) ::gpk::reverse<ubyte_t>({(ubyte_t*)&number, 8})
}

#endif // GPK_ARRAY_VIEW_H_2398472395543
