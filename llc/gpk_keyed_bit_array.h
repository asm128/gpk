#include "gpk_view_bit.h"
#include "gpk_array.h"

#ifndef KEYED_BIT_ARRAY_H_23627
#define KEYED_BIT_ARRAY_H_23627

namespace gpk
{
	tplt<tpnm _tKey>
	struct keyed_bit_array {
		typedef	_tKey			K;
		typedef	gpk::vbit<u3_t>	TView;
		typedef	gpk::vbit<u3_c>	TViewConst;

		::gpk::aau64			Bits;		// One data array per client.
		::gpk::avcc				FieldNames;	// One for all.

		inline	::gpk::error_t	GetFieldIndex	(const ::gpk::vcc & fieldName)			const	{ return FieldNames.find(fieldName); }

		TViewConst				GetView			(uint32_t client)						const	{ 
			const ::gpk::au64			& data			= Bits[client]; 
			return {data.begin(), ::gpk::min(FieldNames.size(), data.bit_count())}; 
		}

		TView					GetView			(uint32_t client, bool resize = false)			{ 
			::gpk::au64					& data			= Bits[client]; 
			if(resize && FieldNames.size() > data.bit_count()) 
				gsthrow_if(data.resize_bits(FieldNames.size())); 

			return {data.begin(), ::gpk::min(FieldNames.size(), data.bit_count())}; 
		}

		::gpk::error_t			CreateField		(const ::gpk::vcc & fieldName)					{ 
			int32_t						fieldIndex;
			ree_if(::gpk::succeeded(fieldIndex = FieldNames.find(fieldName)), "Duplicated field name at index %i: %s", fieldIndex, ::gpk::toString(fieldName).begin()); 
			return FieldNames.push_back(fieldName);
		}

		// This function only appends or clears `true` bits. It doesn't clear `false` bits, avoiding to resize the array.
		::gpk::error_t			SetField		(uint32_t client, uint32_t index, bool value)	{ 
			ree_if(index >= FieldNames.size(), "invalid index: %i", index);

			::gpk::au64					& data			= Bits[client]; 
			
			return (value || data.bit_count() > index) 
				? GetView(client, value)[index] = value
				: 0
				; 
		}

		::gpk::error_t			GetField		(uint32_t client, uint32_t index)		const	{ 
			ree_if(index >= FieldNames.size(), "invalid index: %i", index);

			const ::gpk::au64			& data			= Bits[client]; 
			return (index < data.bit_count())
				? TViewConst{data.begin(), data.bit_count()}[index]
				: 0
				;
		}

	};

} // namespace 

#endif // KEYED_BIT_ARRAY_H_23627
