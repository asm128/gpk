#include "gpk_array_base.h"

#include "gpk_view_serialize.h"

#ifndef GPK_ARRAY_POD_H_23627
#define GPK_ARRAY_POD_H_23627

namespace gpk
{
	// This class is optimized to contain POD instances and won't work for C++ objects that require calling constructors/destructors.
	tpl_tstct array_pod : public array_base<_t> {
		tdfTTCnst(_t);
		tydf	view<T>			TView				;
		tydf	array_pod<T>	TArray				;

		usng array_base<T>	::Count					;
		usng array_base<T>	::Data					;
		usng array_base<T>	::Size					;
		usng array_base<T>	::alloc_with_reserve	;
		usng array_base<T>	::find					;

	private:
		err_t				source_offset		(cnst T * source, u2_t count, u2_t & offset)cnst noexcept	{
			offset					= (u2_t)-1;
			if(0 == Data || 0 == source)
				return 0;
			u3_c				dataAddress			= (u3_t)Data;
			u3_c				sourceAddress		= (u3_t)source;
			u3_c				endAddress			= dataAddress + (u3_t)Count * szof(T);
			if(sourceAddress < dataAddress || sourceAddress > endAddress)
				return 0;
			u3_c				byteOffset			= sourceAddress - dataAddress;
			ree_if(byteOffset % szof(T), "Misaligned source address: 0x%p.", source);
			offset					= (u2_t)(byteOffset / szof(T));
			ree_if(count > Count - offset, GPK_FMT_GT_U2, count, Count - offset);
			return 1;
		}

	public:

		inxp				array_pod			()										noexcept	= default;
							array_pod			(::std::initializer_list<T> init)					{
			if_fail_te(resize((u2_t)init.size()));
			memcpy(Data, init.begin(), Count * szof(T));
		}
							array_pod			(array_pod<T> && other)					noexcept	{
			Size				= other.Size;
			Count				= other.Count;
			Data				= other.Data;

			other.Size			= other.Count			= 0;
			other.Data			= 0;
		}
		inline				array_pod			(cnst array_pod<T> & other)							: array_pod((cnst view<cnst T>&) other) {}
							array_pod			(cnst view<cnst T> & other)							{
			if_true_te(resize(other.size()) != (s2_t)other.size());
			memcpy(Data, other.begin(), other.byte_count());
		}
		tplt<size_t _count>
							array_pod			(cnst T (&other)[_count])							{
			if_true_te(resize(_count) != (s2_t)_count);
			memcpy(Data, other, _count * szof(T));
		}
		inxp	oper		view<cnst T>	()									cnst	noexcept	{ return {Data, Count}; }
		TArray&				oper =			(cnst array_pod<T>& other)								{ return oper=((cnst view<T> &) other); }
		TArray&				oper =			(cnst view<cnst T> & other)								{
			u2_t					sourceOffset		= 0;
			cnst err_t			aliased			= source_offset(other.begin(), other.size(), sourceOffset);
			gsthrow_if(aliased < 0);
			if(aliased) {
				memmove(Data, &Data[sourceOffset], other.byte_count());
				Count					= other.size();
				Data[Count]				= {};
				return *this;
			}
			gsthrow_if(resize(other.size()) != (s2_t)other.size());
			if(Count)
				memcpy(Data, other.begin(), other.byte_count());
			return *this;
		}
		tplN0u	TArray&		oper =			(cnst T (&init)[N])										{
			gsthrow_if(resize(N) != N);
			memcpy(Data, init, Count * szof(T));
			Data[Count]				= {};
			return *this;
		}

		inln	err_t		clear				()										noexcept	{ Count = 0; if(Data) Data[Count] = {}; return Count; }
		err_t				clear_pointer		()										noexcept	{ safe_gpk_free(Data); Data = 0; return Size = Count = 0; }
		// Returns the new size of the array
		err_t				pop_back			()										noexcept	{
			rees_if(0 == Count);
			Data[--Count]				= {};
			return Count;
		}
		// Returns the new size of the array
		err_t				pop_back			(T & oldValue)							noexcept	{
			rees_if(0 == Count);
			oldValue				= Data[--Count];
			Data[Count]				= {};
			return Count;
		}
		// Returns the index of the pushed value or -1 on failure
		err_t				push_back			(cnst T & newValue)						noexcept	{
			cnst s2_t				oldSize				= Count;
			gpk_necs(resize(oldSize + 1, newValue));
			return oldSize;
		}
		inln		err_t	append_string		(cnst gpk::function<err_t(TArray&)> & funcAppend)	noexcept	{ return funcAppend ? funcAppend(*this) : 0; }
		
		tplN2uinln	err_t	append_string		(cnst T (&newChain)[N])								noexcept	{ return append_string(::gpk::vcst_t{newChain}); }
		inln		err_t	append_string		(cnst ::gpk::vcst_t & newChain)						noexcept	{ gpk_necs(append(newChain.begin(), newChain.size())); return newChain.size(); }
		inln		err_t	append_string		(cnst T & element)									noexcept	{ gpk_necs(push_back(element)); return 1; }
		inln		err_t	append_strings		(cnst ::gpk::view<cnst ::gpk::vcst_t> & newChains)	noexcept	{ 
			s2_t					appended				= 0;
			for(u2_t i = 0, stop = newChains.size(); i < stop; ++i) {
				cnst ::gpk::vcst_t & newChain = newChains[i];
				gpk_necs(append(newChain));
				appended += newChain.size();
			}
			return appended;
		}
		tplN2uinln	err_t	append				(cnst T (&newChain)[N])										noexcept	{ return append(newChain, (u2_t)N); }
		inln		err_t	append				(cnst ::gpk::view<cnst T> & newChain)						noexcept	{ return append(newChain.begin(), newChain.size());	}
		inln		err_t	append				(cnst ::gpk::view<cnst ::gpk::view<cnst T>> & newChains)	noexcept	{ 
			s2_t					count				= 0;
			for(u2_t i = 0, stop = newChains.size(); i < stop; ++i) {
				cnst ::gpk::view<cnst T> & newChain		= newChains[i];
				gpk_necs(append(newChain));
				count					+= newChain.size();
			}
			return count;
		}
		err_t				append				(cnst T * chainToAppend, u2_t chainLength)					noexcept	{
			if(0 == chainLength)
				return Count;
			if_null_fe(chainToAppend);
			u2_t					sourceOffset		= 0;
			cnst err_t			aliased			= source_offset(chainToAppend, chainLength, sourceOffset);
			gpk_necs(aliased);
			u2_c				newCount			= Count + chainLength;
			gpk_necs(reserve(newCount));
			if(aliased)
				chainToAppend			= &Data[sourceOffset];
			u2_c				iFirst				= Count;
			memmove(&Data[iFirst], chainToAppend, szof(T) * chainLength);
			Count					+= chainLength;
			Data[Count]				= {};
			return iFirst;
		}
		err_t				reserve				(u2_t newCount)										{
			if(newCount > Size) {
				T							* newData			= 0;
				u2_c				newSize				= alloc_with_reserve(newCount, newData);
				rees_if(0 == newData);
				if(Data)
					memcpy(newData, Data, this->byte_count());
					
				newData[Count]			= {};
				T							* oldData			= Data;
				Data					= newData;
				Size					= newSize;
				::gpk::gpk_free(oldData);
			}
			return Size;
		}
		inln	err_t		resize_bits			(u2_t newBitCount)						noexcept	{
			stxp u2_t					elementBits			= (u2_t)szof(T) * 8U;
			return resize(newBitCount / elementBits + one_if(newBitCount % elementBits));
		}
		// Returns the new size of the array.
		err_t				resize				(u2_t newCount)							noexcept	{
			gpk_necs(reserve(newCount));
			Count					= newCount;
			if(Data)
				Data[Count]				= {};
			return Count;
		}
		// Returns the new size of the array.
		err_t				resize				(u2_t newCount, cnst T & newValue)		noexcept	{
			cnst T				valueCopy			= newValue;
			gpk_necs(reserve(newCount));
			if(Data) {
				for(; Count < newCount; ++Count)
					Data[Count] = valueCopy;
				Data[Count = newCount]	= {};
			}
			return Count;
		}
		// returns the new size of the list or -1 on failure.
		err_t				insert				(u2_t index, cnst T & newValue)	noexcept	{
			ree_if(index > Count, GPK_FMT_GT_U2, index, Count);
			cnst T				valueCopy			= newValue;
			u2_c				newCount			= Count + 1;
			if(Size < newCount) {
				T							* newData			= 0;
				u2_c				newSize				= alloc_with_reserve(newCount, newData);
				rees_if(0 == newData);
				if(Data)
					memcpy(newData, Data, index * szof(T));
				newData[index]			= valueCopy;
				if(Data)
					memcpy(&newData[index + 1], &Data[index], (Count - index) * szof(T));
				T							* oldData			= Data;
				Data					= newData;
				Size					= newSize;
				::gpk::gpk_free(oldData);
			}
			else {
				for(s2_t i = (int)Count - 1; i >= (int)index; --i)
					Data[i + 1]				= Data[i];
				Data[index]				= valueCopy;
			}
			Data[newCount]			= {};
			return Count = newCount;
		}
		// returns the new size of the list or -1 on failure.
		err_t				insert				(u2_t index, cnst T * chainToInsert, u2_t chainLength)	noexcept	{
			ree_if(index > Count, GPK_FMT_GT_U2, index, Count);
			if(0 == chainLength)
				return Count;
			if_null_fe(chainToInsert);
			u2_t					sourceOffset		= 0;
			cnst err_t			aliased			= source_offset(chainToInsert, chainLength, sourceOffset);
			gpk_necs(aliased);

			u2_c				newCount			= Count + chainLength;
			if(Size < newCount) {
				T							* newData			= 0;
				u2_c				newSize				= alloc_with_reserve(newCount, newData);
				rees_if(0 == newData);
				if(Data)
					memcpy(newData, Data, index * szof(T));
				memcpy(&newData[index], chainToInsert, chainLength * szof(T));
				if(Data)
					memcpy(&newData[index + chainLength], &Data[index], (Count - index) * szof(T));
				T							* oldData			= Data;
				Data					= newData;
				Size					= newSize;
				::gpk::gpk_free(oldData);
			}
			else {	// no need to reallocate and copy, just shift rightmost elements and insert in-place
				memmove(&Data[index + chainLength], &Data[index], (Count - index) * szof(T));
				if(aliased) {
					if(sourceOffset >= index)
						chainToInsert			= &Data[sourceOffset + chainLength];
					else if(sourceOffset + chainLength > index) {
						u2_c				prefixCount			= index - sourceOffset;
						memmove(&Data[index], &Data[sourceOffset], prefixCount * szof(T));
						memmove(&Data[index + prefixCount], &Data[index + chainLength], (chainLength - prefixCount) * szof(T));
						chainToInsert			= 0;
					}
				}
				if(chainToInsert)
					memmove(&Data[index], chainToInsert, chainLength * szof(T));
			}
			Data[newCount]			= {};
			return Count = newCount;
		}
		tplt<size_t _chainLength>
		inln	err_t		insert				(u2_t index, cnst T (&chainToInsert)[_chainLength])		noexcept	{ return insert(index, chainToInsert, (u2_t)_chainLength); }
		inln	err_t		insert				(u2_t index, ::gpk::view<cnst T> chainToInsert)			noexcept	{ return insert(index, chainToInsert.begin(), chainToInsert.size()); }
		// Returns the new size of the list or -1 if the array pointer is not initialized.
		err_t				remove_unordered	(u2_t index)											noexcept	{
			ree_if(index >= Count, GPK_FMT_GE_U2, index, Count);
			Data[index]				= Data[--Count];
			Data[Count]				= {};
			return Count;
		}
		// returns the new array size or -1 if failed.
		err_t				erase				(cnst T * address)										noexcept	{
			u2_t					index				= 0;
			ree_if(1 != source_offset(address, 1, index), "Address outside the array range: 0x%p.", address);
			return remove(index);
		}
		// returns the new array size or -1 if failed.
		err_t				remove				(u2_t index)											noexcept	{
			ree_if(index >= Count, GPK_FMT_GE_U2, index, Count);
			--Count;
			while(index < Count) {
				Data[index]				= Data[index + 1];
				++index;
			}
			Data[Count]				= {};
			return Count;
		}
	}; // array_pod

