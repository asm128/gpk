/// Copyright 2009-2017 - asm128
#include "gpk_math.h"
#include "gpk_enum.h"
#include "gpk_view_grid.h"

#ifndef GPK_COLOR_H_29734982734
#define GPK_COLOR_H_29734982734

namespace gpk
{
#pragma pack(push, 1)
	template <typename _tBase> struct color_wa		{ _tBase w, a;			};
	template <typename _tBase> struct color_bg		{ _tBase b, g;			};
	template <typename _tBase> struct color_br		{ _tBase b, r;			};
	template <typename _tBase> struct color_gr		{ _tBase g, r;			};
	template <typename _tBase> struct color_gb		{ _tBase g, b;			};
	template <typename _tBase> struct color_rg		{ _tBase r, g;			};
	template <typename _tBase> struct color_rb		{ _tBase r, b;			};
	template <typename _tBase> struct color_bgr		{ _tBase b, g, r;		};
	template <typename _tBase> struct color_rgb		{ _tBase r, g, b;		};
	template <typename _tBase> struct color_bgra	{ _tBase b, g, r, a;	};
	template <typename _tBase> struct color_rgba	{ _tBase r, g, b, a;	};
	template <typename _tBase> struct color_abgr	{ _tBase a, b, g, r;	};
	template <typename _tBase> struct color_argb	{ _tBase a, r, g, b;	};

	typedef	uint16_t		SColor16;

	// Stores RGBA color channels
	struct SColorRGBA : public ::gpk::color_rgba<uint8_t> {
		constexpr									SColorRGBA		(const SColorRGBA & other)											noexcept	= default;
		constexpr									SColorRGBA		(uint8_t r_ = 0, uint8_t g_ = 0, uint8_t b_ = 0, uint8_t a_=0xff)	noexcept	: color_rgba<uint8_t>{b_, g_, r_, a_}																																										{}
		constexpr									SColorRGBA		(uint32_t other)													noexcept	: color_rgba<uint8_t>
			{ ((uint8_t)(((other & 0x000000FF) >> 0)))
			, ((uint8_t)(((other & 0x0000FF00) >> 8)))
			, ((uint8_t)(((other & 0x00FF0000) >> 16)))
			, ((uint8_t)(((other & 0xFF000000) >> 24)))
			}
		{}

		constexpr					operator		uint32_t		()															const	noexcept	{ return (((uint32_t)a) << 24) | (((uint32_t)b) << 16) | (((uint32_t)g) << 8) | (((uint32_t)r) << 0);																								}
									SColorRGBA&		operator=		(const SColorRGBA& color)											noexcept	= default;

		constexpr					bool			operator ==		(uint32_t other)											const	noexcept	{ return other == *((const uint32_t*)this);																																							}
		constexpr					bool			operator ==		(const SColorRGBA& other)									const	noexcept	{ return b == other.b && g == other.g && r == other.r && a == other.a;																																}
		constexpr					bool			operator !=		(const SColorRGBA& other)									const	noexcept	{ return b != other.b || g != other.g || r != other.r || a != other.a;																																}
		constexpr					SColorRGBA		operator *		(const SColorRGBA& color)									const	noexcept	{ return SColorRGBA((uint8_t)::gpk::clamp(r * (uint16_t)color.r, 0, 255)	, (uint8_t)::gpk::clamp(g * (uint16_t)color.g, 0, 255)	, (uint8_t)::gpk::clamp(b * (uint16_t)color.b, 0, 255)	, a);	}
		constexpr					SColorRGBA		operator +		(const SColorRGBA& color)									const	noexcept	{ return SColorRGBA((uint8_t)::gpk::clamp(r + (uint16_t)color.r, 0, 255)	, (uint8_t)::gpk::clamp(g + (uint16_t)color.g, 0, 255)	, (uint8_t)::gpk::clamp(b + (uint16_t)color.b, 0, 255)	, a);	}
		constexpr					SColorRGBA		operator *		(double scalar)												const	noexcept	{ return SColorRGBA((uint8_t)::gpk::clamp(r * scalar, 0.0 , 255.0)			, (uint8_t)::gpk::clamp(g * scalar, 0.0 , 255.0 )		, (uint8_t)::gpk::clamp(b * scalar, 0.0,  255.0 )		, a);	}
		constexpr					SColorRGBA		operator /		(double scalar)												const				{ return SColorRGBA((uint8_t)::gpk::clamp(r / scalar, 0.0 , 255.0)			, (uint8_t)::gpk::clamp(g / scalar, 0.0 , 255.0 )		, (uint8_t)::gpk::clamp(b / scalar, 0.0,  255.0 )		, a);	}
	};	// struct

	// Stores BGRA color channels
	struct SColorBGRA {
									uint8_t			b = 0, g = 0, r = 0, a = 0xff;
									//uint8_t			b, g, r, a;

