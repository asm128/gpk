#include "gpk_log.h"
#include "gpk_functional.h"
#include "gpk_array_static.h"

#ifndef GPK_ARRAY_CIRCULAR_H
#define GPK_ARRAY_CIRCULAR_H

nmsp gpk
{

	tplt<tpnm _tPOD, u2_t mAx>
	struct array_circular : private astatic<_tPOD, mAx> {
	protected:
		tydf	_tPOD			_t;
		u2_t				Offset	= 0;
		u2_t				Count	= 0;

		usng astatic<_t, mAx>	::Storage;
		usng astatic<_t, mAx>	::size;
		usng astatic<_t, mAx>	::slice;
	public:
		tydf	_tPOD			T;
		tydf	astatic<T, mAx>	TAStatic;

		usng TAStatic			::array_static;

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

		tplt<u2_t _nBatch>
		err_t 		append			(const T (&output)[_nBatch])	{
			if_true_fe(Count + _nBatch > free());
			Storage[(Offset + Count) % mAx] = output;
			return Count += 1;
		}

		err_t 		left			()	csnx	{ return min(Count, size() - Offset); } // left side, first part
		err_t 		right			()	csnx	{ return max((err_t)0, (err_t)Count - left()); } // right side, second part

		inln	err_t left			(view<const T> & _left)	csnx	{ return slice(_left , Offset, left()); }
		inln	err_t right			(view<const T> & _right)	csnx	{ return slice(_right, 0, right()); }

		err_t 		pair			(view<const T> & _left, view<const T> & _right)	csnx	{
			if_fail_fe(left(_left));
			if_fail_fe(right(_right));
			return _left.size() + _right.size(); 
		} 

		inxp	err_t free			()	csnx	{ return size() - Count; }
		inxp	err_t used			()	csnx	{ return Count; }
		inxp	err_t byte_count	()	csnx	{ return Count * szof(T); }

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

	tplt<tpnm T, u2_t _maxCount> usng acir			= array_circular<T, _maxCount>;
	tplt<tpnm T, u2_t _maxCount> usng acircular	= acir<T, _maxCount>;
	
	tplt<u2_t _maxCount>	usng acirsc_t	= acir<sc_t, _maxCount>;
	tplt<u2_t _maxCount>	usng aciruc_t	= acir<uc_t, _maxCount>;
	tplt<u2_t _maxCount>	usng aciru0_t 	= acir<u0_t, _maxCount>;
	tplt<u2_t _maxCount>	usng aciru1_t	= acir<u1_t, _maxCount>;
	tplt<u2_t _maxCount>	usng aciru2_t	= acir<u2_t, _maxCount>;
	tplt<u2_t _maxCount>	usng aciru3_t	= acir<u3_t, _maxCount>;
	tplt<u2_t _maxCount>	usng acirs0_t	= acir<s0_t, _maxCount>;
	tplt<u2_t _maxCount>	usng acirs1_t	= acir<s1_t, _maxCount>;
	tplt<u2_t _maxCount>	usng acirs2_t	= acir<s2_t, _maxCount>;
	tplt<u2_t _maxCount>	usng acirs3_t	= acir<s3_t, _maxCount>;
	tplt<u2_t _maxCount>	usng acirf2_t	= acir<f2_t, _maxCount>;
	tplt<u2_t _maxCount>	usng acirf3_t	= acir<f3_t, _maxCount>;
} // nmsp 

#endif // GPK_ARRAY_CIRCULAR_H
