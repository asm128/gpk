#include "gpk_array_ptr.h"
#include "gpk_label.h"
#include "gpk_apod_serialize.h"

#ifndef GPK_LINEAR_MAP_POBJ_H_23627
#define GPK_LINEAR_MAP_POBJ_H_23627

namespace gpk
{
	tplt <tpnm _tElementWithSaveLoad> 
	struct gpk_linear_map_pobj {
		typedef _tElementWithSaveLoad	T;
		typedef ::gpk::pobj<T>			TPObj;

		::gpk::aobj<TPObj>			Elements		= {};
		::gpk::avcsc_t				Names			= {};

		inline	const TPObj&		operator[]		(uint32_t index)	const	{ return Elements[index]; }
		inline	TPObj&				operator[]		(uint32_t index)			{ return Elements[index]; }
		inline	const TPObj&		operator[]		(::gpk::vcsc_t key)	const	{ return Elements[Names.find(key)]; }
		inline	TPObj&				operator[]		(::gpk::vcsc_t key)			{ return Elements[Names.find(key)]; }

		inlcxpr	uint32_t			size			()											const	{ return Names.size(); }
		inline	::gpk::error_t		find			(::gpk::vcsc_c key, uint32_t offset = 0)	const	{ return Names.find(key, offset); }

		::gpk::error_t				push_back		(const ::gpk::pobj<T> & instance, ::gpk::vcsc_c name = {})	{ 
			Names.push_back(name); 
			return Elements.push_back(instance); 
		}

		inline	::gpk::error_t		Create			(const ::gpk::vcsc_t name = {})									{ return push_back({}, name); }
		::gpk::error_t				Delete			(uint32_t index)												{ 
			Names.remove_unordered(index); 
			return Elements.remove_unordered(index); 
		}

		::gpk::error_t				Clone			(uint32_t index)												{ 
			int32_t							iNew;
			gpk_necall(iNew = Create(Names[index]), "index: %i", index);
			const TPObj						& srcElement	= Elements[index];
			if(srcElement) {
				TPObj							& newElement	= Elements[iNew]; 
				if(!newElement)
					newElement.create();
				*newElement					= *srcElement;
			}
			return iNew; 
		}

		::gpk::error_t				Save			(::gpk::au0_t & output) const { 
			gpk_necs(output.append(::gpk::vcu0_t{(const uint8_t*)&Elements.size(), 4}));
			for(uint32_t iEntity = 0; iEntity < Elements.size(); ++iEntity) {
				gpk_necall(Elements[iEntity]->Save(output), "iEntity: %i", iEntity);
				gpk_necall(gpk::saveView(output, Names[iEntity]), "iEntity: %i", iEntity);
			}
			info_printf("Saved %s, %i", "Elements", Elements.size());
			return 0;
		}

		::gpk::error_t				Load			(::gpk::vcu0_t & input) {
			gpk_necs(Elements.resize(*(uint32_t*)input.begin()));
			input						= {input.begin() + sizeof(uint32_t), input.size() - 4};
			gpk_necall(Names.resize(Elements.size()), "size: %i", Elements.size());
			for(uint32_t iEntity = 0; iEntity < Elements.size(); ++iEntity) {
				gpk_necall(Elements[iEntity]->Load(input), "iEntity: %i", iEntity);
				gpk_necall(gpk::loadLabel(input, Names[iEntity]), "iEntity: %i", iEntity);
			}
			return 0; 
		}
	};

} // namespace

#endif // GPK_LINEAR_MAP_POBJ_H_23627
