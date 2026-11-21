#include "blitter.h"
#include "gpk_minmax.h"

#ifndef BLITTER_PROCESS_H_29038679283
#define BLITTER_PROCESS_H_29038679283

namespace blt
{
	struct SRangeBlockInfo {
		::gpk::vcst_t		OutputRecords			= {};
		::gpk::minmaxu2_t	RelativeIndices			= {};
		u2_t 			BlockIndex				= 0;
		u2_t 			BlockId					= 0;
	};

	::gpk::err_t		recordRange
		( ::gpk::SLoadCache						& loadCache
		, ::blt::TNamedBlitterDB				& database
		, const ::gpk::rangeu3_t				& range
		, const ::gpk::vcst_t					& folder
		, ::gpk::aobj<::blt::SRangeBlockInfo>	& output_records
		, ::gpk::rangeu2_t						& blockRange
		);


	::gpk::err_t		recordLoad
		( ::gpk::SLoadCache			& loadCache
		, ::blt::TNamedBlitterDB	& database
		, const u3_t 			absoluteIndex
		, u2_t 					& relativeIndex
		, u2_t 					& blockIndex
		, const ::gpk::vcst_t		& folder
		);

	::gpk::err_t		recordGet
		( ::gpk::SLoadCache			& loadCache
		, ::blt::TNamedBlitterDB	& database
		, const u3_t 			absoluteIndex
		, ::gpk::vcst_t				& output_record
		, u2_t 					& relativeIndex
		, u2_t 					& blockIndex
		, const ::gpk::vcst_t		& folder
		);

	stin ::gpk::err_t	recordGet
		( ::gpk::SLoadCache			& loadCache
		, ::blt::TNamedBlitterDB	& database
		, const u3_t 			absoluteIndex
		, ::gpk::vcst_t				& output_record
		, const ::gpk::vcst_t		& folder
	) {
		u2_t 					indexRelative		= (u2_t )-1;
		u2_t 					indexBlock			= (u2_t )-1;
		return ::blt::recordGet(loadCache, database, absoluteIndex, output_record, indexRelative, indexBlock, folder);
	}
} // namespace

#endif // BLITTER_PROCESS_H_29038679283
