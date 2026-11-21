#define GPK_CONSOLE_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_log.h"
#include "gpk_file.h"
#include "gpk_path.h"
#include "gpk_json.h"
#include "gpk_parse.h"
#include "gpk_deflate.h"
#include "gpk_aes.h"
#include "gpk_noise.h"

#include "blitter.h"

GPK_USING_TYPEINT();

static constexpr const u2_t 			DEFAULT_BLOCK_SIZE				= 1024;

struct SSplitParams {
	::gpk::vcst_t					FileNameSrc						= {};	// First parameter is the only parameter, which is the name of the source file to be split.
	::gpk::vcst_t					PathWithoutExtension			= {};	// First parameter is the only parameter, which is the name of the source file to be split.
	::gpk::vcst_t					DBName							= {};	// First parameter is the only parameter, which is the name of the source file to be split.
	::gpk::vcst_t					EncryptionKey					= {};	// First parameter is the only parameter, which is the name of the source file to be split.

	u2_t 								BlockSize						= 0;
	bool									DeflatedOutput					= false;
};

::gpk::err_t			loadParams						(SSplitParams& params, int argc, char ** argv)		{
	for(s2_t iArg = 5; iArg < argc; ++iArg)
		info_printf("Unknown parameter: %s.", argv[iArg]);
	if_true_fef(2 > argc, "Usage:\n\t%s [filename] [blockSize] [deflated output (1:0)] [deflated input (1:0)] ", argv[0]);
	if_true_fef(65535 < argc, "Invalid parameter count: %u.", (u2_t )argc);
	params.FileNameSrc						= ::gpk::vcst_t{argv[1], (u2_t )-1};	// First parameter is the only parameter, which is the name of the source file to be split.
	if(argc > 2) {	// load block size param
		::gpk::parseIntegerDecimal({argv[2], (u2_t )-1}, params.BlockSize);
		info_printf("Using block size: %u.", params.BlockSize);
	}
	params.DeflatedOutput					= (argc > 3 && argv[3][0] != '0');
	if(argc > 4)
		params.EncryptionKey					= ::gpk::vcst_t{argv[4], (u2_t )-1};

	if(0 == params.BlockSize)
		params.BlockSize						= ::DEFAULT_BLOCK_SIZE;

	::gpk::err_t								indexOfDot						= ::gpk::rfind('.', params.FileNameSrc);
	::gpk::err_t								indexOfLastSlash				= ::gpk::findLastSlash(params.FileNameSrc);
	params.PathWithoutExtension				= (indexOfDot > indexOfLastSlash) ? ::gpk::vcst_t{params.FileNameSrc.begin(), (u2_t )indexOfDot} : params.FileNameSrc;
	params.DBName							= (-1 == indexOfLastSlash)
		? params.PathWithoutExtension
		: ::gpk::vcst_t{&params.PathWithoutExtension[indexOfLastSlash], params.PathWithoutExtension.size() - indexOfLastSlash}
		;	// First parameter is the only parameter, which is the name of the source file to be split.

	info_printf("Deflated output: %s", params.DeflatedOutput ? "true" : "false");
	if(params.EncryptionKey.size())
		info_printf("Encryption key: %.*s.", GPK_VIEW_ARGS(params.EncryptionKey));
	else
		info_printf("Encryption: %s.", "false");
	return 0;
}

#define SEASON_ENABLE_VERIFICATION

struct SWriteCache {
	::gpk::string				PartFileName					= {};
	::gpk::string				PathToWriteTo					= {};
	::gpk::SLoadCache						LoadCache						= {};
};

::gpk::err_t			writePart						(::SWriteCache & blockCache, const ::SSplitParams & params, const ::gpk::vcst_t dbFolderName, const ::gpk::vcu0_t & partBytes, u2_t  iPart)		{
	::gpk::string					& partFileName					= blockCache.PartFileName					;
	::gpk::string					& pathToWriteTo					= blockCache.PathToWriteTo					;
	::gpk::clear(partFileName, pathToWriteTo, blockCache.LoadCache.Deflated, blockCache.LoadCache.Encrypted);
	pathToWriteTo							= dbFolderName;
	if_fail_fef(::blt::blockFileName(partFileName, params.DBName, params.EncryptionKey.size() > 0, params.DeflatedOutput ? ::blt::DATABASE_HOST_DEFLATE : ::blt::DATABASE_HOST_LOCAL, iPart), "%s", "??");
	if_fail_fe(pathToWriteTo.append(partFileName));
	return ::gpk::fileFromMemorySecure(blockCache.LoadCache, pathToWriteTo, params.EncryptionKey.bytes(), params.DeflatedOutput, partBytes);
}

// Splits a file.json into file.#blocksize.db/file.##.json parts.
int										main							(int argc, char ** argv)		{
	::SSplitParams								params							= {};
	if_fail_fef(::loadParams(params, argc, argv), "%s", "");

	::gpk::SJSONFile							jsonFileToSplit					= {};
	if_fail_fef(::gpk::jsonFileRead(jsonFileToSplit, params.FileNameSrc), "Failed to load file: %.*s.", GPK_VIEW_ARGS(params.FileNameSrc));
	if_true_fef(0 == jsonFileToSplit.Reader.Tree.size(), "Invalid input format. %s", "File content is not a JSON array.");
	if_true_fef(jsonFileToSplit.Reader.Token[0].Type != ::gpk::JSON_TYPE_ARRAY, "Invalid input format. %s", ::gpk::get_value_label(jsonFileToSplit.Reader.Token[0].Type).begin());
	info_printf("Loaded file: %.*s. Size: %u bytes.", GPK_VIEW_ARGS(params.FileNameSrc), jsonFileToSplit.Bytes.size());

	::gpk::string					dbFolderName					= {};
	if_fail_fef(::blt::tableFolderName(dbFolderName, params.DBName, params.BlockSize), "%s", "??");
	if_fail_fef(::gpk::pathCreate({dbFolderName.begin(), dbFolderName.size()}), "Failed to create database folder: %.*s.", GPK_VIEW_ARGS(dbFolderName));
	info_printf("Output folder: %.*s.", GPK_VIEW_ARGS(dbFolderName));
	if(dbFolderName.size() && dbFolderName[dbFolderName.size()-1] != '/' && dbFolderName[dbFolderName.size()-1] != '\\') {
		if_fail_fef(dbFolderName.push_back('/'), "%s", "Out of memory?");
	}
	else if(0 == dbFolderName.size()) {
		if_fail_fef(dbFolderName.append(::gpk::vcst_t{"./"}), "%s", "Out of memory?");
	}

	::gpk::array_obj<::gpk::string>	outputJsons;
	if_fail_fef(::gpk::jsonArraySplit(*jsonFileToSplit.Reader.Tree[0], jsonFileToSplit.Reader.View , params.BlockSize, outputJsons), "%s", "Unknown error!");

	::SWriteCache								blockCache						= {};
	const ::gpk::vcst_t				folder							= {dbFolderName.begin(), dbFolderName.size()};
	for(u2_t  iPart = 0; iPart < outputJsons.size(); ++iPart) {
		::gpk::vcu0_t			partBytes						= outputJsons[iPart].bytes();
		if_fail_fe(::writePart(blockCache, params, folder, partBytes, iPart));
	}
	return 0;
}
