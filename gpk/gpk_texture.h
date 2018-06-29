#include "gpk_array.h"
#include "gpk_view_grid.h"
#include "gpk_view_bit.h"

#ifndef GPK_TEXTURE_H_902387498237
#define GPK_TEXTURE_H_902387498237

namespace gpk
{
	template<typename _tTexel>
	struct STexture {
		typedef				_tTexel												TTexel;

							::gpk::array_pod<_tTexel>							Texels										;
							::gpk::view_grid<_tTexel>							View										;

		constexpr																STexture									()													= default;
																				STexture									(const ::gpk::view_grid<_tTexel>& other)			: Texels(other)			{ View = {Texels.begin(), other		.metrics()}; }
																				STexture									(const ::gpk::STexture<_tTexel>& other)				: Texels(other.Texels)	{ View = {Texels.begin(), other.View.metrics()}; }

							::gpk::STexture<_tTexel>&							operator=									(const ::gpk::view_grid<_tTexel>& other)			{ 
			Texels																	= {other.begin(), other.size()};
			View																	= {Texels.begin(), other.metrics()};
			return *this; 
		}

							::gpk::STexture<_tTexel>&							operator=									(const ::gpk::STexture<_tTexel>& other)				{ 
			Texels																	= other.Texels;
			View																	= {Texels.begin(), other.View.metrics()};
			return *this; 
		}


		inline				::gpk::view_array<_tTexel>							operator[]									(uint32_t index)									{ return View[index]; }
		inline	const		::gpk::view_array<_tTexel>							operator[]									(uint32_t index)	const							{ return View[index]; }

							::gpk::error_t										resize										(uint32_t newSizeX, uint32_t newSizeY)				{ gpk_necall(Texels.resize(newSizeX * newSizeY), "cannot resize?"); View = {Texels.begin(), newSizeX, newSizeY}; return 0; }
		inline				::gpk::error_t										resize										(const ::gpk::SCoord2<uint32_t>& newSize)			{ return resize(newSize.x, newSize.y); }
	}; // struct

	template<typename _tTexel>
	struct STextureMonochrome {
		typedef				_tTexel												TTexel;

							::gpk::array_pod<_tTexel>							Texels										;
							::gpk::view_bit	<_tTexel>							View										;
							uint32_t											Pitch										= 0;

		constexpr																STextureMonochrome							()													= default;
																				STextureMonochrome							(const ::gpk::view_bit<_tTexel>& other)				{ 
			Texels																	= other;
			View																	= {Texels.begin(), Texels.size()};
			Pitch																	= other.Pitch;
			return *this; 
		}

																				STextureMonochrome							(const ::gpk::STextureMonochrome<_tTexel>& other)	{ 
			Texels																	= other.Texels;
			View																	= {Texels.begin(), Texels.size()};
			Pitch																	= other.Pitch;
			return *this; 
		}

							::gpk::STextureMonochrome<_tTexel>&					operator=									(const ::gpk::view_bit<_tTexel>& other)				{ 
			Texels																	= other;
			View																	= {Texels.begin(), Texels.size()};
			Pitch																	= other.Pitch;
			return *this; 
		}

							::gpk::STextureMonochrome<_tTexel>&					operator=									(const ::gpk::STextureMonochrome<_tTexel>& other)	{ 
			Texels																	= other.Texels;
			View																	= {Texels.begin(), Texels.size()};
			Pitch																	= other.Pitch;
			return *this; 
		}

							::gpk::error_t										resize										(uint32_t newSizeX, uint32_t newSizeY)				{ gpk_necall(Texels.resize((newSizeX * (int64_t)newSizeY) / (sizeof(_tTexel) * 8) + 1), "cannot resize?"); View = {Texels.begin(), newSizeX * newSizeY}; Pitch = newSizeX; return 0; }
		inline				::gpk::error_t										resize										(const ::gpk::SCoord2<uint32_t> & newSize)			{ return resize(newSize.x, newSize.y); }
	}; // struct



	template<typename _tTexel>
	struct STextureProcessable {
		typedef				_tTexel												TTexel;

							::gpk::STexture<_tTexel>							Original;
							::gpk::STexture<_tTexel>							Processed;
	}; // struct
} // namespace 

#endif // GPK_TEXTURE_H_902387498237
