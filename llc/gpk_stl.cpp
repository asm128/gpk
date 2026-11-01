#include "gpk_stl.h"
#include "gpk_file.h"

gpk::err_t			gpk::stlFileLoad		(vcst_t filename, SSTLFile& file) {
	if_fail_fef(fileToMemory(filename, file.Raw), "Failed to load file '%.*s'.", GPK_VIEW_ARGS(filename));
	return stlFileLoad(file.Raw, file.Header, file.Triangles);
}

gpk::err_t			gpk::stlFileLoad		(vu0_t fileInMemory, vcu0_t & out_Header, view<SSTLTriangle> & out_Triangles) {
	vcu0_t						header					= {};
	view<SSTLTriangle>			triangles				= {};
	if_fail_fef(fileInMemory.slice(header, 0, 80), "Invalid file size: %u bytes.", fileInMemory.size());
	vu0_t						triangleBytes			= {};
	if_fail_fef(fileInMemory.slice(triangleBytes, 80), "Invalid file size: %u bytes.", fileInMemory.size());
	err_t						bytesRead				= 0;
	if_fail_fef(bytesRead = viewPascal(triangles, triangleBytes), "Invalid triangle data. File size: %u bytes.", fileInMemory.size());
	out_Header					= header;
	out_Triangles				= triangles;
	return bytesRead;
}

gpk::err_t			gpk::stlFileLoad		(vcu0_t fileInMemory, vcu0_t & out_Header, view<cnst SSTLTriangle> & out_Triangles) {
	vcu0_t						header					= {};
	view<cnst SSTLTriangle>		triangles				= {};
	if_fail_fef(fileInMemory.slice(header, 0, 80), "Invalid file size: %u bytes.", fileInMemory.size());
	vcu0_t						triangleBytes			= {};
	if_fail_fef(fileInMemory.slice(triangleBytes, 80), "Invalid file size: %u bytes.", fileInMemory.size());
	err_t						bytesRead				= 0;
	if_fail_fef(bytesRead = viewPascal(triangles, triangleBytes), "Invalid triangle data. File size: %u bytes.", fileInMemory.size());
	out_Header					= header;
	out_Triangles				= triangles;
	return bytesRead;
}
