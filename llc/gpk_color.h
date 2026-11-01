/// Copyright 2009-2024 - ogarnd
#include "gpk_math.h"

#ifndef GPK_COLOR_H_23627
#define GPK_COLOR_H_23627

namespace gpk
{
#pragma pack(push, 1)
#define COLOR_RGBA_OPERATOR_MEMBERS(color_struct_type) \
	cxpr	bool		oper==			(cnst color_struct_type & other)	csnx	{ return b == other.b && g == other.g && r == other.r && a == other.a; } \
	cxpr	bool		oper!=			(cnst color_struct_type & other)	csnx	{ return b != other.b || g != other.g || r != other.r || a != other.a; } \

#define COLOR_RGB_OPERATOR_MEMBERS(color_struct_type) \
	cxpr	bool		oper==			(cnst color_struct_type & other)	csnx	{ return b == other.b && g == other.g && r == other.r; } \
	cxpr	bool		oper!=			(cnst color_struct_type & other)	csnx	{ return b != other.b || g != other.g || r != other.r; } \

	tpl_tstct color_a							{ tdfTTCnst(_t); T a; GPK_DEFAULT_OPERATOR(color_a	, a == other.a); };
	tpl_tstct color_b							{ tdfTTCnst(_t); T b; GPK_DEFAULT_OPERATOR(color_b	, b == other.b); };
	tpl_tstct color_r							{ tdfTTCnst(_t); T r; GPK_DEFAULT_OPERATOR(color_r	, r == other.r); };

	tpl_tstct color_ab		: color_a	<_t>	{ tdfTTCnst(_t); T b; GPK_DEFAULT_OPERATOR(color_ab	, color_a	<T>::oper==(other) && b == other.b); };
	tpl_tstct color_abg		: color_ab	<_t>	{ tdfTTCnst(_t); T g; GPK_DEFAULT_OPERATOR(color_abg	, color_ab	<T>::oper==(other) && g == other.g); };
	tpl_tstct color_abgr	: color_abg	<_t>	{ tdfTTCnst(_t); T r; GPK_DEFAULT_OPERATOR(color_abgr	, color_abg	<T>::oper==(other) && r == other.r); };

	tpl_tstct color_ar		: color_a	<_t>	{ tdfTTCnst(_t); T r; GPK_DEFAULT_OPERATOR(color_ar	, color_a	<T>::oper==(other) && r == other.r); };
	tpl_tstct color_arg		: color_ar	<_t>	{ tdfTTCnst(_t); T g; GPK_DEFAULT_OPERATOR(color_arg	, color_ar	<T>::oper==(other) && g == other.g); };
	tpl_tstct color_argb	: color_arg	<_t>	{ tdfTTCnst(_t); T b; GPK_DEFAULT_OPERATOR(color_argb	, color_arg	<T>::oper==(other) && b == other.b); };

	tpl_tstct color_bg		: color_b	<_t>	{ tdfTTCnst(_t); T g; GPK_DEFAULT_OPERATOR(color_bg	, color_b	<T>::oper==(other) && g == other.g); };
	tpl_tstct color_bgr		: color_bg	<_t>	{ tdfTTCnst(_t); T r; GPK_DEFAULT_OPERATOR(color_bgr	, color_bg	<T>::oper==(other) && r == other.r); };
	tpl_tstct color_bgra	: color_bgr	<_t>	{ tdfTTCnst(_t); T a; GPK_DEFAULT_OPERATOR(color_bgra	, color_bgr	<T>::oper==(other) && a == other.a); };

	tpl_tstct color_rg		: color_r	<_t>	{ tdfTTCnst(_t); T g; GPK_DEFAULT_OPERATOR(color_rg	, color_r	<T>::oper==(other) && g == other.g); };
	tpl_tstct color_rgb		: color_rg	<_t>	{ tdfTTCnst(_t); T b; GPK_DEFAULT_OPERATOR(color_rgb	, color_rg	<T>::oper==(other) && b == other.b); };
	tpl_tstct color_rgba	: color_rgb	<_t>	{ tdfTTCnst(_t); T a; GPK_DEFAULT_OPERATOR(color_rgba	, color_rgb	<T>::oper==(other) && a == other.a); };

	tydf	u1_t		SColor16;

	tydf color_bgr <u0_t>	bgr8 ;
	tydf color_rgb <u0_t>	rgb8 ;
	tydf color_bgra<u0_t>	bgra8;
	tydf color_rgba<u0_t>	rgba8;
	tydf color_abgr<u0_t>	abgr8;
	tydf color_argb<u0_t>	argb8;

