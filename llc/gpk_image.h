#include "gpk_array.h"
#include "gpk_view_grid.h"
#include "gpk_view_bit.h"
#include "gpk_grid_scale.h"

#ifndef GPK_IMAGE_H_902387498237
#define GPK_IMAGE_H_902387498237

namespace gpk
{
	template<typename _tTexel>
	struct SImage {
		typedef									_tTexel						TTexel;

		::gpk::apod<_tTexel>					Texels						;
		::gpk::v2<_tTexel>						View						;

		constexpr								SImage						()													= default;
												SImage						(const ::gpk::v2<_tTexel>& other)					: Texels(other)			{ View = {Texels.begin(), other		.metrics()}; }
												SImage						(const SImage<_tTexel>& other)						: Texels(other.Texels)	{ View = {Texels.begin(), other.View.metrics()}; }

		inlcxpr	operator						v2<const _tTexel>			()								const	noexcept	{ return View; }
		inline	operator						v2<_tTexel>					()										noexcept	{ return View; }

		SImage&									operator=					(const ::gpk::v2<_tTexel>& other)					{
			Texels									= v1<const _tTexel>{other.begin(), other.size()};
			View									= {Texels.begin(), other.metrics()};
			return *this;
		}

		SImage&									operator=					(const SImage<_tTexel>& other)						{
			Texels									= other.Texels;
			View									= {Texels.begin(), other.View.metrics()};
			return *this;
		}


		inline	::gpk::v1<_tTexel>				operator[]					(uint32_t index)														{ return View[index]; }
		inline	const ::gpk::v1<const _tTexel>	operator[]					(uint32_t index)									const				{ return View[index]; }

		inlcxpr	const _tTexel*					begin						()													const	noexcept	{ return View.begin		();	}
		inlcxpr	const _tTexel*					end							()													const	noexcept	{ return View.end		();	}
		inlcxpr	_tTexel*						begin						()															noexcept	{ return View.begin		();	}
		inlcxpr	_tTexel*						end							()															noexcept	{ return View.end		();	}
		inlcxpr	const ::gpk::n2<uint32_t> &		metrics						()													const	noexcept	{ return View.metrics	();	}
		inlcxpr	const uint32_t &				size						()													const	noexcept	{ return Texels.size	();	}
		inlcxpr	uint32_t						area						()													const	noexcept	{ return View.area		();	}
		inlcxpr	uint32_t 						byte_count					()													const	noexcept	{ return Texels.byte_count();	}

		inline	::gpk::error_t					resize						(const ::gpk::n2i8 & newSize)								noexcept	{ return resize(newSize.Cast<uint32_t>()); }
		inline	::gpk::error_t					resize						(const ::gpk::n2i16 & newSize)								noexcept	{ return resize(newSize.Cast<uint32_t>()); }
		inline	::gpk::error_t					resize						(const ::gpk::n2i32 & newSize)								noexcept	{ return resize(newSize.Cast<uint32_t>()); }
		inline	::gpk::error_t					resize						(const ::gpk::n2u8  & newSize)								noexcept	{ return resize(newSize.Cast<uint32_t>()); }
		inline	::gpk::error_t					resize						(const ::gpk::n2u16 & newSize)								noexcept	{ return resize(newSize.Cast<uint32_t>()); }
		inline	::gpk::error_t					resize						(const ::gpk::n2u32 & newSize)								noexcept	{ 
			gpk_necall(Texels.resize(newSize.x * newSize.y), "cannot resize? Requested size: %u.", newSize.x * newSize.y);
			View									= {Texels.begin(), newSize.x, newSize.y};
			return 0;
		}

		inline	::gpk::error_t					resize						(const ::gpk::n2i8  & newSize, const _tTexel & newValue)	noexcept	{ return resize(newSize.Cast<uint32_t>(), newValue); }
		inline	::gpk::error_t					resize						(const ::gpk::n2i16 & newSize, const _tTexel & newValue)	noexcept	{ return resize(newSize.Cast<uint32_t>(), newValue); }
		inline	::gpk::error_t					resize						(const ::gpk::n2i32 & newSize, const _tTexel & newValue)	noexcept	{ return resize(newSize.Cast<uint32_t>(), newValue); }
		inline	::gpk::error_t					resize						(const ::gpk::n2u8  & newSize, const _tTexel & newValue)	noexcept	{ return resize(newSize.Cast<uint32_t>(), newValue); }
		inline	::gpk::error_t					resize						(const ::gpk::n2u16 & newSize, const _tTexel & newValue)	noexcept	{ return resize(newSize.Cast<uint32_t>(), newValue); }
		inline	::gpk::error_t					resize						(const ::gpk::n2u32 & newSize, const _tTexel & newValue)	noexcept	{ 
			gpk_necall(Texels.resize(newSize.x * newSize.y), "cannot resize? Requested size: %u.", newSize.x * newSize.y);
			View									= {Texels.begin(), newSize.x, newSize.y};
			gpk_necall(::gpk::drawRectangle(View, newValue, ::gpk::SRectangle2<int32_t>{{}, View.metrics().template Cast<int32_t>()}), "Unknown error setting value to rectangle of size: %u x %u.", newSize.x, newSize.y);
			return Texels.size();
		}
	}; // struct

