#include "gpk_array.h"
#include "gpk_label.h"

#ifndef GPK_STORAGE_H_2983749283
#define GPK_STORAGE_H_2983749283

namespace gpk
{
		struct SPathContents {
					::gpk::array_obj<::gpk::label>										Files							= {};
					::gpk::array_obj<::gpk::SPathContents>								Folders							= {};
		};

				::gpk::error_t														pathList						(const ::gpk::label & pathToList, SPathContents & out_Contents);				// Recursive
				::gpk::error_t														pathList						(const ::gpk::SPathContents& input, ::gpk::array_obj<::gpk::label>& output);	// recursively walk over a pathcontents hierarchy and store all the file names into "output"
				::gpk::error_t														pathList						(const ::gpk::label & pathToList, ::gpk::array_obj<::gpk::label>& output, bool listFolders);		// Not recursive
				::gpk::error_t														fileToMemory					(const ::gpk::view_const_string& fileName, ::gpk::array_pod<byte_t>& fileInMemory);
				::gpk::error_t														fileFromMemory					(const ::gpk::view_const_string& fileName, const ::gpk::array_pod<byte_t>& fileInMemory);
				::gpk::error_t														dirCreate						(const ::gpk::view_const_string& folderName);				// Recursive
} // namespace

#endif // GPK_STORAGE_H_2983749283