	tydf color_bgr <u1_t>	bgr16 ;
	tydf color_rgb <u1_t>	rgb16 ;
	tydf color_bgra<u1_t>	bgra16;
	tydf color_rgba<u1_t>	rgba16;
	tydf color_abgr<u1_t>	abgr16;
	tydf color_argb<u1_t>	argb16;

	tydf color_bgr <f2_t>	bgrf2_t;
	tydf color_rgb <f2_t>	rgbf2_t;
	tydf color_bgra<f2_t>	bgraf2_t;
	tydf color_rgba<f2_t>	rgbaf2_t;
	tydf color_abgr<f2_t>	abgrf2_t;
	tydf color_argb<f2_t>	argbf2_t;

	tdcs color_bgr <u0_t>	cbgr8 ;
	tdcs color_rgb <u0_t>	crgb8 ;
	tdcs color_bgra<u0_t>	cbgra8;
	tdcs color_rgba<u0_t>	crgba8;
	tdcs color_abgr<u0_t>	cabgr8;
	tdcs color_argb<u0_t>	cargb8;

	tdcs color_bgr <u1_t>	cbgr16 ;
	tdcs color_rgb <u1_t>	crgb16 ;
	tdcs color_bgra<u1_t>	cbgra16;
	tdcs color_rgba<u1_t>	crgba16;
	tdcs color_abgr<u1_t>	cabgr16;
	tdcs color_argb<u1_t>	cargb16;

	tdcs color_bgr <f2_t>	cbgrf2_t;
	tdcs color_rgb <f2_t>	crgbf2_t;
	tdcs color_bgra<f2_t>	cbgraf2_t;
	tdcs color_rgba<f2_t>	crgbaf2_t;
	tdcs color_abgr<f2_t>	cabgrf2_t;
	tdcs color_argb<f2_t>	cargbf2_t;

	cxpr	f2_c			BYTE_SCALE			= 1.0f / 255.0f;

	// Stores RGBA color channels
	stct SColorRGBA : public ::gpk::rgba8 {
								SColorRGBA		()											nxpt	= default;
		cxpr					SColorRGBA		(cnst SColorRGBA &)							nxpt	= default;
		cxpr					SColorRGBA		(u0_t r_, u0_t g_, u0_t b_, u0_t a_=0xff)	nxpt	: color_rgba<u0_t>{{{{r_}, g_}, b_}, a_}						{}
		cxpr					SColorRGBA		(u2_t other)								nxpt	: color_rgba<u0_t>
			{ {{{u0_t(((other & 0x000000FF) >>  0))}
			, u0_t(((other & 0x0000FF00) >>  8))	 }
			, u0_t(((other & 0x00FF0000) >> 16))	 }
			, u0_t(((other & 0xFF000000) >> 24))
			}
		{}

		COLOR_RGBA_OPERATOR_MEMBERS(SColorRGBA); 
		cxpr	oper		u2_t		()								csnx	{ return (((u2_t)a) << 24) | (((u2_t)b) << 16) | (((u2_t)g) << 8) | (((u2_t)r) << 0);					}


		cxpr	bool			oper ==		(u2_t other)				csnx	{ return other == *((cnst u2_t*)this);			}
		cxpr	SColorRGBA		oper *		(cnst SColorRGBA & color)	csnx	{ return {(u0_t)::gpk::clamped(r * (int)color.r, 0, 255)	, (u0_t)::gpk::clamped(g * (int)color.g, 0, 255), (u0_t)::gpk::clamped(b * (int)color.b, 0, 255), a};	}
		cxpr	SColorRGBA		oper +		(cnst SColorRGBA & color)	csnx	{ return {(u0_t)::gpk::clamped(r + (int)color.r, 0, 255)	, (u0_t)::gpk::clamped(g + (int)color.g, 0, 255), (u0_t)::gpk::clamped(b + (int)color.b, 0, 255), a};	}
		cxpr	SColorRGBA		oper *		(f3_t scalar)				csnx	{ return {(u0_t)::gpk::clamped(r * scalar, 0.0 , 255.0)		, (u0_t)::gpk::clamped(g * scalar, 0.0,   255.0), (u0_t)::gpk::clamped(b * scalar, 0.0,   255.0), a};	}
		cxpr	SColorRGBA		oper /		(f3_t scalar)				cnst				{ return {(u0_t)::gpk::clamped(r / scalar, 0.0 , 255.0)		, (u0_t)::gpk::clamped(g / scalar, 0.0,   255.0), (u0_t)::gpk::clamped(b / scalar, 0.0,   255.0), a};	}
	};	// struct

