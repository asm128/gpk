#include "gpk_array_ptr.h"
#include "gpk_label.h"
#include "gpk_apod_serialize.h"

#ifndef GPK_ENGINE_CONTAINER_H
#define GPK_ENGINE_CONTAINER_H

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
			return Names.push_back(::gpk::vcc{Names[index]}); 
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

} // namespace

#endif // GPK_ENGINE_CONTAINER_H