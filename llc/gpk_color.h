/// Copyright 2009-2017 - asm128
#include "gpk_math.h"
#include "gpk_enum.h"
#include "gpk_view_grid.h"

#ifndef GPK_COLOR_H_29734982734
#define GPK_COLOR_H_29734982734

namespace gpk
{
#pragma pack(push, 1)
#define COLOR_RGBA_OPERATOR_MEMBERS(color_struct_type) \
	cnstxpr	bool		operator==			(const color_struct_type & other)	const	noexcept	{ return b == other.b && g == other.g && r == other.r && a == other.a; } \
	cnstxpr	bool		operator!=			(const color_struct_type & other)	const	noexcept	{ return b != other.b || g != other.g || r != other.r || a != other.a; } \

#define COLOR_RGB_OPERATOR_MEMBERS(color_struct_type) \
	cnstxpr	bool		operator==			(const color_struct_type & other)	const	noexcept	{ return b == other.b && g == other.g && r == other.r; } \
	cnstxpr	bool		operator!=			(const color_struct_type & other)	const	noexcept	{ return b != other.b || g != other.g || r != other.r; } \

	template <typename _tBase> struct color_bgr		{ _tBase b, g, r;    COLOR_RGB_OPERATOR_MEMBERS (color_bgr ); };
	template <typename _tBase> struct color_rgb		{ _tBase r, g, b;    COLOR_RGB_OPERATOR_MEMBERS (color_rgb ); };
	template <typename _tBase> struct color_bgra	{ _tBase b, g, r, a; COLOR_RGBA_OPERATOR_MEMBERS(color_bgra); };
	template <typename _tBase> struct color_rgba	{ _tBase r, g, b, a; COLOR_RGBA_OPERATOR_MEMBERS(color_rgba); };
	template <typename _tBase> struct color_abgr	{ _tBase a, b, g, r; COLOR_RGBA_OPERATOR_MEMBERS(color_abgr); };
	template <typename _tBase> struct color_argb	{ _tBase a, r, g, b; COLOR_RGBA_OPERATOR_MEMBERS(color_argb); };

	typedef	uint16_t		SColor16;

	typedef color_bgr <u8>	bgr8 ;
	typedef color_rgb <u8>	rgb8 ;
	typedef color_bgra<u8>	bgra8;
	typedef color_rgba<u8>	rgba8;
	typedef color_abgr<u8>	abgr8;
	typedef color_argb<u8>	argb8;

	typedef color_bgr <f32>	bgrf32;
	typedef color_rgb <f32>	rgbf32;
	typedef color_bgra<f32>	bgraf32;
	typedef color_rgba<f32>	rgbaf32;
	typedef color_abgr<f32>	abgrf32;
	typedef color_argb<f32>	argbf32;

	cnstxpr	float			BYTE_SCALE			= 1.0f / 255.0f;

	// Stores RGBA color channels
	struct SColorRGBA : public ::gpk::rgba8 {
								SColorRGBA		()									noexcept	= default;
		cnstxpr					SColorRGBA		(const SColorRGBA &)				noexcept	= default;
		cnstxpr					SColorRGBA		(u8 r_, u8 g_, u8 b_, u8 a_=0xff)	noexcept	: color_rgba<u8>{r_, g_, b_, a_}																																										{}
		cnstxpr					SColorRGBA		(uint32_t other)					noexcept	: color_rgba<u8>
			{ u8(((other & 0x000000FF) >>  0))
			, u8(((other & 0x0000FF00) >>  8))
			, u8(((other & 0x00FF0000) >> 16))
			, u8(((other & 0xFF000000) >> 24))
			}
		{}

		COLOR_RGBA_OPERATOR_MEMBERS(SColorRGBA); 
		cnstxpr	operator		uint32_t		()							const	noexcept	{ return (((uint32_t)a) << 24) | (((uint32_t)b) << 16) | (((uint32_t)g) << 8) | (((uint32_t)r) << 0);																								}


