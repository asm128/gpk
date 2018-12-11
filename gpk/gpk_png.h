#include "gpk_array.h"
#include "gpk_color.h"
#include "gpk_image.h"
#include "gpk_array_static.h"

#ifndef GPK_PNG_H_0928374982374
#define GPK_PNG_H_0928374982374

namespace gpk
{
#pragma pack(push, 1)
	struct SPNGChunk {
						uint8_t													Type				[4]			= {};
						uint32_t												CRC								= {};
						::gpk::array_pod<uint8_t>								Data							= {};
	};

	struct SPNGIHDR {
						::gpk::SCoord2<uint32_t>								Size							= {};
						int8_t													BitDepth						= 0;
						int8_t													ColorType						= 0;
						int8_t													MethodCompression				= 0;
						int8_t													MethodFilter					= 0;
						int8_t													MethodInterlace					= 0;
	};

	enum PNG_TAG 
		{	PNG_TAG_tEXt	= 0
		,	PNG_TAG_zTXt
		,	PNG_TAG_bKGD
		,	PNG_TAG_cHRM
		,	PNG_TAG_dSIG
		,	PNG_TAG_eXIf
		,	PNG_TAG_gAMA
		,	PNG_TAG_hIST
		,	PNG_TAG_iCCP
		,	PNG_TAG_iTXt
		,	PNG_TAG_pHYs
		,	PNG_TAG_sBIT
		,	PNG_TAG_sPLT
		,	PNG_TAG_sRGB
		,	PNG_TAG_sTER
		,	PNG_TAG_tIME
		,	PNG_TAG_tRNS
		,	PNG_TAG_fcTL
		,	PNG_TAG_fdAT
		,	PNG_TAG_acTL
		,	PNG_TAG_COUNT
		};

	struct SPNGData {
						char													Signature			[8]			= {};
						::gpk::array_obj<SPNGChunk>								Chunks							;
						::gpk::array_pod<ubyte_t>								Deflated						;
						::gpk::array_pod<ubyte_t>								Inflated						;
						::gpk::array_pod<ubyte_t>								Filters							;
						::gpk::array_obj<::gpk::array_pod<ubyte_t>>				Scanlines						;
						::gpk::array_pod<::gpk::color_bgr<uint8_t>>				Palette							;
						::gpk::SCoord2<uint32_t>								Adam7Sizes			[7]			= {};
						//::gpk::SPNGFeature										Feature							= {};
						::gpk::SPNGIHDR											Header							= {};
						::gpk::array_static<int32_t, PNG_TAG_COUNT>				Feature							;
	};
#pragma pack(pop)

					uint32_t												update_crc						(const ::gpk::view_array<const ubyte_t>& buf, uint32_t crc)		;
	static inline	uint32_t												get_crc							(const ::gpk::view_array<const ubyte_t>& buf)					{ return update_crc(buf, 0xffffffffL) ^ 0xffffffffL; }

					::gpk::error_t											pngFileLoad						(::gpk::SPNGData& pngCache, const ::gpk::view_const_string				& filename		, ::gpk::SImage	<::gpk::SColorBGRA>& out_Texture)	;
					::gpk::error_t											pngFileLoad						(::gpk::SPNGData& pngCache, const ::gpk::view_array<const ubyte_t>		& source		, ::gpk::SImage	<::gpk::SColorBGRA>& out_Texture)	;
					::gpk::error_t											pngFileWrite					(const ::gpk::view_grid<::gpk::SColorBGRA> & out_ImageView, ::gpk::array_pod<ubyte_t>& out_Bytes);

	static inline	::gpk::error_t											pngFileLoad						(const ::gpk::view_const_string			& filename	, ::gpk::SImage	<::gpk::SColorBGRA>& out_Texture)	{ ::gpk::SPNGData tempCache; return pngFileLoad(tempCache, filename	, out_Texture); }
	static inline	::gpk::error_t											pngFileLoad						(const ::gpk::view_array<const ubyte_t>	& source	, ::gpk::SImage	<::gpk::SColorBGRA>& out_Texture)	{ ::gpk::SPNGData tempCache; return pngFileLoad(tempCache, source	, out_Texture); }
} // namespace

#endif // GPK_PNG_H_0928374982374
