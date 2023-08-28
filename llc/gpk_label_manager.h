#include "gpk_block_container_nts.h"

#ifndef GPK_LABEL_MANAGER_H_23627
#define GPK_LABEL_MANAGER_H_23627

namespace gpk
{
	class CLabelManager	{
#if defined(GPK_ARDUINO)
		stacxpr	const uint32_t	BLOCK_SIZE	= 1024 >> 1;
#elif defined(GPK_ESP32)
		stacxpr	const uint32_t	BLOCK_SIZE	= 1024 * 4;
#else
		stacxpr	const uint32_t	BLOCK_SIZE	= 1024 * 64;
#endif
		::gpk::block_container_nts<BLOCK_SIZE>	Characters;
		::gpk::vcc							Empty;

	public:	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		::gpk::au32							Counts;
		::gpk::apod<const char*>			Texts;

											~CLabelManager				()																{
			for(uint32_t iText = 0; iText < Texts.size(); ++iText)
				verbose_printf("Label found: %s.", Texts[iText]);
		}
		inline								CLabelManager				()																{ 
			Characters	.push_sequence("", 0U, Empty); 
			Counts		.push_back(0); 
			Texts		.push_back(Empty.begin()); 
		}

		::gpk::error_t						Save		(::gpk::au8 & output)						const	{
			gpk_necs(::gpk::saveView(output, Counts));
			for(uint32_t iArray = 0; iArray < Counts.size(); ++iArray)
				gpk_necs(output.append((const uint8_t*)Texts[iArray], Counts[iArray]));

			return 0;
		}

		::gpk::error_t						Load		(::gpk::vcu8 & input) {
			gpk_necs(::gpk::loadView(input, Counts));
			::gpk::vcc					out_view; 
			uint32_t					offsetByte					= 0;
			for(uint32_t iArray = 0; iArray < Counts.size(); ++iArray) {
				uint32_t					elementCount				= Counts[iArray];
				gpk_necs(Characters.push_sequence((const char*)&input[offsetByte], elementCount, out_view));
				offsetByte				+= elementCount;
				gpk_necs(Texts.push_back(out_view.begin()));
			}
			input					= {input.begin(), input.size() - offsetByte};
			return 0;
		}

		inline	::gpk::error_t				Size		()					const	noexcept	{ return Texts.size(); }
		inline	::gpk::vcc					View		(uint32_t index)	const				{ return {Texts[index], Counts[index]}; }
		inline	::gpk::error_t				View		(const char* elements, uint16_t count)	{ ::gpk::vcc out_view; return View(elements, count, out_view); }

		::gpk::error_t						Index		(const ::gpk::vcc & elements) {
			ree_if(elements.size() >= CLabelManager::BLOCK_SIZE, "Data too large: %u.", elements.size());

			for(uint32_t iView = 0, countLabels = Texts.size(); iView < countLabels; ++iView) {
				if(elements.size() != Counts[iView])
					continue;

				const char								* pStored					= Texts[iView];
				if(0 == memcmp(pStored, elements.begin(), elements.byte_count()))
					return iView;
			}
			return -1;
		}

		::gpk::error_t						View		(const char* text, uint32_t textLen, ::gpk::vcc & out_view)		{
			if(0 == textLen || 0 == text || 0 == text[0]) {
				out_view							= Empty;
				return 0;
			}

			uint32_t								ntslen						= 0;
			for(const uint32_t countChars = ::gpk::min(textLen, CLabelManager::BLOCK_SIZE - 1); ntslen < countChars; ++ntslen)
				if(0 == text[ntslen])
					break;

			const uint32_t							viewIndex					= (uint32_t)Index({text, ntslen});
			if(viewIndex < Texts.size()) {
				out_view							= {Texts[viewIndex], Counts[viewIndex]};
				return viewIndex;
			}

			gpk_necs(Characters	.push_sequence(text, ntslen, out_view));
			gpk_necs(Texts		.push_back(out_view.begin()));
			return Counts		.push_back((uint16_t)out_view.size());
		}
	};
}

#endif // GPK_LABEL_MANAGER_H_23627
