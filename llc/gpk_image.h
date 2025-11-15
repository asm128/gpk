#include "gpk_array_pod.h"
#include "gpk_grid.h"
#include "gpk_view_bit.h"
#include "gpk_grid_scale.h"

#ifndef GPK_IMAGE_H_23627
#define GPK_IMAGE_H_23627

namespace gpk
{
	tplt<tpnm _tCell>
	struct img {
		tydf	_tCell				T;
		tydf	cnst _tCell		TConst;
		tydf	::gpk::view<T>		TView	;
		tydf	::gpk::grid<T>		TGrid	;
		tydf	::gpk::view<TConst>	TViewConst	;

		::gpk::apod<T>				Texels		;
		::gpk::grid<T>				View		;

		cxpr						img			()								= default;
									img			(const ::gpk::grid<T> & other)	: Texels(other)			{ View = {Texels.begin(), other		.metrics()}; }
									img			(const img<T> & other)			: Texels(other.Texels)	{ View = {Texels.begin(), other.View.metrics()}; }

		inxp	oper				grid<const T>	()		const	noexcept	{ return View; }
		inln	oper				grid<T>			()				noexcept	{ return View; }

		img&						operator=	(const ::gpk::grid<T> & other) {
			Texels						= v1<const T>{other.begin(), other.size()};
			View						= {Texels.begin(), other.metrics()};
			return *this;
		}

		img&						operator=	(const img<T> & other) {
			Texels						= other.Texels;
			View						= {Texels.begin(), other.View.metrics()};
			return *this;
		}

		inln	TView				operator[]	(uint32_t index)							{ return View[index]; }
		inln	TViewConst			operator[]	(uint32_t index)		cnst				{ return View[index]; }

		inxp	const T*			begin		()						csnx		{ return View.begin		();	}
		inxp	const T*			end			()						csnx		{ return View.end		();	}
		inxp	T*					begin		()						nxpt		{ return View.begin		();	}
		inxp	T*					end			()						nxpt		{ return View.end		();	}
		inxp	::gpk::n2u2_c&		metrics		()						csnx		{ return View.metrics	();	}
		inxp	u2_c&				size		()						csnx		{ return Texels.size	();	}
		inxp	u2_t				area		()						csnx		{ return View.area		();	}
		inxp	u2_t 				byte_count	()						csnx		{ return Texels.byte_count();	}

		inln	::gpk::error_t		resize		(::gpk::n2s0_c & newSize)	nxpt	{ return resize(newSize.u2_t()); }
		inln	::gpk::error_t		resize		(::gpk::n2s1_c & newSize)	nxpt	{ return resize(newSize.u2_t()); }
		inln	::gpk::error_t		resize		(::gpk::n2s2_c & newSize)	nxpt	{ return resize(newSize.u2_t()); }
		inln	::gpk::error_t		resize		(::gpk::n2u0_c & newSize)	nxpt	{ return resize(newSize.u2_t()); }
		inln	::gpk::error_t		resize		(::gpk::n2u1_c & newSize)	nxpt	{ return resize(newSize.u2_t()); }
		inln	::gpk::error_t		resize		(::gpk::n2u2_c & newSize)	nxpt	{ 
			gpk_necall(Texels.resize(newSize.x * newSize.y), "cannot resize? Requested size: %u.", newSize.x * newSize.y);
			View						= {Texels.begin(), newSize.x, newSize.y};
			return 0;
		}

		inln	::gpk::error_t		resize		(::gpk::n2s0_c & newSize, const T & newValue)	nxpt	{ return resize(newSize.u2_t(), newValue); }
		inln	::gpk::error_t		resize		(::gpk::n2s1_c & newSize, const T & newValue)	nxpt	{ return resize(newSize.u2_t(), newValue); }
		inln	::gpk::error_t		resize		(::gpk::n2s2_c & newSize, const T & newValue)	nxpt	{ return resize(newSize.u2_t(), newValue); }
		inln	::gpk::error_t		resize		(::gpk::n2u0_c & newSize, const T & newValue)	nxpt	{ return resize(newSize.u2_t(), newValue); }
		inln	::gpk::error_t		resize		(::gpk::n2u1_c & newSize, const T & newValue)	nxpt	{ return resize(newSize.u2_t(), newValue); }
		inln	::gpk::error_t		resize		(::gpk::n2u2_c & newSize, const T & newValue)	nxpt	{ 
			gpk_necall(Texels.resize(newSize.x * newSize.y), "cannot resize? Requested size: %u.", newSize.x * newSize.y);
			View						= {Texels.begin(), newSize.x, newSize.y};
			gpk_necall(gpk::drawRectangle(View, newValue, ::gpk::rect2<int32_t>{{}, View.metrics().s2_t()}), "Unknown error setting value to rectangle of size: %u x %u.", newSize.x, newSize.y);
			return Texels.size();
		}
	}; // struct

