/// Copyright 2009-2017 - asm128
#include "gpk_math.h"

#ifndef GPK_COLOR_H_23627
#define GPK_COLOR_H_23627

namespace gpk
{
#pragma pack(push, 1)
#define COLOR_RGBA_OPERATOR_MEMBERS(color_struct_type) \
	cnstxpr	bool		operator==			(const color_struct_type & other)	const	noexcept	{ return b == other.b && g == other.g && r == other.r && a == other.a; } \
	cnstxpr	bool		operator!=			(const color_struct_type & other)	const	noexcept	{ return b != other.b || g != other.g || r != other.r || a != other.a; } \

#define COLOR_RGB_OPERATOR_MEMBERS(color_struct_type) \
	cnstxpr	bool		operator==			(const color_struct_type & other)	const	noexcept	{ return b == other.b && g == other.g && r == other.r; } \
	cnstxpr	bool		operator!=			(const color_struct_type & other)	const	noexcept	{ return b != other.b || g != other.g || r != other.r; } \

	tplt <tpnm _tBase> struct color_a								{ _tBase a; GPK_DEFAULT_OPERATOR(color_a	, a == other.a); };
	tplt <tpnm _tBase> struct color_b								{ _tBase b; GPK_DEFAULT_OPERATOR(color_b	, b == other.b); };
	tplt <tpnm _tBase> struct color_r								{ _tBase r; GPK_DEFAULT_OPERATOR(color_r	, r == other.r); };

	tplt <tpnm _tBase> struct color_ab		: color_a	<_tBase>	{ _tBase b; GPK_DEFAULT_OPERATOR(color_ab	, color_a	<_tBase>::operator==(other) && b == other.b); };
	tplt <tpnm _tBase> struct color_abg		: color_ab	<_tBase>	{ _tBase g; GPK_DEFAULT_OPERATOR(color_abg	, color_ab	<_tBase>::operator==(other) && g == other.g); };
	tplt <tpnm _tBase> struct color_abgr	: color_abg	<_tBase>	{ _tBase r; GPK_DEFAULT_OPERATOR(color_abgr	, color_abg	<_tBase>::operator==(other) && r == other.r); };

	tplt <tpnm _tBase> struct color_ar		: color_a	<_tBase>	{ _tBase r; GPK_DEFAULT_OPERATOR(color_ar	, color_a	<_tBase>::operator==(other) && r == other.r); };
	tplt <tpnm _tBase> struct color_arg		: color_ar	<_tBase>	{ _tBase g; GPK_DEFAULT_OPERATOR(color_arg	, color_ar	<_tBase>::operator==(other) && g == other.g); };
	tplt <tpnm _tBase> struct color_argb	: color_arg	<_tBase>	{ _tBase b; GPK_DEFAULT_OPERATOR(color_argb	, color_arg	<_tBase>::operator==(other) && b == other.b); };

	tplt <tpnm _tBase> struct color_bg		: color_b<_tBase>		{ _tBase g; GPK_DEFAULT_OPERATOR(color_bg	, color_b	<_tBase>::operator==(other) && g == other.g); };
	tplt <tpnm _tBase> struct color_bgr		: color_bg<_tBase>		{ _tBase r; GPK_DEFAULT_OPERATOR(color_bgr	, color_bg	<_tBase>::operator==(other) && r == other.r); };
	tplt <tpnm _tBase> struct color_bgra	: color_bgr<_tBase>		{ _tBase a; GPK_DEFAULT_OPERATOR(color_bgra	, color_bgr	<_tBase>::operator==(other) && a == other.a); };

	tplt <tpnm _tBase> struct color_rg		: color_r<_tBase>		{ _tBase g; GPK_DEFAULT_OPERATOR(color_rg	, color_r	<_tBase>::operator==(other) && g == other.g); };
	tplt <tpnm _tBase> struct color_rgb		: color_rg<_tBase>		{ _tBase b; GPK_DEFAULT_OPERATOR(color_rgb	, color_rg	<_tBase>::operator==(other) && b == other.b); };
	tplt <tpnm _tBase> struct color_rgba	: color_rgb<_tBase>		{ _tBase a; GPK_DEFAULT_OPERATOR(color_rgba	, color_rgb	<_tBase>::operator==(other) && a == other.a); };

	typedef	uint16_t		SColor16;

	typedef color_bgr <u8>	bgr8 ;
	typedef color_rgb <u8>	rgb8 ;
	typedef color_bgra<u8>	bgra8;
	typedef color_rgba<u8>	rgba8;
	typedef color_abgr<u8>	abgr8;
	typedef color_argb<u8>	argb8;

