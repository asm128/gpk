#include "gpk_ptr.h"
#include "gpk_label.h"

#ifndef GPK_ENGINE_ENTITY_H
#define GPK_ENGINE_ENTITY_H

namespace gpk 
{
#pragma pack(push, 1)
	struct SVirtualEntity {
		uint32_t							RenderNode		= (uint32_t)-1;
		uint32_t							RigidBody		= (uint32_t)-1;
		uint32_t							Parent			= (uint32_t)-1;
	};
#pragma pack(pop)

	struct SVirtualEntityManager {
		::gpk::apod<::gpk::SVirtualEntity>	Entities		= {};
		::gpk::aobj<::gpk::vcc>				Names			= {};

		::gpk::apobj<::gpk::apod<uint32_t>>	Children		= {};

		const ::gpk::SVirtualEntity&		operator[]		(uint32_t index)							const	{ return Entities[index]; }
		SVirtualEntity&						operator[]		(uint32_t index)									{ return Entities[index]; }

		uint32_t							size			()											const	{ return Names.size(); }

		::gpk::error_t						Create			() {
			Children.push_back({});
			Names.push_back({});
			return Entities.push_back({});
		}

		::gpk::error_t						Delete			(uint32_t index) {
			Children.remove_unordered(index);
			Names.remove_unordered(index);
			return Entities.remove_unordered(index);
		}

		::gpk::error_t						Save			(::gpk::apod<ubyte_t> & output) const { 
			gpk_necs(::gpk::viewSave(output, Entities));
			for(uint32_t iEntity = 0; iEntity < Entities.size(); ++iEntity) {
				gpk_necall(::gpk::viewSave(output, ::gpk::vcu32	{*Children	[iEntity]}), "iEntity: %i", iEntity);
				gpk_necall(::gpk::viewSave(output, ::gpk::vcc	{Names		[iEntity]}), "iEntity: %i", iEntity);
			}
			info_printf("Saved %s, %i", "Entities", Children.size());
			return 0;
		}

		::gpk::error_t						Load			(::gpk::vcub & input) {
			gpk_necs(::gpk::loadView(input, Entities));
			gpk_necall(Children	.resize(Entities.size()), "size: %i", Entities.size());
			gpk_necall(Names	.resize(Entities.size()), "size: %i", Entities.size());
			for(uint32_t iEntity = 0; iEntity < Entities.size(); ++iEntity) {
				if(!Children[iEntity])
					Children[iEntity].create();
				gpk_necall(::gpk::loadView(input, *Children[iEntity]), "iEntity: %i", iEntity);
				::gpk::apod<char_t>					name;
				gpk_necall(::gpk::loadView(input, name), "iEntity: %i", iEntity);
				Names[iEntity]					= ::gpk::label(::gpk::vcc{name});
			}

			return 0; 
		}

	};
} // namespace

#endif
