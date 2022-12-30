#include "gpk_array.h"
#include "gpk_view_grid.h"
#include "gpk_view_bit.h"

#ifndef GPK_IMAGE_H_902387498237
#define GPK_IMAGE_H_902387498237

namespace gpk
{
	template<typename _tTexel>
	struct SImage {
		typedef				_tTexel									TTexel;

							::gpk::apod<_tTexel>				Texels										;
							::gpk::view2d<_tTexel>				View										;

		constexpr													SImage										()															= default;
																	SImage										(const ::gpk::view2d<_tTexel>& other)					: Texels(other)			{ View = {Texels.begin(), other		.metrics()}; }
																	SImage										(const SImage<_tTexel>& other)						: Texels(other.Texels)	{ View = {Texels.begin(), other.View.metrics()}; }

		inline constexpr	operator								view2d<const _tTexel>					()										const	noexcept	{ return View; }
		inline				operator								view2d<_tTexel>							()												noexcept	{ return View; }

							SImage&									operator=									(const ::gpk::view2d<_tTexel>& other)						{
			Texels															= view_array<const _tTexel>{other.begin(), other.size()};
			View															= {Texels.begin(), other.metrics()};
			return *this;
		}

							SImage&									operator=									(const SImage<_tTexel>& other)							{
			Texels															= other.Texels;
			View															= {Texels.begin(), other.View.metrics()};
			return *this;
		}


		inline				::gpk::view1d<_tTexel>					operator[]									(uint32_t index)																		{ return View[index]; }
		inline				const ::gpk::view1d<const _tTexel>		operator[]									(uint32_t index)												const					{ return View[index]; }

		inline constexpr	const _tTexel*							begin										()																const	noexcept	{ return View.begin		();	}
		inline constexpr	const _tTexel*							end											()																const	noexcept	{ return View.end		();	}
		inline constexpr	_tTexel*								begin										()																		noexcept	{ return View.begin		();	}
		inline constexpr	_tTexel*								end											()																		noexcept	{ return View.end		();	}
		inline constexpr	const ::gpk::SCoord2<uint32_t> &		metrics										()																const	noexcept	{ return View.metrics	();	}
		inline constexpr	const uint32_t &						size										()																const	noexcept	{ return Texels.size	();	}
		inline constexpr	uint32_t								area										()																const	noexcept	{ return View.area		();	}

		inline				::gpk::error_t							resize										(const ::gpk::SCoord2<uint32_t>& newSize)								noexcept	{ return resize(newSize.x, newSize.y); }
							::gpk::error_t							resize										(uint32_t newSizeX, uint32_t newSizeY)									noexcept	{ gpk_necall(Texels.resize(newSizeX * newSizeY), "cannot resize? Requested size: %u.", (uint32_t)(newSizeX * newSizeY)); View = {Texels.begin(), newSizeX, newSizeY}; return 0; }

		inline				::gpk::error_t							resize										(const ::gpk::SCoord2<uint32_t>& newSize, const _tTexel & newValue)		noexcept	{ return resize(newSize.x, newSize.y, newValue); }
							::gpk::error_t							resize										(uint32_t newSizeX, uint32_t newSizeY, const _tTexel & newValue)		noexcept	{
			gpk_necall(Texels.resize(newSizeX * newSizeY), "cannot resize? Requested size: %u.", newSizeX * newSizeY);
			View														= {Texels.begin(), newSizeX, newSizeY};
			gpk_necall(::gpk::drawRectangle(View, newValue, ::gpk::SRectangle2<int32_t>{{}, View.metrics().template Cast<int32_t>()}), "Unknown error setting value to rectangle of size: %u x %u.", newSizeX, newSizeY);
			return Texels.size();
		}
	}; // struct

	template<typename _tCell>	using img				= ::gpk::SImage<_tCell>;

	template<typename _tTexel>
	struct SImageMonochrome {
		typedef				_tTexel									TTexel;
		typedef				::gpk::view_bit	<_tTexel>				TView;

							::gpk::apod<_tTexel>					Texels										;
							::gpk::view_bit	<_tTexel>				View										;
							uint32_t								Pitch										= 0;

