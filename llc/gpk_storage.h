#include "gpk_array.h"

#ifndef GPK_STORAGE_H_2983749283
#define GPK_STORAGE_H_2983749283

namespace gpk
{
	struct SPathContents {
		::gpk::aobj<::gpk::apod<char>>	Files					= {};
		::gpk::aobj<::gpk::SPathContents>	Folders					= {};
	};

	::gpk::error_t						pathList				(const ::gpk::SPathContents & input, ::gpk::aobj<::gpk::apod<char>> & output, const ::gpk::vcc extension = {});	// recursively walk over a pathcontents hierarchy and store all the file names into "output"
	::gpk::error_t						pathList				(const ::gpk::SPathContents & input, ::gpk::aobj<::gpk::vcc> & output, const ::gpk::vcc extension = {});	// recursively walk over a pathcontents hierarchy and store all the file names into "output"
	::gpk::error_t						pathList				(const ::gpk::vcc & pathToList, SPathContents & out_Contents, const ::gpk::vcc extension = {});		// Recursive
	stainli	::gpk::error_t				pathList				(const ::gpk::vcc & pathToList, ::gpk::aobj<::gpk::apod<char>>& output, const ::gpk::vcc extension = {}) {
		::gpk::SPathContents					tree					= {};
		int32_t									error					= ::gpk::pathList(pathToList, tree, extension);
		gpk_necs(error |= ::gpk::pathList(tree, output, extension));
		return 0;
	}	// Recursive
	::gpk::error_t						pathList				(const ::gpk::vcc & pathToList, ::gpk::aobj<::gpk::apod<char>> & output, bool listFolders, const ::gpk::vcc extension = {});		// Not recursive
	::gpk::error_t						fileToMemory			(const ::gpk::vcc & fileName, ::gpk::apod<uint8_t> & fileInMemory);
	::gpk::error_t						fileFromMemory			(const ::gpk::vcc & fileName, const ::gpk::vcu8 & fileInMemory);
	stainli	::gpk::error_t				fileToMemory			(const ::gpk::vcc & fileName, ::gpk::apod<int8_t> & fileInMemory)	{ return ::gpk::fileToMemory	(fileName, *(::gpk::au8*)&fileInMemory); }
	stainli	::gpk::error_t				fileFromMemory			(const ::gpk::vcc & fileName, const ::gpk::vci8 & fileInMemory)		{ return ::gpk::fileFromMemory	(fileName, *(const ::gpk::vcu8*)&fileInMemory);; }
	stainli	::gpk::error_t				fileToMemory			(const ::gpk::vcc & fileName, ::gpk::apod<char> & fileInMemory)		{ return ::gpk::fileToMemory	(fileName, *(::gpk::au8*)&fileInMemory); }
	stainli	::gpk::error_t				fileFromMemory			(const ::gpk::vcc & fileName, const ::gpk::vcc & fileInMemory)		{ return ::gpk::fileFromMemory	(fileName, *(const ::gpk::vcu8*)&fileInMemory);; }
	::gpk::error_t						fileJoin				(const ::gpk::vcc & fileNameDst);								// Joins a file split into file.split.## parts.
	::gpk::error_t						fileSplit				(const ::gpk::vcc & fileNameSrc, const uint32_t partSize);		// Splits a file into file.split.## parts.
	int64_t								fileSize				(const ::gpk::vcc & fileNameSrc);
	::gpk::error_t						pathCreate				(const ::gpk::vcc & folderName, const char separator = '/');	// Recursive

	// this function was ceated in order to work around the problem of the JSON system returning pointers to the original string, without having the opportunity of processing escaped path slashes.
	::gpk::error_t						pathNameCompose			(const ::gpk::vcc & path, const ::gpk::vcc & fileName, ::gpk::apod<char> & out_composed);
	::gpk::error_t						findLastSlash			(const ::gpk::vcc & path);

	struct SFileManager {
		::gpk::aobj<::gpk::au8>				Data					= {};
		::gpk::aobj<::gpk::vcc>				Name					= {};

		::gpk::error_t						Save					(::gpk::au8 & output)	const	{
			gpk_necs(::gpk::savePOD(output, Name.size()));
			for(uint32_t iFile = 0; iFile < Name.size(); ++iFile) {
				gpk_necall(::gpk::saveView(output, Name[iFile]), "iEntity: %i", iFile);
			}
			return 0;
		}
		::gpk::error_t						AddFile					(const ::gpk::vcc & fileName)	{
			for(uint32_t iFile = 0; iFile < Name.size(); ++iFile) {
				if(fileName == Name[iFile])
					return iFile;
			}
			::gpk::au8								fileBytes				= {};
			gpk_necall(::gpk::fileToMemory(fileName, fileBytes), "Failed to load file: '%s'", ::gpk::toString(fileName).begin());
			gpk_necs(Data.push_back(fileBytes));
			gpk_necs(Name.push_back(fileName));
			return -1;
		}
	};
} // namespace

#endif // GPK_STORAGE_H_2983749283
