#include "gpk_array_obj.h"
#include "gpk_array_pod.h"

#ifndef GPK_DEFLATE_H_92173498234
#define GPK_DEFLATE_H_92173498234

namespace gpk
{
#pragma pack(push, 1)
	struct SPackHeader {
		uint32_t				TotalFileCount					;
		uint32_t				SizeCompressedTableFiles		;
		uint32_t				SizeCompressedContentsPacked	;
		uint32_t				SizeUncompressedTableFiles		;
		uint32_t				SizeUncompressedContentsPacked	;
	};
#pragma pack(pop)

	struct SFolderPackage {
		::gpk::SPackHeader 		PackageInfo						= {};
		::gpk::au8				CompressedTableFiles			;
		::gpk::au8				CompressedContentsPacked		;
	};

	struct SFolderInMemory {
		::gpk::au8				DataContents					;
		::gpk::au8				DataInfo						;
		::gpk::avcu8			Contents						;
		::gpk::avcc				Names							;
	};

	stacxpr	uint32_t		DEFLATE_DEFAULT_CHUNK_SIZE		= 1024 * 32;
	stacxpr	uint32_t		INFLATE_DEFAULT_CHUNK_SIZE		= 1024 * 32;

	::gpk::error_t			arrayDeflate			(const ::gpk::vcu8 & inflated, ::gpk::au8 & deflated, const uint32_t chunkSize = ::gpk::DEFLATE_DEFAULT_CHUNK_SIZE);
	::gpk::error_t			arrayInflate			(const ::gpk::vcu8 & deflated, ::gpk::au8 & inflated, const uint32_t chunkSize = ::gpk::INFLATE_DEFAULT_CHUNK_SIZE);

	stainli	::gpk::error_t	arrayDeflate			(const ::gpk::vci8 & inflated, ::gpk::au8 & deflated, const uint32_t chunkSize = ::gpk::DEFLATE_DEFAULT_CHUNK_SIZE) { return ::gpk::arrayDeflate(*(::gpk::vcu8*)&inflated, deflated, chunkSize); }
	stainli	::gpk::error_t	arrayInflate			(const ::gpk::vci8 & deflated, ::gpk::au8 & inflated, const uint32_t chunkSize = ::gpk::INFLATE_DEFAULT_CHUNK_SIZE) { return ::gpk::arrayInflate(*(::gpk::vcu8*)&deflated, inflated, chunkSize); }
	stainli	::gpk::error_t	arrayDeflate			(const ::gpk::vcu8 & inflated, ::gpk::ai8 & deflated, const uint32_t chunkSize = ::gpk::DEFLATE_DEFAULT_CHUNK_SIZE) { return ::gpk::arrayDeflate(inflated, *(::gpk::au8*)&deflated, chunkSize); }
	stainli	::gpk::error_t	arrayInflate			(const ::gpk::vcu8 & deflated, ::gpk::ai8 & inflated, const uint32_t chunkSize = ::gpk::INFLATE_DEFAULT_CHUNK_SIZE) { return ::gpk::arrayInflate(deflated, *(::gpk::au8*)&inflated, chunkSize); }
	stainli	::gpk::error_t	arrayDeflate			(const ::gpk::vci8 & inflated, ::gpk::ai8 & deflated, const uint32_t chunkSize = ::gpk::DEFLATE_DEFAULT_CHUNK_SIZE) { return ::gpk::arrayDeflate(*(::gpk::vcu8*)&inflated, *(::gpk::au8*)&deflated, chunkSize); }
	stainli	::gpk::error_t	arrayInflate			(const ::gpk::vci8 & deflated, ::gpk::ai8 & inflated, const uint32_t chunkSize = ::gpk::INFLATE_DEFAULT_CHUNK_SIZE) { return ::gpk::arrayInflate(*(::gpk::vcu8*)&deflated, *(::gpk::au8*)&inflated, chunkSize); }

