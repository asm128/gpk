#include "gpk_array_pod.h"
#include "gpk_grid.h"
#include "gpk_view_bit.h"
#include "gpk_grid_scale.h"

#ifndef GPK_IMAGE_H_902387498237
#define GPK_IMAGE_H_902387498237

namespace gpk
{
	template<typename _tColor>
	struct img {
		typedef	_tColor				T;
		typedef	const _tColor		TConst;
		typedef	::gpk::view<T>		TView	;
		typedef	::gpk::grid<T>		TGrid	;
		typedef	::gpk::view<TConst>	TViewConst	;

		::gpk::apod<T>				Texels		;
		::gpk::grid<T>				View		;

		constexpr					img			()								= default;
									img			(const ::gpk::grid<T> & other)	: Texels(other)			{ View = {Texels.begin(), other		.metrics()}; }
									img			(const img<T> & other)			: Texels(other.Texels)	{ View = {Texels.begin(), other.View.metrics()}; }

		inlcxpr	operator			grid<const T>	()		const	noexcept	{ return View; }
		inline	operator			grid<T>			()				noexcept	{ return View; }

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

		inline	TView				operator[]	(uint32_t index)							{ return View[index]; }
		inline	TViewConst			operator[]	(uint32_t index)		const				{ return View[index]; }

		inlcxpr	const T*			begin		()						const	noexcept	{ return View.begin		();	}
		inlcxpr	const T*			end			()						const	noexcept	{ return View.end		();	}
		inlcxpr	T*					begin		()								noexcept	{ return View.begin		();	}
		inlcxpr	T*					end			()								noexcept	{ return View.end		();	}
		inlcxpr	const ::gpk::n2u32&	metrics		()						const	noexcept	{ return View.metrics	();	}
		inlcxpr	const uint32_t&		size		()						const	noexcept	{ return Texels.size	();	}
		inlcxpr	uint32_t			area		()						const	noexcept	{ return View.area		();	}
		inlcxpr	uint32_t 			byte_count	()						const	noexcept	{ return Texels.byte_count();	}

		inline	::gpk::error_t		resize		(const ::gpk::n2i8  & newSize)	noexcept	{ return resize(newSize.u32()); }
		inline	::gpk::error_t		resize		(const ::gpk::n2i16 & newSize)	noexcept	{ return resize(newSize.u32()); }
		inline	::gpk::error_t		resize		(const ::gpk::n2i32 & newSize)	noexcept	{ return resize(newSize.u32()); }
		inline	::gpk::error_t		resize		(const ::gpk::n2u8  & newSize)	noexcept	{ return resize(newSize.u32()); }
		inline	::gpk::error_t		resize		(const ::gpk::n2u16 & newSize)	noexcept	{ return resize(newSize.u32()); }
		inline	::gpk::error_t		resize		(const ::gpk::n2u32 & newSize)	noexcept	{ 
			gpk_necall(Texels.resize(newSize.x * newSize.y), "cannot resize? Requested size: %u.", newSize.x * newSize.y);
			View						= {Texels.begin(), newSize.x, newSize.y};
			return 0;
		}

		inline	::gpk::error_t		resize		(const ::gpk::n2i8  & newSize, const T & newValue)	noexcept	{ return resize(newSize.u32(), newValue); }
		inline	::gpk::error_t		resize		(const ::gpk::n2i16 & newSize, const T & newValue)	noexcept	{ return resize(newSize.u32(), newValue); }
		inline	::gpk::error_t		resize		(const ::gpk::n2i32 & newSize, const T & newValue)	noexcept	{ return resize(newSize.u32(), newValue); }
		inline	::gpk::error_t		resize		(const ::gpk::n2u8  & newSize, const T & newValue)	noexcept	{ return resize(newSize.u32(), newValue); }
		inline	::gpk::error_t		resize		(const ::gpk::n2u16 & newSize, const T & newValue)	noexcept	{ return resize(newSize.u32(), newValue); }
		inline	::gpk::error_t		resize		(const ::gpk::n2u32 & newSize, const T & newValue)	noexcept	{ 
			gpk_necall(Texels.resize(newSize.x * newSize.y), "cannot resize? Requested size: %u.", newSize.x * newSize.y);
			View						= {Texels.begin(), newSize.x, newSize.y};
			gpk_necall(::gpk::drawRectangle(View, newValue, ::gpk::rect2<int32_t>{{}, View.metrics().i32()}), "Unknown error setting value to rectangle of size: %u x %u.", newSize.x, newSize.y);
			return Texels.size();
		}
	}; // struct

