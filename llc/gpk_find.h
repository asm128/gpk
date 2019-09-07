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
						::gpk::error_t							find								(const _tElement& element, const ::gpk::view_const_string& target, uint32_t offset = 0)							{
		for(uint32_t iOffset = offset, offsetStop = target.size(); iOffset < offsetStop; ++iOffset)
			if(element == target[iOffset])
				return iOffset;
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t							find								(const _tElement& element, const ::gpk::view_array<const _tElement>& target, uint32_t offset = 0)				{
		for(uint32_t iOffset = offset, offsetStop = target.size(); iOffset < offsetStop; ++iOffset)
			if(element == target[iOffset])
				return iOffset;
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t							rfind								(const _tElement& element, const ::gpk::view_array<const _tElement>& target, int32_t offset = 0)				{
		if(0 == target.size())
			return -1;
		for(int32_t iOffset = target.size() - 1 - offset; iOffset >= 0; --iOffset)
			if(element == target[iOffset])
				return iOffset;
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t							rfind								(const _tElement& element, const ::gpk::view_const_string& target, int32_t offset = 0)							{
		if(0 == target.size())
			return -1;
		for(int32_t iOffset = target.size() - 1 - offset; iOffset >= 0; --iOffset)
			if(element == target[iOffset])
				return iOffset;
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t							find_sequence_pod					(const ::gpk::view_array<_tElement>& sequence, const ::gpk::view_array<_tElement>& target, uint32_t offset = 0)	{
		if(target.size() < sequence.size())
			return -1;
		for(uint32_t iOffset = offset, offsetStop = (target.size() - sequence.size()) + 1; iOffset < offsetStop; ++iOffset)
			if(0 == memcmp(sequence.begin(), &target[iOffset], sequence.size() * sizeof(_tElement)))
				return (::gpk::error_t)iOffset;
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t							rfind_sequence_pod					(const ::gpk::view_array<_tElement>& sequence, const ::gpk::view_array<_tElement>& target)						{
		if(target.size() < sequence.size())
			return -1;
		for(int32_t iOffset = (int32_t)(target.size() - sequence.size()); iOffset >= 0; --iOffset)
			if(0 == memcmp(sequence.begin(), &target[iOffset], sequence.size() * sizeof(_tElement)))
				return iOffset;
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t							split								(const _tElement& valueToFind, const ::gpk::view_array<_tElement> & original, ::gpk::view_array<_tElement> & left, ::gpk::view_array<_tElement> & right) {
		const ::gpk::error_t											arrobaPos							= ::gpk::find(valueToFind, original);
		if(0 > arrobaPos)
			left														= original;
		else {
			left														= {original.begin(), (uint16_t)arrobaPos};
			const uint32_t													offsetDomain						= arrobaPos + 1;
			right														= {original.begin() + offsetDomain, (uint16_t)(original.size() - offsetDomain)};
		}
		return 0;
	}

	template<typename _tElement>
						::gpk::error_t							splitAt								(const _tElement& valueToFind, const ::gpk::view_array<_tElement> & original, ::gpk::view_array<_tElement> & left, ::gpk::view_array<_tElement> & right) {
		const ::gpk::error_t											posFragment							= ::gpk::find(valueToFind, original);
		if(0 > posFragment) { // Read until the end unless fragment is found.
			left														= original;
			right														= {};
		}
		else {
			left														= {original.begin(), (uint32_t)posFragment};
			right														= ::gpk::view_const_char{original.begin() + posFragment, original.size() - posFragment};
		}
		return 0;
	}
} // namespace

#endif // GPK_FIND_H_2983749283742
