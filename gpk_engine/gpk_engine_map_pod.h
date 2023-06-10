#include "gpk_ptr.h"
#include "gpk_label.h"
#include "gpk_apod_serialize.h"
#include "gpk_enum.h"

#ifndef GPK_ENGINE_MAP_POD_H
#define GPK_ENGINE_MAP_POD_H

namespace gpk
{
	template <typename _tElementWithSaveLoad> 
	struct SLinearNamedMap {
		typedef _tElementWithSaveLoad	T;
		typedef ::gpk::pobj<T>			TPObj;

		::gpk::aobj<TPObj>	Elements		= {};
		::gpk::avcc			Names			= {};

		const TPObj&		operator[]		(uint32_t index)			const	{ return Elements[index]; }
		TPObj&				operator[]		(uint32_t index)					{ return Elements[index]; }
		const TPObj&		operator[]		(::gpk::vcc index)			const	{ return Elements[Names.find(index)]; }
		TPObj&				operator[]		(::gpk::vcc index)					{ return Elements[Names.find(index)]; }

		uint32_t			size			()							const	{ return Names.size(); }
		::gpk::error_t		push_back		(const ::gpk::pobj<T> & instance)	{ Names.push_back({}); return Elements.push_back(instance); }

		::gpk::error_t		Create			()									{ Names.push_back({}); return Elements.push_back({}); }
		::gpk::error_t		Delete			(uint32_t index)					{ Names.remove_unordered(index);	return Elements.remove_unordered(index); }
		::gpk::error_t		Clone			(uint32_t index)					{ 
			TPObj					& newElement	= Elements[Elements.push_back({})]; 
			const TPObj				& srcElement	= Elements[index];
			if(srcElement) {
				*newElement.create() = *srcElement;
			}
			char					newName[256]	= {};
			sprintf_s(newName, "%s_%i", Names.size());
			return Names.push_back(::gpk::label{newName}); 
		}

		::gpk::error_t		Save			(::gpk::au8 & output) const { 
			gpk_necs(output.append(::gpk::vcu8{(const uint8_t*)&Elements.size(), 4}));
			for(uint32_t iEntity = 0; iEntity < Elements.size(); ++iEntity) {
				gpk_necall(Elements[iEntity]->Save(output), "iEntity: %i", iEntity);
				gpk_necall(::gpk::saveView(output, Names[iEntity]), "iEntity: %i", iEntity);
			}
			info_printf("Saved %s, %i", "Elements", Elements.size());
			return 0;
		}

		::gpk::error_t		Load			(::gpk::vcu8 & input) {
			gpk_necs(Elements.resize(*(uint32_t*)input.begin()));
			input				= {input.begin() + sizeof(uint32_t), input.size() - 4};
			gpk_necall(Names.resize(Elements.size()), "size: %i", Elements.size());
			for(uint32_t iEntity = 0; iEntity < Elements.size(); ++iEntity) {
				gpk_necall(Elements[iEntity]->Load(input), "iEntity: %i", iEntity);
				gpk_necall(::gpk::loadLabel(input, Names[iEntity]), "iEntity: %i", iEntity);
			}
			return 0; 
		}
	};

	template <typename _tKey, typename _tElement> 
	struct SLinearKeyPODMap {
		typedef _tElement		T;
		typedef _tKey			K;

		::gpk::apod<T>		Elements		= {};
		::gpk::apod<K>		Keys			= {};

		const T&			operator[]		(const K & key)	const	{ return Elements[Keys.find(key)]; }
		T&					operator[]		(const K & key)			{ return Elements[Keys.find(key)]; }

		uint32_t			size			()				const	{ return Keys.size(); }

		::gpk::error_t		push_back		(const K & key, const T & instance)	{ 
			ree_if(Keys.size() > (uint32_t)Keys.find(key), "Key already found: %i (%s).", key, ::gpk::get_value_namep(key));
			Keys.push_back(key); 
			return Elements.push_back(instance); 
		}

		::gpk::error_t		Create			(const K & key)							{
			ree_if(Keys.size() > (uint32_t)Keys.find(key), "Key already found: %i (%s).", key, ::gpk::get_value_namep(key));
			Keys.push_back({});
			return Elements.push_back({});
		}

		::gpk::error_t		Delete			(const K & key)					{
			uint32_t				index			= Keys.find(key);
			ree_if(Keys.size() <= index, "Key not found: %i (%s).", key, ::gpk::get_value_namep(key));
			Keys.remove_unordered(index);
			return Elements.remove_unordered(index); 
		}

		::gpk::error_t		Clone			(const K & key, uint32_t index)	{ 
			T						& newElement	= Elements[Elements.push_back({})]; 
			const T					& srcElement	= Elements[index];
			newElement			= srcElement;
			return Keys.push_back(key); 
		}

		::gpk::error_t		Save			(::gpk::au8 & output) const { 
			gpk_necs(::gpk::saveView(output, Keys));
			gpk_necs(::gpk::saveView(output, Elements));
			return 0;
		}

		::gpk::error_t		Load			(::gpk::vcu8 & input) {
			gpk_necs(::gpk::loadView(input, Keys));
			gpk_necs(::gpk::loadView(input, Elements));
			return 0; 
		}
	};
} // namespace

#endif // GPK_ENGINE_MAP_POD_H