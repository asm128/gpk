#include "gpk_keyval.h"
#include "gpk_memory.h"
#include "gpk_safe.h"
#include "gpk_eval.h"

#if defined(GPK_ATMEL)
#	include "initializer_list.h"
#else
#	include <initializer_list>
#	include <new>
#endif

#ifndef GPK_ARRAY_H_29837498237498237429837
#define GPK_ARRAY_H_29837498237498237429837

namespace gpk
{
	template <typename... _Args>	void			clear					(_Args&&... args)						{ const int32_t results[] = {args.clear()..., 0}; (void)results; }
	template <typename... _Args>	::gpk::error_t	resize					(uint32_t newSize, _Args&&... args)		{
		const uint32_t										oldSizes	[]			= {args.size	()			..., 0};
		const ::gpk::error_t								results		[]			= {args.resize	(newSize)	..., 0};
		for(uint32_t i=0; i < ::gpk::min(::gpk::size(results), ::gpk::size(oldSizes)); ++i)
			if(errored(results[i])) {
				error_printf("Failed to set container size: %i. Out of memory?", (int32_t)newSize);
				int32_t												j						= 0;
				const int32_t										dummy	[]				= {args.resize(oldSizes[j++])..., 0};
				(void)dummy;
				return -1;
			}
		return newSize;
	}

	// Base for arrays that keeps track of its actual size.
	template<typename _tCell>
	struct array_base : public view_array<_tCell> {
	protected:
		using				view_array<_tCell>::		Count;
//							uint32_t					Offset										= 0;
							uint32_t					Size										= 0;

		inline constexpr								array_base									()																			noexcept	= default;
		inline constexpr								array_base									(const array_base<_tCell>&	other)											noexcept	= delete;
		inline constexpr								array_base									(const array_base<_tCell>&&	other)											noexcept	= delete;

							array_base<_tCell>&			operator =									(const array_base<_tCell>&	other)														= delete;
							array_base<_tCell>&			operator =									(const array_base<_tCell>&&	other)														= delete;
		// This helper method is used to prevent redundancies. It returns a safe integer of the same or a higher value than the one passed as argument.
		inline constexpr	uint32_t					calc_reserve_size							(const uint32_t newSize)											const	noexcept	{ return ::gpk::max(newSize, uint32_t(newSize + ::gpk::max(newSize >> 1, 4U)));			}
		inline constexpr	uint32_t					calc_malloc_size							(const uint32_t newSize)											const	noexcept	{ return ::gpk::max(newSize* (uint32_t)sizeof(_tCell), Count*(uint32_t)sizeof(_tCell));	}
	}; // array_base

	// This class is optimized to contain POD instances and won't work for C++ objects that require calling constructors/destructors.
	template<typename _tPOD>
	struct array_pod : public array_base<_tPOD> {
		typedef				array_base<_tPOD>			TArrayBase									;
		typedef				view_array<_tPOD>			TArrayView									;

		using				TArrayBase::				Count										;
		using				TArrayBase::				Data										;
		using				TArrayBase::				Size										;
		using				TArrayBase::				calc_reserve_size							;
		using				TArrayBase::				calc_malloc_size							;
		using				TArrayBase::				operator[]									;

		inline											~array_pod									()																						{ safe_gpk_free(Data);		}
		inline constexpr								array_pod									()																			noexcept	= default;
		//inline											array_pod									(uint32_t initialSize)																	{ gthrow_if(errored(resize(initialSize)), "Failed to resize array! Why? Size requested: %u.", initialSize);	}
														array_pod									(::std::initializer_list<_tPOD> init)													{
			gthrow_if(errored(resize((uint32_t)init.size())), "Failed to resize array! Why? Initializer list size: %u.", (uint32_t)init.size());
			memcpy(Data, init.begin(), Count * sizeof(_tPOD));
		}

