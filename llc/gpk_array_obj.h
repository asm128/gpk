/// Copyright 2010-2024 - ogarnd
#include "gpk_array_base.h"

#include "gpk_keyval.h"

#ifndef GPK_ARRAY_OBJ_H_23627
#define GPK_ARRAY_OBJ_H_23627

namespace gpk
{
	tpl_tstct array_obj : public array_base<_t> {
		tdfTTCnst(_t);
		tydf	view<T>			TView					;
		tydf	array_obj<T>	TArray					;

		usng	array_base<T>		::Count					;
		usng	array_base<T>		::Data					;
		usng	array_base<T>		::Size					;
		usng	array_base<T>		::alloc_with_reserve	;

		inln					~array_obj			()			{ for(u2_t i = 0; i < Count; ++i) Data[i].~T(); }	// dtor

		inxp					array_obj			()			= default;
								array_obj			(::std::initializer_list<T> init)	{ if_fail_te(append(init.begin(), (u2_t)init.size())); }
		tpl_nu2					array_obj			(TCnst (&other)[_nu2])				{ if_fail_te(append(other, _nu2)); }
								array_obj			(cnst TArray & other)				: array_obj((cnst view<TCnst>&) other)	{}
								array_obj			(cnst view<TCnst> & other)			{
			u2_t					newCount			= other.size();
			if(newCount) {
				if_fail_tef(reserve(newCount), "Requested size: %" GPK_FMT_U2 ". ", (u2_t)newCount);
				for(; Count < newCount; ++Count)
					new (&Data[Count]) T(other.begin()[Count]);
			}
		}
								array_obj			(cnst view<T> & other)												{
			u2_t					newCount			= other.size();
			if(newCount) {
				if_fail_tef(reserve(newCount), "Requested size: %" GPK_FMT_U2 ". ", (u2_t)newCount);
				for(; Count < newCount; ++Count)
					new (&Data[Count]) T(other.begin()[Count]);
			}
		}
		inln					array_obj			(TArray&& other)											nxpt	{
			Size					= other.Size;
			Count					= other.Count;
			Data					= other.Data;

			other.Size = other.Count	= 0;
			other.Data				= 0;
		}	// move ctor
		inxp	oper			view<TCnst>			()								csnx	{ rtrn {Data, Count}; }
		inln	TArray&			oper=				(cnst TArray & other)					{
			if_true_te(resize(other.size()) != (s2_t)other.size());
			for(u2_t iElement = 0; iElement < Count; ++iElement)
				Data[iElement]			= other[iElement];
			rtrn *this;
		}
		err_t			clear				()												{ for(u2_t i = 0; i < Count; ++i) Data[i].~T(); rtrn Count = 0; }
		err_t			clear_pointer		()										nxpt	{ clear(); safe_gpk_free(Data); rtrn Size = 0; }
		err_t			reserve				(u2_t newCount)									{
			if(newCount > Size) {
				T							* newData			= 0;
				u2_c				newSize				= alloc_with_reserve(newCount, newData);
				ree_if(0 == newData, "newCount: %" GPK_FMT_S2 ", newSize: %" GPK_FMT_S2 "", newCount, newSize);
				for(u2_t iElement = 0; iElement < Count; ++iElement) {
					new (&newData[iElement]) T(Data[iElement]);
					Data[iElement].~T();
				}
				T						* oldData			= Data;
				Data					= newData;
				Size					= newSize;
				::gpk::gpk_free(oldData);
			}
			rtrn Size;
		}
		// Returns the new size of the array.
		err_t			resize				(u2_t newCount)															nxpt	{
			gpk_necs(reserve(newCount));
			for(; Count < newCount; ++Count)
				new (&Data[Count]) T{};
			for(; Count > newCount; --Count)
				Data[Count - 1].~T();
			rtrn Count;
		}
		// Returns the new size of the array.
		err_t			resize				(u2_t newCount, TCnst & newValue)									nxpt	{
			gpk_necs(reserve(newCount));
			for(; Count < newCount; ++Count)
				new (&Data[Count]) T{newValue};
			for(; Count > newCount; --Count)
				Data[Count - 1].~T();
			rtrn Count;
		}
		// Returns the new size of the array.
		tpl_vtArgs	err_t	resize				(u2_t newCount, _tArgs&&... constructorArgs)											{
			gpk_necs(reserve(newCount));
			for(; Count < newCount; ++Count)
				new (&Data[Count]) T{constructorArgs...};
			for(; Count > newCount; --Count)
				Data[Count - 1].~T();
			rtrn Count;
		}
		// Returns the new size of the array
		inln	err_t	pop_back			()											nxpt	{
			ree_if(0 == Count, "%s", "Cannot pop elements of an empty array.");
			Data[--Count].~T();
			rtrn Count;
		}
		// Returns the new size of the array
		inln	err_t	pop_back			(T & oldValue)								nxpt	{
			ree_if(0 == Count, "%s", "Cannot pop elements of an empty array.");
			oldValue				= Data[--Count];
			Data[Count].~T();
			rtrn Count;
		}
		// Returns the index of the pushed value or -1 on failure
		err_t			push_back			(TCnst & newValue)														nxpt	{
			cnst s2_t				oldSize				= Count;
			gpk_necs(resize(oldSize + 1, newValue));
			rtrn oldSize;
		}
		// returns the new array size or -1 if failed.
		inln	err_t	erase				(TCnst * address)																	{
			ree_if(0 == Data, "Uninitialized array pointer! Invalid address to erase: %p.", address);
			cnst ptrdiff_t		ptrDiff				= ptrdiff_t(address) - (ptrdiff_t)Data;
			u2_c				index				= (u2_t)(ptrDiff / (ptrdiff_t)szof(T));
			ree_if(index >= Count, GPK_FMT_GE_U2, index, Count);
			rtrn remove(index);
		}
		err_t			remove				(u2_t index)																		{
			ree_if(0 == Data, "Uninitialized array pointer! Invalid index to erase: %" GPK_FMT_U2 ".", index);
			ree_if(index >= Count, GPK_FMT_GE_U2, index, Count);
			--Count;
			while(index < Count) {
				Data[index].~T();							// Destroy old
				new (&Data[index]) T(Data[index + 1]);		// Placement new
				++index;
			}
			Data[index].~T();							// Destroy last
			rtrn Count;
		}
		// Returns the new size of the list or -1 if the array pointer is not initialized.
		err_t			remove_unordered	(u2_t index)																		{
			ree_if(0 == Data, "Uninitialized array pointer! Invalid index to erase: %" GPK_FMT_U2 ".", index);
			ree_if(index >= Count, GPK_FMT_GE_U2, index, Count);
			Data[index].~T();							// Destroy old
			if(1 == Count || index == (Count - 1))
				--Count;
			else {
				new (&Data[index]) T(Data[--Count]);	// Placement new
				Data[Count].~T();						// Destroy reordered
			}

			rtrn Count;
		}
		tpl_nu2	inln	err_t	append				(TCnst (&newChain)[_nu2])						nxpt	{ rtrn append(newChain, _nu2); }
				inln	err_t	append				(cnst view<TCnst> & newChain)					nxpt	{ rtrn append(newChain.begin(), newChain.size());	}
						err_t	append				(TCnst * chainToAppend, u2_t chainLength)		nxpt	{
			if(0 == chainLength)
				rtrn Count;

			gpk_necs(reserve(Count + chainLength));
			u2_c				iFirst				= Count;
			for(u2_t i = 0; i < chainLength; ++i)
				new (&Data[Count++]) T{chainToAppend[i]};
			rtrn iFirst;
		}
		// returns the new size of the list or -1 on failure.
		tpl_nu2	inln	err_t	insert				(u2_t index, TCnst* (&chainToInsert)[_nu2])	nxpt	{ rtrn insert(index, chainToInsert, (u2_t)_nu2); }
		inln			err_t	insert				(u2_t index, view<TCnst> chainToInsert)		nxpt	{ rtrn insert(index, chainToInsert.begin(), chainToInsert.size()); }
						err_t	insert				(u2_t index, TCnst & newValue)										nxpt	{
			ree_if(index > Count, "Invalid index: %" GPK_FMT_U2 ".", index);
			u2_c			newCount			= Count + 1;
			if(Size < newCount) {
				T				* newData			= 0;
				u2_c			newSize				= alloc_with_reserve(newCount, newData);
				rees_if(0 == newData);
				for(u2_t i = 0; i < index; ++i) {
					new (&newData[i]) T(Data[i]);
					Data[i].~T();
				}
				new (&newData[index]) T(newValue);
				for(u2_t i = index; i < Count; ++i) {
					new (&newData[i + 1]) T(Data[i]);
					Data[i].~T();
				}
				T				* oldData			= Data;
				Data		= newData;
				Size		= newSize;
				::gpk::gpk_free(oldData);
			}
			else {
				for(s2_t i = (int)Count - 1; i >= (int)index; --i) {
					new (&Data[i + 1]) T(Data[i]);
					Data[i].~T();
				}
				new (&Data[index]) T(newValue);
			}
			rtrn Count = newCount;
		}
		// returns the new size of the list or -1 on failure.
		err_t			insert				(u2_t index, TCnst * chainToInsert, u2_t chainLength)			nxpt	{
			ree_if(index > Count, "Invalid index: %" GPK_FMT_U2 ".", index);

			u2_c				newCount			= Count + chainLength;
			if(Size < newCount) {
				T					* newData			= 0;
				u2_c				newSize				= alloc_with_reserve(newCount, newData);
				rees_if(0 == newData);
				for(u2_t i = 0; i < index; ++i) {
					new (&newData[i]) T(Data[i]);
					Data[i].~T();
				}

				for(u2_t i = 0; i < chainLength; ++i) 
					new (&newData[index + i]) T(chainToInsert[i]);

				for(u2_t i = index; i < Count; ++i) {
					new (&newData[i + chainLength]) T(Data[i]);
					Data[i].~T();
				}
				T					* oldData			= Data;
				Data			= newData;
				Size			= newSize;
				::gpk::gpk_free(oldData);
			}
			else {	// no need to reallocate and copy, just shift rightmost elements and insert in-place
				for(s2_t i = (int)Count - 1; i >= (int)index; --i) {
					new (&Data[i + chainLength]) T(Data[i]);
					Data[i].~T();
				}

				for(u2_t i = 0; i < chainLength; ++i) 
					new (&Data[index + i]) T(chainToInsert[i]);
			}
			rtrn Count = newCount;
		}

	}; // array_obj

