#include "gpk_keyval.h"
#include "gpk_memory.h"
#include "gpk_safe.h"
#include "gpk_size.h"

#ifndef GPK_ARRAY_H_29837498237498237429837
#define GPK_ARRAY_H_29837498237498237429837

namespace gpk
{
	template <typename... _Args>	void	clear					(_Args&&... args)						{ const int32_t results[] = {args.clear()..., 0}; (void)results; }
	template <typename... _Args>	
	::gpk::error_t							resize					(uint32_t newSize, _Args&&... args)		{
		const uint32_t								oldSizes	[]			= {args.size	()			..., 0};
		const ::gpk::error_t						results		[]			= {args.resize	(newSize)	..., 0};
		for(uint32_t i=0; i < ::gpk::min(::gpk::size(results), ::gpk::size(oldSizes)); ++i)
			if(errored(results[i])) {
				error_printf("Failed to set container size: %i. Out of memory?", (int32_t)newSize);
				int32_t										j						= 0;
				const int32_t								dummy	[]				= {args.resize(oldSizes[j++])..., 0};
				(void)dummy;
				return -1;
			}
		return newSize;
	}

	// Base for arrays that keeps track of its actual size.
#pragma pack(push, 1)
	template<typename _tCell>
	struct array_base : public view<_tCell> {
	protected:
		typedef									array_base<_tCell>							TypeArray;
		using									view<_tCell>::Count;
		uint32_t								Size										= 0;
//		uint32_t								Offset										= 0;

		inlcxpr									array_base									()									noexcept	= default;
		inlcxpr									array_base									(const TypeArray &	other)			noexcept	= delete;
		inlcxpr									array_base									(const TypeArray &&	other)			noexcept	= delete;

		array_base<_tCell>&						operator =									(const TypeArray &	other)						= delete;
		array_base<_tCell>&						operator =									(const TypeArray &&	other)						= delete;
		// This helper method is used to prevent redundancies. It returns a safe integer of the same or a higher value than the one passed as argument.
		inlcxpr	uint32_t						calc_reserve_size							(const uint32_t newSize)	const	noexcept	{ return ::gpk::max(newSize, uint32_t(newSize + ::gpk::max(newSize >> 1, 4U)));			}
		inlcxpr	uint32_t						calc_malloc_size							(const uint32_t newSize)	const	noexcept	{ return ::gpk::max(newSize* (uint32_t)sizeof(_tCell), Count*(uint32_t)sizeof(_tCell));	}
	}; // array_base
#pragma pack(pop)

	// This class is optimized to contain POD instances and won't work for C++ objects that require calling constructors/destructors.
	template<typename _tPOD>
	struct array_pod : public array_base<_tPOD> {
		typedef				array_base<_tPOD>	TArrayBase									;
		typedef				view<_tPOD>			TArrayView									;

		using				TArrayBase::		Count										;
		using				TArrayBase::		Data										;
		using				TArrayBase::		Size										;
		using				TArrayBase::		calc_reserve_size							;
		using				TArrayBase::		calc_malloc_size							;
		using				TArrayBase::		operator[]									;

		inline									~array_pod									()																						{ safe_gpk_free(Data);		}
		inlcxpr									array_pod									()																			noexcept	= default;
		//inline									array_pod									(uint32_t initialSize)																	{ gthrow_if(errored(resize(initialSize)), "Failed to resize array! Why? Size requested: %u.", initialSize);	}
												array_pod									(::std::initializer_list<_tPOD> init)													{
			gthrow_if(errored(resize((uint32_t)init.size())), "Failed to resize array! Why? Initializer list size: %u.", (uint32_t)init.size());
			memcpy(Data, init.begin(), Count * sizeof(_tPOD));
		}