	tydf	img<s0_t>	imgi8;
	tydf	img<s1_t>	imgi16;
	tydf	img<s2_t>	imgi32;
	tydf	img<s3_t>	imgi64;
	tydf	img<u0_t>	imgu8;
	tydf	img<u1_t>	imgu16;
	tydf	img<u2_t>	imgu32;
	tydf	img<u3_t>	imgu64;
	tydf	img<f2_t>	imgf32;
	tydf	img<f3_t>	imgf64;

	tplt<tpnm _tType>
	struct imgmono {
		tydf	_tType				T;
		tydf	::gpk::view_bit<T>	TView;

		::gpk::apod<T>				Texels				;
		::gpk::view_bit<T>			View				;
		uint32_t					Pitch				= 0;

		cxpr				imgmono	()		noexcept	= default;
							imgmono	(const ::gpk::view_bit<T> & other)
			:	Texels	(other.begin(), other.size() / TView::ELEMENT_BITS + one_if(other.size() % TView::ELEMENT_BITS))
			,	View	(Texels.begin(), other.size())
			,	Pitch	(other.Pitch)
			{}
							imgmono	(const ::gpk::imgmono<T> & other)
			: Texels	(other.Texels)
			, View		(Texels.begin(), other.View.size())
			, Pitch		(other.Pitch)
			{}

		::gpk::imgmono<T>&	operator=			(const ::gpk::view_bit<T> & other) {
			Texels						= {other.begin(), other.size() / TView::ELEMENT_BITS + one_if(other.size() % TView::ELEMENT_BITS)};
			View						= {Texels.begin(), other.size()};
			Pitch						= other.Pitch;
			return *this;
		}
		::gpk::imgmono<T>&	operator=			(const ::gpk::imgmono<T> & other) {
			Texels						= other.Texels;
			View						= {Texels.begin(), other.View.size()};
			Pitch						= other.Pitch;
			return *this;
		}

		inxp	::gpk::n2u2_c&	metrics				()									const	noexcept	{ return {Pitch, View.size() / Pitch}; }
		inln	::gpk::error_t	resize				(const ::gpk::n2u2_c & newSize)				noexcept	{ return resize(newSize.x, newSize.y); }
		::gpk::error_t			resize				(uint32_t newSizeX, uint32_t newSizeY)		noexcept	{
			gpk_necall(Texels.resize((newSizeX * (int64_t)newSizeY) / View.ELEMENT_BITS + 1), "cannot resize? Size requested: %u.", (uint32_t)((newSizeX * (int64_t)newSizeY) / View.ELEMENT_BITS + 1));
			View						= {Texels.begin(), newSizeX * newSizeY};
			Pitch						= newSizeX;
			return 0;
		}
	}; // struct

	tydf	imgmono<uint64_t>	imgmonou64;

	tplt<tpnm _tColor, tpnm _tDepthStencil>
	struct rt {
		tydf _tColor						TColor;
		tydf _tDepthStencil				TDepth;

		tydf ::gpk::view<TColor>			VColor;
		tydf ::gpk::view<const TColor>	VCColor;

		::gpk::img<TColor>		Color			= {};
		::gpk::img<TDepth>		DepthStencil	= {};

		inln	VColor			operator[]		(u2_t index)							{ return Color[index]; }
		inln	VCColor			operator[]		(u2_t index)		const				{ return Color[index]; }

		inxp	const TColor*	begin			()					const	noexcept	{ return Color.begin	();	}
		inxp	const TColor*	end				()					const	noexcept	{ return Color.end		();	}
		inxp	TColor*			begin			()							noexcept	{ return Color.begin	();	}
		inxp	TColor*			end				()							noexcept	{ return Color.end		();	}
		inxp	::gpk::n2u2_c&	metrics			()					const	noexcept	{ return Color.metrics	();	}
		inxp	u2_c&			size			()					const	noexcept	{ return Color.size		();	}
		inxp	u2_t			area			()					const	noexcept	{ return Color.area		();	}