		constexpr									SColorBGRA		()																noexcept	= default;
		constexpr									SColorBGRA		(const SColorRGBA& other)										noexcept	: b(other.b), g(other.g), r(other.r), a(other.a)																																										{}
		constexpr									SColorBGRA		(const SColorBGRA& other)										noexcept	= default;
		constexpr									SColorBGRA		(uint8_t b_, uint8_t g_, uint8_t r_, uint8_t a_=0xff)			noexcept	: b(b_), g(g_), r(r_), a(a_)																																										{}
		constexpr									SColorBGRA		(uint32_t other)												noexcept	: b((uint8_t)(((other & 0x000000FF) >> 0))), g((uint8_t)(((other & 0x0000FF00) >> 8))), r((uint8_t)(((other & 0x00FF0000) >> 16))), a((uint8_t)(((other & 0xFF000000) >> 24)))						{}

		constexpr					operator		SColorRGBA		()														const	noexcept	{ return {r, g, b, a};																																												}
		constexpr					operator		uint32_t		()														const	noexcept	{ return (((uint32_t)a) << 24) | (((uint32_t)r) << 16) | (((uint32_t)g) << 8) | (((uint32_t)b) << 0);																								}
									SColorBGRA&		operator=		(const SColorBGRA& color)										noexcept	= default;
		constexpr					bool			operator==		(uint32_t other)										const	noexcept	{ return other == *((const uint32_t*)this);																																							}
		constexpr					bool			operator==		(const SColorBGRA& other)								const	noexcept	{ return b == other.b && g == other.g && r == other.r && a == other.a;																																}
		constexpr					bool			operator!=		(const SColorBGRA& other)								const	noexcept	{ return b != other.b || g != other.g || r != other.r || a != other.a;																																}
		constexpr					SColorBGRA		operator*		(const SColorBGRA& color)								const	noexcept	{ return SColorBGRA{(uint8_t)::gpk::min(b * (uint16_t)color.b, 255)	, (uint8_t)::gpk::min(g * (uint16_t)color.g, 255)	,(uint8_t)::gpk::min(r * (uint16_t)color.r, 255), a};			}
		constexpr					SColorBGRA		operator+		(const SColorBGRA& color)								const	noexcept	{ return SColorBGRA{(uint8_t)::gpk::min(b + (uint16_t)color.b, 255)	, (uint8_t)::gpk::min(g + (uint16_t)color.g, 255)	,(uint8_t)::gpk::min(r + (uint16_t)color.r, 255), a};			}
		constexpr					SColorBGRA		operator-		(const SColorBGRA& color)								const	noexcept	{ return SColorBGRA{(uint8_t)::gpk::clamp(b - (int16_t)color.b, 0, 255), (uint8_t)::gpk::clamp(g - (int16_t)color.g, 0, 255),(uint8_t)::gpk::clamp(r - (int16_t)color.r, 0, 255), a};	}
		constexpr					SColorBGRA		operator*		(float scalar)											const	noexcept	{ return SColorBGRA{(uint8_t)::gpk::clamp(b * scalar, 0.0f, 255.0f)	, (uint8_t)::gpk::clamp(g * scalar, 0.0f, 255.0f)	,(uint8_t)::gpk::clamp(r * scalar, 0.0f, 255.0f), a};			}
		constexpr					SColorBGRA		operator/		(float scalar)											const				{ return SColorBGRA{(uint8_t)::gpk::clamp(b / scalar, 0.0f, 255.0f)	, (uint8_t)::gpk::clamp(g / scalar, 0.0f, 255.0f)	,(uint8_t)::gpk::clamp(r / scalar, 0.0f, 255.0f), a};			}
		constexpr					SColorBGRA		operator*		(double scalar)											const	noexcept	{ return SColorBGRA{(uint8_t)::gpk::clamp(b * scalar, 0.0,  255.0 )	, (uint8_t)::gpk::clamp(g * scalar, 0.0 , 255.0 )	,(uint8_t)::gpk::clamp(r * scalar, 0.0, 255.0),	a};				}
		constexpr					SColorBGRA		operator/		(double scalar)											const				{ return SColorBGRA{(uint8_t)::gpk::clamp(b / scalar, 0.0,  255.0 )	, (uint8_t)::gpk::clamp(g / scalar, 0.0 , 255.0 )	,(uint8_t)::gpk::clamp(r / scalar, 0.0, 255.0),	a};				}
									SColorBGRA&		operator*=		(float scalar)													noexcept	{ b = (uint8_t)::gpk::clamp(b * scalar, 0.0f, 255.0f); g = (uint8_t)::gpk::clamp(g * scalar, 0.0f, 255.0f); r = (uint8_t)::gpk::clamp(r * scalar, 0.0f, 255.0f);	return *this; }
									SColorBGRA&		operator/=		(float scalar)																{ b = (uint8_t)::gpk::clamp(b / scalar, 0.0f, 255.0f); g = (uint8_t)::gpk::clamp(g / scalar, 0.0f, 255.0f); r = (uint8_t)::gpk::clamp(r / scalar, 0.0f, 255.0f);	return *this; }
									SColorBGRA&		operator*=		(double scalar)													noexcept	{ b = (uint8_t)::gpk::clamp(b * scalar, 0.0,  255.0 ); g = (uint8_t)::gpk::clamp(g * scalar, 0.0 , 255.0 ); r = (uint8_t)::gpk::clamp(r * scalar, 0.0, 255.0);		return *this; }
									SColorBGRA&		operator/=		(double scalar)																{ b = (uint8_t)::gpk::clamp(b / scalar, 0.0,  255.0 ); g = (uint8_t)::gpk::clamp(g / scalar, 0.0 , 255.0 ); r = (uint8_t)::gpk::clamp(r / scalar, 0.0, 255.0);		return *this; }