	template<typename Val>	using img		= ::gpk::SImage<Val>;

	typedef	img<int8_t >		imgi8;
	typedef	img<int16_t>		imgi16;
	typedef	img<int32_t>		imgi32;
	typedef	img<int64_t>		imgi64;
	typedef	img<uint8_t>		imgu8;
	typedef	img<uint16_t>		imgu16;
	typedef	img<uint32_t>		imgu32;
	typedef	img<uint64_t>		imgu64;

	typedef	img<::gpk::bgr	>	img8bgr	;
	typedef	img<::gpk::rgb	>	img8rgb	;
	typedef	img<::gpk::bgra>	img8bgra;
	typedef	img<::gpk::rgba>	img8rgba;
	typedef	img<::gpk::rgbaf>	imgfrgba;

	template<typename _tTexel>
	struct SImageMonochrome {
		typedef	_tTexel							TTexel;
		typedef	::gpk::view_bit<_tTexel>		TView;

		::gpk::apod<_tTexel>					Texels				;
		::gpk::view_bit<_tTexel>				View				;
		uint32_t								Pitch				= 0;

		cnstxpr									SImageMonochrome	()													noexcept	= default;
												SImageMonochrome	(const ::gpk::view_bit<_tTexel>& other)
			:	Texels	(other.begin(), other.size() / TView::ELEMENT_BITS + one_if(other.size() % TView::ELEMENT_BITS))
			,	View	(Texels.begin(), other.size())
			,	Pitch	(other.Pitch)
			{}
												SImageMonochrome	(const ::gpk::SImageMonochrome<_tTexel>& other)
			: Texels	(other.Texels)
			, View		(Texels.begin(), other.View.size())
			, Pitch		(other.Pitch)
			{}
		::gpk::SImageMonochrome<_tTexel>&		operator=			(const ::gpk::view_bit<_tTexel>& other)							{
			Texels									= {other.begin(), other.size() / TView::ELEMENT_BITS + one_if(other.size() % TView::ELEMENT_BITS)};
			View									= {Texels.begin(), other.size()};
			Pitch									= other.Pitch;
			return *this;
		}
		::gpk::SImageMonochrome<_tTexel>&		operator=			(const ::gpk::SImageMonochrome<_tTexel>& other)					{
			Texels									= other.Texels;
			View									= {Texels.begin(), other.View.size()};
			Pitch									= other.Pitch;
			return *this;
		}
		inlcxpr	const ::gpk::n2<uint32_t>&		metrics				()											const	noexcept	{ return {Pitch, View.size() / Pitch}; }
		inline	::gpk::error_t					resize				(const ::gpk::n2<uint32_t> & newSize)				noexcept	{ return resize(newSize.x, newSize.y); }
		::gpk::error_t							resize				(uint32_t newSizeX, uint32_t newSizeY)				noexcept	{
			gpk_necall(Texels.resize((newSizeX * (int64_t)newSizeY) / View.ELEMENT_BITS + 1), "cannot resize? Size requested: %u.", (uint32_t)((newSizeX * (int64_t)newSizeY) / View.ELEMENT_BITS + 1));
			View									= {Texels.begin(), newSizeX * newSizeY};
			Pitch									= newSizeX;
			return 0;
		}
	}; // struct

	template<typename Val>	
	using	imgmono				= ::gpk::SImageMonochrome<Val>;

	typedef	imgmono<uint64_t>	imgmonou64;

	template<typename _tTexel, typename _tDepthStencil>
	struct SRenderTarget {
		::gpk::img<_tTexel>						Color				= {};
		::gpk::img<_tDepthStencil>				DepthStencil		= {};

		inline	::gpk::v1<_tTexel>				operator[]			(uint32_t index)												{ return Color[index]; }
		inline	const ::gpk::v1<const _tTexel>	operator[]			(uint32_t index)							const				{ return Color[index]; }

		inlcxpr	const _tTexel*					begin				()											const	noexcept	{ return Color.begin	();	}
		inlcxpr	const _tTexel*					end					()											const	noexcept	{ return Color.end		();	}
		inlcxpr	_tTexel*						begin				()													noexcept	{ return Color.begin	();	}
		inlcxpr	_tTexel*						end					()													noexcept	{ return Color.end		();	}
		inlcxpr	const ::gpk::n2u32&				metrics				()											const	noexcept	{ return Color.metrics	();	}
		inlcxpr	const uint32_t&					size				()											const	noexcept	{ return Color.size		();	}
		inlcxpr	uint32_t						area				()											const	noexcept	{ return Color.area		();	}

