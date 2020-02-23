#include "gpk_array.h"
#include "gpk_view_grid.h"
#include "gpk_view_bit.h"

#ifndef GPK_IMAGE_H_902387498237
#define GPK_IMAGE_H_902387498237

namespace gpk
{
	template<typename _tTexel>
	struct SImage {
		typedef				_tTexel												TTexel;

							::gpk::array_pod<_tTexel>							Texels										;
							::gpk::view_grid<_tTexel>							View										;

		constexpr																SImage										()																= default;
																				SImage										(const ::gpk::view_grid<_tTexel>& other)						: Texels(other)			{ View = {Texels.begin(), other		.metrics()}; }
																				SImage										(const ::gpk::SImage<_tTexel>& other)							: Texels(other.Texels)	{ View = {Texels.begin(), other.View.metrics()}; }

		inline constexpr	operator											view_grid<const _tTexel>					()											const	noexcept	{ return View; }
		inline				operator											view_grid<_tTexel>							()													noexcept	{ return View; }

							::gpk::SImage<_tTexel>&								operator=									(const ::gpk::view_grid<_tTexel>& other)						{
			Texels																	= view_array<const _tTexel>{other.begin(), other.size()};
			View																	= {Texels.begin(), other.metrics()};
			return *this;
		}

							::gpk::SImage<_tTexel>&								operator=									(const ::gpk::SImage<_tTexel>& other)							{
			Texels																	= other.Texels;
			View																	= {Texels.begin(), other.View.metrics()};
			return *this;
		}


		inline				::gpk::view_array<_tTexel>							operator[]									(uint32_t index)																	{ return View[index]; }
		inline				const ::gpk::view_array<const _tTexel>				operator[]									(uint32_t index)												const				{ return View[index]; }

		inline constexpr	const ::gpk::SCoord2<uint32_t>&						metrics										()																const	noexcept	{ return View.metrics(); }
		inline				::gpk::error_t										resize										(const ::gpk::SCoord2<uint32_t>& newSize)								noexcept	{ return resize(newSize.x, newSize.y); }
							::gpk::error_t										resize										(uint32_t newSizeX, uint32_t newSizeY)									noexcept	{ gpk_necall(Texels.resize(newSizeX * newSizeY), "cannot resize? Requested size: %u.", (uint32_t)(newSizeX * newSizeY)); View = {Texels.begin(), newSizeX, newSizeY}; return 0; }

		inline				::gpk::error_t										resize										(const ::gpk::SCoord2<uint32_t>& newSize, const _tTexel & newValue)		noexcept	{ return resize(newSize.x, newSize.y, newValue); }
							::gpk::error_t										resize										(uint32_t newSizeX, uint32_t newSizeY, const _tTexel & newValue)		noexcept	{
			gpk_necall(Texels.resize(newSizeX * newSizeY), "cannot resize? Requested size: %u.", newSizeX * newSizeY);
			View																	= {Texels.begin(), newSizeX, newSizeY};
			gpk_necall(::gpk::drawRectangle(View, newValue, ::gpk::SRectangle2<int32_t>{{}, View.metrics().template Cast<int32_t>()}), "Unknown error setting value to rectangle of size: %u x %u.", newSizeX, newSizeY);
			return 0;
		}
	}; // struct

	template<typename _tTexel>
	struct SImageMonochrome {
		typedef				_tTexel												TTexel;

							::gpk::array_pod<_tTexel>							Texels										;
							::gpk::view_bit	<_tTexel>							View										;
							uint32_t											Pitch										= 0;

		constexpr																SImageMonochrome							()													noexcept	= default;
																				SImageMonochrome							(const ::gpk::view_bit<_tTexel>& other)							{
			Texels																	= other;
			View																	= {Texels.begin(), Texels.size()};
			Pitch																	= other.Pitch;
		}

																				SImageMonochrome							(const ::gpk::SImageMonochrome<_tTexel>& other)				{
			Texels																	= other.Texels;
			View																	= {Texels.begin(), Texels.size()};
			Pitch																	= other.Pitch;
		}