		//template<size_t sizeStatic>
		//										array_pod									(const _tPOD (&init)[sizeStatic])														{
		//	gthrow_if(errored(resize(sizeStatic)), "Failed to resize array! Why?");
		//	memcpy(Data, init, Count * sizeof(_tPOD));
		//}
		//										array_pod									(const view<const _tPOD>& other)													{
		//if(0 == other.Count) 
		//	return;
		//	const uint32_t										newSize										= other.Count;
		//	const uint32_t										reserveSize									= calc_reserve_size(newSize);
		//	uint32_t											mallocSize									= calc_malloc_size(reserveSize) + 2;
		//	gthrow_if(mallocSize != (reserveSize * (uint32_t)sizeof(_tPOD) + 2), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize);
		//	::gpk::auto_gpk_free								safeguard;
		//	Data											= (_tPOD*)(safeguard.Handle = ::gpk::gpk_malloc(mallocSize));
		//	gthrow_if(0 == Data			, "Failed to allocate array. Requested size: %u. ", (uint32_t)newSize);
		//	gthrow_if(0 == other.Data	, "%s", "other.Data is null!");
		//	memcpy(Data, other.Data, newSize * sizeof(_tPOD));
		//	//for(uint32_t i = 0, count = newSize; i<count; ++i)
		//	//	Data[i]											= other[i];
		//	Size											= (uint32_t)reserveSize;
		//	Count											= other.Count;
		//	safeguard.Handle								= 0;
		//}
												array_pod									(array_pod<_tPOD> && other)			noexcept	{
			Size									= other.Size									;
			Count									= other.Count									;
			Data									= other.Data									;

			other.Size								= other.Count									= 0;
			other.Data								= 0;
		}
		inline									array_pod									(const array_pod<_tPOD> & other)				: array_pod((const view<const _tPOD>&) other) {}
												array_pod									(const view<const _tPOD> & other)				{
			if(0 == other.size()) 
				return; 
			const uint32_t								newSize										= other.size();
			const uint32_t								reserveSize									= calc_reserve_size(newSize);
			uint32_t									mallocSize									= calc_malloc_size(reserveSize) + 2;
			gthrow_if(mallocSize != (reserveSize * (uint32_t)sizeof(_tPOD) + 2), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize);
			::gpk::auto_gpk_free						safeguard;
			Data									= (_tPOD*)(safeguard.Handle = ::gpk::gpk_malloc(mallocSize));
			gthrow_if(0 == Data				, "Failed to allocate array. Requested size: %u. ", (uint32_t)newSize);
			gthrow_if(0 == other.begin()	, "%s", "other.Data is null!");
			memcpy(Data, other.begin(), newSize * sizeof(_tPOD));
			memset(&((char*)safeguard.Handle)[newSize * sizeof(_tPOD)], 0, mallocSize - newSize * sizeof(_tPOD));
			//for(uint32_t i = 0, count = newSize; i<count; ++i)
			//	Data[i]									= other[i];
			Size									= (uint32_t)reserveSize;
			Count									= other.size();
			safeguard.Handle						= 0;
			*(uint16_t*)(&Data[Count])				= 0;
		}
		template <size_t _otherCount>			array_pod									(const _tPOD (&other)[_otherCount])														{
			const uint32_t								newSize										= _otherCount;
			const uint32_t								reserveSize									= calc_reserve_size(newSize);
			uint32_t									mallocSize									= calc_malloc_size(reserveSize) + 2;
			gthrow_if(mallocSize != (reserveSize * sizeof(_tPOD) + 2), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize);
			::gpk::auto_gpk_free						safeguard;
			Data									= (_tPOD*)(safeguard.Handle = ::gpk::gpk_malloc(mallocSize));
			gthrow_if(0 == Data, "Failed to allocate array. Requested size: %u. ", (uint32_t)newSize);
			memcpy(Data, other, newSize * sizeof(_tPOD));
			memset(&Data[newSize], 0, mallocSize - newSize * sizeof(_tPOD));
			Size									= (uint32_t)reserveSize;
			Count									= _otherCount;
			safeguard.Handle						= 0;
			//*(uint16_t*)(&Data[Count])			= 0;
		}
		inlcxpr	operator						view<const _tPOD>						()								const	noexcept	{ return {Data, Count}; }
		array_pod<_tPOD>&						operator =									(const array_pod<_tPOD>& other)															{ return operator=((const view<_tPOD>&) other); }
		array_pod<_tPOD>&						operator =									(const view<_tPOD>& other)															{
			gthrow_if(resize(other.size()) != (int32_t)other.size(), "%s", "Failed to assign array.");
			if(Count)
				memcpy(Data, other.begin(), Count * sizeof(_tPOD));

			if(Data)
				*(uint16_t*)(&Data[Count])				= 0;

			return *this;
		}
		template<size_t sizeStatic>
		array_pod<_tPOD>&						operator =									(const _tPOD (&init)[sizeStatic])														{
			gthrow_if(resize(sizeStatic) != sizeStatic, "Failed to resize array! Why? Size requested: %u.", (uint32_t)sizeStatic);
			memcpy(Data, init, Count * sizeof(_tPOD));
			*(uint16_t*)(&Data[Count])				= 0;
			return *this;
		}

		// This method doesn't call destructors of the contained PODs.
		inline	::gpk::error_t					clear										()																			noexcept	{ return Count = 0; }

		// This method doesn't call destructors of the contained PODs.
		::gpk::error_t							clear_pointer								()																			noexcept	{ safe_gpk_free(Data); Data = 0; return Size = Count = 0; }

		// Returns the new size of the array
		inline	::gpk::error_t					pop_back									(_tPOD* oldValue)															noexcept	{
			ree_if(0 == Count, "%s", "Cannot pop elements of an empty array.");
			--Count;
			gpk_safe_podcpy(oldValue, &Data[Count]);
			return Count;
		}

		// Returns the index of the pushed value or -1 on failure
		::gpk::error_t							push_back									(const _tPOD & newValue)														noexcept	{
			const int32_t								oldSize										= Count;
			const int32_t								newSize										= oldSize + 1;
			const int32_t								finalSize									= resize(newSize, newValue);
			ree_if(finalSize != newSize, "Failed to push value! Array size: %i.", oldSize);
			return oldSize;
		}

		// Returns the index of the pushed value
		template<size_t _Length>
		inline	::gpk::error_t					append_string								(const _tPOD (&newChain)[_Length])											noexcept	{ return append(newChain, (uint32_t)strnlen(newChain, (uint32_t)_Length)); }
		inline	::gpk::error_t					append_string								(const ::gpk::vcs & newChain)												noexcept	{ return append(newChain.begin(), newChain.size()); }
		template<size_t _Length>
		inline	::gpk::error_t					append										(const _tPOD (&newChain)[_Length])											noexcept	{ return append(newChain, (uint32_t)_Length);					}
		inline	::gpk::error_t					append										(const ::gpk::view<const _tPOD>& newChain)									noexcept	{ return append(newChain.begin(), newChain.size());	}
		::gpk::error_t							append										(const _tPOD* chainToAppend, uint32_t chainLength)							noexcept	{
			if(0 == chainLength)
				return Count;
			const uint32_t								startIndex									= Count;
			const uint32_t								requestedSize								= Count + chainLength;
			ree_if(requestedSize < Count, "Size overflow. Cannot append chain. count: %u. Chain length: %u.", Count, chainLength);
			const int32_t								newSize										= resize(requestedSize);
			ree_if(newSize != (int32_t)requestedSize, "Failed to resize array for appending. Requested size: %u.", requestedSize);
			memcpy(&Data[startIndex], chainToAppend, sizeof(_tPOD) * chainLength);
			*(uint16_t*)(&Data[Count])				= 0;
			return startIndex;
		}

