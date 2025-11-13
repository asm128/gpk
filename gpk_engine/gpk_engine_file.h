#include "gpk_label_manager.h"
#include "gpk_n2.h"
#include "gpk_file.h"

#ifndef GPK_ENGINE_FILE_H_23627
#define GPK_ENGINE_FILE_H_23627

namespace gpk 
{
	struct SRAMFolder {
		::gpk::aobj<::gpk::vcsc_t>		Names;
		::gpk::apod<::gpk::rangeu2_t>	Ranges;
		::gpk::au0_t					Contents;
		::gpk::au0_t					LoadCache;

		::gpk::error_t					AddFile			(::gpk::vcsc_t fileName, ::gpk::vu8 fileContents) {
			const uint32_t						ssizes[]		= {Ranges.size(), Contents.size(), Names.size()};
			uint32_t							itemIndex;
			::gpk::resize(::gpk::min<const uint32_t>(ssizes), Ranges, Contents, Names);
			gpk_necall(Ranges.push_back({Contents.size(), fileContents.size()})	, "Failed to store file: %s", toString(fileName).begin());
			gpk_necall(Contents.append(fileContents)							, "Failed to store file: %s", toString(fileName).begin());
			if(failed(itemIndex = Names.push_back(fileName))) {
				error_printf("Failed to store file: %s", toString(fileName).begin());
				gpk_necs(Contents.resize(Ranges[Ranges.size() - 1].Offset));
				gpk_necs(Ranges.resize(Ranges.size() - 1));
			}
			return itemIndex;
		}
		::gpk::error_t					AddFile			(::gpk::vcsc_t filePath) {
			LoadCache.clear();
			gpk_necs(gpk::fileToMemory(filePath, LoadCache));
			return AddFile(filePath, LoadCache);
		}
		::gpk::error_t					GetFile			(uint32_t index, ::gpk::vcu0_t & view)		const	{ return (view = {&Contents[Ranges[index].Offset], Ranges[index].Count}).size(); }
		::gpk::error_t					GetFile			(::gpk::vcsc_t filePath, ::gpk::vcu0_t & view)	const	{
			::gpk::error_t						index			= -1;
			gpk_necall(index = ::gpk::find(filePath, ::gpk::vcvcc{Names}), "File not found: '%s'", toString(filePath).begin());
			return GetFile(index, view);
		}
	};


} // namespace

#endif // GPK_ENGINE_FILE_H_23627