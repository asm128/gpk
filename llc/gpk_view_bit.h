#include "gpk_log.h"
#include "gpk_eval.h"

#ifndef GPK_ARRAY_VIEW_BIT_H_9276349872384
#define GPK_ARRAY_VIEW_BIT_H_9276349872384

namespace gpk
{
	template <typename _tElement>
	struct view_bit_proxy {
							_tElement							& Element;
							uint8_t								Offset;

							operator							bool						()																	const				{ return Element & (1ULL << Offset); }
							view_bit_proxy&						operator=					(bool value)																			{ value ? Element |= (1ULL << Offset) : Element &= ~(1ULL << Offset); return *this; }
	};

	template <typename _tElement>
	struct view_bit_iterator {
		stacxpr	const uint32_t						ELEMENT_BITS				= sizeof(_tElement) * 8;

							const _tElement						* Begin;
							const _tElement						* End;
							_tElement							* Element;
							int8_t								Offset;

		inline				operator							bool						()																	const				{ gthrow_if(Element >= End, "Out of range: %u. End: %u.", Element, End); return (*Element) & (1ULL << Offset); }
		inlcxpr	bool								operator==					(const view_bit_iterator& other)									const	noexcept	{ return (((1ULL << Offset)) & *Element) == (((1ULL << other.Offset)) & *other.Element); }
		inlcxpr	bool								operator!=					(const view_bit_iterator& other)									const	noexcept	{ return (((1ULL << Offset)) & *Element) != (((1ULL << other.Offset)) & *other.Element); }

		inline				view_bit_iterator&					operator=					(bool value)																			{ value ? *Element |= (1ULL << Offset) : *Element &= ~(1ULL << Offset); return *this; }
							view_bit_iterator&					operator++					()																						{ ++Offset; if(Offset >= ELEMENT_BITS)	{ ++Element; Offset = 0;				gthrow_if(Element >= (End	+ 1), "Out of range: %u.", Element); } return *this; }
							view_bit_iterator&					operator--					()																						{ --Offset; if(Offset < 0)				{ --Element; Offset = ELEMENT_BITS - 1; gthrow_if(Element <  (Begin	- 1), "Out of range: %u.", Element); } return *this; }
							view_bit_iterator					operator++					(int)																					{
			view_bit_iterator											result						(*this);	// Make a copy.
			++(*this);																								// Use the prefix version to do the work.
			return result;																							// Return the old value.
		}
							view_bit_iterator					operator--					(int)																					{
			view_bit_iterator											result						(*this);	// Make a copy.
			--(*this);																								// Use the prefix version to do the work.
			return result;																							// Return the old value.
		}
	};

	template <typename _tElement>
	class view_bit {
	protected:
		// Properties / Member Variables
							_tElement							* Data						= 0;
							uint32_t							Count						= 0;
	public:
		typedef				_tElement							TElement;
		typedef				view_bit_iterator<_tElement>		iterator;

		stacxpr	const uint32_t						ELEMENT_BITS				= sizeof(_tElement) * 8;

		// Constructors
		inlcxpr										view_bit					()																			noexcept	= default;
		inline													view_bit					(_tElement* dataElements, uint32_t elementCount)										: Data(dataElements), Count(elementCount)										{
			gthrow_if(0 == dataElements && 0 != elementCount, "Invalid parameters. Element count: %u.", elementCount);	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}

		template <size_t _elementCount>
		inlcxpr										view_bit					(_tElement (&_dataElements)[_elementCount])									noexcept	: Data(_dataElements), Count(_elementCount * ELEMENT_BITS)											{}

		template <size_t _elementCount>
		inline													view_bit					(_tElement (&_dataElements)[_elementCount], uint32_t elementCount)						: Data(_dataElements), Count(::gpk::min((uint32_t)(_elementCount * ELEMENT_BITS), elementCount))	{
			gthrow_if(elementCount > (_elementCount * ELEMENT_BITS), "Out of range count. Max count: %u. Requested: %u.", _elementCount, elementCount);
		}

		// Operators
							view_bit_proxy<_tElement>			operator[]					(uint32_t index)																		{
			gthrow_if(0 == Data, "Uninitialized array pointer. Invalid index: %u.", index);
			gthrow_if(index >= Count, "Invalid index: %u.", index);
			const uint32_t												offsetRow					= index / ELEMENT_BITS;
			const uint32_t												offsetBit					= index % ELEMENT_BITS;
			_tElement													& selectedElement			= Data[offsetRow];
			return {selectedElement, (uint8_t)offsetBit};
		}

							bool								operator[]					(uint32_t index)													const				{
			gthrow_if(0 == Data, "Uninitialized array pointer. Invalid index: %u.", index);
			gthrow_if(index >= Count, "Invalid index: %u.", index);
			const uint32_t													offsetElement				= index / ELEMENT_BITS;
			const uint32_t													offsetLocal					= index % ELEMENT_BITS;
			const _tElement													& selectedElement			= Data[offsetElement];
			return (selectedElement & (1ULL << offsetLocal)) > 0 ? true : false;
		}

		// Methods
		inline				view_bit_iterator<_tElement>		begin						()																			noexcept	{ return {Data, Data + Count / ELEMENT_BITS, Data, 0};							}
		inline				view_bit_iterator<_tElement>		end							()																			noexcept	{ return {Data, Data + Count / ELEMENT_BITS, Data + Count / ELEMENT_BITS, 0};	}

		inlcxpr	const view_bit_iterator<_tElement>	begin						()																	const	noexcept	{ return {Data, Data + Count / ELEMENT_BITS, Data, 0};							}
		inlcxpr	const view_bit_iterator<_tElement>	end							()																	const	noexcept	{ return {Data, Data + Count / ELEMENT_BITS, Data + Count / ELEMENT_BITS, 0};	}

		inlcxpr	uint32_t							size						()																	const	noexcept	{ return Count;																	}
	};

	template<typename Val>	using vbit					= ::gpk::view_bit<Val>;

	template<typename _tField>
	::gpk::error_t											reverse_bits				(::gpk::view_bit<_tField> toReverse)													{
		const uint32_t												countBits					= toReverse.size() / 2;
		const uint32_t												lastBitIndex				= toReverse.size() - 1;
		for(uint32_t iBit = 0; iBit < countBits; ++iBit) {
			const uint32_t												iRev						= lastBitIndex - iBit;
			const bool													current						= toReverse[iBit];
			toReverse[iBit]											= (bool)toReverse[iRev];
			toReverse[iRev]											= current;
		}
		return 0;
	}

} // namespace

#endif // GPK_ARRAY_VIEW_BIT_H_9276349872384
