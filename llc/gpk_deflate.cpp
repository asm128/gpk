#include "gpk_deflate.h"
#include "gpk_storage.h"
#include "gpk_coord.h"
#include "gpk_safe.h"
#include "deflate.h"
#include "gpk_find.h"
#include "gpk_io.h"

		::gpk::error_t									gpk::arrayDeflate								(const ::gpk::view_const_byte	& inflated, ::gpk::array_pod<byte_t>& deflated, const uint32_t chunkSize)	{
    int															ret;																												  
	z_stream													strm											= {};
    ret																											= deflateInit(&strm, Z_BEST_COMPRESSION);
    if (ret != Z_OK)
        return ret;
    strm.avail_in											= inflated.size();
    strm.next_in											= (Bytef*)inflated.begin();
	::gpk::array_pod<byte_t>									block;
	block.resize(chunkSize);
	while(true) {
		strm.avail_out											= block.size();
		strm.next_out											= (Bytef*)block.begin();
		ret														= deflate(&strm, Z_FINISH);    // no bad return value 
		be_if(ret == Z_STREAM_ERROR, "Failed to compress: 0x%x.", ret);  // state not clobbered 
		const uint32_t												deflatedSize									= (uint32_t)((byte_t*)strm.next_out - block.begin());
		deflated.append(block.begin(), deflatedSize);
		if(ret == Z_STREAM_END)
			break;
	}
	int															ret_end											= deflateEnd(&strm);
	gerror_if(strm.avail_in != 0, "%s", "Not all of the input bytes were consumed.");	/* all input will be used */
	ree_if(ret < 0, "%s", "Unknown error");				/* stream will be complete */
	gerror_if(ret != Z_STREAM_END && ret != Z_OK, "%s", "Unknown error");				/* stream will be complete */
	ree_if(ret_end == Z_STREAM_ERROR, "deflateEnd() returned %s", "Z_STREAM_ERROR")
	info_printf("deflateEnd: %u.", (uint32_t)ret);
	return 0;
}

		::gpk::error_t									gpk::arrayInflate								(const ::gpk::view_const_byte & deflated, ::gpk::array_pod<byte_t>& inflated, const uint32_t chunkSize)	{
	z_stream													strm											= {};
	int															ret												= inflateInit(&strm);	 // allocate inflate state
	if (ret != Z_OK)
		return ret;

	strm.avail_in											= (uint32_t)deflated.size();
	strm.next_in											= (Bytef *)deflated.begin();
	::gpk::array_pod<byte_t>									block;
	block.resize(chunkSize);
	while(true) {
		strm.avail_out											= (uint32_t)block.size();
		strm.next_out											= (Bytef *)block.begin();
		ret														= ::inflate(&strm, Z_NO_FLUSH);
		switch (ret) {
		case Z_NEED_DICT		:
			ret														= Z_DATA_ERROR;     // and fall through
		case Z_VERSION_ERROR	:
		case Z_STREAM_ERROR		:
		case Z_DATA_ERROR		:
		case Z_MEM_ERROR		:
			break;
		}
		ree_if(ret < 0, "Failed to decompress? inflate error: %i.", ret);
		const uint32_t												inflatedSize									= (uint32_t)((byte_t*)strm.next_out - block.begin());
		inflated.append(block.begin(), inflatedSize);
		if(ret == Z_STREAM_END)
			break;
	}
	ret														= inflateEnd(&strm);
	ree_if(ret != Z_STREAM_END && ret != Z_OK, "Failed to decompress? inflateEnd error: %i.", ret);
	return 0;
}

static constexpr const uint32_t				FOLDERPACK_DEFLATE_CHUNK_SIZE	= 1024 * 1024 * 32;
static constexpr const uint32_t				FOLDERPACK_INFLATE_CHUNK_SIZE	= 1024 * 1024 * 32;