	typedef color_bgr <u16>	bgr16 ;
	typedef color_rgb <u16>	rgb16 ;
	typedef color_bgra<u16>	bgra16;
	typedef color_rgba<u16>	rgba16;
	typedef color_abgr<u16>	abgr16;
	typedef color_argb<u16>	argb16;

	typedef color_bgr <f32>	bgrf32;
	typedef color_rgb <f32>	rgbf32;
	typedef color_bgra<f32>	bgraf32;
	typedef color_rgba<f32>	rgbaf32;
	typedef color_abgr<f32>	abgrf32;
	typedef color_argb<f32>	argbf32;

	typedef const color_bgr <u8>	cbgr8 ;
	typedef const color_rgb <u8>	crgb8 ;
	typedef const color_bgra<u8>	cbgra8;
	typedef const color_rgba<u8>	crgba8;
	typedef const color_abgr<u8>	cabgr8;
	typedef const color_argb<u8>	cargb8;

	typedef const color_bgr <u16>	cbgr16 ;
	typedef const color_rgb <u16>	crgb16 ;
	typedef const color_bgra<u16>	cbgra16;
	typedef const color_rgba<u16>	crgba16;
	typedef const color_abgr<u16>	cabgr16;
	typedef const color_argb<u16>	cargb16;

	typedef const color_bgr <f32>	cbgrf32;
	typedef const color_rgb <f32>	crgbf32;
	typedef const color_bgra<f32>	cbgraf32;
	typedef const color_rgba<f32>	crgbaf32;
	typedef const color_abgr<f32>	cabgrf32;
	typedef const color_argb<f32>	cargbf32;

	cnstxpr	float			BYTE_SCALE			= 1.0f / 255.0f;

	// Stores RGBA color channels
	struct SColorRGBA : public ::gpk::rgba8 {
								SColorRGBA		()									noexcept	= default;
		cnstxpr					SColorRGBA		(const SColorRGBA &)				noexcept	= default;
		cnstxpr					SColorRGBA		(u8 r_, u8 g_, u8 b_, u8 a_=0xff)	noexcept	: color_rgba<u8>{r_, g_, b_, a_}						{}
		cnstxpr					SColorRGBA		(uint32_t other)					noexcept	: color_rgba<u8>
			{ u8(((other & 0x000000FF) >>  0))
			, u8(((other & 0x0000FF00) >>  8))
			, u8(((other & 0x00FF0000) >> 16))
			, u8(((other & 0xFF000000) >> 24))
			}
		{}

		COLOR_RGBA_OPERATOR_MEMBERS(SColorRGBA); 
		cnstxpr	operator		uint32_t		()							const	noexcept	{ return (((uint32_t)a) << 24) | (((uint32_t)b) << 16) | (((uint32_t)g) << 8) | (((uint32_t)r) << 0);					}


		cnstxpr	bool			operator ==		(uint32_t other)			const	noexcept	{ return other == *((const uint32_t*)this);			}
		cnstxpr	SColorRGBA		operator *		(const SColorRGBA & color)	const	noexcept	{ return {(u8)::gpk::clamped(r * (uint16_t)color.r, 0, 255)	, (u8)::gpk::clamped(g * (uint16_t)color.g, 0, 255)	, (u8)::gpk::clamped(b * (uint16_t)color.b, 0, 255)	, a};	}
		cnstxpr	SColorRGBA		operator +		(const SColorRGBA & color)	const	noexcept	{ return {(u8)::gpk::clamped(r + (uint16_t)color.r, 0, 255)	, (u8)::gpk::clamped(g + (uint16_t)color.g, 0, 255)	, (u8)::gpk::clamped(b + (uint16_t)color.b, 0, 255)	, a};	}
		cnstxpr	SColorRGBA		operator *		(double scalar)				const	noexcept	{ return {(u8)::gpk::clamped(r * scalar, 0.0 , 255.0)		, (u8)::gpk::clamped(g * scalar, 0.0 , 255.0 )		, (u8)::gpk::clamped(b * scalar, 0.0,  255.0 )		, a};	}
		cnstxpr	SColorRGBA		operator /		(double scalar)				const				{ return {(u8)::gpk::clamped(r / scalar, 0.0 , 255.0)		, (u8)::gpk::clamped(g / scalar, 0.0 , 255.0 )		, (u8)::gpk::clamped(b / scalar, 0.0,  255.0 )		, a};	}
	};	// struct

