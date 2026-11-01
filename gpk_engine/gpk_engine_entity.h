#include "gpk_array_ptr.h"
#include "gpk_label.h"

#include "gpk_apod_serialize.h"

#ifndef GPK_ENGINE_ENTITY_H
#define GPK_ENGINE_ENTITY_H

namespace gpk 
{
#pragma pack(push, 1)
	typedef	u2_t		eid_t;
	stxp	eid_t			EID_INVALID		= eid_t(-1);

	typedef	const eid_t		ceid_t;
	typedef	apod<eid_t>		aeid;
	typedef	view<eid_t>		veid;
	typedef	view<ceid_t>	vceid;
	typedef	papod<eid_t>	paeid;
	struct SEntity {
		u2_t				RenderNode		= (u2_t)-1;
		u2_t				RigidBody		= (u2_t)-1;
		::gpk::eid_t			Parent			= EID_INVALID;
	};
#pragma pack(pop)

	struct SVirtualEntityManager {
		::gpk::apod<SEntity>			Entities		= {};
		::gpk::avcsc_t					Names			= {};

		::gpk::aobj<::gpk::paeid>		Children		= {};

		const ::gpk::SEntity&			operator[]		(::gpk::eid_t id)	const	{ return Entities[id]; }
		SEntity&						operator[]		(::gpk::eid_t id)			{ return Entities[id]; }

		u2_t						size			()					const	{ return Names.size(); }
		u2_t						clear			()							{ ::gpk::clear(Entities, Names, Children); return 0; }

		::gpk::err_t					Create			() {
			Children.push_back({});
			Names.push_back({});
			return Entities.push_back({});
		}

		::gpk::err_t					Delete			(::gpk::eid_t id) {
			Children.remove_unordered(id);
			Names.remove_unordered(id);
			return Entities.remove_unordered(id);
		}

		inline	::gpk::err_t			GetIndexParent	(::gpk::eid_t id)					const	{ return Entities[id].Parent; }
		inline	::gpk::err_t			GetIndexChild	(::gpk::eid_t id, s2_t iChild)	const	{ return (*Children[id])[iChild]; }
		inline	const ::gpk::SEntity&	GetParent		(::gpk::eid_t id)					const	{ return Entities[GetIndexParent(id)]; }
		inline	const ::gpk::SEntity&	GetChild		(::gpk::eid_t id, s2_t iChild)	const	{ return Entities[GetIndexChild	(id, iChild)]; }
		inline	SEntity&				GetParent		(::gpk::eid_t id)							{ return Entities[GetIndexParent(id)]; }
		inline	SEntity&				GetChild		(::gpk::eid_t id, s2_t iChild)			{ return Entities[GetIndexChild	(id, iChild)]; }
		inline	::gpk::err_t			GetChildCount	(u2_t iEntity)					const	{ const ::gpk::papod<::gpk::eid_t> & children = Children[iEntity]; return children ? children->size() : 0; }
		inline	::gpk::vceid			GetChildren		(u2_t iEntity)					const	{ const ::gpk::papod<::gpk::eid_t> & children = Children[iEntity]; return children ? ::gpk::vceid{*children} : ::gpk::vceid{}; }
		inline	::gpk::veid				GetChildren		(u2_t iEntity)							{ ::gpk::papod<::gpk::eid_t> & children = Children[iEntity]; return children ? *children : ::gpk::veid{}; }


		::gpk::err_t					SetParent		(::gpk::eid_t id, ::gpk::eid_t idParent)		{
			if_true_vef(-1, idParent != ::gpk::EID_INVALID && idParent >= Entities.size(), "Invalid parent for entity %i, %i", id, idParent);
			::gpk::eid_t						& oldParent		= Entities[id].Parent;
			if(oldParent < Entities.size() && Children[oldParent]) {
				const s2_t						oldIndex		= Children[oldParent]->find(id);
				if(oldIndex >= 0)
					if_fail_fef(Children[oldParent]->remove(oldIndex), "oldParent: %i, oldIndex: %i.", oldParent, oldIndex);
			}
			oldParent						= idParent;
			return (0 > (s2_t)idParent) ? 0 : Children[idParent]->push_back(id);
		}

		::gpk::err_t					Save			(::gpk::au0_t & output) const { 
			if_fail_fe(gpk::saveView(output, Entities));
			for(u2_t iEntity = 0; iEntity < Entities.size(); ++iEntity) {
				if_fail_fef(gpk::saveView(output, Children[iEntity] ? ::gpk::vcu2_t{*Children[iEntity]} : ::gpk::vcu2_t{}), "iEntity: %i", iEntity);
				if_fail_fef(gpk::saveView(output, Names[iEntity]), "iEntity: %i", iEntity);
			}
			info_printf("Saved %s, %i", "Entities", Children.size());
			return 0;
		}

		::gpk::err_t					Load			(::gpk::vcu0_t & input) {
			if_fail_fe(gpk::loadView(input, Entities));
			if_fail_fe(Children	.resize(Entities.size()));
			if_fail_fe(Names		.resize(Entities.size()));
			for(u2_t iEntity = 0; iEntity < Entities.size(); ++iEntity) {
				if(!Children[iEntity])
					Children[iEntity].create();

				if_fail_fef(gpk::loadView(input, *Children[iEntity]), "iEntity: %i", iEntity);
				::gpk::apod<sc_t>					name;
				if_fail_fef(gpk::loadView(input, name), "iEntity: %i", iEntity);
				Names[iEntity]					= ::gpk::label(::gpk::view<sc_t>{name});
			}

			return 0; 
		}

	};
} // namespace

#endif // GPK_ENGINE_ENTITY_H
