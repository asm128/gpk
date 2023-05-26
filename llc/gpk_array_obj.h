#include "gpk_array_base.h"

#include "gpk_keyval.h"

#ifndef GPK_ARRAY_OBJ_H_230518
#define GPK_ARRAY_OBJ_H_230518

namespace gpk
{
	template<typename _tObj>
	struct array_obj : public array_base<_tObj> {
		typedef	_tObj			T						;
		typedef	view<T>			TView					;
		typedef	array_obj<T>	TArray					;

		using array_base<T>		::Count					;
		using array_base<T>		::Data					;
		using array_base<T>		::Size					;
		using array_base<T>		::alloc_with_reserve	;

		inline					~array_obj			()																				{ for(uint32_t i = 0; i < Count; ++i) Data[i].~T(); }	// dtor

		inlcxpr					array_obj			()																				= default;
		inline					array_obj			(TArray&& other)											noexcept	{
			Size					= other.Size;
			Count					= other.Count;
			Data					= other.Data;

			other.Size = other.Count	= 0;
			other.Data				= 0;
		}	// move ctor
								array_obj			(const TArray & other)			: array_obj((const view<const T>&) other)	{}
								array_obj			(const view<const T> & other)												{
			uint32_t					newCount			= other.size();
			if(newCount) {
				gthrow_if(errored(reserve(newCount)), "Requested size: %u. ", (uint32_t)newCount);
				for(; Count < newCount; ++Count)
					new (&Data[Count]) T(other.begin()[Count]);
			}
		}
								array_obj			(::std::initializer_list<T> init)				{
			gthrow_if(errored(append(init.begin(), (uint32_t)init.size())), "Failed to resize array! Why? Initializer list size: %u.", (uint32_t)init.size());
		}
		template<size_t _count>
								array_obj			(const T (&other)[_count])							{
			gthrow_if(errored(append(other, (uint32_t)_count)), "Failed to resize array! Why? Initializer list size: %u.", (uint32_t)_count);
		}
		inlcxpr	operator		view<const T>		()									const	noexcept	{ return {Data, Count}; }
		inline	TArray&			operator=			(const TArray & other)									{
			gsthrow_if(resize(other.size()) != (int32_t)other.size());
			for(uint32_t iElement = 0; iElement < Count; ++iElement)
				Data[iElement]			= other[iElement];
			return *this;
		}
		::gpk::error_t			clear				()														{ for(uint32_t i = 0; i < Count; ++i) Data[i].~T(); return Count = 0; }
		::gpk::error_t			clear_pointer		()											noexcept	{ clear(); safe_gpk_free(Data); return Size = 0; }

		::gpk::error_t			reserve				(uint32_t newCount)										{
			if(newCount > Size) {
				T							* newData			= 0;
				const uint32_t				newSize				= alloc_with_reserve(newCount, newData);
				rees_if(0 == newData);
				for(uint32_t iElement = 0; iElement < Count; ++iElement) {
					new (&newData[iElement]) T(Data[iElement]);
					Data[iElement].~T();
				}
				T						* oldData			= Data;
				Data					= newData;
				Size					= newSize;
				::gpk::gpk_free(oldData);
			}
			return Size;
		}

		// Returns the new size of the array.
		::gpk::error_t			resize				(uint32_t newCount)															noexcept	{
			gpk_necs(reserve(newCount));
			for(; Count < newCount; ++Count)
				new (&Data[Count]) T{};
			for(; Count > newCount; --Count)
				Data[Count - 1].~T();
			return Count;
		}

		// Returns the new size of the array.
		::gpk::error_t			resize				(uint32_t newCount, const T & newValue)									noexcept	{
			gpk_necs(reserve(newCount));
			for(; Count < newCount; ++Count)
				new (&Data[Count]) T{newValue};
			for(; Count > newCount; --Count)
				Data[Count - 1].~T();
			return Count;
		}

		// Returns the new size of the array.
		template <typename... _tArgs>
		::gpk::error_t			resize				(uint32_t newCount, _tArgs&&... constructorArgs)											{
			gpk_necs(reserve(newCount));
			for(; Count < newCount; ++Count)
				new (&Data[Count]) T{constructorArgs...};
			for(; Count > newCount; --Count)
				Data[Count - 1].~T();
			return Count;
		}

		// Returns the new size of the array
		inline	::gpk::error_t	pop_back			()											noexcept	{
			ree_if(0 == Count, "%s", "Cannot pop elements of an empty array.");
			Data[--Count].~T();
			return Count;
		}

		// Returns the new size of the array
		inline	::gpk::error_t	pop_back			(T & oldValue)								noexcept	{
			ree_if(0 == Count, "%s", "Cannot pop elements of an empty array.");
			oldValue				= Data[--Count];
			Data[Count].~T();
			return Count;
		}