	// Stores BGRA color channels
	stct SColorBGRA : public ::gpk::bgra8 {
								SColorBGRA		()											nxpt	= default;
		cxpr					SColorBGRA		(cnst SColorBGRA &)							nxpt	= default;
		cxpr					SColorBGRA		(cnst SColorRGBA & other)					nxpt	: color_bgra<u0_t>{{{{other.b}, other.g}, other.r}, other.a} {}
		cxpr					SColorBGRA		(u0_t b_, u0_t g_, u0_t r_, u0_t a_=0xff)	nxpt	: color_bgra<u0_t>{{{{b_}, g_}, r_}, a_} {}
		cxpr					SColorBGRA		(u2_t other)								nxpt	: color_bgra<u0_t>
			{ {{{u0_t(((other & 0x000000FF) >>  0)) }
			, u0_t(((other & 0x0000FF00) >>  8))	  }
			, u0_t(((other & 0x00FF0000) >> 16))	  }
			, u0_t(((other & 0xFF000000) >> 24))
			}
		{}

		COLOR_RGBA_OPERATOR_MEMBERS(SColorBGRA); 
		cxpr	oper		SColorRGBA		()								csnx	{ return {r, g, b, a};						}
		cxpr	oper		u2_t			()								csnx	{ return (((u2_t)a) << 24) | (((u2_t)r) << 16) | (((u2_t)g) << 8) | (((u2_t)b) << 0);					}

		cxpr	bool			oper==		(u2_t other)					csnx	{ return other == *((cnst u2_t*)this);			}
		cxpr	SColorBGRA		oper*		(cnst SColorBGRA & color)		csnx	{ return {(u0_t)::gpk::min(b * (int)color.b, 255)	, (u0_t)::gpk::min(g * (int)color.g, 255)	,(u0_t)::gpk::min(r * (int)color.r, 255), a};			}
		cxpr	SColorBGRA		oper+		(cnst SColorBGRA & color)		csnx	{ return {(u0_t)::gpk::min(b + (int)color.b, 255)	, (u0_t)::gpk::min(g + (int)color.g, 255)	,(u0_t)::gpk::min(r + (int)color.r, 255), a};			}
		cxpr	SColorBGRA		oper-		(cnst SColorBGRA & color)		csnx	{ return {(u0_t)::gpk::clamped(b - (int16_t)color.b, 0, 255), (u0_t)::gpk::clamped(g - (int16_t)color.g, 0, 255),(u0_t)::gpk::clamped(r - (int16_t)color.r, 0, 255), a};	}
		cxpr	SColorBGRA		oper*		(f2_t scalar)					csnx	{ return {(u0_t)::gpk::clamped(b * scalar, 0.0f, 255.0f)	, (u0_t)::gpk::clamped(g * scalar, 0.0f, 255.0f)	,(u0_t)::gpk::clamped(r * scalar, 0.0f, 255.0f), a};			}
		cxpr	SColorBGRA		oper/		(f2_t scalar)					cnst		{ return {(u0_t)::gpk::clamped(b / scalar, 0.0f, 255.0f)	, (u0_t)::gpk::clamped(g / scalar, 0.0f, 255.0f)	,(u0_t)::gpk::clamped(r / scalar, 0.0f, 255.0f), a};			}
		cxpr	SColorBGRA		oper*		(f3_t scalar)					csnx	{ return {(u0_t)::gpk::clamped(b * scalar, 0.0,  255.0 )	, (u0_t)::gpk::clamped(g * scalar, 0.0 , 255.0 )	,(u0_t)::gpk::clamped(r * scalar, 0.0, 255.0),	a};				}
		cxpr	SColorBGRA		oper/		(f3_t scalar)					cnst		{ return {(u0_t)::gpk::clamped(b / scalar, 0.0,  255.0 )	, (u0_t)::gpk::clamped(g / scalar, 0.0 , 255.0 )	,(u0_t)::gpk::clamped(r / scalar, 0.0, 255.0),	a};				}
				SColorBGRA&		oper*=		(f2_t scalar)						nxpt	{ b = (u0_t)::gpk::clamped(b * scalar, 0.0f, 255.0f); g = (u0_t)::gpk::clamped(g * scalar, 0.0f, 255.0f); r = (u0_t)::gpk::clamped(r * scalar, 0.0f, 255.0f);	return *this; }
				SColorBGRA&		oper/=		(f2_t scalar)								{ b = (u0_t)::gpk::clamped(b / scalar, 0.0f, 255.0f); g = (u0_t)::gpk::clamped(g / scalar, 0.0f, 255.0f); r = (u0_t)::gpk::clamped(r / scalar, 0.0f, 255.0f);	return *this; }
				SColorBGRA&		oper*=		(f3_t scalar)						nxpt	{ b = (u0_t)::gpk::clamped(b * scalar, 0.0,  255.0 ); g = (u0_t)::gpk::clamped(g * scalar, 0.0 , 255.0 ); r = (u0_t)::gpk::clamped(r * scalar, 0.0, 255.0);		return *this; }
				SColorBGRA&		oper/=		(f3_t scalar)								{ b = (u0_t)::gpk::clamped(b / scalar, 0.0,  255.0 ); g = (u0_t)::gpk::clamped(g / scalar, 0.0 , 255.0 ); r = (u0_t)::gpk::clamped(r / scalar, 0.0, 255.0);		return *this; }

				SColorBGRA&		FromBGR16		(cnst SColor16 & other)				nxpt	{ b = u0_t((other & 0x1F) / f2_t(0x1F) * 255); g = u0_t(((other & 0x07E0) >> 5) / f2_t(0x3F) * 255); r = u0_t(((other & 0xF800) >> 11) / f2_t(0x1F) * 255);	return *this; }
				SColorBGRA&		FromRGB16		(cnst SColor16 & other)				nxpt	{ r = u0_t((other & 0x1F) / f2_t(0x1F) * 255); g = u0_t(((other & 0x07E0) >> 5) / f2_t(0x3F) * 255); b = u0_t(((other & 0xF800) >> 11) / f2_t(0x1F) * 255);	return *this; }
	};	// struct

