#include "gpk_array_ptr.h"
#include "gpk_label.h"
#include "gpk_apod_serialize.h"
#include "gpk_enum.h"

#ifndef GPK_LINEAR_MAP_POD_H_23627
#define GPK_LINEAR_MAP_POD_H_23627

namespace gpk
{
	template <typename _tKey, typename _tValue> 
	struct linear_map_pod {
		typedef _tValue		T;
		typedef _tKey		K;

		::gpk::apod<T>		Values			= {};
		::gpk::apod<K>		Keys			= {};

		const T&			operator[]		(const K & key)	const	{ return Values[Keys.find(key)]; }
		T&					operator[]		(const K & key)			{ return Values[Keys.find(key)]; }

		uint32_t			size			()				const	{ return Keys.size(); }

		::gpk::error_t		push_back		(const K & key, const T & instance)	{ 
			rees_if(Keys.size() > (uint32_t)Keys.find(key));
			Keys.push_back(key); 
			return Values.push_back(instance); 
		}

		::gpk::error_t		Create			(const K & key)							{
			rees_if(Keys.size() > (uint32_t)Keys.find(key));
			Keys.push_back(key);
			return Values.push_back({});
		}

		::gpk::error_t		Delete			(const K & key)					{
			uint32_t				index			= Keys.find(key);
			rees_if(Keys.size() <= index);
			Keys.remove_unordered(index);
			return Values.remove_unordered(index); 
		}

		::gpk::error_t		Clone			(const K & key, uint32_t index)	{ 
			Values[Values.push_back({})] = Values[index]; 
			return Keys.push_back(key); 
		}

		::gpk::error_t		Save			(::gpk::au8 & output) const { 
			gpk_necs(::gpk::saveView(output, Keys));
			gpk_necs(::gpk::saveView(output, Values));
			return 0;
		}

		::gpk::error_t		Load			(::gpk::vcu8 & input) {
			gpk_necs(::gpk::loadView(input, Keys));
			gpk_necs(::gpk::loadView(input, Values));
			return 0; 
		}
	};
	template <typename _tKey, typename _tValue> 
	using	lmpod	= linear_map_pod<_tKey, _tValue>;
} // namespace

#endif // GPK_LINEAR_MAP_POD_H_23627