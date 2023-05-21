#include "gpk_deflate.h"
#include "gpk_storage.h"
#include "gpk_coord.h"
#include "gpk_safe.h"
#include "deflate.h"
#include "gpk_io.h"
#include "gpk_aes.h"
#include "gpk_noise.h"
#include "gpk_apod_serialize.h"

stacxpr	const uint32_t		GPK_CRC_CRC_SEED			= 18973;

::gpk::error_t				gpk::crcGenerate			(const ::gpk::vcu8 & bytes, uint64_t & crc)	{
	crc							= 0;
	const uint32_t					lastPos						= bytes.size() - 1;
	for(uint32_t i=0; i < bytes.size(); ++i) {
		crc							+= ::gpk::noise1DBase(bytes[i], ::GPK_CRC_CRC_SEED);
		crc							+= ::gpk::noise1DBase(bytes[lastPos - i], ::GPK_CRC_CRC_SEED);
	}
	return 0;
}

::gpk::error_t				gpk::crcGenerateAndAppend	(::gpk::au8 & bytes)	{
	uint64_t						crcToStore					= 0;
	::gpk::crcGenerate(bytes, crcToStore);
	gpk_necall(bytes.append((const uint8_t*)&crcToStore, sizeof(uint64_t)), "%s", "Out of memory?");;
	return 0;
}

::gpk::error_t				gpk::crcVerifyAndRemove		(::gpk::au8 & bytes)	{
	ree_if(bytes.size() < 8, "Invalid input. No CRC can be found in an array of %u bytes.", bytes.size());
	uint64_t						check						= 0;
	const uint32_t					startOfCRC					= bytes.size() - 8;
	::gpk::crcGenerate({bytes.begin(), startOfCRC}, check);
	const uint64_t					found						= *(uint64_t*)&bytes[startOfCRC];
	ree_if(check != found, "CRC Check failed: Stored: %llu. Calculated: %llu.", found, check);
	gpk_necall(bytes.resize(bytes.size() - 8), "%s", "Out of memory?");
	return 0;
}

::gpk::error_t				gpk::arrayDeflate		(const ::gpk::vcu8 & inflated, ::gpk::au8 & deflated, const uint32_t chunkSize)	{
	z_stream						strm					= {};
	int								ret						= deflateInit(&strm, Z_BEST_COMPRESSION);
	if (ret != Z_OK)
		return ret;

	strm.avail_in				= inflated.size();
	strm.next_in				= (Bytef*)inflated.begin();
	::gpk::au8						block;
	gpk_necall(block.resize(chunkSize), "%s", "Out of memory?");
	while(true) {
		strm.avail_out				= block.size();
		strm.next_out				= (Bytef*)block.begin();
		ret							= deflate(&strm, Z_FINISH);    // no bad return value
		be_if(ret == Z_STREAM_ERROR, "Failed to compress: 0x%x.", ret);  // state not clobbered
		const uint32_t					deflatedSize			= (uint32_t)((uint8_t*)strm.next_out - block.begin());
		gpk_necall(deflated.append(block.begin(), deflatedSize), "%s", "Out of memory?");
		if(ret == Z_STREAM_END)
			break;
	}
	int								ret_end					= deflateEnd(&strm);
	gerror_if(strm.avail_in != 0, "%s", "Not all of the input bytes were consumed.");	/* all input will be used */
	ree_if(ret < 0, "%s", "Unknown error");				/* stream will be complete */
	gerror_if(ret != Z_STREAM_END && ret != Z_OK, "%s", "Unknown error");				/* stream will be complete */
	ree_if(ret_end == Z_STREAM_ERROR, "deflateEnd() returned %s", "Z_STREAM_ERROR")
	info_printf("deflateEnd: %u.", (uint32_t)ret);
	return 0;
}

::gpk::error_t				gpk::arrayInflate		(const ::gpk::vcu8 & deflated, ::gpk::au8 & inflated, const uint32_t chunkSize)	{
	z_stream						strm					= {};
	int								ret						= inflateInit(&strm);	 // allocate inflate state
	if (ret != Z_OK)
		return ret;

	strm.avail_in				= (uint32_t)deflated.size();
	strm.next_in				= (Bytef *)deflated.begin();
	::gpk::au8						block;
	gpk_necall(block.resize(chunkSize), "%s", "Out of memory?");
	while(true) {
		strm.avail_out				= (uint32_t)block.size();
		strm.next_out				= (Bytef *)block.begin();
		ret							= ::inflate(&strm, Z_NO_FLUSH);
		switch (ret) {
		case Z_NEED_DICT		:
			ret							= Z_DATA_ERROR;	// and fall through
		case Z_VERSION_ERROR	:
		case Z_STREAM_ERROR		:
		case Z_DATA_ERROR		:
		case Z_MEM_ERROR		:
			break;
		}
		ree_if(ret < 0, "Failed to decompress? inflate error: %i.", ret);
		const uint32_t						inflatedSize			= (uint32_t)((uint8_t*)strm.next_out - block.begin());
		gpk_necall(inflated.append(block.begin(), inflatedSize), "%s", "Out of memory?");
		if(ret == Z_STREAM_END)
			break;
	}
	ret								= inflateEnd(&strm);
	ree_if(ret != Z_STREAM_END && ret != Z_OK, "Failed to decompress? inflateEnd error: %i.", ret);
	return 0;
}

