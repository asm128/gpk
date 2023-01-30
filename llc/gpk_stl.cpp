#include "gpk_stl.h"
#include "gpk_storage.h"

::gpk::error_t		gpk::stlFileLoad		(::gpk::vcc filename, ::gpk::SSTLFile& file) {
	gpk_necall(::gpk::fileToMemory(filename, file.Raw), "Failed to load file '%s'.", ::gpk::toString(filename).begin());
	return ::gpk::stlFileLoad(file.Raw, file.Header, file.Triangles);
}

::gpk::error_t		gpk::stlFileLoad		(::gpk::view_byte fileInMemory, ::gpk::view_byte & out_Header, ::gpk::view<::gpk::SSTLTriangle>	& out_Triangles) {
	gpk_necall(fileInMemory.slice(out_Header, 0, 80), "Invalid file size: %u bytes.", fileInMemory.size());
	if(fileInMemory.size() > 80) {
		::gpk::view<byte_t>	triangleBytes = {(byte_t*)&fileInMemory[80], fileInMemory.size() - 80};
		return ::gpk::viewRead(out_Triangles, triangleBytes);
	}
	return 0;
}