		// Returns the new size of the array.
		::gpk::error_t							resize										(uint32_t newSize, const _tPOD & newValue)									noexcept	{
			int32_t										oldCount									= (int32_t)Count;
			int32_t										newCount									= resize(newSize);
			ree_if(newCount != (int32_t)newSize, "Failed to resize array. Requested size: %u. Current size: %u (%u).", newCount, Count, Size);
			for( int32_t i = oldCount; i < newCount; ++i )
				Data[i]									= newValue;
			return newCount;
		}

		int32_t									reserve										(uint32_t newCount)								{
			if(newCount > Size) {
				uint32_t								newSize										= ::gpk::max(4U, (uint32_t)(newCount + (newCount / 4)));
				_tPOD									* newData									= (_tPOD*)::gpk::gpk_malloc(newSize * sizeof(_tPOD) + 2);
				if(0 == newData)
					return -1;
				for(uint32_t iElement = 0; iElement < Count; ++iElement)
					new (&newData[iElement]) _tPOD(Data[iElement]);
				_tPOD									* oldData			= Data;
				Size								= newSize;
				Data								= newData;
				for(uint32_t iElement = 0; iElement < Count; ++iElement)
					oldData[iElement].~_tPOD();
				::gpk::gpk_free(oldData);
			}
			return Count;
		}
		// Returns the new size of the array.
		::gpk::error_t							resize										(uint32_t newSize)															noexcept	{
			const uint32_t								oldCount									= Count;
			if(newSize > Size) {
				_tPOD										* oldData									= Data;
				uint32_t									reserveSize									= calc_reserve_size(newSize);
				uint32_t									mallocSize									= calc_malloc_size(reserveSize) + 2;
				ree_if(mallocSize != (reserveSize * (uint32_t)sizeof(_tPOD) + 2), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize);
				::gpk::auto_gpk_free						safeguard;
				_tPOD										* newData									= (_tPOD*)(safeguard.Handle = ::gpk::gpk_malloc(mallocSize));
				ree_if(nullptr == newData, "Failed to resize array. Requested size: %u. Current size: %u.", newSize, (uint32_t)Size);
				memset(&((char*)safeguard.Handle)[newSize * sizeof(_tPOD)], 0, mallocSize - newSize * sizeof(_tPOD));
				//TArrayView									safe_data									= {newData, reserveSize};
				if(oldData)
					memcpy(newData, oldData, ::gpk::min(newSize, oldCount) * sizeof(_tPOD));
				Size									= (uint32_t)reserveSize;
				Count									= newSize;
				Data									= newData;
				safeguard.Handle						= 0;
				if(oldData)
					::gpk::gpk_free(oldData);
			}
			else
				Count									= newSize;
			//*(uint16_t*)(&Data[Count])				= 0;
			return (int32_t)Count;
		}

		// returns the new size of the list or -1 on failure.
		::gpk::error_t							insert										(uint32_t index, const _tPOD & newValue)										noexcept	{
			ree_if(index >= Count, "Invalid index: %u.", index);
			if(Size < Count + 1) {
				_tPOD										* oldData									= Data;
				uint32_t									reserveSize									= calc_reserve_size(Count + 1);
				uint32_t									mallocSize									= calc_malloc_size(reserveSize) + 2;
				ree_if(mallocSize != (reserveSize * sizeof(_tPOD) + 2), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize);
				::gpk::auto_gpk_free						safeguard;
				_tPOD										* newData									= (_tPOD*)(safeguard.Handle = ::gpk::gpk_malloc(mallocSize));
				ree_if(nullptr == newData, "Failed to allocate array for inserting new value. Memory requesteD: %u.", mallocSize);

				TArrayView									viewSafe									= {newData, Count+1};
				memcpy(viewSafe.begin(), oldData, ::gpk::min(index, Count) * sizeof(_tPOD));
				viewSafe[index]							= newValue;
				for(uint32_t i = index, maxCount = Count; i < maxCount; ++i)
					viewSafe[i + 1]							= oldData[i];
				Data									= newData;
				safeguard.Handle						= 0;
				::gpk::gpk_free(oldData);
			}
			else {
				for(int32_t i = (int32_t)Count, iStop = index; i > iStop; --i)
					memcpy(&Data[i], &Data[i - 1], sizeof(_tPOD));
				Data[index]										= newValue;
			}
			return ++Count;
		}

