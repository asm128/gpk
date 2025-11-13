#include "gpk_block.h"

::gpk::error_t			gpk::SMapBlock::MapGet					(uint32_t index, ::gpk::vcu0_t & data)				const	{
	ree_if(index >= Allocator.Views.size(), "Index out of range: %i", index);
	data											= {Allocator.Views[index], Allocator.Counts[index]};
	return 0;
}

::gpk::error_t			gpk::SMapBlock::MapId					(::gpk::vcu0_c & dataToAdd)						const	{
	ree_if(0 == dataToAdd.size(), "%s", "Empty data is not allowed");
	for(uint32_t iMap = 0; iMap < Allocator.Views.size(); ++iMap) {
		const uint16_t										currentViewLen							= Allocator.Counts[iMap];
		if(currentViewLen != dataToAdd.size())
			continue;

		if(currentViewLen && 0 == memcmp(Allocator.Views[iMap], dataToAdd.begin(), currentViewLen))
			return iMap;
	}
	return -1;
}
