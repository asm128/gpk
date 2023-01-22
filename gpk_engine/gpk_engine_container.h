#include "gpk_ptr.h"
#include "gpk_label.h"

#ifndef GPK_ENGINE_CONTAINER_H
#define GPK_ENGINE_CONTAINER_H

namespace gpk
{
	template <typename _tElement> 
	struct SLinearMap {
		::gpk::apobj<_tElement>				Elements		= {};
		::gpk::aobj<::gpk::vcc>				Names			= {};

		const ::gpk::pobj<_tElement>&		operator[]		(uint32_t index)							const	{ return Elements[index]; }
		::gpk::pobj<_tElement>&				operator[]		(uint32_t index)									{ return Elements[index]; }

		uint32_t							size			()											const	{ return Names.size(); }
		::gpk::error_t						push_back		(const ::gpk::pobj<_tElement> & instance)			{ Names.push_back({});				return Elements.push_back(instance); }

		::gpk::error_t						Create			()													{ Names.push_back({});				return Elements.push_back({}); }
		::gpk::error_t						Delete			(uint32_t index)									{ Names.remove_unordered(index);	return Elements.remove_unordered(index); }
		::gpk::error_t						Clone			(uint32_t index)									{ 
			::gpk::pobj<_tElement>					& newElement	= Elements[Elements.push_back({})]; 
			const ::gpk::pobj<_tElement>			& srcElement	= Elements[index];
			if(srcElement) {
				*newElement.create()				= *srcElement;
			}
			return Names.push_back(::gpk::vcc{Names[index]}); 
		}

		::gpk::error_t						Save			(::gpk::apod<ubyte_t> & output) const { 
			gpk_necs(output.append(::gpk::vcub{(const ubyte_t*)&Elements.size(), 4}));
			for(uint32_t iEntity = 0; iEntity < Elements.size(); ++iEntity) {
				gpk_necall(Elements[iEntity]->Save(output), "iEntity: %i", iEntity);
				gpk_necall(::gpk::viewSave(output, ::gpk::vcc{Names[iEntity]}), "iEntity: %i", iEntity);
			}
			info_printf("Saved %s, %i", "Elements", Elements.size());
			return 0;
		}

		::gpk::error_t						Load			(::gpk::vcub & input) {
			gpk_necs(Elements.resize(*(uint32_t*)input.begin()));
			input											= {input.begin() + sizeof(uint32_t), input.size() - 4};
			gpk_necall(Names.resize(Elements.size()), "size: %i", Elements.size());
			for(uint32_t iEntity = 0; iEntity < Elements.size(); ++iEntity) {
				gpk_necall(Elements[iEntity]->Load(input), "iEntity: %i", iEntity);
				::gpk::apod<char_t>					name;
				gpk_necall(::gpk::loadView(input, name), "iEntity: %i", iEntity);
				Names[iEntity]					= ::gpk::label(::gpk::vcc{name});
			}

			return 0; 
		}
	};

} // namespace

#endif // GPK_ENGINE_CONTAINER_H