	typedef	img<int8_t >	imgi8;
	typedef	img<int16_t>	imgi16;
	typedef	img<int32_t>	imgi32;
	typedef	img<int64_t>	imgi64;
	typedef	img<uint8_t>	imgu8;
	typedef	img<uint16_t>	imgu16;
	typedef	img<uint32_t>	imgu32;
	typedef	img<uint64_t>	imgu64;
	typedef	img<float>		imgf32;
	typedef	img<double>		imgf64;

	template<typename _tType>
	struct SImageMonochrome {
		typedef	_tType				T;
		typedef	::gpk::view_bit<T>	TView;

		::gpk::apod<T>				Texels				;
		::gpk::view_bit<T>			View				;
		uint32_t					Pitch				= 0;

		cnstxpr						SImageMonochrome	()		noexcept	= default;
									SImageMonochrome	(const ::gpk::view_bit<T> & other)
			:	Texels	(other.begin(), other.size() / TView::ELEMENT_BITS + one_if(other.size() % TView::ELEMENT_BITS))
			,	View	(Texels.begin(), other.size())
			,	Pitch	(other.Pitch)
			{}
									SImageMonochrome	(const ::gpk::SImageMonochrome<T> & other)
			: Texels	(other.Texels)
			, View		(Texels.begin(), other.View.size())
			, Pitch		(other.Pitch)
			{}

		::gpk::SImageMonochrome<T>&	operator=			(const ::gpk::view_bit<T> & other) {
			Texels						= {other.begin(), other.size() / TView::ELEMENT_BITS + one_if(other.size() % TView::ELEMENT_BITS)};
			View						= {Texels.begin(), other.size()};
			Pitch						= other.Pitch;
			return *this;
		}
		::gpk::SImageMonochrome<T>&	operator=			(const ::gpk::SImageMonochrome<T> & other) {
			Texels						= other.Texels;
			View						= {Texels.begin(), other.View.size()};
			Pitch						= other.Pitch;
			return *this;
		}

		inlcxpr	const ::gpk::n2u32&	metrics				()									const	noexcept	{ return {Pitch, View.size() / Pitch}; }
		inline	::gpk::error_t		resize				(const ::gpk::n2u32 & newSize)				noexcept	{ return resize(newSize.x, newSize.y); }
		::gpk::error_t				resize				(uint32_t newSizeX, uint32_t newSizeY)		noexcept	{
			gpk_necall(Texels.resize((newSizeX * (int64_t)newSizeY) / View.ELEMENT_BITS + 1), "cannot resize? Size requested: %u.", (uint32_t)((newSizeX * (int64_t)newSizeY) / View.ELEMENT_BITS + 1));
			View						= {Texels.begin(), newSizeX * newSizeY};
			Pitch						= newSizeX;
			return 0;
		}
	}; // struct

	template<typename T>	
	using	imgmono				= ::gpk::SImageMonochrome<T>;

	typedef	imgmono<uint64_t>	imgmonou64;

	template<typename _tColor, typename _tDepthStencil>
	struct rt {
		typedef _tColor						TColor;
		typedef _tDepthStencil				TDepth;

		typedef ::gpk::view<TColor>			VColor;
		typedef ::gpk::view<const TColor>	VCColor;

		::gpk::img<TColor>			Color			= {};
		::gpk::img<TDepth>			DepthStencil	= {};

		inline	VColor				operator[]		(uint32_t index)							{ return Color[index]; }
		inline	VCColor				operator[]		(uint32_t index)		const				{ return Color[index]; }

		inlcxpr	const TColor*		begin			()						const	noexcept	{ return Color.begin	();	}
		inlcxpr	const TColor*		end				()						const	noexcept	{ return Color.end		();	}
		inlcxpr	TColor*				begin			()								noexcept	{ return Color.begin	();	}
		inlcxpr	TColor*				end				()								noexcept	{ return Color.end		();	}
		inlcxpr	const ::gpk::n2u32&	metrics			()						const	noexcept	{ return Color.metrics	();	}
		inlcxpr	const uint32_t&		size			()						const	noexcept	{ return Color.size		();	}
		inlcxpr	uint32_t			area			()						const	noexcept	{ return Color.area		();	}

