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
		::gpk::SSerializedViewHeader	viewHeader			= {tail_width(headerToWrite.size()), tail_multiplier(headerToWrite.size()), tail_base(headerToWrite.size())};
		gpk_necs(output.append({(const uint8_t*)&viewHeader, viewHeader.ActualWidth()}));
		gpk_necs(output.append({(const uint8_t*)headerToWrite.begin(), headerToWrite.size() * (uint32_t)sizeof(T)}));
		return viewHeader.ActualWidth() + headerToWrite.size() * sizeof(T);
	}
	tplt<tpnm T> ::gpk::error_t	saveView(::gpk::ai8 & output, const ::gpk::view<T> & headerToWrite) { return ::gpk::saveView(headerToWrite, *(const ::gpk::au8*)&output); }

	tplt<tpnm T> ::gpk::error_t	savePOD	(::gpk::au8 & output, const T & input)						{ return ::gpk::saveView(output, ::gpk::view<const T>{&input, 1}); }
	tplt<tpnm T> ::gpk::error_t	savePOD	(::gpk::ai8 & output, const T & input)						{ return ::gpk::saveView(output, ::gpk::view<const T>{&input, 1}); }

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
