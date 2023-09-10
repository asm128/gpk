#include "gpk_array_pod.h"

#include "gpk_keyval.h"

#include "gpk_view_serialize.h"

#ifndef GPK_APOD_SERIALIZE_H_23627
#define GPK_APOD_SERIALIZE_H_23627

namespace gpk
{
	::gpk::error_t				keyValConstStringSerialize		(const ::gpk::view<const ::gpk::TKeyValConstChar> & keyVals, const ::gpk::vcvcc & keysToSave, ::gpk::au8 & output);

	tplt<tpnm T>
	::gpk::error_t				saveView			(::gpk::au8 & output, const ::gpk::view<T> & headerToWrite)	{
		::gpk::packedu32				viewHeader			= {tail_width(headerToWrite.size()), tail_multiplier(headerToWrite.size()), tail_base(headerToWrite.size())};
		const uint32_t					counterWidth		= viewHeader.ValueWidth();
		gpk_necs(output.append({(const uint8_t*)&viewHeader, counterWidth}));
		gpk_necs(output.append({(const uint8_t*)headerToWrite.begin(), headerToWrite.byte_count()}));
		return counterWidth + headerToWrite.byte_count();
	}
	tplt<tpnm T> ::gpk::error_t	saveView(::gpk::ai8 & output, const ::gpk::view<T> & headerToWrite) { return ::gpk::saveView(*(::gpk::au8*)&output, headerToWrite); }

	tplt<tpnm T> ::gpk::error_t			savePOD	(::gpk::au8 & output, const T & input)	{ gpk_necs(output.append((const uint8_t*)&input, sizeof(T))); return sizeof(T); }
	tplt<tpnm T> stainli ::gpk::error_t	savePOD	(::gpk::ai8 & output, const T & input)	{ return ::gpk::savePOD(*(::gpk::au8*)&output, input); }

	tplt<tpnm T> 
	::gpk::error_t				loadView			(::gpk::vcu8 & input, ::gpk::apod<T> & output) { 
		::gpk::view<const T>			readView			= {}; 
		uint32_t						bytesRead			= 0;
		gpk_necs(bytesRead = ::gpk::viewRead(readView, input)); 
		gpk_necs(input.slice(input, bytesRead));
		output						= readView;// gpk_necs(output.append(readView)); 
		return 0;
	}
	tplt<tpnm T> ::gpk::error_t	loadView			(::gpk::vci8 & input, ::gpk::apod<T> & output) { return loadView(*(::gpk::vcu8*)& input, output); }
	tplt<tpnm T> ::gpk::error_t	loadView			(::gpk::vcc  & input, ::gpk::apod<T> & output) { return loadView(*(::gpk::vcu8*)& input, output); }

} // namespace

#endif // GPK_APOD_SERIALIZE_H_23627
