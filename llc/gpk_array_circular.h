#include "gpk_log.h"
#include "gpk_functional.h"
#include "gpk_array_static.h"

#ifndef GPK_ARRAY_CIRCULAR_H
#define GPK_ARRAY_CIRCULAR_H

namespace gpk
{

	tplt<tpnm _tPOD, uint32_t mAx>
	struct array_circular : private astatic<_tPOD, mAx> {
	protected:
		typedef	_tPOD			_t;
		uint32_t				Offset	= 0;
		uint32_t				Count	= 0;

		using astatic<_t, mAx>	::Storage;
		using astatic<_t, mAx>	::size;
		using astatic<_t, mAx>	::slice;
	public:
		typedef	_tPOD			T;
		typedef	astatic<T, mAx>	TAStatic;

		using TAStatic			::array_static;

		::gpk::error_t			next			(T & output)	{
			rews_if(0 == Count);
			output = Storage[Offset % mAx];
			if(size() == ++Offset)
				Offset = 0;
			return Count -= 1;
		}

		::gpk::error_t			push_back		(const T & output)	{
			rees_if(0 == free());
			Storage[(Offset + Count) % mAx] = output;
			return Count += 1;
		}

		template<uint32_t _nBatch>
		::gpk::error_t			append			(const T (&output)[_nBatch])	{
			rees_if(Count + _nBatch > free());
			Storage[(Offset + Count) % mAx] = output;
			return Count += 1;
		}

		::gpk::error_t			left			()	const	noexcept	{ return min(Count, size() - Offset); } // left side, first part
		::gpk::error_t			right			()	const	noexcept	{ return max((::gpk::error_t)0, (::gpk::error_t)Count - left()); } // right side, second part

		inline	::gpk::error_t	left			(::gpk::view<const T> & _left)	const	noexcept	{ slice(_left , Offset, left()); }
		inline	::gpk::error_t	right			(::gpk::view<const T> & _right)	const	noexcept	{ slice(_right, 0, right()); }

		::gpk::error_t			pair			(::gpk::view<const T> & _left, ::gpk::view<const T> & _right)	const	noexcept	{
			gpk_necs(left(_left));
			gpk_necs(right(_right));
			return _left.size() + _right.size(); 
		} 

		inlcxpr	::gpk::error_t	free			()	const	noexcept	{ return size() - Count; }
		inlcxpr	::gpk::error_t	used			()	const	noexcept	{ return Count; }
		inlcxpr	::gpk::error_t	byte_count		()	const	noexcept	{ return Count * sizeof(T); }

//		::gpk::error_t			read			(const ::gpk::function<int()> & funcRemaining, const ::gpk::function<int(T *, uint32_t)> & funcRead) {
//			return 0;	
//		}	
		::gpk::error_t			read			(const ::gpk::function<int()> & funcRemaining, const ::gpk::function<T()> & funcNext) {
			const uint32_t			stop			= mAx - Count;
			int32_t					readCount		= 0; 
			while(readCount < stop)
			switch(const error_t funcRemainingResult = funcRemaining()) {
			case 0:
				return readCount;
			default:
				rve_if_failed(-readCount, funcRemainingResult, "%" GPK_FMT_I32 ":%" GPK_FMT_I32, readCount, funcRemainingResult);
				rve_if_failed(-readCount, push_back(funcNext()), "%" GPK_FMT_I32, readCount);
				++readCount;
			}
			return readCount;
		}

		::gpk::error_t			read		(uint32_t countToRead, const ::gpk::function<int()> & funcRemaining, const ::gpk::function<T()> & funcNext) {
			const uint32_t			stop			= ::gpk::min(countToRead, mAx - Count);
			for(int32_t readCount = 0; readCount < stop; ++readCount) {
				rve_if(-readCount, 0 >= funcRemaining(), "%" GPK_FMT_I32 " < %" GPK_FMT_I32, readCount, countToRead);
				rve_if_failed(-readCount, push_back(funcNext()), "%" GPK_FMT_I32, readCount);
			}
			return stop;
		}
	};

	tplt<tpnm T, uint32_t _maxCount> using acir			= ::gpk::array_circular<T, _maxCount>;
	tplt<tpnm T, uint32_t _maxCount> using acircular	= ::gpk::acir<T, _maxCount>;
	
	tplt<uint32_t _maxCount>	using acirc		= ::gpk::acir<char     , _maxCount>; tplt<uint32_t _maxCount>	using acirchar	= ::gpk::acir<char    , _maxCount>;
	tplt<uint32_t _maxCount>	using aciruc 	= ::gpk::acir<uchar_t  , _maxCount>; tplt<uint32_t _maxCount>	using aciruchar	= ::gpk::acir<uchar_t , _maxCount>;
	tplt<uint32_t _maxCount>	using aciru8 	= ::gpk::acir<uint8_t  , _maxCount>; 
	tplt<uint32_t _maxCount>	using aciru16	= ::gpk::acir<uint16_t , _maxCount>; 
	tplt<uint32_t _maxCount>	using aciru32	= ::gpk::acir<uint32_t , _maxCount>; 
	tplt<uint32_t _maxCount>	using aciru64	= ::gpk::acir<uint64_t , _maxCount>; 
	tplt<uint32_t _maxCount>	using aciri8 	= ::gpk::acir<int8_t   , _maxCount>; 
	tplt<uint32_t _maxCount>	using aciri16	= ::gpk::acir<int16_t  , _maxCount>; 
	tplt<uint32_t _maxCount>	using aciri32	= ::gpk::acir<int32_t  , _maxCount>; 
	tplt<uint32_t _maxCount>	using aciri64	= ::gpk::acir<int64_t  , _maxCount>; 
	tplt<uint32_t _maxCount>	using acirf32	= ::gpk::acir<float32_t, _maxCount>; 
	tplt<uint32_t _maxCount>	using acirf64	= ::gpk::acir<float64_t, _maxCount>; 
} // namespace 

#endif // GPK_ARRAY_CIRCULAR_H
