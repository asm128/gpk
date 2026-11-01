#include "gpk_label_manager.h"
#include "gpk_n2.h"
#include "gpk_file.h"
#include "gpk_range.h"

#ifndef GPK_ENGINE_FILE_H_23627
#define GPK_ENGINE_FILE_H_23627

namespace gpk 
{
	struct SRAMFolder {
		::gpk::aobj<::gpk::vcst_t>		Names;
		::gpk::apod<::gpk::rangeu2_t>	Ranges;
		::gpk::au0_t					Contents;
		::gpk::au0_t					LoadCache;

		gpk::err_t 				AddFile			(::gpk::vcst_t fileName, ::gpk::vu0_t fileContents) {
			const u2_t						ssizes[]		= {Ranges.size(), Contents.size(), Names.size()};
			u2_t							itemIndex;
			::gpk::resize(::gpk::min<const u2_t>(ssizes), Ranges, Contents, Names);
			if_fail_fef(Ranges.push_back({Contents.size(), fileContents.size()})	, "Failed to store file: %.*s", GPK_VIEW_ARGS(fileName));
			if_fail_fef(Contents.append(fileContents)							, "Failed to store file: %.*s", GPK_VIEW_ARGS(fileName));
			if(failed(itemIndex = Names.push_back(fileName))) {
				error_printf("Failed to store file: %.*s", GPK_VIEW_ARGS(fileName));
				if_fail_fe(Contents.resize(Ranges[Ranges.size() - 1].Offset));
				if_fail_fe(Ranges.resize(Ranges.size() - 1));
			}
			return itemIndex;
		}
		gpk::err_t 				AddFile			(::gpk::vcst_t filePath) {
			LoadCache.clear();
			if_fail_fe(gpk::fileToMemory(filePath, LoadCache));
			return AddFile(filePath, LoadCache);
		}
		gpk::err_t 				GetFile			(u2_t index, ::gpk::vcu0_t & view)		const	{ return (view = {&Contents[Ranges[index].Offset], Ranges[index].Count}).size(); }
		gpk::err_t 				GetFile			(::gpk::vcst_t filePath, ::gpk::vcu0_t & view)	const	{
			gpk::err_t 					index			= -1;
			if_fail_fef(index = ::gpk::find(filePath, ::gpk::vcvcc{Names}), "File not found: '%.*s'", GPK_VIEW_ARGS(filePath));
			return GetFile(index, view);
		}
	};


} // namespace

#endif // GPK_ENGINE_FILE_H_23627