	// Stores RGB color channels
	stct SColorRGB : public ::gpk::rgb8 {
								SColorRGB		()									nxpt	= default;
		cxpr					SColorRGB		(cnst SColorRGB & other)			nxpt	= default;
		cxpr					SColorRGB		(u0_t r_, u0_t g_, u0_t b_)			nxpt	: color_rgb<u0_t>{{{r_}, g_}, b_} {}
		cxpr					SColorRGB		(u2_t other)						nxpt	: color_rgb<u0_t>
			{ {{u0_t(((other & 0x000000FF) >>  0))}
			, u0_t(((other & 0x0000FF00) >>  8))	}
			, u0_t(((other & 0x00FF0000) >> 16))
			}
		{}

		COLOR_RGB_OPERATOR_MEMBERS(SColorRGB); 
		cxpr	oper		u2_t			()							csnx	{ return  0xFF000000 | (((u2_t)b) << 16) | (((u2_t)g) << 8)	| (((u2_t)r) << 0);  }
		cxpr	oper		SColorRGBA		()							csnx	{ return {0xFF000000 | (((u2_t)b) << 16) | (((u2_t)g) << 8)	| (((u2_t)r) << 0)}; }
		cxpr	oper		SColor16		()							csnx	{ return (((u1_t)(r * BYTE_SCALE * 0x001F)) << 0) | (((u1_t)(g * BYTE_SCALE * 0x003F)) << 5) | (((u1_t)(b * BYTE_SCALE * 0x001F)) << 11 );												}
		cxpr	bool			oper ==		(cnst SColorRGBA & other)	csnx	{ return r == other.r && g == other.g && b == other.b;	}
		cxpr	bool			oper !=		(cnst SColorRGBA & other)	csnx	{ return r != other.r || g != other.g || b != other.b;	}
		cxpr	bool			oper ==		(cnst SColor16   & other)	csnx	{ return (SColor16)(*this) == other;					}
		cxpr	bool			oper !=		(cnst SColor16   & other)	csnx	{ return (SColor16)(*this) != other;					}
		cxpr	SColorRGB		oper *		(f2_t scalar)				csnx	{ return {(u0_t)::gpk::clamped(r * scalar, 0.0f, 255.0f), (u0_t)::gpk::clamped(g * scalar, 0.0f, 255.0f),	(u0_t)::gpk::clamped(b * scalar, 0.0f, 255.0f)}; }
		cxpr	SColorRGB		oper /		(f2_t scalar)				cnst				{ return {(u0_t)::gpk::clamped(r / scalar, 0.0f, 255.0f), (u0_t)::gpk::clamped(g / scalar, 0.0f, 255.0f),	(u0_t)::gpk::clamped(b / scalar, 0.0f, 255.0f)}; }
		cxpr	SColorRGB		oper *		(f3_t scalar)				csnx	{ return {(u0_t)::gpk::clamped(r * scalar, 0.0,  255.0),  (u0_t)::gpk::clamped(g * scalar, 0.0,  255.0),	(u0_t)::gpk::clamped(b * scalar, 0.0,  255.0)};  }
		cxpr	SColorRGB		oper /		(f3_t scalar)				cnst				{ return {(u0_t)::gpk::clamped(r / scalar, 0.0,  255.0),  (u0_t)::gpk::clamped(g / scalar, 0.0,  255.0),	(u0_t)::gpk::clamped(b / scalar, 0.0,  255.0)};  }
		cxpr	SColorRGB		oper *		(cnst SColorRGB  & color)	csnx	{ return {(u0_t)::gpk::clamped(r * (int)color.r, 0, 255), (u0_t)::gpk::clamped(g * (int)color.g, 0, 255), (u0_t)::gpk::clamped(b * (int)color.b, 0, 255)};			}
		cxpr	SColorRGB		oper +		(cnst SColorRGB  & color)	csnx	{ return {(u0_t)::gpk::clamped(r + (int)color.r, 0, 255), (u0_t)::gpk::clamped(g + (int)color.g, 0, 255), (u0_t)::gpk::clamped(b + (int)color.b, 0, 255)};			}
		cxpr	SColorRGBA		oper *		(cnst SColorRGBA & color)	csnx	{ return {(u0_t)::gpk::clamped(r * (int)color.r, 0, 255), (u0_t)::gpk::clamped(g * (int)color.g, 0, 255), (u0_t)::gpk::clamped(b * (int)color.b, 0, 255), color.a};	}
		cxpr	SColorRGBA		oper +		(cnst SColorRGBA & color)	csnx	{ return {(u0_t)::gpk::clamped(r + (int)color.r, 0, 255), (u0_t)::gpk::clamped(g + (int)color.g, 0, 255), (u0_t)::gpk::clamped(b + (int)color.b, 0, 255), color.a};	}
	};	// struct

