#include "gpk_stl.h"
#include "gpk_storage.h"

::gpk::error_t		gpk::stlFileLoad		(::gpk::view_const_char filename, ::gpk::SSTLFile& file) {
	gpk_necall(::gpk::fileToMemory(filename, file.Raw), "Failed to load file '%s'.", ::gpk::toString(filename).begin());
	file.Header			= {(uint8_t*)file.Raw.begin(), 80};
	::gpk::viewRead(file.Triangles, {&file.Raw[80], file.Raw.size() - 80});
	return 0;
}
