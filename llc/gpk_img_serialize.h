#include "gpk_image.h"
#include "gpk_apod_serialize.h"

#ifndef GPK_IMG_SERIALIZE_H_23627
#define GPK_IMG_SERIALIZE_H_23627

namespace gpk
{
	tplt<tpnm TPod>
	::gpk::error_t				saveImage					(::gpk::au0_t & output, const ::gpk::grid<TPod> & input) {
		gpk_necs(gpk::savePOD(output, input.metrics()));
		gpk_necs(gpk::saveView(output, ::gpk::view<const TPod>{input.begin(), input.size()}));
		return 0;
	}

	tplt<tpnm TPod>
	::gpk::error_t				loadImage					(::gpk::vcu0_t & input, ::gpk::img<TPod> & output) {
		::gpk::vcu0_t				inputToRead				= input;
		::gpk::n2u2_t					metrics						= {};
		gpk_necs(gpk::loadPOD(inputToRead, metrics));
		::gpk::view<cnst TPod>			texels						= {};
		::gpk::err_t					bytesRead					= 0;
		gpk_necs(bytesRead = gpk::viewRead(texels, inputToRead));
		cnst ::gpk::u3_t				texelCount					= (::gpk::u3_t)metrics.x * metrics.y;
		if_true_fef(texelCount != texels.size(), "Image metrics: %u x %u. Texel count: %u.", metrics.x, metrics.y, texels.size());
		gpk_necs(inputToRead.slice(inputToRead, bytesRead));
		gpk_necs(output.Texels.resize(texels.size()));
		if(texels.size())
			memcpy(output.Texels.begin(), texels.begin(), texels.byte_count());
		output.View					= {output.Texels.begin(), metrics};
		input						= inputToRead;
		return 0;
	}

} // namespace

#endif // GPK_IMG_SERIALIZE_H_23627