		constexpr													SImageMonochrome							()													noexcept	= default;
																	SImageMonochrome							(const ::gpk::view_bit<_tTexel>& other)
			:	Texels	(other.begin(), other.size() / TView::ELEMENT_BITS + one_if(other.size() % TView::ELEMENT_BITS))
			,	View	(Texels.begin(), other.size())
			,	Pitch	(other.Pitch)
			{}
																	SImageMonochrome							(const ::gpk::SImageMonochrome<_tTexel>& other)
			: Texels	(other.Texels)
			, View		(Texels.begin(), other.View.size())
			, Pitch		(other.Pitch)
			{}
							::gpk::SImageMonochrome<_tTexel>&		operator=									(const ::gpk::view_bit<_tTexel>& other)							{
			Texels														= {other.begin(), other.size() / TView::ELEMENT_BITS + one_if(other.size() % TView::ELEMENT_BITS)};
			View														= {Texels.begin(), other.size()};
			Pitch														= other.Pitch;
			return *this;
		}
							::gpk::SImageMonochrome<_tTexel>&		operator=									(const ::gpk::SImageMonochrome<_tTexel>& other)					{
			Texels														= other.Texels;
			View														= {Texels.begin(), other.View.size()};
			Pitch														= other.Pitch;
			return *this;
		}
		inline constexpr	const ::gpk::SCoord2<uint32_t>&			metrics										()											const	noexcept	{ return {Pitch, View.size() / Pitch}; }
		inline				::gpk::error_t							resize										(const ::gpk::SCoord2<uint32_t> & newSize)			noexcept	{ return resize(newSize.x, newSize.y); }
							::gpk::error_t							resize										(uint32_t newSizeX, uint32_t newSizeY)				noexcept	{
			gpk_necall(Texels.resize((newSizeX * (int64_t)newSizeY) / View.ELEMENT_BITS + 1), "cannot resize? Size requested: %u.", (uint32_t)((newSizeX * (int64_t)newSizeY) / View.ELEMENT_BITS + 1));
			View														= {Texels.begin(), newSizeX * newSizeY};
			Pitch														= newSizeX;
			return 0;
		}
	}; // struct

	template<typename _tTexel>
	struct SImageProcessable {
		typedef				_tTexel									TTexel;

							::gpk::img<_tTexel>						Original;
							::gpk::img<_tTexel>						Processed;
	}; // struct

	template<typename _tTexel, typename _tDepthStencil>
	struct SRenderTarget {
							::gpk::img<_tTexel>						Color										= {};
							::gpk::img<_tDepthStencil>				DepthStencil								= {};

		inline				::gpk::view1d<_tTexel>					operator[]									(uint32_t index)												{ return Color[index]; }
		inline				const ::gpk::view1d<const _tTexel>		operator[]									(uint32_t index)							const				{ return Color[index]; }

		inline constexpr	const _tTexel*							begin										()											const	noexcept	{ return Color.begin	();	}
		inline constexpr	const _tTexel*							end											()											const	noexcept	{ return Color.end		();	}
		inline constexpr	_tTexel*								begin										()													noexcept	{ return Color.begin	();	}
		inline constexpr	_tTexel*								end											()													noexcept	{ return Color.end		();	}
		inline constexpr	const ::gpk::SCoord2<uint32_t>&			metrics										()											const	noexcept	{ return Color.metrics	();	}
		inline constexpr	const uint32_t&							size										()											const	noexcept	{ return Color.size		();	}
		inline constexpr	uint32_t								area										()											const	noexcept	{ return Color.area		();	}

							::gpk::error_t							resize										(uint32_t newSizeX, uint32_t newSizeY)															noexcept	{
			gpk_necall(Color		.resize(newSizeX, newSizeY), "cannot resize? Size requested: %u x %u.", newSizeX, newSizeY);
			gpk_necall(DepthStencil	.resize(newSizeX, newSizeY), "cannot resize? Size requested: %u x %u.", newSizeX, newSizeY);
			return 0;
		}
		inline				::gpk::error_t							resize										(const ::gpk::SCoord2<uint8_t > & newSize)														noexcept	{ return resize(newSize.x, newSize.y); }
		inline				::gpk::error_t							resize										(const ::gpk::SCoord2<uint16_t> & newSize)														noexcept	{ return resize(newSize.x, newSize.y); }
		inline				::gpk::error_t							resize										(const ::gpk::SCoord2<uint32_t> & newSize)														noexcept	{ return resize(newSize.x, newSize.y); }
							::gpk::error_t							resize										(uint32_t newSizeX, uint32_t newSizeY, const _tTexel& color, const _tDepthStencil& depth)		noexcept	{
			gpk_necall(Color		.resize(newSizeX, newSizeY, color), "cannot resize? Size requested: %u x %u.", newSizeX, newSizeY);
			gpk_necall(DepthStencil	.resize(newSizeX, newSizeY, depth), "cannot resize? Size requested: %u x %u.", newSizeX, newSizeY);
			return 0;
		}
		inline				::gpk::error_t							resize										(const ::gpk::SCoord2<uint32_t> & newSize, const _tTexel& color, const _tDepthStencil& depth)	noexcept	{ return resize(newSize.x, newSize.y, color, depth); }
	};

	template<typename _tTexel, typename _tDepthStencil>
							::gpk::error_t						clearTarget									(::gpk::SRenderTarget<_tTexel, _tDepthStencil>& targetToClear)		{
		::gpk::img<_tTexel>												& offscreen									= targetToClear.Color;
		::gpk::img<_tDepthStencil>										& offscreenDepth							= targetToClear.DepthStencil;
		::memset(offscreenDepth	.Texels.begin(), -1, sizeof(_tDepthStencil)	* offscreenDepth	.Texels.size());	// Clear target.
		::memset(offscreen		.Texels.begin(),  0, sizeof(_tTexel)		* offscreen			.Texels.size());	// Clear target.
		return 0;
	}
} // namespace

#endif // GPK_IMAGE_H_902387498237
