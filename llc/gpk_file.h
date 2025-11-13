#include "gpk_cstdio.h"

#ifndef GPK_FILE_H_23627
#define GPK_FILE_H_23627

namespace gpk
{
	err_t			with			(vcst_t filepath, vcst_t mode, const function<err_t(FILE*&)> & funcFile);	// 

	err_t			fileJoin		(vcst_t fileNameDst);								// Joins a file split into file.split.## parts.
	err_t			fileSplit		(vcst_t fileNameSrc, u2_c partSize);		// Splits a file into file.split.## parts.
	s3_t			fileSize		(vcst_t fileName);
	err_t			fileDelete		(vcst_t fileName);

	err_t			fileFromMemory	(vcst_t fileName, vcu0_c & fileInMemory, bool append = false);
	err_t			fileToMemory	(vcst_t fileName, au0_t & fileInMemory, uint32_t maxSize = 0xFFFFFFFFU, uint64_t offset = 0);
	err_t			fileToMemory	(vcst_t folderPath, vcst_t fileName, au0_t & fileBytes, uint32_t maxSize = 0xFFFFFFFFU, uint64_t offset = 0);
	err_t			fileFromMemory	(vcst_t folderPath, vcst_t fileName, vcu0_c & fileInMemory, bool append = false);

	stin	err_t	fileToMemory	(vcst_t fileName, as0_t & fileInMemory, uint32_t maxSize = 0xFFFFFFFFU, uint64_t offset = 0)	{ rtrn fileToMemory		(fileName, *(au0_t*)&fileInMemory, maxSize, offset); }
	stin	err_t	fileToMemory	(vcst_t fileName, asc_t & fileInMemory, uint32_t maxSize = 0xFFFFFFFFU, uint64_t offset = 0)	{ rtrn fileToMemory		(fileName, *(au0_t*)&fileInMemory, maxSize, offset); }
	stin	err_t	fileFromMemory	(vcst_t fileName, vcs0_c & fileInMemory, bool append = false)									{ rtrn fileFromMemory	(fileName, *(vcu0_c*)&fileInMemory, append); }
	stin	err_t	fileFromMemory	(vcst_t fileName, vcsc_c & fileInMemory, bool append = false)									{ rtrn fileFromMemory	(fileName, *(vcu0_c*)&fileInMemory, append); }

	GDEFINE_ENUM_TYPE(OPEN_MODE, i2u_t);
	GDEFINE_ENUM_VALUE(OPEN_MODE, CLOSE		, 0);												
	GDEFINE_ENUM_VALUE(OPEN_MODE, READ		, 'r' + (i1u_t('b') << 8));							//  1
	GDEFINE_ENUM_VALUE(OPEN_MODE, WRITE		, 'w' + (i1u_t('b') << 8));							//  2
	GDEFINE_ENUM_VALUE(OPEN_MODE, APPEND	, 'a' + (i2u_t('b') << 8) + (i2u_t('+') << 16));	//  3
	// FILE wrapper
	tpl_t struct SFile {	
		tdfTTCnst(_t);
		vcst_t						Name			= {};
		u3_t						Offset			= {};
		T							File			{};

		inln						~SFile			()					{ gpk_safe_fclose(File); }

		inxp		oper			T&				()					{ return File; }
		
		u3_t						size			()					{
			if(0 == File) {
				if_true_fef(fopen_s(&File, Name.begin(), "rb"), "%s", Name.begin());
				Offset = 0;
			}
			if_true_fef(gpk::fseek(File, 0, SEEK_END), "Failed to seek to end of file. %s.", "why?");
			u3_c fileSize = ftell(File);
			if_true_block_logf(error_printf, Offset > fileSize, Offset = fileSize, "Offset: %" GPK_FMT_U64 ". fileSize: %" GPK_FMT_U64 ".", Offset, fileSize);
			if_true_fe(fseek(File, Offset, SEEK_SET));
			return fileSize;
		}
	};

	struct CFile : SFile<FILE*> {};

} // namespace

#endif // GPK_FILE_H_23627
