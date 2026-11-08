#include "gpk_view_bit.h"
#include "gpk_array.h"

#ifndef ARRAY_BIT_ARRAY_H_23627
#define ARRAY_BIT_ARRAY_H_23627

nmsp gpk
{
	tplt<tpnm _tType>
	struct array_bit {
		tydf	_tType			T;
		tydf	::gpk::vbit<T>	TView;

		::gpk::apod<T>			Data			;
		::gpk::view_bit<T>		View			;

		cxpr					array_bit		()					nxpt	= default;
								array_bit		(const ::gpk::view_bit<T> & other)
			:	Data(other.begin(), round_up(other.size(), TView::ELEMENT_BITS))
			,	View(Data.begin(), other.size())
			{}
								array_bit		(const ::gpk::array_bit<T> & other)
			: Data(other.Data)
			, View(Data.begin(), other.View.size())
			{}

		::gpk::array_bit<T>&	operator=			(const ::gpk::view_bit<T> & other) {
			Data					= {other.begin(), round_up(other.size(), TView::ELEMENT_BITS)};
			View					= {Data.begin(), other.size()};
			return *this;
		}

		::gpk::array_bit<T>&	operator=			(const ::gpk::array_bit<T> & other) {
			Data					= other.Data;
			View					= {Data.begin(), other.View.size()};
			return *this;
		}

		inxp	const u2_t&	size			()				csnx	{ return View.size(); }

		gpk::err_t 		resize			(u2_t newSize)		nxpt	{
			if_fail_fe(Data.resize(round_up(newSize, View.ELEMENT_BITS)));
			View					= {Data.begin(), newSize};
			return 0;
		}
	}; // struct

	tplt<tpnm T>	
	usng	abit	= ::gpk::array_bit<T>;

	tydf	abit<u3_t>	abitu3_t;
} // 

#endif // ARRAY_BIT_ARRAY_H_29037498023612