		// returns the new size of the list or -1 on failure.
		::gpk::error_t							insert										(uint32_t index, const _tPOD* chainToInsert, uint32_t chainLength)			noexcept	{
			ree_if(index >= Count, "Invalid index: %u.", index);

			if(Size < Count + chainLength) {
				_tPOD										* oldData									= Data;
				uint32_t									newSize										= Count + chainLength;
				uint32_t									reserveSize									= calc_reserve_size(newSize);
				uint32_t									mallocSize									= calc_malloc_size(reserveSize) + 2;
				ree_if(mallocSize != (reserveSize * (uint32_t)sizeof(_tPOD) + 2), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize);
				::gpk::auto_gpk_free								safeguard;
				ree_if(nullptr == (safeguard.Handle = ::gpk::gpk_malloc(mallocSize)), "Failed to allocate array for inserting new value. memory requesteD: %u.", mallocSize);
				::gpk::view<_tPOD>							viewSafe									= {(_tPOD*)safeguard.Handle, newSize};
				uint32_t									i, maxCount;
				for(i = 0, maxCount = ::gpk::min(index, Count)					; i < maxCount; ++i) viewSafe[i]						= oldData[i];
				for(i = 0, maxCount = ::gpk::min(chainLength, newSize - index)	; i < maxCount; ++i) viewSafe[i + index]				= chainToInsert[i];
				for(i = 0, maxCount = Count - index								; i < maxCount; ++i) viewSafe[i + index + chainLength]	= oldData[i + index];
				Data									= (_tPOD*)safeguard.Handle; //newData;
				safeguard.Handle						= 0;
				::gpk::gpk_free(oldData);
			}
			else {	// no need to reallocate and copy, just shift rightmost elements and insert in-place
				for(int32_t  i = (int32_t)::gpk::min(index, Count - 1), maxCount = (int32_t)index	; i >=	maxCount; --i)
					Data[i + chainLength]					= Data[i];
				for(uint32_t i = 0, maxCount = chainLength; i <	maxCount; ++i)
					Data[i + index		]					= chainToInsert[i];
			}
			return Count += chainLength;
		}

		inline	::gpk::error_t					insert										(uint32_t index, ::gpk::view<const _tPOD> chainToInsert)			noexcept	{ return insert(index, chainToInsert.begin(), chainToInsert.size()); }

		template<size_t _chainLength>
		inline	::gpk::error_t					insert										(uint32_t index, const _tPOD* (&chainToInsert)[_chainLength])		noexcept	{ return insert(index, chainToInsert, _chainLength); }

		// Returns the new size of the list or -1 if the array pointer is not initialized.
		::gpk::error_t							remove_unordered							(uint32_t index)													noexcept	{
			ree_if(0 == Data, "%s", "Uninitialized array pointer!");
			ree_if(index >= Count, "Invalid index: %u.", index);
			Data[index]								= Data[--Count];
			return Count;
		}

		// returns the new array size or -1 if failed.
		template<typename _tObj>
		::gpk::error_t							erase										(const _tObj* address)												noexcept	{
			ree_if(0 == Data, "Uninitialized array pointer! When trying to erase element at address: %p", address);
			const ptrdiff_t								ptrDiff										= ptrdiff_t(address) - (ptrdiff_t)Data;
			const uint32_t								index										= (uint32_t)(ptrDiff / (ptrdiff_t)sizeof(_tObj));
			ree_if(index >= Count, "Invalid index: %u.", index);
			return remove(index);
		}

		// returns the new array size or -1 if failed.
		::gpk::error_t							remove										(uint32_t index)													noexcept	{
			ree_if(0 == Data, "Uninitialized array pointer! When trying to remove element at index: %u.", index);
			ree_if(index >= Count, "Invalid index: %u.", index);
			--Count;
			while(index < Count) {
				memcpy(&Data[index], &Data[index + 1], sizeof(_tPOD));		// Placement new
				++index;
			}
			return Count;
		}

		// Returns the index of the argument if found or -1 if not.
		inline	::gpk::error_t					find										(const _tPOD & valueToLookFor)								const	noexcept	{
			for(uint32_t i = 0; i < Count; ++i)
				if(0 == ::gpk::podcmp(&Data[i], &valueToLookFor))
					return i;
			return -1;
		}

		::gpk::error_t							read										(const int8_t* input, uint32_t* inout_bytesRead)					noexcept	{
			uint32_t									elementsToRead								= 0;
			if(input) {
				elementsToRead							= *(uint32_t*)input;
				input									+= sizeof(uint32_t);
			}

			*inout_bytesRead						+= sizeof(uint32_t);
			if(0 == elementsToRead) {
				Count									= 0;
				return 0;
			}

			::gpk::view<_tPOD>							newStorage									= {(_tPOD*)::gpk::gpk_malloc(sizeof(_tPOD) * elementsToRead + 2), elementsToRead};
			ree_if(0 == newStorage.begin(), "Failed to allocate array for storing read %u elements.", elementsToRead);
			if(0 == input) {
				for(uint32_t i = 0; i < Count; ++i)
					*inout_bytesRead						+= sizeof(_tPOD);
			}
			else {
				for(uint32_t i = 0; i < Count; ++i) {
					::gpk::podcpy(&newStorage[i], input);
					input									+= sizeof(_tPOD);
					*inout_bytesRead						+= sizeof(_tPOD);
				}
			}
			Count									= elementsToRead;
			Size									= elementsToRead;
			_tPOD									* old											= Data;
			Data									= newStorage.begin();
			::gpk::safe_gpk_free(old);
			return 0;
		}

		::gpk::error_t							write										(int8_t* input, uint32_t* inout_bytesWritten)				const	noexcept	{
			if(0 == input) {
				inout_bytesWritten						+= sizeof(uint32_t) + sizeof(_tPOD) * Count;	// Just return the size required to store this.
				return 0;
			}

			*(uint32_t*)input						= Count;
			input									+= sizeof(uint32_t);
			*inout_bytesWritten						+= sizeof(uint32_t);

			::gpk::view<_tPOD>							newStorage									= {input, Count};
			for(uint32_t i = 0; i < Count; ++i) {
				::gpk::podcpy(&newStorage[i], &Data[i]);
				input									+= sizeof(_tPOD);
				*inout_bytesWritten						+= sizeof(_tPOD);
			}
			return 0;
		}
	}; // array_pod