	tplT	using aobj		= ::gpk::array_obj<T>; 
	tplT	using ao		= ::gpk::aobj<T>; 

	tplT	using aview		= ::gpk::aobj	<::gpk::view<T>>; 
	tplT	using av		= ::gpk::aview	<T>; 

	tydf	aview<uc_t>	avuc_t;
	tydf	aview<sc_t>	avsc_t;
	tydf	aview<f2_t>	avf2_t;
	tydf	aview<f3_t>	avf3_t;
	tydf	aview<u0_t>	avu0_t;
	tydf	aview<u1_t>	avu1_t;
	tydf	aview<u2_t>	avu2_t;
	tydf	aview<u3_t>	avu3_t;
	tydf	aview<s0_t>	avs0_t;
	tydf	aview<s1_t>	avs1_t;
	tydf	aview<s2_t>	avs2_t;
	tydf	aview<s3_t>	avs3_t;
	tdcs	avuc_t	avuc_c;
	tdcs	avsc_t	avsc_c;
	tdcs	avf2_t	avf2_c;
	tdcs	avf3_t	avf3_c;
	tdcs	avu0_t	avu0_c;
	tdcs	avu1_t	avu1_c;
	tdcs	avu2_t	avu2_c;
	tdcs	avu3_t	avu3_c;
	tdcs	avs0_t	avs0_c;
	tdcs	avs1_t	avs1_c;
	tdcs	avs2_t	avs2_c;
	tdcs	avs3_t	avs3_c;
	
