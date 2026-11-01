#include "gpk_enum.h"

#ifndef GPK_FILE_PACK_H
#define GPK_FILE_PACK_H

namespace gpk
{
#pragma pack(push, 1)
	stct SPackHeader {
		::gpk::u2_t				TotalFileCount					;
		::gpk::u2_t				SizeCompressedTableFiles		;
		::gpk::u2_t				SizeCompressedContentsPacked	;
		::gpk::u2_t				SizeUncompressedTableFiles		;
		::gpk::u2_t				SizeUncompressedContentsPacked	;
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
		aobj<vcst_t>		Names							;
	};

	stct SLoadCache {
		au0_t				Deflated;
		au0_t				Encrypted;
	};
	err_t			folderLoad				(vcst_c nameFolderSrc, au0_t & tableFiles, au0_t & contentsPacked);
	err_t			folderPack				(SFolderPackage  & out_packed, vcst_t nameFolderSrc);
	err_t			folderUnpack			(SFolderInMemory & out_loaded, vcu0_t rawFileInMemory);
	err_t			folderUnpack			(SFolderInMemory & out_loaded, vcst_t nameFileSrc);
	err_t			folderToDisk			(cnst SFolderInMemory & virtualFolder, vcst_t destinationPath);
	err_t			folderToDisk			(cnst SFolderPackage  & folderPackage, vcst_t nameFileDst);
	err_t			folderPackToDisk		(vcst_t nameFileDst, vcst_t nameFolderSrc);
	err_t			folderUnpackToDisk		(vcst_t namePathDst, vcst_t nameFileSrc);

	err_t			inflateToMemory  		(au0_t & tempCache, vcst_t fileName, au0_t & output);
	err_t			deflateFromMemory		(au0_t & tempCache, vcst_t fileName, vcu0_t input);
	stin	err_t	inflateToMemory			(vcst_t fileName, au0_t & input)	{ au0_t temp; rtrn inflateToMemory  (temp, fileName, input); }
	stin	err_t	deflateFromMemory		(vcst_t fileName, vcu0_t input)		{ au0_t temp; rtrn deflateFromMemory(temp, fileName, input); }

	err_t			fileToMemorySecure		(SLoadCache & recycle, vcst_t fileName, vcu0_t key, b8_c deflate, au0_t & output);
	err_t			fileFromMemorySecure	(SLoadCache & recycle, vcst_t fileName, vcu0_t key, b8_c deflate, vcu0_t input);
	stin	err_t	fileToMemorySecure		(SLoadCache & recycle, vcst_t fileName, vcu0_t key, b8_c deflate, string & output)	{ rtrn fileToMemorySecure  (recycle, fileName, key, deflate, *(au0_t*)&output); }

	stin	err_t	fileToMemorySecure		(vcst_t fileName, vcu0_c & key, b8_t deflate, string & output)	{ SLoadCache temp; rtrn fileToMemorySecure  (temp, fileName, key, deflate, *(au0_t*)&output); }
	stin	err_t	fileToMemorySecure		(vcst_t fileName, vcu0_c & key, b8_t deflate, au0_t & output)	{ SLoadCache temp; rtrn fileToMemorySecure  (temp, fileName, key, deflate, output); }
	stin	err_t	fileFromMemorySecure	(vcst_t fileName, vcu0_c & key, b8_t deflate, vcu0_t input)		{ SLoadCache temp; rtrn fileFromMemorySecure(temp, fileName, key, deflate, input); }
} // namespace

#endif // GPK_FILE_PACK_H
