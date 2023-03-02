#include "gpk_block.h"

::gpk::error_t									gpk::SMapBlock::MapAdd					(const ::gpk::vcub & dataToAdd)								{
	return Allocator.View(dataToAdd.begin(), (uint16_t)dataToAdd.size());
}

::gpk::error_t									gpk::SMapBlock::MapGet					(int32_t index, ::gpk::vcub & data)				const	{
	ree_if(((uint32_t)index) >= Allocator.Views.size(), "Index out of range: %i", index);
	data											= {Allocator.Views[index], Allocator.Counts[index]};
	return 0;
}

::gpk::error_t									gpk::SMapBlock::MapId					(const ::gpk::vcub & dataToAdd)						const	{
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


