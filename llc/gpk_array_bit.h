#include "gpk_view_bit.h"
#include "gpk_array.h"

#ifndef GPK_ARRAY_BIT_H_23627
#define GPK_ARRAY_BIT_H_23627

namespace gpk
{
	tplt<tpnm _tType>
	struct array_bit {
		typedef	_tType			T;
		typedef	::gpk::vbit<T>	TView;

		::gpk::apod<T>			Data			;
		::gpk::view_bit<T>		View			;

		cnstxpr					array_bit		()					noexcept	= default;
								array_bit		(const ::gpk::view_bit<T> & other)
			:	Data(::gpk::view<cnst T>{other.begin().Begin, round_up(other.size(), TView::ELEMENT_BITS)})
			,	View(Data.begin(), other.size())
			{}
								array_bit		(const ::gpk::array_bit<T> & other)
			: Data(other.Data)
			, View(Data.begin(), other.View.size())
			{}

		::gpk::array_bit<T>&	operator=			(const ::gpk::view_bit<T> & other) {
			Data					= ::gpk::view<cnst T>{other.begin().Begin, round_up(other.size(), TView::ELEMENT_BITS)};
			View					= {Data.begin(), other.size()};
			return *this;
		}

		::gpk::array_bit<T>&	operator=			(const ::gpk::array_bit<T> & other) {
			Data					= other.Data;
			View					= {Data.begin(), other.View.size()};
			return *this;
		}

		inlcxpr	const u2_t&		size			()				const	noexcept	{ return View.size(); }

		::gpk::error_t			resize			(u2_t newSize)			noexcept	{
			gpk_necs(Data.resize(round_up(newSize, TView::ELEMENT_BITS)));
			View					= {Data.begin(), newSize};
			return 0;
		}
	}; // struct

	tplt<tpnm T>
	using	abit	= ::gpk::array_bit<T>;

	tydf	abit<u0_t>	abitu0_t;
	tydf	abit<u1_t>	abitu1_t;
	tydf	abit<u2_t>	abitu2_t;
	tydf	abit<u3_t>	abitu3_t;
} // namespace gpk

#endif // GPK_ARRAY_BIT_H_23627