	// Stores BGRA color channels
	struct SColorBGRA : public ::gpk::bgra8 {
								SColorBGRA		()									noexcept	= default;
		cnstxpr					SColorBGRA		(const SColorBGRA &)				noexcept	= default;
		cnstxpr					SColorBGRA		(const SColorRGBA & other)			noexcept	: color_bgra<u8>{other.b, other.g, other.r, other.a} {}
		cnstxpr					SColorBGRA		(u8 b_, u8 g_, u8 r_, u8 a_=0xff)	noexcept	: color_bgra<u8>{b_, g_, r_, a_} {}
		cnstxpr					SColorBGRA		(uint32_t other)					noexcept	: color_bgra<u8>
			{ u8(((other & 0x000000FF) >>  0)) 
			, u8(((other & 0x0000FF00) >>  8))
			, u8(((other & 0x00FF0000) >> 16))
			, u8(((other & 0xFF000000) >> 24))
			}
		{}

		COLOR_RGBA_OPERATOR_MEMBERS(SColorBGRA); 
		cnstxpr	operator		SColorRGBA		()							const	noexcept	{ return {r, g, b, a};						}
		cnstxpr	operator		uint32_t		()							const	noexcept	{ return (((uint32_t)a) << 24) | (((uint32_t)r) << 16) | (((uint32_t)g) << 8) | (((uint32_t)b) << 0);					}

		cnstxpr	bool			operator==		(uint32_t other)			const	noexcept	{ return other == *((const uint32_t*)this);			}
		cnstxpr	SColorBGRA		operator*		(const SColorBGRA & color)	const	noexcept	{ return {(u8)::gpk::min(b * (uint16_t)color.b, 255)	, (u8)::gpk::min(g * (uint16_t)color.g, 255)	,(u8)::gpk::min(r * (uint16_t)color.r, 255), a};			}
		cnstxpr	SColorBGRA		operator+		(const SColorBGRA & color)	const	noexcept	{ return {(u8)::gpk::min(b + (uint16_t)color.b, 255)	, (u8)::gpk::min(g + (uint16_t)color.g, 255)	,(u8)::gpk::min(r + (uint16_t)color.r, 255), a};			}
		cnstxpr	SColorBGRA		operator-		(const SColorBGRA & color)	const	noexcept	{ return {(u8)::gpk::clamped(b - (int16_t)color.b, 0, 255), (u8)::gpk::clamped(g - (int16_t)color.g, 0, 255),(u8)::gpk::clamped(r - (int16_t)color.r, 0, 255), a};	}
		cnstxpr	SColorBGRA		operator*		(float scalar)				const	noexcept	{ return {(u8)::gpk::clamped(b * scalar, 0.0f, 255.0f)	, (u8)::gpk::clamped(g * scalar, 0.0f, 255.0f)	,(u8)::gpk::clamped(r * scalar, 0.0f, 255.0f), a};			}
		cnstxpr	SColorBGRA		operator/		(float scalar)				const				{ return {(u8)::gpk::clamped(b / scalar, 0.0f, 255.0f)	, (u8)::gpk::clamped(g / scalar, 0.0f, 255.0f)	,(u8)::gpk::clamped(r / scalar, 0.0f, 255.0f), a};			}
		cnstxpr	SColorBGRA		operator*		(double scalar)				const	noexcept	{ return {(u8)::gpk::clamped(b * scalar, 0.0,  255.0 )	, (u8)::gpk::clamped(g * scalar, 0.0 , 255.0 )	,(u8)::gpk::clamped(r * scalar, 0.0, 255.0),	a};				}
		cnstxpr	SColorBGRA		operator/		(double scalar)				const				{ return {(u8)::gpk::clamped(b / scalar, 0.0,  255.0 )	, (u8)::gpk::clamped(g / scalar, 0.0 , 255.0 )	,(u8)::gpk::clamped(r / scalar, 0.0, 255.0),	a};				}
				SColorBGRA&		operator*=		(float scalar)						noexcept	{ b = (u8)::gpk::clamped(b * scalar, 0.0f, 255.0f); g = (u8)::gpk::clamped(g * scalar, 0.0f, 255.0f); r = (u8)::gpk::clamped(r * scalar, 0.0f, 255.0f);	return *this; }
				SColorBGRA&		operator/=		(float scalar)									{ b = (u8)::gpk::clamped(b / scalar, 0.0f, 255.0f); g = (u8)::gpk::clamped(g / scalar, 0.0f, 255.0f); r = (u8)::gpk::clamped(r / scalar, 0.0f, 255.0f);	return *this; }
				SColorBGRA&		operator*=		(double scalar)						noexcept	{ b = (u8)::gpk::clamped(b * scalar, 0.0,  255.0 ); g = (u8)::gpk::clamped(g * scalar, 0.0 , 255.0 ); r = (u8)::gpk::clamped(r * scalar, 0.0, 255.0);		return *this; }
				SColorBGRA&		operator/=		(double scalar)									{ b = (u8)::gpk::clamped(b / scalar, 0.0,  255.0 ); g = (u8)::gpk::clamped(g / scalar, 0.0 , 255.0 ); r = (u8)::gpk::clamped(r / scalar, 0.0, 255.0);		return *this; }

