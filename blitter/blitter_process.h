#include "blitter.h"
#include "llc_minmax.h"

#ifndef BLITTER_PROCESS_H_29038679283
#define BLITTER_PROCESS_H_29038679283

namespace blt
{
	struct SRangeBlockInfo {
		::llc::vcst_t		OutputRecords			= {};
		::llc::minmaxu2_t	RelativeIndices			= {};
		u2_t 			BlockIndex				= 0;
		u2_t 			BlockId					= 0;
	};

	::llc::err_t		recordRange
		( ::llc::SLoadCache						& loadCache
		, ::blt::TNamedBlitterDB				& database
		, const ::llc::rangeu3_t				& range
		, const ::llc::vcst_t					& folder
		, ::llc::aobj<::blt::SRangeBlockInfo>	& output_records
		, ::llc::rangeu2_t						& blockRange
		);


	::llc::err_t		recordLoad
		( ::llc::SLoadCache			& loadCache
		, ::blt::TNamedBlitterDB	& database
		, const u3_t 			absoluteIndex
		, u2_t 					& relativeIndex
		, u2_t 					& blockIndex
		, const ::llc::vcst_t		& folder
		);

	::llc::err_t		recordGet
		( ::llc::SLoadCache			& loadCache
		, ::blt::TNamedBlitterDB	& database
		, const u3_t 			absoluteIndex
		, ::llc::vcst_t				& output_record
		, u2_t 					& relativeIndex
		, u2_t 					& blockIndex
		, const ::llc::vcst_t		& folder
		);

	stin ::llc::err_t	recordGet
		( ::llc::SLoadCache			& loadCache
		, ::blt::TNamedBlitterDB	& database
		, const u3_t 			absoluteIndex
		, ::llc::vcst_t				& output_record
		, const ::llc::vcst_t		& folder
	) {
		u2_t 					indexRelative		= (u2_t )-1;
		u2_t 					indexBlock			= (u2_t )-1;
		return ::blt::recordGet(loadCache, database, absoluteIndex, output_record, indexRelative, indexBlock, folder);
	}
} // namespace

#endif // BLITTER_PROCESS_H_29038679283
