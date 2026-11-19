#define GPK_CONSOLE_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_file.h"
#include "gpk_json.h"
#include "gpk_parse.h"
#include "gpk_deflate.h"
#include "gpk_aes.h"
#include "gpk_noise.h"

#include "blitter.h"
#include "gpk_path.h"

GPK_USING_TYPEINT();

static constexpr const u2_t 			DEFAULT_BLOCK_SIZE				= 1024;

struct SSplitParams {
	::gpk::vcst_t				FileNameSrc						= {};	// First parameter is the only parameter, which is the name of the source file to be split.
	::gpk::vcst_t				PathWithoutExtension			= {};	// First parameter is the only parameter, which is the name of the source file to be split.
	::gpk::vcst_t				DBName							= {};	// First parameter is the only parameter, which is the name of the source file to be split.
	::gpk::vcst_t				EncryptionKey					= {};	// First parameter is the only parameter, which is the name of the source file to be split.

	bool									DeflatedOutput					= false;
};

::gpk::err_t			loadParams						(SSplitParams& params, int argc, char ** argv)		{
	for(s2_t iArg = 5; iArg < argc; ++iArg)
		info_printf("Unknown parameter: %s.", argv[iArg]);
	if_true_fef(2 > argc, "Usage:\n\t%s [filename] [blockSize] [deflated output (1:0)] [deflated input (1:0)] ", argv[0]);
	if_true_fef(65535 < argc, "Invalid parameter count: %u.", (u2_t )argc);
	params.FileNameSrc						= {argv[1], (u2_t )-1};	// First parameter is the only parameter, which is the name of the source file to be split.
	params.DeflatedOutput					= (argc > 2 && argv[2][0] != '0');
	if(argc > 3)
		params.EncryptionKey					= {argv[3], (u2_t )-1};


	::gpk::err_t								indexOfDot						= ::gpk::rfind('.', params.FileNameSrc);
	::gpk::err_t								indexOfLastSlash				= ::gpk::findLastSlash(params.FileNameSrc);
	params.PathWithoutExtension				= (indexOfDot > indexOfLastSlash) ? ::gpk::vcst_t{params.FileNameSrc.begin(), (u2_t )indexOfDot} : params.FileNameSrc;
	params.DBName							= (-1 == indexOfLastSlash)
		? params.PathWithoutExtension
		: ::gpk::vcst_t{&params.PathWithoutExtension[indexOfLastSlash], params.PathWithoutExtension.size() - indexOfLastSlash}
		;	// First parameter is the only parameter, which is the name of the source file to be split.

	info_printf("Deflated output: %s", params.DeflatedOutput ? "true" : "false");
	if(params.EncryptionKey.size())
		info_printf("Encryption: %s.", "false");
	else
		info_printf("Encryption key: %.*s.", GPK_VIEW_ARGS(params.EncryptionKey));
	return 0;
}

#define SEASON_ENABLE_VERIFICATION

struct SWriteCache {
	::gpk::string				PartFileName					= {};
	::gpk::string				PathToWriteTo					= {};
	::gpk::SLoadCache						LoadCache						= {};
#if defined SEASON_ENABLE_VERIFICATION
	::gpk::string				Verify							= {};
#endif
};

::gpk::err_t			writePart						(::SWriteCache & blockCache, const ::SSplitParams & params, const ::gpk::vcu0_t & partBytes)		{
	::gpk::string					& partFileName					= blockCache.PartFileName					;
	::gpk::string					& pathToWriteTo					= blockCache.PathToWriteTo					;
	::gpk::clear(partFileName, pathToWriteTo, blockCache.LoadCache.Deflated, blockCache.LoadCache.Encrypted);
	if_fail_fef(::blt::tableFileName(partFileName, params.DeflatedOutput ? ::blt::DATABASE_HOST_DEFLATE : ::blt::DATABASE_HOST_LOCAL, params.EncryptionKey.size() > 0, params.DBName), "%s", "??");
	if_fail_fe(pathToWriteTo.append(partFileName));
	return ::gpk::fileFromMemorySecure(blockCache.LoadCache, pathToWriteTo, params.EncryptionKey.bytes(), params.DeflatedOutput, partBytes);
}

// Splits a file.json into file.#blocksize.db/file.##.json parts.
int										main							(int argc, char ** argv)		{
	::SSplitParams								params							= {};
	if_fail_fef(::loadParams(params, argc, argv), "%s", "");
	::gpk::string					bytesToSave						= {};
	{
		::gpk::SJSONFile							jsonFileToSplit					= {};
		if_fail_fef(::gpk::jsonFileRead(jsonFileToSplit, params.FileNameSrc), "Failed to load file: %.*s.", GPK_VIEW_ARGS(params.FileNameSrc));
		if_true_fef(0 == jsonFileToSplit.Reader.Tree.size(), "Invalid input format. %s", "File content is not a JSON array.");
		if_true_fef(jsonFileToSplit.Reader.Token[0].Type != ::gpk::JSON_TYPE_ARRAY, "Invalid input format. %s", ::gpk::get_value_label(jsonFileToSplit.Reader.Token[0].Type).begin());
		info_printf("Loaded file: %.*s. Size: %u bytes.", GPK_VIEW_ARGS(params.FileNameSrc), jsonFileToSplit.Bytes.size());
		if_fail_fef(::gpk::jsonWrite(jsonFileToSplit.Reader.Tree[0], jsonFileToSplit.Reader.View, bytesToSave), "Out of memory or corrupt json file: %.*s.", GPK_VIEW_ARGS(params.FileNameSrc));
	}

	::SWriteCache								blockCache						= {};
	if_fail_fe(::writePart(blockCache, params, bytesToSave.bytes()));
	return 0;
}
