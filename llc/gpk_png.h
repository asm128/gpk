#include "gpk_img_color.h"
#include "gpk_array_static.h"
#include "gpk_color_type.h"
#include "gpk_apod_color.h"
#include "gpk_array.h"
#include "gpk_grid_color.h"

#ifndef GPK_PNG_H_23627
#define GPK_PNG_H_23627

namespace gpk
{
#pragma pack(push, 1)
	struct SPNGChunk {
		uint8_t			Type				[4]	= {};
		uint32_t		CRC						= {};
		::gpk::au0_t	Data					= {};
	};


	struct SPNGIHDR {
		::gpk::n2u2_t	Size					= {};
		int8_t			BitDepth				= 0;
		COLOR_TYPE		ColorType				= COLOR_TYPE_GRAYSCALE;
		int8_t			MethodCompression		= 0;
		int8_t			MethodFilter			= 0;
		int8_t			MethodInterlace			= 0;
	};

	GDEFINE_ENUM_TYPE(PNG_TAG, uint8_t);
	GDEFINE_ENUM_VALUE(PNG_TAG, tEXt ,  0);
	GDEFINE_ENUM_VALUE(PNG_TAG, zTXt ,  1);
	GDEFINE_ENUM_VALUE(PNG_TAG, bKGD ,  2);
	GDEFINE_ENUM_VALUE(PNG_TAG, cHRM ,  3);
	GDEFINE_ENUM_VALUE(PNG_TAG, dSIG ,  4);
	GDEFINE_ENUM_VALUE(PNG_TAG, eXIf ,  5);
	GDEFINE_ENUM_VALUE(PNG_TAG, gAMA ,  6);
	GDEFINE_ENUM_VALUE(PNG_TAG, hIST ,  7);
	GDEFINE_ENUM_VALUE(PNG_TAG, iCCP ,  8);
	GDEFINE_ENUM_VALUE(PNG_TAG, iTXt ,  9);
	GDEFINE_ENUM_VALUE(PNG_TAG, pHYs , 10);
	GDEFINE_ENUM_VALUE(PNG_TAG, sBIT , 11);
	GDEFINE_ENUM_VALUE(PNG_TAG, sPLT , 12);
	GDEFINE_ENUM_VALUE(PNG_TAG, sRGB , 13);
	GDEFINE_ENUM_VALUE(PNG_TAG, sTER , 14);
	GDEFINE_ENUM_VALUE(PNG_TAG, tIME , 15);
	GDEFINE_ENUM_VALUE(PNG_TAG, tRNS , 16);
	GDEFINE_ENUM_VALUE(PNG_TAG, fcTL , 17);
	GDEFINE_ENUM_VALUE(PNG_TAG, fdAT , 18);
	GDEFINE_ENUM_VALUE(PNG_TAG, acTL , 19);
	GDEFINE_ENUM_VALUE(PNG_TAG, COUNT, 20);
	
	struct SPNGData {
		sc_t							Signature	[8]	= {};
		::gpk::aobj<SPNGChunk>			Chunks			;
		::gpk::au0_t					Deflated		;
		::gpk::au0_t					Inflated		;
		::gpk::au0_t					Filters			;
		::gpk::aau8						Scanlines		;
		::gpk::a8bgr					Palette			;
		::gpk::n2u2_t					Adam7Sizes	[7]	= {};
		//::gpk::SPNGFeature				Feature			= {};
		::gpk::SPNGIHDR					Header			= {};
		::gpk::asti32<PNG_TAG_COUNT>	Feature			;
	};
#pragma pack(pop)

	uint32_t				update_crc	(const ::gpk::vcu0_t & buf, uint32_t crc)		;
	stainli	uint32_t		get_crc		(const ::gpk::vcu0_t & buf)					{ return update_crc(buf, 0xffffffffL) ^ 0xffffffffL; }

