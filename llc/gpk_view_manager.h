#include "gpk_block_container.h"

#ifndef GPK_VIEW_MANAGER_H_23627
#define GPK_VIEW_MANAGER_H_23627

namespace gpk
{
	tplt<tpnm T>
	class CViewManager	{
#ifdef GPK_ESP32
		stacxpr	const uint32_t	BLOCK_SIZE	= 1024 * 4;
#else
		stacxpr	const uint32_t	BLOCK_SIZE	= 1024 * 64;
#endif
		::gpk::block_container<T, BLOCK_SIZE>	Cells;

	public:
		::gpk::au16								Counts;
		::gpk::apod<const T*>					Views;

												~CViewManager				()				{}

		::gpk::error_t							Save						(::gpk::apod<uint8_t> & output)	const	{
			gpk_necs(::gpk::saveView(output, Counts));
			for(uint32_t iArray = 0; iArray < Counts.size(); ++iArray)
				gpk_necs(output.append((const uint8_t*)Views[iArray], Counts[iArray] * sizeof(T)));

			return 0;
		}

		::gpk::error_t							Load						(::gpk::vcu8 & input) {
			gpk_necs(::gpk::loadView(input, Counts));
			::gpk::view<const T>						out_view; 
			uint32_t									offsetByte					= 0;
			for(uint32_t iArray = 0; iArray < Counts.size(); ++iArray) {
				uint32_t									elementCount				= Counts[iArray];
				uint32_t									byteCount					= elementCount * sizeof(T);
				gpk_necs(Cells.push_sequence((const T*)&input[offsetByte], elementCount, out_view));
				offsetByte								+= byteCount;
				gpk_necs(Views.push_back(out_view.begin()));
			}
			input									= {input.begin(), input.size() - offsetByte};
			return 0;
		}

		inline	::gpk::view<const T>			View						(uint32_t index)							const	{ return {Views[index], Counts[index]}; }
		inline	::gpk::error_t					View		(const T * elements, uint16_t count)			{ ::gpk::view<const T> out_view; return View(elements, count, out_view); }

		::gpk::error_t							Index		(const ::gpk::view<const T> & elements) {
			ree_if(elements.size() > CViewManager::BLOCK_SIZE, "Data too large: %u.", elements.size());

			for(uint32_t iView = 0, countLabels = Views.size(); iView < countLabels; ++iView) {
				if(elements.size() != Counts[iView])
					continue;

				const T										* pStored					= Views[iView];
				if(0 == memcmp(pStored, elements.begin(), elements.byte_count()))
					return iView;
			}
			return -1;
		}

		::gpk::error_t							View		(const T * elements, uint16_t count, ::gpk::view<const T>& out_view)	{
			if(0 == count || 0 == elements) {
				out_view								= {};
				return -1;
			}
			const uint32_t								totalChars					= ::gpk::min((uint32_t)count, CViewManager::BLOCK_SIZE);
			const uint32_t								viewIndex					= (uint32_t)Index({elements, totalChars});
			if(viewIndex < Views.size()) {
				out_view								= {Views[viewIndex], Counts[viewIndex]};
				return viewIndex;
			}
			gpk_necs(Cells.push_sequence(elements, totalChars, out_view));
			uint32_t									newIndex;
			gpk_necs(newIndex = Counts.push_back((uint16_t)out_view.size()));
			gpk_necs(Views.push_back(out_view.begin()));
			return newIndex;
		}
	};
} // namespace

#endif // GPK_LABEL_MANAGER_H_23627
