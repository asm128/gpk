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
						::gpk::error_t							find								(const _tElement& element, const ::gpk::view_array<const _tElement>& target, uint32_t offset = 0)				{
		for(int32_t iOffset = offset, offsetStop = target.size(); iOffset < offsetStop; ++iOffset)
			if(element == target[iOffset])
				return iOffset;
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t							rfind								(const _tElement& element, const ::gpk::view_array<const _tElement>& target, int32_t offset = 0)				{
		for(int32_t iOffset = target.size() - 1 - offset; iOffset >= 0; --iOffset)
			if(element == target[iOffset])
				return iOffset;
		return -1;
	}

	template<typename _tElement>
						::gpk::error_t							find_sequence_pod					(const ::gpk::view_array<_tElement>& sequence, const ::gpk::view_array<_tElement>& target, uint32_t offset = 0)	{
		for(int32_t iOffset = (int32_t)offset, offsetStop = ((int32_t)target.size() - sequence.size()) + 1; iOffset < offsetStop; ++iOffset)
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

	template<typename _tElement>
						::gpk::error_t							split								(const _tElement& valueToFind, const ::gpk::view_array<_tElement> & original, ::gpk::view_array<_tElement> & left, ::gpk::view_array<_tElement> & right) {
		const ::gpk::error_t											arrobaPos							= ::gpk::find(valueToFind, original);
		if(0 > arrobaPos) {
			left														= original;
			right														= {};
		}
		else {
			gpk_necall(original.slice(left, 0, arrobaPos), "%s", "Invalid slice");
			const uint32_t													offsetDomain						= arrobaPos + 1;
			gpk_necall(original.slice(right, offsetDomain, original.size() - offsetDomain), "%s", "Invalid slice");
		}
		return arrobaPos;
	}

	template<typename _tElement>
						::gpk::error_t							splitAt								(const _tElement& valueToFind, const ::gpk::view_array<_tElement> & original, ::gpk::view_array<_tElement> & left, ::gpk::view_array<_tElement> & right) {
		const ::gpk::error_t											posFragment							= ::gpk::find(valueToFind, original);
		if(0 > posFragment) { // Read until the end unless fragment is found.
			left														= original;
			right														= {};
		}
		else {
			gpk_necall(original.slice(left, 0, (uint32_t)posFragment), "%s", "Invalid slice");
			gpk_necall(original.slice(right, posFragment, (uint32_t)original.size() - posFragment), "%s", "Invalid slice");
		}
		return posFragment;
	}

	template<typename _tElement>
						::gpk::error_t							split								(const ::gpk::view_array<_tElement>& sequenceToFind, const ::gpk::view_array<_tElement> & original, ::gpk::view_array<_tElement> & left, ::gpk::view_array<_tElement> & right) {
		const ::gpk::error_t											arrobaPos							= ::gpk::find_sequence_pod(sequenceToFind, original);
		if(0 > arrobaPos) {
			left														= original;
			right														= {};
		}
		else {
			gpk_necall(original.slice(left, 0, arrobaPos), "%s", "Invalid slice");
			const uint32_t													offsetDomain						= arrobaPos + sequenceToFind.size();
			gpk_necall(original.slice(right, offsetDomain, original.size() - offsetDomain), "%s", "Invalid slice");
		}
		return arrobaPos;
	}

	template<typename _tElement>
						::gpk::error_t							splitAt								(const ::gpk::view_array<_tElement>& sequenceToFind, const ::gpk::view_array<_tElement> & original, ::gpk::view_array<_tElement> & left, ::gpk::view_array<_tElement> & right) {
		const ::gpk::error_t											posFragment							= ::gpk::find_sequence_pod(sequenceToFind, original);
		if(0 > posFragment) { // Read until the end unless fragment is found.
			left														= original;
			right														= {};
		}
		else {
			gpk_necall(original.slice(left, 0, (uint32_t)posFragment), "%s", "Invalid slice");
			gpk_necall(original.slice(right, posFragment, (uint32_t)original.size() - posFragment), "%s", "Invalid slice");
		}
		return posFragment;
	}
} // namespace

#endif // GPK_FIND_H_2983749283742