	tydf	aview<uc_c>	avcuc_t;
	tydf	aview<sc_c>	avcsc_t;
	tydf	aview<f2_c>	avcf2_t;
	tydf	aview<f3_c>	avcf3_t;
	tydf	aview<u0_c>	avcu0_t;
	tydf	aview<u1_c>	avcu1_t;
	tydf	aview<u2_c>	avcu2_t;
	tydf	aview<u3_c>	avcu3_t;
	tydf	aview<s0_c>	avcs0_t;
	tydf	aview<s1_c>	avcs1_t;
	tydf	aview<s2_c>	avcs2_t;
	tydf	aview<s3_c>	avcs3_t;
	tdcs	avcuc_t	avcuc_c;
	tdcs	avcsc_t	avcsc_c;
	tdcs	avcf2_t	avcf2_c;
	tdcs	avcf3_t	avcf3_c;
	tdcs	avcu0_t	avcu0_c;
	tdcs	avcu1_t	avcu1_c;
	tdcs	avcu2_t	avcu2_c;
	tdcs	avcu3_t	avcu3_c;
	tdcs	avcs0_t	avcs0_c;
	tdcs	avcs1_t	avcs1_c;
	tdcs	avcs2_t	avcs2_c;
	tdcs	avcs3_t	avcs3_c;


