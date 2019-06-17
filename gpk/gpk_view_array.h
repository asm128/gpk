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
		template<typename _tElement>
							bool					equal						(const _tElement * other, const _tElement * local, uint32_t count)						const	{
			for(uint32_t iElement = 0; iElement < count; ++iElement)
				if(other[iElement] != local[iElement])
					return false;
			return true; 
		}
		template<>	inline	bool					equal						(const double	* other, const double	* local, uint32_t count)						const	{ return 0 == memcmp(other, local, sizeof(double	)*count); }
		template<>	inline	bool					equal						(const float	* other, const float	* local, uint32_t count)						const	{ return 0 == memcmp(other, local, sizeof(float		)*count); }
		template<>	inline	bool					equal						(const int32_t	* other, const int32_t	* local, uint32_t count)						const	{ return 0 == memcmp(other, local, sizeof(int32_t	)*count); }
		template<>	inline	bool					equal						(const uint32_t	* other, const uint32_t	* local, uint32_t count)						const	{ return 0 == memcmp(other, local, sizeof(uint32_t	)*count); }
		template<>	inline	bool					equal						(const int16_t	* other, const int16_t	* local, uint32_t count)						const	{ return 0 == memcmp(other, local, sizeof(int16_t	)*count); }
		template<>	inline	bool					equal						(const uint16_t	* other, const uint16_t	* local, uint32_t count)						const	{ return 0 == memcmp(other, local, sizeof(uint16_t	)*count); }
		template<>	inline	bool					equal						(const char_t	* other, const char_t	* local, uint32_t count)						const	{ return 0 == memcmp(other, local, sizeof(char_t	)*count); }
		template<>	inline	bool					equal						(const uchar_t	* other, const uchar_t	* local, uint32_t count)						const	{ return 0 == memcmp(other, local, sizeof(uchar_t	)*count); }
	public:
		typedef				_tElement				TElement;

		// Constructors
		inline constexpr							view_array					()																			noexcept	= default;
		inline										view_array					(_tElement* dataElements, uint32_t elementCount)										: Data(dataElements), Count(elementCount)										{
			throw_if(0 == dataElements && 0 != elementCount, "Invalid parameters: %p, %u.", dataElements, elementCount);	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}

		template <size_t _elementCount>
		inline constexpr							view_array					(_tElement (&_dataElements)[_elementCount])									noexcept	: Data(_dataElements), Count(_elementCount)										{}

		template <size_t _elementCount>
		inline										view_array					(_tElement (&_dataElements)[_elementCount], uint32_t elementCount)						: Data(_dataElements), Count(::gpk::min((uint32_t)_elementCount, elementCount))			{
			throw_if(elementCount > _elementCount, "Element count out of range. Max count: %u. Requested: %u.", (uint32_t)_elementCount, elementCount);
		}

		// Operators
		inline constexpr	operator				view_array<const _tElement>	()																			noexcept	{ return {Data, Count}; }
							_tElement&				operator[]					(uint32_t index)																		{ throw_if(0 == Data, "%s", "Uninitialized array pointer."); throw_if(index >= Count, "Invalid index: %u.", index); return Data[index]; }
							const _tElement&		operator[]					(uint32_t index)													const				{ throw_if(0 == Data, "%s", "Uninitialized array pointer."); throw_if(index >= Count, "Invalid index: %u.", index); return Data[index]; }

							bool					operator					!=(const ::gpk::view_array<const _tElement>& other)					const				{ return !operator==(other); }
							bool					operator					==(const ::gpk::view_array<const _tElement>& other)					const				{
		if(this->size() != other.size())
			return false;
		if(this->begin() == other.begin())
			return true;
		return this->equal(other.begin(), this->begin(), this->size());
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
		inline constexpr											view_const_string					()																				= default;
																	view_const_string					(const char* inputString, uint32_t length)										: view_array(inputString, length)			{ Count = (length == (uint32_t)-1) ? (uint32_t)strlen(inputString) : length;										}
		template<size_t _stringLength>								view_const_string					(const char (&inputString)[_stringLength], uint32_t length)						: view_array(inputString, length)			{ Count = (length == (uint32_t)-1) ? (uint32_t)strlen(inputString) : ::gpk::min(length, (uint32_t)_stringLength);	}
		template<size_t _stringLength>								view_const_string					(const char (&inputString)[_stringLength])										: view_array(inputString, _stringLength)	{ Count = (uint32_t)strlen(inputString);																			}
	};

	struct view_string : public view_array<char_t> {
		inline constexpr											view_string							()																				= default;
																	view_string							(char* inputString, uint32_t length)											: view_array(inputString, length)			{ Count = (length == (uint32_t)-1) ? (uint32_t)strlen(inputString) : length;										}
		template<size_t _stringLength>								view_string							(char (&inputString)[_stringLength], uint32_t length)							: view_array(inputString, length)			{ Count = (length == (uint32_t)-1) ? (uint32_t)strlen(inputString) : ::gpk::min(length, (uint32_t)_stringLength);	}
		template<size_t _stringLength>								view_string							(char (&inputString)[_stringLength])											: view_array(inputString, _stringLength)	{ Count = (uint32_t)strlen(inputString);																			}
		//constexpr		operator									const view_const_char&			()															const	noexcept	{ return *(const view_const_string*)this; }
	};


	template <typename _tCell>
						::gpk::error_t							reverse								(::gpk::view_array<_tCell> elements)											{
		for(uint32_t i = 0, swapCount = elements.size() / 2; i < swapCount; ++i) {
			uint8_t															old									= elements[i];
			elements[i]													= elements[elements.size() - 1 - i];
			elements[elements.size() - 1 - i]							= old;
		}
		return 0;
	}

	template<typename _tElement>
						::gpk::error_t							find_sequence_obj					(const ::gpk::view_array<_tElement>& sequence, const ::gpk::view_array<_tElement>& target)	{
		for(uint32_t iOffset = 0, offsetStop = target.size() - sequence.size(); iOffset < offsetStop; ++iOffset) {
			bool															equal								= true;
			for(uint32_t iSequenceElement = 0; iSequenceElement < sequence.size(); ++iSequenceElement) {
				if(sequence[iSequenceElement] != target[iOffset + iSequenceElement])
					equal														= false;
			}
			if(equal)
				return iOffset;
		}
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t							find_sequence_pod					(const ::gpk::view_array<_tElement>& sequence, const ::gpk::view_array<_tElement>& target, int32_t offset = 0)	{
		for(int32_t iOffset = offset, offsetStop = (int32_t)(target.size() - sequence.size()) + 1; iOffset < offsetStop; ++iOffset) {
			if(0 == memcmp(sequence.begin(), &target[iOffset], sequence.size() * sizeof(_tElement))) 
				return iOffset;
		}
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t							rfind_sequence_pod					(const ::gpk::view_array<_tElement>& sequence, const ::gpk::view_array<_tElement>& target)	{
		for(int32_t iOffset = (int32_t)(target.size() - sequence.size()); iOffset >= 0; --iOffset) {
			if(0 == memcmp(sequence.begin(), &target[iOffset], sequence.size() * sizeof(_tElement))) 
				return iOffset;
		}
		return -1;
	}

#define be2le_16(number) ::gpk::reverse<ubyte_t>({(ubyte_t*)&number, 2})
#define be2le_32(number) ::gpk::reverse<ubyte_t>({(ubyte_t*)&number, 4})
#define be2le_64(number) ::gpk::reverse<ubyte_t>({(ubyte_t*)&number, 8})
}

#endif // GPK_ARRAY_VIEW_H_2398472395543
