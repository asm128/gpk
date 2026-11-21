#include "gpk_block_container.h"

#ifndef GPK_VIEW_MANAGER_H_23627
#define GPK_VIEW_MANAGER_H_23627

nmsp gpk
{
	tplt<tpnm T>
	class CViewManager	{
#if defined(GPK_ARDUINO)
		stxp	u2_c	BLOCK_SIZE	= 1024 >> 1;
#elif defined(GPK_ESP32)
		stxp	u2_c	BLOCK_SIZE	= 1024 * 4;
#else
		stxp	u2_c	BLOCK_SIZE	= 1024 * 64;
#endif
		::gpk::block_container<T, BLOCK_SIZE>	Cells;

	public:
		::gpk::au1_t					Counts;
		::gpk::apod<cnst T*>			Views;

										~CViewManager				()				{}

		gpk::err_t 				Save						(::gpk::apod<u0_t> & output)	cnst	{
			if_fail_fe(gpk::saveView(output, Counts));
			for(u2_t iArray = 0; iArray < Counts.size(); ++iArray)
				if_fail_fe(output.append((cnst u0_t*)Views[iArray], Counts[iArray] * sizeof(T)));

			return 0;
		}

		gpk::err_t 				Load						(::gpk::vcu0_t & input) {
			if_fail_fe(gpk::loadView(input, Counts));
			::gpk::view<cnst T>				out_view; 
			u2_t							offsetByte					= 0;
			for(u2_t iArray = 0; iArray < Counts.size(); ++iArray) {
				u2_c							elementCount				= Counts[iArray];
				u2_c							byteCount					= elementCount * sizeof(T);
				if_fail_fe(Cells.push_sequence((cnst T*)&input[offsetByte], elementCount, out_view));
				offsetByte						+= byteCount;
				if_fail_fe(Views.push_back(out_view.begin()));
			}
			input							= {input.begin(), input.size() - offsetByte};
			return 0;
		}

		inln	::gpk::view<cnst T>	View		(u2_t index)							cnst	{ return {Views[index], Counts[index]}; }
		inln	gpk::err_t 		View		(cnst T * elements, u1_t count)			{ ::gpk::view<cnst T> out_view; return View(elements, count, out_view); }
		gpk::err_t 				Index		(cnst ::gpk::view<cnst T> & elements) {
			if_true_fef(elements.size() > CViewManager::BLOCK_SIZE, "Data too large: %u.", elements.size());

			for(u2_t iView = 0, countLabels = Views.size(); iView < countLabels; ++iView) {
				if(elements.size() != Counts[iView])
					continue;

				cnst T								* pStored					= Views[iView];
				if(0 == memcmp(pStored, elements.begin(), elements.byte_count()))
					return iView;
			}
			return -1;
		}

		gpk::err_t 				View		(cnst T * elements, u1_t count, ::gpk::view<cnst T> & out_view)	{
			if(0 == count || 0 == elements) {
				out_view						= {};
				return -1;
			}
			u2_c								totalChars					= ::gpk::min((u2_t)count, CViewManager::BLOCK_SIZE);
			u2_c								viewIndex					= (u2_t)Index({elements, totalChars});
			if(viewIndex < Views.size()) {
				out_view								= {Views[viewIndex], Counts[viewIndex]};
				return viewIndex;
			}
			if_fail_fe(Cells.push_sequence(elements, totalChars, out_view));
			u2_t									newIndex;
			if_fail_fe(newIndex = Counts.push_back((u1_t)out_view.size()));
			if_fail_fe(Views.push_back(out_view.begin()));
			return newIndex;
		}
	};
} // nmsp

#endif // GPK_LABEL_MANAGER_H_23627
