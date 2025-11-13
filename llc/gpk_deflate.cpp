#include "gpk_deflate.h"

#include "gpk_path.h"

#include "gpk_file.h"
#include "gpk_safe.h"
#include "gpk_cstdio.h"
#include "gpk_aes.h"
#include "gpk_noise.h"
#include "gpk_apod_serialize.h"

#if defined(GPK_ESP32) || defined(GPK_ARDUINO)
#	ifdef GPK_ESP32
#	endif
#else
#	include "deflate.h"
#endif

#include "gpk_range.h"

GPK_USING_TYPEINT();

stxp	u2_c	GPK_CRC_CRC_SEED			= 18973;

::gpk::error_t			gpk::crcGenerate			(vcu0_c & bytes, uint64_t & crc)	{
	crc						= 0;
	u2_c				lastPos						= bytes.size() - 1;
	for(uint32_t i=0; i < bytes.size(); ++i) {
		crc						+= ::gpk::noise1DBase(bytes[i], ::GPK_CRC_CRC_SEED);
		crc						+= ::gpk::noise1DBase(bytes[lastPos - i], ::GPK_CRC_CRC_SEED);
	}
	return 0;
}

::gpk::error_t			gpk::crcGenerateAndAppend	(::gpk::au0_t & bytes)	{
	uint64_t					crcToStore					= 0;
	::gpk::crcGenerate(bytes, crcToStore);
	gpk_necs(bytes.append((const uint8_t*)&crcToStore, szof(uint64_t)));;
	return 0;
}

::gpk::error_t			gpk::crcVerifyAndRemove		(::gpk::au0_t & bytes)	{
	ree_if(bytes.size() < 8, "Invalid input. No CRC can be found in an array of %u bytes.", bytes.size());
	uint64_t					check						= 0;
	u2_c				startOfCRC					= bytes.size() - 8;
	::gpk::crcGenerate({bytes.begin(), startOfCRC}, check);
	const uint64_t				found						= *(uint64_t*)&bytes[startOfCRC];
	ree_if(check != found, "CRC Check failed: Stored: %llu. Calculated: %llu.", found, check);
	gpk_necs(bytes.resize(bytes.size() - 8));
	return 0;
}

::gpk::error_t			gpk::arrayDeflate		(vcu0_c & inflated, ::gpk::au0_t & deflated, u2_c chunkSize)	{
#if defined(GPK_ESP32) || defined(GPK_ARDUINO)
#	ifdef GPK_ESP32
	deflated				= inflated;
#	else
	deflated				= inflated;
#	endif
#else
	z_stream					strm					= {};
	int							ret						= deflateInit(&strm, Z_BEST_COMPRESSION);
	if (ret != Z_OK)
		return ret;

	strm.avail_in			= inflated.size();
	strm.next_in			= (Bytef*)inflated.begin();
	::gpk::au0_t					block;
	gpk_necs(block.resize(chunkSize));
	while(true) {
		strm.avail_out			= block.size();
		strm.next_out			= (Bytef*)block.begin();
		ret						= deflate(&strm, Z_FINISH);    // no bad return value
		bef_if(ret == Z_STREAM_ERROR, "Failed to compress: 0x%x.", ret);  // state not clobbered
		u2_c				deflatedSize			= (uint32_t)((uint8_t*)strm.next_out - block.begin());
		gpk_necs(deflated.append(block.begin(), deflatedSize));
		if(ret == Z_STREAM_END)
			break;
	}
	int							ret_end					= deflateEnd(&strm);
	ef_if(strm.avail_in != 0, "%s", "Not all of the input bytes were consumed.");	// all input will be used
	ree_if(ret < 0, "%s", "Unknown error");												// stream will be complete
	ef_if(ret != Z_STREAM_END && ret != Z_OK, "%s", "Unknown error");				// stream will be complete
	ree_if(ret_end == Z_STREAM_ERROR, "deflateEnd() returned %s", "Z_STREAM_ERROR");
	info_printf("deflateEnd: %u.", (uint32_t)ret);
#endif
	return 0;
}