	::gpk::error_t			folderLoad					
		( const ::gpk::vcs	nameFolderSrc 
		, ::gpk::au8		& tableFiles				
		, ::gpk::au8		& contentsPacked			
		);
	::gpk::error_t			folderPack				(::gpk::SFolderPackage	& out_packed, const ::gpk::vcs nameFolderSrc);
	::gpk::error_t			folderUnpack			(::gpk::SFolderInMemory	& out_loaded, const ::gpk::vcu8 & rawFileInMemory);
	::gpk::error_t			folderUnpack			(::gpk::SFolderInMemory	& out_loaded, const ::gpk::vcs nameFileSrc);
	::gpk::error_t			folderToDisk			(const ::gpk::SFolderInMemory & virtualFolder, const ::gpk::vcs destinationPath);
	::gpk::error_t			folderToDisk			(const ::gpk::SFolderPackage & folderPackage, const ::gpk::vcs nameFileDst);
	::gpk::error_t			folderPackToDisk		(const ::gpk::vcs nameFileDst, const ::gpk::vcs nameFolderSrc);
	::gpk::error_t			folderUnpackToDisk		(const ::gpk::vcs namePathDst, const ::gpk::vcs nameFileSrc);

	::gpk::error_t			crcGenerate				(const ::gpk::vcu8 & bytes, uint64_t & crc);
	::gpk::error_t			crcVerifyAndRemove		(::gpk::au8 & bytes);
	::gpk::error_t			crcGenerateAndAppend	(::gpk::au8 & bytes);

	struct SLoadCache {
		::gpk::au8				Deflated;
		::gpk::au8				Encrypted;
	};

	::gpk::error_t			inflateToMemory  	(::gpk::au8 & tempCache, const ::gpk::vcc & fileName, ::gpk::au8 & output);
	::gpk::error_t			deflateFromMemory	(::gpk::au8 & tempCache, const ::gpk::vcc & fileName, const ::gpk::vcu8 & input);

	stainli	::gpk::error_t	inflateToMemory			(const ::gpk::vcc & fileName, ::gpk::au8 & input)			{ ::gpk::au8 temp; return ::gpk::inflateToMemory  (temp, fileName, input); }
	stainli	::gpk::error_t	deflateFromMemory		(const ::gpk::vcc & fileName, const ::gpk::vcu8 & input)	{ ::gpk::au8 temp; return ::gpk::deflateFromMemory(temp, fileName, input); }

	::gpk::error_t			fileToMemorySecure		(::gpk::SLoadCache & recycle, const ::gpk::vcc & fileName, const ::gpk::vcu8 & key, const bool deflate, ::gpk::au8 & output);
	::gpk::error_t			fileFromMemorySecure	(::gpk::SLoadCache & recycle, const ::gpk::vcc & fileName, const ::gpk::vcu8 & key, const bool deflate, const ::gpk::vcu8 & input);
	stainli	::gpk::error_t	fileToMemorySecure		(::gpk::SLoadCache & recycle, const ::gpk::vcc & fileName, const ::gpk::vcu8 & key, const bool deflate, ::gpk::achar & output)		{ return ::gpk::fileToMemorySecure  (recycle, fileName, key, deflate, *(::gpk::au8*)&output); }

	stainli	::gpk::error_t	fileToMemorySecure		(const ::gpk::vcc & fileName, const ::gpk::vcu8 & key, const bool deflate, ::gpk::achar & output)		{ ::gpk::SLoadCache temp; return ::gpk::fileToMemorySecure  (temp, fileName, key, deflate, *(::gpk::au8*)&output); }
	stainli	::gpk::error_t	fileToMemorySecure		(const ::gpk::vcc & fileName, const ::gpk::vcu8 & key, const bool deflate, ::gpk::au8 & output)			{ ::gpk::SLoadCache temp; return ::gpk::fileToMemorySecure  (temp, fileName, key, deflate, output); }
	stainli	::gpk::error_t	fileFromMemorySecure	(const ::gpk::vcc & fileName, const ::gpk::vcu8 & key, const bool deflate, const ::gpk::vcu8 & input)	{ ::gpk::SLoadCache temp; return ::gpk::fileFromMemorySecure(temp, fileName, key, deflate, input); }
} // namespace

#endif // GPK_DEFLATE_H_92173498234