	tplTusng	apod		= ::gpk::array_pod<T>;
	tplTusng	ap			= ::gpk::apod	<T>;

	tydf		apod<b8_t>	ab8_t;	tdcs	ab8_t	ab8_c;
	tydf		apod<uc_t>	auc_t;	tdcs	auc_t	auc_c;
	tydf		apod<sc_t>	asc_t;	tdcs	asc_t	asc_c;
	tydf		apod<u0_t>	au0_t;	tdcs	au0_t	au0_c;
	tydf		apod<u1_t>	au1_t;	tdcs	au1_t	au1_c;
	tydf		apod<u2_t>	au2_t;	tdcs	au2_t	au2_c;
	tydf		apod<u3_t>	au3_t;	tdcs	au3_t	au3_c;
	tydf		apod<s0_t>	as0_t;	tdcs	as0_t	as0_c;
	tydf		apod<s1_t>	as1_t;	tdcs	as1_t	as1_c;
	tydf		apod<s2_t>	as2_t;	tdcs	as2_t	as2_c;
	tydf		apod<s3_t>	as3_t;	tdcs	as3_t	as3_c;
	tydf		apod<f2_t>	af2_t;	tdcs	af2_t	af2_c;
	tydf		apod<f3_t>	af3_t;	tdcs	af3_t	af3_c;