		cnstxpr	bool			operator ==		(uint32_t other)			const	noexcept	{ return other == *((const uint32_t*)this);																																							}
		cnstxpr	SColorRGBA		operator *		(const SColorRGBA & color)	const	noexcept	{ return {(u8)::gpk::clamp(r * (uint16_t)color.r, 0, 255)	, (u8)::gpk::clamp(g * (uint16_t)color.g, 0, 255)	, (u8)::gpk::clamp(b * (uint16_t)color.b, 0, 255)	, a};	}
		cnstxpr	SColorRGBA		operator +		(const SColorRGBA & color)	const	noexcept	{ return {(u8)::gpk::clamp(r + (uint16_t)color.r, 0, 255)	, (u8)::gpk::clamp(g + (uint16_t)color.g, 0, 255)	, (u8)::gpk::clamp(b + (uint16_t)color.b, 0, 255)	, a};	}
		cnstxpr	SColorRGBA		operator *		(double scalar)				const	noexcept	{ return {(u8)::gpk::clamp(r * scalar, 0.0 , 255.0)		, (u8)::gpk::clamp(g * scalar, 0.0 , 255.0 )		, (u8)::gpk::clamp(b * scalar, 0.0,  255.0 )		, a};	}
		cnstxpr	SColorRGBA		operator /		(double scalar)				const				{ return {(u8)::gpk::clamp(r / scalar, 0.0 , 255.0)		, (u8)::gpk::clamp(g / scalar, 0.0 , 255.0 )		, (u8)::gpk::clamp(b / scalar, 0.0,  255.0 )		, a};	}
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
		cnstxpr	operator		SColorRGBA		()							const	noexcept	{ return {r, g, b, a};																																												}
		cnstxpr	operator		uint32_t		()							const	noexcept	{ return (((uint32_t)a) << 24) | (((uint32_t)r) << 16) | (((uint32_t)g) << 8) | (((uint32_t)b) << 0);																								}

