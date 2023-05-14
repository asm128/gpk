#include "gpk_array.h"
#include "gpk_array_static.h"
#include "gpk_ptr.h"

#ifndef GPK_LABEL_MANAGER_H_29037492837
#define GPK_LABEL_MANAGER_H_29037492837

namespace gpk
{
	template<size_t _blockSize>
	class block_string_container {
		::gpk::apobj<::gpk::astatic<char_t, _blockSize>>	Blocks;
		::gpk::apod<uint32_t>								RemainingSpace;

	public:	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		::gpk::error_t						Clear						()	{ return ::gpk::clear(Blocks, RemainingSpace); }
		::gpk::error_t						Save						(::gpk::apod<uint8_t> & output)	const	{
			gpk_necs(::gpk::saveView(output, RemainingSpace));
			for(uint32_t iArray = 0; iArray < RemainingSpace.size(); ++iArray)
				gpk_necs(::gpk::saveView(output, *Blocks[iArray]));
			return 0;
		}

		::gpk::error_t						Load						(::gpk::vcu8 & input) {
			Clear();
			gpk_necs(::gpk::loadView(input, RemainingSpace));
			gpk_necs(Blocks.resize(RemainingSpace.size()));
			for(uint32_t iArray = 0; iArray < Blocks.size(); ++iArray)
				gpk_necs(::gpk::loadView(input, Blocks[iArray]->Storage));
			return 0;
		}

		::gpk::error_t						push_sequence				(const char* sequence, uint32_t length, ::gpk::vcc & out_view)	{
			const uint32_t							lengthPlusOne				= length + 1;
			for(uint32_t iBlock = 0; iBlock < Blocks.size(); ++iBlock) {
				uint32_t								& blkRemainingSpace			= RemainingSpace[iBlock];
				if(blkRemainingSpace >= lengthPlusOne) {
					char									* sequenceStart				= &Blocks[iBlock]->operator[](_blockSize - blkRemainingSpace);
					out_view							= {sequenceStart, length};
					memcpy(sequenceStart, sequence, length);
					sequenceStart[lengthPlusOne]		= 0;
					blkRemainingSpace					-= lengthPlusOne;
					return iBlock;
				}
			}
			int32_t									indexNewBlock				= Blocks.size();
			gpk_necs(Blocks			.resize(Blocks.size() + 1));
			gpk_necs(RemainingSpace	.resize(Blocks.size() + 1, _blockSize - length - 1));
			out_view							= {&Blocks[indexNewBlock]->operator[](0), length};
			memcpy(Blocks[indexNewBlock]->Storage, sequence, length);
			return indexNewBlock;
		}
	};

	class CLabelManager	{
		stacxpr	const uint32_t	BLOCK_SIZE					= 1024 * 64;

		block_string_container<BLOCK_SIZE>	Characters;
		::gpk::vcc							Empty;

	public:	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		::gpk::apod<uint16_t>				Counts;
		::gpk::apod<const char_t*>			Texts;

											~CLabelManager				()																{
			for(uint32_t iText = 0; iText < Texts.size(); ++iText)
				verbose_printf("Label found: %s.", Texts[iText]);
		}
		inline								CLabelManager				()																{ 
			Characters	.push_sequence("", 0U, Empty); 
			Counts		.push_back(0); 
			Texts		.push_back(Empty.begin()); 
		}

		::gpk::error_t						Save						(::gpk::apod<uint8_t> & output)	const	{
			gpk_necs(::gpk::saveView(output, Counts));
			for(uint32_t iArray = 0; iArray < Counts.size(); ++iArray)
				gpk_necs(output.append((const uint8_t*)Texts[iArray], Counts[iArray]));

			return 0;
		}

		::gpk::error_t						Load						(::gpk::vcu8 & input) {
			gpk_necs(::gpk::loadView(input, Counts));
			::gpk::vcc								out_view; 
			uint32_t								offsetByte					= 0;
			for(uint32_t iArray = 0; iArray < Counts.size(); ++iArray) {
				uint32_t								elementCount				= Counts[iArray];
				gpk_necs(Characters.push_sequence((const char*)&input[offsetByte], elementCount, out_view));
				offsetByte							+= elementCount;
				gpk_necs(Texts.push_back(out_view.begin()));
			}
			input								= {input.begin(), input.size() - offsetByte};
			return 0;
		}

		inline	::gpk::error_t				Size						()					const	noexcept	{ return Texts.size(); }
		inline	::gpk::vcc					View						(uint32_t index)	const				{ return {Texts[index], Counts[index]}; }
		inline	::gpk::error_t				View						(const char* elements, uint16_t count)	{ ::gpk::vcc out_view; return View(elements, count, out_view); }

		::gpk::error_t						Index						(const ::gpk::vcc & elements) {
			ree_if(elements.size() > CLabelManager::BLOCK_SIZE, "Data too large: %u.", elements.size());

			for(uint32_t iView = 0, countLabels = Texts.size(); iView < countLabels; ++iView) {
				if(elements.size() != Counts[iView])
					continue;

				const char								* pStored					= Texts[iView];
				if(0 == memcmp(pStored, elements.begin(), elements.byte_count()))
					return iView;
			}
			return -1;
		}

		::gpk::error_t						View						(const char* text, uint32_t textLen, ::gpk::vcc & out_view)		{
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

#endif // GPK_LABEL_MANAGER_H_29037492837