		//template<size_t sizeStatic>
		//												array_pod									(const _tPOD (&init)[sizeStatic])														{
		//	gthrow_if(errored(resize(sizeStatic)), "Failed to resize array! Why?");
		//	memcpy(Data, init, Count * sizeof(_tPOD));
		//}
		//												array_pod									(const view_array<const _tPOD>& other)													{
		//	if(other.Count) {
		//		const uint32_t										newSize										= other.Count;
		//		const uint32_t										reserveSize									= calc_reserve_size(newSize);
		//		uint32_t											mallocSize									= calc_malloc_size(reserveSize) + 2;
		//		gthrow_if(mallocSize != (reserveSize * (uint32_t)sizeof(_tPOD) + 2), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize);
		//		::gpk::auto_gpk_free								safeguard;
		//		Data											= (_tPOD*)(safeguard.Handle = ::gpk::gpk_malloc(mallocSize));
		//		gthrow_if(0 == Data			, "Failed to allocate array. Requested size: %u. ", (uint32_t)newSize);
		//		gthrow_if(0 == other.Data	, "%s", "other.Data is null!");
		//		memcpy(Data, other.Data, newSize * sizeof(_tPOD));
		//		//for(uint32_t i = 0, count = newSize; i<count; ++i)
		//		//	Data[i]											= other[i];
		//		Size											= (uint32_t)reserveSize;
		//		Count											= other.Count;
		//		safeguard.Handle								= 0;
		//	}
		//}
														array_pod									(array_pod<_tPOD>&& other)													noexcept	{
			Size											= other.Size									;
			Count											= other.Count									;
			Data											= other.Data									;

			other.Size										= other.Count									= 0;
			other.Data										= 0;
		}
		inline											array_pod									(const array_pod<_tPOD>& other)															: array_pod((const view_array<const _tPOD>&) other) {}
														array_pod									(const view_array<const _tPOD>& other)													{
			if(other.size()) {
				const uint32_t										newSize										= other.size();
				const uint32_t										reserveSize									= calc_reserve_size(newSize);
				uint32_t											mallocSize									= calc_malloc_size(reserveSize) + 2;
				gthrow_if(mallocSize != (reserveSize * (uint32_t)sizeof(_tPOD) + 2), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize);
				::gpk::auto_gpk_free								safeguard;
				Data											= (_tPOD*)(safeguard.Handle = ::gpk::gpk_malloc(mallocSize));
				gthrow_if(0 == Data				, "Failed to allocate array. Requested size: %u. ", (uint32_t)newSize);
				gthrow_if(0 == other.begin()	, "%s", "other.Data is null!");
				memcpy(Data, other.begin(), newSize * sizeof(_tPOD));
				memset(&((char*)safeguard.Handle)[newSize * sizeof(_tPOD)], 0, mallocSize - newSize * sizeof(_tPOD));
				//for(uint32_t i = 0, count = newSize; i<count; ++i)
				//	Data[i]											= other[i];
				Size											= (uint32_t)reserveSize;
				Count											= other.size();
				safeguard.Handle								= 0;
				*(uint16_t*)(&Data[Count])						= 0;
			}
		}
		template <size_t _otherCount>
														array_pod									(const _tPOD (&other)[_otherCount])														{
			const uint32_t										newSize										= _otherCount;
			const uint32_t										reserveSize									= calc_reserve_size(newSize);
			uint32_t											mallocSize									= calc_malloc_size(reserveSize) + 2;
			gthrow_if(mallocSize != (reserveSize * sizeof(_tPOD) + 2), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize);
			::gpk::auto_gpk_free								safeguard;
			Data											= (_tPOD*)(safeguard.Handle = ::gpk::gpk_malloc(mallocSize));
			gthrow_if(0 == Data, "Failed to allocate array. Requested size: %u. ", (uint32_t)newSize);
			memcpy(Data, other, newSize * sizeof(_tPOD));
			memset(&Data[newSize], 0, mallocSize - newSize * sizeof(_tPOD));
			Size											= (uint32_t)reserveSize;
			Count											= _otherCount;
			safeguard.Handle								= 0;
			//*(uint16_t*)(&Data[Count])						= 0;
		}
		inline constexpr	operator					view_array<const _tPOD>						()								const	noexcept	{ return {Data, Count}; }
							array_pod<_tPOD>&			operator =									(const array_pod<_tPOD>& other)															{ return operator=((const view_array<_tPOD>&) other); }
							array_pod<_tPOD>&			operator =									(const view_array<_tPOD>& other)														{
			gthrow_if(resize(other.size()) != (int32_t)other.size(), "%s", "Failed to assign array.");
			if(Count)
				memcpy(Data, other.begin(), Count * sizeof(_tPOD));
			if(Data)
				*(uint16_t*)(&Data[Count])						= 0;
			return *this;
		}
		template<size_t sizeStatic>
							array_pod<_tPOD>&			operator =									(const _tPOD (&init)[sizeStatic])														{
			gthrow_if(resize(sizeStatic) != sizeStatic, "Failed to resize array! Why? Size requested: %u.", (uint32_t)sizeStatic);
			memcpy(Data, init, Count * sizeof(_tPOD));
			*(uint16_t*)(&Data[Count])						= 0;
			return *this;
		}