				SColorBGRA&		FromBGR16		(const SColor16 & other)			noexcept	{ b = u8((other & 0x1F) / float(0x1F) * 255); g = u8(((other & 0x07E0) >> 5) / float(0x3F) * 255); r = u8(((other & 0xF800) >> 11) / float(0x1F) * 255);	return *this; }
				SColorBGRA&		FromRGB16		(const SColor16 & other)			noexcept	{ r = u8((other & 0x1F) / float(0x1F) * 255); g = u8(((other & 0x07E0) >> 5) / float(0x3F) * 255); b = u8(((other & 0xF800) >> 11) / float(0x1F) * 255);	return *this; }
	};	// struct

	// Stores RGB color channels
	struct SColorRGB : public ::gpk::rgb8 {
								SColorRGB		()									noexcept	= default;
		cnstxpr					SColorRGB		(const SColorRGB & other)			noexcept	= default;
		cnstxpr					SColorRGB		(u8 r_, u8 g_, u8 b_)				noexcept	: color_rgb<u8>{r_, g_, b_} {}
		cnstxpr					SColorRGB		(uint32_t other)					noexcept	: color_rgb<u8>
			{ u8(((other & 0x000000FF) >>  0))
			, u8(((other & 0x0000FF00) >>  8))
			, u8(((other & 0x00FF0000) >> 16))
			}
		{}

		COLOR_RGB_OPERATOR_MEMBERS(SColorRGB); 
		cnstxpr	operator		uint32_t		()							const	noexcept	{ return  0xFF000000 | (((uint32_t)b) << 16) | (((uint32_t)g) << 8)	| (((uint32_t)r) << 0);  }
		cnstxpr	operator		SColorRGBA		()							const	noexcept	{ return {0xFF000000 | (((uint32_t)b) << 16) | (((uint32_t)g) << 8)	| (((uint32_t)r) << 0)}; }
		cnstxpr	operator		SColor16		()							const	noexcept	{ return (((uint16_t)(r * BYTE_SCALE * 0x001F)) << 0) | (((uint16_t)(g * BYTE_SCALE * 0x003F)) << 5) | (((uint16_t)(b * BYTE_SCALE * 0x001F)) << 11 );												}
		cnstxpr	bool			operator ==		(const SColorRGBA & other)	const	noexcept	{ return r == other.r && g == other.g && b == other.b;	}
		cnstxpr	bool			operator !=		(const SColorRGBA & other)	const	noexcept	{ return r != other.r || g != other.g || b != other.b;	}
		cnstxpr	bool			operator ==		(const SColor16   & other)	const	noexcept	{ return (SColor16)(*this) == other;					}
		cnstxpr	bool			operator !=		(const SColor16   & other)	const	noexcept	{ return (SColor16)(*this) != other;					}
		cnstxpr	SColorRGB		operator *		(float scalar)				const	noexcept	{ return {(u8)::gpk::clamped(r * scalar, 0.0f, 255.0f), (u8)::gpk::clamped(g * scalar, 0.0f, 255.0f),	(u8)::gpk::clamped(b * scalar, 0.0f, 255.0f)}; }
		cnstxpr	SColorRGB		operator /		(float scalar)				const				{ return {(u8)::gpk::clamped(r / scalar, 0.0f, 255.0f), (u8)::gpk::clamped(g / scalar, 0.0f, 255.0f),	(u8)::gpk::clamped(b / scalar, 0.0f, 255.0f)}; }
		cnstxpr	SColorRGB		operator *		(double scalar)				const	noexcept	{ return {(u8)::gpk::clamped(r * scalar, 0.0,  255.0),  (u8)::gpk::clamped(g * scalar, 0.0,  255.0),	(u8)::gpk::clamped(b * scalar, 0.0,  255.0)};  }
		cnstxpr	SColorRGB		operator /		(double scalar)				const				{ return {(u8)::gpk::clamped(r / scalar, 0.0,  255.0),  (u8)::gpk::clamped(g / scalar, 0.0,  255.0),	(u8)::gpk::clamped(b / scalar, 0.0,  255.0)};  }
		cnstxpr	SColorRGB		operator *		(const SColorRGB  & color)	const	noexcept	{ return {(u8)::gpk::clamped(r * (uint16_t)color.r, 0, 255), (u8)::gpk::clamped(g * (uint16_t)color.g, 0, 255), (u8)::gpk::clamped(b * (uint16_t)color.b, 0, 255)};			}
		cnstxpr	SColorRGB		operator +		(const SColorRGB  & color)	const	noexcept	{ return {(u8)::gpk::clamped(r + (uint16_t)color.r, 0, 255), (u8)::gpk::clamped(g + (uint16_t)color.g, 0, 255), (u8)::gpk::clamped(b + (uint16_t)color.b, 0, 255)};			}
		cnstxpr	SColorRGBA		operator *		(const SColorRGBA & color)	const	noexcept	{ return {(u8)::gpk::clamped(r * (uint16_t)color.r, 0, 255), (u8)::gpk::clamped(g * (uint16_t)color.g, 0, 255), (u8)::gpk::clamped(b * (uint16_t)color.b, 0, 255), color.a};	}
		cnstxpr	SColorRGBA		operator +		(const SColorRGBA & color)	const	noexcept	{ return {(u8)::gpk::clamped(r + (uint16_t)color.r, 0, 255), (u8)::gpk::clamped(g + (uint16_t)color.g, 0, 255), (u8)::gpk::clamped(b + (uint16_t)color.b, 0, 255), color.a};	}
	};	// struct

