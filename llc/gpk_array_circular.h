#include "gpk_log.h"
#include "gpk_functional.h"
#include "gpk_array_static.h"

#ifndef GPK_ARRAY_CIRCULAR_H
#define GPK_ARRAY_CIRCULAR_H

namespace gpk
{

	tplt<tpnm _tPOD, u2_t mAx>
	struct array_circular : private astatic<_tPOD, mAx> {
	protected:
		tydf	_tPOD			_t;
		u2_t				Offset	= 0;
		u2_t				Count	= 0;

		using astatic<_t, mAx>	::Storage;
		using astatic<_t, mAx>	::size;
		using astatic<_t, mAx>	::slice;
	public:
		tydf	_tPOD			T;
		tydf	astatic<T, mAx>	TAStatic;

		using TAStatic			::array_static;

		err_t 		next			(T & output)	{
			if_true_vw(-1, 0 == Count);
			output = Storage[Offset % mAx];
			if(size() == ++Offset)
				Offset = 0;
			return Count -= 1;
		}

		err_t 		push_back		(const T & output)	{
			if_true_fe(0 == free());
			Storage[(Offset + Count) % mAx] = output;
			return Count += 1;
		}

		template<u2_t _nBatch>
		err_t 		append			(const T (&output)[_nBatch])	{
			if_true_fe(Count + _nBatch > free());
			Storage[(Offset + Count) % mAx] = output;
			return Count += 1;
		}

		err_t 		left			()	const	noexcept	{ return min(Count, size() - Offset); } // left side, first part
		err_t 		right			()	const	noexcept	{ return max((err_t)0, (err_t)Count - left()); } // right side, second part

		inln	err_t left			(view<const T> & _left)	const	noexcept	{ return slice(_left , Offset, left()); }
		inln	err_t right			(view<const T> & _right)	const	noexcept	{ return slice(_right, 0, right()); }

		err_t 		pair			(view<const T> & _left, view<const T> & _right)	const	noexcept	{
			if_fail_fe(left(_left));
			if_fail_fe(right(_right));
			return _left.size() + _right.size(); 
		} 

		inxp	err_t free			()	const	noexcept	{ return size() - Count; }
		inxp	err_t used			()	const	noexcept	{ return Count; }
		inxp	err_t byte_count	()	const	noexcept	{ return Count * szof(T); }

//		err_t 		read			(const function<int()> & funcRemaining, const function<int(T *, u2_t)> & funcRead) {
//			return 0;	
//		}	
		err_t 		read			(const function<int()> & funcRemaining, const function<T()> & funcNext) {
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

		err_t 		read		(u2_t countToRead, const function<int()> & funcRemaining, const function<T()> & funcNext) {
			u2_c			stop			= min(countToRead, mAx - Count);
			for(s2_t readCount = 0; readCount < stop; ++readCount) {
				if_true_vef(-readCount, 0 >= funcRemaining(), "%" GPK_FMT_S2 " < %" GPK_FMT_S2, readCount, countToRead);
				if_fail_vef(-readCount, push_back(funcNext()), "%" GPK_FMT_S2, readCount);
			}
			return stop;
		}
	};

	tplt<tpnm T, u2_t _maxCount> using acir			= array_circular<T, _maxCount>;
	tplt<tpnm T, u2_t _maxCount> using acircular	= acir<T, _maxCount>;
	
	tplt<u2_t _maxCount>	using acirsc_t	= acir<sc_t, _maxCount>;
	tplt<u2_t _maxCount>	using aciruc_t	= acir<uc_t, _maxCount>;
	tplt<u2_t _maxCount>	using aciru0_t 	= acir<u0_t, _maxCount>;
	tplt<u2_t _maxCount>	using aciru1_t	= acir<u1_t, _maxCount>;
	tplt<u2_t _maxCount>	using aciru2_t	= acir<u2_t, _maxCount>;
	tplt<u2_t _maxCount>	using aciru3_t	= acir<u3_t, _maxCount>;
	tplt<u2_t _maxCount>	using acirs0_t	= acir<s0_t, _maxCount>;
	tplt<u2_t _maxCount>	using acirs1_t	= acir<s1_t, _maxCount>;
	tplt<u2_t _maxCount>	using acirs2_t	= acir<s2_t, _maxCount>;
	tplt<u2_t _maxCount>	using acirs3_t	= acir<s3_t, _maxCount>;
	tplt<u2_t _maxCount>	using acirf2_t	= acir<f2_t, _maxCount>;
	tplt<u2_t _maxCount>	using acirf3_t	= acir<f3_t, _maxCount>;
} // namespace 

#endif // GPK_ARRAY_CIRCULAR_H