	tplT	err_t							split					(cnst ::gpk::view<cnst T> & target, cnst T & separator, ::gpk::aobj<::gpk::view<cnst T>> & split)	{
		u2_t									lastOffset				= 0;
		for(u2_t iChar = 0; iChar < target.size(); ++iChar) {
			if(target[iChar] == separator) {
				cnst ::gpk::view<cnst T>			newView					= {&target[lastOffset], iChar - lastOffset};
				++iChar;
				gpk_necs(split.push_back(newView));
				lastOffset								= iChar;
			}
		}
		if(lastOffset < target.size())
			gpk_necs(split.push_back({&target[lastOffset], target.size() - lastOffset}));
		rtrn (s2_t)split.size();
	}

	tplT	err_t							split					(cnst ::gpk::view<cnst T> & target, cnst ::gpk::view<cnst T>& separators, ::gpk::aobj<::gpk::view<cnst T>> & split)	{
		u2_t									lastOffset				= 0;
		for(u2_t iChar = 0; iChar < target.size(); ++iChar) {
			for(u2_t iSeparator = 0; iSeparator < separators.size(); ++iSeparator) {
				if(target[iChar] == separators[iSeparator]) {
					cnst ::gpk::view<cnst T>			newView					= {&target[lastOffset], iChar - lastOffset};
					++iChar;
					gpk_necs(split.push_back(newView));
					lastOffset								= iChar;
				}
			}
		}
		if(lastOffset < target.size())
			gpk_necs(split.push_back({&target[lastOffset], target.size() - lastOffset}));
		rtrn (s2_t)split.size();
	}

	tplT	err_t							split					(cnst ::gpk::vcst_t & target, cnst T & separator, ::gpk::aobj<::gpk::vcst_t> & split)	{
		s2_t										lastOffset				= 0;
		for(s2_t iChar = 0, countChars = target.size(); iChar < countChars; ++iChar) {
			if(target[iChar] == separator) {
				cnst ::gpk::vcst_t							newView					= {&target[lastOffset], (u2_t)::gpk::max((s2_t)0, s2_t(iChar - lastOffset))};
				++iChar;
				gpk_necs(split.push_back(newView));
				lastOffset								= iChar;
			}
		}
		if(lastOffset < (s2_t)target.size()) {
			gpk_necs(split.push_back({&target[lastOffset], target.size() - lastOffset}));
			//if(split[split.size()-1][] == separator)
		}
		rtrn (s2_t)split.size();
	}



	tydf ::gpk::SKeyVal<::gpk::vcst_t, ::gpk::aobj<::gpk::vcst_t>>	TKeyValConstStringArray;
	//------------------------------------------------------------------------------------------------------------
	err_t							keyValConstStringDeserialize	(vcu0_c & input, ::gpk::aobj<::gpk::TKeyValConstChar> & output);

	err_t							filterPrefix					(::gpk::view<::gpk::vcsc_c> input, ::gpk::vcsc_c prefix, ::gpk::aobj<::gpk::vcsc_t> & filtered, bool nullIncluded = false);
	err_t							filterPostfix					(::gpk::view<::gpk::vcsc_c> input, ::gpk::vcsc_c prefix, ::gpk::aobj<::gpk::vcsc_t> & filtered, bool nullIncluded = false);
}
#endif // GPK_ARRAY_OBJ_H_23627