									SColorBGRA&		FromBGR16		(const SColor16& other)											noexcept	{ b = uint8_t((other & 0x1F) / float(0x1F) * 255); g = uint8_t(((other & 0x07E0) >> 5) / float(0x3F) * 255); r = uint8_t(((other & 0xF800) >> 11) / float(0x1F) * 255);	return *this; }
									SColorBGRA&		FromRGB16		(const SColor16& other)											noexcept	{ r = uint8_t((other & 0x1F) / float(0x1F) * 255); g = uint8_t(((other & 0x07E0) >> 5) / float(0x3F) * 255); b = uint8_t(((other & 0xF800) >> 11) / float(0x1F) * 255);	return *this; }
	};	// struct

	// Stores BGR color channels
	struct SColorBGR {
									uint8_t							b = 0, g = 0, r = 0;

		constexpr					SColorBGR						()																noexcept	= default;
		constexpr					SColorBGR						(const SColorBGR& otherColorInt)								noexcept	= default;
		constexpr					SColorBGR						(uint8_t b_, uint8_t g_, uint8_t r_)							noexcept	: b(b_), g(g_), r(r_)																																														{}
		constexpr					SColorBGR						(const SColorBGRA& other)										noexcept	: b(other.b), g(other.b), r(other.r)																																										{}
		//constexpr					SColorBGR						(const SColor16& other)											noexcept	: b((uint8_t)((other & 0x001F) / ((float)(0x1F))*255))	, g((uint8_t)(((other & 0x07E0) >>5) / ((float)(0x3F))*255)), r((uint8_t)(((other & 0xF800) >>11) / ((float)(0x1F))*255))							{}
		constexpr					SColorBGR						(uint32_t other)												noexcept	: b((uint8_t)(((other & 0x000000FF) >> 0)))				, g((uint8_t)((other & 0x0000FF00) >> 8))					, r((uint8_t)(((other & 0x00FF0000) >> 16)))											{}

		constexpr					operator		uint32_t		()														const	noexcept	{ return				0xFF000000 | (((uint32_t)r) << 16)	| (((uint32_t)g) << 8)							| (((uint32_t)b) << 0);																			}
		constexpr					operator		SColorBGRA		()														const	noexcept	{ return {b, g, r, 0xFF};																																													}
		constexpr					operator		SColor16		()														const	noexcept	{ return (((uint16_t)(b * (1/255.0f) * 0x001F)) << 0)	| (((uint16_t)(g * (1/255.0f) * 0x003F)) << 5)	| (((uint16_t)(r * (1/255.0f) * 0x001F)) << 11 );													}
									SColorBGR&		operator=		(const SColorBGR& color)										noexcept	= default;
		constexpr					bool			operator ==		(const SColorBGRA& other)								const	noexcept	{ return r == other.r && g == other.g && b == other.b;																																						}
		constexpr					bool			operator ==		(const SColorBGR& other)								const	noexcept	{ return r == other.r && g == other.g && b == other.b;																																						}
		constexpr					bool			operator ==		(const SColor16& other)									const	noexcept	{ return operator == (SColorBGR(other));																																									}
		constexpr					bool			operator !=		(const SColorBGRA& other)								const	noexcept	{ return r != other.r || g != other.g || b != other.b;																																						}
		constexpr					bool			operator !=		(const SColorBGR& other)								const	noexcept	{ return r != other.r || g != other.g || b != other.b;																																						}
		constexpr					bool			operator !=		(const SColor16& other)									const	noexcept	{ return operator != (SColorBGR(other));																																									}
		constexpr					SColorBGR		operator *		(float scalar)											const	noexcept	{ return ::gpk::SColorBGR {(uint8_t)::gpk::clamp(b * scalar, 0.0f, 255.0f)		, (uint8_t)::gpk::clamp(g * scalar, 0.0f, 255.0f)		, (uint8_t)::gpk::clamp(r * scalar, 0.0f, 255.0f)};					}
		constexpr					SColorBGR		operator /		(float scalar)											const				{ return ::gpk::SColorBGR {(uint8_t)::gpk::clamp(b / scalar, 0.0f, 255.0f)		, (uint8_t)::gpk::clamp(g / scalar, 0.0f, 255.0f)		, (uint8_t)::gpk::clamp(r / scalar, 0.0f, 255.0f)};					}
		constexpr					SColorBGR		operator *		(double scalar)											const	noexcept	{ return ::gpk::SColorBGR {(uint8_t)::gpk::clamp(b * scalar, 0.0,  255.0)		, (uint8_t)::gpk::clamp(g * scalar, 0.0,  255.0)		, (uint8_t)::gpk::clamp(r * scalar, 0.0,  255.0)};					}
		constexpr					SColorBGR		operator /		(double scalar)											const				{ return ::gpk::SColorBGR {(uint8_t)::gpk::clamp(b / scalar, 0.0,  255.0)		, (uint8_t)::gpk::clamp(g / scalar, 0.0,  255.0)		, (uint8_t)::gpk::clamp(r / scalar, 0.0,  255.0)};					}
		constexpr					SColorBGR		operator *		(const SColorBGR& color)								const	noexcept	{ return ::gpk::SColorBGR {(uint8_t)::gpk::clamp(b * (uint16_t)color.b, 0, 255) , (uint8_t)::gpk::clamp(g * (uint16_t)color.g, 0, 255)	, (uint8_t)::gpk::clamp(r * (uint16_t)color.r, 0, 255)};			}
		constexpr					SColorBGR		operator +		(const SColorBGR& color)								const	noexcept	{ return ::gpk::SColorBGR {(uint8_t)::gpk::clamp(b + (uint16_t)color.b, 0, 255) , (uint8_t)::gpk::clamp(g + (uint16_t)color.g, 0, 255)	, (uint8_t)::gpk::clamp(r + (uint16_t)color.r, 0, 255)};			}
		constexpr					SColorBGRA		operator *		(const SColorBGRA& color)								const	noexcept	{ return ::gpk::SColorBGRA{(uint8_t)::gpk::clamp(b * (uint16_t)color.b, 0, 255) , (uint8_t)::gpk::clamp(g * (uint16_t)color.g, 0, 255)	, (uint8_t)::gpk::clamp(r * (uint16_t)color.r, 0, 255), color.a};	}
		constexpr					SColorBGRA		operator +		(const SColorBGRA& color)								const	noexcept	{ return ::gpk::SColorBGRA{(uint8_t)::gpk::clamp(b + (uint16_t)color.b, 0, 255) , (uint8_t)::gpk::clamp(g + (uint16_t)color.g, 0, 255)	, (uint8_t)::gpk::clamp(r + (uint16_t)color.r, 0, 255), color.a};	}