	//template<typename _tPOD>
	//struct string : public array_pod<_tPOD> {};

	//------------------------------------------------------------------------------------------------------------
	template<typename _tObj>
	struct array_obj : public array_base<_tObj> {
		typedef									array_base<_tObj>						TVectorBase;

		using									TVectorBase::Count;
		using									TVectorBase::Data;
		using									TVectorBase::Size;
		using									TVectorBase::calc_reserve_size;
		using									TVectorBase::calc_malloc_size;
		using									TVectorBase::operator[];

		inline									~array_obj								()																				{ for(uint32_t i = 0; i < Count; ++i) Data[i].~_tObj(); safe_gpk_free(Data); }	// dtor
		inlcxpr									array_obj								()																				= default;
		inline									array_obj								(const uint32_t newSize)														{ gthrow_if(((int32_t)newSize) != resize(newSize), "", "%s", "Failed to resize array."); }
		inline									array_obj								(array_obj<_tObj>&& other)											noexcept	{
			Size									= other.Size								;
			Count									= other.Count								;
			Data									= other.Data								;

			other.Size								= other.Count								= 0;
			other.Data								= 0;
		}	// move ctor
												array_obj								(const array_obj<_tObj>& other)	: array_obj((const view<const _tObj>&) other) {}
												array_obj								(const view<const _tObj>& other)												{
			if(other.size()) {
				uint32_t									newSize									= other.size();
				uint32_t									reserveSize								= calc_reserve_size(newSize);
				uint32_t									mallocSize								= calc_malloc_size(reserveSize);
				gthrow_if(mallocSize != (reserveSize*(uint32_t)sizeof(_tObj)), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize);
				::gpk::auto_gpk_free								safeguard;
				_tObj												* newData								= (_tObj*)(safeguard.Handle = ::gpk::gpk_malloc(mallocSize));
				gthrow_if(0 == newData		, "Failed to resize array. Requested size: %u. Current size: %u.", (uint32_t)newSize, (uint32_t)Size);
				gthrow_if(0 == other.begin(), "%s", "other.Data is null!");
				for(uint32_t i = 0; i < newSize; ++i)
					new (&newData[i]) _tObj(other.begin()[i]);
				Data											= newData;
				Size											= reserveSize;
				Count											= other.size();
				safeguard.Handle								= 0;
			}
		}
		inlcxpr	operator						view<const _tObj>						()								const	noexcept	{ return {Data, Count}; }
		inline	array_obj<_tObj>&				operator =								(const array_obj<_tObj>& other)															{
			gthrow_if(resize(other.Count) != (int32_t)other.Count, "", "Failed to resize array!");
			for(uint32_t iElement = 0; iElement < other.Count; ++iElement)
				this->operator[](iElement)							= other[iElement];
			return *this;
		}
		inline	int32_t							clear									()																						{
			for(uint32_t i = 0; i < Count; ++i)
				Data[i].~_tObj();
			return Count							= 0;
		}
		// Returns the new size of the array
		inline	int32_t							pop_back								(_tObj* oldValue)																		{
			ree_if(0 == Count, "%s", "Cannot pop elements from an empty array.");
			--Count;
			gpk_safe_assign(oldValue, Data[Count]);
			Data[Count].~_tObj();
			return Count;
		}
		// Returns the index of the pushed value or -1 on failure
		inline	int32_t							push_back								(const _tObj & newValue)																	{
			int32_t										indexExpected							= Count;
			int32_t										indexNew								= resize(Count+1, newValue)-1;
			ree_if(indexNew != indexExpected, "Failed to push value! Array size: %i.", indexExpected);
			return indexNew;
		}

		// returns the new array size or -1 if failed.
		inline	int32_t							erase									(const _tObj* address)																	{
			ree_if(0 == Data, "Uninitialized array pointer! Invalid address to erase: %p.", address);
			const ptrdiff_t								ptrDiff									= ptrdiff_t(address) - (ptrdiff_t)Data;
			const uint32_t								index									= (uint32_t)(ptrDiff / (ptrdiff_t)sizeof(_tObj));
			ree_if(index >= Count, "Invalid index: %u.", index);
			return remove(index);
		}

		int32_t									remove									(uint32_t index)																		{
			ree_if(0 == Data, "Uninitialized array pointer! Invalid index to erase: %u.", index);
			ree_if(index >= Count, "Invalid index: %u.", index);
			--Count;
			while(index < Count) {
				Data[index].~_tObj();							// Destroy old
				new (&Data[index]) _tObj(Data[index + 1]);		// Placement new
				++index;
			}
			Data[index].~_tObj();							// Destroy last
			return Count;
		}

