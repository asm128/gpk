#include "gpk_block.h"

::gpk::error_t									gpk::SMapBlock::GetMapId				(const ::gpk::view_const_char & dataToAdd)						const	{
	for(uint32_t iMap = 0; iMap < Indices.size(); ++iMap) {
		const int32_t										indexView							= Indices[iMap];
		const uint16_t										currentViewLen						= (-1 == indexView) ? 0 : Allocator.Counts[indexView];
		if(currentViewLen != dataToAdd.size())
			continue;
		if(currentViewLen && 0 == memcmp(Allocator.Views[indexView], dataToAdd.begin(), currentViewLen))
			return iMap;
	}
	return -1;
}

::gpk::error_t									gpk::SMapBlock::GetMap					(int32_t index, ::gpk::array_pod<char_t> & data)				const	{
	ree_if(((uint32_t)index) >= Indices.size(), "Index out of range: %i", index);
	const int32_t										indexView								= Indices[index];
	gpk_necall(data.append(Allocator.Views[indexView], Allocator.Counts[indexView]), "%s", "Out of memory?");
	return 0;
}

::gpk::error_t									gpk::SMapBlock::AddMap					(const ::gpk::view_const_char & dataToAdd)								{
	::gpk::error_t										indexToReturn							= -1;
	int32_t												indexView								= dataToAdd.size() ? Allocator.View(dataToAdd.begin(), (uint16_t)dataToAdd.size()) : -1;
	gpk_necall(indexToReturn = Indices.push_back(indexView), "%s", "Out of memory?");
	return indexToReturn;
}

::gpk::error_t									gpk::SMapBlock::Save					(::gpk::array_pod<byte_t> & output)								const	{
	gpk_necall(::gpk::viewWrite(::gpk::view_const_uint16{Allocator.Counts.begin(), Allocator.Counts.size()}, output), "%s", "Out of memory?");
	for(uint32_t iView = 0; iView < Allocator.Counts.size(); ++iView)
		gpk_necall(output.append(Allocator.Views[iView], Allocator.Counts[iView]), "%s", "Out of memory?");

	gpk_necall(::gpk::viewWrite(::gpk::view_array<const _tIndex>{Indices.begin(), Indices.size()}, output), "%s", "Out of memory?");
	return 0;
}

::gpk::error_t									gpk::SMapBlock::Load					(const ::gpk::view_const_byte & input)									{
	if(0 == input.size())
		return 0;
	const uint32_t										countArrays								= *(const uint32_t*)input.begin();
	uint32_t											offsetViewSize							= sizeof(uint32_t);
	typedef												uint16_t								_tViewLen;
	uint32_t											offsetViewData							= offsetViewSize + sizeof(_tViewLen) * countArrays;
	for(uint32_t iView = 0; iView < countArrays; ++iView) {
		const _tViewLen										currentArraySize						= *(_tViewLen*)&input[offsetViewSize];
		Allocator.View(&input[offsetViewData], currentArraySize);
		offsetViewData									+= currentArraySize;
		offsetViewSize									+= sizeof(_tViewLen);
	}
	const uint32_t										countMaps								= *(const uint32_t*)&input[offsetViewData];
	const uint32_t										offsetIndices							= offsetViewData + sizeof(uint32_t);
	Indices											= ::gpk::view_array<_tIndex>{(_tIndex*)&input[offsetIndices], countMaps};
	return 0;
}
