#include "gpk_array_ptr.h"
#include "gpk_label.h"

#include "gpk_apod_serialize.h"

#ifndef GPK_ENGINE_ENTITY_H_23627
#define GPK_ENGINE_ENTITY_H_23627

namespace gpk 
{
#pragma pack(push, 1)
	typedef	uint32_t			eid_t;

	typedef	apod<eid_t>			aeid;
	typedef	view<const eid_t>	vceid;

	struct SVirtualEntity {
		uint32_t						RenderNode		= (uint32_t)-1;
		uint32_t						RigidBody		= (uint32_t)-1;
		eid_t							Parent			= (eid_t)-1;
	};
#pragma pack(pop)

	struct SVirtualEntityManager {
		::gpk::apod<SVirtualEntity>		Entities		= {};
		::gpk::avcc						Names			= {};

		::gpk::apobj<::gpk::aeid>		Children		= {};

		const ::gpk::SVirtualEntity&	operator[]		(uint32_t index)	const	{ return Entities[index]; }
		SVirtualEntity&					operator[]		(uint32_t index)			{ return Entities[index]; }

		uint32_t						size			()					const	{ return Names.size(); }
		uint32_t						clear			()							{ ::gpk::clear(Entities, Names, Children); return 0; }

		::gpk::error_t					Create			() {
			Children.push_back({});
			Names.push_back({});
			return Entities.push_back({});
		}

		::gpk::error_t					Delete			(uint32_t index) {
			Children.remove_unordered(index);
			Names.remove_unordered(index);
			return Entities.remove_unordered(index);
		}

		::gpk::error_t					SetParent		(uint32_t index, int32_t indexParent) {
			for(uint32_t iParent = 0; iParent < Entities.size(); ++iParent) {
				if(Children[iParent]) {
					const int32_t						oldChild		= Children[iParent]->find(index);
					if(-1 != oldChild)
						Children[iParent]->remove(oldChild);
				}
			}
			Entities[index].Parent			= indexParent;
			return (indexParent != -1) ? 0 : Children[indexParent]->push_back(index);
		}

		::gpk::error_t					Save			(::gpk::au8 & output) const { 
			gpk_necs(::gpk::saveView(output, Entities));
			for(uint32_t iEntity = 0; iEntity < Entities.size(); ++iEntity) {
				gpk_necall(::gpk::saveView(output, Children[iEntity] ? ::gpk::vcu32{*Children[iEntity]} : ::gpk::vcu32{}), "iEntity: %i", iEntity);
				gpk_necall(::gpk::saveView(output, Names[iEntity]), "iEntity: %i", iEntity);
			}
			info_printf("Saved %s, %i", "Entities", Children.size());
			return 0;
		}

		::gpk::error_t					Load			(::gpk::vcu8 & input) {
			gpk_necs(::gpk::loadView(input, Entities));
			gpk_necs(Children	.resize(Entities.size()));
			gpk_necs(Names		.resize(Entities.size()));
			for(uint32_t iEntity = 0; iEntity < Entities.size(); ++iEntity) {
				if(!Children[iEntity])
					Children[iEntity].create();

				gpk_necall(::gpk::loadView(input, *Children[iEntity]), "iEntity: %i", iEntity);
				::gpk::apod<char>					name;
				gpk_necall(::gpk::loadView(input, name), "iEntity: %i", iEntity);
				Names[iEntity]					= ::gpk::label(::gpk::vcc{name});
			}

			return 0; 
		}

	};
} // namespace

#endif // GPK_ENGINE_ENTITY_H_23627
