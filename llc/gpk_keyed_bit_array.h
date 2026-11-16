#include "gpk_view_bit.h"
#include "gpk_array.h"

#ifndef KEYED_BIT_ARRAY_H_23627
#define KEYED_BIT_ARRAY_H_23627

nmsp gpk
{
	tplt<tpnm _tKey>
	struct keyed_bit_array {
		tydf	_tKey			K;
		tydf	vbit<u3_t>	TView;
		tydf	vbit<u3_c>	TViewConst;

		aau3_t				Bits;		// One data array per client.
		aobj<vcst_t>		FieldNames;	// One for all.

		inln	err_t		GetFieldIndex	(vcst_c & fieldName)			cnst	{ return FieldNames.find(fieldName); }

		TViewConst			GetView			(u2_t client)						cnst	{ 
			cnst au3_t				& data			= Bits[client]; 
			return {data.begin(), ::gpk::min(FieldNames.size(), (u2_t)data.bit_count())}; 
		}

		TView				GetView			(u2_t client, b8_t resize = false)			{ 
			au3_t					& data			= Bits[client]; 
			if(resize && FieldNames.size() > data.bit_count()) 
				if_true_te(data.resize_bits(FieldNames.size())); 

			return {data.begin(), ::gpk::min(FieldNames.size(), (u2_t)data.bit_count())}; 
		}

		err_t 				CreateField		(vcst_c & fieldName)					{ 
			s2_t					fieldIndex;
			if_true_fef(succeeded(fieldIndex = FieldNames.find(fieldName)), "Duplicated field name at index %i: %.*s", fieldIndex, GPK_VIEW_ARGS(fieldName));
			return FieldNames.push_back(fieldName);
		}

		// This function only appends or clears `true` bits. It doesn't clear `false` bits, avoiding to resize the array.
		err_t 				SetField		(u2_t client, u2_t index, b8_t value)		{ 
			if_true_fef(index >= FieldNames.size(), "invalid index: %i", index);
			au3_t					& data			= Bits[client]; 
			return (value || data.bit_count() > index) 
				? GetView(client, value)[index] = value
				: 0
				; 
		}

		err_t 				GetField		(u2_t client, u2_t index)		cnst		{ 
			if_true_fef(index >= FieldNames.size(), "invalid index: %i", index);
			cnst au3_t			& data			= Bits[client]; 
			return (index < data.bit_count())
				? TViewConst{data.begin(), data.bit_count()}[index]
				: 0
				;
		}

	};

} // nmsp 

#endif // KEYED_BIT_ARRAY_H_23627
