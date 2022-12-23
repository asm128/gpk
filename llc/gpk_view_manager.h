#include "gpk_array.h"
#include "gpk_array_static.h"
#include "gpk_ptr.h"

#ifndef GPK_VIEW_MANAGER_H_29037492837
#define GPK_VIEW_MANAGER_H_29037492837

namespace gpk
{
	template<typename _tElement, size_t _blockSize>
	class unshrinkable_view_container {
		typedef				::gpk::pobj<::gpk::array_static<_tElement, _blockSize>>	ptr_block_type;
							::gpk::array_obj<ptr_block_type>							Blocks;
							::gpk::array_pod<uint32_t>									RemainingSpace;
	public:
							::gpk::error_t												push_sequence				(const _tElement* sequence, uint32_t length, ::gpk::view_array<const _tElement>& out_view)	{
			for(uint32_t iBlock = 0; iBlock < Blocks.size(); ++iBlock) {
				uint32_t																			& blkRemainingSpace			= RemainingSpace[iBlock];
				if(blkRemainingSpace >= length) {
					_tElement																			* sequenceStart				= &Blocks[iBlock]->operator[](_blockSize - blkRemainingSpace);
					out_view																		= {sequenceStart, length};
					memcpy(sequenceStart, sequence, length * sizeof(_tElement));
					blkRemainingSpace																-= length;
					return 0;
				}
			}
			int32_t																				indexNewBlock				= Blocks.size();
			Blocks			.resize(Blocks.size() + 1);
			RemainingSpace	.resize(Blocks.size(), _blockSize - length);
			out_view																		= {&Blocks[indexNewBlock]->operator[](0), length};
			memcpy(Blocks[indexNewBlock]->Storage, sequence, length * sizeof(_tElement));
			return indexNewBlock;
		}
	};

	template<typename _tElement>
	class CViewManager	{
		static constexpr	const uint32_t												BLOCK_SIZE					= 0xFFFF;
							::gpk::unshrinkable_view_container<_tElement, BLOCK_SIZE>	Elements;
	public:
							::gpk::array_pod<uint16_t>									Counts;
							::gpk::array_pod<const _tElement*>							Views;

																						~CViewManager				()																					{}

			inline			::gpk::error_t												View						(const _tElement* elements, uint16_t count)													{ ::gpk::view_array<const _tElement> out_view; return View(elements, count, out_view); }
							::gpk::error_t												View						(const _tElement* elements, uint16_t count, ::gpk::view_array<const _tElement>& out_view)	{
			if(0 == count || 0 == elements) {
				out_view																		= {};
				return -1;
			}
			const uint32_t																		totalChars					= ::gpk::min((uint32_t)count, CViewManager::BLOCK_SIZE);
			for(uint32_t iView = 0, countLabels = Views.size(); iView < countLabels; ++iView) {
				if(totalChars != Counts[iView])
					continue;
				const _tElement																		* pStored					= Views[iView];
				if(0 == memcmp(pStored, elements, totalChars * sizeof(_tElement))) {
					out_view																	= {pStored, totalChars};
					return iView;
				}
			}
			gpk_necall(Elements.push_sequence(elements, totalChars, out_view)	, "%s", "Out of memory?");
			const uint32_t																		newIndex					= Counts.push_back((uint16_t)out_view.size());
			gpk_necall(newIndex, "%s", "Out of memory?");
			gpk_necall(Views.push_back(out_view.begin())					, "%s", "Out of memory?");
			return newIndex;
		}
	};

	template<typename _tElement>
			::gpk::error_t																viewManagerSave								(const ::gpk::CViewManager<_tElement> & allocator, ::gpk::array_pod<byte_t> & output) {
		gpk_necall(::gpk::viewWrite(::gpk::view_const_uint16{allocator.Counts.begin(), allocator.Counts.size()}, output), "%s", "Out of memory?");
		for(uint32_t iArray = 0; iArray < allocator.Counts.size(); ++iArray)
			gpk_necall(output.append((const char*)allocator.Views[iArray], sizeof(_tElement) * allocator.Counts[iArray]), "%s", "Out of memory?");
		return 0;
	}

	template<typename _tElement>
			::gpk::error_t																viewManagerLoad								(::gpk::CViewManager<_tElement> & allocator, const ::gpk::view_const_byte & input) {
		typedef																					uint16_t									_tViewLen;
		const uint32_t																			countViewsChar								= *(const uint32_t*)&input[0];
		uint32_t																				offsetArraySize								= sizeof(uint32_t);
		uint32_t																				offsetArrayData								= offsetArraySize + sizeof(_tViewLen) * countViewsChar;
		for(uint32_t iArray = 0; iArray < countViewsChar; ++iArray) {
			const _tViewLen																			currentArraySize							= *(_tViewLen*)&input[offsetArraySize];
			gpk_necall(allocator.View((const _tElement*)&input[offsetArrayData], currentArraySize), "%s", "Out of memory?");
			offsetArrayData																		+= currentArraySize * sizeof(_tElement);
			offsetArraySize																		+= sizeof(_tViewLen);
		}
		return offsetArrayData;
	}
} // namespace

#endif // GPK_LABEL_MANAGER_H_29037492837