		cnstxpr	bool			operator==		(uint32_t other)			const	noexcept	{ return other == *((const uint32_t*)this);																																							}
		cnstxpr	SColorBGRA		operator*		(const SColorBGRA & color)	const	noexcept	{ return {(u8)::gpk::min(b * (uint16_t)color.b, 255)	, (u8)::gpk::min(g * (uint16_t)color.g, 255)	,(u8)::gpk::min(r * (uint16_t)color.r, 255), a};			}
		cnstxpr	SColorBGRA		operator+		(const SColorBGRA & color)	const	noexcept	{ return {(u8)::gpk::min(b + (uint16_t)color.b, 255)	, (u8)::gpk::min(g + (uint16_t)color.g, 255)	,(u8)::gpk::min(r + (uint16_t)color.r, 255), a};			}
		cnstxpr	SColorBGRA		operator-		(const SColorBGRA & color)	const	noexcept	{ return {(u8)::gpk::clamp(b - (int16_t)color.b, 0, 255), (u8)::gpk::clamp(g - (int16_t)color.g, 0, 255),(u8)::gpk::clamp(r - (int16_t)color.r, 0, 255), a};	}
		cnstxpr	SColorBGRA		operator*		(float scalar)				const	noexcept	{ return {(u8)::gpk::clamp(b * scalar, 0.0f, 255.0f)	, (u8)::gpk::clamp(g * scalar, 0.0f, 255.0f)	,(u8)::gpk::clamp(r * scalar, 0.0f, 255.0f), a};			}
		cnstxpr	SColorBGRA		operator/		(float scalar)				const				{ return {(u8)::gpk::clamp(b / scalar, 0.0f, 255.0f)	, (u8)::gpk::clamp(g / scalar, 0.0f, 255.0f)	,(u8)::gpk::clamp(r / scalar, 0.0f, 255.0f), a};			}
		cnstxpr	SColorBGRA		operator*		(double scalar)				const	noexcept	{ return {(u8)::gpk::clamp(b * scalar, 0.0,  255.0 )	, (u8)::gpk::clamp(g * scalar, 0.0 , 255.0 )	,(u8)::gpk::clamp(r * scalar, 0.0, 255.0),	a};				}
		cnstxpr	SColorBGRA		operator/		(double scalar)				const				{ return {(u8)::gpk::clamp(b / scalar, 0.0,  255.0 )	, (u8)::gpk::clamp(g / scalar, 0.0 , 255.0 )	,(u8)::gpk::clamp(r / scalar, 0.0, 255.0),	a};				}
				SColorBGRA&		operator*=		(float scalar)						noexcept	{ b = (u8)::gpk::clamp(b * scalar, 0.0f, 255.0f); g = (u8)::gpk::clamp(g * scalar, 0.0f, 255.0f); r = (u8)::gpk::clamp(r * scalar, 0.0f, 255.0f);	return *this; }
				SColorBGRA&		operator/=		(float scalar)									{ b = (u8)::gpk::clamp(b / scalar, 0.0f, 255.0f); g = (u8)::gpk::clamp(g / scalar, 0.0f, 255.0f); r = (u8)::gpk::clamp(r / scalar, 0.0f, 255.0f);	return *this; }
				SColorBGRA&		operator*=		(double scalar)						noexcept	{ b = (u8)::gpk::clamp(b * scalar, 0.0,  255.0 ); g = (u8)::gpk::clamp(g * scalar, 0.0 , 255.0 ); r = (u8)::gpk::clamp(r * scalar, 0.0, 255.0);		return *this; }
				SColorBGRA&		operator/=		(double scalar)									{ b = (u8)::gpk::clamp(b / scalar, 0.0,  255.0 ); g = (u8)::gpk::clamp(g / scalar, 0.0 , 255.0 ); r = (u8)::gpk::clamp(r / scalar, 0.0, 255.0);		return *this; }

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
		cnstxpr	SColorRGB		operator *		(float scalar)				const	noexcept	{ return {(u8)::gpk::clamp(r * scalar, 0.0f, 255.0f), (u8)::gpk::clamp(g * scalar, 0.0f, 255.0f),	(u8)::gpk::clamp(b * scalar, 0.0f, 255.0f)}; }
		cnstxpr	SColorRGB		operator /		(float scalar)				const				{ return {(u8)::gpk::clamp(r / scalar, 0.0f, 255.0f), (u8)::gpk::clamp(g / scalar, 0.0f, 255.0f),	(u8)::gpk::clamp(b / scalar, 0.0f, 255.0f)}; }
		cnstxpr	SColorRGB		operator *		(double scalar)				const	noexcept	{ return {(u8)::gpk::clamp(r * scalar, 0.0,  255.0),  (u8)::gpk::clamp(g * scalar, 0.0,  255.0),	(u8)::gpk::clamp(b * scalar, 0.0,  255.0)};  }
		cnstxpr	SColorRGB		operator /		(double scalar)				const				{ return {(u8)::gpk::clamp(r / scalar, 0.0,  255.0),  (u8)::gpk::clamp(g / scalar, 0.0,  255.0),	(u8)::gpk::clamp(b / scalar, 0.0,  255.0)};  }
		cnstxpr	SColorRGB		operator *		(const SColorRGB  & color)	const	noexcept	{ return {(u8)::gpk::clamp(r * (uint16_t)color.r, 0, 255), (u8)::gpk::clamp(g * (uint16_t)color.g, 0, 255), (u8)::gpk::clamp(b * (uint16_t)color.b, 0, 255)};			}
		cnstxpr	SColorRGB		operator +		(const SColorRGB  & color)	const	noexcept	{ return {(u8)::gpk::clamp(r + (uint16_t)color.r, 0, 255), (u8)::gpk::clamp(g + (uint16_t)color.g, 0, 255), (u8)::gpk::clamp(b + (uint16_t)color.b, 0, 255)};			}
		cnstxpr	SColorRGBA		operator *		(const SColorRGBA & color)	const	noexcept	{ return {(u8)::gpk::clamp(r * (uint16_t)color.r, 0, 255), (u8)::gpk::clamp(g * (uint16_t)color.g, 0, 255), (u8)::gpk::clamp(b * (uint16_t)color.b, 0, 255), color.a};	}
		cnstxpr	SColorRGBA		operator +		(const SColorRGBA & color)	const	noexcept	{ return {(u8)::gpk::clamp(r + (uint16_t)color.r, 0, 255), (u8)::gpk::clamp(g + (uint16_t)color.g, 0, 255), (u8)::gpk::clamp(b + (uint16_t)color.b, 0, 255), color.a};	}
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
		cnstxpr	operator		SColorBGRA		()							const	noexcept	{ return {b, g, r, 0xFF};																																	}
		cnstxpr	operator		SColor16		()							const	noexcept	{ return (((uint16_t)(b * BYTE_SCALE * 0x001F)) << 0) | (((uint16_t)(g * BYTE_SCALE * 0x003F)) << 5) | (((uint16_t)(r * BYTE_SCALE * 0x001F)) << 11 );	}