	// Stores BGR color channels
	struct SColorBGR : public ::gpk::color_bgr<u8> {
								SColorBGR		()									noexcept	= default;
		cnstxpr					SColorBGR		(const SColorBGR & other)			noexcept	= default;
		cnstxpr					SColorBGR		(u8 b_, u8 g_, u8 r_)				noexcept	: color_bgr<u8>{b_, g_, r_} {}
		cnstxpr					SColorBGR		(uint32_t other)					noexcept	: color_bgr<u8>
			{ u8(((other & 0x000000FF) >>  0))
			, u8(((other & 0x0000FF00) >>  8))
			, u8(((other & 0x00FF0000) >> 16))
			}
		{}

		COLOR_RGB_OPERATOR_MEMBERS(SColorBGR); 
		cnstxpr	operator		uint32_t		()							const	noexcept	{ return 0xFF000000 | (((uint32_t)r) << 16)	| (((uint32_t)g) << 8) | (((uint32_t)b) << 0);																	}
		cnstxpr	operator		SColorBGRA		()							const	noexcept	{ return {b, g, r, 0xFF};														}
		cnstxpr	operator		SColor16		()							const	noexcept	{ return (((uint16_t)(b * BYTE_SCALE * 0x001F)) << 0) | (((uint16_t)(g * BYTE_SCALE * 0x003F)) << 5) | (((uint16_t)(r * BYTE_SCALE * 0x001F)) << 11 );	}

