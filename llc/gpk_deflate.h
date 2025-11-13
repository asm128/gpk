#include "gpk_array_obj.h"
#include "gpk_array_pod.h"

#ifndef GPK_DEFLATE_H_23627
#define GPK_DEFLATE_H_23627

namespace gpk
{
#pragma pack(push, 1)
	stct SPackHeader {
		uint32_t				TotalFileCount					;
		uint32_t				SizeCompressedTableFiles		;
		uint32_t				SizeCompressedContentsPacked	;
		uint32_t				SizeUncompressedTableFiles		;
		uint32_t				SizeUncompressedContentsPacked	;
	};
#pragma pack(pop)

	stct SFolderPackage {
		SPackHeader 		PackageInfo						= {};
		au0_t				CompressedTableFiles			;
		au0_t				CompressedContentsPacked		;
	};

	stct SFolderInMemory {
		au0_t				DataContents					;
		au0_t				DataInfo						;
		avcu0_t				Contents						;
		avcsc_t				Names							;
	};


#if defined(GPK_ESP32) || defined(GPK_ARDUINO)
	stxp	uint32_t		DEFLATE_DEFAULT_CHUNK_SIZE		= 1024 * 4;
	stxp	uint32_t		INFLATE_DEFAULT_CHUNK_SIZE		= 1024 * 4;
#else
	stxp	uint32_t		DEFLATE_DEFAULT_CHUNK_SIZE		= 1024 * 32;
	stxp	uint32_t		INFLATE_DEFAULT_CHUNK_SIZE		= 1024 * 32;
#endif	

	err_t			arrayDeflate			(vcu0_c & inflated, au0_t & deflated, u2_c chunkSize = DEFLATE_DEFAULT_CHUNK_SIZE);
	err_t			arrayInflate			(vcu0_c & deflated, au0_t & inflated, u2_c chunkSize = INFLATE_DEFAULT_CHUNK_SIZE);

	stin	err_t	arrayDeflate			(vcs0_c & inflated, au0_t & deflated, u2_c chunkSize = DEFLATE_DEFAULT_CHUNK_SIZE)	{ rtrn arrayDeflate(*(vcu0_t*)&inflated, deflated, chunkSize); }
	stin	err_t	arrayInflate			(vcs0_c & deflated, au0_t & inflated, u2_c chunkSize = INFLATE_DEFAULT_CHUNK_SIZE)	{ rtrn arrayInflate(*(vcu0_t*)&deflated, inflated, chunkSize); }
	stin	err_t	arrayDeflate			(vcu0_c & inflated, as0_t & deflated, u2_c chunkSize = DEFLATE_DEFAULT_CHUNK_SIZE)	{ rtrn arrayDeflate(inflated, *(au0_t*)&deflated, chunkSize); }
	stin	err_t	arrayInflate			(vcu0_c & deflated, as0_t & inflated, u2_c chunkSize = INFLATE_DEFAULT_CHUNK_SIZE)	{ rtrn arrayInflate(deflated, *(au0_t*)&inflated, chunkSize); }
	stin	err_t	arrayDeflate			(vcs0_c & inflated, as0_t & deflated, u2_c chunkSize = DEFLATE_DEFAULT_CHUNK_SIZE)	{ rtrn arrayDeflate(*(vcu0_t*)&inflated, *(au0_t*)&deflated, chunkSize); }
	stin	err_t	arrayInflate			(vcs0_c & deflated, as0_t & inflated, u2_c chunkSize = INFLATE_DEFAULT_CHUNK_SIZE)	{ rtrn arrayInflate(*(vcu0_t*)&deflated, *(au0_t*)&inflated, chunkSize); }

	err_t			folderLoad					
		( vcst_c	nameFolderSrc 
		, au0_t		& tableFiles				
		, au0_t		& contentsPacked			
		);
	err_t			folderPack				(SFolderPackage	& out_packed, vcst_c nameFolderSrc);
	err_t			folderUnpack			(SFolderInMemory	& out_loaded, vcu0_c & rawFileInMemory);
	err_t			folderUnpack			(SFolderInMemory	& out_loaded, vcst_c nameFileSrc);
	err_t			folderToDisk			(cnst SFolderInMemory & virtualFolder, vcst_c destinationPath);
	err_t			folderToDisk			(cnst SFolderPackage & folderPackage, vcst_c nameFileDst);
	err_t			folderPackToDisk		(vcst_c nameFileDst, vcst_c nameFolderSrc);
	err_t			folderUnpackToDisk		(vcst_c namePathDst, vcst_c nameFileSrc);

	err_t			crcGenerate				(vcu0_c & bytes, uint64_t & crc);
	err_t			crcVerifyAndRemove		(au0_t & bytes);
	err_t			crcGenerateAndAppend	(au0_t & bytes);

	stct SLoadCache {
		au0_t				Deflated;
		au0_t				Encrypted;
	};

	err_t			inflateToMemory  		(au0_t & tempCache, vcsc_c & fileName, au0_t & output);
	err_t			deflateFromMemory		(au0_t & tempCache, vcsc_c & fileName, vcu0_c & input);
	stin	err_t	inflateToMemory			(vcsc_c & fileName, au0_t & input)	{ au0_t temp; rtrn inflateToMemory  (temp, fileName, input); }
	stin	err_t	deflateFromMemory		(vcsc_c & fileName, vcu0_c & input)	{ au0_t temp; rtrn deflateFromMemory(temp, fileName, input); }

	err_t			fileToMemorySecure		(SLoadCache & recycle, vcsc_c & fileName, vcu0_c & key, b8_c deflate, au0_t & output);
	err_t			fileFromMemorySecure	(SLoadCache & recycle, vcsc_c & fileName, vcu0_c & key, b8_c deflate, vcu0_c & input);
	stin	err_t	fileToMemorySecure		(SLoadCache & recycle, vcsc_c & fileName, vcu0_c & key, b8_c deflate, asc_t & output)	{ rtrn fileToMemorySecure  (recycle, fileName, key, deflate, *(au0_t*)&output); }

	stin	err_t	fileToMemorySecure		(vcsc_c & fileName, vcu0_c & key, b8_t deflate, asc_t & output)	{ SLoadCache temp; rtrn fileToMemorySecure  (temp, fileName, key, deflate, *(au0_t*)&output); }
	stin	err_t	fileToMemorySecure		(vcsc_c & fileName, vcu0_c & key, b8_t deflate, au0_t & output)	{ SLoadCache temp; rtrn fileToMemorySecure  (temp, fileName, key, deflate, output); }
	stin	err_t	fileFromMemorySecure	(vcsc_c & fileName, vcu0_c & key, b8_t deflate, vcu0_c & input)	{ SLoadCache temp; rtrn fileFromMemorySecure(temp, fileName, key, deflate, input); }
} // namespace

#endif // GPK_DEFLATE_H_23627