									SColorBGR&		FromBGR16		(const SColor16& other)											noexcept	{ b = uint8_t((other & 0x1F) / float(0x1F) * 255); g = uint8_t(((other & 0x07E0) >> 5) / float(0x3F) * 255); r = uint8_t(((other & 0xF800) >> 11) / float(0x1F) * 255);	return *this; }
									SColorBGR&		FromRGB16		(const SColor16& other)											noexcept	{ r = uint8_t((other & 0x1F) / float(0x1F) * 255); g = uint8_t(((other & 0x07E0) >> 5) / float(0x3F) * 255); b = uint8_t(((other & 0xF800) >> 11) / float(0x1F) * 255);	return *this; }
	};	// struct

	// Stores RGB color channels
	struct SColorRGB {
									uint8_t							r = 0, g = 0, b = 0;

		constexpr					SColorRGB						()																noexcept	= default;
		constexpr					SColorRGB						(const SColorRGB& otherColorInt)								noexcept	= default;
		constexpr					SColorRGB						(uint8_t b_, uint8_t g_, uint8_t r_)							noexcept	: r(r_), g(g_), b(b_)																																													{}
		constexpr					SColorRGB						(const SColorRGBA& other)										noexcept	: r(other.r), g(other.g), b(other.b)																																									{}
		//constexpr					SColorRGB						(const SColor16& other)											noexcept	: r((uint8_t)((other & 0x001F) / ((float)(0x1F))*255))	, g((uint8_t)(((other & 0x07E0) >>5) / ((float)(0x3F))*255)), b((uint8_t)(((other & 0xF800) >>11) / ((float)(0x1F))*255))						{}
		constexpr					SColorRGB						(uint32_t other)												noexcept	: r((uint8_t)(((other & 0x000000FF) >> 0)))				, g((uint8_t)((other & 0x0000FF00) >> 8))					, b((uint8_t)(((other & 0x00FF0000) >> 16)))										{}