							::gpk::SImageMonochrome<_tTexel>&					operator=									(const ::gpk::view_bit<_tTexel>& other)							{
			Texels																	= other;
			View																	= {Texels.begin(), Texels.size()};
			Pitch																	= other.Pitch;
			return *this;
		}

							::gpk::SImageMonochrome<_tTexel>&					operator=									(const ::gpk::SImageMonochrome<_tTexel>& other)				{
			Texels																	= other.Texels;
			View																	= {Texels.begin(), Texels.size()};
			Pitch																	= other.Pitch;
			return *this;
		}

							::gpk::error_t										resize										(uint32_t newSizeX, uint32_t newSizeY)				noexcept	{ gpk_necall(Texels.resize((newSizeX * (int64_t)newSizeY) / (sizeof(_tTexel) * 8) + 1), "cannot resize? Size requested: %u.", (uint32_t)((newSizeX * (int64_t)newSizeY) / (sizeof(_tTexel) * 8) + 1)); View = {Texels.begin(), newSizeX * newSizeY}; Pitch = newSizeX; return 0; }
		inline				::gpk::error_t										resize										(const ::gpk::SCoord2<uint32_t> & newSize)			noexcept	{ return resize(newSize.x, newSize.y); }
		inline constexpr	const ::gpk::SCoord2<uint32_t>&						metrics										()											const	noexcept	{ return {Pitch, View.size() / Pitch}; }
	}; // struct

	template<typename _tTexel>
	struct SImageProcessable {
		typedef				_tTexel												TTexel;

							::gpk::SImage<_tTexel>								Original;
							::gpk::SImage<_tTexel>								Processed;
	}; // struct

	template<typename _tTexel, typename _tDepthStencil>
	struct SRenderTarget {
							::gpk::SImage<_tTexel>								Color										= {};
							::gpk::SImage<_tDepthStencil>						DepthStencil								= {};
							::gpk::error_t										resize										(uint32_t newSizeX, uint32_t newSizeY)															noexcept	{
			gpk_necall(Color		.resize(newSizeX, newSizeY), "cannot resize? Size requested: %u x %u.", newSizeX, newSizeY);
			gpk_necall(DepthStencil	.resize(newSizeX, newSizeY), "cannot resize? Size requested: %u x %u.", newSizeX, newSizeY);
			return 0;
		}
		inline				::gpk::error_t										resize										(const ::gpk::SCoord2<uint32_t> & newSize)														noexcept	{ return resize(newSize.x, newSize.y); }
							::gpk::error_t										resize										(uint32_t newSizeX, uint32_t newSizeY, const _tTexel& color, const _tDepthStencil& depth)		noexcept	{
			gpk_necall(Color		.resize(newSizeX, newSizeY, color), "cannot resize? Size requested: %u x %u.", newSizeX, newSizeY);
			gpk_necall(DepthStencil	.resize(newSizeX, newSizeY, depth), "cannot resize? Size requested: %u x %u.", newSizeX, newSizeY);
			return 0;
		}
		inline				::gpk::error_t										resize										(const ::gpk::SCoord2<uint32_t> & newSize, const _tTexel& color, const _tDepthStencil& depth)	noexcept	{ return resize(newSize.x, newSize.y, color, depth); }
	};

	template<typename _tTexel, typename _tDepthStencil>
							::gpk::error_t									clearTarget									(::gpk::SRenderTarget<_tTexel, _tDepthStencil>& targetToClear)		{
		::gpk::SImage<_tTexel>													& offscreen									= targetToClear.Color;
		::gpk::SImage<_tDepthStencil>											& offscreenDepth							= targetToClear.DepthStencil;
		::memset(offscreenDepth	.Texels.begin(), -1, sizeof(_tDepthStencil)	* offscreenDepth	.Texels.size());	// Clear target.
		::memset(offscreen		.Texels.begin(),  0, sizeof(_tTexel)		* offscreen			.Texels.size());	// Clear target.
		return 0;
	}
} // namespace

#endif // GPK_IMAGE_H_902387498237