		// Returns the new size of the list or -1 if the array pointer is not initialized.
		int32_t									remove_unordered						(uint32_t index)																		{
			ree_if(0 == Data, "Uninitialized array pointer! Invalid index to erase: %u.", index);
			ree_if(index >= Count, "Invalid index: %u.", index);
			Data[index].~_tObj();							// Destroy old
			if(1 == Count || index == (Count - 1))
				--Count;
			else {
				new (&Data[index]) _tObj(Data[--Count]);	// Placement new
				Data[Count].~_tObj();						// Destroy reordered
			}

			return Count;
		}
		template<size_t _Length>
		inline	::gpk::error_t					append									(const _tObj (&newChain)[_Length])											noexcept	{ return append(newChain, (uint32_t)_Length);					}
		inline	::gpk::error_t					append									(const ::gpk::view<const _tObj> & newChain)									noexcept	{ return append(newChain.begin(), newChain.size());	}
		::gpk::error_t							append									(const _tObj* chainToAppend, uint32_t chainLength)							noexcept	{
			const uint32_t								startIndex								= Count;
			const uint32_t								requestedSize							= Count + chainLength;
			ree_if(requestedSize < Count, "Size overflow. Cannot append chain. count: %u. Chain length: %u.", Count, chainLength);
			if(requestedSize > Size) {
				_tObj										* oldData								= Data;
				uint32_t									reserveSize								= calc_reserve_size(requestedSize);
				uint32_t									mallocSize								= calc_malloc_size(reserveSize);
				ree_if(mallocSize != (reserveSize*(uint32_t)sizeof(_tObj)), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize);
				::gpk::auto_gpk_free						safeguard;
				_tObj										* newData								= (_tObj*)(safeguard.Handle = ::gpk::gpk_malloc(mallocSize));
				memset(&((char*)safeguard.Handle)[requestedSize * sizeof(_tObj)], 0, mallocSize - requestedSize * sizeof(_tObj));
				ree_if(nullptr == newData, "Failed to resize array. Requested size: %u. Current size: %u.", requestedSize, Size);
				if(oldData) {
					for(uint32_t i = 0, copyCount = ::gpk::min(Count, requestedSize); i < copyCount; ++i)
						new (&newData[i]) _tObj{oldData[i]};
					for(uint32_t i = 0; i < Count; ++i)
						oldData[i].~_tObj();
				}
				Data									= newData;
				Size									= reserveSize;
				Count									= (uint32_t)requestedSize;
				safeguard.Handle						= 0;
				if(oldData)
					::gpk::gpk_free(oldData);
			}
			for(uint32_t iEl = 0; iEl < chainLength; ++iEl) {
				new (&Data[startIndex + iEl]) _tObj(chainToAppend[iEl]);
			}
			return startIndex;
		}

		// returns the new size of the list or -1 on failure.
		int32_t									insert									(uint32_t index, const _tObj & newValue)													{
			ree_if(index >= Count, "Invalid index: %u.", index);

			if(Size < Count + 1) {
				_tObj										* oldData								= Data;
				uint32_t									reserveSize								= calc_reserve_size(Count+1);
				uint32_t									mallocSize								= calc_malloc_size(reserveSize);
				ree_if(mallocSize != (reserveSize*(uint32_t)sizeof(_tObj)), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize);
				::gpk::auto_gpk_free						safeguard;
				_tObj										* newData								= (_tObj*)(safeguard.Handle = ::gpk::gpk_malloc(mallocSize));
				ree_if(0 == newData, "Failed to allocate for inserting new element into array! current size: %u. new size: %u.", Size, mallocSize);
				::gpk::view<_tObj>							viewSafe								= {newData, Count+1};
				for(uint32_t i = 0, maxCount = ::gpk::min(index, Count); i < maxCount; ++i) {
					new (&viewSafe[i]) _tObj(oldData[i]);
					oldData[i].~_tObj();
				}
				new (&viewSafe[index]) _tObj(newValue);
				for(uint32_t i = index, maxCount = Count; i < maxCount; ++i) {
					new (&viewSafe[i + 1]) _tObj(oldData[i]);
					oldData[i].~_tObj();
				}
				Data									= newData;
				::gpk::gpk_free(oldData);
				Size									= reserveSize;
				safeguard.Handle						= 0;
			}
			else {
				for(int32_t i = Count - 1, maxCount = (int32_t)index; i >= maxCount; --i) {
					new (&Data[i + 1]) _tObj(Data[i]);
					Data[i].~_tObj();
				}
				new (&Data[index]) _tObj(newValue);
			}
			return ++Count;
		}
		int32_t									reserve									(uint32_t newCount)								{
			if(newCount > Size) {
				uint32_t									newSize									= ::gpk::max(4U, newCount + (newCount / 4));
				_tObj										* newData								= (_tObj*)::gpk::gpk_malloc(newSize * sizeof(_tObj));
				if(0 == newData)
					return -1;

				_tObj										* oldData			= Data;
				for(uint32_t iElement = 0; iElement < Count; ++iElement) {
					new (&newData[iElement]) _tObj(Data[iElement]);
					oldData[iElement].~_tObj();
				}
				Size									= newSize;
				Data									= newData;
				::gpk::gpk_free(oldData);
			}
			return Count;
		}
		// Returns the new size of the array or -1 if failed.
		template <typename... _tArgs>
		int32_t									resize									(uint32_t newSize, _tArgs&&... constructorArgs)											{
			uint32_t									oldCount								= Count;
			if(newSize > Size) {
				_tObj										* oldData								= Data;
				uint32_t									reserveSize								= calc_reserve_size(newSize);
				uint32_t									mallocSize								= calc_malloc_size(reserveSize);
				ree_if(mallocSize != (reserveSize*(uint32_t)sizeof(_tObj)), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize);
				::gpk::auto_gpk_free						safeguard;
				_tObj										* newData									= (_tObj*)(safeguard.Handle = ::gpk::gpk_malloc(mallocSize));
				memset(&((char*)safeguard.Handle)[newSize * sizeof(_tObj)], 0, mallocSize - newSize * sizeof(_tObj));
				ree_if(nullptr == newData, "Failed to resize array. Requested size: %u. Current size: %u.", newSize, Size);
				if(oldData) {
					for(uint32_t i = 0, copyCount = ::gpk::min(oldCount, newSize); i < copyCount; ++i)
						new (&newData[i]) _tObj{oldData[i]};
					for(uint32_t i = 0; i < oldCount; ++i)
						oldData[i].~_tObj();
				}
				Data									= newData;
				Size									= reserveSize;
				Count									= (uint32_t)newSize;
				safeguard.Handle						= 0;
				if(oldData)
					::gpk::gpk_free(oldData);
			}
			else {
				for(int32_t i = ((int32_t)oldCount) - 1, newCount = (int32_t)newSize; i >= newCount; --i)	// we need to cast to int32_t because otherwise --i	will underflow to UINT_MAX and loop forever.
					Data[i].~_tObj();
				Count									= (uint32_t)newSize;
			}
			for(uint32_t i = oldCount; i < Count; ++i)
				new (&Data[i])_tObj{constructorArgs...};

			return (int32_t)Count;
		}
	}; // array_obj

