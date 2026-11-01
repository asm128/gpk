#include "gpk_cstdio.h"
#include "gpk_string.h"

#ifndef GPK_FILE_H_23627
#define GPK_FILE_H_23627

namespace gpk
{
	err_t			with				(vcst_t filepath, vcst_t mode, const function<err_t(FILE*&)> & funcFile);	//

	err_t			fileJoin			(vcst_t fileNameDst);								// Joins a file split into file.split.## parts.
	err_t			fileSplit			(vcst_t fileNameSrc, u2_c partSize);		// Writes file.0, file.1, ...; returns part count. Failure retains completed/partial parts.
	s3_t			fileSize			(vcst_t fileName);
	err_t			fileLastWriteTime	(vcst_t fileName, s3_t & modified);
	err_t			fileDelete			(vcst_t fileName);

	err_t			fileFromMemory		(vcst_t folderPath, vcst_t fileName, vcu0_t fileInMemory, bool append = false);
	err_t			fileFromMemory		(vcst_t fileName, vcu0_t fileInMemory, bool append = false);
	stin	err_t	fileFromMemory		(vcst_t fileName, vcst_t fileInMemory, bool append = false)									{ rtrn fileFromMemory	(fileName, vcu0_t{(u0_c*)fileInMemory.begin(), fileInMemory.size()}, append); }

	err_t			fileToMemory		(vcst_t fileName, au0_t & fileInMemory, u2_t maxSize = 0xFFFFFFFFU, u3_t offset = 0);
	err_t			fileToMemory		(vcst_t folderPath, vcst_t fileName, au0_t & fileBytes, u2_t maxSize = 0xFFFFFFFFU, u3_t offset = 0);
	stin	err_t	fileToMemory		(vcst_t fileName, as0_t & fileInMemory, u2_t maxSize = 0xFFFFFFFFU, u3_t offset = 0)	{ rtrn fileToMemory		(fileName, *(au0_t*)&fileInMemory, maxSize, offset); }
	stin	err_t	fileToMemory		(vcst_t fileName, string & fileInMemory, u2_t maxSize = 0xFFFFFFFFU, u3_t offset = 0)	{ rtrn fileToMemory		(fileName, *(au0_t*)&fileInMemory, maxSize, offset); }

	GDEFINE_ENUM_TYPE(OPEN_MODE, u2_t);
	GDEFINE_ENUM_VALUE(OPEN_MODE, CLOSE		, 0);
	GDEFINE_ENUM_VALUE(OPEN_MODE, READ		, 'r' + (u1_t('b') << 8));							//  1
	GDEFINE_ENUM_VALUE(OPEN_MODE, WRITE		, 'w' + (u1_t('b') << 8));							//  2
	GDEFINE_ENUM_VALUE(OPEN_MODE, APPEND	, 'a' + (u2_t('b') << 8) + (u2_t('+') << 16));	//  3
} // namespace

#endif // GPK_FILE_H_23627
