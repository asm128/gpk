#include "gpk_array_pod.h"
#include "gpk_grid.h"
#include "gpk_view_bit.h"

#ifndef GPK_IMAGE_H_23627
#define GPK_IMAGE_H_23627

namespace gpk
{
	tplt<tpnm TCell>
	stct img {
		tydf TCell			T;
		tydf cnst TCell		TConst;
		tydf view<T>		TView;
		tydf view<TConst>	TViewConst;
		tydf grid<T>		TGrid;

		apod<T>				Texels		= {};
		grid<T>				View		= {};

		inxp				img			() = default;
							img			(cnst grid<T> & other) : Texels(view<cnst T>{other.begin(), other.size()}) {
			View = {Texels.begin(), other.metrics()};
		}
							img			(cnst img<T> & other) : Texels(other.Texels) {
			View = {Texels.begin(), other.View.metrics()};
		}

		inxp	oper		grid<cnst T>	() cnst nxpt { rtrn View; }
		inln	oper		grid<T>			()		nxpt { rtrn View; }

		img&				oper=			(cnst grid<T> & other) {
			Texels = view<cnst T>{other.begin(), other.size()};
			View = {Texels.begin(), other.metrics()};
			rtrn *this;
		}
		img&				oper=			(cnst img<T> & other) {
			Texels = other.Texels;
			View = {Texels.begin(), other.View.metrics()};
			rtrn *this;
		}

		inln	TView		oper[]			(u2_t row)				{ rtrn View[row]; }
		inln	TViewConst	oper[]			(u2_t row) cnst			{ rtrn View[row]; }
		inxp	cnst T*		begin			()			cnst nxpt	{ rtrn View.begin(); }
		inxp	cnst T*		end				()			cnst nxpt	{ rtrn View.end(); }
		inxp	T*			begin			()			nxpt		{ rtrn View.begin(); }
		inxp	T*			end				()			nxpt		{ rtrn View.end(); }
		inxp	n2u2_c&		metrics			()			cnst nxpt	{ rtrn View.metrics(); }
		inxp	u2_c&		size			()			cnst nxpt	{ rtrn Texels.size(); }
		inxp	u2_t		area			()			cnst nxpt	{ rtrn View.area(); }
		inxp	u2_t		byte_count		()			cnst nxpt	{ rtrn Texels.byte_count(); }

		inln	err_t		resize			(n2s0_c & newSize) nxpt { rtrn resize(newSize.u2()); }
		inln	err_t		resize			(n2s1_c & newSize) nxpt { rtrn resize(newSize.u2()); }
		inln	err_t		resize			(n2s2_c & newSize) nxpt { rtrn resize(newSize.u2()); }
		inln	err_t		resize			(n2u0_c & newSize) nxpt { rtrn resize(newSize.u2()); }
		inln	err_t		resize			(n2u1_c & newSize) nxpt { rtrn resize(newSize.u2()); }
		err_t				resize			(n2u2_c & newSize) nxpt {
			if_fail_fef(Texels.resize(newSize.x * newSize.y), "Cannot resize image to %u x %u.", newSize.x, newSize.y);
			View = {Texels.begin(), newSize.x, newSize.y};
			rtrn 0;
		}

		inln	err_t		resize			(n2s0_c & newSize, cnst T & value) nxpt { rtrn resize(newSize.u2(), value); }
		inln	err_t		resize			(n2s1_c & newSize, cnst T & value) nxpt { rtrn resize(newSize.u2(), value); }
		inln	err_t		resize			(n2s2_c & newSize, cnst T & value) nxpt { rtrn resize(newSize.u2(), value); }
		inln	err_t		resize			(n2u0_c & newSize, cnst T & value) nxpt { rtrn resize(newSize.u2(), value); }
		inln	err_t		resize			(n2u1_c & newSize, cnst T & value) nxpt { rtrn resize(newSize.u2(), value); }
		err_t				resize			(n2u2_c & newSize, cnst T & value) nxpt {
			if_fail_fef(Texels.resize(newSize.x * newSize.y), "Cannot resize image to %u x %u.", newSize.x, newSize.y);
			View = {Texels.begin(), newSize.x, newSize.y};
			if_fail_fe(View.fill(value));
			rtrn Texels.size();
		}
	};