::gpk::error_t			gpk::arrayInflate		(vcu0_c & deflated, ::gpk::au0_t & inflated, u2_c chunkSize)	{
#if defined(GPK_ESP32) || defined(GPK_ARDUINO)
#	ifdef GPK_ESP32
	inflated				= deflated;
#	else
	inflated				= deflated;
#	endif
#else
	z_stream					strm					= {};
	int							ret						= inflateInit(&strm);	 // allocate inflate state
	if (ret != Z_OK)
		return ret;

	strm.avail_in			= (uint32_t)deflated.size();
	strm.next_in			= (Bytef *)deflated.begin();
	::gpk::au0_t					block;
	gpk_necs(block.resize(chunkSize));
	while(true) {
		strm.avail_out			= (uint32_t)block.size();
		strm.next_out			= (Bytef *)block.begin();
		ret						= ::inflate(&strm, Z_NO_FLUSH);
		switch (ret) {
		case Z_NEED_DICT		:
			ret						= Z_DATA_ERROR;	// and fall through
		case Z_VERSION_ERROR	:
		case Z_STREAM_ERROR		:
		case Z_DATA_ERROR		:
		case Z_MEM_ERROR		:
			break;
		}
		ree_if(ret < 0, "Failed to decompress? inflate error: %i.", ret);
		u2_c				inflatedSize			= (uint32_t)((const uint8_t*)strm.next_out - block.begin());
		gpk_necs(inflated.append(block.begin(), inflatedSize));
		if(ret == Z_STREAM_END)
			break;
	}
	ret						= inflateEnd(&strm);
	ree_if(ret != Z_STREAM_END && ret != Z_OK, "Failed to decompress? inflateEnd error: %i.", ret);
#endif
	return 0;
}

::gpk::error_t			gpk::folderUnpack			(::gpk::SFolderInMemory & out_loaded, const ::gpk::vcst_t nameFileSrc)					{
	::gpk::au0_t					rawFileInMemory				= {};
	gpk_necall(gpk::fileToMemory(nameFileSrc, rawFileInMemory), "Failed to load pak file: %s.", nameFileSrc);
	gpk_necall(gpk::folderUnpack(out_loaded, rawFileInMemory), "Failed to unpack pak file: %s.", nameFileSrc);
	return 0;
}

::gpk::error_t			gpk::folderToDisk			(const ::gpk::SFolderPackage & folderPackage, const ::gpk::vcst_t nameFileDst)			{
	const ::gpk::SPackHeader 	& fileHeader				= folderPackage.PackageInfo;
	const ::gpk::au0_t			& compressedTableFiles		= folderPackage.CompressedTableFiles		;
	const ::gpk::au0_t			& compressedContentsPacked	= folderPackage.CompressedContentsPacked	;
	{
		FILE						* fp						= 0;
		gpk_necall(::gpk::fopen_s(&fp, nameFileDst, "wb"), "'%s'", nameFileDst.begin());
		ree_if(0 == fp, "Failed to create file: %s.", ::gpk::toString(nameFileDst).begin());
		fwrite(&fileHeader							, 1, szof(::gpk::SPackHeader)			, fp);
		fwrite(compressedTableFiles		.begin	()	, 1, compressedTableFiles		.size()	, fp);
		fwrite(compressedContentsPacked	.begin	()	, 1, compressedContentsPacked	.size()	, fp);
		fclose(fp);
	}
	return 0;
}

