#include "llc_system_event.h"
#include "llc_geometry2.h"
#include "llc_n2.h"

#ifndef LLC_EVENT_RASTER_H
#define LLC_EVENT_RASTER_H

namespace llc
{
	GDEFINE_ENUM_TYPE (EVENT_RASTER, u0_t);
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
	GDEFINE_ENUM_VALUE(EVENT_RASTER, FontBackground	, 10);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, Color			, 11);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, ColorIndex		, 12);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, Palette		, 13);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, PaletteIndex	, 14);
	GDEFINE_ENUM_VALUE(EVENT_RASTER, PaletteColor	, 15);

	tydf SEvent            <EVENT_RASTER>	SEventRaster;
	tydf SEventView        <EVENT_RASTER>	SEViewRaster;
	tydf FEventHandler     <EVENT_RASTER>	FRasterEvent;
	tydf FEventHandlerConst<EVENT_RASTER>	FRasterEventConst;

	tpl_tstin	err_t	eventEnqueueChild	(apobj<SEventSystem> & queue, EVENT_RASTER eventType, _t & eventData)	{ 
		rtrn eventEnqueueChild(queue, SYSTEM_EVENT_Raster, eventType, eventData); 
	}

	stin	err_t	eventEnqueueRasterClear			(apobj<SEventSystem> & queue, u0_t key)	{ return eventEnqueueChild(queue, EVENT_RASTER_Clear			, key); }
	stin	err_t	eventEnqueueRasterOutline		(apobj<SEventSystem> & queue, u0_t key)	{ return eventEnqueueChild(queue, EVENT_RASTER_Outline		, key); }
	stin	err_t	eventEnqueueRasterFill			(apobj<SEventSystem> & queue, u0_t key)	{ return eventEnqueueChild(queue, EVENT_RASTER_Fill			, key); }
	stin	err_t	eventEnqueueRasterBitBlt		(apobj<SEventSystem> & queue, u0_t key)	{ return eventEnqueueChild(queue, EVENT_RASTER_BitBlt			, key); }
	stin	err_t	eventEnqueueRasterText			(apobj<SEventSystem> & queue, u0_t key)	{ return eventEnqueueChild(queue, EVENT_RASTER_Text			, key); }
	stin	err_t	eventEnqueueRasterFont			(apobj<SEventSystem> & queue, u0_t key)	{ return eventEnqueueChild(queue, EVENT_RASTER_Font			, key); }
	stin	err_t	eventEnqueueRasterFontIndex		(apobj<SEventSystem> & queue, u0_t key)	{ return eventEnqueueChild(queue, EVENT_RASTER_FontIndex		, key); }
	stin	err_t	eventEnqueueRasterColor			(apobj<SEventSystem> & queue, u0_t key)	{ return eventEnqueueChild(queue, EVENT_RASTER_Color			, key); }
	stin	err_t	eventEnqueueRasterColorIndex	(apobj<SEventSystem> & queue, u0_t key)	{ return eventEnqueueChild(queue, EVENT_RASTER_ColorIndex		, key); }
	stin	err_t	eventEnqueueRasterPalette		(apobj<SEventSystem> & queue, u0_t key)	{ return eventEnqueueChild(queue, EVENT_RASTER_Palette		, key); }
	stin	err_t	eventEnqueueRasterPaletteIndex	(apobj<SEventSystem> & queue, u0_t key)	{ return eventEnqueueChild(queue, EVENT_RASTER_PaletteIndex	, key); }
	stin	err_t	eventEnqueueRasterPaletteColor	(apobj<SEventSystem> & queue, u0_t key)	{ return eventEnqueueChild(queue, EVENT_RASTER_PaletteColor	, key); }
} // namespace 

#endif // LLC_EVENT_RASTER_H
