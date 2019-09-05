#include "gpk_array.h"
#include "gpk_array_static.h"
#include "gpk_ptr.h"

#ifndef GPK_VIEW_MANAGER_H_29037492837
#define GPK_VIEW_MANAGER_H_29037492837

namespace gpk
{
	template<typename _tElement, size_t _blockSize>
	class unshrinkable_view_container {
		typedef				::gpk::ptr_obj<::gpk::array_static<_tElement, _blockSize>>	ptr_block_type;
							::gpk::array_obj<ptr_block_type>							Blocks;
							::gpk::array_pod<uint32_t>									RemainingSpace;
	public:
							::gpk::error_t												push_sequence				(const char* sequence, uint32_t length, ::gpk::view_array<const _tElement>& out_view)	{
			for(uint32_t iBlock = 0; iBlock < Blocks.size(); ++iBlock) {
				uint32_t																			& blkRemainingSpace			= RemainingSpace[iBlock];
				if(blkRemainingSpace >= length) {
					char																				* sequenceStart				= &Blocks[iBlock]->operator[](_blockSize - blkRemainingSpace);
					out_view																		= {sequenceStart, length};
					memcpy(sequenceStart, sequence, length);
					blkRemainingSpace																-= length;
					return 0;
				}
			}
			int32_t																				indexNewBlock				= Blocks.size();
			Blocks			.resize(Blocks.size() + 1);
			RemainingSpace	.resize(Blocks.size() + 1, _blockSize - length - 1);
			out_view																		= {&Blocks[indexNewBlock]->operator[](0), length};
			return indexNewBlock;
		}
	};

	template<typename _tElement, size_t _blockSize>
	class CViewManager	{
		static constexpr	const uint32_t												BLOCK_SIZE					= (uint32_t)_blockSize;
							::gpk::unshrinkable_view_container<_tElement, BLOCK_SIZE>	Elements;
	public:
							::gpk::array_pod<uint16_t>									Counts;
							::gpk::array_pod<const _tElement*>							Views;

																						~CViewManager				()																					{}
																						CViewManager				()																					{ Views.push_back(0); Counts.push_back(0); }

			inline			::gpk::error_t												View						(const _tElement* elements, uint16_t count)													{ ::gpk::view_array<const _tElement> out_view; return View(elements, count, out_view); }
							::gpk::error_t												View						(const _tElement* elements, uint16_t count, ::gpk::view_array<const _tElement>& out_view)	{
			if(0 == count || 0 == elements) {
				out_view																		= {};
				return 0;
			}
			const uint32_t																		totalChars					= ::gpk::min((uint32_t)count, CViewManager::BLOCK_SIZE);
			for(uint32_t iView = 0, countLabels = Views.size(); iView < countLabels; ++iView) {
				if(totalChars != Counts[iView])
					continue;
				const _tElement																		* pStored					= Views[iView];
				if(0 == memcmp(pStored, elements, totalChars)) {
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
}

#endif // GPK_LABEL_MANAGER_H_29037492837
