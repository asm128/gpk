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
		u0_t			Type				[4]	= {};
		u2_t			CRC						= {};
		au0_t			Data					= {};
	};


	struct SPNGIHDR {
		n2u2_t			Size					= {};
		s0_t			BitDepth				= 0;
		COLOR_TYPE		ColorType				= COLOR_TYPE_GRAYSCALE;
		s0_t			MethodCompression		= 0;
		s0_t			MethodFilter			= 0;
		s0_t			MethodInterlace			= 0;
	};

	GDEFINE_ENUM_TYPE(PNG_TAG, u0_t);
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
		sc_t					Signature	[8]	= {};
		aobj<SPNGChunk>			Chunks			;
		au0_t					Deflated		;
		au0_t					Inflated		;
		au0_t					Filters			;
		aau0_t					Scanlines		;
		a8bgr					Palette			;
		n2u2_t					Adam7Sizes	[7]	= {};
		//SPNGFeature			Feature			= {};
		SPNGIHDR				Header			= {};
		asts2_t<PNG_TAG_COUNT>	Feature			;
	};
#pragma pack(pop)

			u2_t		pngCRCUpdate	(vcu0_t buf, u2_t crc);
	stin	u2_t		pngCRCGet		(vcu0_t buf)			{ rtrn pngCRCUpdate(buf, 0xffffffffL) ^ 0xffffffffL; }
		
			err_t 		pngDecode		(SPNGData & pngData, gu0_t		out_View);
			err_t 		pngDecode		(SPNGData & pngData, gu1_t		out_View);
			err_t 		pngDecode		(SPNGData & pngData, g8bgra		out_View);
			err_t 		pngDecode		(SPNGData & pngData, au0_t		& out_Data);
			err_t 		pngDecode		(SPNGData & pngData, au0_t		& out_Data, gu0_t	& out_View); 
			err_t 		pngDecode		(SPNGData & pngData, au0_t		& out_Data, gu1_t	& out_View); 
			err_t 		pngDecode		(SPNGData & pngData, au0_t		& out_Data, g8bgra	& out_View); 
			err_t 		pngDecode		(SPNGData & pngData, imgu0_t	& out_Image);
			err_t 		pngDecode		(SPNGData & pngData, imgu1_t	& out_Image);
			err_t 		pngDecode		(SPNGData & pngData, img8bgra	& out_Image);
			
			err_t 		pngLoad			(SPNGData & pngData, vcu0_t source);
			err_t 		pngLoad			(SPNGData & pngData, vcu0_t source, img8bgra & out_Texture);
			err_t 		pngLoad			(SPNGData & pngData, vcu0_t source, au0_t & out_Data, g8bgra & out_View);
			err_t 		pngLoad			(SPNGData & pngData, vcu0_t source, au0_t & out_Data);
	stin	err_t		pngLoad			(vcu0_t source, img8bgra & out_Texture)					{ SPNGData tempCache; return pngLoad(tempCache, source	, out_Texture); }
	stin	err_t		pngLoad			(vcu0_t source, au0_t & out_Data, g8bgra & out_View)	{ SPNGData tempCache; return pngLoad(tempCache, source	, out_Data, out_View); }
	stin	err_t		pngLoad			(vcu0_t source, au0_t & out_Data)						{ SPNGData tempCache; return pngLoad(tempCache, source	, out_Data); }
	//
			err_t 		pngLoadFile		(SPNGData & pngData, vcst_t filename);
			err_t 		pngLoadFile		(SPNGData & pngData, vcst_t filename, img8bgra & out_Texture);
			err_t 		pngLoadFile		(SPNGData & pngData, vcst_t filename, au0_t & out_Data, g8bgra & out_View);
			err_t 		pngLoadFile		(SPNGData & pngData, vcst_t filename, au0_t & out_Data);
	stin	err_t		pngLoadFile		(vcst_t filename	, img8bgra & out_Texture)				{ SPNGData tempCache; return pngLoadFile		(tempCache, filename	, out_Texture); }
	stin	err_t		pngLoadFile		(vcst_t filename	, au0_t & out_Data, g8bgra & out_View)	{ SPNGData tempCache; return pngLoadFile		(tempCache, filename	, out_Data, out_View); }
	stin	err_t		pngLoadFile		(vcst_t filename	, au0_t & out_Data)						{ SPNGData tempCache; return pngLoadFile		(tempCache, filename	, out_Data); }

			err_t 		pngFileWrite	(gc8bgra in_ImageView, au0_t & out_Bytes);
} // namespace

#endif // GPK_PNG_H_23627