		// This method doesn't call destructors of the contained PODs.
		inline				::gpk::error_t				clear										()																			noexcept	{ return Count = 0; }

		// This method doesn't call destructors of the contained PODs.
							::gpk::error_t				clear_pointer								()																			noexcept	{ safe_gpk_free(Data); Data = 0; return Size = Count = 0; }

		// Returns the new size of the array
		inline				::gpk::error_t				pop_back									(_tPOD* oldValue)															noexcept	{
			ree_if(0 == Count, "%s", "Cannot pop elements of an empty array.");
			--Count;
			gpk_safe_podcpy(oldValue, &Data[Count]);
			return Count;
		}

		// Returns the index of the pushed value or -1 on failure
							::gpk::error_t				push_back									(const _tPOD& newValue)														noexcept	{
			const int32_t										oldSize										= Count;
			const int32_t										newSize										= oldSize + 1;
			const int32_t										finalSize									= resize(newSize, newValue);
			ree_if(finalSize != newSize, "Failed to push value! Array size: %i.", oldSize);
			return oldSize;
		}

		// Returns the index of the pushed value
		template<size_t _Length>
		inline				::gpk::error_t				append_string								(const _tPOD (&newChain)[_Length])											noexcept	{ return append(newChain, (uint32_t)strnlen(newChain, (uint32_t)_Length)); }
		inline				::gpk::error_t				append_string								(const ::gpk::view_const_string & newChain)									noexcept	{ return append(newChain.begin(), newChain.size()); }
		template<size_t _Length>
		inline				::gpk::error_t				append										(const _tPOD (&newChain)[_Length])											noexcept	{ return append(newChain, (uint32_t)_Length);					}
		inline				::gpk::error_t				append										(const ::gpk::view_array<const _tPOD>& newChain)							noexcept	{ return append(newChain.begin(), newChain.size());	}
							::gpk::error_t				append										(const _tPOD* chainToAppend, uint32_t chainLength)							noexcept	{
			const uint32_t										startIndex									= Count;
			const uint32_t										requestedSize								= Count + chainLength;
			ree_if(requestedSize < Count, "Size overflow. Cannot append chain. count: %u. Chain length: %u.", Count, chainLength);
			const int32_t										newSize										= resize(requestedSize);
			ree_if(newSize != (int32_t)requestedSize, "Failed to resize array for appending. Requested size: %u.", requestedSize);
			if(chainLength) {
				memcpy(&Data[startIndex], chainToAppend, sizeof(_tPOD) * chainLength);
				*(uint16_t*)(&Data[Count])						= 0;
			}
			return startIndex;
		}

		// Returns the new size of the array.
							::gpk::error_t				resize										(uint32_t newSize, const _tPOD& newValue)									noexcept	{
			int32_t												oldCount									= (int32_t)Count;
			int32_t												newCount									= resize(newSize);
			ree_if(newCount != (int32_t)newSize, "Failed to resize array. Requested size: %u. Current size: %u (%u).", newCount, Count, Size);
			for( int32_t i = oldCount; i < newCount; ++i )
				Data[i]											= newValue;
			return newCount;
		}

		int32_t											reserve										(uint32_t newCount)								{
			if(newCount > Size) {
				uint32_t											newSize										= ::gpk::max(4U, (uint32_t)(newCount + (newCount / 4)));
				_tPOD												* newData									= (_tPOD*)::gpk::gpk_malloc(newSize * sizeof(_tPOD) + 2);
				if(0 == newData)
					return -1;
				for(uint32_t iElement = 0; iElement < Count; ++iElement)
					new (&newData[iElement]) _tPOD(Data[iElement]);
				_tPOD								* oldData			= Data;
				Size								= newSize;
				Data								= newData;
				for(uint32_t iElement = 0; iElement < Count; ++iElement)
					oldData[iElement].~_tPOD();
				::gpk::gpk_free(oldData);
			}
			return Count;
		}
		// Returns the new size of the array.
							::gpk::error_t				resize										(uint32_t newSize)															noexcept	{
			const uint32_t										oldCount									= Count;
			if(newSize >= Size) {
				_tPOD												* oldData									= Data;
				uint32_t											reserveSize									= calc_reserve_size(newSize);
				uint32_t											mallocSize									= calc_malloc_size(reserveSize) + 2;
				ree_if(mallocSize != (reserveSize * (uint32_t)sizeof(_tPOD) + 2), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize);
				::gpk::auto_gpk_free								safeguard;
				_tPOD												* newData									= (_tPOD*)(safeguard.Handle = ::gpk::gpk_malloc(mallocSize));
				ree_if(nullptr == newData, "Failed to resize array. Requested size: %u. Current size: %u.", newSize, (uint32_t)Size);
				memset(&((char*)safeguard.Handle)[newSize * sizeof(_tPOD)], 0, mallocSize - newSize * sizeof(_tPOD));
				//TArrayView											safe_data									= {newData, reserveSize};
				if(oldData)
					memcpy(newData, oldData, ::gpk::min(newSize, oldCount) * sizeof(_tPOD));
				Size											= (uint32_t)reserveSize;
				Count											= newSize;
				Data											= newData;
				safeguard.Handle								= 0;
				if(oldData)
					::gpk::gpk_free(oldData);
			}
			else
				Count											= newSize;
			*(uint16_t*)(&Data[Count])						= 0;
			return (int32_t)Count;
		}