	template <typename T>	using aobj		= ::gpk::array_obj<T>; 
	template <typename T>	using apod		= ::gpk::array_pod<T>;
	template <typename T>	using aview		= ::gpk::array_obj<::gpk::view<T>>; 

	template <typename T>	using ao		= ::gpk::aobj	<T>; 
	template <typename T>	using ap		= ::gpk::apod	<T>;
	template <typename T>	using av		= ::gpk::aview	<T>; 

	typedef	::gpk::aview<uchar_t	>		aview_uchar			, av1uc, avuc;
	typedef	::gpk::aview<char_t		>		aview_char			, av1c, avc;
	typedef	::gpk::aview<float		>		aview_float32		, av1f32, av1f, avf32, avf;
	typedef	::gpk::aview<double		>		aview_float64		, av1f64, av1d, avf64, avd;
	typedef	::gpk::aview<uint8_t	>		aview_uint8			, av1u8, avu8;
	typedef	::gpk::aview<uint16_t	>		aview_uint16		, av1u16, avu16;
	typedef	::gpk::aview<uint32_t	>		aview_uint32		, av1u32, avu32;
	typedef	::gpk::aview<uint64_t	>		aview_uint64		, av1u64, avu64;
	typedef	::gpk::aview<int8_t		>		aview_int8			, av1i8, avi8;
	typedef	::gpk::aview<int16_t	>		aview_int16			, av1i16, avi16;
	typedef	::gpk::aview<int32_t	>		aview_int32			, av1i32, avi32;
	typedef	::gpk::aview<int64_t	>		aview_int64			, av1i64, avi64;

	// view_array<const> common typedefs
	//typedef ::gpk::view_array<const char_t		>	view_const_string	;
	typedef	::gpk::aview<const uchar_t	>	aview_const_uchar	, av1cuc, avcuc;
	typedef	::gpk::aview<const char_t	>	aview_const_char	, av1cc, avcc;
	typedef	::gpk::aview<const float	>	aview_const_float32	, av1cf32, av1cf, avcf32, avcf;
	typedef	::gpk::aview<const double	>	aview_const_float64	, av1cf64, av1cd, avcf64, avcd;
	typedef	::gpk::aview<const uint8_t	>	aview_const_uint8	, av1cu8, avcu8;
	typedef	::gpk::aview<const uint16_t	>	aview_const_uint16	, av1cu16, avcu16;
	typedef	::gpk::aview<const uint32_t	>	aview_const_uint32	, av1cu32, avcu32;
	typedef	::gpk::aview<const uint64_t	>	aview_const_uint64	, av1cu64, avcu64;
	typedef	::gpk::aview<const int8_t	>	aview_const_int8	, av1ci8, avci8;
	typedef	::gpk::aview<const int16_t	>	aview_const_int16	, av1ci16, avci16;
	typedef	::gpk::aview<const int32_t	>	aview_const_int32	, av1ci32, avci32;
	typedef	::gpk::aview<const int64_t	>	aview_const_int64	, av1ci64, avci64;

	typedef apod<uchar_t	>				array_uchar			, auc	, auc;
	typedef apod<char_t		>				array_char			, ac	, ac;
	typedef apod<float_t	>				array_float32		, af32	, array_float	, af;
	typedef apod<double_t	>				array_float64		, af64	, array_double	, ad;
	typedef apod<uint8_t	>				array_uint8			, au8	, au8;
	typedef apod<uint16_t	>				array_uint16		, au16	, au16;
	typedef apod<uint32_t	>				array_uint32		, au32	, au32;
	typedef apod<uint64_t	>				array_uint64		, au64	, au64;
	typedef apod<int8_t		>				array_int8			, ai8	, ai8;
	typedef apod<int16_t	>				array_int16			, ai16	, ai16;
	typedef apod<int32_t	>				array_int32			, ai32	, ai32;
	typedef apod<int64_t	>				array_int64			, ai64	, ai64;

	template<typename _tCell>
	::gpk::error_t							split					(const ::gpk::view<const _tCell> & target, const _tCell & separator, ::gpk::aobj<::gpk::view<const _tCell>> & split)	{
		uint32_t									lastOffset				= 0;
		for(uint32_t iChar = 0; iChar < target.size(); ++iChar) {
			if(target[iChar] == separator) {
				const ::gpk::view<const _tCell>			newView					= {&target[lastOffset], iChar - lastOffset};
				++iChar;
				gpk_necs(split.push_back(newView));
				lastOffset								= iChar;
			}
		}
		if(lastOffset < target.size())
			gpk_necs(split.push_back({&target[lastOffset], target.size() - lastOffset}));
		return (int32_t)split.size();
	}

