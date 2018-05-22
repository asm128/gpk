#include "gpk_log.h"
#include "gpk_eval.h"
#include <cstring>

#include <exception>	// for ::std::exception

#ifndef GPK_ARRAY_VIEW_H_2398472395543
#define GPK_ARRAY_VIEW_H_2398472395543

namespace gpk
{
	template <typename _tElement>
	class array_view {
	protected:
		// Properties / Member Variables
							_tElement				* Data						= 0;
							uint32_t				Count						= 0;
	public:
		typedef				_tElement				TElement;
		// Constructors
		inline constexpr							array_view					()																			noexcept	= default;
		inline										array_view					(_tElement* dataElements, uint32_t elementCount)										: Data(dataElements), Count(elementCount)										{
			throw_if(0 == dataElements && 0 != elementCount, ::std::exception(""), "Invalid parameters.");	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}

		template <size_t _elementCount>
		inline constexpr							array_view					(_tElement (&_dataElements)[_elementCount])									noexcept	: Data(_dataElements), Count(_elementCount)										{}

		template <size_t _elementCount>
		inline constexpr							array_view					(_tElement (&_dataElements)[_elementCount], uint32_t elementCount)						: Data(_dataElements), Count(::gpk::min(_elementCount, elementCount))			{
			throw_if(elementCount > _elementCount, ::std::exception(""), "Element count out of range.");
		}

		// Operators
		inline constexpr	operator				array_view<const _tElement>	()																			noexcept	{ return {Data, Count}; }
							_tElement&				operator[]					(uint32_t index)																		{ throw_if(0 == Data, ::std::exception(""), "Uninitialized array pointer."); throw_if(index >= Count, ::std::exception(""), "Invalid index: %u.", index); return Data[index]; }
							const _tElement&		operator[]					(uint32_t index)													const				{ throw_if(0 == Data, ::std::exception(""), "Uninitialized array pointer."); throw_if(index >= Count, ::std::exception(""), "Invalid index: %u.", index); return Data[index]; }

		// Methods
		inline				_tElement*				begin						()																			noexcept	{ return Data;			}
		inline				_tElement*				end							()																			noexcept	{ return Data + Count;	}

		inline constexpr	const _tElement*		begin						()																	const	noexcept	{ return Data;			}
		inline constexpr	const _tElement*		end							()																	const	noexcept	{ return Data + Count;	}

		inline constexpr	uint32_t				size						()																	const	noexcept	{ return Count;			}
	};

	// array_view common typedefs
	//typedef				::gpk::array_view<char_t			>	view_string			;
	typedef				::gpk::array_view<ubyte_t			>	view_ubyte			;
	typedef				::gpk::array_view<byte_t			>	view_byte			;
	typedef				::gpk::array_view<float				>	view_float32		;
	typedef				::gpk::array_view<double			>	view_float64		;
	typedef				::gpk::array_view<uint8_t			>	view_uint8			;
	typedef				::gpk::array_view<uint16_t			>	view_uint16			;
	typedef				::gpk::array_view<uint32_t			>	view_uint32			;
	typedef				::gpk::array_view<uint64_t			>	view_uint64			;
	typedef				::gpk::array_view<int8_t			>	view_int8			;
	typedef				::gpk::array_view<int16_t			>	view_int16			;
	typedef				::gpk::array_view<int32_t			>	view_int32			;
	typedef				::gpk::array_view<int64_t			>	view_int64			;

	// array_view<const> common typedefs
	//typedef				::gpk::array_view<const char_t		>	view_const_string	;
	typedef				::gpk::array_view<const ubyte_t		>	view_const_ubyte	;
	typedef				::gpk::array_view<const byte_t		>	view_const_byte		;
	typedef				::gpk::array_view<const float		>	view_const_float32	;
	typedef				::gpk::array_view<const double		>	view_const_float64	;
	typedef				::gpk::array_view<const uint8_t		>	view_const_uint8	;
	typedef				::gpk::array_view<const uint16_t	>	view_const_uint16	;
	typedef				::gpk::array_view<const uint32_t	>	view_const_uint32	;
	typedef				::gpk::array_view<const uint64_t	>	view_const_uint64	;
	typedef				::gpk::array_view<const int8_t		>	view_const_int8		;
	typedef				::gpk::array_view<const int16_t		>	view_const_int16	;
	typedef				::gpk::array_view<const int32_t		>	view_const_int32	;
	typedef				::gpk::array_view<const int64_t		>	view_const_int64	;

	struct view_const_string : public array_view<const char_t> {
																	view_const_string					(const char* inputString, uint32_t length)					: array_view(inputString, length)			{ Count = (length == (uint32_t)-1) ? (uint32_t)strlen(inputString) : length;										}
		template<size_t _stringLength>								view_const_string					(const char (&inputString)[_stringLength], uint32_t length)	: array_view(inputString, length)			{ Count = (length == (uint32_t)-1) ? (uint32_t)strlen(inputString) : ::gpk::min(length, (uint32_t)_stringLength);	}
		template<size_t _stringLength>								view_const_string					(const char (&inputString)[_stringLength])					: array_view(inputString, _stringLength)	{ Count = (uint32_t)strlen(inputString);																			}	
	};

	struct view_string : public array_view<char_t> {
																	view_string							(char* inputString, uint32_t length)						: array_view(inputString, length)			{ Count = (length == (uint32_t)-1) ? (uint32_t)strlen(inputString) : length;										}
		template<size_t _stringLength>								view_string							(char (&inputString)[_stringLength], uint32_t length)		: array_view(inputString, length)			{ Count = (length == (uint32_t)-1) ? (uint32_t)strlen(inputString) : ::gpk::min(length, (uint32_t)_stringLength);	}
		template<size_t _stringLength>								view_string							(char (&inputString)[_stringLength])						: array_view(inputString, _stringLength)	{ Count = (uint32_t)strlen(inputString);																			}	
	};

}

#endif GPK_ARRAY_VIEW_H_2398472395543