::gpk::error_t								gpk::folderPack				(::gpk::SFolderPackage& out_packed, const ::gpk::view_const_string	nameFolderSrc) {
	::gpk::SPackHeader 								& fileHeader				= out_packed.PackageInfo;
	// -- The following two arrays store the file table and the file contents that are going to be compressed and stored on disk
	::gpk::array_pod<byte_t>						tableFiles					= {};
	::gpk::array_pod<byte_t>						contentsPacked				= {};
	::gpk::array_pod<char_t>						bufferFormat				= {};
	::gpk::array_pod<char_t>						finalPathName				= {};
	finalPathName.resize(1024*8);
	bufferFormat.resize(64);
	{
		::gpk::array_obj<::gpk::array_pod<char_t>>		listFiles				= {};
		gpk_necall(::gpk::pathList(nameFolderSrc, listFiles), "Failed to list folder: %s.", nameFolderSrc.begin());
		::gpk::array_pod<byte_t>						contentsTemp			= {};
		::gpk::SRange<uint32_t>							fileLocation			= {0, 0};
		for(uint32_t iFile = 0; iFile < listFiles.size(); ++iFile) {
			fileLocation.Offset							= contentsPacked.size();
			const ::gpk::view_const_string					pathToLoad				= {listFiles[iFile].begin(), listFiles[iFile].size()};
			if(0 == pathToLoad.size())
				continue;

			sprintf_s(bufferFormat.begin(), bufferFormat.size(), "%%.%us", pathToLoad.size());
			uint32_t lenFinalPath = (uint32_t)sprintf_s(finalPathName.begin(), finalPathName.size(), bufferFormat.begin(), pathToLoad.begin());
			info_printf("pathToLoad (%u): '%s'.", iFile, finalPathName.begin());
			gpk_necall(::gpk::fileToMemory({finalPathName.begin(), lenFinalPath}, contentsTemp), "Failed to load file: %s. Out of memory?", pathToLoad.begin());

			fileLocation.Count							= contentsTemp.size();
			gpk_necall(tableFiles.append((const byte_t*)&fileLocation, sizeof(::gpk::SRange<uint32_t>)), "Failed to append data! %s.", "Out of memory?");
			uint32_t										pathLen					= pathToLoad.size();
			gpk_necall(tableFiles		.append((const byte_t*)&pathLen, sizeof(uint32_t))	, "Failed to append data bytes. Buffer sizes:\ntableFiles     : %u.\ncontentsPacked : %u.", tableFiles.size(), contentsPacked.size());
			gpk_necall(tableFiles		.append(pathToLoad.begin(), pathLen)				, "Failed to append data bytes. Buffer sizes:\ntableFiles     : %u.\ncontentsPacked : %u.", tableFiles.size(), contentsPacked.size());
			gpk_necall(contentsPacked	.append(contentsTemp.begin(), contentsTemp.size())	, "Failed to append data bytes. Buffer sizes:\ntableFiles     : %u.\ncontentsPacked : %u.", tableFiles.size(), contentsPacked.size());
			contentsTemp.clear();
			++fileHeader.TotalFileCount;
		}
	}
	fileHeader.SizeUncompressedTableFiles		= tableFiles		.size();
	fileHeader.SizeUncompressedContentsPacked	= contentsPacked	.size();
	::gpk::array_pod<byte_t>						& compressedTableFiles		= out_packed.CompressedTableFiles		;
	::gpk::array_pod<byte_t>						& compressedContentsPacked	= out_packed.CompressedContentsPacked	;
	{	// compress
		gpk_necall(::gpk::arrayDeflate(tableFiles		, compressedTableFiles		, FOLDERPACK_DEFLATE_CHUNK_SIZE), "%s", "Unknown error.");
		gpk_necall(::gpk::arrayDeflate(contentsPacked	, compressedContentsPacked	, FOLDERPACK_DEFLATE_CHUNK_SIZE), "%s", "Unknown error.");
		fileHeader.SizeCompressedTableFiles		= compressedTableFiles		.size();
		fileHeader.SizeCompressedContentsPacked	= compressedContentsPacked	.size();
	}	
	return 0;
}

