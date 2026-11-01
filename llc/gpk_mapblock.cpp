#include "gpk_block.h"
/*
gpk::err_t 		gpk::SMapBlock::MapGet					(u2_t index, ::gpk::vcu0_t & data)				const	{
	if_true_vef(-1, index >= Allocator.Views.size(), "Index out of range: %i", index);
	data											= {Allocator.Views[index], Allocator.Counts[index]};
	return 0;
}

gpk::err_t 		gpk::SMapBlock::MapId					(::gpk::vcu0_c & dataToAdd)						const	{
	if_true_vef(-1, 0 == dataToAdd.size(), "%s", "Empty data is not allowed");
	for(u2_t iMap = 0; iMap < Allocator.Views.size(); ++iMap) {
		const u1_t										currentViewLen							= Allocator.Counts[iMap];
		if(currentViewLen != dataToAdd.size())
			continue;

		if(currentViewLen && 0 == memcmp(Allocator.Views[iMap], dataToAdd.begin(), currentViewLen))
			return iMap;
	}
	return -1;
}
*/