stxp	uint32_t		DEFLATE_CHUNK_SIZE			= uint32_t(1024) * 1024 * 4;
::gpk::error_t			gpk::folderPack				(::gpk::SFolderPackage & output, const ::gpk::vcst_t nameFolderSrc) {
	::gpk::SPackHeader 			& fileHeader			= output.PackageInfo = {};
	// -- The following two arrays store the file table and the file contents that are going to be compressed and stored on disk
	::gpk::asc_t					finalPathName			= {};
	finalPathName.resize(1024*8);

	::gpk::au0_t					tableFiles				;
	::gpk::au0_t					contentsPacked			;

	gpk_necs(gpk::folderLoad(nameFolderSrc, tableFiles, contentsPacked));



	fileHeader.SizeUncompressedTableFiles		= tableFiles		.size();
	fileHeader.SizeUncompressedContentsPacked	= contentsPacked	.size();
	::gpk::au0_t					& compressedTableFiles		= output.CompressedTableFiles		;
	::gpk::au0_t					& compressedContentsPacked	= output.CompressedContentsPacked	;
	{	// compress
		gpk_necall(gpk::arrayDeflate(tableFiles		, compressedTableFiles		, ::DEFLATE_CHUNK_SIZE), "%s", "Unknown error.");
		gpk_necall(gpk::arrayDeflate(contentsPacked	, compressedContentsPacked	, ::DEFLATE_CHUNK_SIZE), "%s", "Unknown error.");
		fileHeader.SizeCompressedTableFiles		= compressedTableFiles		.size();
		fileHeader.SizeCompressedContentsPacked	= compressedContentsPacked	.size();
	}

	return 0;
}

stxp	uint32_t		INFLATE_CHUNK_SIZE			= uint32_t(1024) * 1024 * 4;
::gpk::error_t			gpk::folderUnpack			(::gpk::SFolderInMemory & output, vcu0_c & rawFileInMemory)		{
	const ::gpk::SPackHeader	& header					= *(::gpk::SPackHeader*)&rawFileInMemory[0];
	output.Names	.resize(header.TotalFileCount);
	output.Contents	.resize(header.TotalFileCount);
	output.DataInfo		.clear();
	output.DataContents	.clear();
	gpk_necs(gpk::arrayInflate({&rawFileInMemory[0] + szof(::gpk::SPackHeader)									, header.SizeCompressedTableFiles		}, output.DataInfo	, ::INFLATE_CHUNK_SIZE));
	gpk_necs(gpk::arrayInflate({&rawFileInMemory[0] + szof(::gpk::SPackHeader) + header.SizeCompressedTableFiles, header.SizeCompressedContentsPacked	}, output.DataContents	, ::INFLATE_CHUNK_SIZE));
	{ // Build access tables.
		uint32_t					offsetInfo					= 0;
		for(uint32_t iFile = 0; iFile < output.Names.size(); ++iFile) {
			const ::gpk::rangeu2_t		& fileLocation				= *(const ::gpk::rangeu2_t*)&output.DataInfo[offsetInfo];
			offsetInfo				+= szof(::gpk::rangeu2_t);
			u2_c				lenName						= *(uint32_t*)&output.DataInfo[offsetInfo];
			offsetInfo				+= szof(uint32_t);
			output.Names[iFile]	= {(const char*)&output.DataInfo[offsetInfo], lenName};
			offsetInfo				+= lenName;
			output.Contents[iFile]	= {&output.DataContents	[fileLocation.Offset], fileLocation.Count};
		}
	}
	return 0;
}

::gpk::error_t			gpk::folderLoad			
	( const ::gpk::vcst_t	nameFolderSrc 
	, ::gpk::au0_t		& tableFiles				
	, ::gpk::au0_t		& contentsPacked			
	) {
	// -- The following two arrays store the file table and the file contents that are going to be compressed and stored on disk
	uint32_t					totalFileCount			= 0;
	{
		::gpk::aobj<::gpk::asc_t>		listFiles				= {};
		gpk_necall(gpk::pathList(nameFolderSrc, listFiles), "Failed to list folder: %s.", nameFolderSrc.begin());

		::gpk::au0_t					contentsTemp			= {};
		::gpk::rangeu2_t			fileLocation			= {0, 0};
		for(uint32_t iFile = 0; iFile < listFiles.size(); ++iFile) {
			fileLocation.Offset		= contentsPacked.size();
			const ::gpk::vcst_t			pathToLoad				= {listFiles[iFile].begin(), listFiles[iFile].size()};
			if(0 == pathToLoad.size())
				continue;

			info_printf("pathToLoad (%u): '%s'.", iFile, ::gpk::toString(pathToLoad).begin());
			gpk_necall(gpk::fileToMemory(pathToLoad, contentsTemp), "Failed to load file: %s.", ::gpk::toString(pathToLoad).begin());

			fileLocation.Count		= contentsTemp.size();
			gpk_necs(gpk::savePOD(tableFiles, fileLocation));
			gpk_necs(gpk::saveView(tableFiles, pathToLoad));
			gpk_necall(contentsPacked.append(contentsTemp.begin(), contentsTemp.size())	, "Failed to append data bytes. Buffer sizes:\ntableFiles     : %u.\ncontentsPacked : %u.", tableFiles.size(), contentsPacked.size());
			contentsTemp.clear();

			++totalFileCount;
		}
	}
	return totalFileCount;
}

