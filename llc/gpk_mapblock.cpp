#include "gpk_block.h"

::gpk::error_t									gpk::SMapBlock::Save						(::gpk::array_pod<byte_t> & output)								const	{
	gpk_necall(::gpk::viewWrite(::gpk::view_const_uint16{Allocator.Counts.begin(), Allocator.Counts.size()}, output), "%s", "Out of memory?");
	for(uint32_t iArray = 0; iArray < Allocator.Counts.size(); ++iArray)
		gpk_necall(output.append(Allocator.Views[iArray], Allocator.Counts[iArray]), "%s", "Out of memory?");

	gpk_necall(::gpk::viewWrite(::gpk::view_array<const _tIndex>{Indices.begin(), Indices.size()}, output), "%s", "Out of memory?");
	return 0;
}

::gpk::error_t									gpk::SMapBlock::Load						(const ::gpk::view_const_byte & input)									{
	if(0 == input.size())
		return 0;
	const uint32_t										countArrays								= *(const uint32_t*)input.begin();
	uint32_t											offsetArraySize							= sizeof(uint32_t);
	typedef												uint16_t								_tViewLen;
	uint32_t											offsetArrayData							= offsetArraySize + sizeof(_tViewLen) * countArrays;
	for(uint32_t iArray = 0; iArray < countArrays; ++iArray) {
		const _tViewLen										currentArraySize						= *(_tViewLen*)&input[offsetArraySize];
		Allocator.View(&input[offsetArrayData], currentArraySize);
		offsetArrayData									+= currentArraySize;
		offsetArraySize									+= sizeof(_tViewLen);
	}
	const uint32_t										countMaps								= *(const uint32_t*)&input[offsetArrayData];
	const uint32_t										offsetIndices							= offsetArrayData + sizeof(uint32_t);
	Indices											= ::gpk::view_array<_tIndex>{(_tIndex*)&input[offsetIndices], countMaps};
	return 0;
}

::gpk::error_t									gpk::SMapBlock::GetMapId					(const ::gpk::view_const_char & dataToAdd)						const	{
	for(uint32_t iAddress = 0; iAddress < Indices.size(); ++iAddress) {
		const int32_t										idDomain							= Indices[iAddress];
		const uint16_t										currentViewLen						= (-1 == idDomain) ? 0 : Allocator.Counts[idDomain];
		if(currentViewLen != dataToAdd.size())
			continue;
		if(currentViewLen && 0 == memcmp(Allocator.Views[idDomain], dataToAdd.begin(), currentViewLen))
			return iAddress;
	}
	return -1;
}

::gpk::error_t									gpk::SMapBlock::GetMap						(int32_t index, ::gpk::array_pod<char_t> & data)				const	{
	ree_if(((uint32_t)index) >= Indices.size(), "Index out of range: %i", index);
	const int32_t										indexViewData							= Indices[index];
	gpk_necall(data.append(Allocator.Views[indexViewData], Allocator.Counts[indexViewData]), "%s", "Out of memory?");
	return 0;
}

::gpk::error_t									gpk::SMapBlock::AddMap						(const ::gpk::view_const_char & dataToAdd)								{
	::gpk::error_t										indexToReturn							= -1;
	int32_t												idxData									= dataToAdd.size() ? Allocator.View(dataToAdd.begin(), (uint16_t)dataToAdd.size()) : -1;
	gpk_necall(indexToReturn = Indices.push_back(idxData), "%s", "Out of memory?");
	return indexToReturn;
}