	::gpk::error_t			pngDecode	(::gpk::SPNGData & pngData, ::gpk::g8bgra	out_View);
	::gpk::error_t			pngDecode	(::gpk::SPNGData & pngData, ::gpk::gu16		out_View);
	::gpk::error_t			pngDecode	(::gpk::SPNGData & pngData, ::gpk::gu8		out_View);
	::gpk::error_t			pngDecode	(::gpk::SPNGData & pngData, ::gpk::au0_t	& out_Data);
	::gpk::error_t			pngDecode	(::gpk::SPNGData & pngData, ::gpk::au0_t	& out_Data, ::gpk::g8bgra & out_View); 
	::gpk::error_t			pngDecode	(::gpk::SPNGData & pngData, ::gpk::au0_t	& out_Data, ::gpk::gu16   & out_View); 
	::gpk::error_t			pngDecode	(::gpk::SPNGData & pngData, ::gpk::au0_t	& out_Data, ::gpk::gu8    & out_View); 
	::gpk::error_t			pngDecode	(::gpk::SPNGData & pngData, ::gpk::img8bgra & out_Image);
	::gpk::error_t			pngDecode	(::gpk::SPNGData & pngData, ::gpk::imgu16	& out_Image);
	::gpk::error_t			pngDecode	(::gpk::SPNGData & pngData, ::gpk::imgu8	& out_Image);
	
	::gpk::error_t			pngFileLoad	(::gpk::SPNGData & pngData, const ::gpk::vcst_t	& filename	);
	::gpk::error_t			pngFileLoad	(::gpk::SPNGData & pngData, const ::gpk::vcu0_t	& source	);
	::gpk::error_t			pngFileLoad	(::gpk::SPNGData & pngData, const ::gpk::vcst_t	& filename	, ::gpk::img8bgra & out_Texture)	;
	::gpk::error_t			pngFileLoad	(::gpk::SPNGData & pngData, const ::gpk::vcu0_t	& source	, ::gpk::img8bgra & out_Texture)	;
	::gpk::error_t			pngFileLoad	(::gpk::SPNGData & pngData, const ::gpk::vcst_t	& filename	, ::gpk::au0_t & out_Data, ::gpk::g8bgra & out_View)	;
	::gpk::error_t			pngFileLoad	(::gpk::SPNGData & pngData, const ::gpk::vcu0_t	& source	, ::gpk::au0_t & out_Data, ::gpk::g8bgra & out_View)	;
	::gpk::error_t			pngFileLoad	(::gpk::SPNGData & pngData, const ::gpk::vcst_t	& filename	, ::gpk::au0_t & out_Data)	;
	::gpk::error_t			pngFileLoad	(::gpk::SPNGData & pngData, const ::gpk::vcu0_t	& source	, ::gpk::au0_t & out_Data)	;

	::gpk::error_t			pngFileWrite(const ::gpk::gc8bgra & out_ImageView, ::gpk::au0_t & out_Bytes);

	stainli	::gpk::error_t	pngFileLoad	(const ::gpk::vcst_t	& filename	, ::gpk::img8bgra & out_Texture)					{ ::gpk::SPNGData tempCache; return pngFileLoad(tempCache, filename	, out_Texture); }
	stainli	::gpk::error_t	pngFileLoad	(const ::gpk::vcu0_t	& source	, ::gpk::img8bgra & out_Texture)					{ ::gpk::SPNGData tempCache; return pngFileLoad(tempCache, source	, out_Texture); }
	stainli	::gpk::error_t	pngFileLoad	(const ::gpk::vcst_t	& filename	, ::gpk::au0_t & out_Data, ::gpk::g8bgra & out_View)	{ ::gpk::SPNGData tempCache; return pngFileLoad(tempCache, filename	, out_Data, out_View); }
	stainli	::gpk::error_t	pngFileLoad	(const ::gpk::vcu0_t	& source	, ::gpk::au0_t & out_Data, ::gpk::g8bgra & out_View)	{ ::gpk::SPNGData tempCache; return pngFileLoad(tempCache, source	, out_Data, out_View); }
	stainli	::gpk::error_t	pngFileLoad	(const ::gpk::vcst_t	& filename	, ::gpk::au0_t & out_Data)							{ ::gpk::SPNGData tempCache; return pngFileLoad(tempCache, filename	, out_Data); }
	stainli	::gpk::error_t	pngFileLoad	(const ::gpk::vcu0_t	& source	, ::gpk::au0_t & out_Data)							{ ::gpk::SPNGData tempCache; return pngFileLoad(tempCache, source	, out_Data); }
} // namespace

#endif // GPK_PNG_H_23627