		// Returns the index of the pushed value or -1 on failure
		::gpk::error_t			push_back			(const T & newValue)														noexcept	{
			const int32_t				oldSize				= Count;
			gpk_necs(resize(oldSize + 1, newValue));
			return oldSize;
		}

		// returns the new array size or -1 if failed.
		inline	::gpk::error_t	erase				(const T * address)																	{
			ree_if(0 == Data, "Uninitialized array pointer! Invalid address to erase: %p.", address);
			const ptrdiff_t				ptrDiff				= ptrdiff_t(address) - (ptrdiff_t)Data;
			const uint32_t				index				= (uint32_t)(ptrDiff / (ptrdiff_t)sizeof(T));
			ree_if(index >= Count, "Invalid index: %u.", index);
			return remove(index);
		}

		::gpk::error_t			remove				(uint32_t index)																		{
			ree_if(0 == Data, "Uninitialized array pointer! Invalid index to erase: %u.", index);
			ree_if(index >= Count, "Invalid index: %u.", index);
			--Count;
			while(index < Count) {
				Data[index].~T();							// Destroy old
				new (&Data[index]) T(Data[index + 1]);		// Placement new
				++index;
			}
			Data[index].~T();							// Destroy last
			return Count;
		}

		// Returns the new size of the list or -1 if the array pointer is not initialized.
		::gpk::error_t			remove_unordered	(uint32_t index)																		{
			ree_if(0 == Data, "Uninitialized array pointer! Invalid index to erase: %u.", index);
			ree_if(index >= Count, "Invalid index: %u.", index);
			Data[index].~T();							// Destroy old
			if(1 == Count || index == (Count - 1))
				--Count;
			else {
				new (&Data[index]) T(Data[--Count]);	// Placement new
				Data[Count].~T();						// Destroy reordered
			}

			return Count;
		}
		template<size_t _len>
		inline	::gpk::error_t	append				(const T (&newChain)[_len])							noexcept	{ return append(newChain, (uint32_t)_len); }
		inline	::gpk::error_t	append				(const ::gpk::view<const T> & newChain)				noexcept	{ return append(newChain.begin(), newChain.size());	}
		::gpk::error_t			append				(const T * chainToAppend, uint32_t chainLength)		noexcept	{
			if(0 == chainLength)
				return Count;

			gpk_necs(reserve(Count + chainLength));
			const uint32_t				iFirst				= Count;
			for(uint32_t i = 0; i < chainLength; ++i)
				new (&Data[Count++]) T{chainToAppend[i]};
			return iFirst;
		}

		// returns the new size of the list or -1 on failure.
		::gpk::error_t			insert				(uint32_t index, const T & newValue)										noexcept	{
			ree_if(index > Count, "Invalid index: %u.", index);
			const uint32_t				newCount			= Count + 1;
			if(Size < newCount) {
				T							* newData			= 0;
				const uint32_t				newSize				= alloc_with_reserve(newCount, newData);
				rees_if(0 == newData);
				for(uint32_t i = 0; i < index; ++i) {
					new (&newData[i]) T(Data[i]);
					Data[i].~T();
				}
				new (&newData[index]) T(newValue);
				for(uint32_t i = index; i < Count; ++i) {
					new (&newData[i + 1]) T(Data[i]);
					Data[i].~T();
				}
				T							* oldData			= Data;
				Data					= newData;
				Size					= newSize;
				::gpk::gpk_free(oldData);
			}
			else {
				for(int32_t i = (int)Count - 1; i >= (int)index; --i) {
					new (&Data[i + 1]) T(Data[i]);
					Data[i].~T();
				}
				new (&Data[index]) T(newValue);
			}
			return Count = newCount;
		}

		// returns the new size of the list or -1 on failure.
		::gpk::error_t			insert				(uint32_t index, const T * chainToInsert, uint32_t chainLength)			noexcept	{
			ree_if(index > Count, "Invalid index: %u.", index);

			const uint32_t				newCount			= Count + chainLength;
			if(Size < newCount) {
				T							* newData			= 0;
				const uint32_t				newSize				= alloc_with_reserve(newCount, newData);
				rees_if(0 == newData);
				for(uint32_t i = 0; i < index; ++i) {
					new (&newData[i]) T(Data[i]);
					Data[i].~T();
				}

				for(uint32_t i = 0; i < chainLength; ++i) 
					new (&newData[index + i]) T(chainToInsert[i]);

				for(uint32_t i = index; i < Count; ++i) {
					new (&newData[i + chainLength]) T(Data[i]);
					Data[i].~T();
				}
				T							* oldData			= Data;
				Data					= newData;
				Size					= newSize;
				::gpk::gpk_free(oldData);
			}
			else {	// no need to reallocate and copy, just shift rightmost elements and insert in-place
				for(int32_t i = (int)Count - 1; i >= (int)index; --i) {
					new (&Data[i + chainLength]) T(Data[i]);
					Data[i].~T();
				}

				for(uint32_t i = 0; i < chainLength; ++i) 
					new (&Data[index + i]) T(chainToInsert[i]);
			}
			return Count = newCount;
		}

