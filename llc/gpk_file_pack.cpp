#include "gpk_file_pack.h"
#include "gpk_deflate.h"
#include "gpk_path.h"
#include "gpk_file.h"
#include "gpk_aes.h"
#include "gpk_apod_serialize.h"
#include "gpk_string_compose.h"
//#include "gpk_cstdio.h"


GPK_USING_TYPEINT();

gpk::err_t			gpk::folderUnpack			(SFolderInMemory & out_loaded, vcst_t nameFileSrc)					{
	au0_t				rawFileInMemory				= {};
	if_fail_fef(fileToMemory(nameFileSrc, rawFileInMemory), "Failed to load pak file: %.*s.", (int)nameFileSrc.size(), nameFileSrc.begin());
	if_fail_fef(folderUnpack(out_loaded, rawFileInMemory), "Failed to unpack pak file: %.*s.", (int)nameFileSrc.size(), nameFileSrc.begin());
	return 0;
}
gpk::err_t			gpk::folderToDisk			(const SFolderPackage & folderPackage, const vcst_t nameFileDst)			{
	const SPackHeader 	& fileHeader				= folderPackage.PackageInfo;
	const au0_t			& compressedTableFiles		= folderPackage.CompressedTableFiles		;
	const au0_t			& compressedContentsPacked	= folderPackage.CompressedContentsPacked	;
	{
		FILE						* fp						= 0;
		if_fail_fef(fopen_s(fp, nameFileDst, "wb"), "\"%.*s\"", (int)nameFileDst.size(), nameFileDst.begin());
		if_null_fef(fp, "Failed to create file: \"%.*s\"", (int)nameFileDst.size(), nameFileDst.begin());

		fwrite(&fileHeader							, 1, szof(SPackHeader)			, fp);
		fwrite(compressedTableFiles		.begin	()	, 1, compressedTableFiles		.size()	, fp);
		fwrite(compressedContentsPacked	.begin	()	, 1, compressedContentsPacked	.size()	, fp);
		fclose(fp);
	}
	return 0;
}

