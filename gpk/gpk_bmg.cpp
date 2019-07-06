#include "gpk_bmg.h"
#include "gpk_io.h"

					::gpk::error_t																	gpk::bmgFileLoad							(FILE							* source		, ::gpk::array_pod<::gpk::SColorBGRA>& out_Colors, ::gpk::view_grid<::gpk::SColorBGRA>& out_ImageView)	{
	ree_if(0 == source, "%s", "Invalid function usage: destionation file cannot be NULL.");
	uint32_t																								sizeRead									= (uint32_t)(out_ImageView.size() * sizeof(::gpk::SColorBGRA) + sizeof(uint32_t) + sizeof(uint32_t));
	uint32_t																								elementSize									= 0;  fread(&elementSize, sizeof(uint32_t					), 1, source);
	::gpk::SCoord2<uint32_t>																				gridMetrics									= {}; fread(&gridMetrics, sizeof(::gpk::SCoord2<uint32_t>	), 1, source);
	gpk_necall(out_Colors.resize(gridMetrics.x * gridMetrics.y), "%s", "Out of memory?");
	ree_if(out_Colors.size() != fread(out_Colors.begin(), ::gpk::min((size_t)elementSize, sizeof(::gpk::SColorBGRA)), out_Colors.size(), source), "%s", "Corrupt file?");
	out_ImageView																						= {out_Colors.begin(), gridMetrics};
	return sizeRead;
}

					::gpk::error_t																	gpk::bmgFileLoad							(const byte_t					* source		, ::gpk::array_pod<::gpk::SColorBGRA>& out_Colors, ::gpk::view_grid<::gpk::SColorBGRA>& out_ImageView)	{
	ree_if(0 == source, "%s", "Invalid function usage: source cannot be NULL.");
	uint32_t																								sizeRead									= (uint32_t)(sizeof(uint32_t) + sizeof(uint32_t));
	uint32_t																								& elementSize								= *(uint32_t*)source;
	::gpk::SCoord2<uint32_t>																				& gridMetrics								= *(::gpk::SCoord2<uint32_t>*)(source + sizeof(uint32_t));
	::gpk::SColorBGRA																						* elementGrid								= (::gpk::SColorBGRA*)(source + sizeof(uint32_t) + sizeof(::gpk::SCoord2<uint32_t>));
	gpk_necall(out_Colors.resize(gridMetrics.x * gridMetrics.y), "%s", "Out of memory");
	memcpy(out_Colors.begin(), elementGrid, out_Colors.size() * ::gpk::min((size_t)elementSize, sizeof(::gpk::SColorBGRA)));
	out_ImageView																						= {out_Colors.begin(), gridMetrics};
	return sizeRead + out_Colors.size();
}

					::gpk::error_t																	gpk::bmgFileWrite							(byte_t							* destination	, const ::gpk::view_grid<::gpk::SColorBGRA>& in_ImageView)							{
	uint32_t																								sizeToWrite									= (uint32_t)(in_ImageView.size() * sizeof(::gpk::SColorBGRA) + sizeof(uint32_t) + sizeof(uint32_t));
	if(0 == destination)
		return sizeToWrite;	// count + element size + (grid size * element size)
	uint32_t																								& elementSize								= *(uint32_t*)destination;
	::gpk::SCoord2<uint32_t>																				& gridMetrics								= *(::gpk::SCoord2<uint32_t>*)(destination + sizeof(uint32_t));
	::gpk::SColorBGRA																						* elementGrid								= (::gpk::SColorBGRA*)(destination + sizeof(uint32_t) + sizeof(::gpk::SCoord2<uint32_t>));
	elementSize																							= (uint32_t)sizeof(::gpk::SColorBGRA);
	gridMetrics																							= in_ImageView.metrics();
	memcpy(elementGrid, in_ImageView.begin(), in_ImageView.size() * (size_t)elementSize);
	return sizeToWrite;
}

					::gpk::error_t																	gpk::bmgFileWrite							(FILE							* destination	, const ::gpk::view_grid<::gpk::SColorBGRA>& in_ImageView)												{
	ree_if(0 == destination, "%s", "Invalid function usage: destionation file cannot be NULL.");
	uint32_t																								sizeToWrite									= (uint32_t)(in_ImageView.size() * sizeof(::gpk::SColorBGRA) + sizeof(uint32_t) + sizeof(uint32_t));
	const uint32_t																							elementSize									= (uint32_t)sizeof(::gpk::SColorBGRA);	ree_if(fwrite(&elementSize, sizeof(uint32_t					), 1, destination) != 1, "%s", "No space on disk?");
	const ::gpk::SCoord2<uint32_t>																			& gridMetrics								= in_ImageView.metrics();				ree_if(fwrite(&gridMetrics, sizeof(::gpk::SCoord2<uint32_t> ), 1, destination) != 1, "%s", "No space on disk?");
	ree_if(fwrite(in_ImageView.begin(), elementSize, in_ImageView.size(), destination) != in_ImageView.size(), "%s", "No space on disk?");
	return sizeToWrite;
}

					::gpk::error_t																	gpk::bmgFileLoad							(const ::gpk::view_const_string	& filename		, ::gpk::array_pod<::gpk::SColorBGRA>& out_Colors, ::gpk::view_grid<::gpk::SColorBGRA>& out_ImageView)	{
	FILE																									* source									= 0;
	fopen_s(&source, filename.begin(), "rb");
	if(0 == source) {
		error_printf("Failed to open file: %s. File not found?", filename.begin());
		return -1;
	}
	if errored(::gpk::bmgFileLoad(source, out_Colors, out_ImageView)) {
		error_printf("Failed to load file: '%s'. File corrupt?", filename.begin());
		fclose(source);
		return -1;
	}
	fclose(source);
	return 0;
}

					::gpk::error_t																	gpk::bmgFileWrite							(const ::gpk::view_const_string	& filename		, const ::gpk::view_grid<::gpk::SColorBGRA>& in_ImageView)												{
	FILE																									* source									= 0;
	fopen_s(&source, filename.begin(), "wb");
	if(0 == source) {
		error_printf("Failed to open file: %s. Forbidden?", filename.begin());
		return -1;
	}
	if errored(::gpk::bmgFileWrite(source, in_ImageView)) {
		error_printf("Failed to store file: '%s'. No space on disk?", filename.begin());
		fclose(source);
		return -1;
	}
	fclose(source);
	return 0;
}

