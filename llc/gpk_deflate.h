#include "gpk_array.h"

#ifndef GPK_DEFLATE_H_92173498234
#define GPK_DEFLATE_H_92173498234

namespace gpk
{
#pragma pack(push, 1)
	struct SPackHeader {
		uint32_t					TotalFileCount					;
		uint32_t					SizeCompressedTableFiles		;
		uint32_t					SizeCompressedContentsPacked	;
		uint32_t					SizeUncompressedTableFiles		;
		uint32_t					SizeUncompressedContentsPacked	;
	};
#pragma pack(pop)

	struct SFolderPackage {
		::gpk::SPackHeader 			PackageInfo						= {};
		::gpk::au8					CompressedTableFiles			;
		::gpk::au8					CompressedContentsPacked		;
	};

	struct SFolderInMemory {
		::gpk::au8					DataContents					;
		::gpk::au8					DataInfo						;
		::gpk::apod<::gpk::vcu8>	Contents						;
		::gpk::aobj<::gpk::vcc>		Names							;
	};

	stacxpr	uint32_t			DEFLATE_DEFAULT_CHUNK_SIZE		= 1024 * 32;
	stacxpr	uint32_t			INFLATE_DEFAULT_CHUNK_SIZE		= 1024 * 32;

	::gpk::error_t				arrayDeflate					(const ::gpk::vcu8 & inflated, ::gpk::au8 & deflated, const uint32_t chunkSize = ::gpk::DEFLATE_DEFAULT_CHUNK_SIZE);
	::gpk::error_t				arrayInflate					(const ::gpk::vcu8 & deflated, ::gpk::au8 & inflated, const uint32_t chunkSize = ::gpk::INFLATE_DEFAULT_CHUNK_SIZE);
	stainli	::gpk::error_t		arrayDeflate					(const ::gpk::vci8 & inflated, ::gpk::au8 & deflated, const uint32_t chunkSize = ::gpk::DEFLATE_DEFAULT_CHUNK_SIZE) { return ::gpk::arrayDeflate(*(::gpk::vcu8*)&inflated, deflated, chunkSize); }
	stainli	::gpk::error_t		arrayInflate					(const ::gpk::vci8 & deflated, ::gpk::au8 & inflated, const uint32_t chunkSize = ::gpk::INFLATE_DEFAULT_CHUNK_SIZE) { return ::gpk::arrayInflate(*(::gpk::vcu8*)&deflated, inflated, chunkSize); }
	stainli	::gpk::error_t		arrayDeflate					(const ::gpk::vcu8 & inflated, ::gpk::ai8 & deflated, const uint32_t chunkSize = ::gpk::DEFLATE_DEFAULT_CHUNK_SIZE) { return ::gpk::arrayDeflate(inflated, *(::gpk::au8*)&deflated, chunkSize); }
	stainli	::gpk::error_t		arrayInflate					(const ::gpk::vcu8 & deflated, ::gpk::ai8 & inflated, const uint32_t chunkSize = ::gpk::INFLATE_DEFAULT_CHUNK_SIZE) { return ::gpk::arrayInflate(deflated, *(::gpk::au8*)&inflated, chunkSize); }
	stainli	::gpk::error_t		arrayDeflate					(const ::gpk::vci8 & inflated, ::gpk::ai8 & deflated, const uint32_t chunkSize = ::gpk::DEFLATE_DEFAULT_CHUNK_SIZE) { return ::gpk::arrayDeflate(*(::gpk::vcu8*)&inflated, *(::gpk::au8*)&deflated, chunkSize); }
	stainli	::gpk::error_t		arrayInflate					(const ::gpk::vci8 & deflated, ::gpk::ai8 & inflated, const uint32_t chunkSize = ::gpk::INFLATE_DEFAULT_CHUNK_SIZE) { return ::gpk::arrayInflate(*(::gpk::vcu8*)&deflated, *(::gpk::au8*)&inflated, chunkSize); }

	::gpk::error_t				folderLoad							
		( const ::gpk::vcs	nameFolderSrc 
		, ::gpk::au8		& tableFiles				
		, ::gpk::au8		& contentsPacked			
		);
	::gpk::error_t				folderPack						(::gpk::SFolderPackage	& out_packed, const ::gpk::vcs nameFolderSrc);
	::gpk::error_t				folderUnpack					(::gpk::SFolderInMemory	& out_loaded, const ::gpk::vcu8 & rawFileInMemory);
	::gpk::error_t				folderUnpack					(::gpk::SFolderInMemory	& out_loaded, const ::gpk::vcs nameFileSrc);
	::gpk::error_t				folderToDisk					(const ::gpk::SFolderInMemory & virtualFolder, const ::gpk::vcs destinationPath);
	::gpk::error_t				folderToDisk					(const ::gpk::SFolderPackage & folderPackage, const ::gpk::vcs nameFileDst);
	::gpk::error_t				folderPackToDisk				(const ::gpk::vcs nameFileDst, const ::gpk::vcs nameFolderSrc);
	::gpk::error_t				folderUnpackToDisk				(const ::gpk::vcs namePathDst, const ::gpk::vcs nameFileSrc);

	::gpk::error_t				crcGenerate						(const ::gpk::vcu8 & bytes, uint64_t & crc);
	::gpk::error_t				crcVerifyAndRemove				(::gpk::au8 & bytes);
	::gpk::error_t				crcGenerateAndAppend			(::gpk::au8 & bytes);

	struct SLoadCache {
		::gpk::au8					Deflated;
		::gpk::au8					Encrypted;
	};

	::gpk::error_t				fileToMemorySecure				(::gpk::SLoadCache & recycle, ::gpk::au8 & loadedBytes, const ::gpk::vcc & fileName, const ::gpk::vcu8 & key, const bool deflate);
	::gpk::error_t				fileFromMemorySecure			(::gpk::SLoadCache & recycle, const ::gpk::vcu8	& blockBytes, const ::gpk::vcc & fileName, const ::gpk::vcu8 & key, const bool deflate);
	stainli	::gpk::error_t		fileToMemorySecure				(::gpk::au8			& loadedBytes	, const ::gpk::vcc & fileName, const ::gpk::vcu8 & key, const bool deflate) { ::gpk::SLoadCache temp; return ::gpk::fileToMemorySecure	(temp, loadedBytes	, fileName, key, deflate); }
	stainli	::gpk::error_t		fileFromMemorySecure			(const ::gpk::vcu8	& blockBytes	, const ::gpk::vcc & fileName, const ::gpk::vcu8 & key, const bool deflate) { ::gpk::SLoadCache temp; return ::gpk::fileFromMemorySecure(temp, blockBytes	, fileName, key, deflate); }
} // namespace

#endif // GPK_DEFLATE_H_92173498234