		inln	::gpk::error_t	resize			(::gpk::n2u0_c & newSize)												noexcept	{ return resize(newSize.u2_t()); }
		inln	::gpk::error_t	resize			(::gpk::n2u1_c & newSize)												noexcept	{ return resize(newSize.u2_t()); }
		inln	::gpk::error_t	resize			(::gpk::n2u0_c  & newSize, const TColor & color, const TDepth & depth)	noexcept	{ return resize(newSize.u2_t(), color, depth); }
		inln	::gpk::error_t	resize			(::gpk::n2u1_c & newSize, const TColor & color, const TDepth & depth)	noexcept	{ return resize(newSize.u2_t(), color, depth); }
		::gpk::error_t			resize			(::gpk::n2u2_c & newSize)	noexcept	{
			gpk_necs(Color			.resize(newSize));
			gpk_necs(DepthStencil	.resize(newSize));
			return 0;
		}
		::gpk::error_t			resize			(::gpk::n2u2_c & newSize, const TColor & color, const TDepth & depth)	noexcept	{
			gpk_necs(Color			.resize(newSize, color));
			gpk_necs(DepthStencil	.resize(newSize, depth));
			return 0;
		}
	};

	tplt<tpnm TColor, tpnm TDepth>
	using	SRenderTarget	= rt<TColor, TDepth>;

	tplt<tpnm TColor, tpnm TDepth>
	::gpk::error_t				clearTarget			(::gpk::rt<TColor, TDepth> & targetToClear)		{
		::gpk::img<TColor>				& offscreen			= targetToClear.Color;
		::gpk::img<TDepth>				& offscreenDepth	= targetToClear.DepthStencil;
		::memset(offscreenDepth	.Texels.begin(), -1, sizeof(TDepth)	* offscreenDepth.Texels.size());	// Clear target.
		::memset(offscreen		.Texels.begin(),  0, sizeof(TColor)	* offscreen		.Texels.size());	// Clear target.
		return 0;
	}
	tplt<tpnm TColor>
	::gpk::error_t				updateSizeDependentTarget	(::gpk::apod<TColor> & out_colors, ::gpk::grid<TColor> & out_view, ::gpk::n2u1_c & newSize)											{
		gpk_necs(out_colors.resize(newSize.x * newSize.y));		// Update size-dependent resources.
		if( out_view.metrics().u1_t() != newSize)
			out_view					= {out_colors.begin(), newSize.u2_t() };
		return 0;
	}
	tplt<tpnm TColor>
	::gpk::error_t				updateSizeDependentTarget	(::gpk::apod<TColor> & out_colors, ::gpk::grid<TColor> & out_view, ::gpk::n2u1_c & newSize, const TColor & newValue) {
		gpk_necs(out_colors.resize(newSize.x * newSize.y, newValue));		// Update size-dependent resources.
		if( out_view.metrics().u1_t() != newSize)
			out_view					= {out_colors.begin(), newSize};
		return 0;
	}

	tplt<tpnm TColor>
	::gpk::error_t				updateSizeDependentImage	(::gpk::apod<TColor> & out_scaled, ::gpk::grid<TColor> & out_view, const ::gpk::grid<TColor> & in_view, ::gpk::n2u1_c & newSize) {
		gpk_necs(out_scaled.resize(newSize.x * newSize.y));		// Update size-dependent resources.
		if( out_view.metrics().u1_t() != newSize) {
			out_view					= {out_scaled.begin(), newSize.x, newSize.y};
			if(in_view.size())
				es_if(failed(::gpk::grid_scale(out_view, in_view)));
		}
		return 0;
	}

	tplt<tpnm TColor>
	stin	::gpk::err_t	updateSizeDependentTarget	(::gpk::img<TColor> & out_texture, ::gpk::n2u1_c & newSize) {
		return updateSizeDependentTarget(out_texture.Texels, out_texture.View, newSize);
	}
	tplt<tpnm TColor>
	stin	::gpk::err_t	updateSizeDependentTarget	(::gpk::img<TColor> & out_texture, ::gpk::n2u1_c & newSize, const TColor & newValue) {
		return updateSizeDependentTarget(out_texture.Texels, out_texture.View, newSize, newValue);
	}
	tplt<tpnm TColor>
	stin	::gpk::err_t	updateSizeDependentImage	(::gpk::img<TColor> & out_texture, const ::gpk::grid<TColor> & in_view, ::gpk::n2u1_c & newSize) {
		return updateSizeDependentImage(out_texture.Texels, out_texture.View, in_view, newSize);
	}

} // namespace

#endif // GPK_IMAGE_H_23627
