#include "gpk_array.h"

#ifndef GPK_DEFLATE_H_92173498234
#define GPK_DEFLATE_H_92173498234

namespace gpk
{
	struct SPackHeader {
		uint32_t												TotalFileCount					= 0;
		uint32_t												SizeCompressedTableFiles		= 0;
		uint32_t												SizeCompressedContentsPacked	= 0;
		uint32_t												SizeUncompressedTableFiles		= 0;
		uint32_t												SizeUncompressedContentsPacked	= 0;
	};

	struct SFolderPackage {
		::gpk::SPackHeader 										PackageInfo						;
		::gpk::array_pod<byte_t>								CompressedTableFiles			;
		::gpk::array_pod<byte_t>								CompressedContentsPacked		;
	};

	struct SFolderInMemory {
		::gpk::array_pod<byte_t>								DataContents					;
		::gpk::array_pod<byte_t>								DataInfo						;
		::gpk::array_pod<::gpk::view_array<const char_t>>		Contents						;
		::gpk::array_pod<::gpk::view_const_string>				Names							;
	};

	::gpk::error_t											arrayDeflate					(const ::gpk::view_const_byte	& inflated, ::gpk::array_pod<byte_t> & deflated);
	::gpk::error_t											arrayInflate					(const ::gpk::view_const_byte	& deflated, ::gpk::array_pod<byte_t> & inflated);
	::gpk::error_t											folderPack						(::gpk::SFolderPackage	& out_packed, const ::gpk::view_const_string nameFolderSrc);
	::gpk::error_t											folderUnpack					(::gpk::SFolderInMemory	& out_loaded, const ::gpk::view_const_byte & rawFileInMemory);
	::gpk::error_t											folderUnpack					(::gpk::SFolderInMemory	& out_loaded, const ::gpk::view_const_string nameFileSrc);
	::gpk::error_t											folderToDisk					(const ::gpk::SFolderInMemory & virtualFolder, const ::gpk::view_const_string destinationPath);
	::gpk::error_t											folderToDisk					(const ::gpk::SFolderPackage & folderPackage, const ::gpk::view_const_string nameFileDst);
} // namespace

#endif // GPK_DEFLATE_H_92173498234
