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
		typedef				_tElement				TElement;

		// Constructors
		inline constexpr							view_array					()																			noexcept	= default;
		inline constexpr							view_array					(_tElement* dataElements, uint32_t elementCount)										: Data(dataElements), Count(elementCount)										{
			gthrow_if(0 == dataElements && 0 != elementCount, "Invalid parameters: %p, %u.", dataElements, elementCount);	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}

		template <size_t _elementCount>
		inline constexpr							view_array					(_tElement (&_dataElements)[_elementCount])									noexcept	: Data(_dataElements), Count(_elementCount)										{}

		template <size_t _elementCount>
		inline										view_array					(_tElement (&_dataElements)[_elementCount], uint32_t elementCount)						: Data(_dataElements), Count(::gpk::min((uint32_t)_elementCount, elementCount))			{
			gthrow_if(elementCount > _elementCount, "Element count out of range. Max count: %u. Requested: %u.", (uint32_t)_elementCount, elementCount);
		}

		// Operators
		inline constexpr	operator				view_array<const _tElement>	()																	const	noexcept	{ return {Data, Count}; }
							_tElement&				operator[]					(uint32_t index)																		{ gthrow_if(0 == Data, "%s", "Uninitialized array pointer."); gthrow_if(index >= Count, "Invalid index: %i.", index); return Data[index]; }
							const _tElement&		operator[]					(uint32_t index)													const				{ gthrow_if(0 == Data, "%s", "Uninitialized array pointer."); gthrow_if(index >= Count, "Invalid index: %i.", index); return Data[index]; }

							bool					operator					!=(const ::gpk::view_array<const _tElement>& other)					const				{ return !operator==(other); }
							bool					operator					==(const ::gpk::view_array<const _tElement>& other)					const				{
			if(this->size() != other.size())
				return false;
			if(this->begin() == other.begin())
				return true;
			return ::gpk::equal(other.begin(), this->begin(), this->size());
		}

		// Methods
		inline				_tElement*				begin						()																			noexcept	{ return Data;			}
		inline				_tElement*				end							()																			noexcept	{ return Data + Count;	}

		inline constexpr	const _tElement*		begin						()																	const	noexcept	{ return Data;			}
		inline constexpr	const _tElement*		end							()																	const	noexcept	{ return Data + Count;	}

		inline constexpr	const uint32_t&			size						()																	const	noexcept	{ return Count;			}
	};

	// view_array common typedefs
	//typedef				::gpk::view_array<char_t			>	view_string			;
	typedef				::gpk::view_array<ubyte_t			>	view_ubyte			;
	typedef				::gpk::view_array<byte_t			>	view_byte			;
	typedef				::gpk::view_array<uchar_t			>	view_uchar			;
	typedef				::gpk::view_array<char_t			>	view_char			;
	typedef				::gpk::view_array<float				>	view_float32		;
	typedef				::gpk::view_array<double			>	view_float64		;
	typedef				::gpk::view_array<uint8_t			>	view_uint8			;
	typedef				::gpk::view_array<uint16_t			>	view_uint16			;
	typedef				::gpk::view_array<uint32_t			>	view_uint32			;
	typedef				::gpk::view_array<uint64_t			>	view_uint64			;
	typedef				::gpk::view_array<int8_t			>	view_int8			;
	typedef				::gpk::view_array<int16_t			>	view_int16			;
	typedef				::gpk::view_array<int32_t			>	view_int32			;
	typedef				::gpk::view_array<int64_t			>	view_int64			;

	// view_array<const> common typedefs
	//typedef				::gpk::view_array<const char_t		>	view_const_string	;
	typedef				::gpk::view_array<const ubyte_t		>	view_const_ubyte	;
	typedef				::gpk::view_array<const byte_t		>	view_const_byte		;
	typedef				::gpk::view_array<const uchar_t		>	view_const_uchar	;
	typedef				::gpk::view_array<const char_t		>	view_const_char		;
	typedef				::gpk::view_array<const float		>	view_const_float32	;
	typedef				::gpk::view_array<const double		>	view_const_float64	;
	typedef				::gpk::view_array<const uint8_t		>	view_const_uint8	;
	typedef				::gpk::view_array<const uint16_t	>	view_const_uint16	;
	typedef				::gpk::view_array<const uint32_t	>	view_const_uint32	;
	typedef				::gpk::view_array<const uint64_t	>	view_const_uint64	;
	typedef				::gpk::view_array<const int8_t		>	view_const_int8		;
	typedef				::gpk::view_array<const int16_t		>	view_const_int16	;
	typedef				::gpk::view_array<const int32_t		>	view_const_int32	;
	typedef				::gpk::view_array<const int64_t		>	view_const_int64	;

	struct view_const_string : public view_array<const char_t> {
		inline constexpr											view_const_string					()																= default;
																	view_const_string					(const char* inputString, uint32_t length)						: view_array(inputString, length)			{ Count = (length == (uint32_t)-1) ? (uint32_t)strlen(inputString) : length;																}
		template<size_t _stringLength>								view_const_string					(const char (&inputString)[_stringLength], uint32_t length)		: view_array(inputString, length)			{ Count = (length == (uint32_t)-1) ? (uint32_t)strnlen(inputString, (uint32_t)_stringLength) : ::gpk::min(length, (uint32_t)_stringLength);	}
		template<size_t _stringLength>								view_const_string					(const char (&inputString)[_stringLength])						: view_array(inputString, _stringLength)	{ Count = (uint32_t)strnlen(inputString, (uint32_t)_stringLength);																			}
		// This constructor generates ambiguities so discourage its use	view_const_string					(const ::gpk::view_array<char_t>& inputString)					: view_array(inputString.begin(), inputString.size())	{}
	};

	struct view_string : public view_array<char_t> {
		inline constexpr											view_string							()																= default;
																	view_string							(char* inputString, uint32_t length)							: view_array(inputString, length)			{ Count = (length == (uint32_t)-1) ? (uint32_t)strlen(inputString) : length;																}
		template<size_t _stringLength>								view_string							(char (&inputString)[_stringLength], uint32_t length)			: view_array(inputString, length)			{ Count = (length == (uint32_t)-1) ? (uint32_t)strnlen(inputString, (uint32_t)_stringLength) : ::gpk::min(length, (uint32_t)_stringLength);	}
		template<size_t _stringLength>								view_string							(char (&inputString)[_stringLength])							: view_array(inputString, _stringLength)	{ Count = (uint32_t)strnlen(inputString, (uint32_t)_stringLength);																			}
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

						::gpk::error_t							rtrim								(::gpk::view_const_char & trimmed, const ::gpk::view_const_char & original, const ::gpk::view_const_string & characters = " \t\b\n\r");
						::gpk::error_t							ltrim								(::gpk::view_const_char & trimmed, const ::gpk::view_const_char & original, const ::gpk::view_const_string & characters = " \t\b\n\r");
						::gpk::error_t							trim								(::gpk::view_const_char & trimmed, const ::gpk::view_const_char & original, const ::gpk::view_const_string & characters = " \t\b\n\r");

	static inline		::gpk::error_t							rtrim								(::gpk::view_const_char & trimmed, const ::gpk::view_const_string & characters = " \t\b\n\r")	{ return ::gpk::rtrim	(trimmed, trimmed, characters); }
	static inline		::gpk::error_t							ltrim								(::gpk::view_const_char & trimmed, const ::gpk::view_const_string & characters = " \t\b\n\r")	{ return ::gpk::ltrim	(trimmed, trimmed, characters); }
	static inline		::gpk::error_t							trim								(::gpk::view_const_char & trimmed, const ::gpk::view_const_string & characters = " \t\b\n\r")	{ return ::gpk::trim	(trimmed, trimmed, characters); }

	template<typename _tElement>
						::gpk::error_t							max									(const ::gpk::view_array<const _tElement> & input, const _tElement * result) {
		if(0 == input.size())
			return 0;
		result														= &input[0];
		for(uint32_t iElement = 1; iElement < input.size(); ++iElement) {
			const _tElement													& currentElement					= input[iElement];
			if(currentElement > *result)
				*result														= currentElement;
		}
		return 0;
	}

	template<typename _tElement>
						::gpk::error_t							min									(const ::gpk::view_array<const _tElement> & input, const _tElement * result) {
		if(0 == input.size())
			return 0;
		result														= &input[0];
		for(uint32_t iElement = 1; iElement < input.size(); ++iElement) {
			const _tElement													& currentElement					= input[iElement];
			if(currentElement < *result)
				*result														= currentElement;
		}
		return 0;
	}

#define be2le_16(number) ::gpk::reverse<ubyte_t>({(ubyte_t*)&number, 2})
#define be2le_32(number) ::gpk::reverse<ubyte_t>({(ubyte_t*)&number, 4})
#define be2le_64(number) ::gpk::reverse<ubyte_t>({(ubyte_t*)&number, 8})
}

#endif // GPK_ARRAY_VIEW_H_2398472395543