::gpk::error_t								gpk::folderUnpack			(::gpk::SFolderInMemory& out_loaded, const ::gpk::view_const_byte & rawFileInMemory)		{
	const ::gpk::SPackHeader						& header				= *(::gpk::SPackHeader*)&rawFileInMemory[0];
	out_loaded.Names		.resize(header.TotalFileCount);
	out_loaded.Contents		.resize(header.TotalFileCount);
	out_loaded.DataInfo		.clear();
	out_loaded.DataContents	.clear();
	gpk_necall(::gpk::arrayInflate({&rawFileInMemory[0] + sizeof(::gpk::SPackHeader)									, header.SizeCompressedTableFiles		}, out_loaded.DataInfo		, FOLDERPACK_INFLATE_CHUNK_SIZE), "%s", "Failed to uncompress file table.");
	gpk_necall(::gpk::arrayInflate({&rawFileInMemory[0] + sizeof(::gpk::SPackHeader) + header.SizeCompressedTableFiles	, header.SizeCompressedContentsPacked	}, out_loaded.DataContents	, FOLDERPACK_INFLATE_CHUNK_SIZE), "%s", "Failed to uncompress file contents.");
	{ // Build access tables.
		uint32_t										offsetInfo				= 0;
		for(uint32_t iFile = 0; iFile < out_loaded.Names.size(); ++iFile) {
			const ::gpk::SRange<uint32_t>					& fileLocation			= *(const ::gpk::SRange<uint32_t>*)&out_loaded.DataInfo[offsetInfo];
			offsetInfo									+= sizeof(const ::gpk::SRange<uint32_t>);
			const uint32_t									lenName					= *(uint32_t*)&out_loaded.DataInfo		[offsetInfo];
			offsetInfo									+= sizeof(uint32_t);
			out_loaded.Names	[iFile]					= {&out_loaded.DataInfo		[offsetInfo], lenName};
			offsetInfo									+= lenName;
			out_loaded.Contents	[iFile]					= {&out_loaded.DataContents	[fileLocation.Offset], fileLocation.Count};
		}
	}
	return 0;
}

::gpk::error_t							gpk::folderUnpack			(::gpk::SFolderInMemory& out_loaded, const ::gpk::view_const_string	nameFileSrc)					{
	::gpk::array_pod<byte_t>					rawFileInMemory				= {};
	gpk_necall(::gpk::fileToMemory(nameFileSrc, rawFileInMemory), "Failed to load pak file: %s.", nameFileSrc);
	gpk_necall(::gpk::folderUnpack(out_loaded, rawFileInMemory), "Failed to unpack pak file: %s.", nameFileSrc);
	return 0;
}

::gpk::error_t							gpk::folderToDisk			(const ::gpk::SFolderPackage & folderPackage, const ::gpk::view_const_string nameFileDst)			{
	const ::gpk::SPackHeader 					& fileHeader				= folderPackage.PackageInfo;
	const ::gpk::array_pod<byte_t>				& compressedTableFiles		= folderPackage.CompressedTableFiles		;
	const ::gpk::array_pod<byte_t>				& compressedContentsPacked	= folderPackage.CompressedContentsPacked	;
	{
		FILE										* fp						= 0;
		::gpk::array_pod<char_t>					bufferFormat					= {};
		::gpk::array_pod<char_t>					finalPathName					= {};
		finalPathName.resize(1024*8);
		bufferFormat.resize(64);
		sprintf_s(bufferFormat.begin(), bufferFormat.size(), "%%.%us", nameFileDst.size());
		sprintf_s(finalPathName.begin(), finalPathName.size(), bufferFormat.begin(), nameFileDst.begin());
		fopen_s(&fp, finalPathName.begin(), "wb");
		ree_if(0 == fp, "Failed to create file: %s.", nameFileDst.begin());
		fwrite(&fileHeader							, 1, sizeof(::gpk::SPackHeader)			, fp);
		fwrite(compressedTableFiles		.begin	()	, 1, compressedTableFiles		.size()	, fp);
		fwrite(compressedContentsPacked	.begin	()	, 1, compressedContentsPacked	.size()	, fp);
		fclose(fp);
	}
	return 0;
}

