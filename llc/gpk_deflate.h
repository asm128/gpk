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

	static constexpr const uint32_t							DEFLATE_DEFAULT_CHUNK_SIZE		= 1024 * 32;
	static constexpr const uint32_t							INFLATE_DEFAULT_CHUNK_SIZE		= 1024 * 32;

	::gpk::error_t											arrayDeflate					(const ::gpk::view_const_byte	& inflated, ::gpk::array_pod<byte_t> & deflated, const uint32_t chunkSize = DEFLATE_DEFAULT_CHUNK_SIZE);
	::gpk::error_t											arrayInflate					(const ::gpk::view_const_byte	& deflated, ::gpk::array_pod<byte_t> & inflated, const uint32_t chunkSize = INFLATE_DEFAULT_CHUNK_SIZE);
	::gpk::error_t											folderPack						(::gpk::SFolderPackage	& out_packed, const ::gpk::view_const_string nameFolderSrc);
	::gpk::error_t											folderUnpack					(::gpk::SFolderInMemory	& out_loaded, const ::gpk::view_const_byte & rawFileInMemory);
	::gpk::error_t											folderUnpack					(::gpk::SFolderInMemory	& out_loaded, const ::gpk::view_const_string nameFileSrc);
	::gpk::error_t											folderToDisk					(const ::gpk::SFolderInMemory & virtualFolder, const ::gpk::view_const_string destinationPath);
	::gpk::error_t											folderToDisk					(const ::gpk::SFolderPackage & folderPackage, const ::gpk::view_const_string nameFileDst);
	::gpk::error_t											folderPackToDisk				(const ::gpk::view_const_string nameFileDst, const ::gpk::view_const_string nameFolderSrc);
	::gpk::error_t											folderUnpackToDisk				(const ::gpk::view_const_string namePathDst, const ::gpk::view_const_string nameFileSrc);
} // namespace

#endif // GPK_DEFLATE_H_92173498234