		constexpr					operator		uint32_t		()														const	noexcept	{ return				0xFF000000 | (((uint32_t)b) << 16)	| (((uint32_t)g) << 8)							| (((uint32_t)r) << 0);																			}
		constexpr					operator		SColorRGBA		()														const	noexcept	{ return SColorRGBA(	0xFF000000 | (((uint32_t)b) << 16)	| (((uint32_t)g) << 8)							| (((uint32_t)r) << 0));																	}
		constexpr					operator		SColor16		()														const	noexcept	{ return (((uint16_t)(r * (1/255.0f) * 0x001F)) << 0)	| (((uint16_t)(g * (1/255.0f) * 0x003F)) << 5)	| (((uint16_t)(b * (1/255.0f) * 0x001F)) << 11 );												}
									SColorRGB&		operator=		(const SColorRGB& color)										noexcept	= default;
		constexpr					bool			operator ==		(const SColorRGBA& other)								const	noexcept	{ return r == other.r && g == other.g && b == other.b;																																					}
		constexpr					bool			operator ==		(const SColorRGB& other)								const	noexcept	{ return r == other.r && g == other.g && b == other.b;																																					}
		constexpr					bool			operator ==		(const SColor16& other)									const	noexcept	{ return operator == (SColorRGB(other));																																								}
		constexpr					bool			operator !=		(const SColorRGBA& other)								const	noexcept	{ return r != other.r || g != other.g || b != other.b;																																					}
		constexpr					bool			operator !=		(const SColorRGB& other)								const	noexcept	{ return r != other.r || g != other.g || b != other.b;																																					}
		constexpr					bool			operator !=		(const SColor16& other)									const	noexcept	{ return operator != (SColorRGB(other));																																								}
		constexpr					SColorRGB		operator *		(float scalar)											const	noexcept	{ return ::gpk::SColorRGB((uint8_t)::gpk::clamp(r * scalar, 0.0f, 255.0f),	(uint8_t)::gpk::clamp(g * scalar, 0.0f, 255.0f),	(uint8_t)::gpk::clamp(b * scalar, 0.0f, 255.0f));						}
		constexpr					SColorRGB		operator /		(float scalar)											const				{ return ::gpk::SColorRGB((uint8_t)::gpk::clamp(r / scalar, 0.0f, 255.0f),	(uint8_t)::gpk::clamp(g / scalar, 0.0f, 255.0f),	(uint8_t)::gpk::clamp(b / scalar, 0.0f, 255.0f));						}
		constexpr					SColorRGB		operator *		(double scalar)											const	noexcept	{ return ::gpk::SColorRGB((uint8_t)::gpk::clamp(r * scalar, 0.0,  255.0), 	(uint8_t)::gpk::clamp(g * scalar, 0.0,  255.0),	(uint8_t)::gpk::clamp(b * scalar, 0.0,  255.0));							}
		constexpr					SColorRGB		operator /		(double scalar)											const				{ return ::gpk::SColorRGB((uint8_t)::gpk::clamp(r / scalar, 0.0,  255.0), 	(uint8_t)::gpk::clamp(g / scalar, 0.0,  255.0),	(uint8_t)::gpk::clamp(b / scalar, 0.0,  255.0));							}
		constexpr					SColorRGB		operator *		(const SColorRGB& color)								const	noexcept	{ return ::gpk::SColorRGB((uint8_t)::gpk::clamp(r * (uint16_t)color.r, 0, 255), (uint8_t)::gpk::clamp(g * (uint16_t)color.g, 0, 255), (uint8_t)::gpk::clamp(b * (uint16_t)color.b, 0, 255));			}
		constexpr					SColorRGB		operator +		(const SColorRGB& color)								const	noexcept	{ return ::gpk::SColorRGB((uint8_t)::gpk::clamp(r + (uint16_t)color.r, 0, 255), (uint8_t)::gpk::clamp(g + (uint16_t)color.g, 0, 255), (uint8_t)::gpk::clamp(b + (uint16_t)color.b, 0, 255));			}
		constexpr					SColorRGBA		operator *		(const SColorRGBA& color)								const	noexcept	{ return ::gpk::SColorRGBA((uint8_t)::gpk::clamp(r * (uint16_t)color.r, 0, 255), (uint8_t)::gpk::clamp(g * (uint16_t)color.g, 0, 255), (uint8_t)::gpk::clamp(b * (uint16_t)color.b, 0, 255), color.a);	}
		constexpr					SColorRGBA		operator +		(const SColorRGBA& color)								const	noexcept	{ return ::gpk::SColorRGBA((uint8_t)::gpk::clamp(r + (uint16_t)color.r, 0, 255), (uint8_t)::gpk::clamp(g + (uint16_t)color.g, 0, 255), (uint8_t)::gpk::clamp(b + (uint16_t)color.b, 0, 255), color.a);	}
	};	// struct

	// Stores RGBA floating point color channels
	struct SColorFloat {
									float			r = 0, g = 0, b = 0, a = 0;		// store the color values as floating point ranged in the values (0,1)

