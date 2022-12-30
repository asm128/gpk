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
		::gpk::apod<byte_t>								CompressedTableFiles			;
		::gpk::apod<byte_t>								CompressedContentsPacked		;
	};

	struct SFolderInMemory {
		::gpk::apod<byte_t>								DataContents					;
		::gpk::apod<byte_t>								DataInfo						;
		::gpk::apod<::gpk::view_array<const char_t>>		Contents						;
		::gpk::apod<::gpk::vcs>				Names							;
	};

	static constexpr const uint32_t							DEFLATE_DEFAULT_CHUNK_SIZE		= 1024 * 32;
	static constexpr const uint32_t							INFLATE_DEFAULT_CHUNK_SIZE		= 1024 * 32;

	::gpk::error_t											arrayDeflate					(const ::gpk::view_const_byte	& inflated, ::gpk::apod<byte_t> & deflated, const uint32_t chunkSize = DEFLATE_DEFAULT_CHUNK_SIZE);
	::gpk::error_t											arrayInflate					(const ::gpk::view_const_byte	& deflated, ::gpk::apod<byte_t> & inflated, const uint32_t chunkSize = INFLATE_DEFAULT_CHUNK_SIZE);
	::gpk::error_t											folderPack						(::gpk::SFolderPackage	& out_packed, const ::gpk::vcs nameFolderSrc);
	::gpk::error_t											folderUnpack					(::gpk::SFolderInMemory	& out_loaded, const ::gpk::view_const_byte & rawFileInMemory);
	::gpk::error_t											folderUnpack					(::gpk::SFolderInMemory	& out_loaded, const ::gpk::vcs nameFileSrc);
	::gpk::error_t											folderToDisk					(const ::gpk::SFolderInMemory & virtualFolder, const ::gpk::vcs destinationPath);
	::gpk::error_t											folderToDisk					(const ::gpk::SFolderPackage & folderPackage, const ::gpk::vcs nameFileDst);
	::gpk::error_t											folderPackToDisk				(const ::gpk::vcs nameFileDst, const ::gpk::vcs nameFolderSrc);
	::gpk::error_t											folderUnpackToDisk				(const ::gpk::vcs namePathDst, const ::gpk::vcs nameFileSrc);

	::gpk::error_t											crcGenerate						(const ::gpk::view_const_byte & bytes, uint64_t & crc);
	::gpk::error_t											crcVerifyAndRemove				(::gpk::apod<byte_t> & bytes);
	::gpk::error_t											crcGenerateAndAppend			(::gpk::apod<byte_t> & bytes);

	struct SLoadCache {
		::gpk::apod<byte_t>								Deflated;
		::gpk::apod<byte_t>								Encrypted;
	};

					::gpk::error_t							fileToMemorySecure				(::gpk::SLoadCache& recycle, ::gpk::apod<char_t>		& loadedBytes	, const ::gpk::vcc & fileName, const ::gpk::vcc & key, const bool deflate);
					::gpk::error_t							fileFromMemorySecure			(::gpk::SLoadCache& recycle, const ::gpk::vcc	& blockBytes	, const ::gpk::vcc & fileName, const ::gpk::vcc & key, const bool deflate);
	static inline	::gpk::error_t							fileToMemorySecure				(::gpk::apod<char_t>		& loadedBytes	, const ::gpk::vcc & fileName, const ::gpk::vcc & key, const bool deflate) { ::gpk::SLoadCache temp; return ::gpk::fileToMemorySecure	(temp, loadedBytes	, fileName, key, deflate); }
	static inline	::gpk::error_t							fileFromMemorySecure			(const ::gpk::vcc	& blockBytes	, const ::gpk::vcc & fileName, const ::gpk::vcc & key, const bool deflate) { ::gpk::SLoadCache temp; return ::gpk::fileFromMemorySecure	(temp, blockBytes	, fileName, key, deflate); }

} // namespace

#endif // GPK_DEFLATE_H_92173498234