		template<size_t _chainLength>
		inline	::gpk::error_t	insert				(uint32_t index, const T* (&chainToInsert)[_chainLength])		noexcept	{ return insert(index, chainToInsert, (uint32_t)_chainLength); }
		inline	::gpk::error_t	insert				(uint32_t index, ::gpk::view<const T> chainToInsert)			noexcept	{ return insert(index, chainToInsert.begin(), chainToInsert.size()); }

	}; // array_obj

	template <typename T>	using aobj		= ::gpk::array_obj<T>; 
	template <typename T>	using ao		= ::gpk::aobj<T>; 

	template <typename T>	using aview		= ::gpk::aobj	<::gpk::view<T>>; 
	template <typename T>	using av		= ::gpk::aview	<T>; 

	typedef	::gpk::aview<uchar_t	>	aview_uchar			, av1uc, avuc;
	typedef	::gpk::aview<char		>	aview_char			, av1c, avc;
	typedef	::gpk::aview<float		>	aview_float32		, av1f32, av1f, avf32, avf;
	typedef	::gpk::aview<double		>	aview_float64		, av1f64, av1d, avf64, avd;
	typedef	::gpk::aview<uint8_t	>	aview_uint8			, av1u8, avu8;
	typedef	::gpk::aview<uint16_t	>	aview_uint16		, av1u16, avu16;
	typedef	::gpk::aview<uint32_t	>	aview_uint32		, av1u32, avu32;
	typedef	::gpk::aview<uint64_t	>	aview_uint64		, av1u64, avu64;
	typedef	::gpk::aview<int8_t		>	aview_int8			, av1i8, avi8;
	typedef	::gpk::aview<int16_t	>	aview_int16			, av1i16, avi16;
	typedef	::gpk::aview<int32_t	>	aview_int32			, av1i32, avi32;
	typedef	::gpk::aview<int64_t	>	aview_int64			, av1i64, avi64;

	// view_array<const> common typedefs
	//typedef ::gpk::view_array<const char		>	view_const_string	;
	typedef	::gpk::aview<const uchar_t	>	aview_const_uchar	, av1cuc, avcuc;
	typedef	::gpk::aview<const char		>	aview_const_char	, av1cc, avcc;
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


	template<typename T>
	::gpk::error_t							split					(const ::gpk::view<const T> & target, const T & separator, ::gpk::aobj<::gpk::view<const T>> & split)	{
		uint32_t									lastOffset				= 0;
		for(uint32_t iChar = 0; iChar < target.size(); ++iChar) {
			if(target[iChar] == separator) {
				const ::gpk::view<const T>			newView					= {&target[lastOffset], iChar - lastOffset};
				++iChar;
				gpk_necs(split.push_back(newView));
				lastOffset								= iChar;
			}
		}
		if(lastOffset < target.size())
			gpk_necs(split.push_back({&target[lastOffset], target.size() - lastOffset}));
		return (int32_t)split.size();
	}

	template<typename T>
	::gpk::error_t							split					(const ::gpk::view<const T> & target, const ::gpk::view<const T>& separators, ::gpk::aobj<::gpk::view<const T>> & split)	{
		uint32_t									lastOffset				= 0;
		for(uint32_t iChar = 0; iChar < target.size(); ++iChar) {
			for(uint32_t iSeparator = 0; iSeparator < separators.size(); ++iSeparator) {
				if(target[iChar] == separators[iSeparator]) {
					const ::gpk::view<const T>			newView					= {&target[lastOffset], iChar - lastOffset};
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

	template<typename T>
	::gpk::error_t							split					(const ::gpk::vcs & target, const T & separator, ::gpk::aobj<::gpk::vcs> & split)	{
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
	//------------------------------------------------------------------------------------------------------------
	::gpk::error_t							keyValConstStringDeserialize	(const ::gpk::vcu8 & input, ::gpk::aobj<::gpk::TKeyValConstChar> & output);

	::gpk::error_t							filterPrefix					(::gpk::view<const ::gpk::vcc> input, const ::gpk::vcc prefix, ::gpk::aobj<::gpk::vcc> & filtered, bool nullIncluded = false);
	::gpk::error_t							filterPostfix					(::gpk::view<const ::gpk::vcc> input, const ::gpk::vcc prefix, ::gpk::aobj<::gpk::vcc> & filtered, bool nullIncluded = false);
}
#endif // GPK_ARRAY_OBJ_H_230518
