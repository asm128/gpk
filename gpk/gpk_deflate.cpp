#include "gpk_deflate.h"
#include "gpk_storage.h"
#include "gpk_coord.h"
#include "gpk_safe.h"
#include "deflate.h"

		::gpk::error_t									gpk::arrayDeflate								(const ::gpk::view_const_byte	& inflated, ::gpk::array_pod<byte_t>& deflated)	{
    int				ret;
	z_stream		strm												= {};
    ret																	= deflateInit(&strm, Z_BEST_COMPRESSION);
    if (ret != Z_OK)
        return ret;
    strm.avail_in														= inflated.size();
    strm.next_in														= (Bytef*)inflated.begin();
    strm.avail_out														= deflated.size();
    strm.next_out														= (Bytef*)deflated.begin();
    ret																	= deflate(&strm, Z_FINISH);    /* no bad return value */
    if(ret == Z_STREAM_ERROR) {
	   (void)deflateEnd(&strm);
		error_printf("Failed to compress: 0x%x.", ret);  /* state not clobbered */
		return -1;
	}
    int ret_end = deflateEnd(&strm);
    error_if(strm.avail_in != 0, "%s", "Not all of the input bytes were consumed.");	/* all input will be used */
    error_if(ret != Z_STREAM_END && ret != Z_OK, "%s", "Unknown error");				/* stream will be complete */
	ree_if(ret_end == Z_STREAM_ERROR, "deflateEnd() returned %s", "Z_STREAM_ERROR")
	deflated.resize((uint32_t)((ptrdiff_t)strm.next_out - (ptrdiff_t)deflated.begin()));
    /* clean up and return */
	info_printf("deflateEnd: %u.", (uint32_t)ret);
	return 0;
}

		::gpk::error_t									gpk::arrayInflate								(const ::gpk::view_const_byte & deflated, ::gpk::array_pod<byte_t>& inflated)	{
	z_stream																	strm											= {};
	int																			ret												= inflateInit(&strm);	 // allocate inflate state
	if (ret != Z_OK)
		return ret;

	strm.avail_in															= (uint32_t)deflated.size();
	strm.avail_out															= (uint32_t)inflated.size();
	strm.next_in															= (Bytef *)deflated.begin();
	strm.next_out															= (Bytef *)inflated.begin();
	ret																		= ::inflate(&strm, Z_NO_FLUSH);
	ree_if(ret == Z_STREAM_ERROR, "%s", "ZIP Error");  // state not clobbered
	switch (ret) {
	case Z_NEED_DICT		:
		ret																			= Z_DATA_ERROR;     // and fall through
	case Z_VERSION_ERROR	:
	case Z_STREAM_ERROR		:
	case Z_DATA_ERROR		:
	case Z_MEM_ERROR		:
		(void)inflateEnd(&strm);
		return -1;
	}
	if(ret != Z_STREAM_END && ret != Z_OK) {
		(void)inflateEnd(&strm);
		error_printf("%s", "Failed to decompress?");
		return -1;
	}
	inflated.resize((uint32_t)((ptrdiff_t)strm.next_out - (ptrdiff_t)inflated.begin()));
	ret																			= ::inflateEnd(&strm);
	info_printf("inflateEnd: %u.", (uint32_t)ret);
	return 0;
}

::gpk::error_t								gpk::folderPack				(::gpk::SFolderPackage& out_packed, const ::gpk::view_const_string	nameFolderSrc) {
	::gpk::SPackHeader 								& fileHeader			= out_packed.PackageInfo;
	// -- The following two arrays store the file table and the file contents that are going to be compressed and stored on disk
	::gpk::array_pod<byte_t>						tableFiles				= {};
	::gpk::array_pod<byte_t>						contentsPacked			= {};
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
			printf("pathToLoad (%u): '%s'.\n", iFile, pathToLoad.begin());
			gpk_necall(::gpk::fileToMemory(pathToLoad, contentsTemp), "Failed to load file: %s. Out of memory?", pathToLoad.begin());

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
	compressedTableFiles		.resize(tableFiles		.size()	);
	compressedContentsPacked	.resize(contentsPacked	.size()	);
	{	// compress
		gpk_necall(::gpk::arrayDeflate(tableFiles		, compressedTableFiles		), "%s", "Unknown error.");
		gpk_necall(::gpk::arrayDeflate(contentsPacked	, compressedContentsPacked	), "%s", "Unknown error.");
		fileHeader.SizeCompressedTableFiles		= compressedTableFiles		.size();
		fileHeader.SizeCompressedContentsPacked	= compressedContentsPacked	.size();
	}	
	return 0;
}

::gpk::error_t								gpk::folderUnpack			(::gpk::SFolderInMemory& out_loaded, const ::gpk::view_const_byte & rawFileInMemory)		{
	const ::gpk::SPackHeader						& header				= *(::gpk::SPackHeader*)&rawFileInMemory[0];
	out_loaded.DataInfo		.resize(header.SizeUncompressedTableFiles		);
	out_loaded.DataContents	.resize(header.SizeUncompressedContentsPacked	);
	out_loaded.Names		.resize(header.TotalFileCount);
	out_loaded.Contents		.resize(header.TotalFileCount);
	gpk_necall(::gpk::arrayInflate({&rawFileInMemory[0] + sizeof(::gpk::SPackHeader)									, header.SizeCompressedTableFiles		}, out_loaded.DataInfo		), "Failed to uncompress file table.");
	gpk_necall(::gpk::arrayInflate({&rawFileInMemory[0] + sizeof(::gpk::SPackHeader) + header.SizeCompressedTableFiles	, header.SizeCompressedContentsPacked	}, out_loaded.DataContents	), "Failed to uncompress file contents.");
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
		fopen_s(&fp, nameFileDst.begin(), "wb");
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
	::gpk::array_pod<char_t>					finalPathName					= {};
	finalPathName.resize(1024*16);
	memset(finalPathName.begin(), 0, finalPathName.size());
	FILE										* fp						= 0;
	for(uint32_t iFile = 0, countFiles = virtualFolder.Names.size(); iFile < countFiles; ++iFile) {
		safe_fclose(fp);
		const ::gpk::view_const_string				& fileName					= virtualFolder.Names		[iFile];
		const ::gpk::view_const_byte				& fileContent				= virtualFolder.Contents	[iFile];
		info_printf("File found (%u): %s. Size: %u.", iFile, fileName.begin(), fileContent.size());
		sprintf_s(finalPathName.begin(), finalPathName.size(), "%s%s", destinationPath.begin(), fileName.begin());
		::gpk::error_t								indexSlash					= ::gpk::rfind_sequence_pod(::gpk::view_array<const char>{"\\", 1}, {finalPathName.begin(), (uint32_t)-1});	
		if(-1 != indexSlash) { // Create path if any specified.
			finalPathName[indexSlash + 1]			= 0;
			ce_if(errored(::gpk::pathCreate({finalPathName.begin(), (uint32_t)-1})), "Failed to create foder: %s.", finalPathName.begin());
			finalPathName[indexSlash + 1]			= '\\';
		}
		fopen_s(&fp, finalPathName.begin(), "wb");
		ce_if(0 == fp, "Failed to create file: %s.", finalPathName.begin());
		ce_if(fileContent.size() != fwrite(fileContent.begin(), 1, fileContent.size(), fp), "Failed to write file: %s. Disk full?", finalPathName.begin());
	}
	safe_fclose(fp);
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