	// Stores BGR color channels
	stct SColorBGR : public ::gpk::color_bgr<u0_t> {
								SColorBGR		()								nxpt	= default;
		cxpr					SColorBGR		(cnst SColorBGR & other)		nxpt	= default;
		cxpr					SColorBGR		(u0_t b_, u0_t g_, u0_t r_)		nxpt	: color_bgr<u0_t>{{{b_}, g_}, r_} {}
		cxpr					SColorBGR		(u2_t other)					nxpt	: color_bgr<u0_t>
			{ {{u0_t(((other & 0x000000FF) >>  0))}
			, u0_t(((other & 0x0000FF00) >>  8))	}
			, u0_t(((other & 0x00FF0000) >> 16))
			}
		{}
		COLOR_RGB_OPERATOR_MEMBERS(SColorBGR); 
		cxpr	oper		u2_t			()							csnx	{ return 0xFF000000 | (((u2_t)r) << 16)	| (((u2_t)g) << 8) | (((u2_t)b) << 0);																	}
		cxpr	oper		SColorBGRA		()							csnx	{ return {b, g, r, 0xFF};														}
		cxpr	oper		SColor16		()							csnx	{ return (((u1_t)(b * BYTE_SCALE * 0x001F)) << 0) | (((u1_t)(g * BYTE_SCALE * 0x003F)) << 5) | (((u1_t)(r * BYTE_SCALE * 0x001F)) << 11 );	}

		cxpr	bool			oper ==		(cnst SColorBGRA & other)	csnx	{ return r == other.r && g == other.g && b == other.b;	}
		cxpr	bool			oper !=		(cnst SColorBGRA & other)	csnx	{ return r != other.r || g != other.g || b != other.b;	}
		cxpr	bool			oper ==		(cnst SColor16   & other)	csnx	{ return (SColor16)(*this) == other;					}
		cxpr	bool			oper !=		(cnst SColor16   & other)	csnx	{ return (SColor16)(*this) != other;					}
		cxpr	SColorBGR		oper *		(f2_t scalar)				csnx	{ return {(u0_t)::gpk::clamped(b * scalar, 0.0f, 255.0f), (u0_t)::gpk::clamped(g * scalar, 0.0f, 255.0f), (u0_t)::gpk::clamped(r * scalar, 0.0f, 255.0f)};	}
		cxpr	SColorBGR		oper /		(f2_t scalar)				cnst				{ return {(u0_t)::gpk::clamped(b / scalar, 0.0f, 255.0f), (u0_t)::gpk::clamped(g / scalar, 0.0f, 255.0f), (u0_t)::gpk::clamped(r / scalar, 0.0f, 255.0f)};	}
		cxpr	SColorBGR		oper *		(f3_t scalar)				csnx	{ return {(u0_t)::gpk::clamped(b * scalar, 0.0,  255.0) , (u0_t)::gpk::clamped(g * scalar, 0.0,  255.0) , (u0_t)::gpk::clamped(r * scalar, 0.0,  255.0)};	}
		cxpr	SColorBGR		oper /		(f3_t scalar)				cnst				{ return {(u0_t)::gpk::clamped(b / scalar, 0.0,  255.0) , (u0_t)::gpk::clamped(g / scalar, 0.0,  255.0) , (u0_t)::gpk::clamped(r / scalar, 0.0,  255.0)};	}
		cxpr	SColorBGR		oper *		(cnst SColorBGR  & color)	csnx	{ return {(u0_t)::gpk::clamped(b * (int)color.b, 0, 255) , (u0_t)::gpk::clamped(g * (int)color.g, 0, 255)	, (u0_t)::gpk::clamped(r * (int)color.r, 0, 255)};			}
		cxpr	SColorBGR		oper +		(cnst SColorBGR  & color)	csnx	{ return {(u0_t)::gpk::clamped(b + (int)color.b, 0, 255) , (u0_t)::gpk::clamped(g + (int)color.g, 0, 255)	, (u0_t)::gpk::clamped(r + (int)color.r, 0, 255)};			}
		cxpr	SColorBGRA		oper *		(cnst SColorBGRA & color)	csnx	{ return {(u0_t)::gpk::clamped(b * (int)color.b, 0, 255) , (u0_t)::gpk::clamped(g * (int)color.g, 0, 255)	, (u0_t)::gpk::clamped(r * (int)color.r, 0, 255), color.a};	}
		cxpr	SColorBGRA		oper +		(cnst SColorBGRA & color)	csnx	{ return {(u0_t)::gpk::clamped(b + (int)color.b, 0, 255) , (u0_t)::gpk::clamped(g + (int)color.g, 0, 255)	, (u0_t)::gpk::clamped(r + (int)color.r, 0, 255), color.a};	}

				SColorBGR&		FromBGR16		(cnst SColor16   & other)			nxpt	{ b = u0_t((other & 0x1F) / f2_t(0x1F) * 255); g = u0_t(((other & 0x07E0) >> 5) / f2_t(0x3F) * 255); r = u0_t(((other & 0xF800) >> 11) / f2_t(0x1F) * 255);	return *this; }
				SColorBGR&		FromRGB16		(cnst SColor16   & other)			nxpt	{ r = u0_t((other & 0x1F) / f2_t(0x1F) * 255); g = u0_t(((other & 0x07E0) >> 5) / f2_t(0x3F) * 255); b = u0_t(((other & 0xF800) >> 11) / f2_t(0x1F) * 255);	return *this; }
	};	// struct

