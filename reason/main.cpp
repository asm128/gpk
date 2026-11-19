#define LLC_CONSOLE_LOG_ENABLED
#define LLC_ERROR_PRINTF_ENABLED
#define LLC_WARNING_PRINTF_ENABLED
#define LLC_INFO_PRINTF_ENABLED

#include "llc_file.h"
#include "llc_json.h"
#include "llc_parse.h"
#include "llc_deflate.h"
#include "llc_aes.h"
#include "llc_noise.h"

#include "blitter.h"
#include "llc_path.h"

static constexpr const u2_t 			DEFAULT_BLOCK_SIZE				= 1024;

struct SSplitParams {
	::llc::vcst_t				FileNameSrc						= {};	// First parameter is the only parameter, which is the name of the source file to be split.
	::llc::vcst_t				PathWithoutExtension			= {};	// First parameter is the only parameter, which is the name of the source file to be split.
	::llc::vcst_t				DBName							= {};	// First parameter is the only parameter, which is the name of the source file to be split.
	::llc::vcst_t				EncryptionKey					= {};	// First parameter is the only parameter, which is the name of the source file to be split.

	bool									DeflatedOutput					= false;
};

::llc::err_t			loadParams						(SSplitParams& params, int argc, char ** argv)		{
	for(s2_t iArg = 5; iArg < argc; ++iArg)
		info_printf("Unknown parameter: %s.", argv[iArg]);
	if_true_fef(2 > argc, "Usage:\n\t%s [filename] [blockSize] [deflated output (1:0)] [deflated input (1:0)] ", argv[0]);
	if_true_fef(65535 < argc, "Invalid parameter count: %u.", (u2_t )argc);
	params.FileNameSrc						= {argv[1], (u2_t )-1};	// First parameter is the only parameter, which is the name of the source file to be split.
	params.DeflatedOutput					= (argc > 2 && argv[2][0] != '0');
	if(argc > 3)
		params.EncryptionKey					= {argv[3], (u2_t )-1};


	::llc::err_t								indexOfDot						= ::llc::rfind('.', params.FileNameSrc);
	::llc::err_t								indexOfLastSlash				= ::llc::findLastSlash(params.FileNameSrc);
	params.PathWithoutExtension				= (indexOfDot > indexOfLastSlash) ? ::llc::vcst_t{params.FileNameSrc.begin(), (u2_t )indexOfDot} : params.FileNameSrc;
	params.DBName							= (-1 == indexOfLastSlash)
		? params.PathWithoutExtension
		: ::llc::vcst_t{&params.PathWithoutExtension[indexOfLastSlash], params.PathWithoutExtension.size() - indexOfLastSlash}
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
	::llc::string				PartFileName					= {};
	::llc::string				PathToWriteTo					= {};
	::llc::SLoadCache						LoadCache						= {};
#if defined SEASON_ENABLE_VERIFICATION
	::llc::string				Verify							= {};
#endif
};

::llc::err_t			writePart						(::SWriteCache & blockCache, const ::SSplitParams & params, const ::llc::vcu0_t & partBytes)		{
	::llc::string					& partFileName					= blockCache.PartFileName					;
	::llc::string					& pathToWriteTo					= blockCache.PathToWriteTo					;
	::llc::clear(partFileName, pathToWriteTo, blockCache.LoadCache.Deflated, blockCache.LoadCache.Encrypted);
	if_fail_fef(::blt::tableFileName(partFileName, params.DeflatedOutput ? ::blt::DATABASE_HOST_DEFLATE : ::blt::DATABASE_HOST_LOCAL, params.EncryptionKey.size() > 0, params.DBName), "%s", "??");
	if_fail_fe(pathToWriteTo.append(partFileName));
	return ::llc::fileFromMemorySecure(blockCache.LoadCache, pathToWriteTo, params.EncryptionKey.cu8(), params.DeflatedOutput, partBytes);
}

// Splits a file.json into file.#blocksize.db/file.##.json parts.
int										main							(int argc, char ** argv)		{
	::SSplitParams								params							= {};
	if_fail_fef(::loadParams(params, argc, argv), "%s", "");
	::llc::string					bytesToSave						= {};
	{
		::llc::SJSONFile							jsonFileToSplit					= {};
		if_fail_fef(::llc::jsonFileRead(jsonFileToSplit, params.FileNameSrc), "Failed to load file: %.*s.", GPK_VIEW_ARGS(params.FileNameSrc));
		if_true_fef(0 == jsonFileToSplit.Reader.Tree.size(), "Invalid input format. %s", "File content is not a JSON array.");
		if_true_fef(jsonFileToSplit.Reader.Token[0].Type != ::llc::JSON_TYPE_ARRAY, "Invalid input format. %s", ::llc::get_value_label(jsonFileToSplit.Reader.Token[0].Type).begin());
		info_printf("Loaded file: %.*s. Size: %u bytes.", GPK_VIEW_ARGS(params.FileNameSrc), jsonFileToSplit.Bytes.size());
		if_fail_fef(::llc::jsonWrite(jsonFileToSplit.Reader.Tree[0], jsonFileToSplit.Reader.View, bytesToSave), "Out of memory or corrupt json file: %.*s.", GPK_VIEW_ARGS(params.FileNameSrc));
	}

	::SWriteCache								blockCache						= {};
	if_fail_fe(::writePart(blockCache, params, bytesToSave.cu8()));
	return 0;
}
