#include "gpk_array_ptr.h"
#include "gpk_label.h"
#include "gpk_apod_serialize.h"
#include "gpk_enum.h"

#ifndef GPK_LINEAR_MAP_POD_H_23627
#define GPK_LINEAR_MAP_POD_H_23627

namespace gpk
{
	tplt <tpnm _tKey, tpnm _tValue>
	struct linear_map_pod {
		typedef _tValue		T;
		typedef _tKey		K;

		::gpk::apod<T>		Values			= {};
		::gpk::apod<K>		Keys			= {};

		const T&			operator[]		(const K & key)	const	{ return Values[Keys.find(key)]; }
		T&					operator[]		(const K & key)			{ return Values[Keys.find(key)]; }

		::gpk::u2_t			size			()				const	{ return Keys.size(); }

		::gpk::error_t		push_back		(const K & key, const T & instance)	{
			if_true_fef(0 <= Keys.find(key), "%s", "Duplicate key.");
			::gpk::err_t				index			= 0;
			gpk_necs(index = Keys.push_back(key));
			cnst ::gpk::err_t		valueIndex		= Values.push_back(instance);
			if(::gpk::failed(valueIndex)) {
				Keys.pop_back();
				return valueIndex;
			}
			return index;
		}

		::gpk::error_t		Create			(const K & key)							{
			return push_back(key, T{});
		}

		::gpk::error_t		Delete			(const K & key)					{
			cnst ::gpk::err_t		index			= Keys.find(key);
			gpk_necs(index);
			gpk_necs(Keys.remove_unordered(index));
			return Values.remove_unordered(index);
		}

		::gpk::error_t		Clone			(const K & key, ::gpk::u2_t index)	{
			if_true_fef(index >= Values.size(), "Invalid source index:%u. Value count:%u.", index, Values.size());
			return push_back(key, Values[index]);
		}

		::gpk::error_t		Save			(::gpk::au0_t & output) const {
			if_true_fef(Keys.size() != Values.size(), "Key count:%u. Value count:%u.", Keys.size(), Values.size());
			gpk_necs(gpk::saveView(output, Keys));
			gpk_necs(gpk::saveView(output, Values));
			return 0;
		}

		::gpk::error_t		Load			(::gpk::vcu0_t & input) {
			::gpk::vcu0_t			inputToRead		= input;
			::gpk::apod<K>			loadedKeys		= {};
			::gpk::apod<T>			loadedValues	= {};
			gpk_necs(gpk::loadView(inputToRead, loadedKeys));
			gpk_necs(gpk::loadView(inputToRead, loadedValues));
			if_true_fef(loadedKeys.size() != loadedValues.size(), "Key count:%u. Value count:%u.", loadedKeys.size(), loadedValues.size());
			for(::gpk::u2_t iKey = 0; iKey < loadedKeys.size(); ++iKey)
				if_true_fef(0 <= loadedKeys.find(loadedKeys[iKey], iKey + 1), "Duplicate key at index:%u.", iKey);
			Keys		= loadedKeys;
			Values		= loadedValues;
			input		= inputToRead;
			return 0;
		}
	};
	tplt <tpnm _tKey, tpnm _tValue>
	using	lmpod	= linear_map_pod<_tKey, _tValue>;
} // namespace

#endif // GPK_LINEAR_MAP_POD_H_23627


