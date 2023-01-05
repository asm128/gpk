#include "gpk_ptr.h"

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
	};
} // namespace

#endif