// Write folder to disk.
::gpk::error_t			gpk::folderToDisk			(const ::gpk::SFolderInMemory & virtualFolder, ::gpk::vcst_t destinationPath)				{
	char						bufferFormat	[32]		= {};
	::gpk::asc_t					finalPathName				= {};
	finalPathName.resize(8 * 1024);
	finalPathName.fill(0);
	FILE						* fp						= 0;
	for(uint32_t iFile = 0, countFiles = virtualFolder.Names.size(); iFile < countFiles; ++iFile) {
		gpk_safe_fclose(fp);
		const ::gpk::vcst_t			& fileName					= virtualFolder.Names		[iFile];
		vcu0_c			& fileContent				= virtualFolder.Contents	[iFile];
		sprintf_s(bufferFormat, "%%.%us%%.%us", destinationPath.size(), fileName.size());
		snprintf(finalPathName.begin(), finalPathName.size(), bufferFormat, destinationPath.begin(), fileName.begin());
		info_printf("File found (%u):'%s'. Size: %u.", iFile, finalPathName.begin(), fileContent.size());
		uint32_t					lenPath						= (uint32_t)strlen(finalPathName.begin());
		::gpk::error_t				indexSlash					= ::gpk::findLastSlash(::gpk::vcst_t{finalPathName.begin(), uint32_t(-1)});
		if(-1 != indexSlash) { // Create path if any specified.
			finalPathName[indexSlash]	= 0;
			lenPath						= (uint32_t)strlen(finalPathName.begin());
 			if_fail_cef(::gpk::pathCreate({finalPathName.begin(), lenPath}), "Failed to create foder: %s.", finalPathName.begin());
			finalPathName[indexSlash]	= '/';
		}
		gpk_necall(::gpk::fopen_s(&fp, finalPathName, "wb"), "%s", finalPathName.begin());
		cef_if(0 == fp, "Failed to create file: %s.", finalPathName.begin());
		cef_if(fileContent.size() != fwrite(fileContent.begin(), 1, fileContent.size(), fp), "Failed to write file: %s. Disk full?", finalPathName.begin());
	}
	gpk_safe_fclose(fp);
	return 0;
}

::gpk::error_t			gpk::folderPackToDisk		(const ::gpk::vcst_t nameFileDst,	const ::gpk::vcst_t nameFolderSrc)		{
	::gpk::SFolderPackage		folderPackage;
	gpk_necall(gpk::folderPack(folderPackage, nameFolderSrc), "Failed to pack folder: %s.", nameFolderSrc.begin());
	gpk_necall(gpk::folderToDisk(folderPackage, nameFileDst), "Failed to pack folder: %s.", nameFolderSrc.begin());
	return 0;
}

::gpk::error_t			gpk::folderUnpackToDisk		(const ::gpk::vcst_t namePathDst, const ::gpk::vcst_t nameFileSrc)		{
	::gpk::SFolderInMemory		virtualFolder				= {};
	gpk_necall(gpk::folderUnpack(virtualFolder, nameFileSrc), "Failed to unpack file: %s.", nameFileSrc);
	gpk_necall(gpk::folderToDisk(virtualFolder, namePathDst), "Failed to write folder to disk. Disk full or insufficient permissions. File name: %s. Destionation Path: %s.", nameFileSrc, namePathDst);
	return 0;
}