		cnstxpr	bool			operator ==		(const SColorBGRA & other)	const	noexcept	{ return r == other.r && g == other.g && b == other.b;	}
		cnstxpr	bool			operator !=		(const SColorBGRA & other)	const	noexcept	{ return r != other.r || g != other.g || b != other.b;	}
		cnstxpr	bool			operator ==		(const SColor16   & other)	const	noexcept	{ return (SColor16)(*this) == other;					}
		cnstxpr	bool			operator !=		(const SColor16   & other)	const	noexcept	{ return (SColor16)(*this) != other;					}
		cnstxpr	SColorBGR		operator *		(float scalar)				const	noexcept	{ return {(u8)::gpk::clamp(b * scalar, 0.0f, 255.0f), (u8)::gpk::clamp(g * scalar, 0.0f, 255.0f), (u8)::gpk::clamp(r * scalar, 0.0f, 255.0f)};	}
		cnstxpr	SColorBGR		operator /		(float scalar)				const				{ return {(u8)::gpk::clamp(b / scalar, 0.0f, 255.0f), (u8)::gpk::clamp(g / scalar, 0.0f, 255.0f), (u8)::gpk::clamp(r / scalar, 0.0f, 255.0f)};	}
		cnstxpr	SColorBGR		operator *		(double scalar)				const	noexcept	{ return {(u8)::gpk::clamp(b * scalar, 0.0,  255.0) , (u8)::gpk::clamp(g * scalar, 0.0,  255.0) , (u8)::gpk::clamp(r * scalar, 0.0,  255.0)};	}
		cnstxpr	SColorBGR		operator /		(double scalar)				const				{ return {(u8)::gpk::clamp(b / scalar, 0.0,  255.0) , (u8)::gpk::clamp(g / scalar, 0.0,  255.0) , (u8)::gpk::clamp(r / scalar, 0.0,  255.0)};	}
		cnstxpr	SColorBGR		operator *		(const SColorBGR  & color)	const	noexcept	{ return {(u8)::gpk::clamp(b * (uint16_t)color.b, 0, 255) , (u8)::gpk::clamp(g * (uint16_t)color.g, 0, 255)	, (u8)::gpk::clamp(r * (uint16_t)color.r, 0, 255)};			}
		cnstxpr	SColorBGR		operator +		(const SColorBGR  & color)	const	noexcept	{ return {(u8)::gpk::clamp(b + (uint16_t)color.b, 0, 255) , (u8)::gpk::clamp(g + (uint16_t)color.g, 0, 255)	, (u8)::gpk::clamp(r + (uint16_t)color.r, 0, 255)};			}
		cnstxpr	SColorBGRA		operator *		(const SColorBGRA & color)	const	noexcept	{ return {(u8)::gpk::clamp(b * (uint16_t)color.b, 0, 255) , (u8)::gpk::clamp(g * (uint16_t)color.g, 0, 255)	, (u8)::gpk::clamp(r * (uint16_t)color.r, 0, 255), color.a};	}
		cnstxpr	SColorBGRA		operator +		(const SColorBGRA & color)	const	noexcept	{ return {(u8)::gpk::clamp(b + (uint16_t)color.b, 0, 255) , (u8)::gpk::clamp(g + (uint16_t)color.g, 0, 255)	, (u8)::gpk::clamp(r + (uint16_t)color.r, 0, 255), color.a};	}

				SColorBGR&		FromBGR16		(const SColor16   & other)			noexcept	{ b = u8((other & 0x1F) / float(0x1F) * 255); g = u8(((other & 0x07E0) >> 5) / float(0x3F) * 255); r = u8(((other & 0xF800) >> 11) / float(0x1F) * 255);	return *this; }
				SColorBGR&		FromRGB16		(const SColor16   & other)			noexcept	{ r = u8((other & 0x1F) / float(0x1F) * 255); g = u8(((other & 0x07E0) >> 5) / float(0x3F) * 255); b = u8(((other & 0xF800) >> 11) / float(0x1F) * 255);	return *this; }
	};	// struct

	// Stores RGBA floating point color channels
	struct SColorFloat : public ::gpk::rgbaf32 {
								SColorFloat		()										noexcept	= default;
		cnstxpr					SColorFloat		(const SColorFloat & color)				noexcept	= default;
		cnstxpr					SColorFloat		(f32 r_, f32 g_, f32 b_, f32 a_=1.0f)	noexcept	: color_rgba<float>{r_, g_, b_, a_}																																										{}
		cnstxpr					SColorFloat		(const float* rgbaColor)							: color_rgba<float>{rgbaColor[0], rgbaColor[1], rgbaColor[2], rgbaColor[3]}	{}
		cnstxpr					SColorFloat		(SColorBGRA Color)						noexcept	: color_rgba<float>
			{ Color.r * BYTE_SCALE
			, Color.g * BYTE_SCALE
			, Color.b * BYTE_SCALE
			, Color.a * BYTE_SCALE
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

				SColorFloat&	Clamp			()									noexcept	{ r = ::gpk::clamp(r, 0.0f, 1.0f); g = ::gpk::clamp(g, 0.0f, 1.0f); b = ::gpk::clamp(b, 0.0f, 1.0f); return *this;	}
	};	// struct