	// Stores RGBA f2_ting point color channels
	stct SColorFloat : public ::gpk::rgbaf2_t {
								SColorFloat		()										nxpt	= default;
		cxpr					SColorFloat		(cnst SColorFloat & color)				nxpt	= default;
		cxpr					SColorFloat		(f2_t r_, f2_t g_, f2_t b_, f2_t a_=1.0f)	nxpt	: color_rgba<f2_t>{{{{r_}, g_}, b_}, a_}						{}
		cxpr					SColorFloat		(cnst f2_t* rgbaColor)							: color_rgba<f2_t>{{{{rgbaColor[0]}, rgbaColor[1]}, rgbaColor[2]}, rgbaColor[3]}	{}
		cxpr					SColorFloat		(SColorBGRA color)						nxpt	: color_rgba<f2_t>
			{ {{{color.r * BYTE_SCALE}
			, color.g * BYTE_SCALE	 }
			, color.b * BYTE_SCALE	 }
			, color.a * BYTE_SCALE
			} {}
		cxpr					SColorFloat		(::gpk::bgr8 color, u0_t alpha)		nxpt	: color_rgba<f2_t>
			{ {{{color.r * BYTE_SCALE}
			, color.g * BYTE_SCALE	 }
			, color.b * BYTE_SCALE	 }
			, alpha * BYTE_SCALE
			} {}
		cxpr					SColorFloat		(::gpk::rgbf2_t color, f2_t alpha)		nxpt	: color_rgba<f2_t>
			{ {{{color.r }
			, color.g }
			, color.b }
			, alpha 
			} {}
		COLOR_RGBA_OPERATOR_MEMBERS(SColorFloat); 
		cxpr	oper		SColorBGRA		()							csnx	{ return {u0_t(b * 255), u0_t(g * 255), u0_t(r * 255), u0_t(a * 255)};	}
		cxpr	oper		cnst f2_t*	()							csnx	{ return &r;															}
				SColorFloat&	oper +=		(cnst SColorFloat & color)			nxpt	{ r = r + color.r;					g = g + color.g;				b = b + color.b;				return *this;	}
				SColorFloat&	oper *=		(cnst SColorFloat & color)			nxpt	{ r = r * color.r;					g = g * color.g;				b = b * color.b;				return *this;	}
				SColorFloat&	oper *=		(f3_t scalar)						nxpt	{ r = (f2_t)(r * scalar);			g = (f2_t)(g * scalar);		b = (f2_t)(b * scalar);		return *this;	}
				SColorFloat&	oper /=		(f3_t scalar)									{ r = (f2_t)(r / scalar);			g = (f2_t)(g / scalar);		b = (f2_t)(b / scalar);		return *this;	}
				SColorFloat&	oper *=		(f2_t scalar)						nxpt	{ r = r * scalar;					g = g * scalar;					b = b * scalar;					return *this;	}
				SColorFloat&	oper /=		(f2_t scalar)									{ r = r / scalar;					g = g / scalar;					b = b / scalar;					return *this;	}
				SColorFloat&	oper *=		(cnst SColorBGRA  & color)			nxpt	{ r = r * (color.r * BYTE_SCALE);	g = g * (color.g * BYTE_SCALE);	b = b * (color.b * BYTE_SCALE);	return *this;	}
				SColorFloat&	oper +=		(cnst SColorBGRA  & color)			nxpt	{ r = r + (color.r * BYTE_SCALE);	g = g + (color.g * BYTE_SCALE);	b = b + (color.b * BYTE_SCALE);	return *this;	}
				SColorFloat&	oper *=		(cnst SColorBGR   & color)			nxpt	{ r = r * (color.r * BYTE_SCALE);	g = g * (color.g * BYTE_SCALE);	b = b * (color.b * BYTE_SCALE);	return *this;	}
				SColorFloat&	oper +=		(cnst SColorBGR   & color)			nxpt	{ r = r + (color.r * BYTE_SCALE);	g = g + (color.g * BYTE_SCALE);	b = b + (color.b * BYTE_SCALE);	return *this;	}
				SColorFloat		oper *		(cnst SColorBGRA  & color)	csnx	{ return {r * (color.r * BYTE_SCALE), g * (color.g * BYTE_SCALE), b * (color.b * BYTE_SCALE), a};	}
				SColorFloat		oper +		(cnst SColorBGRA  & color)	csnx	{ return {r + (color.r * BYTE_SCALE), g + (color.g * BYTE_SCALE), b + (color.b * BYTE_SCALE), a};	}
				SColorFloat		oper *		(cnst SColorBGR   & color)	csnx	{ return {r * (color.r * BYTE_SCALE), g * (color.g * BYTE_SCALE), b * (color.b * BYTE_SCALE), a};	}
				SColorFloat		oper +		(cnst SColorBGR   & color)	csnx	{ return {r + (color.r * BYTE_SCALE), g + (color.g * BYTE_SCALE), b + (color.b * BYTE_SCALE), a};	}
		cxpr	SColorFloat		oper *		(cnst SColorFloat & color)	csnx	{ return {r * color.r, g * color.g, b * color.b, a};												}
		cxpr	SColorFloat		oper +		(cnst SColorFloat & color)	csnx	{ return {r + color.r, g + color.g, b + color.b, a};												}
		cxpr	SColorFloat		oper -		(cnst SColorFloat & color)	csnx	{ return {r - color.r, g - color.g, b - color.b, a};												}
		cxpr	SColorFloat		oper *		(f3_t scalar)				csnx	{ return {(f2_t)(r * scalar), (f2_t)(g * scalar), (f2_t)(b * scalar), a};						}
		cxpr	SColorFloat		oper *		(f2_t scalar)				csnx	{ return {(r * scalar), (g * scalar),	(b * scalar)};												}
		cxpr	SColorFloat		oper /		(f3_t scalar)				cnst				{ return {(f2_t)(r / scalar), (f2_t)(g / scalar), (f2_t)(b / scalar), a};						}
		cxpr	SColorFloat		oper /		(f2_t scalar)				cnst				{ return {r / scalar,	g / scalar,	b / scalar};													}

				SColorFloat&	Clamp			()									nxpt	{ r = ::gpk::clamped(r, 0.0f, 1.0f); g = ::gpk::clamped(g, 0.0f, 1.0f); b = ::gpk::clamped(b, 0.0f, 1.0f); return *this;	}

		cxpr	::gpk::rgbf2_t	rgb				()							cnst				{ return {{{r}, g}, b}; }
	};	// struct

