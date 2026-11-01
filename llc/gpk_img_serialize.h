#include "gpk_image.h"
#include "gpk_apod_serialize.h"

#ifndef GPK_IMG_SERIALIZE_H_23627
#define GPK_IMG_SERIALIZE_H_23627

namespace gpk 
{
	tplt<tpnm TPod>
	::gpk::err_t				saveImage					(::gpk::au0_t & output, const ::gpk::grid<TPod> & input) {
		if_fail_fe(gpk::savePOD(output, input.metrics())); 
		if_fail_fe(gpk::saveView(output, ::gpk::view<const TPod>{input.begin(), input.size()})); 
		return 0;
	}

	tplt<tpnm TPod>
	::gpk::err_t				loadImage					(::gpk::vcu0_t & input, ::gpk::img<TPod> & output) {
		output.Texels.clear();
		::gpk::n2u2_t					metrics						= {};
		if_fail_fe(gpk::loadPOD(input, metrics));
		if_fail_fe(gpk::loadView(input, output.Texels)); 
		output.View					= {output.Texels.begin(), metrics};
		return 0;
	}

} // namespace

#endif // GPK_IMG_SERIALIZE_H_23627