	tplt<tpnm TCell>
	stct imgmono {
		tydf TCell			T;
		tydf view_bit<T>	TView;

		apod<T>			Texels	= {};
		view_bit<T>		View	= {};
		u2_t			Pitch	= 0;

		inxp			imgmono	() = default;
						imgmono	(cnst imgmono<T> & other) : Texels(other.Texels), View(Texels.begin(), other.View.size()), Pitch(other.Pitch) {}

		imgmono&		oper=	(cnst imgmono<T> & other) {
			Texels	= other.Texels;
			View	= {Texels.begin(), other.View.size()};
			Pitch	= other.Pitch;
			rtrn *this;
		}

		inxp	n2u2_t	metrics		() cnst nxpt	{ rtrn {Pitch, Pitch ? View.size() / Pitch : 0}; }
		inxp	u2_c&	size		() cnst nxpt	{ rtrn View.size(); }
		inxp	u2_t	byte_count	() cnst nxpt	{ rtrn Texels.byte_count(); }

		inln	err_t	resize		(n2u2_c & newSize) nxpt { rtrn resize(newSize.x, newSize.y); }
		err_t		resize		(u2_t newSizeX, u2_t newSizeY) nxpt {
			u3_c bitCount = u3_t(newSizeX) * newSizeY;
			if_true_fef(bitCount > 0xFFFFFFFFULL, "Monochrome image dimensions exceed the supported bit count: %u x %u.", newSizeX, newSizeY);
			u2_c cellCount = u2_t(bitCount / TView::ELEMENT_BITS + one_if(bitCount % TView::ELEMENT_BITS));
			if_fail_fef(Texels.resize(cellCount), "Cannot resize monochrome image to %u x %u.", newSizeX, newSizeY);
			View	= {Texels.begin(), u2_t(bitCount)};
			Pitch	= newSizeX;
			rtrn 0;
		}
	};

	tplt<tpnm TColor, tpnm TDepth>
	stct rt {
		tydf TColor				TColorCell;
		tydf TDepth				TDepthCell;
		tydf view<TColor>		TViewColor;
		tydf view<cnst TColor>	TViewColorConst;

		img<TColor>		Color			= {};
		img<TDepth>		DepthStencil	= {};

		inln	TViewColor		oper[]		(u2_t row)		{ rtrn Color[row]; }
		inln	TViewColorConst	oper[]		(u2_t row) cnst	{ rtrn Color[row]; }
		inxp	cnst TColor*	begin		() cnst nxpt	{ rtrn Color.begin(); }
		inxp	cnst TColor*	end			() cnst nxpt	{ rtrn Color.end(); }
		inxp	TColor*			begin		() nxpt			{ rtrn Color.begin(); }
		inxp	TColor*			end			() nxpt			{ rtrn Color.end(); }
		inxp	n2u2_c&		metrics		() cnst nxpt	{ rtrn Color.metrics(); }
		inxp	u2_c&			size		() cnst nxpt	{ rtrn Color.size(); }
		inxp	u2_t			area		() cnst nxpt	{ rtrn Color.area(); }

		inln	err_t			resize		(n2s0_c & newSize) nxpt { rtrn resize(newSize.u2()); }
		inln	err_t			resize		(n2s1_c & newSize) nxpt { rtrn resize(newSize.u2()); }
		inln	err_t			resize		(n2s2_c & newSize) nxpt { rtrn resize(newSize.u2()); }
		inln	err_t			resize		(n2u0_c & newSize) nxpt { rtrn resize(newSize.u2()); }
		inln	err_t			resize		(n2u1_c & newSize) nxpt { rtrn resize(newSize.u2()); }
		err_t				resize		(n2u2_c & newSize) nxpt {
			if_fail_fe(Color.resize(newSize));
			if_fail_fe(DepthStencil.resize(newSize));
			rtrn 0;
		}

		inln	err_t			resize		(n2s0_c & newSize, cnst TColor & color, cnst TDepth & depth) nxpt { rtrn resize(newSize.u2(), color, depth); }
		inln	err_t			resize		(n2s1_c & newSize, cnst TColor & color, cnst TDepth & depth) nxpt { rtrn resize(newSize.u2(), color, depth); }
		inln	err_t			resize		(n2s2_c & newSize, cnst TColor & color, cnst TDepth & depth) nxpt { rtrn resize(newSize.u2(), color, depth); }
		inln	err_t			resize		(n2u0_c & newSize, cnst TColor & color, cnst TDepth & depth) nxpt { rtrn resize(newSize.u2(), color, depth); }
		inln	err_t			resize		(n2u1_c & newSize, cnst TColor & color, cnst TDepth & depth) nxpt { rtrn resize(newSize.u2(), color, depth); }
		err_t				resize		(n2u2_c & newSize, cnst TColor & color, cnst TDepth & depth) nxpt {
			if_fail_fe(Color.resize(newSize, color));
			if_fail_fe(DepthStencil.resize(newSize, depth));
			rtrn 0;
		}
	};

	tydf img<u0_t>	imgu0_t;
	tydf img<u1_t>	imgu1_t;
	tydf img<u2_t>	imgu2_t;
	tydf img<u3_t>	imgu3_t;
	tydf img<s0_t>	imgs0_t;
	tydf img<s1_t>	imgs1_t;
	tydf img<s2_t>	imgs2_t;
	tydf img<s3_t>	imgs3_t;
	tydf img<f2_t>	imgf2_t;
	tydf img<f3_t>	imgf3_t;
	tydf imgmono<u3_t>	imgmonou3_t;
	tydf imgmonou3_t	imgmonou64;
} // namespace gpk

#endif // GPK_IMAGE_H_23627