	stin	vs		str		(asc_t & arg)	{ rtrn {arg.begin(), arg.size()}; }
	stin	vcst_t	str		(asc_c & arg)	{ rtrn {arg.begin(), arg.size()}; }

	asc_t		toString	(::gpk::vcsc_c & strToLog);
	err_t		camelCase	(::gpk::vcsc_t input, ::gpk::asc_t & camelCased);
	err_t		join		(::gpk::asc_t & query, char separator, ::gpk::vcvsc_t fields);
} // namespace

#define GPK_USING_APOD()				\
	usng	::gpk::ab8_t, ::gpk::ab8_c	\
		,	::gpk::auc_t, ::gpk::auc_c	\
		,	::gpk::asc_t, ::gpk::asc_c	\
		,	::gpk::au0_t, ::gpk::au0_c	\
		,	::gpk::au1_t, ::gpk::au1_c	\
		,	::gpk::au2_t, ::gpk::au2_c	\
		,	::gpk::au3_t, ::gpk::au3_c	\
		,	::gpk::as0_t, ::gpk::as0_c	\
		,	::gpk::as1_t, ::gpk::as1_c	\
		,	::gpk::as2_t, ::gpk::as2_c	\
		,	::gpk::as3_t, ::gpk::as3_c	\
		,	::gpk::af2_t, ::gpk::af2_c	\
		,	::gpk::af3_t, ::gpk::af3_c

#endif // GPK_ARRAY_POD_H_23627