		constexpr									SColorFloat		()																noexcept	= default;
		constexpr									SColorFloat		(const SColorFloat& color)										noexcept	= default;
		constexpr									SColorFloat		(float _r, float _g, float _b, float _a=1.0f)					noexcept	: r(_r), g(_g), b(_b), a(_a) {} // r(::gpk::clamp(_r, 0.0f, 1.0f)) ,g(::gpk::clamp(_g, 0.0f, 1.0f)) ,b(::gpk::clamp(_b, 0.0f, 1.0f)) ,a(::gpk::clamp(_a, 0.0f, 1.0f))		{}
		//constexpr									SColorFloat		(uint16_t Color)												noexcept	: SColorFloat(SColorBGR(Color))																																				{}
		//constexpr									SColorFloat		(uint32_t Color)												noexcept	: r(((Color & 0x00FF0000L)>>16)	*(1/255.0f)), g(((Color & 0x0000FF00L)>>8)*(1/255.0f)), b(((Color & 0x000000FFL)>>0)*(1/255.0f)), a(((Color & 0xFF000000L)>>24)*(1/255.0f))	{}
		constexpr									SColorFloat		(SColorBGRA Color)												noexcept	: r(Color.r * (1/255.0f)), g(Color.g * (1/255.0f)), b(Color.b * (1/255.0f)), a(Color.a * (1/255.0f))																		{}
		//constexpr									SColorFloat		(SColorBGR Color)												noexcept	: r(Color.r	* (1/255.0f)), g(Color.g * (1/255.0f)), b(Color.b * (1/255.0f)), a(1.0f)																						{}
		constexpr									SColorFloat		(const float* rgbaColor)													: r(rgbaColor[0]), g(rgbaColor[1]), b(rgbaColor[2]), a(rgbaColor[3])																										{}

									SColorFloat&	operator=		(const SColorFloat& color)													= default;
		//constexpr					operator		uint32_t		()														const	noexcept	{ return (((uint32_t)(a * 255.0f)) << 24) | (((uint32_t)(r * 255.0f)) << 16) | (((uint32_t)(g * 255.0f)) << 8) | ((uint32_t)(b*255.0f));									}
		constexpr					operator		SColorBGRA		()														const	noexcept	{ return {(uint8_t)(b * 255), (uint8_t)(g * 255), (uint8_t)(r * 255), (uint8_t)(a * 255)};																					}
		//constexpr					operator		SColorBGR		()														const	noexcept	{ return SColorBGR ((uint8_t)(b * 255), (uint8_t)(g * 255), (uint8_t)(r * 255));																							}
		//constexpr					operator		SColor16		()														const	noexcept	{ return ((((uint16_t)(r * 0x1F)) << 11) | (((uint16_t)(g * 0x3F)) << 5) | (((uint16_t)(b * 0x1F)) << 0));																	}
		constexpr					operator		const float*	()														const	noexcept	{ return &r;																																								}
		constexpr					bool			operator ==		(const SColorFloat& color)								const	noexcept	{ return SColorBGR((uint8_t)(color.b * 255), (uint8_t)(color.g * 255), (uint8_t)(color.r * 255)) == SColorBGR((uint8_t)(b * 255), (uint8_t)(g * 255), (uint8_t)(r * 255));	}
		constexpr					bool			operator !=		(const SColorFloat& color)								const	noexcept	{ return SColorBGR((uint8_t)(color.b * 255), (uint8_t)(color.g * 255), (uint8_t)(color.r * 255)) != SColorBGR((uint8_t)(b * 255), (uint8_t)(g * 255), (uint8_t)(r * 255));	}
									SColorFloat&	operator +=		(const SColorFloat& color)										noexcept	{ r = r + color.r;					g = g + color.g;				b = b + color.b;				return *this;	}
									SColorFloat&	operator *=		(const SColorFloat& color)										noexcept	{ r = r * color.r;					g = g * color.g;				b = b * color.b;				return *this;	}
									SColorFloat&	operator *=		(double scalar)													noexcept	{ r = (float)(r * scalar);			g = (float)(g * scalar);		b = (float)(b * scalar);		return *this;	}
									SColorFloat&	operator /=		(double scalar)																{ r = (float)(r / scalar);			g = (float)(g / scalar);		b = (float)(b / scalar);		return *this;	}
									SColorFloat&	operator *=		(float scalar)													noexcept	{ r = r * scalar;					g = g * scalar;					b = b * scalar;					return *this;	}
									SColorFloat&	operator /=		(float scalar)																{ r = r / scalar;					g = g / scalar;					b = b / scalar;					return *this;	}
									SColorFloat&	operator *=		(const SColorBGRA& color)										noexcept	{ r = r * (color.r * (1/255.0f));	g = g * (color.g * (1/255.0f));	b = b * (color.b * (1/255.0f));	return *this;	}
									SColorFloat&	operator +=		(const SColorBGRA& color)										noexcept	{ r = r + (color.r * (1/255.0f));	g = g + (color.g * (1/255.0f));	b = b + (color.b * (1/255.0f));	return *this;	}
									SColorFloat&	operator *=		(const SColorBGR& color)										noexcept	{ r = r * (color.r * (1/255.0f));	g = g * (color.g * (1/255.0f));	b = b * (color.b * (1/255.0f));	return *this;	}
									SColorFloat&	operator +=		(const SColorBGR& color)										noexcept	{ r = r + (color.r * (1/255.0f));	g = g + (color.g * (1/255.0f));	b = b + (color.b * (1/255.0f));	return *this;	}
									SColorFloat		operator *		(const SColorBGRA& color)								const	noexcept	{ return ::gpk::SColorFloat{r * (color.r * (1/255.0f)), g * (color.g * (1/255.0f)), b * (color.b * (1/255.0f)), a};	}
									SColorFloat		operator +		(const SColorBGRA& color)								const	noexcept	{ return ::gpk::SColorFloat{r + (color.r * (1/255.0f)), g + (color.g * (1/255.0f)), b + (color.b * (1/255.0f)), a};	}
									SColorFloat		operator *		(const SColorBGR& color)								const	noexcept	{ return ::gpk::SColorFloat{r * (color.r * (1/255.0f)), g * (color.g * (1/255.0f)), b * (color.b * (1/255.0f)), a};	}
									SColorFloat		operator +		(const SColorBGR& color)								const	noexcept	{ return ::gpk::SColorFloat{r + (color.r * (1/255.0f)), g + (color.g * (1/255.0f)), b + (color.b * (1/255.0f)), a};	}
		constexpr					SColorFloat		operator *		(const SColorFloat& color)								const	noexcept	{ return ::gpk::SColorFloat{r * color.r, g * color.g, b * color.b, a};												}
		constexpr					SColorFloat		operator +		(const SColorFloat& color)								const	noexcept	{ return ::gpk::SColorFloat{r + color.r, g + color.g, b + color.b, a};												}
		constexpr					SColorFloat		operator -		(const SColorFloat& color)								const	noexcept	{ return ::gpk::SColorFloat(r - color.r, g - color.g, b - color.b, a);												}
		constexpr					SColorFloat		operator *		(double scalar)											const	noexcept	{ return ::gpk::SColorFloat{(float)(r * scalar), (float)(g * scalar), (float)(b * scalar), a};						}
		constexpr					SColorFloat		operator *		(float scalar)											const	noexcept	{ return ::gpk::SColorFloat{(r * scalar), (g * scalar),	(b * scalar)};												}
		constexpr					SColorFloat		operator /		(double scalar)											const				{ return ::gpk::SColorFloat{(float)(r / scalar), (float)(g / scalar), (float)(b / scalar), a};						}
		constexpr					SColorFloat		operator /		(float scalar)											const				{ return ::gpk::SColorFloat(r / scalar,	g / scalar,	b / scalar);													}

