#include "gpk_array_ptr.h"
#include "gpk_label.h"

#include "gpk_apod_serialize.h"

#ifndef GPK_ENGINE_ENTITY_H
#define GPK_ENGINE_ENTITY_H

namespace gpk 
{
#pragma pack(push, 1)
	typedef	uint32_t		eid_t;
	stacxpr	eid_t			EID_INVALID		= eid_t(-1);

	typedef	const eid_t		ceid_t;
	typedef	apod<eid_t>		aeid;
	typedef	view<eid_t>		veid;
	typedef	view<ceid_t>	vceid;
	typedef	papod<eid_t>	paeid;
	struct SEntity {
		uint32_t				RenderNode		= (uint32_t)-1;
		uint32_t				RigidBody		= (uint32_t)-1;
		::gpk::eid_t			Parent			= EID_INVALID;
	};
#pragma pack(pop)

	struct SVirtualEntityManager {
		::gpk::apod<SEntity>			Entities		= {};
		::gpk::avcc						Names			= {};

		::gpk::aobj<::gpk::paeid>		Children		= {};

		const ::gpk::SEntity&			operator[]		(::gpk::eid_t id)	const	{ return Entities[id]; }
		SEntity&						operator[]		(::gpk::eid_t id)			{ return Entities[id]; }

		uint32_t						size			()					const	{ return Names.size(); }
		uint32_t						clear			()							{ ::gpk::clear(Entities, Names, Children); return 0; }

		::gpk::error_t					Create			() {
			Children.push_back({});
			Names.push_back({});
			return Entities.push_back({});
		}

		::gpk::error_t					Delete			(::gpk::eid_t id) {
			Children.remove_unordered(id);
			Names.remove_unordered(id);
			return Entities.remove_unordered(id);
		}

		inline	::gpk::error_t			GetIndexParent	(::gpk::eid_t id)					const	{ return Entities[id].Parent; }
		inline	::gpk::error_t			GetIndexChild	(::gpk::eid_t id, int32_t iChild)	const	{ return (*Children[id])[iChild]; }
		inline	const ::gpk::SEntity&	GetParent		(::gpk::eid_t id)					const	{ return Entities[GetIndexParent(id)]; }
		inline	const ::gpk::SEntity&	GetChild		(::gpk::eid_t id, int32_t iChild)	const	{ return Entities[GetIndexChild	(id, iChild)]; }
		inline	SEntity&				GetParent		(::gpk::eid_t id)							{ return Entities[GetIndexParent(id)]; }
		inline	SEntity&				GetChild		(::gpk::eid_t id, int32_t iChild)			{ return Entities[GetIndexChild	(id, iChild)]; }
		inline	::gpk::error_t			GetChildCount	(uint32_t iEntity)					const	{ const ::gpk::papod<::gpk::eid_t> & children = Children[iEntity]; return children ? children->size() : 0; }
		inline	::gpk::vceid			GetChildren		(uint32_t iEntity)					const	{ const ::gpk::papod<::gpk::eid_t> & children = Children[iEntity]; return children ? ::gpk::vceid{*children} : ::gpk::vceid{}; }
		inline	::gpk::veid				GetChildren		(uint32_t iEntity)							{ ::gpk::papod<::gpk::eid_t> & children = Children[iEntity]; return children ? *children : ::gpk::veid{}; }


		::gpk::error_t					SetParent		(::gpk::eid_t id, ::gpk::eid_t idParent)		{
			ree_if(idParent != ::gpk::EID_INVALID && idParent >= Entities.size(), "Invalid parent for entity %i, %i", id, idParent);
			::gpk::eid_t						& oldParent		= Entities[id].Parent;
			if(oldParent < Entities.size() && Children[oldParent]) {
				const int32_t						oldIndex		= Children[oldParent]->find(id);
				if(oldIndex >= 0)
					gpk_necall(Children[oldParent]->remove(oldIndex), "oldParent: %i, oldIndex: %i.", oldParent, oldIndex);
			}
			oldParent						= idParent;
			return (0 > (int32_t)idParent) ? 0 : Children[idParent]->push_back(id);
		}

		::gpk::error_t					Save			(::gpk::au8 & output) const { 
			gpk_necs(gpk::saveView(output, Entities));
			for(uint32_t iEntity = 0; iEntity < Entities.size(); ++iEntity) {
				gpk_necall(gpk::saveView(output, Children[iEntity] ? ::gpk::vcu32{*Children[iEntity]} : ::gpk::vcu32{}), "iEntity: %i", iEntity);
				gpk_necall(gpk::saveView(output, Names[iEntity]), "iEntity: %i", iEntity);
			}
			info_printf("Saved %s, %i", "Entities", Children.size());
			return 0;
		}

		::gpk::error_t					Load			(::gpk::vcu8 & input) {
			gpk_necs(gpk::loadView(input, Entities));
			gpk_necs(Children	.resize(Entities.size()));
			gpk_necs(Names		.resize(Entities.size()));
			for(uint32_t iEntity = 0; iEntity < Entities.size(); ++iEntity) {
				if(!Children[iEntity])
					Children[iEntity].create();

				gpk_necall(gpk::loadView(input, *Children[iEntity]), "iEntity: %i", iEntity);
				::gpk::apod<char>					name;
				gpk_necall(gpk::loadView(input, name), "iEntity: %i", iEntity);
				Names[iEntity]					= ::gpk::label(::gpk::vcc{name});
			}

			return 0; 
		}

	};
} // namespace

#endif // GPK_ENGINE_ENTITY_H
