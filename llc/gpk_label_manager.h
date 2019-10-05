#include "gpk_array.h"
#include "gpk_array_static.h"
#include "gpk_ptr.h"

#ifndef GPK_LABEL_MANAGER_H_29037492837
#define GPK_LABEL_MANAGER_H_29037492837

namespace gpk
{
	template<size_t _blockSize>
	class unshrinkable_string_container {
		typedef	::gpk::ptr_obj<::gpk::array_static<char_t, _blockSize>>	ptr_block_type;
				::gpk::array_obj<ptr_block_type>						Blocks;
				::gpk::array_pod<uint32_t>								RemainingSpace;
	public:	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				::gpk::error_t											push_sequence				(const char* sequence, uint32_t length, ::gpk::view_const_char& out_view)	{
			const uint32_t														lengthPlusOne				= length + 1;
			for(uint32_t iBlock = 0; iBlock < Blocks.size(); ++iBlock) {
				uint32_t															& blkRemainingSpace			= RemainingSpace[iBlock];
				if(blkRemainingSpace >= lengthPlusOne) {
					char																* sequenceStart				= &Blocks[iBlock]->operator[](_blockSize - blkRemainingSpace);
					out_view														= {sequenceStart, length};
					memcpy(sequenceStart, sequence, length);
					sequenceStart[lengthPlusOne]									= 0;
					blkRemainingSpace												-= lengthPlusOne;
					return 0;
				}
			}
			int32_t																indexNewBlock				= Blocks.size();
			Blocks			.resize(Blocks.size() + 1);
			RemainingSpace	.resize(Blocks.size() + 1, _blockSize - length - 1);
			out_view														= {&Blocks[indexNewBlock]->operator[](0), length};
			memcpy(Blocks[indexNewBlock]->Storage, sequence, length);
			return indexNewBlock;
		}
	};

	class CLabelManager	{
		static constexpr	const uint32_t								BLOCK_SIZE					= 1024*64;
							unshrinkable_string_container<BLOCK_SIZE>	Characters;
							::gpk::view_const_char						Empty;
							::gpk::array_pod<uint16_t>					Counts;
							::gpk::array_pod<const char_t*>				Texts;
	public:	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
																		~CLabelManager				()																					{
			for(uint32_t iText = 0; iText < Texts.size(); ++iText) {
				verbose_printf("Label found: %s.", Texts[iText]);
			}
		}
																		CLabelManager				()																					{ Characters.push_sequence("", 0U, Empty); }

							::gpk::error_t								ArrayView					(const char* text, uint32_t textLen, ::gpk::view_const_char& out_view)		{
			if(0 == textLen || 0 == text || 0 == text[0]) {
				out_view														= Empty;
				return 0;
			}
			uint32_t															iChar						= 0;
			for(uint32_t countChars = ::gpk::min(textLen, CLabelManager::BLOCK_SIZE - 1); iChar < countChars; ++iChar)
				if(0 == text[iChar])
					break;
			for(uint32_t iLabel = 0, countLabels = Texts.size(); iLabel < countLabels; ++iLabel) {
				if(Counts[iLabel] != iChar)
					continue;
				const char															* ptext						= Texts[iLabel];
				if(0 == memcmp(ptext, text, iChar)) {
					out_view														= {ptext, iChar};
					return iChar;
				}
			}
			gpk_necall(Characters	.push_sequence(text, iChar, out_view)	, "%s", "Out of memory?");
			gpk_necall(Counts		.push_back((uint16_t)out_view.size())	, "%s", "Out of memory?");
			gpk_necall(Texts		.push_back(out_view.begin())			, "%s", "Out of memory?");
			return 0;
		}
	};
}

#endif // GPK_LABEL_MANAGER_H_29037492837
