#define GPK_CONSOLE_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_log.h"
#include "gpk_storage.h"
#include "gpk_json.h"
#include "gpk_parse.h"
#include "gpk_find.h"
#include "gpk_deflate.h"

static constexpr const uint32_t			DEFAULT_BLOCK_SIZE				= 1024;

::gpk::error_t							jsonArraySplit					(const ::gpk::SJSONNode & jsonArrayToSplit, const ::gpk::view_array<::gpk::view_const_string> & jsonViews, const uint32_t blockSize, ::gpk::array_obj<::gpk::array_pod<char_t>> & outputJsons)		{
	const uint32_t								remainder						= jsonArrayToSplit.Children.size() % blockSize;
	const uint32_t								countParts						= jsonArrayToSplit.Children.size() / blockSize + one_if(remainder);
	outputJsons.resize(countParts);
	uint32_t									iSourceRecord					= 0;
	for(uint32_t iPart = 0; iPart < outputJsons.size(); ++iPart) {
		::gpk::array_pod<char_t>					& outputJson					= outputJsons[iPart];
		outputJson.push_back('[');
		for(uint32_t iPartRecord = 0, countPartRecords = (remainder && iPart == countParts - 1) ? remainder : blockSize
			; iPartRecord < countPartRecords
			; ++iPartRecord) {
			::gpk::jsonWrite(jsonArrayToSplit.Children[iSourceRecord++], jsonViews, outputJson);
			if(iPartRecord < countPartRecords - 1)
				outputJson.push_back(',');
		}
		outputJson.push_back(']');
	}
	return 0;
}


// Splits a file into file.split.## parts.
int										main							(int argc, char ** argv)		{
	for(int32_t iArg = 5; iArg < argc; ++iArg)
		info_printf("Unknown parameter: %s.", argv[iArg]);
	ree_if(2 > argc, "Usage:\n\t%s [filename] [blockSize] [deflated input (1:0)] [deflated output (1:0)]", argv[0]);
	ree_if(65535 < argc, "Invalid parameter count: %u.", (uint32_t)argc);
	const ::gpk::view_const_string				fileNameSrc						= {argv[1], (uint32_t)-1};	// First parameter is the only parameter, which is the name of the source file to be split.
	uint32_t									blockSize						= 0;
	if(argc > 2) {	// load block size param
		::gpk::parseIntegerDecimal({argv[2], (uint32_t)-1}, &blockSize);
		info_printf("Using block size: %u.", blockSize);
	}
	bool										deflatedOutput					= (argc <= 3 || argv[3][0] != '0');
	//bool										deflatedinput					= (argc <= 4 || argv[4][0] == '0');
	if(0 == blockSize)
		blockSize								= DEFAULT_BLOCK_SIZE;
	info_printf("Deflated output: %s", deflatedOutput ? "true" : "false");
	
	::gpk::SJSONFile							jsonFileToSplit					= {};
	gpk_necall(::gpk::jsonFileRead(jsonFileToSplit, fileNameSrc), "Failed to load file: %s.", fileNameSrc.begin());
	::gpk::array_obj<::gpk::array_pod<char_t>>	outputJsons;
	ree_if(0 == jsonFileToSplit.Reader.Tree.size() || jsonFileToSplit.Reader.Object[0].Type != ::gpk::JSON_TYPE_ARRAY, "Invalid input format. %s", "JSON contents not an array.");
	gpk_necall(::jsonArraySplit(*jsonFileToSplit.Reader.Tree[0], jsonFileToSplit.Reader.View , blockSize, outputJsons), "%s", "Unknown error!");
	::gpk::error_t								indexOfDot						= ::gpk::rfind('.', fileNameSrc);
	::gpk::error_t								indexOfLastSlash				= ::gpk::findLastSlash(fileNameSrc);
	::gpk::array_pod<char_t>					partFileName					= {};
	char										fileNameDigits	[32]			= {};
	::gpk::array_pod<char_t>					deflated						= {};
	for(uint32_t iPart = 0; iPart < outputJsons.size(); ++iPart) {
		partFileName							= (indexOfDot > indexOfLastSlash) ? ::gpk::view_const_string{fileNameSrc.begin(), (uint32_t)indexOfDot} : fileNameSrc;
		sprintf_s(fileNameDigits, ".%u", iPart);
		partFileName.append(::gpk::view_const_string{fileNameDigits});
		const ::gpk::array_pod<char_t>				& partBytes						= outputJsons[iPart];
		if(deflatedOutput){
			partFileName.append(".zson");
			deflated.append((char*)&partBytes.size(), sizeof(uint32_t));
			gpk_necall(::gpk::arrayDeflate(partBytes, deflated), "Failed to deflate part: %u.", iPart);
			gpk_necall(::gpk::fileFromMemory({partFileName.begin(), partFileName.size()}, deflated), "Failed to write part: %u.", iPart);
			deflated.clear();
		}
		else {
			partFileName.append(".json");
			gpk_necall(::gpk::fileFromMemory({partFileName.begin(), partFileName.size()}, partBytes), "Failed to write part: %u.", iPart);
		}
	}
	return 0; 
}
