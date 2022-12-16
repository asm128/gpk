#include "gpk_ptr.h"

#ifndef GPK_ENGINE_CONTAINER_H
#define GPK_ENGINE_CONTAINER_H

namespace gpk
{
	template <typename _tElement> 
	struct SLinearMap {
		::gpk::array_pobj<_tElement>				Elements		= {};
		::gpk::array_obj<::gpk::vcc>				Names			= {};

		const ::gpk::ptr_obj<_tElement>&			operator[]		(uint32_t index)								const	{ return Elements[index]; }
		::gpk::ptr_obj<_tElement>&					operator[]		(uint32_t index)										{ return Elements[index]; }

		uint32_t									size			()												const	{ return Names.size(); }
		::gpk::error_t								push_back		(const ::gpk::ptr_obj<_tElement> & instance)			{ Names.push_back({});				return Elements.push_back(instance); }

		::gpk::error_t								Create			()														{ Names.push_back({});				return Elements.push_back({}); }
		::gpk::error_t								Delete			(uint32_t index)										{ Names.remove_unordered(index);	return Elements.remove_unordered(index); }
		::gpk::error_t								Clone			(uint32_t index)										{ 
			::gpk::ptr_obj<_tElement>						& newElement	= Elements[Elements.push_back({})]; 
			const ::gpk::ptr_obj<_tElement>					& srcElement	= Elements[index];
			if(srcElement) {
				*newElement.create() = *srcElement;
			}
			return Names.push_back(::gpk::vcc{Names[index]}); 
		}
	};

} // namespace

#endif // GPK_ENGINE_CONTAINER_H