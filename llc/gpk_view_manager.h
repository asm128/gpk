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
		static constexpr	const uint32_t												BLOCK_SIZE					= _blockSize;
							::gpk::unshrinkable_view_container<_tElement, BLOCK_SIZE>	Elements;
							::gpk::array_pod<uint32_t>									Counts;
							::gpk::array_pod<const _tElement*>							Views;
	public:
																						~CViewManager				()																					{}

							::gpk::error_t												View						(const _tElement* elements, uint32_t count, ::gpk::view_array<const _tElement>& out_view)		{
			if(0 == count || 0 == elements) {
				out_view																		= {};
				return 0;
			}
			const uint32_t																		totalChars					= ::gpk::min(count, CViewManager::BLOCK_SIZE);
			for(uint32_t iLabel = 0, countLabels = Views.size(); iLabel < countLabels; ++iLabel) {
				if(totalChars != Counts[iLabel])
					continue;
				const _tElement																		* pStored					= Views[iLabel];
				if(0 == memcmp(pStored, elements, totalChars)) {
					out_view																	= {pStored, totalChars};
					return totalChars;
				}
			}
			gpk_necall(Elements		.push_sequence(elements, totalChars, out_view)	, "%s", "Out of memory?");
			gpk_necall(Counts		.push_back(out_view.size	())					, "%s", "Out of memory?");
			gpk_necall(Views		.push_back(out_view.begin	())					, "%s", "Out of memory?");
			return 0;
		}
	};
}

#endif // GPK_LABEL_MANAGER_H_29037492837