									SColorFloat&	Clamp			()																noexcept	{ r = ::gpk::clamp(r, 0.0f, 1.0f); g = ::gpk::clamp(g, 0.0f, 1.0f); b = ::gpk::clamp(b, 0.0f, 1.0f); return *this;	}
	};	// struct

	typedef ::gpk::SColorFloat							rgbaf	;
	typedef ::gpk::SColorBGRA							bgra	;
	typedef ::gpk::SColorBGR							bgr		;
	typedef ::gpk::SColorRGBA							rgba	;
	typedef ::gpk::SColorRGB							rgb		;

	typedef	::gpk::view<::gpk::rgbaf	>				v1rgbaf	;
	typedef	::gpk::view<::gpk::bgra		>				v1bgra	;
	typedef	::gpk::view<::gpk::bgr		>				v1bgr	;
	typedef	::gpk::view<::gpk::rgba		>				v1rgba	;
	typedef	::gpk::view<::gpk::rgb		>				v1rgb	;

	typedef	::gpk::view<const	::gpk::rgbaf	>		v1crgbaf;
	typedef	::gpk::view<const	::gpk::bgra		>		v1cbgra	;
	typedef	::gpk::view<const	::gpk::bgr		>		v1cbgr	;
	typedef	::gpk::view<const	::gpk::rgba		>		v1crgba	;
	typedef	::gpk::view<const	::gpk::rgb		>		v1crgb	;

	typedef	::gpk::view2d<::gpk::rgbaf	>				v2rgbaf	;
	typedef	::gpk::view2d<::gpk::bgra	>				v2bgra	;
	typedef	::gpk::view2d<::gpk::bgr	>				v2bgr	;
	typedef	::gpk::view2d<::gpk::rgba	>				v2rgba	;
	typedef	::gpk::view2d<::gpk::rgb	>				v2rgb	;

	typedef	::gpk::view2d<const	::gpk::rgbaf	>		v2crgbaf;
	typedef	::gpk::view2d<const	::gpk::bgra		>		v2cbgra	;
	typedef	::gpk::view2d<const	::gpk::bgr		>		v2cbgr	;
	typedef	::gpk::view2d<const	::gpk::rgba		>		v2crgba	;
	typedef	::gpk::view2d<const	::gpk::rgb		>		v2crgb	;

	typedef	::gpk::apod<::gpk::rgbaf	>				aprgbaf	;
	typedef	::gpk::apod<::gpk::bgra		>				apbgra	;
	typedef	::gpk::apod<::gpk::bgr		>				apbgr	;
	typedef	::gpk::apod<::gpk::rgba		>				aprgba	;
	typedef	::gpk::apod<::gpk::rgb		>				aprgb	;

	typedef	::gpk::apod<const ::gpk::rgbaf	>			apcrgbaf;
	typedef	::gpk::apod<const ::gpk::bgra	>			apcbgra	;
	typedef	::gpk::apod<const ::gpk::bgr	>			apcbgr	;
	typedef	::gpk::apod<const ::gpk::rgba	>			apcrgba	;
	typedef	::gpk::apod<const ::gpk::rgb	>			apcrgb	;