	tydf ::gpk::SColorFloat					frgba	;
	tydf ::gpk::SColorBGRA					u8bgra	;
	tydf ::gpk::SColorBGR					u8bgr	;
	tydf ::gpk::SColorRGBA					u8rgba	;
	tydf ::gpk::SColorRGB					u8rgb	;

	tdcs frgba							cfrgba	;
	tdcs u8bgra						c8bgra	;
	tdcs u8bgr							c8bgr	;
	tdcs u8rgba						c8rgba	;
	tdcs u8rgb							c8rgb	;

	tydf frgba								rgbaf	;
	tydf u8bgra								bgra	;
	tydf u8bgr								bgr		;
	tydf u8rgba								rgba	;
	tydf u8rgb								rgb		;


	stxp	::gpk::rgbaf	BLACK			= {0.0f , 0.0f               , 0.00f, 1.0f};
	stxp	::gpk::rgbaf	WHITE			= {1.0f , 1.0f               , 1.0f , 1.0f};
	stxp	::gpk::rgbaf	RED				= {1.0f , 0.0f               , 0.0f , 1.0f};
	stxp	::gpk::rgbaf	GREEN			= {0.0f , 1.0f               , 0.0f , 1.0f};
	stxp	::gpk::rgbaf	BLUE			= {0.0f , 0.0f               , 1.0f , 1.0f};
	stxp	::gpk::rgbaf	YELLOW			= {1.0f , 1.0f               , 0.0f , 1.0f};
	stxp	::gpk::rgbaf	MAGENTA			= {1.0f , 0.0f               , 1.0f , 1.0f};
	stxp	::gpk::rgbaf	CYAN			= {0.0f , 1.0f               , 1.0f , 1.0f};
	stxp	::gpk::rgbaf	ORANGE			= {1.0f , 0.647f             , 0.0f , 1.0f};
	stxp	::gpk::rgbaf	PANOCHE			= {0.25f, 0.5f               , 1.0f , 1.0f};
	stxp	::gpk::rgbaf	PURPLE			= {0.5f , 0.1f               , 1.0f , 1.0f};
	stxp	::gpk::rgbaf	TURQUOISE		= {0.2f , 1.0                , 0.65f, 1.0f};
	stxp	::gpk::rgbaf	BROWN			= {0.4f , 0.223f             , 0.0f , 1.0f};
	stxp	::gpk::rgbaf	GRAY			= {0.5f , 0.5f               , 0.5f , 1.0f};
	stxp	::gpk::rgbaf	LIGHTGRAY		= {0.75f, 0.75f              , 0.75f, 1.0f};
	stxp	::gpk::rgbaf	LIGHTRED		= {1.0f , 0.25f              , 0.25f, 1.0f};
	stxp	::gpk::rgbaf	LIGHTGREEN		= {0.5f , 1.0f               , 0.5f , 1.0f};
	stxp	::gpk::rgbaf	LIGHTBLUE		= {0.25f, 0.25f              , 1.0f , 1.0f};
	stxp	::gpk::rgbaf	LIGHTYELLOW		= {1.0f , 1.0f               , 0.25f, 1.0f};
	stxp	::gpk::rgbaf	LIGHTMAGENTA	= {1.0f , 0.25f              , 1.0f , 1.0f};
	stxp	::gpk::rgbaf	LIGHTCYAN		= {0.25f, 1.0f               , 1.0f , 1.0f};
	stxp	::gpk::rgbaf	LIGHTORANGE		= {1.0f , 0.780f             , 0.25f, 1.0f};
	stxp	::gpk::rgbaf	DARKGRAY		= {0.25f, 0.25f              , 0.25f, 1.0f};
	stxp	::gpk::rgbaf	DARKRED			= {0.5f , 0.0f               , 0.0f , 1.0f};
	stxp	::gpk::rgbaf	DARKGREEN		= {0.0f , 0.5f               , 0.0f , 1.0f};
	stxp	::gpk::rgbaf	DARKBLUE		= {0.0f , 0.0f               , 0.5f , 1.0f};
	stxp	::gpk::rgbaf	DARKYELLOW		= {0.5f , 0.5f               , 0.0f , 1.0f};
	stxp	::gpk::rgbaf	DARKMAGENTA		= {0.5f , 0.0f               , 0.5f , 1.0f};
	stxp	::gpk::rgbaf	DARKCYAN		= {0.0f , 0.5f               , 0.5f , 1.0f};
	stxp	::gpk::rgbaf	DARKORANGE		= {1.0f , 0.5490196078431373f, 0.00f, 1.0f};
#pragma pack(pop)
} // namespace

#endif // GPK_COLOR_H_23627