		cnstxpr	bool			operator ==		(const SColorBGRA & other)	const	noexcept	{ return r == other.r && g == other.g && b == other.b;	}
		cnstxpr	bool			operator !=		(const SColorBGRA & other)	const	noexcept	{ return r != other.r || g != other.g || b != other.b;	}
		cnstxpr	bool			operator ==		(const SColor16   & other)	const	noexcept	{ return (SColor16)(*this) == other;					}
		cnstxpr	bool			operator !=		(const SColor16   & other)	const	noexcept	{ return (SColor16)(*this) != other;					}
		cnstxpr	SColorBGR		operator *		(float scalar)				const	noexcept	{ return {(u8)::gpk::clamped(b * scalar, 0.0f, 255.0f), (u8)::gpk::clamped(g * scalar, 0.0f, 255.0f), (u8)::gpk::clamped(r * scalar, 0.0f, 255.0f)};	}
		cnstxpr	SColorBGR		operator /		(float scalar)				const				{ return {(u8)::gpk::clamped(b / scalar, 0.0f, 255.0f), (u8)::gpk::clamped(g / scalar, 0.0f, 255.0f), (u8)::gpk::clamped(r / scalar, 0.0f, 255.0f)};	}
		cnstxpr	SColorBGR		operator *		(double scalar)				const	noexcept	{ return {(u8)::gpk::clamped(b * scalar, 0.0,  255.0) , (u8)::gpk::clamped(g * scalar, 0.0,  255.0) , (u8)::gpk::clamped(r * scalar, 0.0,  255.0)};	}
		cnstxpr	SColorBGR		operator /		(double scalar)				const				{ return {(u8)::gpk::clamped(b / scalar, 0.0,  255.0) , (u8)::gpk::clamped(g / scalar, 0.0,  255.0) , (u8)::gpk::clamped(r / scalar, 0.0,  255.0)};	}
		cnstxpr	SColorBGR		operator *		(const SColorBGR  & color)	const	noexcept	{ return {(u8)::gpk::clamped(b * (uint16_t)color.b, 0, 255) , (u8)::gpk::clamped(g * (uint16_t)color.g, 0, 255)	, (u8)::gpk::clamped(r * (uint16_t)color.r, 0, 255)};			}
		cnstxpr	SColorBGR		operator +		(const SColorBGR  & color)	const	noexcept	{ return {(u8)::gpk::clamped(b + (uint16_t)color.b, 0, 255) , (u8)::gpk::clamped(g + (uint16_t)color.g, 0, 255)	, (u8)::gpk::clamped(r + (uint16_t)color.r, 0, 255)};			}
		cnstxpr	SColorBGRA		operator *		(const SColorBGRA & color)	const	noexcept	{ return {(u8)::gpk::clamped(b * (uint16_t)color.b, 0, 255) , (u8)::gpk::clamped(g * (uint16_t)color.g, 0, 255)	, (u8)::gpk::clamped(r * (uint16_t)color.r, 0, 255), color.a};	}
		cnstxpr	SColorBGRA		operator +		(const SColorBGRA & color)	const	noexcept	{ return {(u8)::gpk::clamped(b + (uint16_t)color.b, 0, 255) , (u8)::gpk::clamped(g + (uint16_t)color.g, 0, 255)	, (u8)::gpk::clamped(r + (uint16_t)color.r, 0, 255), color.a};	}

				SColorBGR&		FromBGR16		(const SColor16   & other)			noexcept	{ b = u8((other & 0x1F) / float(0x1F) * 255); g = u8(((other & 0x07E0) >> 5) / float(0x3F) * 255); r = u8(((other & 0xF800) >> 11) / float(0x1F) * 255);	return *this; }
				SColorBGR&		FromRGB16		(const SColor16   & other)			noexcept	{ r = u8((other & 0x1F) / float(0x1F) * 255); g = u8(((other & 0x07E0) >> 5) / float(0x3F) * 255); b = u8(((other & 0xF800) >> 11) / float(0x1F) * 255);	return *this; }
	};	// struct

	// Stores RGBA floating point color channels
	struct SColorFloat : public ::gpk::rgbaf32 {
								SColorFloat		()										noexcept	= default;
		cnstxpr					SColorFloat		(const SColorFloat & color)				noexcept	= default;
		cnstxpr					SColorFloat		(f32 r_, f32 g_, f32 b_, f32 a_=1.0f)	noexcept	: color_rgba<float>{r_, g_, b_, a_}						{}
		cnstxpr					SColorFloat		(const float* rgbaColor)							: color_rgba<float>{rgbaColor[0], rgbaColor[1], rgbaColor[2], rgbaColor[3]}	{}
		cnstxpr					SColorFloat		(SColorBGRA color)						noexcept	: color_rgba<float>
			{ color.r * BYTE_SCALE
			, color.g * BYTE_SCALE
			, color.b * BYTE_SCALE
			, color.a * BYTE_SCALE
			} {}

		cnstxpr					SColorFloat		(::gpk::bgr8 color, uint8_t alpha)		noexcept	: color_rgba<float>
			{ color.r * BYTE_SCALE
			, color.g * BYTE_SCALE
			, color.b * BYTE_SCALE
			, alpha * BYTE_SCALE
			} {}

		cnstxpr					SColorFloat		(::gpk::rgbf32 color, float alpha)		noexcept	: color_rgba<float>
			{ color.r 
			, color.g 
			, color.b 
			, alpha 
			} {}

