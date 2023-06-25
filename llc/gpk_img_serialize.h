#include "gpk_image.h"
#include "gpk_apod_serialize.h"

#ifndef GPK_IMG_SERIALIZE_H_902387498237
#define GPK_IMG_SERIALIZE_H_902387498237

namespace gpk 
{
	template<typename TPod>
	::gpk::error_t				saveImage					(::gpk::au8 & output, ::gpk::grid<TPod> input) {
		gpk_necs(::gpk::savePOD(output, input.metrics())); 
		gpk_necs(::gpk::saveView(output, ::gpk::view<TPod>{input.begin(), input.size()})); 
		return 0;
	}

	template<typename TPod>
	::gpk::error_t				loadImage					(::gpk::vcu8 & input, ::gpk::img<TPod> & output) {
		output.Texels.clear();
		::gpk::n2u32					metrics						= {};
		gpk_necs(::gpk::loadPOD(input, metrics));
		gpk_necs(::gpk::loadView(input, output.Texels)); 
		output.View					= {output.Texels.begin(), metrics};
		return 0;
	}

} // namespace

#endif // GPK_IMG_SERIALIZE_H_902387498237