		// returns the new size of the list or -1 on failure.
							::gpk::error_t				insert										(uint32_t index, const _tPOD& newValue)										noexcept	{
			ree_if(index >= Count, "Invalid index: %u.", index);
			if(Size < Count + 1) {
				_tPOD												* oldData									= Data;
				uint32_t											reserveSize									= calc_reserve_size(Count + 1);
				uint32_t											mallocSize									= calc_malloc_size(reserveSize) + 2;
				ree_if(mallocSize != (reserveSize * sizeof(_tPOD) + 2), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize);
				::gpk::auto_gpk_free								safeguard;
				_tPOD												* newData									= (_tPOD*)(safeguard.Handle = ::gpk::gpk_malloc(mallocSize));
				ree_if(nullptr == newData, "Failed to allocate array for inserting new value. Memory requesteD: %u.", mallocSize);

				TArrayView											viewSafe									= {newData, Count+1};
				memcpy(viewSafe.begin(), oldData, ::gpk::min(index, Count) * sizeof(_tPOD));
				viewSafe[index]									= newValue;
				for(uint32_t i = index, maxCount = Count; i < maxCount; ++i)
					viewSafe[i + 1]									= oldData[i];
				Data											= newData;
				safeguard.Handle								= 0;
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
							::gpk::error_t				insert										(uint32_t index, const _tPOD* chainToInsert, uint32_t chainLength)			noexcept	{
			ree_if(index >= Count, "Invalid index: %u.", index);

			if(Size < Count + chainLength) {
				_tPOD												* oldData									= Data;
				uint32_t											newSize										= Count + chainLength;
				uint32_t											reserveSize									= calc_reserve_size(newSize);
				uint32_t											mallocSize									= calc_malloc_size(reserveSize) + 2;
				ree_if(mallocSize != (reserveSize * (uint32_t)sizeof(_tPOD) + 2), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize);
				::gpk::auto_gpk_free								safeguard;
				ree_if(nullptr == (safeguard.Handle = ::gpk::gpk_malloc(mallocSize)), "Failed to allocate array for inserting new value. memory requesteD: %u.", mallocSize);
				::gpk::view_array<_tPOD>							viewSafe									= {(_tPOD*)safeguard.Handle, newSize};
				uint32_t											i, maxCount;
				for(i = 0, maxCount = ::gpk::min(index, Count)					; i < maxCount; ++i) viewSafe[i]						= oldData[i];
				for(i = 0, maxCount = ::gpk::min(chainLength, newSize - index)	; i < maxCount; ++i) viewSafe[i + index]				= chainToInsert[i];
				for(i = 0, maxCount = Count - index								; i < maxCount; ++i) viewSafe[i + index + chainLength]	= oldData[i + index];
				Data											= (_tPOD*)safeguard.Handle; //newData;
				safeguard.Handle								= 0;
				::gpk::gpk_free(oldData);
			}
			else {	// no need to reallocate and copy, just shift rightmost elements and insert in-place
				for(int32_t  i = (int32_t)::gpk::min(index, Count - 1), maxCount = (int32_t)index	; i >=	maxCount; --i)
					Data[i + chainLength]	= Data[i];
				for(uint32_t i = 0, maxCount = chainLength; i <	maxCount; ++i)
					Data[i + index		]	= chainToInsert[i];
			}
			return Count += chainLength;
		}

		inline				::gpk::error_t				insert										(uint32_t index, ::gpk::view_array<const _tPOD> chainToInsert)				noexcept	{ return insert(index, chainToInsert.begin(), chainToInsert.size()); }

		template<size_t _chainLength>
		inline				::gpk::error_t				insert										(uint32_t index, const _tPOD* (&chainToInsert)[_chainLength])				noexcept	{ return insert(index, chainToInsert, _chainLength); }

		// Returns the new size of the list or -1 if the array pointer is not initialized.
							::gpk::error_t				remove_unordered							(uint32_t index)															noexcept	{
			ree_if(0 == Data, "%s", "Uninitialized array pointer!");
			ree_if(index >= Count, "Invalid index: %u.", index);
			Data[index]										= Data[--Count];
			return Count;
		}

		// returns the new array size or -1 if failed.
		template<typename _tObj>
							::gpk::error_t				erase										(const _tObj* address)														noexcept	{
			ree_if(0 == Data, "Uninitialized array pointer! When trying to erase element at address: %p", address);
			const ptrdiff_t										ptrDiff										= ptrdiff_t(address) - (ptrdiff_t)Data;
			const uint32_t										index										= (uint32_t)(ptrDiff / (ptrdiff_t)sizeof(_tObj));
			ree_if(index >= Count, "Invalid index: %u.", index);
			return remove(index);
		}

		// returns the new array size or -1 if failed.
							::gpk::error_t				remove										(uint32_t index)															noexcept	{
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
		inline				::gpk::error_t				find										(const _tPOD& valueToLookFor)										const	noexcept	{
			for(uint32_t i = 0; i < Count; ++i)
				if(0 == ::gpk::podcmp(&Data[i], &valueToLookFor))
					return i;
			return -1;
		}

							::gpk::error_t				read										(const byte_t* input, uint32_t* inout_bytesRead)							noexcept	{
			uint32_t											elementsToRead								= 0;
			if(input) {
				elementsToRead									= *(uint32_t*)input;
				input											+= sizeof(uint32_t);
			}

			*inout_bytesRead								+= sizeof(uint32_t);
			if(0 == elementsToRead) {
				Count											= 0;
				return 0;
			}

			::gpk::view_array<_tPOD>							newStorage									= {(_tPOD*)::gpk::gpk_malloc(sizeof(_tPOD) * elementsToRead + 2), elementsToRead};
			ree_if(0 == newStorage.begin(), "Failed to allocate array for storing read %u elements.", elementsToRead);
			if(0 == input) {
				for(uint32_t i = 0; i < Count; ++i)
					*inout_bytesRead								+= sizeof(_tPOD);
			}
			else {
				for(uint32_t i = 0; i < Count; ++i) {
					::gpk::podcpy(&newStorage[i], input);
					input											+= sizeof(_tPOD);
					*inout_bytesRead								+= sizeof(_tPOD);
				}
			}
			Count											= elementsToRead;
			Size											= elementsToRead;
			_tPOD											* old											= Data;
			Data											= newStorage.begin();
			::gpk::safe_gpk_free(old);
			return 0;
		}

							::gpk::error_t				write										(byte_t* input, uint32_t* inout_bytesWritten)						const	noexcept	{
			if(0 == input) {
				inout_bytesWritten							+= sizeof(uint32_t) + sizeof(_tPOD) * Count;	// Just return the size required to store this.
				return 0;
			}

			*(uint32_t*)input								= Count;
			input											+= sizeof(uint32_t);
			*inout_bytesWritten								+= sizeof(uint32_t);

			::gpk::view_array<_tPOD>							newStorage									= {input, Count};
			for(uint32_t i = 0; i < Count; ++i) {
				::gpk::podcpy(&newStorage[i], &Data[i]);
				input											+= sizeof(_tPOD);
				*inout_bytesWritten								+= sizeof(_tPOD);
			}
			return 0;
		}
	}; // array_pod

	//template<typename _tPOD>
	//struct string : public array_pod<_tPOD> {};

	//------------------------------------------------------------------------------------------------------------
	template<typename _tObj>
	struct array_obj : public array_base<_tObj> {
		typedef				array_base<_tObj>			TVectorBase;

		using											TVectorBase::Count;
		using											TVectorBase::Data;
		using											TVectorBase::Size;
		using											TVectorBase::calc_reserve_size;
		using											TVectorBase::calc_malloc_size;
		using											TVectorBase::operator[];

		inline											~array_obj									()																						{ for(uint32_t i = 0; i < Count; ++i) Data[i].~_tObj(); safe_gpk_free(Data); }	// dtor
		inline constexpr								array_obj									()																						= default;
		inline											array_obj									(const uint32_t newSize)																{ gthrow_if(((int32_t)newSize) != resize(newSize), "", "%s", "Failed to resize array."); }
		inline											array_obj									(array_obj<_tObj>&& other)													noexcept	{
			Size											= other.Size									;
			Count											= other.Count									;
			Data											= other.Data									;

			other.Size										= other.Count									= 0;
			other.Data										= 0;
		}	// move ctor
														array_obj									(const array_obj<_tObj>& other)	: array_obj((const view_array<const _tObj>&) other) {}
														array_obj									(const view_array<const _tObj>& other)													{
			if(other.size()) {
				uint32_t											newSize										= other.size();
				uint32_t											reserveSize									= calc_reserve_size(newSize);
				uint32_t											mallocSize									= calc_malloc_size(reserveSize);
				gthrow_if(mallocSize != (reserveSize*(uint32_t)sizeof(_tObj)), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize);
				::gpk::auto_gpk_free								safeguard;
				_tObj												* newData									= (_tObj*)(safeguard.Handle = ::gpk::gpk_malloc(mallocSize));
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
		inline constexpr	operator					view_array<const _tObj>						()								const	noexcept	{ return {Data, Count}; }
		inline				array_obj<_tObj>&			operator =									(const array_obj<_tObj>& other)															{
			gthrow_if(resize(other.Count) != (int32_t)other.Count, "", "Failed to resize array!");
			for(uint32_t iElement = 0; iElement < other.Count; ++iElement)
				this->operator[](iElement)							= other[iElement];
			return *this;
		}
		inline				int32_t						clear										()																						{
			for(uint32_t i = 0; i < Count; ++i)
				Data[i].~_tObj();
			return Count									= 0;
		}
		// Returns the new size of the array
		inline				int32_t						pop_back									(_tObj* oldValue)																		{
			ree_if(0 == Count, "%s", "Cannot pop elements from an empty array.");
			--Count;
			gpk_safe_assign(oldValue, Data[Count]);
			Data[Count].~_tObj();
			return Count;
		}
		// Returns the index of the pushed value or -1 on failure
		inline				int32_t						push_back									(const _tObj& newValue)																	{
			int32_t												indexExpected								= Count;
			int32_t												indexNew									= resize(Count+1, newValue)-1;
			ree_if(indexNew != indexExpected, "Failed to push value! Array size: %i.", indexExpected);
			return indexNew;
		}

		// returns the new array size or -1 if failed.
		inline				int32_t						erase										(const _tObj* address)																	{
			ree_if(0 == Data, "Uninitialized array pointer! Invalid address to erase: %p.", address);
			const ptrdiff_t										ptrDiff										= ptrdiff_t(address) - (ptrdiff_t)Data;
			const uint32_t										index										= (uint32_t)(ptrDiff / (ptrdiff_t)sizeof(_tObj));
			ree_if(index >= Count, "Invalid index: %u.", index);
			return remove(index);
		}

							int32_t						remove										(uint32_t index)																		{
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
							int32_t						remove_unordered							(uint32_t index)																		{
			ree_if(0 == Data, "Uninitialized array pointer! Invalid index to erase: %u.", index);
			ree_if(index >= Count, "Invalid index: %u.", index);
			Data[index].~_tObj();							// Destroy old
			if(1 == Count)
				--Count;
			else {
				new (&Data[index]) _tObj(Data[--Count]);	// Placement new
				Data[Count].~_tObj();						// Destroy reordered
			}

			return Count;
		}
		// returns the new size of the list or -1 on failure.
							int32_t						insert										(uint32_t index, const _tObj& newValue)													{
			ree_if(index >= Count, "Invalid index: %u.", index);

			if(Size < Count + 1) {
				_tObj												* oldData								= Data;
				uint32_t											reserveSize								= calc_reserve_size(Count+1);
				uint32_t											mallocSize								= calc_malloc_size(reserveSize);
				ree_if(mallocSize != (reserveSize*(uint32_t)sizeof(_tObj)), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize);
				::gpk::auto_gpk_free								safeguard;
				_tObj												* newData								= (_tObj*)(safeguard.Handle = ::gpk::gpk_malloc(mallocSize));
				ree_if(0 == newData, "Failed to allocate for inserting new element into array! current size: %u. new size: %u.", Size, mallocSize);
				::gpk::view_array<_tObj>							viewSafe								= {newData, Count+1};
				for(uint32_t i = 0, maxCount = ::gpk::min(index, Count); i < maxCount; ++i) {
					new (&viewSafe[i]) _tObj(oldData[i]);
					oldData[i].~_tObj();
				}
				new (&viewSafe[index]) _tObj(newValue);
				for(uint32_t i = index, maxCount = Count; i < maxCount; ++i) {
					new (&viewSafe[i + 1]) _tObj(oldData[i]);
					oldData[i].~_tObj();
				}
				Data											= newData;
				::gpk::gpk_free(oldData);
				Size											= reserveSize;
				safeguard.Handle								= 0;
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
		int32_t									reserve				(uint32_t newCount)								{
			if(newCount > Size) {
				uint32_t									newSize				= ::gpk::max(4U, newCount + (newCount / 4));
				_tObj										* newData			= (_tObj*)::gpk::gpk_malloc(newSize * sizeof(_tObj));
				if(0 == newData)
					return -1;
				for(uint32_t iElement = 0; iElement < Count; ++iElement)
					new (&newData[iElement]) _tObj(Data[iElement]);
				_tObj								* oldData			= Data;
				Size								= newSize;
				Data								= newData;
				for(uint32_t iElement = 0; iElement < Count; ++iElement)
					oldData[iElement].~_tObj();
				::gpk::gpk_free(oldData);
			}
			return Count;
		}
		// Returns the new size of the array or -1 if failed.
		template <typename... _tArgs>
							int32_t						resize										(uint32_t newSize, _tArgs&&... constructorArgs)											{
			uint32_t											oldCount									= Count;
			if(newSize > Size) {
				_tObj												* oldData									= Data;
				uint32_t											reserveSize									= calc_reserve_size(newSize);
				uint32_t											mallocSize									= calc_malloc_size(reserveSize);
				ree_if(mallocSize != (reserveSize*(uint32_t)sizeof(_tObj)), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize);
				::gpk::auto_gpk_free								safeguard;
				_tObj												* newData									= (_tObj*)(safeguard.Handle = ::gpk::gpk_malloc(mallocSize));
				memset(&((char*)safeguard.Handle)[newSize * sizeof(_tObj)], 0, mallocSize - newSize * sizeof(_tObj));
				ree_if(nullptr == newData, "Failed to resize array. Requested size: %u. Current size: %u.", newSize, Size);
				if(oldData) {
					for(uint32_t i = 0, copyCount = ::gpk::min(oldCount, newSize); i < copyCount; ++i)
						new (&newData[i]) _tObj{oldData[i]};
					for(uint32_t i = 0; i < oldCount; ++i)
						oldData[i].~_tObj();
				}
				Data											= newData;
				Size											= reserveSize;
				Count											= (uint32_t)newSize;
				safeguard.Handle								= 0;
				if(oldData)
					::gpk::gpk_free(oldData);
			}
			else {
				for(int32_t i = ((int32_t)oldCount) - 1, newCount = (int32_t)newSize; i >= newCount; --i)	// we need to cast to int32_t because otherwise --i	will underflow to UINT_MAX and loop forever.
					Data[i].~_tObj();
				Count											= (uint32_t)newSize;
			}
			for(uint32_t i = oldCount; i < Count; ++i)
				new (&Data[i])_tObj{constructorArgs...};

			return (int32_t)Count;
		}
	}; // array_obj

	template<typename _tElement>
	::gpk::error_t									split					(const ::gpk::view_array<const _tElement> & target, const _tElement& separator, ::gpk::array_obj<::gpk::view_array<const _tElement>> & split)	{
		uint32_t											lastOffset				= 0;
		for(uint32_t iChar = 0; iChar < target.size(); ++iChar) {
			if(target[iChar] == separator) {
				const ::gpk::view_array<const _tElement>			newView					= {&target[lastOffset], iChar - lastOffset};
				++iChar;
				gpk_necall(split.push_back(newView), "%s", "Out of memory?");
				lastOffset										= iChar;
			}
		}
		if(lastOffset < target.size())
			gpk_necall(split.push_back({&target[lastOffset], target.size() - lastOffset}), "%s", "Out of memory?");
		return (int32_t)split.size();
	}

	template<typename _tElement>
	::gpk::error_t									split					(const ::gpk::view_const_string & target, const _tElement& separator, ::gpk::array_obj<view_const_string> & split)	{
		int32_t												lastOffset				= 0;
		for(int32_t iChar = 0, countChars = target.size(); iChar < countChars; ++iChar) {
			if(target[iChar] == separator) {
				const ::gpk::view_const_string					newView					= {&target[lastOffset], (uint32_t)::gpk::max(0, iChar - lastOffset)};
				++iChar;
				gpk_necall(split.push_back(newView), "%s", "Out of memory?");
				lastOffset										= iChar;
			}
		}
		if(lastOffset < (int32_t)target.size()) {
			gpk_necall(split.push_back({&target[lastOffset], target.size() - lastOffset}), "%s", "Out of memory?");
			//if(split[split.size()-1][] == separator)
		}
		return (int32_t)split.size();
	}


	template<typename _tElement>
	::gpk::error_t									viewWrite							(const ::gpk::view_array<const _tElement>& headerToWrite, ::gpk::array_pod<byte_t>	& output)	{
		output.append(::gpk::view_const_byte{(const char*)&headerToWrite.size(), (uint32_t)sizeof(uint32_t)});
		output.append(::gpk::view_const_byte{(const char*)headerToWrite.begin(), headerToWrite.size() * (uint32_t)sizeof(_tElement)});
		return sizeof(uint32_t) + headerToWrite.size() * sizeof(_tElement);
	}

	template<typename _tElement>
	::gpk::error_t									viewRead							(::gpk::view_array<const _tElement> & headerToRead, const ::gpk::view_const_byte	& input	)	{
		ree_if(input.size() < 4, "Invalid input size: %u", input.size());
		const uint32_t										elementCount						= *(uint32_t*)input.begin();
		ree_if((elementCount * sizeof(_tElement)) > (input.size() - sizeof(uint32_t)), "Invalid input size: %u. Expected: %u", input.size(), elementCount * sizeof(_tElement));
		headerToRead									= {(input.size() > sizeof(uint32_t)) ? (const _tElement*)&input[sizeof(uint32_t)] : 0, elementCount};
		return sizeof(uint32_t) + headerToRead.size() * sizeof(_tElement);
	}

	template<typename _tElement>
	::gpk::error_t									viewRead							(::gpk::view_array<_tElement> & headerToRead, const ::gpk::view_byte	& input	)	{
		ree_if(input.size() < 4, "Invalid input size: %u", input.size());
		const uint32_t										elementCount						= *(uint32_t*)input.begin();
		ree_if((elementCount * sizeof(_tElement)) > (input.size() - sizeof(uint32_t)), "Invalid input size: %u. Expected: %u", input.size(), elementCount * sizeof(_tElement));
		headerToRead									= {(input.size() > sizeof(uint32_t)) ? (_tElement*)&input[sizeof(uint32_t)] : 0, elementCount};
		return sizeof(uint32_t) + headerToRead.size() * sizeof(_tElement);
	}

	static inline	::gpk::error_t					viewRead							(::gpk::view_const_string & headerToRead, const ::gpk::view_const_byte	& input	)	{
		ree_if(input.size() < 4, "Invalid input size: %u", input.size());
		const uint32_t										elementCount						= *(uint32_t*)input.begin();
		ree_if(elementCount > (input.size() - sizeof(uint32_t)), "Invalid input size: %u. Expected: %u", input.size(), elementCount);
		headerToRead									= {(input.size() > sizeof(uint32_t)) ? &input[sizeof(uint32_t)] : 0, elementCount};
		return sizeof(uint32_t) + headerToRead.size();
	}

	typedef ::gpk::SKeyVal<::gpk::view_const_string, ::gpk::array_obj<::gpk::view_const_string>>	TKeyValConstStringArray;

	::gpk::error_t									keyValConstStringSerialize		(const ::gpk::view_array<const ::gpk::TKeyValConstString> & keyVals, const ::gpk::view_array<const ::gpk::view_const_char> & keysToSave, ::gpk::array_pod<byte_t> & output);
	::gpk::error_t									keyValConstStringDeserialize	(const ::gpk::view_const_byte & input, ::gpk::array_obj<::gpk::TKeyValConstString> & output);
	::gpk::array_pod<char_t>						toString						(const ::gpk::view_const_char& strToLog);
	::gpk::error_t									join							(::gpk::array_pod<char_t> & query, char separator, ::gpk::view_array<const gpk::view_const_char>	fields);
	::gpk::error_t									append_quoted					(::gpk::array_pod<char_t>& output, ::gpk::view_const_char text);
}

#endif // GPK_ARRAY_H_29837498237498237429837