	template<typename _tCell>
	::gpk::error_t							split					(const ::gpk::view<const _tCell> & target, const ::gpk::view<const _tCell>& separators, ::gpk::aobj<::gpk::view<const _tCell>> & split)	{
		uint32_t									lastOffset				= 0;
		for(uint32_t iChar = 0; iChar < target.size(); ++iChar) {
			for(uint32_t iSeparator = 0; iSeparator < separators.size(); ++iSeparator) {
				if(target[iChar] == separators[iSeparator]) {
					const ::gpk::view<const _tCell>			newView					= {&target[lastOffset], iChar - lastOffset};
					++iChar;
					gpk_necs(split.push_back(newView));
					lastOffset								= iChar;
				}
			}
		}
		if(lastOffset < target.size())
			gpk_necs(split.push_back({&target[lastOffset], target.size() - lastOffset}));
		return (int32_t)split.size();
	}

	template<typename _tCell>
	::gpk::error_t							split					(const ::gpk::vcs & target, const _tCell & separator, ::gpk::aobj<::gpk::vcs> & split)	{
		int32_t										lastOffset				= 0;
		for(int32_t iChar = 0, countChars = target.size(); iChar < countChars; ++iChar) {
			if(target[iChar] == separator) {
				const ::gpk::vcs							newView					= {&target[lastOffset], (uint32_t)::gpk::max(0, iChar - lastOffset)};
				++iChar;
				gpk_necs(split.push_back(newView));
				lastOffset								= iChar;
			}
		}
		if(lastOffset < (int32_t)target.size()) {
			gpk_necs(split.push_back({&target[lastOffset], target.size() - lastOffset}));
			//if(split[split.size()-1][] == separator)
		}
		return (int32_t)split.size();
	}



	typedef ::gpk::SKeyVal<::gpk::vcs, ::gpk::aobj<::gpk::vcs>>	TKeyValConstStringArray;

	::gpk::error_t							keyValConstStringSerialize		(const ::gpk::view<const ::gpk::TKeyValConstChar> & keyVals, const ::gpk::view<const ::gpk::vcc> & keysToSave, ::gpk::au8 & output);
	::gpk::error_t							keyValConstStringDeserialize	(const ::gpk::vcu8 & input, ::gpk::aobj<::gpk::TKeyValConstChar> & output);

	::gpk::apod<char_t>						toString						(const ::gpk::vcc & strToLog);
	stainli	::gpk::apod<char_t>				toString						(const ::gpk::vcu8 & strToLog) { return ::gpk::toString(*(const ::gpk::vcc*)&strToLog); }

	::gpk::error_t							join							(::gpk::apod<char_t> & query, char separator, ::gpk::view<const gpk::vcc> fields);
	::gpk::error_t							append_quoted					(::gpk::apod<char_t> & output, ::gpk::vcc text);


	::gpk::error_t							filterPrefix					(::gpk::view<const ::gpk::vcc> input, const ::gpk::vcc prefix, ::gpk::aobj<::gpk::vcc> & filtered, bool nullIncluded = false);
	::gpk::error_t							filterPostfix					(::gpk::view<const ::gpk::vcc> input, const ::gpk::vcc prefix, ::gpk::aobj<::gpk::vcc> & filtered, bool nullIncluded = false);

	template<typename _tPOD>
	::gpk::error_t							saveView			(::gpk::au8 & output, const ::gpk::view<_tPOD> & headerToWrite)	{
		gpk_necs(output.append({(const uint8_t*)&headerToWrite.size(), (uint32_t)sizeof(uint32_t)}));
		gpk_necs(output.append({(const uint8_t*)headerToWrite.begin(), headerToWrite.size() * (uint32_t)sizeof(_tPOD)}));
		return sizeof(uint32_t) + headerToWrite.size() * sizeof(_tPOD);
	}

	template<typename _tPOD> ::gpk::error_t	saveView			(::gpk::ai8 & output, const ::gpk::view<_tPOD> & headerToWrite) { return ::gpk::saveView(headerToWrite, *(const ::gpk::au8*)&output); }
	template<typename _tPOD> ::gpk::error_t	savePOD				(::gpk::au8 & output, const _tPOD & input) { return ::gpk::saveView(output, ::gpk::view<const _tPOD>{&input, 1}); }
	template<typename _tPOD> ::gpk::error_t	savePOD				(::gpk::ai8 & output, const _tPOD & input) { return ::gpk::saveView(output, ::gpk::view<const _tPOD>{&input, 1}); }

	template<typename _tPOD> ::gpk::error_t	loadView			(::gpk::vcu8 & input, ::gpk::apod<_tPOD> & output) { 
		::gpk::view<const _tPOD>					readView			= {}; 
		uint32_t									bytesRead			= 0;
		gpk_necs(bytesRead = ::gpk::viewRead(readView, input)); 
		input									= {input.begin() + bytesRead, input.size() - bytesRead}; 
		output									= readView; 
		return 0;
	}
	template<typename _tPOD> ::gpk::error_t	loadView			(::gpk::vci8 & input, ::gpk::apod<_tPOD> & output) { return loadView(*(::gpk::vcu8*)& input, output); }
	template<typename _tPOD> ::gpk::error_t	loadView			(::gpk::vcc  & input, ::gpk::apod<_tPOD> & output) { return loadView(*(::gpk::vcu8*)& input, output); }

}

#endif // GPK_ARRAY_H_29837498237498237429837
