#include "gpk_image_color.h"
#include "gpk_array_static.h"
#include "gpk_color_type.h"
#include "gpk_array_color.h"
#include "gpk_array.h"


#ifndef GPK_PNG_H_0928374982374
#define GPK_PNG_H_0928374982374

namespace gpk
{
#pragma pack(push, 1)
	struct SPNGChunk {
		uint8_t			Type				[4]	= {};
		uint32_t		CRC						= {};
		::gpk::au8		Data					= {};
	};


	struct SPNGIHDR {
		::gpk::n2u32	Size					= {};
		int8_t			BitDepth				= 0;
		COLOR_TYPE		ColorType				= COLOR_TYPE_GRAYSCALE;
		int8_t			MethodCompression		= 0;
		int8_t			MethodFilter			= 0;
		int8_t			MethodInterlace			= 0;
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
		char							Signature	[8]	= {};
		::gpk::aobj<SPNGChunk>			Chunks			;
		::gpk::au8						Deflated		;
		::gpk::au8						Inflated		;
		::gpk::au8						Filters			;
		::gpk::aau8						Scanlines		;
		::gpk::a8bgr					Palette			;
		::gpk::n2u32					Adam7Sizes	[7]	= {};
		//::gpk::SPNGFeature				Feature			= {};
		::gpk::SPNGIHDR					Header			= {};
		::gpk::asti32<PNG_TAG_COUNT>	Feature			;
	};
#pragma pack(pop)

	uint32_t				update_crc		(const ::gpk::vcu8 & buf, uint32_t crc)		;
	stainli	uint32_t		get_crc			(const ::gpk::vcu8 & buf)					{ return update_crc(buf, 0xffffffffL) ^ 0xffffffffL; }

	::gpk::error_t			pngFileLoad		(::gpk::SPNGData & pngCache, const ::gpk::vcs	& filename	);
	::gpk::error_t			pngFileLoad		(::gpk::SPNGData & pngCache, const ::gpk::vcu8	& source	);
	::gpk::error_t			pngFileLoad		(::gpk::SPNGData & pngCache, const ::gpk::vcs	& filename	, ::gpk::img8bgra & out_Texture)	;
	::gpk::error_t			pngFileLoad		(::gpk::SPNGData & pngCache, const ::gpk::vcu8	& source	, ::gpk::img8bgra & out_Texture)	;
				 
	::gpk::error_t			pngDecode		(::gpk::SPNGData & pngData, ::gpk::img8bgra & out_Texture);
	::gpk::error_t			pngDecode		(::gpk::SPNGData & pngData, ::gpk::imgu16	& out_Texture);
	::gpk::error_t			pngDecode		(::gpk::SPNGData & pngData, ::gpk::imgu8	& out_Texture);
	::gpk::error_t			pngFileWrite	(const ::gpk::v28bgra & out_ImageView, ::gpk::au8 & out_Bytes);

	stainli	::gpk::error_t	pngFileLoad		(const ::gpk::vcs	& filename	, ::gpk::img8bgra & out_Texture)	{ ::gpk::SPNGData tempCache; return pngFileLoad(tempCache, filename	, out_Texture); }
	stainli	::gpk::error_t	pngFileLoad		(const ::gpk::vcu8	& source	, ::gpk::img8bgra & out_Texture)	{ ::gpk::SPNGData tempCache; return pngFileLoad(tempCache, source	, out_Texture); }
} // namespace

#endif // GPK_PNG_H_0928374982374
