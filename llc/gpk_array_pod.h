	#include "gpk_array_base.h"


	#include "gpk_keyval.h"

	#include "gpk_view_serialize.h"

	#ifndef GPK_ARRAY_POD_H_23627
	#define GPK_ARRAY_POD_H_23627

	namespace gpk
	{
		// This class is optimized to contain POD instances and won't work for C++ objects that require calling constructors/destructors.
		tplt<tpnm _tPOD>
		struct array_pod : public array_base<_tPOD> {
			typedef	_tPOD			T						;
			typedef	view<T>			TView					;
			typedef	array_pod<T>	TArray					;

			using array_base<T>		::Count					;
			using array_base<T>		::Data					;
			using array_base<T>		::Size					;
			using array_base<T>		::alloc_with_reserve	;
			using array_base<T>		::find					;

			inlcxpr					array_pod			()										noexcept	= default;
									array_pod			(::std::initializer_list<T> init)				{
				gthrow_if(errored(resize((uint32_t)init.size())), "init.size() -> %u.", (uint32_t)init.size());
				memcpy(Data, init.begin(), Count * sizeof(T));
			}
									array_pod			(array_pod<T> && other)					noexcept	{
				Size					= other.Size;
				Count					= other.Count;
				Data					= other.Data;

				other.Size				= other.Count			= 0;
				other.Data				= 0;
			}
			inline					array_pod			(const array_pod<T> & other)						: array_pod((const view<const T>&) other) {}
									array_pod			(const view<const T> & other)						{
				gsthrow_if(resize(other.size()) != (int32_t)other.size());
				memcpy(Data, other.begin(), other.byte_count());
			}
			tplt<size_t _count>
									array_pod			(const T (&other)[_count])							{
				gsthrow_if(resize(_count) != (int32_t)_count);
				memcpy(Data, other, _count * sizeof(T));
			}
			inlcxpr	operator		view<const T>	()									const	noexcept	{ return {Data, Count}; }
			TArray&					operator =			(const array_pod<T>& other)							{ return operator=((const view<T> &) other); }
			TArray&					operator =			(const view<const T> & other)						{
				gsthrow_if(resize(other.size()) != (int32_t)other.size());
				if(Count)
					memcpy(Data, other.begin(), other.byte_count());
				return *this;
			}
			tplt<size_t sizeStatic>
			TArray&					operator =			(const T (&init)[sizeStatic])						{
				gsthrow_if(resize(sizeStatic) != sizeStatic);
				memcpy(Data, init, Count * sizeof(T));
				*(u1_t*)&Data[Count]		= 0;
				return *this;
			}

			inline	::gpk::error_t	clear				()										noexcept	{ return Count = 0; }
			::gpk::error_t			clear_pointer		()										noexcept	{ safe_gpk_free(Data); Data = 0; return Size = Count = 0; }
			// Returns the new size of the array
			::gpk::error_t			pop_back			()										noexcept	{
				rees_if(0 == Count);
				*(u1_t*)&Data[--Count]		= 0;
				return Count;
			}
			// Returns the new size of the array
			::gpk::error_t			pop_back			(T & oldValue)							noexcept	{
				rees_if(0 == Count);
				oldValue				= Data[--Count];
				*(u1_t*)&Data[Count]		= 0;
				return Count;
			}
			// Returns the index of the pushed value or -1 on failure
			::gpk::error_t			push_back			(const T & newValue)					noexcept	{
				const int32_t				oldSize				= Count;
				gpk_necs(resize(oldSize + 1, newValue));
				return oldSize;
			}
			inline	::gpk::error_t	append_string		(const gpk::function<err_t(TArray&)> & funcAppend)	noexcept	{ return funcAppend ? funcAppend(*this) : 0; }
			tplt<size_t _len>
			inline	::gpk::error_t	append_string		(const T (&newChain)[_len])							noexcept	{ return append(::gpk::vcs{newChain}); }
			inline	::gpk::error_t	append_string		(const ::gpk::vcs & newChain)						noexcept	{ return append(newChain.begin(), newChain.size()); }
			inline	::gpk::error_t	append_string		(const T & element)									noexcept	{ gpk_necs(push_back(element)); return 1; }
			inline	::gpk::error_t	append_strings		(const ::gpk::view<const ::gpk::vcs> & newChains)	noexcept	{ 
				int32_t					appended				= 0;
				for(uint32_t i = 0, stop = newChains.size(); i < stop; ++i) {
					const ::gpk::vcs & newChain = newChains[i];
					gpk_necs(append(newChain));
					appended += newChain.size();
				}
				return appended;
			}
			tplt<size_t 					_len>
			inline	::gpk::error_t	append				(const T (&newChain)[_len])									noexcept	{ return append(newChain, (uint32_t)_len); }
			inline	::gpk::error_t	append				(const ::gpk::view<const T> & newChain)						noexcept	{ return append(newChain.begin(), newChain.size());	}
			inline	::gpk::error_t	append				(const ::gpk::view<const ::gpk::view<const T>> & newChains)	noexcept	{ 
				int32_t count = 0;
				newChains.for_each([this, &count](const view<const T> &newChain) { if_fail_e(append(newChain)); count += newChain.size(); }); 
				return count;
			}
			::gpk::error_t			append				(const T * chainToAppend, uint32_t chainLength)				noexcept	{
				if(0 == chainLength)
					return Count;
				const uint32_t				newCount			= Count + chainLength;
				gpk_necs(reserve(newCount));
				const uint32_t				iFirst				= Count;
				memcpy(&Data[iFirst], chainToAppend, sizeof(T) * chainLength);
				Count					+= chainLength;
				*(u1_t*)&Data[Count]		= 0;
				return iFirst;
			}
			::gpk::error_t			reserve				(uint32_t newCount)								{
				if(newCount > Size) {
					T							* newData			= 0;
					const uint32_t				newSize				= alloc_with_reserve(newCount, newData);
					rees_if(0 == newData);
					if(Data)
						memcpy(newData, Data, this->byte_count());
						
					*(u1_t*)&newData[Count]	= 0;
					T							* oldData			= Data;
					Data					= newData;
					Size					= newSize;
					::gpk::gpk_free(oldData);
				}
				return Size;
			}

			inline	::gpk::error_t	resize_bits			(uint32_t newBitCount)						noexcept	{
				stacxpr	uint32_t			elementBits			= (uint32_t)sizeof(T) * 8U;
				return resize(newBitCount / elementBits + one_if(newBitCount % elementBits));
			}

			// Returns the new size of the array.
			::gpk::error_t			resize				(uint32_t newCount)						noexcept	{
				gpk_necs(reserve(newCount));
				Count					= newCount;
				if(Data)
					*(u1_t*)&Data[Count]		= 0;
				return Count;
			}
			// Returns the new size of the array.
			::gpk::error_t			resize				(uint32_t newCount, const T & newValue)	noexcept	{
				gpk_necs(reserve(newCount));
				if(Data) {
					for(; Count < newCount; ++Count)
						Data[Count] = newValue;
					*(u1_t*)&Data[Count = newCount]	= 0;
				}
				return Count;
			}
			// returns the new size of the list or -1 on failure.
			::gpk::error_t			insert				(uint32_t index, const T & newValue)	noexcept	{
				ree_if(index > Count, "%u > %u", index, Count);
				const uint32_t				newCount			= Count + 1;
				if(Size < newCount) {
					T							* newData			= 0;
					const uint32_t				newSize				= alloc_with_reserve(newCount, newData);
					rees_if(0 == newData);
					if(Data)
						memcpy(newData, Data, index * sizeof(T));
					newData[index]			= newValue;
					if(Data)
						memcpy(&newData[index + 1], &Data[index], (Count - index) * sizeof(T));
					T							* oldData			= Data;
					Data					= newData;
					Size					= newSize;
					::gpk::gpk_free(oldData);
				}
				else {
					for(int32_t i = (int)Count - 1; i >= (int)index; --i)
						Data[i + 1]				= Data[i];
					Data[index]				= newValue;
				}
				*(u1_t*)&Data[newCount]	= 0;
				return Count = newCount;
			}
			// returns the new size of the list or -1 on failure.
			::gpk::error_t			insert				(uint32_t index, const T* chainToInsert, uint32_t chainLength)	noexcept	{
				ree_if(index > Count, "%u > %u", index, Count);

				const uint32_t				newCount			= Count + chainLength;
				if(Size < newCount) {
					T							* newData			= 0;
					const uint32_t				newSize				= alloc_with_reserve(newCount, newData);
					rees_if(0 == newData);
					if(Data)
						memcpy(newData, Data, index * sizeof(T));
					memcpy(&newData[index], chainToInsert, chainLength * sizeof(T));
					if(Data)
						memcpy(&newData[index + chainLength], &Data[index], (Count - index) * sizeof(T));
					T							* oldData			= Data;
					Data					= newData;
					Size					= newSize;
					::gpk::gpk_free(oldData);
				}
				else {	// no need to reallocate and copy, just shift rightmost elements and insert in-place
					for(int32_t i = (int)Count - 1; i >= (int)index; --i)
						Data[i + chainLength]	= Data[i];
					memcpy(&Data[index], chainToInsert, chainLength * sizeof(T));
				}
				*(u1_t*)&Data[newCount]	= 0;
				return Count = newCount;
			}
			tplt<size_t _chainLength>
			inline	::gpk::error_t	insert				(uint32_t index, const T (&chainToInsert)[_chainLength])		noexcept	{ return insert(index, chainToInsert, (uint32_t)_chainLength); }
			inline	::gpk::error_t	insert				(uint32_t index, ::gpk::view<const T> chainToInsert)			noexcept	{ return insert(index, chainToInsert.begin(), chainToInsert.size()); }
			// Returns the new size of the list or -1 if the array pointer is not initialized.
			::gpk::error_t			remove_unordered	(uint32_t index)												noexcept	{
				ree_if(index >= Count, "%u >= %u.", index, Count);
				Data[index]				= Data[--Count];
				*(u1_t*)&Data[Count]		= 0;
				return Count;
			}
			// returns the new array size or -1 if failed.
			::gpk::error_t			erase				(const T* address)												noexcept	{
				const ptrdiff_t				ptrDiff				= ptrdiff_t(address) - (ptrdiff_t)Data;
				const uint32_t				index				= (uint32_t)(ptrDiff / (ptrdiff_t)sizeof(T));
				ree_if(index >= Count, "%u >= %u. p: 0x%p", index, Count, address);
				return remove(index);
			}
			// returns the new array size or -1 if failed.
			::gpk::error_t			remove				(uint32_t index)													noexcept	{
				ree_if(index >= Count, "%u >= %u.", index, Count);
				--Count;
				while(index < Count) {
					Data[index]				= Data[index + 1];
					++index;
				}
				*(u1_t*)&Data[Count]		= 0;
				return Count;
			}
		}; // array_pod

		tplt <tpnm T>	using apod		= ::gpk::array_pod<T>;
		tplt <tpnm T>	using ap		= ::gpk::apod	<T>;

		typedef apod<uc_t	>	auchar, auc;
		typedef apod<sc_t		>	achar , ac ;
		typedef apod<float		>	af32;
		typedef apod<double		>	af64;
		typedef apod<uint8_t	>	au8	;
		typedef apod<uint16_t	>	au16;
		typedef apod<uint32_t	>	au32;
		typedef apod<uint64_t	>	au64;
		typedef apod<int8_t		>	ai8	;
		typedef apod<int16_t	>	ai16;
		typedef apod<int32_t	>	ai32;
		typedef apod<int64_t	>	ai64;

		gpk::achar				toString	(const ::gpk::vcc & strToLog);
		stainli	gpk::achar		toString	(const ::gpk::vcu8 & strToLog) { return ::gpk::toString(*(const ::gpk::vcc*)&strToLog); }

		::gpk::error_t			camelCase		(::gpk::vcc input, ::gpk::achar & camelCased);

		::gpk::error_t			join			(::gpk::achar & query, sc_t separator, ::gpk::vcvcc fields);
	} // namespace

	#endif // GPK_ARRAY_POD_H_23627
