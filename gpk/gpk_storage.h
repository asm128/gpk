#include "gpk_array.h"
#include "gpk_label.h"

#ifndef GPK_STORAGE_H_2983749283
#define GPK_STORAGE_H_2983749283

namespace gpk
{
	struct SPathContents {
						::gpk::array_obj<::gpk::array_pod<char_t>>		Files					= {};
						::gpk::array_obj<::gpk::SPathContents>			Folders					= {};
	};

					::gpk::error_t									pathList				(const ::gpk::SPathContents& input, ::gpk::array_obj<::gpk::array_pod<char_t>>& output);	// recursively walk over a pathcontents hierarchy and store all the file names into "output"
					::gpk::error_t									pathList				(const ::gpk::SPathContents& input, ::gpk::array_obj<::gpk::view_const_string>& output);	// recursively walk over a pathcontents hierarchy and store all the file names into "output"
					::gpk::error_t									pathList				(const ::gpk::view_const_string& pathToList, SPathContents & out_Contents);		// Recursive
	static inline	::gpk::error_t									pathList				(const ::gpk::view_const_string& pathToList, ::gpk::array_obj<::gpk::array_pod<char_t>>& output) {
		::gpk::SPathContents												tree					= {};
		gpk_necall(::gpk::pathList(pathToList, tree), "%s", "?");
		gpk_necall(::gpk::pathList(tree, output), "%s", "?");
		return 0;
	}	// Recursive
					::gpk::error_t									pathList				(const ::gpk::view_const_string& pathToList, ::gpk::array_obj<::gpk::array_pod<char_t>>& output, bool listFolders);		// Not recursive
					::gpk::error_t									fileToMemory			(const ::gpk::view_const_string& fileName, ::gpk::array_pod<byte_t>& fileInMemory);
					::gpk::error_t									fileFromMemory			(const ::gpk::view_const_string& fileName, const ::gpk::array_pod<byte_t>& fileInMemory);
					::gpk::error_t									pathCreate				(const ::gpk::view_const_string& folderName);	// Recursive
					::gpk::error_t									fileJoin				(const ::gpk::view_const_string	& fileNameDst);	// Joins a file split into file.split.## parts.
					::gpk::error_t									fileSplit				(const ::gpk::view_const_string	& fileNameSrc, const uint32_t partSize);	// Splits a file into file.split.## parts.
} // namespace

#endif // GPK_STORAGE_H_2983749283
