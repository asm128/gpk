#include "gpk_block_container_nts.h"

#ifndef GPK_LABEL_MANAGER_H_23627
#define GPK_LABEL_MANAGER_H_23627

namespace gpk
{
	class CLabelManager	{
#if defined(GPK_ARDUINO)
		stxp	const u2_t	BLOCK_SIZE	= 1024 >> 1;
#elif defined(GPK_ESP32)
		stxp	const u2_t	BLOCK_SIZE	= 1024 * 4;
#else
		stxp	const u2_t	BLOCK_SIZE	= 1024 * 64;
#endif
		block_container_nts<BLOCK_SIZE>	Characters;
		vcst_t							Empty;

	public:	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		au2_t							Counts;
		apod<sc_c*>						Texts;

										~CLabelManager				() {
			for(u2_t iText = 0; iText < Texts.size(); ++iText)
				verbose_printf("Label found: %s.", Texts[iText]);
		}
		inln							CLabelManager				() { 
			Characters	.push_sequence("", 0U, Empty); 
			Counts		.push_back(0); 
			Texts		.push_back(Empty.begin()); 
		}

		inln	err_t					Size						()					csnx	{ return Texts.size(); }
		inln	vcst_t					View						(u2_t index)		cnst	{ return {Texts[index], Counts[index]}; }
		err_t							Save						(au0_t & output)	cnst	{
			if_fail_fe(gpk::saveView(output, Counts));
			for(u2_t iArray = 0; iArray < Counts.size(); ++iArray) {
				if_fail_fe(output.append((const u0_t*)Texts[iArray], Counts[iArray]));
			}
			return 0;
		}
		err_t							Load						(vcu0_t & input)			{
			if_fail_fe(gpk::loadView(input, Counts));
			vcst_t					out_view; 
			u2_t					offsetByte					= 0;
			for(u2_t iArray = 0; iArray < Counts.size(); ++iArray) {
				u2_t					elementCount				= Counts[iArray];
				if_fail_fe(Characters.push_sequence((const sc_t*)&input[offsetByte], elementCount, out_view));
				offsetByte				+= elementCount;
				if_fail_fe(Texts.push_back(out_view.begin()));
			}
			input					= {input.begin(), input.size() - offsetByte};
			return 0;
		}

		err_t							Index		(const vcst_t & elements) {
			if_true_vef(-1, elements.size() >= CLabelManager::BLOCK_SIZE, "Data too large: %u.", elements.size());

			for(u2_t iView = 0, countLabels = Texts.size(); iView < countLabels; ++iView) {
				if(elements.size() != Counts[iView])
					continue;

				const sc_t								* pStored					= Texts[iView];
				if(0 == memcmp(pStored, elements.begin(), elements.byte_count()))
					return iView;
			}
			return -1;
		}

		err_t							View		(vcst_t & inout_view)		{
			if(0 == inout_view.size()) {
				return 0;
			}

			u2_t								ntslen						= 0;
			for(const u2_t countChars = min(inout_view.size(), CLabelManager::BLOCK_SIZE - 1); ntslen < countChars; ++ntslen) {
				if(0 == inout_view[ntslen])
					break;
			}

			const u2_t							viewIndex					= (u2_t)Index({inout_view.begin(), ntslen});
			if(viewIndex < Texts.size()) {
				inout_view				= {Texts[viewIndex], Counts[viewIndex]};
				return viewIndex;
			}
			if_fail_fe(Characters.push_sequence(inout_view, ntslen, inout_view));
			if_fail_fe(Texts.push_back(inout_view.begin()));
			return Counts.push_back((u1_t)inout_view.size());
		}
	};
}

#endif // GPK_LABEL_MANAGER_H_23627