// Write folder to disk.
::gpk::error_t							gpk::folderToDisk				(const ::gpk::SFolderInMemory & virtualFolder, ::gpk::view_const_string destinationPath)				{
	::gpk::array_pod<char_t>					bufferFormat					= {};
	::gpk::array_pod<char_t>					finalPathName					= {};
	finalPathName.resize(1024*8);
	bufferFormat.resize(64);
	memset(finalPathName.begin(), 0, finalPathName.size());
	FILE										* fp						= 0;
	for(uint32_t iFile = 0, countFiles = virtualFolder.Names.size(); iFile < countFiles; ++iFile) {
		gpk_safe_fclose(fp);
		const ::gpk::view_const_string				& fileName					= virtualFolder.Names		[iFile];
		const ::gpk::view_const_byte				& fileContent				= virtualFolder.Contents	[iFile];
		sprintf_s(bufferFormat.begin(), bufferFormat.size(), "%%.%us%%.%us", destinationPath.size(), fileName.size());
		sprintf_s(finalPathName.begin(), finalPathName.size(), bufferFormat.begin(), destinationPath.begin(), fileName.begin());
		info_printf("File found (%u): %s. Size: %u.", iFile, finalPathName.begin(), fileContent.size());
		uint32_t									lenPath						= (uint32_t)strlen(finalPathName.begin());
		::gpk::error_t								indexSlash					= ::gpk::findLastSlash(::gpk::view_const_string{finalPathName.begin(), uint32_t(-1)});	
		if(-1 != indexSlash) { // Create path if any specified.
			finalPathName[indexSlash]				= 0;
			lenPath									= (uint32_t)strlen(finalPathName.begin());
 			ce_if(errored(::gpk::pathCreate({finalPathName.begin(), lenPath})), "Failed to create foder: %s.", finalPathName.begin());
			finalPathName[indexSlash]				= '/';
		}
		fopen_s(&fp, finalPathName.begin(), "wb");
		ce_if(0 == fp, "Failed to create file: %s.", finalPathName.begin());
		ce_if(fileContent.size() != fwrite(fileContent.begin(), 1, fileContent.size(), fp), "Failed to write file: %s. Disk full?", finalPathName.begin());
	}
	gpk_safe_fclose(fp);
	return 0;
}

::gpk::error_t							gpk::folderPackToDisk		(	const ::gpk::view_const_string nameFileDst,	const ::gpk::view_const_string nameFolderSrc)		{
	::gpk::SFolderPackage						folderPackage;
	gpk_necall(::gpk::folderPack(folderPackage, nameFolderSrc), "Failed to pack folder: %s.", nameFolderSrc.begin());
	gpk_necall(::gpk::folderToDisk(folderPackage, nameFileDst), "Failed to pack folder: %s.", nameFolderSrc.begin());
	return 0;
}

::gpk::error_t							gpk::folderUnpackToDisk			(const ::gpk::view_const_string namePathDst, const ::gpk::view_const_string nameFileSrc)		{
	::gpk::SFolderInMemory						virtualFolder					= {};
	gpk_necall(::gpk::folderUnpack(virtualFolder, nameFileSrc), "Failed to unpack file: %s.", nameFileSrc);
	gpk_necall(::gpk::folderToDisk(virtualFolder, namePathDst), "Failed to write folder to disk. Disk full or insufficient permissions. File name: %s. Destionation Path: %s.", nameFileSrc, namePathDst);
	return 0;
}

