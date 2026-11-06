#include "gpk_view_bit.h"
#include "gpk_array.h"

#ifndef KEYED_BIT_ARRAY_H_23627
#define KEYED_BIT_ARRAY_H_23627

namespace gpk
{
	tplt<tpnm _tKey>
	struct keyed_bit_array {
		typedef	_tKey			K;
		typedef	vbit<u3_t>	TView;
		typedef	vbit<u3_c>	TViewConst;

		aau3_t				Bits;		// One data array per client.
		aobj<vcst_t>		FieldNames;	// One for all.

		inline	err_t		GetFieldIndex	(const vcst_t & fieldName)			const	{ return FieldNames.find(fieldName); }

		TViewConst			GetView			(u2_t client)						const	{ 
			const au3_t				& data			= Bits[client]; 
			return {data.begin(), ::gpk::min(FieldNames.size(), (u2_t)data.bit_count())}; 
		}

		TView				GetView			(u2_t client, bool resize = false)			{ 
			au3_t					& data			= Bits[client]; 
			if(resize && FieldNames.size() > data.bit_count()) 
				if_true_te(data.resize_bits(FieldNames.size())); 

			return {data.begin(), ::gpk::min(FieldNames.size(), (u2_t)data.bit_count())}; 
		}

		err_t 				CreateField		(const vcst_t & fieldName)					{ 
			s2_t					fieldIndex;
			if_true_fef(succeeded(fieldIndex = FieldNames.find(fieldName)), "Duplicated field name at index %i: %s", fieldIndex, GPK_VIEW_ARGS(fieldName).begin()); 
			return FieldNames.push_back(fieldName);
		}

		// This function only appends or clears `true` bits. It doesn't clear `false` bits, avoiding to resize the array.
		err_t 				SetField		(u2_t client, u2_t index, bool value)		{ 
			if_true_fef(index >= FieldNames.size(), "invalid index: %i", index);
			au3_t					& data			= Bits[client]; 
			return (value || data.bit_count() > index) 
				? GetView(client, value)[index] = value
				: 0
				; 
		}

		err_t 				GetField		(u2_t client, u2_t index)		const		{ 
			if_true_fef(index >= FieldNames.size(), "invalid index: %i", index);
			const au3_t			& data			= Bits[client]; 
			return (index < data.bit_count())
				? TViewConst{data.begin(), data.bit_count()}[index]
				: 0
				;
		}

	};

} // namespace 

#endif // KEYED_BIT_ARRAY_H_23627