		::gpk::error_t							resize				(const ::gpk::n2u32 & newSize)						noexcept	{
			gpk_necs(Color			.resize(newSize));
			gpk_necs(DepthStencil	.resize(newSize));
			return 0;
		}
		::gpk::error_t							resize				(const ::gpk::n2u32 & newSize, const _tTexel& color, const _tDepthStencil& depth)	noexcept	{
			gpk_necs(Color			.resize(newSize, color));
			gpk_necs(DepthStencil	.resize(newSize, depth));
			return 0;
		}
		inline	::gpk::error_t					resize				(const ::gpk::n2u8  & newSize)														noexcept	{ return resize(newSize.Cast<uint32_t>()); }
		inline	::gpk::error_t					resize				(const ::gpk::n2u16 & newSize)														noexcept	{ return resize(newSize.Cast<uint32_t>()); }
		inline	::gpk::error_t					resize				(const ::gpk::n2u8  & newSize, const _tTexel& color, const _tDepthStencil& depth)	noexcept	{ return resize(newSize.Cast<uint32_t>(), color, depth); }
		inline	::gpk::error_t					resize				(const ::gpk::n2u16 & newSize, const _tTexel& color, const _tDepthStencil& depth)	noexcept	{ return resize(newSize.Cast<uint32_t>(), color, depth); }
	};

	template<typename _tTexel, typename _tDepthStencil>
	using									rt					= SRenderTarget<_tTexel, _tDepthStencil>;

	typedef SRenderTarget<::gpk::bgra, uint32_t>	rtbgra8d32;
	typedef SRenderTarget<::gpk::rgba, uint32_t>	rtrgba8d32;

	typedef SRenderTarget<::gpk::bgra, uint8_t>		rtbgra8s8;
	typedef SRenderTarget<::gpk::rgba, uint8_t>		rtrgba8s8;

	template<typename _tTexel, typename _tDepthStencil>
	::gpk::error_t							clearTarget			(::gpk::rt<_tTexel, _tDepthStencil>& targetToClear)		{
		::gpk::img<_tTexel>							& offscreen			= targetToClear.Color;
		::gpk::img<_tDepthStencil>					& offscreenDepth	= targetToClear.DepthStencil;
		::memset(offscreenDepth	.Texels.begin(), -1, sizeof(_tDepthStencil)	* offscreenDepth	.Texels.size());	// Clear target.
		::memset(offscreen		.Texels.begin(),  0, sizeof(_tTexel)		* offscreen			.Texels.size());	// Clear target.
		return 0;
	}


	template<typename _tElement>
	::gpk::error_t							updateSizeDependentTarget	(::gpk::apod<_tElement> & out_colors, ::gpk::v2<_tElement> & out_view, const ::gpk::n2u16 & newSize)											{
		ree_if(errored(out_colors.resize(newSize.x * newSize.y)), "%s", "Out of memory?");		// Update size-dependent resources.
		if( out_view.metrics().template Cast<uint16_t>() != newSize)
			out_view								= {out_colors.begin(), newSize.Cast<uint32_t>() };
		return 0;
	}

	template<typename _tElement>
	::gpk::error_t							updateSizeDependentTarget	(::gpk::apod<_tElement> & out_colors, ::gpk::v2<_tElement> & out_view, const ::gpk::n2u16 & newSize, const _tElement & newValue)											{
		ree_if(errored(out_colors.resize(newSize.x * newSize.y, newValue)), "%s", "Out of memory?");		// Update size-dependent resources.
		if( out_view.metrics().template Cast<uint16_t>() != newSize)
			out_view								= {out_colors.begin(), newSize};

		return 0;
	}

	template<typename _tElement>
	::gpk::error_t							updateSizeDependentImage	(::gpk::apod<_tElement> & out_scaled, ::gpk::v2<_tElement> & out_view, const ::gpk::v2<_tElement> & in_view, const ::gpk::n2u16 & newSize)											{
		ree_if(errored(out_scaled.resize(newSize.x * newSize.y)), "%s", "Out of memory?");		// Update size-dependent resources.
		if( out_view.metrics().template Cast<uint16_t>() != newSize ) {
			out_view								= {out_scaled.begin(), newSize.x, newSize.y};
			if(in_view.size())
				gerror_if(errored(::gpk::grid_scale(out_view, in_view)), "%s", "I believe this never fails.");
		}
		return 0;
	}

	template<typename _tElement>
	stainli	::gpk::error_t					updateSizeDependentTarget					(::gpk::img<_tElement> & out_texture, const ::gpk::n2u16 & newSize)																					{
		return updateSizeDependentTarget(out_texture.Texels, out_texture.View, newSize);
	}
	template<typename _tElement>
	stainli	::gpk::error_t					updateSizeDependentTarget					(::gpk::img<_tElement> & out_texture, const ::gpk::n2u16 & newSize, const _tElement & newValue)																					{
		return updateSizeDependentTarget(out_texture.Texels, out_texture.View, newSize, newValue);
	}
	template<typename _tElement>
	stainli	::gpk::error_t					updateSizeDependentImage					(::gpk::img<_tElement> & out_texture, const ::gpk::v2<_tElement> & in_view, const ::gpk::n2u16 & newSize)																					{
		return updateSizeDependentImage(out_texture.Texels, out_texture.View, in_view, newSize);
	}

} // namespace

#endif // GPK_IMAGE_H_902387498237
