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
		::gpk::aobj<vcst_t>			FieldNames;	// One for all.

		inline	gpk::err_t GetFieldIndex	(const ::gpk::vcst_t & fieldName)			const	{ return FieldNames.find(fieldName); }

		TViewConst				GetView			(u2_t client)						const	{ 
			const ::gpk::au3_t			& data			= Bits[client]; 
			return {data.begin(), ::gpk::min(FieldNames.size(), data.bit_count())}; 
		}

		TView					GetView			(u2_t client, bool resize = false)			{ 
			::gpk::au3_t					& data			= Bits[client]; 
			if(resize && FieldNames.size() > data.bit_count()) 
				if_true_te(data.resize_bits(FieldNames.size())); 

			return {data.begin(), ::gpk::min(FieldNames.size(), data.bit_count())}; 
		}

		gpk::err_t 		CreateField		(const ::gpk::vcst_t & fieldName)					{ 
			s2_t						fieldIndex;
			if_true_vef(-1, ::gpk::succeeded(fieldIndex = FieldNames.find(fieldName)), "Duplicated field name at index %i: %s", fieldIndex, GPK_VIEW_ARGS(fieldName).begin()); 
			return FieldNames.push_back(fieldName);
		}

		// This function only appends or clears `true` bits. It doesn't clear `false` bits, avoiding to resize the array.
		gpk::err_t 		SetField		(u2_t client, u2_t index, bool value)	{ 
			if_true_vef(-1, index >= FieldNames.size(), "invalid index: %i", index);
			::gpk::au3_t					& data			= Bits[client]; 
			return (value || data.bit_count() > index) 
				? GetView(client, value)[index] = value
				: 0
				; 
		}

		gpk::err_t 		GetField		(u2_t client, u2_t index)		const	{ 
			if_true_vef(-1, index >= FieldNames.size(), "invalid index: %i", index);
			const ::gpk::au3_t			& data			= Bits[client]; 
			return (index < data.bit_count())
				? TViewConst{data.begin(), data.bit_count()}[index]
				: 0
				;
		}

	};

} // namespace 

#endif // KEYED_BIT_ARRAY_H_23627
