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

		gpk::err_t					AddFile			(::gpk::vcsc_t fileName, ::gpk::vu0_t fileContents) {
			const ::gpk::u2_t						ssizes[]		= {Ranges.size(), Contents.size(), Names.size()};
			::gpk::u2_t							itemIndex;
			::gpk::resize(::gpk::min<const ::gpk::u2_t>(ssizes), Ranges, Contents, Names);
			if_fail_fef(Ranges.push_back({Contents.size(), fileContents.size()})	, "Failed to store file: %s", toString(fileName).begin());
			if_fail_fef(Contents.append(fileContents)							, "Failed to store file: %s", toString(fileName).begin());
			if(failed(itemIndex = Names.push_back(fileName))) {
				error_printf("Failed to store file: %s", toString(fileName).begin());
				if_fail_fe(Contents.resize(Ranges[Ranges.size() - 1].Offset));
				if_fail_fe(Ranges.resize(Ranges.size() - 1));
			}
			return itemIndex;
		}
		gpk::err_t					AddFile			(::gpk::vcsc_t filePath) {
			LoadCache.clear();
			if_fail_fe(gpk::fileToMemory(filePath, LoadCache));
			return AddFile(filePath, LoadCache);
		}
		gpk::err_t					GetFile			(::gpk::u2_t index, ::gpk::vcu0_t & view)		const	{ return (view = {&Contents[Ranges[index].Offset], Ranges[index].Count}).size(); }
		gpk::err_t					GetFile			(::gpk::vcsc_t filePath, ::gpk::vcu0_t & view)	const	{
			gpk::err_t						index			= -1;
			if_fail_fef(index = ::gpk::find(filePath, ::gpk::vcvcc{Names}), "File not found: '%s'", toString(filePath).begin());
			return GetFile(index, view);
		}
	};


} // namespace

#endif // GPK_ENGINE_FILE_H_23627