stxp u2_t			DEFLATE_CHUNK_SIZE			= u2_t(1024) * 1024 * 4;
stxp u2_t			INFLATE_CHUNK_SIZE			= u2_t(1024) * 1024 * 4;
gpk::err_t			gpk::folderPack				(SFolderPackage & output, const vcst_t nameFolderSrc) {
	SPackHeader 			& fileHeader			= output.PackageInfo = {};
	// -- The following two arrays store the file table and the file contents that are going to be compressed and stored on disk
	au0_t					tableFiles				;
	au0_t					contentsPacked			;

	if_fail_fe(gpk::folderLoad(nameFolderSrc, tableFiles, contentsPacked));

	fileHeader.SizeUncompressedTableFiles		= tableFiles		.size();
	fileHeader.SizeUncompressedContentsPacked	= contentsPacked	.size();
	au0_t					& compressedTableFiles		= output.CompressedTableFiles		;
	au0_t					& compressedContentsPacked	= output.CompressedContentsPacked	;
	{	// compress
		if_fail_fef(gpk::arrayDeflate(tableFiles		, compressedTableFiles		, ::DEFLATE_CHUNK_SIZE), "%s", "Unknown error.");
		if_fail_fef(gpk::arrayDeflate(contentsPacked	, compressedContentsPacked	, ::DEFLATE_CHUNK_SIZE), "%s", "Unknown error.");
		fileHeader.SizeCompressedTableFiles		= compressedTableFiles		.size();
		fileHeader.SizeCompressedContentsPacked	= compressedContentsPacked	.size();
	}

	return 0;
}
gpk::err_t			gpk::folderUnpack			(SFolderInMemory & output, vcu0_t rawFileInMemory)		{
	const SPackHeader	& header					= *(SPackHeader*)&rawFileInMemory[0];
	output.Names	.resize(header.TotalFileCount);
	output.Contents	.resize(header.TotalFileCount);
	output.DataInfo		.clear();
	output.DataContents	.clear();
	if_fail_fe(gpk::arrayInflate({&rawFileInMemory[0] + szof(SPackHeader)									, header.SizeCompressedTableFiles		}, output.DataInfo	, ::INFLATE_CHUNK_SIZE));
	if_fail_fe(gpk::arrayInflate({&rawFileInMemory[0] + szof(SPackHeader) + header.SizeCompressedTableFiles, header.SizeCompressedContentsPacked	}, output.DataContents	, ::INFLATE_CHUNK_SIZE));
	{ // Build access tables.
		u2_t					offsetInfo					= 0;
		for(u2_t iFile = 0; iFile < output.Names.size(); ++iFile) {
			const rangeu2_t		& fileLocation				= *(const rangeu2_t*)&output.DataInfo[offsetInfo];
			offsetInfo				+= szof(rangeu2_t);
			u2_c				lenName						= *(u2_t*)&output.DataInfo[offsetInfo];
			offsetInfo				+= szof(u2_t);
			output.Names[iFile]	= {(const char*)&output.DataInfo[offsetInfo], lenName};
			offsetInfo				+= lenName;
			output.Contents[iFile]	= {&output.DataContents	[fileLocation.Offset], fileLocation.Count};
		}
	}
	return 0;
}
gpk::err_t			gpk::folderLoad				(vcst_t nameFolderSrc, au0_t & tableFiles, au0_t & contentsPacked) {
	// -- The following two arrays store the file table and the file contents that are going to be compressed and stored on disk
	u2_t					totalFileCount			= 0;
	{
		aobj<string>	listFiles				= {};
		if_fail_fef(gpk::pathList(nameFolderSrc, listFiles), "Failed to list folder: %.*s.", (int)nameFolderSrc.size(), nameFolderSrc.begin());

		au0_t				contentsTemp			= {};
		rangeu2_t			fileLocation			= {0, 0};
		for(u2_t iFile = 0; iFile < listFiles.size(); ++iFile) {
			fileLocation.Offset		= contentsPacked.size();
			cnst vcst_t			pathToLoad				= {listFiles[iFile].begin(), listFiles[iFile].size()};
			if(0 == pathToLoad.size())
				continue;

			info_printf("pathToLoad (%u): '%.*s'.", iFile, (int)pathToLoad.size(), pathToLoad.begin());
			if_fail_fef(gpk::fileToMemory(pathToLoad, contentsTemp), "Failed to load file: %.*s.", (int)pathToLoad.size(), pathToLoad.begin());

			fileLocation.Count		= contentsTemp.size();
			if_fail_fe(gpk::savePOD(tableFiles, fileLocation));
			if_fail_fe(gpk::saveView(tableFiles, pathToLoad));
			if_fail_fef(contentsPacked.append(contentsTemp.begin(), contentsTemp.size())	, "Failed to append data bytes. Buffer sizes:\ntableFiles     : %u.\ncontentsPacked : %u.", tableFiles.size(), contentsPacked.size());
			contentsTemp.clear();

			++totalFileCount;
		}
	}
	return totalFileCount;
}
gpk::err_t			gpk::folderToDisk			(const SFolderInMemory & virtualFolder, vcst_t destinationPath)				{
	string				finalPathName				= {};
	FILE						* fp						= 0;
	for(u2_t iFile = 0, countFiles = virtualFolder.Names.size(); iFile < countFiles; ++iFile) {
		gpk_safe_fclose(fp);
		const vcst_t			& fileName					= virtualFolder.Names		[iFile];
		vcu0_c			& fileContent				= virtualFolder.Contents	[iFile];
		finalPathName.clear();
		 if_fail_fe(append_strings(finalPathName, destinationPath, fileName));
		info_printf("File found (%u):'%s'. Size: %u.", iFile, finalPathName.begin(), fileContent.size());
		err_t				indexSlash					= findLastSlash(finalPathName);
		if(-1 != indexSlash) { // Create path if any specified.
			vcst_t				folderName					= {};
			if_fail_fe(finalPathName.slice(folderName, 0, (u2_t)indexSlash));
			if_fail_cef(pathCreate(folderName), "Failed to create folder: %.*s.", (int)folderName.size(), folderName.begin());
		}
		if_fail_fef(fopen_s(fp, finalPathName, "wb"), "%s", finalPathName.begin());
		if_null_cef(fp, "Failed to create file: %s.", finalPathName.begin());
		if_true_cef(fileContent.size() != fwrite(fileContent.begin(), 1, fileContent.size(), fp), "Failed to write file: %s. Disk full?", finalPathName.begin());
	}
	gpk_safe_fclose(fp);
	return 0;
}
gpk::err_t			gpk::folderPackToDisk		(vcst_t nameFileDst	 , vcst_t nameFolderSrc)		{
	SFolderPackage		folderPackage;
	if_fail_fef(gpk::folderPack(folderPackage, nameFolderSrc), "Failed to pack folder: %.*s.", (int)nameFolderSrc.size(), nameFolderSrc.begin());
	if_fail_fef(gpk::folderToDisk(folderPackage, nameFileDst), "Failed to pack folder: %.*s.", (int)nameFolderSrc.size(), nameFolderSrc.begin());
	return 0;
}
gpk::err_t			gpk::folderUnpackToDisk		(vcst_t namePathDst	 , vcst_t nameFileSrc)		{
	SFolderInMemory		virtualFolder				= {};
	if_fail_fef(gpk::folderUnpack(virtualFolder, nameFileSrc), "Failed to unpack file: %.*s.", (int)nameFileSrc.size(), nameFileSrc.begin());
	if_fail_fef(gpk::folderToDisk(virtualFolder, namePathDst), "Failed to write folder to disk. Disk full or insufficient permissions. File name: %.*s. Destination path: %.*s.", (int)nameFileSrc.size(), nameFileSrc.begin(), (int)namePathDst.size(), namePathDst.begin());
	return 0;
}
gpk::err_t			gpk::inflateToMemory  		(au0_t & tempCache	 , vcst_t fileName, au0_t & output) {
	if_fail_fe(gpk::fileToMemory(fileName, tempCache));
	info_printf("File size: %u.", tempCache.size());

	if_fail_fe(gpk::arrayInflate(tempCache, output));
	info_printf("Inflated size: %u.", output.size());
	return 0;
}
gpk::err_t			gpk::deflateFromMemory		(au0_t & tempCache	 , vcst_t fileName, vcu0_t input) {
	info_printf("Input size: %u.", input.size());
	if_fail_fe(gpk::arrayDeflate(input, tempCache));

	info_printf("Deflated size: %u.", tempCache.size());
	return fileFromMemory(fileName, tempCache);
}
gpk::err_t			gpk::fileFromMemorySecure	(SLoadCache & recycle, vcst_t fileName, vcu0_t key, b8_t deflate, vcu0_t blockBytes) {
	if(false == deflate && 0 == key.size())
		recycle.Encrypted		= blockBytes;
	else {
		if(false == deflate) {
			if_fail_fef(gpk::aesEncode(blockBytes, key, AES_LEVEL_256, recycle.Encrypted), "Failed to encrypt file: %.*s.", (int)fileName.size(), fileName.begin());
		}
		else if(0 == key.size()) {
			if_fail_fef(gpk::arrayDeflate(blockBytes, recycle.Encrypted), "Failed to deflate file: %.*s.", (int)fileName.size(), fileName.begin());
		}
		else {
			if_fail_fef(gpk::arrayDeflate(blockBytes, recycle.Deflated), "Failed to deflate file: %.*s.", (int)fileName.size(), fileName.begin());
			if_fail_fef(gpk::aesEncode(recycle.Deflated, key, AES_LEVEL_256, recycle.Encrypted), "Failed to encrypt file: %.*s.", (int)fileName.size(), fileName.begin());
		}
	}
	if_fail_fe(gpk::crcGenerateAndAppend(recycle.Encrypted));
	if_fail_fef(gpk::fileFromMemory(fileName, recycle.Encrypted), "Failed to save file: %.*s.", (int)fileName.size(), fileName.begin());
	return 0;
}
gpk::err_t			gpk::fileToMemorySecure		(SLoadCache & recycle, vcst_t fileName, vcu0_t key, const b8_t deflate, au0_t & loadedBytes)								{
	if(false == deflate && 0 == key.size()) {
		if_fail_fef(gpk::fileToMemory(fileName, loadedBytes), "Failed to read file: %.*s.", (int)fileName.size(), fileName.begin());
		if_fail_fe(gpk::crcVerifyAndRemove(loadedBytes));
	}
	else {
		if_fail_fef(gpk::fileToMemory(fileName, recycle.Encrypted), "Failed to read file: %.*s.", (int)fileName.size(), fileName.begin());
		if_fail_fe(gpk::crcVerifyAndRemove(recycle.Encrypted));
		if(false == deflate) {
			if_fail_fef(gpk::aesDecode(recycle.Encrypted, key, AES_LEVEL_256, loadedBytes), "Failed to decrypt file: %.*s.", (int)fileName.size(), fileName.begin());
		}
		else if(0 == key.size()) {
			if_fail_fef(gpk::arrayInflate(recycle.Encrypted, loadedBytes), "Failed to inflate file: %.*s.", (int)fileName.size(), fileName.begin());
		}
		else {
			if_fail_fef(gpk::aesDecode(recycle.Encrypted, key, AES_LEVEL_256, recycle.Deflated), "Failed to decrypt file: %.*s.", (int)fileName.size(), fileName.begin());
			if_fail_fef(gpk::arrayInflate(recycle.Deflated, loadedBytes), "Failed to inflate file: %.*s.", (int)fileName.size(), fileName.begin());
		}
	}
	return 0;
}
