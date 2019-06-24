#include "gpk_view_array.h"

#ifndef GPK_FIND_H_2983749283742
#define GPK_FIND_H_2983749283742

namespace gpk
{
	template<typename _tElement>
						::gpk::error_t							find_sequence_obj					(const ::gpk::view_array<_tElement>& sequence, const ::gpk::view_array<_tElement>& target)						{
		for(uint32_t iOffset = 0, offsetStop = target.size() - sequence.size(); iOffset < offsetStop; ++iOffset) {
			bool															equal								= true;
			for(uint32_t iSequenceElement = 0; iSequenceElement < sequence.size(); ++iSequenceElement) {
				if(sequence[iSequenceElement] != target[iOffset + iSequenceElement])
					equal														= false;
			}
			if(equal)
				return iOffset;
		}
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t							find								(const _tElement& element, const ::gpk::view_const_string& target, int32_t offset = 0)							{
		for(int32_t iOffset = offset, offsetStop = (int32_t)target.size(); iOffset < offsetStop; ++iOffset)
			if(element == target[iOffset]) 
				return iOffset;
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t							find								(const _tElement& element, const ::gpk::view_array<_tElement>& target, int32_t offset = 0)						{
		for(int32_t iOffset = offset, offsetStop = (int32_t)target.size(); iOffset < offsetStop; ++iOffset)
			if(element == target[iOffset]) 
				return iOffset;
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t							rfind								(const _tElement& element, const ::gpk::view_array<_tElement>& target, int32_t offset = 0)						{
		for(int32_t iOffset = target.size() - 1 - offset; iOffset >= 0; --iOffset) 
			if(element == target[iOffset]) 
				return iOffset;
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t							rfind								(const _tElement& element, const ::gpk::view_const_string& target, int32_t offset = 0)							{
		for(int32_t iOffset = target.size() - 1 - offset; iOffset >= 0; --iOffset) 
			if(element == target[iOffset]) 
				return iOffset;
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t							find_sequence_pod					(const ::gpk::view_array<_tElement>& sequence, const ::gpk::view_array<_tElement>& target, int32_t offset = 0)	{
		for(int32_t iOffset = offset, offsetStop = (int32_t)(target.size() - sequence.size()) + 1; iOffset < offsetStop; ++iOffset) 
			if(0 == memcmp(sequence.begin(), &target[iOffset], sequence.size() * sizeof(_tElement))) 
				return iOffset;
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t							rfind_sequence_pod					(const ::gpk::view_array<_tElement>& sequence, const ::gpk::view_array<_tElement>& target)						{
		for(int32_t iOffset = (int32_t)(target.size() - sequence.size()); iOffset >= 0; --iOffset) 
			if(0 == memcmp(sequence.begin(), &target[iOffset], sequence.size() * sizeof(_tElement))) 
				return iOffset;
		return -1;
	}

} // namespace 

#endif // GPK_FIND_H_2983749283742