		COLOR_RGBA_OPERATOR_MEMBERS(SColorFloat); 
		cnstxpr	operator		SColorBGRA		()							const	noexcept	{ return {(u8)(b * 255), (u8)(g * 255), (u8)(r * 255), (u8)(a * 255)};	}
		cnstxpr	operator		const float*	()							const	noexcept	{ return &r;															}
				SColorFloat&	operator +=		(const SColorFloat & color)			noexcept	{ r = r + color.r;					g = g + color.g;				b = b + color.b;				return *this;	}
				SColorFloat&	operator *=		(const SColorFloat & color)			noexcept	{ r = r * color.r;					g = g * color.g;				b = b * color.b;				return *this;	}
				SColorFloat&	operator *=		(double scalar)						noexcept	{ r = (float)(r * scalar);			g = (float)(g * scalar);		b = (float)(b * scalar);		return *this;	}
				SColorFloat&	operator /=		(double scalar)									{ r = (float)(r / scalar);			g = (float)(g / scalar);		b = (float)(b / scalar);		return *this;	}
				SColorFloat&	operator *=		(float scalar)						noexcept	{ r = r * scalar;					g = g * scalar;					b = b * scalar;					return *this;	}
				SColorFloat&	operator /=		(float scalar)									{ r = r / scalar;					g = g / scalar;					b = b / scalar;					return *this;	}
				SColorFloat&	operator *=		(const SColorBGRA  & color)			noexcept	{ r = r * (color.r * BYTE_SCALE);	g = g * (color.g * BYTE_SCALE);	b = b * (color.b * BYTE_SCALE);	return *this;	}
				SColorFloat&	operator +=		(const SColorBGRA  & color)			noexcept	{ r = r + (color.r * BYTE_SCALE);	g = g + (color.g * BYTE_SCALE);	b = b + (color.b * BYTE_SCALE);	return *this;	}
				SColorFloat&	operator *=		(const SColorBGR   & color)			noexcept	{ r = r * (color.r * BYTE_SCALE);	g = g * (color.g * BYTE_SCALE);	b = b * (color.b * BYTE_SCALE);	return *this;	}
				SColorFloat&	operator +=		(const SColorBGR   & color)			noexcept	{ r = r + (color.r * BYTE_SCALE);	g = g + (color.g * BYTE_SCALE);	b = b + (color.b * BYTE_SCALE);	return *this;	}
				SColorFloat		operator *		(const SColorBGRA  & color)	const	noexcept	{ return {r * (color.r * BYTE_SCALE), g * (color.g * BYTE_SCALE), b * (color.b * BYTE_SCALE), a};	}
				SColorFloat		operator +		(const SColorBGRA  & color)	const	noexcept	{ return {r + (color.r * BYTE_SCALE), g + (color.g * BYTE_SCALE), b + (color.b * BYTE_SCALE), a};	}
				SColorFloat		operator *		(const SColorBGR   & color)	const	noexcept	{ return {r * (color.r * BYTE_SCALE), g * (color.g * BYTE_SCALE), b * (color.b * BYTE_SCALE), a};	}
				SColorFloat		operator +		(const SColorBGR   & color)	const	noexcept	{ return {r + (color.r * BYTE_SCALE), g + (color.g * BYTE_SCALE), b + (color.b * BYTE_SCALE), a};	}
		cnstxpr	SColorFloat		operator *		(const SColorFloat & color)	const	noexcept	{ return {r * color.r, g * color.g, b * color.b, a};												}
		cnstxpr	SColorFloat		operator +		(const SColorFloat & color)	const	noexcept	{ return {r + color.r, g + color.g, b + color.b, a};												}
		cnstxpr	SColorFloat		operator -		(const SColorFloat & color)	const	noexcept	{ return {r - color.r, g - color.g, b - color.b, a};												}
		cnstxpr	SColorFloat		operator *		(double scalar)				const	noexcept	{ return {(float)(r * scalar), (float)(g * scalar), (float)(b * scalar), a};						}
		cnstxpr	SColorFloat		operator *		(float scalar)				const	noexcept	{ return {(r * scalar), (g * scalar),	(b * scalar)};												}
		cnstxpr	SColorFloat		operator /		(double scalar)				const				{ return {(float)(r / scalar), (float)(g / scalar), (float)(b / scalar), a};						}
		cnstxpr	SColorFloat		operator /		(float scalar)				const				{ return {r / scalar,	g / scalar,	b / scalar};													}

				SColorFloat&	Clamp			()									noexcept	{ r = ::gpk::clamped(r, 0.0f, 1.0f); g = ::gpk::clamped(g, 0.0f, 1.0f); b = ::gpk::clamped(b, 0.0f, 1.0f); return *this;	}

		cnstxpr	::gpk::rgbf32	rgb				()							const				{ return {r, g, b}; }
	};	// struct

	typedef ::gpk::SColorFloat					frgba	;
	typedef ::gpk::SColorBGRA					u8bgra	;
	typedef ::gpk::SColorBGR					u8bgr	;
	typedef ::gpk::SColorRGBA					u8rgba	;
	typedef ::gpk::SColorRGB					u8rgb	;

