#include "gpk_log.h"
#include "gpk_eval.h"

#ifndef GPK_ARRAY_VIEW_BIT_H_9276349872384
#define GPK_ARRAY_VIEW_BIT_H_9276349872384

namespace gpk
{
#pragma pack(push, 1)
	template <typename _tInt>
	struct bit_proxy {
		typedef	_tInt			T;

		T						& Element;
		uint8_t					Offset;

		operator				bool			()				const	{ return Element & (1ULL << Offset); }
		bit_proxy&				operator=		(bool value)			{ value ? Element |= (1ULL << Offset) : Element &= ~(1ULL << Offset); return *this; }
	};

	template <typename _tInt>
	struct bit_iterator {
		typedef	_tInt			T;

		stacxpr	uint32_t		ELEMENT_BITS	= sizeof(T) * 8;

		const T					& Begin;
		const T					& End;
		T						* Element;
		uint8_t					Offset	: 4;
		uint8_t					Stop	: 4;

		bit_proxy<T>			operator*		()				{
			gthrow_if(Element == &End, "Invalid index: %u.", uint32_t(&Element - &Begin) + Offset);
			return {Element, (uint8_t)Offset};
		}
		
		bool					operator*		()				{
			gthrow_if(Element == &End, "Invalid index: %u.", uint32_t(&Element - &Begin) + Offset);
			return (*Element) & (1ULL << Offset);
		}

		inline	operator		bool			()									const				{ gthrow_if(Element == &End, "Invalid index: %u.", uint32_t(&Element - &Begin) + Offset); return (*Element) & (1ULL << Offset); }
		inlcxpr	bool			operator==		(const bit_iterator & other)		const	noexcept	{ return (((1ULL << Offset)) & *Element) == (((1ULL << other.Offset)) & *other.Element); }
		inlcxpr	bool			operator!=		(const bit_iterator & other)		const	noexcept	{ return (((1ULL << Offset)) & *Element) != (((1ULL << other.Offset)) & *other.Element); }

		inline	bit_iterator&	operator=		(bool value)	{ value ? *Element |= (1ULL << Offset)	: *Element &= ~(1ULL << Offset); return *this; }
		bit_iterator&			operator++		()				{ ++Offset; if(Offset >= ELEMENT_BITS)	{ ++Element; Offset = 0;				gthrow_if(Element >= (End   + 1), "Out of range: %u. End: %u.", Element, End  ); } return *this; }
		bit_iterator&			operator--		()				{ --Offset; if(Offset < 0)				{ --Element; Offset = ELEMENT_BITS - 1; gthrow_if(Element <  (Begin - 1), "Out of range: %u. End: %u.", Element, Begin); } return *this; }
		bit_iterator			operator++		(int)			{
			bit_iterator				result			(*this);	// Make a copy.
			++(*this);					// Use the prefix version to do the work.
			return result;				// Return the old value.
		}
		bit_iterator			operator--		(int)			{
			bit_iterator				result			(*this);	// Make a copy.
			--(*this);					// Use the prefix version to do the work.
			return result;				// Return the old value.
		}
	};

	template <typename _tInt>
	class view_bit {
	protected:
		// Properties / Member Variables
		_tInt					* Data			= 0;
		uint32_t				Count			= 0;
	public:
		typedef	_tInt			T;
		typedef	bit_iterator<T>	TIter;
		typedef	bit_iterator<const T>	TIterConst;
		typedef	TIter			iterator;

		stacxpr	uint8_t			ELEMENT_BITS	= sizeof(T) * 8;
		//stacxpr	uint8_t			SHIFT_VALUE		= ;

		// Constructors
		inlcxpr					view_bit		()							noexcept	= default;
		inline					view_bit		(T * data, uint32_t bitCount)			: Data(data), Count(bitCount) {
			gthrow_if(bitCount && 0 == data, "Invalid parameters. Element count: %u.", bitCount);	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}

		template <size_t _length>
		inlcxpr					view_bit		(T (&data)[_length])		noexcept	: Data(data), Count(_length * ELEMENT_BITS)											{}

		template <size_t _length>
		inline					view_bit		(T (&data)[_length], uint32_t bitCount)	: Data(data), Count(::gpk::min(uint32_t(_length * ELEMENT_BITS), bitCount))	{
			gthrow_if(bitCount > (_length * ELEMENT_BITS), "Out of range count. Max count: %u. Requested: %u.", _length * ELEMENT_BITS, bitCount);
		}

		// Operators
		bit_proxy<T>			operator[]		(uint32_t index)			{
			gthrow_if(index >= Count, "Invalid index: %u.", index);
			const uint32_t				offsetRow		= index / ELEMENT_BITS;
			const uint32_t				offsetBit		= index % ELEMENT_BITS;
			return {Data[offsetRow], (uint8_t)offsetBit};
		}

		bool					operator[]		(uint32_t index)	const	{
			gthrow_if(index >= Count, "Invalid index: %u.", index);
			const uint32_t				offsetRow		= index / ELEMENT_BITS;
			const uint32_t				offsetBit		= index % ELEMENT_BITS;
			return Data[offsetRow] & (1ULL << offsetBit);
		}

		// Methods
		inline	TIter			begin			()			noexcept	{ return {*Data, *(Data + round_up(Count, ELEMENT_BITS)), Data, 0, Count % ELEMENT_BITS}; }
		inline	TIter			end				()			noexcept	{ return {*Data, *(Data + round_up(Count, ELEMENT_BITS)), Data + round_up(Count, ELEMENT_BITS), 0, Count % ELEMENT_BITS}; }
		//																		  *		 *(									   )  *
		inlcxpr	TIterConst		begin			()	const	noexcept	{ return {*Data, *(Data + round_up(Count, ELEMENT_BITS)), Data, 0, Count % ELEMENT_BITS}; }
		inlcxpr	TIterConst		end				()	const	noexcept	{ return {*Data, *(Data + round_up(Count, ELEMENT_BITS)), Data + round_up(Count, ELEMENT_BITS), 0, Count % ELEMENT_BITS}; }

		inlcxpr	const uint32_t&	size			()	const	noexcept	{ return Count; }
	};
#pragma pack(pop)

	template<typename T>	using vbit	= ::gpk::view_bit<T>;

	template<typename _tField>
	::gpk::error_t			reverse_bits		(::gpk::view_bit<_tField> toReverse)													{
		const uint32_t				countBits			= toReverse.size() / 2;
		const uint32_t				lastBitIndex		= toReverse.size() - 1;
		for(uint32_t iBit = 0; iBit < countBits; ++iBit) {
			const uint32_t				iRev				= lastBitIndex - iBit;
			const bool					current				= toReverse[iBit];
			toReverse[iBit]			= (bool)toReverse[iRev];
			toReverse[iRev]			= current;
		}
		return 0;
	}

} // namespace

#endif // GPK_ARRAY_VIEW_BIT_H_9276349872384