stacxpr	const uint32_t		DEFLATE_CHUNK_SIZE		= 1024 * 1024 * 32;
stacxpr	const uint32_t		INFLATE_CHUNK_SIZE		= 1024 * 1024 * 32;

::gpk::error_t				gpk::folderUnpack			(::gpk::SFolderInMemory & out_loaded, const ::gpk::vcs nameFileSrc)					{
	::gpk::au8			rawFileInMemory				= {};
	gpk_necall(::gpk::fileToMemory(nameFileSrc, rawFileInMemory), "Failed to load pak file: %s.", nameFileSrc);
	gpk_necall(::gpk::folderUnpack(out_loaded, rawFileInMemory), "Failed to unpack pak file: %s.", nameFileSrc);
	return 0;
}

::gpk::error_t				gpk::folderToDisk			(const ::gpk::SFolderPackage & folderPackage, const ::gpk::vcs nameFileDst)			{
	const ::gpk::SPackHeader 		& fileHeader				= folderPackage.PackageInfo;
	const ::gpk::au8				& compressedTableFiles		= folderPackage.CompressedTableFiles		;
	const ::gpk::au8				& compressedContentsPacked	= folderPackage.CompressedContentsPacked	;
	{
		FILE							* fp						= 0;
		fopen_s(&fp, ::gpk::toString(nameFileDst).begin(), "wb");
		ree_if(0 == fp, "Failed to create file: %s.", ::gpk::toString(nameFileDst).begin());
		fwrite(&fileHeader							, 1, sizeof(::gpk::SPackHeader)			, fp);
		fwrite(compressedTableFiles		.begin	()	, 1, compressedTableFiles		.size()	, fp);
		fwrite(compressedContentsPacked	.begin	()	, 1, compressedContentsPacked	.size()	, fp);
		fclose(fp);
	}
	return 0;
}

::gpk::error_t				gpk::folderPack			(::gpk::SFolderPackage & out_packed, const ::gpk::vcs nameFolderSrc) {
	::gpk::SPackHeader 				& fileHeader			= out_packed.PackageInfo = {};
	// -- The following two arrays store the file table and the file contents that are going to be compressed and stored on disk
	::gpk::ac						finalPathName			= {};
	finalPathName.resize(1024*8);

	::gpk::au8						tableFiles				;
	::gpk::au8						contentsPacked			;

	gpk_necs(::gpk::folderLoad(nameFolderSrc, tableFiles, contentsPacked));

	fileHeader.SizeUncompressedTableFiles		= tableFiles		.size();
	fileHeader.SizeUncompressedContentsPacked	= contentsPacked	.size();
	::gpk::au8						& compressedTableFiles		= out_packed.CompressedTableFiles		;
	::gpk::au8						& compressedContentsPacked	= out_packed.CompressedContentsPacked	;
	{	// compress
		gpk_necall(::gpk::arrayDeflate(tableFiles		, compressedTableFiles		, ::DEFLATE_CHUNK_SIZE), "%s", "Unknown error.");
		gpk_necall(::gpk::arrayDeflate(contentsPacked	, compressedContentsPacked	, ::DEFLATE_CHUNK_SIZE), "%s", "Unknown error.");
		fileHeader.SizeCompressedTableFiles		= compressedTableFiles		.size();
		fileHeader.SizeCompressedContentsPacked	= compressedContentsPacked	.size();
	}

	return 0;
}