::gpk::error_t			gpk::inflateToMemory  		(::gpk::au0_t & tempCache, ::gpk::vcsc_c & fileName, ::gpk::au0_t & output) {
	gpk_necs(gpk::fileToMemory(fileName, tempCache));
	info_printf("File size: %u.", tempCache.size());

	gpk_necs(gpk::arrayInflate(tempCache, output));
	info_printf("Inflated size: %u.", output.size());
	return 0; 
}

::gpk::error_t			gpk::deflateFromMemory		(::gpk::au0_t & tempCache, ::gpk::vcsc_c & fileName, vcu0_c & input) {
	info_printf("Input size: %u.", input.size());
	gpk_necs(gpk::arrayDeflate(input, tempCache));

	info_printf("Deflated size: %u.", tempCache.size());
	return ::gpk::fileFromMemory(fileName, tempCache);
}

::gpk::error_t			gpk::fileFromMemorySecure	(::gpk::SLoadCache & recycle, ::gpk::vcsc_c & fileName, vcu0_c & key, const bool deflate, vcu0_c & blockBytes) {
	if(false == deflate && 0 == key.size())
		recycle.Encrypted		= blockBytes;
	else {
		if(false == deflate)
			gpk_necall(gpk::aesEncode(blockBytes, key, ::gpk::AES_LEVEL_256, recycle.Encrypted), "Failed to encrypt file: %s.", ::gpk::toString(fileName).begin());
		else if(0 == key.size())
			gpk_necall(gpk::arrayDeflate(blockBytes, recycle.Encrypted), "Failed to deflate file: %s.", ::gpk::toString(fileName).begin());
		else {
			gpk_necall(gpk::arrayDeflate(blockBytes, recycle.Deflated), "Failed to deflate file: %s.", ::gpk::toString(fileName).begin());
			gpk_necall(gpk::aesEncode(recycle.Deflated, key, ::gpk::AES_LEVEL_256, recycle.Encrypted), "Failed to encrypt file: %s.", ::gpk::toString(fileName).begin());
		}
	}
	gpk_necall(gpk::crcGenerateAndAppend(recycle.Encrypted), "%s", "CRC Check failed!");
	gpk_necall(gpk::fileFromMemory(fileName, recycle.Encrypted), "Failed to save file: %s.", ::gpk::toString(fileName).begin());
	return 0;
}

::gpk::error_t			gpk::fileToMemorySecure		(::gpk::SLoadCache & recycle, ::gpk::vcsc_c & fileName, vcu0_c & key, const bool deflate, ::gpk::au0_t & loadedBytes)								{
	::gpk::vcst_t					strFilename					= {fileName.begin(), fileName.size()};
	if(false == deflate && 0 == key.size()) {
		gpk_necall(gpk::fileToMemory(strFilename, loadedBytes), "Failed to read file: %s.", ::gpk::toString(fileName).begin());
		gpk_necs(gpk::crcVerifyAndRemove(loadedBytes));
	}
	else {
		gpk_necall(gpk::fileToMemory(strFilename, recycle.Encrypted), "Failed to read file: %s.", ::gpk::toString(fileName).begin());
		gpk_necall(gpk::crcVerifyAndRemove(recycle.Encrypted), "%s", "CRC Check failed!");
		if(false == deflate)
			gpk_necall(gpk::aesDecode(recycle.Encrypted, key, ::gpk::AES_LEVEL_256, loadedBytes), "Failed to decrypt file: %s.", ::gpk::toString(fileName).begin());
		else if(0 == key.size())
			gpk_necall(gpk::arrayInflate(recycle.Encrypted, loadedBytes), "Failed to inflate file: %s.", ::gpk::toString(fileName).begin());
		else {
			gpk_necall(gpk::aesDecode(recycle.Encrypted, key, ::gpk::AES_LEVEL_256, recycle.Deflated), "Failed to decrypt file: %s.", ::gpk::toString(fileName).begin());
			gpk_necall(gpk::arrayInflate(recycle.Deflated, loadedBytes), "Failed to inflate file: %s.", ::gpk::toString(fileName).begin());
		}
	}
	return 0;
}
