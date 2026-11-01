#include "gpk_array_ptr.h"
#include "gpk_label.h"
#include "gpk_string.h"

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
		eid_t				Parent			= EID_INVALID;
	};
#pragma pack(pop)

	struct SVirtualEntityManager {
		apod<SEntity>			Entities		= {};
		aobj<vcst_t>			Names			= {};

		aobj<paeid>				Children		= {};

		const SEntity&			operator[]		(eid_t id)	const	{ return Entities[id]; }
		SEntity&				operator[]		(eid_t id)			{ return Entities[id]; }

		u2_t					size			()			const	{ return Names.size(); }
		u2_t					clear			()					{ ::gpk::clear(Entities, Names, Children); return 0; }

		err_t 					Create			() {
			Children.push_back({});
			Names.push_back({});
			return Entities.push_back({});
		}

		err_t 					Delete			(eid_t id) {
			Children.remove_unordered(id);
			Names.remove_unordered(id);
			return Entities.remove_unordered(id);
		}

		inln	err_t 			GetIndexParent	(eid_t id)					const	{ return Entities[id].Parent; }
		inln	err_t 			GetIndexChild	(eid_t id, s2_t iChild)		const	{ return (*Children[id])[iChild]; }
		inln	const SEntity&	GetParent		(eid_t id)					const	{ return Entities[GetIndexParent(id)]; }
		inln	const SEntity&	GetChild		(eid_t id, s2_t iChild)		const	{ return Entities[GetIndexChild	(id, iChild)]; }
		inln	SEntity&		GetParent		(eid_t id)							{ return Entities[GetIndexParent(id)]; }
		inln	SEntity&		GetChild		(eid_t id, s2_t iChild)				{ return Entities[GetIndexChild	(id, iChild)]; }
		inln	err_t 			GetChildCount	(u2_t iEntity)				const	{ const papod<eid_t> & children = Children[iEntity]; return children ? children->size() : 0; }
		inln	vceid			GetChildren		(u2_t iEntity)				const	{ const papod<eid_t> & children = Children[iEntity]; return children ? vceid{*children} : vceid{}; }
		inln	veid			GetChildren		(u2_t iEntity)						{ papod<eid_t> & children = Children[iEntity]; return children ? *children : veid{}; }


		err_t 					SetParent		(eid_t id, eid_t idParent)		{
			if_true_vef(-1, idParent != EID_INVALID && idParent >= Entities.size(), "Invalid parent for entity %i, %i", id, idParent);
			eid_t						& oldParent		= Entities[id].Parent;
			if(oldParent < Entities.size() && Children[oldParent]) {
				const s2_t						oldIndex		= Children[oldParent]->find(id);
				if(oldIndex >= 0)
					if_fail_fef(Children[oldParent]->remove(oldIndex), "oldParent: %i, oldIndex: %i.", oldParent, oldIndex);
			}
			oldParent						= idParent;
			return (0 > (s2_t)idParent) ? 0 : Children[idParent]->push_back(id);
		}

		err_t 					Save			(au0_t & output) const { 
			if_fail_fe(saveView(output, Entities));
			for(u2_t iEntity = 0; iEntity < Entities.size(); ++iEntity) {
				if_fail_fef(saveView(output, Children[iEntity] ? vcu2_t{*Children[iEntity]} : vcu2_t{}), "iEntity: %i", iEntity);
				if_fail_fef(saveView(output, Names[iEntity]), "iEntity: %i", iEntity);
			}
			info_printf("Saved %s, %i", "Entities", Children.size());
			return 0;
		}

		err_t 					Load			(vcu0_t & input) {
			if_fail_fe(loadView(input, Entities));
			if_fail_fe(Children	.resize(Entities.size()));
			if_fail_fe(Names		.resize(Entities.size()));
			for(u2_t iEntity = 0; iEntity < Entities.size(); ++iEntity) {
				if(!Children[iEntity])
					Children[iEntity].create();

				if_fail_fef(loadView(input, *Children[iEntity]), "iEntity: %i", iEntity);
				string					name;
				if_fail_fef(loadView(input, name), "iEntity: %i", iEntity);
				Names[iEntity]					= label(name);
			}

			return 0; 
		}

	};
} // namespace

#endif // GPK_ENGINE_ENTITY_H
