#include "gpk_stl.h"
#include "gpk_file.h"

::gpk::error_t			gpk::stlFileLoad		(::gpk::vcc filename, ::gpk::SSTLFile& file) {
	gpk_necall(::gpk::fileToMemory(filename, file.Raw), "Failed to load file '%s'.", ::gpk::toString(filename).begin());
	return ::gpk::stlFileLoad(file.Raw, file.Header, file.Triangles);
}

::gpk::error_t			gpk::stlFileLoad		(::gpk::vcu8 fileInMemory, ::gpk::vcu8 & out_Header, ::gpk::view<::gpk::SSTLTriangle>	& out_Triangles) {
	gpk_necall(fileInMemory.slice(out_Header, 0, 80), "Invalid file size: %u bytes.", fileInMemory.size());
	if(fileInMemory.size() > 80) {
		::gpk::vu8				triangleBytes			= {(uint8_t*)&fileInMemory[80], fileInMemory.size() - 80};
		return ::gpk::viewReadLegacy(out_Triangles, triangleBytes);
	}
	return 0;
}
