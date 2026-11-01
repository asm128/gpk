#include "gpk_log.h"
#include "gpk_functional.h"
#include "gpk_array_static.h"

#ifndef GPK_ARRAY_CIRCULAR_H
#define GPK_ARRAY_CIRCULAR_H

namespace gpk
{

	tplt<tpnm _tPOD, ::gpk::u2_t mAx>
	struct array_circular : private astatic<_tPOD, mAx> {
	protected:
		tydf	_tPOD			_t;
		::gpk::u2_t				Offset	= 0;
		::gpk::u2_t				Count	= 0;

		using astatic<_t, mAx>	::Storage;
		using astatic<_t, mAx>	::size;
		using astatic<_t, mAx>	::slice;
	public:
		tydf	_tPOD			T;
		tydf	astatic<T, mAx>	TAStatic;

		using TAStatic			::array_static;

		::gpk::err_t			next			(T & output)	{
			if_true_vw(-1, 0 == Count);
			output = Storage[Offset % mAx];
			if(size() == ++Offset)
				Offset = 0;
			return Count -= 1;
		}

		::gpk::err_t			push_back		(const T & output)	{
			if_true_fe(0 == free());
			Storage[(Offset + Count) % mAx] = output;
			return Count += 1;
		}

		template<::gpk::u2_t _nBatch>
		::gpk::err_t			append			(const T (&output)[_nBatch])	{
			if_true_fe(Count + _nBatch > free());
			Storage[(Offset + Count) % mAx] = output;
			return Count += 1;
		}

		::gpk::err_t			left			()	const	noexcept	{ return min(Count, size() - Offset); } // left side, first part
		::gpk::err_t			right			()	const	noexcept	{ return max((::gpk::err_t)0, (::gpk::err_t)Count - left()); } // right side, second part

		inline	::gpk::err_t	left			(::gpk::view<const T> & _left)	const	noexcept	{ return slice(_left , Offset, left()); }
		inline	::gpk::err_t	right			(::gpk::view<const T> & _right)	const	noexcept	{ return slice(_right, 0, right()); }

		::gpk::err_t			pair			(::gpk::view<const T> & _left, ::gpk::view<const T> & _right)	const	noexcept	{
			if_fail_fe(left(_left));
			if_fail_fe(right(_right));
			return _left.size() + _right.size(); 
		} 

		inxp	::gpk::err_t	free			()	const	noexcept	{ return size() - Count; }
		inxp	::gpk::err_t	used			()	const	noexcept	{ return Count; }
		inxp	::gpk::err_t	byte_count		()	const	noexcept	{ return Count * szof(T); }

//		::gpk::err_t			read			(const ::gpk::function<int()> & funcRemaining, const ::gpk::function<int(T *, ::gpk::u2_t)> & funcRead) {
//			return 0;	
//		}	
		::gpk::err_t			read			(const ::gpk::function<int()> & funcRemaining, const ::gpk::function<T()> & funcNext) {
			u2_c			stop			= mAx - Count;
			s2_t					readCount		= 0; 
			while(readCount < stop)
			switch(const err_t funcRemainingResult = funcRemaining()) {
			case 0:
				return readCount;
			default:
				if_fail_vef(-readCount, funcRemainingResult, "%" GPK_FMT_S2 ":%" GPK_FMT_S2, readCount, funcRemainingResult);
				if_fail_vef(-readCount, push_back(funcNext()), "%" GPK_FMT_S2, readCount);
				++readCount;
			}
			return readCount;
		}

		::gpk::err_t			read		(::gpk::u2_t countToRead, const ::gpk::function<int()> & funcRemaining, const ::gpk::function<T()> & funcNext) {
			u2_c			stop			= ::gpk::min(countToRead, mAx - Count);
			for(s2_t readCount = 0; readCount < stop; ++readCount) {
				if_true_vef(-readCount, 0 >= funcRemaining(), "%" GPK_FMT_S2 " < %" GPK_FMT_S2, readCount, countToRead);
				if_fail_vef(-readCount, push_back(funcNext()), "%" GPK_FMT_S2, readCount);
			}
			return stop;
		}
	};

	tplt<tpnm T, ::gpk::u2_t _maxCount> using acir			= ::gpk::array_circular<T, _maxCount>;
	tplt<tpnm T, ::gpk::u2_t _maxCount> using acircular	= ::gpk::acir<T, _maxCount>;
	
	tplt<::gpk::u2_t _maxCount>	using acirc		= ::gpk::acir<char     , _maxCount>; tplt<::gpk::u2_t _maxCount>	using acirchar	= ::gpk::acir<char    , _maxCount>;
	tplt<::gpk::u2_t _maxCount>	using aciruc 	= ::gpk::acir<uc_t  , _maxCount>; tplt<::gpk::u2_t _maxCount>	using aciruchar	= ::gpk::acir<uc_t , _maxCount>;
	tplt<::gpk::u2_t _maxCount>	using aciru8 	= ::gpk::acir<u0_t  , _maxCount>; 
	tplt<::gpk::u2_t _maxCount>	using aciru1_t	= ::gpk::acir<::gpk::u1_t , _maxCount>; 
	tplt<::gpk::u2_t _maxCount>	using aciru32	= ::gpk::acir<::gpk::u2_t , _maxCount>; 
	tplt<::gpk::u2_t _maxCount>	using aciru64	= ::gpk::acir<u3_t , _maxCount>; 
	tplt<::gpk::u2_t _maxCount>	using aciri8 	= ::gpk::acir<s0_t   , _maxCount>; 
	tplt<::gpk::u2_t _maxCount>	using aciri16	= ::gpk::acir<int16_t  , _maxCount>; 
	tplt<::gpk::u2_t _maxCount>	using aciri32	= ::gpk::acir<s2_t  , _maxCount>; 
	tplt<::gpk::u2_t _maxCount>	using aciri64	= ::gpk::acir<s3_t  , _maxCount>; 
	tplt<::gpk::u2_t _maxCount>	using acirf2_t	= ::gpk::acir<f2s_t, _maxCount>; 
	tplt<::gpk::u2_t _maxCount>	using acirf64	= ::gpk::acir<f3s_t, _maxCount>; 
} // namespace 

#endif // GPK_ARRAY_CIRCULAR_H