	static	constexpr	::gpk::rgbaf	BLACK			= ::gpk::rgbaf(0.0f, 0.0f, 0.0f, 1.0f					);
	static	constexpr	::gpk::rgbaf	WHITE			= ::gpk::rgbaf(1.0f, 1.0f, 1.0f, 1.0f					);
	static	constexpr	::gpk::rgbaf	RED				= ::gpk::rgbaf(1.0f, 0.0f, 0.0f, 1.0f					);
	static	constexpr	::gpk::rgbaf	GREEN			= ::gpk::rgbaf(0.0f, 1.0f, 0.0f, 1.0f					);
	static	constexpr	::gpk::rgbaf	BLUE			= ::gpk::rgbaf(0.0f, 0.0f, 1.0f, 1.0f					);
	static	constexpr	::gpk::rgbaf	YELLOW			= ::gpk::rgbaf(1.0f, 1.0f, 0.0f, 1.0f					);
	static	constexpr	::gpk::rgbaf	MAGENTA			= ::gpk::rgbaf(1.0f, 0.0f, 1.0f, 1.0f					);
	static	constexpr	::gpk::rgbaf	CYAN			= ::gpk::rgbaf(0.0f, 1.0f, 1.0f, 1.0f					);
	static	constexpr	::gpk::rgbaf	ORANGE			= ::gpk::rgbaf(1.0f, 0.647f, 0.0f, 1.0f					);
	static	constexpr	::gpk::rgbaf	PANOCHE			= ::gpk::rgbaf(0.25f, 0.5f, 1.0f, 1.0f					);
	static	constexpr	::gpk::rgbaf	PURPLE			= ::gpk::rgbaf(0.5f, 0.1f, 1.0f, 1.0f					);
	static	constexpr	::gpk::rgbaf	TURQUOISE		= ::gpk::rgbaf(0.2f, 1.0, 0.65f, 1.0f					);
	static	constexpr	::gpk::rgbaf	BROWN			= ::gpk::rgbaf(0.4f, 0.223f, 0.0f, 1.0f					);
	static	constexpr	::gpk::rgbaf	GRAY			= ::gpk::rgbaf(0.5f, 0.5f, 0.5f, 1.0f					);
	static	constexpr	::gpk::rgbaf	DARKGRAY		= ::gpk::rgbaf(0.25f, 0.25f, 0.25f, 1.0f				);
	static	constexpr	::gpk::rgbaf	DARKRED			= ::gpk::rgbaf(0.5f, 0.0f, 0.0f, 1.0f					);
	static	constexpr	::gpk::rgbaf	DARKGREEN		= ::gpk::rgbaf(0.0f, 0.5f, 0.0f, 1.0f					);
	static	constexpr	::gpk::rgbaf	DARKBLUE		= ::gpk::rgbaf(0.0f, 0.0f, 0.5f, 1.0f					);
	static	constexpr	::gpk::rgbaf	DARKYELLOW		= ::gpk::rgbaf(0.5f, 0.5f, 0.0f, 1.0f					);
	static	constexpr	::gpk::rgbaf	DARKMAGENTA		= ::gpk::rgbaf(0.5f, 0.0f, 0.5f, 1.0f					);
	static	constexpr	::gpk::rgbaf	DARKCYAN		= ::gpk::rgbaf(0.0f, 0.5f, 0.5f, 1.0f					);
	static	constexpr	::gpk::rgbaf	DARKORANGE		= ::gpk::rgbaf(1.0f, 0.5490196078431373f, 0.0f, 1.0f	);
	static	constexpr	::gpk::rgbaf	LIGHTGRAY		= ::gpk::rgbaf(0.75f, 0.75f, 0.75f, 1.0f				);
	static	constexpr	::gpk::rgbaf	LIGHTRED		= ::gpk::rgbaf(1.0f, 0.25f, 0.25f, 1.0f					);
	static	constexpr	::gpk::rgbaf	LIGHTGREEN		= ::gpk::rgbaf(0.5f,  1.0f, 0.5f, 1.0f					);
	static	constexpr	::gpk::rgbaf	LIGHTBLUE		= ::gpk::rgbaf(0.25f, 0.25f, 1.0f, 1.0f					);
	static	constexpr	::gpk::rgbaf	LIGHTYELLOW		= ::gpk::rgbaf(1.0f, 1.0f, 0.25f, 1.0f					);
	static	constexpr	::gpk::rgbaf	LIGHTMAGENTA	= ::gpk::rgbaf(1.0f, 0.25f, 1.0f, 1.0f					);
	static	constexpr	::gpk::rgbaf	LIGHTCYAN		= ::gpk::rgbaf(0.25f, 1.0f, 1.0f, 1.0f					);
	static	constexpr	::gpk::rgbaf	LIGHTORANGE		= ::gpk::rgbaf(1.0f, 0.780f, 0.25f, 1.0f				);

	GDEFINE_ENUM_TYPE(COLOR_TYPE, uint8_t);
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
