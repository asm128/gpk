#include "gpk_system_event.h"
#include "gpk_geometry2.h"
#include "gpk_n2.h"

#ifndef GPK_EVENT_RASTER_H
#define GPK_EVENT_RASTER_H

namespace gpk
{
#ifdef GPK_ATMEL
	enum EVENT_RASTER : uint8_t
		{ EVENT_RASTER_Clear		=  0
		, EVENT_RASTER_Pixel		=  1
		, EVENT_RASTER_Line			=  2
		, EVENT_RASTER_Outline		=  3
		, EVENT_RASTER_Fill			=  4
		, EVENT_RASTER_BitBlt		=  5
		, EVENT_RASTER_Text			=  6
		, EVENT_RASTER_Font			=  7
		, EVENT_RASTER_FontIndex	=  8
		, EVENT_RASTER_FontColor	=  9
		, EVENT_RASTER_Color		= 10
		, EVENT_RASTER_ColorIndex	= 11
		, EVENT_RASTER_Palette		= 12
		, EVENT_RASTER_PaletteIndex	= 13
		, EVENT_RASTER_PaletteColor	= 14
		};
#else
	GDEFINE_ENUM_TYPE (EVENT_RASTER, uint8_t);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, Clear			,  0);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, Pixel			,  1);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, Line			,  2);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, Outline		,  3);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, Fill			,  4);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, BitBlt			,  5);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, Text			,  6);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, Font			,  7);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, FontIndex		,  8);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, FontColor		,  9);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, Color			, 10);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, ColorIndex		, 11);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, Palette		, 12);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, PaletteIndex	, 13);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, PaletteColor	, 14);
#endif
	typedef ::gpk::SEvent            <EVENT_RASTER>	SEventRaster;
	typedef ::gpk::SEventView        <EVENT_RASTER>	SEViewRaster;
	typedef ::gpk::FEventHandler     <EVENT_RASTER>	FRasterEvent;
	typedef ::gpk::FEventHandlerConst<EVENT_RASTER>	FRasterEventConst;

	template<typename _t>
	stainli	::gpk::error_t	eventEnqueueChild	(::gpk::apobj<::gpk::SSystemEvent> & queue, ::gpk::EVENT_RASTER eventType, _t & eventData)	{ 
		return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Raster, eventType, eventData); 
	}

	stainli	::gpk::error_t	eventEnqueueRasterClear			(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::EVENT_RASTER_Clear			, key); }
	stainli	::gpk::error_t	eventEnqueueRasterOutline		(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::EVENT_RASTER_Outline		, key); }
	stainli	::gpk::error_t	eventEnqueueRasterFill			(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::EVENT_RASTER_Fill			, key); }
	stainli	::gpk::error_t	eventEnqueueRasterBitBlt		(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::EVENT_RASTER_BitBlt			, key); }
	stainli	::gpk::error_t	eventEnqueueRasterText			(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::EVENT_RASTER_Text			, key); }
	stainli	::gpk::error_t	eventEnqueueRasterFont			(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::EVENT_RASTER_Font			, key); }
	stainli	::gpk::error_t	eventEnqueueRasterFontIndex		(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::EVENT_RASTER_FontIndex		, key); }
	stainli	::gpk::error_t	eventEnqueueRasterColor			(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::EVENT_RASTER_Color			, key); }
	stainli	::gpk::error_t	eventEnqueueRasterColorIndex	(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::EVENT_RASTER_ColorIndex		, key); }
	stainli	::gpk::error_t	eventEnqueueRasterPalette		(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::EVENT_RASTER_Palette		, key); }
	stainli	::gpk::error_t	eventEnqueueRasterPaletteIndex	(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::EVENT_RASTER_PaletteIndex	, key); }
	stainli	::gpk::error_t	eventEnqueueRasterPaletteColor	(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::EVENT_RASTER_PaletteColor	, key); }
} // namespace 

#endif // GPK_EVENT_RASTER_H