	typedef ::gpk::SColorFloat					rgbaf	;
	typedef ::gpk::SColorBGRA					bgra	;
	typedef ::gpk::SColorBGR					bgr		;
	typedef ::gpk::SColorRGBA					rgba	;
	typedef ::gpk::SColorRGB					rgb		;

	typedef	::gpk::view<::gpk::rgbaf	>		v1rgbaf	;
	typedef	::gpk::view<::gpk::bgra		>		v1bgra	;
	typedef	::gpk::view<::gpk::bgr		>		v1bgr	;
	typedef	::gpk::view<::gpk::rgba		>		v1rgba	;
	typedef	::gpk::view<::gpk::rgb		>		v1rgb	;

	typedef	::gpk::view<const ::gpk::rgbaf	>	v1crgbaf;
	typedef	::gpk::view<const ::gpk::bgra	>	v1cbgra	;
	typedef	::gpk::view<const ::gpk::bgr	>	v1cbgr	;
	typedef	::gpk::view<const ::gpk::rgba	>	v1crgba	;
	typedef	::gpk::view<const ::gpk::rgb	>	v1crgb	;

	typedef	::gpk::view2d<::gpk::rgbaf	>		v2rgbaf	;
	typedef	::gpk::view2d<::gpk::bgra	>		v2bgra	;
	typedef	::gpk::view2d<::gpk::bgr	>		v2bgr	;
	typedef	::gpk::view2d<::gpk::rgba	>		v2rgba	;
	typedef	::gpk::view2d<::gpk::rgb	>		v2rgb	;

	typedef	::gpk::view2d<const ::gpk::rgbaf>	v2crgbaf;
	typedef	::gpk::view2d<const ::gpk::bgra	>	v2cbgra	;
	typedef	::gpk::view2d<const ::gpk::bgr	>	v2cbgr	;
	typedef	::gpk::view2d<const ::gpk::rgba	>	v2crgba	;
	typedef	::gpk::view2d<const ::gpk::rgb	>	v2crgb	;

	typedef	::gpk::apod<::gpk::rgbaf	>		aprgbaf	;
	typedef	::gpk::apod<::gpk::bgra		>		apbgra	;
	typedef	::gpk::apod<::gpk::bgr		>		apbgr	;
	typedef	::gpk::apod<::gpk::rgba		>		aprgba	;
	typedef	::gpk::apod<::gpk::rgb		>		aprgb	;

	typedef	::gpk::apod<const ::gpk::rgbaf	>	apcrgbaf;
	typedef	::gpk::apod<const ::gpk::bgra	>	apcbgra	;
	typedef	::gpk::apod<const ::gpk::bgr	>	apcbgr	;
	typedef	::gpk::apod<const ::gpk::rgba	>	apcrgba	;
	typedef	::gpk::apod<const ::gpk::rgb	>	apcrgb	;

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

	GDEFINE_ENUM_TYPE(COLOR_TYPE, u8);
	GDEFINE_ENUM_VALUE(COLOR_TYPE, GRAYSCALE		, 0); // Grayscale
	GDEFINE_ENUM_VALUE(COLOR_TYPE, RGB				, 2); // Red Green Blue 
	GDEFINE_ENUM_VALUE(COLOR_TYPE, PALETTE			, 3); // Palette
	GDEFINE_ENUM_VALUE(COLOR_TYPE, GRAYSCALE_ALPHA	, 4); // Grayscale Alpha 
	GDEFINE_ENUM_VALUE(COLOR_TYPE, RGBA				, 6); // Red Green Blue Alpha
	GDEFINE_ENUM_VALUE(COLOR_TYPE, BGR				, 7); // Blue Green Red
	GDEFINE_ENUM_VALUE(COLOR_TYPE, BGRA				, 8); // Blue Green Red Alpha
	GDEFINE_ENUM_VALUE(COLOR_TYPE, FILE				, 255); // Defined by file on load
#pragma pack(pop)
} // namespace

#endif // GPK_COLOR_H_29734982734