	typedef const frgba							cfrgba	;
	typedef const u8bgra						c8bgra	;
	typedef const u8bgr							c8bgr	;
	typedef const u8rgba						c8rgba	;
	typedef const u8rgb							c8rgb	;

	typedef frgba								rgbaf	;
	typedef u8bgra								bgra	;
	typedef u8bgr								bgr		;
	typedef u8rgba								rgba	;
	typedef u8rgb								rgb		;


	stacxpr	::gpk::rgbaf	BLACK			= {0.0f , 0.0f               , 0.00f, 1.0f};
	stacxpr	::gpk::rgbaf	WHITE			= {1.0f , 1.0f               , 1.0f , 1.0f};
	stacxpr	::gpk::rgbaf	RED				= {1.0f , 0.0f               , 0.0f , 1.0f};
	stacxpr	::gpk::rgbaf	GREEN			= {0.0f , 1.0f               , 0.0f , 1.0f};
	stacxpr	::gpk::rgbaf	BLUE			= {0.0f , 0.0f               , 1.0f , 1.0f};
	stacxpr	::gpk::rgbaf	YELLOW			= {1.0f , 1.0f               , 0.0f , 1.0f};
	stacxpr	::gpk::rgbaf	MAGENTA			= {1.0f , 0.0f               , 1.0f , 1.0f};
	stacxpr	::gpk::rgbaf	CYAN			= {0.0f , 1.0f               , 1.0f , 1.0f};
	stacxpr	::gpk::rgbaf	ORANGE			= {1.0f , 0.647f             , 0.0f , 1.0f};
	stacxpr	::gpk::rgbaf	PANOCHE			= {0.25f, 0.5f               , 1.0f , 1.0f};
	stacxpr	::gpk::rgbaf	PURPLE			= {0.5f , 0.1f               , 1.0f , 1.0f};
	stacxpr	::gpk::rgbaf	TURQUOISE		= {0.2f , 1.0                , 0.65f, 1.0f};
	stacxpr	::gpk::rgbaf	BROWN			= {0.4f , 0.223f             , 0.0f , 1.0f};
	stacxpr	::gpk::rgbaf	GRAY			= {0.5f , 0.5f               , 0.5f , 1.0f};
	stacxpr	::gpk::rgbaf	LIGHTGRAY		= {0.75f, 0.75f              , 0.75f, 1.0f};
	stacxpr	::gpk::rgbaf	LIGHTRED		= {1.0f , 0.25f              , 0.25f, 1.0f};
	stacxpr	::gpk::rgbaf	LIGHTGREEN		= {0.5f , 1.0f               , 0.5f , 1.0f};
	stacxpr	::gpk::rgbaf	LIGHTBLUE		= {0.25f, 0.25f              , 1.0f , 1.0f};
	stacxpr	::gpk::rgbaf	LIGHTYELLOW		= {1.0f , 1.0f               , 0.25f, 1.0f};
	stacxpr	::gpk::rgbaf	LIGHTMAGENTA	= {1.0f , 0.25f              , 1.0f , 1.0f};
	stacxpr	::gpk::rgbaf	LIGHTCYAN		= {0.25f, 1.0f               , 1.0f , 1.0f};
	stacxpr	::gpk::rgbaf	LIGHTORANGE		= {1.0f , 0.780f             , 0.25f, 1.0f};
	stacxpr	::gpk::rgbaf	DARKGRAY		= {0.25f, 0.25f              , 0.25f, 1.0f};
	stacxpr	::gpk::rgbaf	DARKRED			= {0.5f , 0.0f               , 0.0f , 1.0f};
	stacxpr	::gpk::rgbaf	DARKGREEN		= {0.0f , 0.5f               , 0.0f , 1.0f};
	stacxpr	::gpk::rgbaf	DARKBLUE		= {0.0f , 0.0f               , 0.5f , 1.0f};
	stacxpr	::gpk::rgbaf	DARKYELLOW		= {0.5f , 0.5f               , 0.0f , 1.0f};
	stacxpr	::gpk::rgbaf	DARKMAGENTA		= {0.5f , 0.0f               , 0.5f , 1.0f};
	stacxpr	::gpk::rgbaf	DARKCYAN		= {0.0f , 0.5f               , 0.5f , 1.0f};
	stacxpr	::gpk::rgbaf	DARKORANGE		= {1.0f , 0.5490196078431373f, 0.00f, 1.0f};
#pragma pack(pop)
} // namespace

#endif // GPK_COLOR_H_23627
