#include "gpk_block.h"

::gpk::error_t									gpk::SMapBlock::MapId					(const ::gpk::vcc & dataToAdd)						const	{
	for(uint32_t iMap = 0; iMap < Indices.size(); ++iMap) {
		const int32_t										indexView								= Indices[iMap];
		const uint16_t										currentViewLen							= (-1 == indexView) ? 0 : Allocator.Counts[indexView];
		if(currentViewLen != dataToAdd.size())
			continue;
		if(currentViewLen && 0 == memcmp(Allocator.Views[indexView], dataToAdd.begin(), currentViewLen))
			return iMap;
	}
	return -1;
}

::gpk::error_t									gpk::SMapBlock::MapGet					(int32_t index, ::gpk::apod<char_t> & data)				const	{
	ree_if(((uint32_t)index) >= Indices.size(), "Index out of range: %i", index);
	const int32_t										indexView								= Indices[index];
	gpk_necall(data.append(Allocator.Views[indexView], Allocator.Counts[indexView]), "%s", "Out of memory?");
	return 0;
}

::gpk::error_t									gpk::SMapBlock::MapAdd					(const ::gpk::vcc & dataToAdd)								{
	::gpk::error_t										indexToReturn							= -1;
	int32_t												indexView								= dataToAdd.size() ? Allocator.View(dataToAdd.begin(), (uint16_t)dataToAdd.size()) : -1;
	gpk_necall(indexToReturn = Indices.push_back(indexView), "%s", "Out of memory?");
	return indexToReturn;
}

::gpk::error_t									gpk::SMapBlock::Save					(::gpk::apod<byte_t> & output)								const	{
	gpk_necall(::gpk::viewWrite(::gpk::view_const_uint16{Allocator.Counts.begin(), Allocator.Counts.size()}, output), "%s", "Out of memory?");
	for(uint32_t iView = 0; iView < Allocator.Counts.size(); ++iView)
		gpk_necall(output.append(Allocator.Views[iView], Allocator.Counts[iView]), "%s", "Out of memory?");

	gpk_necall(::gpk::viewWrite(::gpk::view_array<const _tIndex>{Indices.begin(), Indices.size()}, output), "%s", "Out of memory?");
	return 0;
}

::gpk::error_t									gpk::SMapBlock::Load					(const ::gpk::vcb & input)									{
	if(0 == input.size())
		return 0;
	const uint32_t										countViews								= *(const uint32_t*)input.begin();
	uint32_t											offsetViewSize							= sizeof(uint32_t);
	typedef												uint16_t								_tViewLen;
	uint32_t											offsetViewData							= offsetViewSize + sizeof(_tViewLen) * countViews;
	for(uint32_t iView = 0; iView < countViews; ++iView) {
		const _tViewLen										lenCurrentView							= *(_tViewLen*)&input[offsetViewSize];
		Allocator.View(&input[offsetViewData], lenCurrentView);
		offsetViewData									+= lenCurrentView;
		offsetViewSize									+= sizeof(_tViewLen);
	}
	::gpk::view_array<const _tIndex>					inputView;
	::gpk::viewRead(inputView, {&input[offsetViewData], input.size() - offsetViewData});
	Indices											= inputView;
	return 0;
}