::gpk::error_t				gpk::folderUnpack			(::gpk::SFolderInMemory & out_loaded, const ::gpk::vcu8 & rawFileInMemory)		{
	const ::gpk::SPackHeader		& header					= *(::gpk::SPackHeader*)&rawFileInMemory[0];
	out_loaded.Names	.resize(header.TotalFileCount);
	out_loaded.Contents	.resize(header.TotalFileCount);
	out_loaded.DataInfo		.clear();
	out_loaded.DataContents	.clear();
	gpk_necs(::gpk::arrayInflate({&rawFileInMemory[0] + sizeof(::gpk::SPackHeader)									, header.SizeCompressedTableFiles		}, out_loaded.DataInfo		, ::INFLATE_CHUNK_SIZE));
	gpk_necs(::gpk::arrayInflate({&rawFileInMemory[0] + sizeof(::gpk::SPackHeader) + header.SizeCompressedTableFiles, header.SizeCompressedContentsPacked	}, out_loaded.DataContents	, ::INFLATE_CHUNK_SIZE));
	{ // Build access tables.
		uint32_t						offsetInfo					= 0;
		for(uint32_t iFile = 0; iFile < out_loaded.Names.size(); ++iFile) {
			const ::gpk::SRange<uint32_t>	& fileLocation				= *(const ::gpk::SRange<uint32_t>*)&out_loaded.DataInfo[offsetInfo];
			offsetInfo					+= sizeof(const ::gpk::SRange<uint32_t>);
			const uint32_t					lenName						= *(uint32_t*)&out_loaded.DataInfo		[offsetInfo];
			offsetInfo					+= sizeof(uint32_t);
			out_loaded.Names	[iFile]	= {(const char*)&out_loaded.DataInfo[offsetInfo], lenName};
			offsetInfo					+= lenName;
			out_loaded.Contents	[iFile]	= {&out_loaded.DataContents	[fileLocation.Offset], fileLocation.Count};
		}
	}
	return 0;
}

::gpk::error_t				gpk::folderLoad			
	( const ::gpk::vcs	nameFolderSrc 
	, ::gpk::au8		& tableFiles				
	, ::gpk::au8		& contentsPacked			
	) {
	// -- The following two arrays store the file table and the file contents that are going to be compressed and stored on disk
	uint32_t						totalFileCount			= 0;
	{
		::gpk::aobj<::gpk::ac>			listFiles				= {};
		gpk_necall(::gpk::pathList(nameFolderSrc, listFiles), "Failed to list folder: %s.", nameFolderSrc.begin());

		::gpk::au8						contentsTemp			= {};
		::gpk::SRange<uint32_t>			fileLocation			= {0, 0};
		for(uint32_t iFile = 0; iFile < listFiles.size(); ++iFile) {
			fileLocation.Offset			= contentsPacked.size();
			const ::gpk::vcs				pathToLoad				= {listFiles[iFile].begin(), listFiles[iFile].size()};
			if(0 == pathToLoad.size())
				continue;

			info_printf("pathToLoad (%u): '%s'.", iFile, ::gpk::toString(pathToLoad).begin());
			gpk_necall(::gpk::fileToMemory(pathToLoad, contentsTemp), "Failed to load file: %s.", ::gpk::toString(pathToLoad).begin());

			fileLocation.Count			= contentsTemp.size();
			gpk_necs(::gpk::savePOD(tableFiles, fileLocation));
			gpk_necs(::gpk::saveView(tableFiles, pathToLoad));
			gpk_necall(contentsPacked.append(contentsTemp.begin(), contentsTemp.size())	, "Failed to append data bytes. Buffer sizes:\ntableFiles     : %u.\ncontentsPacked : %u.", tableFiles.size(), contentsPacked.size());
			contentsTemp.clear();

			++totalFileCount;
		}
	}
	return totalFileCount;
}

// Write folder to disk.
::gpk::error_t				gpk::folderToDisk			(const ::gpk::SFolderInMemory & virtualFolder, ::gpk::vcs destinationPath)				{
	::gpk::ac						bufferFormat				= {};
	::gpk::ac						finalPathName				= {};
	finalPathName.resize(1024*8);
	bufferFormat.resize(64);
	memset(finalPathName.begin(), 0, finalPathName.size());
	FILE							* fp						= 0;
	for(uint32_t iFile = 0, countFiles = virtualFolder.Names.size(); iFile < countFiles; ++iFile) {
		gpk_safe_fclose(fp);
		const ::gpk::vcs				& fileName					= virtualFolder.Names		[iFile];
		const ::gpk::vcu8				& fileContent				= virtualFolder.Contents	[iFile];
		snprintf(bufferFormat.begin(), bufferFormat.size(), "%%.%us%%.%us", destinationPath.size(), fileName.size());
		snprintf(finalPathName.begin(), finalPathName.size(), bufferFormat.begin(), destinationPath.begin(), fileName.begin());
		info_printf("File found (%u): %s. Size: %u.", iFile, finalPathName.begin(), fileContent.size());
		uint32_t						lenPath						= (uint32_t)strlen(finalPathName.begin());
		::gpk::error_t					indexSlash					= ::gpk::findLastSlash(::gpk::vcs{finalPathName.begin(), uint32_t(-1)});
		if(-1 != indexSlash) { // Create path if any specified.
			finalPathName[indexSlash]	= 0;
			lenPath						= (uint32_t)strlen(finalPathName.begin());
 			ce_if(errored(::gpk::pathCreate({finalPathName.begin(), lenPath})), "Failed to create foder: %s.", finalPathName.begin());
			finalPathName[indexSlash]	= '/';
		}
		fopen_s(&fp, finalPathName.begin(), "wb");
		ce_if(0 == fp, "Failed to create file: %s.", finalPathName.begin());
		ce_if(fileContent.size() != fwrite(fileContent.begin(), 1, fileContent.size(), fp), "Failed to write file: %s. Disk full?", finalPathName.begin());
	}
	gpk_safe_fclose(fp);
	return 0;
}