		::gpk::error_t				resize			(const ::gpk::n2u32 & newSize)	noexcept	{
			gpk_necs(Color			.resize(newSize));
			gpk_necs(DepthStencil	.resize(newSize));
			return 0;
		}
		::gpk::error_t				resize			(const ::gpk::n2u32 & newSize, const TColor & color, const TDepth & depth)	noexcept	{
			gpk_necs(Color			.resize(newSize, color));
			gpk_necs(DepthStencil	.resize(newSize, depth));
			return 0;
		}

		inline	::gpk::error_t		resize				(const ::gpk::n2u8  & newSize)												noexcept	{ return resize(newSize.u32()); }
		inline	::gpk::error_t		resize				(const ::gpk::n2u16 & newSize)												noexcept	{ return resize(newSize.u32()); }
		inline	::gpk::error_t		resize				(const ::gpk::n2u8  & newSize, const TColor & color, const TDepth & depth)	noexcept	{ return resize(newSize.u32(), color, depth); }
		inline	::gpk::error_t		resize				(const ::gpk::n2u16 & newSize, const TColor & color, const TDepth & depth)	noexcept	{ return resize(newSize.u32(), color, depth); }
	};

	template<typename TColor, typename TDepth>
	using	SRenderTarget	= rt<TColor, TDepth>;

	template<typename TColor, typename TDepth>
	::gpk::error_t				clearTarget			(::gpk::rt<TColor, TDepth> & targetToClear)		{
		::gpk::img<TColor>				& offscreen			= targetToClear.Color;
		::gpk::img<TDepth>				& offscreenDepth	= targetToClear.DepthStencil;
		::memset(offscreenDepth	.Texels.begin(), -1, sizeof(TDepth)	* offscreenDepth.Texels.size());	// Clear target.
		::memset(offscreen		.Texels.begin(),  0, sizeof(TColor)	* offscreen		.Texels.size());	// Clear target.
		return 0;
	}


	template<typename TColor>
	::gpk::error_t				updateSizeDependentTarget	(::gpk::apod<TColor> & out_colors, ::gpk::grid<TColor> & out_view, const ::gpk::n2u16 & newSize)											{
		gpk_necs(out_colors.resize(newSize.x * newSize.y));		// Update size-dependent resources.
		if( out_view.metrics().u16() != newSize)
			out_view					= {out_colors.begin(), newSize.u32() };
		return 0;
	}

	template<typename TColor>
	::gpk::error_t				updateSizeDependentTarget	(::gpk::apod<TColor> & out_colors, ::gpk::grid<TColor> & out_view, const ::gpk::n2u16 & newSize, const TColor & newValue) {
		gpk_necs(out_colors.resize(newSize.x * newSize.y, newValue));		// Update size-dependent resources.
		if( out_view.metrics().u16() != newSize)
			out_view					= {out_colors.begin(), newSize};

		return 0;
	}

	template<typename TColor>
	::gpk::error_t				updateSizeDependentImage	(::gpk::apod<TColor> & out_scaled, ::gpk::grid<TColor> & out_view, const ::gpk::grid<TColor> & in_view, const ::gpk::n2u16 & newSize)											{
		gpk_necs(out_scaled.resize(newSize.x * newSize.y));		// Update size-dependent resources.
		if( out_view.metrics().u16() != newSize ) {
			out_view					= {out_scaled.begin(), newSize.x, newSize.y};
			if(in_view.size())
				es_if(errored(::gpk::grid_scale(out_view, in_view)));
		}
		return 0;
	}

	template<typename TColor>
	stainli	::gpk::error_t		updateSizeDependentTarget	(::gpk::img<TColor> & out_texture, const ::gpk::n2u16 & newSize) {
		return updateSizeDependentTarget(out_texture.Texels, out_texture.View, newSize);
	}
	template<typename TColor>
	stainli	::gpk::error_t		updateSizeDependentTarget	(::gpk::img<TColor> & out_texture, const ::gpk::n2u16 & newSize, const TColor & newValue) {
		return updateSizeDependentTarget(out_texture.Texels, out_texture.View, newSize, newValue);
	}
	template<typename TColor>
	stainli	::gpk::error_t		updateSizeDependentImage	(::gpk::img<TColor> & out_texture, const ::gpk::grid<TColor> & in_view, const ::gpk::n2u16 & newSize) {
		return updateSizeDependentImage(out_texture.Texels, out_texture.View, in_view, newSize);
	}

} // namespace

#endif // GPK_IMAGE_H_902387498237
