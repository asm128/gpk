#include "gpk_array_obj.h"
#include "gpk_lifecycle.h"
#include "gpk_range.h"
#include "gpk_string.h"

#ifndef GPK_DEFLATE_H_23627
#define GPK_DEFLATE_H_23627

namespace gpk
{
#if defined(GPK_ESP32) || defined(GPK_ARDUINO)
	stxp	u2_t		DEFLATE_DEFAULT_CHUNK_SIZE		= 1024 * 4;
	stxp	u2_t		INFLATE_DEFAULT_CHUNK_SIZE		= 1024 * 4;
#else
	stxp	u2_t		DEFLATE_DEFAULT_CHUNK_SIZE		= 1024 * 32;
	stxp	u2_t		INFLATE_DEFAULT_CHUNK_SIZE		= 1024 * 32;
#endif	
	struct SDeflateSettings {
		u2_t				SearchDepth			= 256;
		u0_t				WindowBits			= 15;
		u0_t				HashBits				= 16;
		u0_t				CompressionLevel		= 3;
	};

	struct SZipStream {
		apod<u0_t>			Window, Previous, Head;
		vcu0_t				Stream;
		SDeflateSettings	Settings				= {};
		LIFECYCLE_STATE		Status					= LIFECYCLE_STATE_IDLE;
	};


	err_t			deflateInit				(SZipStream & output, vcu0_t input, SDeflateSettings settings = {});
	err_t			inflateInit				(SZipStream & output, vcu0_t input);

	err_t			arrayDeflate			(vcu0_t inflated, au0_t & deflated, SDeflateSettings settings, u2_c chunkSize = DEFLATE_DEFAULT_CHUNK_SIZE);
	err_t			arrayDeflate			(vcu0_t inflated, au0_t & deflated, u2_c chunkSize = DEFLATE_DEFAULT_CHUNK_SIZE);
	err_t			arrayInflate			(vcu0_t deflated, au0_t & inflated, u2_c chunkSize = INFLATE_DEFAULT_CHUNK_SIZE);

	stin	err_t	arrayDeflate			(vcs0_t inflated, au0_t & deflated, u2_c chunkSize = DEFLATE_DEFAULT_CHUNK_SIZE)	{ rtrn arrayDeflate(*(vcu0_t*)&inflated, deflated, chunkSize); }
	stin	err_t	arrayInflate			(vcs0_t deflated, au0_t & inflated, u2_c chunkSize = INFLATE_DEFAULT_CHUNK_SIZE)	{ rtrn arrayInflate(*(vcu0_t*)&deflated, inflated, chunkSize); }
	stin	err_t	arrayDeflate			(vcu0_t inflated, as0_t & deflated, u2_c chunkSize = DEFLATE_DEFAULT_CHUNK_SIZE)	{ rtrn arrayDeflate(inflated, *(au0_t*)&deflated, chunkSize); }
	stin	err_t	arrayInflate			(vcu0_t deflated, as0_t & inflated, u2_c chunkSize = INFLATE_DEFAULT_CHUNK_SIZE)	{ rtrn arrayInflate(deflated, *(au0_t*)&inflated, chunkSize); }
	stin	err_t	arrayDeflate			(vcs0_t inflated, as0_t & deflated, u2_c chunkSize = DEFLATE_DEFAULT_CHUNK_SIZE)	{ rtrn arrayDeflate(*(vcu0_t*)&inflated, *(au0_t*)&deflated, chunkSize); }
	stin	err_t	arrayInflate			(vcs0_t deflated, as0_t & inflated, u2_c chunkSize = INFLATE_DEFAULT_CHUNK_SIZE)	{ rtrn arrayInflate(*(vcu0_t*)&deflated, *(au0_t*)&inflated, chunkSize); }

	err_t			crcGenerate				(vcu0_t bytes, u3_t & crc);
	err_t			crcVerifyAndRemove		(au0_t & bytes);
	err_t			crcGenerateAndAppend	(au0_t & bytes);
} // namespace

#endif // GPK_DEFLATE_H_23627