::gpk::error_t				gpk::folderPackToDisk		(const ::gpk::vcs nameFileDst,	const ::gpk::vcs nameFolderSrc)		{
	::gpk::SFolderPackage			folderPackage;
	gpk_necall(::gpk::folderPack(folderPackage, nameFolderSrc), "Failed to pack folder: %s.", nameFolderSrc.begin());
	gpk_necall(::gpk::folderToDisk(folderPackage, nameFileDst), "Failed to pack folder: %s.", nameFolderSrc.begin());
	return 0;
}

::gpk::error_t				gpk::folderUnpackToDisk		(const ::gpk::vcs namePathDst, const ::gpk::vcs nameFileSrc)		{
	::gpk::SFolderInMemory			virtualFolder				= {};
	gpk_necall(::gpk::folderUnpack(virtualFolder, nameFileSrc), "Failed to unpack file: %s.", nameFileSrc);
	gpk_necall(::gpk::folderToDisk(virtualFolder, namePathDst), "Failed to write folder to disk. Disk full or insufficient permissions. File name: %s. Destionation Path: %s.", nameFileSrc, namePathDst);
	return 0;
}

::gpk::error_t				gpk::fileFromMemorySecure	(::gpk::SLoadCache & recycle, const ::gpk::vcu8 & blockBytes, const ::gpk::vcc & fileName, const ::gpk::vcu8 & key, const bool deflate) {
	if(false == deflate && 0 == key.size())
		recycle.Encrypted			= blockBytes;
	else {
		if(false == deflate)
			gpk_necall(::gpk::aesEncode(blockBytes, key, ::gpk::AES_LEVEL_256, recycle.Encrypted), "Failed to encrypt file: %s.", ::gpk::toString(fileName).begin());
		else if(0 == key.size())
			gpk_necall(::gpk::arrayDeflate(blockBytes, recycle.Encrypted), "Failed to deflate file: %s.", ::gpk::toString(fileName).begin());
		else {
			gpk_necall(::gpk::arrayDeflate(blockBytes, recycle.Deflated), "Failed to deflate file: %s.", ::gpk::toString(fileName).begin());
			gpk_necall(::gpk::aesEncode(recycle.Deflated, key, ::gpk::AES_LEVEL_256, recycle.Encrypted), "Failed to encrypt file: %s.", ::gpk::toString(fileName).begin());
		}
	}
	gpk_necall(::gpk::crcGenerateAndAppend(recycle.Encrypted), "%s", "CRC Check failed!");
	gpk_necall(::gpk::fileFromMemory(fileName, recycle.Encrypted), "Failed to save file: %s.", ::gpk::toString(fileName).begin());
	return 0;
}

::gpk::error_t				gpk::fileToMemorySecure		(::gpk::SLoadCache & recycle, ::gpk::au8 & loadedBytes, const ::gpk::vcc & fileName, const ::gpk::vcu8 & key, const bool deflate)								{
	::gpk::vcs						strFilename					= {fileName.begin(), fileName.size()};
	if(false == deflate && 0 == key.size()) {
		gpk_necall(::gpk::fileToMemory(strFilename, loadedBytes), "Failed to read file: %s.", ::gpk::toString(fileName).begin());
		gpk_necs(::gpk::crcVerifyAndRemove(loadedBytes));
	}
	else {
		gpk_necall(::gpk::fileToMemory(strFilename, recycle.Encrypted), "Failed to read file: %s.", ::gpk::toString(fileName).begin());
		gpk_necall(::gpk::crcVerifyAndRemove(recycle.Encrypted), "%s", "CRC Check failed!");
		if(false == deflate)
			gpk_necall(::gpk::aesDecode(recycle.Encrypted, key, ::gpk::AES_LEVEL_256, loadedBytes), "Failed to decrypt file: %s.", ::gpk::toString(fileName).begin());
		else if(0 == key.size())
			gpk_necall(::gpk::arrayInflate(recycle.Encrypted, loadedBytes), "Failed to inflate file: %s.", ::gpk::toString(fileName).begin());
		else {
			gpk_necall(::gpk::aesDecode(recycle.Encrypted, key, ::gpk::AES_LEVEL_256, recycle.Deflated), "Failed to decrypt file: %s.", ::gpk::toString(fileName).begin());
			gpk_necall(::gpk::arrayInflate(recycle.Deflated, loadedBytes), "Failed to inflate file: %s.", ::gpk::toString(fileName).begin());
		}
	}
	return 0;
}
