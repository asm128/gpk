#include "gpk_enum.h"

#ifndef GPK_POD_DEFINITION_H_23627
#define GPK_POD_DEFINITION_H_23627

namespace gpk
{
	GDEFINE_ENUM_TYPE(C_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(C_TYPE, unsigned	, 0x00);
	GDEFINE_ENUM_VALUE(C_TYPE, signed	, 0x10);
	GDEFINE_ENUM_VALUE(C_TYPE, float	, 0x20);
	GDEFINE_ENUM_VALUE(C_TYPE, sc_t		, 0x41);
	GDEFINE_ENUM_VALUE(C_TYPE, apod		, 0x80);
	// 
	GDEFINE_ENUM_VALUE(C_TYPE, u0_t	, C_TYPE_unsigned	| 1);
	GDEFINE_ENUM_VALUE(C_TYPE, u1_t	, C_TYPE_unsigned	| 2);
	GDEFINE_ENUM_VALUE(C_TYPE, u2_t	, C_TYPE_unsigned	| 4);
	GDEFINE_ENUM_VALUE(C_TYPE, u3_t	, C_TYPE_unsigned	| 8);
	GDEFINE_ENUM_VALUE(C_TYPE, s0_t	, C_TYPE_signed		| 1);
	GDEFINE_ENUM_VALUE(C_TYPE, s1_t	, C_TYPE_signed		| 2);
	GDEFINE_ENUM_VALUE(C_TYPE, s2_t	, C_TYPE_signed		| 4);
	GDEFINE_ENUM_VALUE(C_TYPE, s3_t	, C_TYPE_signed		| 8);
	GDEFINE_ENUM_VALUE(C_TYPE, f2_t	, C_TYPE_float		| 4);
	GDEFINE_ENUM_VALUE(C_TYPE, f3_t	, C_TYPE_float		| 8);
	GDEFINE_ENUM_VALUE(C_TYPE, uc_t, C_TYPE_char		| C_TYPE_unsigned);

	stincxp	C_TYPE	toCType	(u0_t		)	{ return ::gpk::C_TYPE_u8   ; } 
	stincxp	C_TYPE	toCType	(u1_t	)	{ return ::gpk::C_TYPE_u16  ; } 
	stincxp	C_TYPE	toCType	(u2_t	)	{ return ::gpk::C_TYPE_u32  ; } 
	stincxp	C_TYPE	toCType	(u3_t	)	{ return ::gpk::C_TYPE_u64  ; } 
	stincxp	C_TYPE	toCType	(s0_t		)	{ return ::gpk::C_TYPE_i8   ; } 
	stincxp	C_TYPE	toCType	(s1_t	)	{ return ::gpk::C_TYPE_i16  ; } 
	stincxp	C_TYPE	toCType	(s2_t	)	{ return ::gpk::C_TYPE_i32  ; } 
	stincxp	C_TYPE	toCType	(s3_t	)	{ return ::gpk::C_TYPE_i64  ; } 
	stincxp	C_TYPE	toCType	(f2_t	)	{ return ::gpk::C_TYPE_f32  ; } 
	stincxp	C_TYPE	toCType	(f3_t	)	{ return ::gpk::C_TYPE_f64  ; } 
	stincxp	C_TYPE	toCType	(sc_t	)	{ return ::gpk::C_TYPE_char ; } 
	stincxp	C_TYPE	toCType	(uc_t	)	{ return ::gpk::C_TYPE_uchar; } 

	struct SPODDefinition { 
		::gpk::au0_t					DefaultData;
		::gpk::avcc					Labels;
		::gpk::au1_t					Offsets;
		::gpk::apod<::gpk::C_TYPE>	Types;

		::gpk::error_t				push_any		(const ::gpk::C_TYPE type, const ::gpk::vcsc_t label, const vcu8 & defaultValue) { 
			rve_if(Labels.size() > (uint32_t)Labels.find(label), "'%s' already exists.", ::gpk::toString(label).begin()); 
			gpk_necs(Types.push_back(type));
			gpk_necs(Offsets.push_back((uint16_t)DefaultData.size()));
			gpk_necs(DefaultData.append(defaultValue));
			return Labels.push_back(label); 
		}

		tplt<tpnm _tValue>
		::gpk::error_t				push_any		(const ::gpk::vcsc_t label, const _tValue defaultValue) { 
			const ::gpk::C_TYPE				type			= ::gpk::toCType(defaultValue); 
			return push_any(type, label, ::gpk::vcu0_t{(const u0_t*)&defaultValue, type & 0xF}); 
		}

		::gpk::error_t				push_u8			(::gpk::vcsc_t label, u0_t    defaultValue = (u0_t   )::gpk::DEBUG_BUILD) { return push_any(label, defaultValue); }
		::gpk::error_t				push_u16		(::gpk::vcsc_t label, u1_t   defaultValue = (u1_t  )::gpk::DEBUG_BUILD) { return push_any(label, defaultValue); }
		::gpk::error_t				push_u32		(::gpk::vcsc_t label, u2_t   defaultValue = (u2_t  )::gpk::DEBUG_BUILD) { return push_any(label, defaultValue); }
		::gpk::error_t				push_u64		(::gpk::vcsc_t label, u3_t   defaultValue = (u3_t  )::gpk::DEBUG_BUILD) { return push_any(label, defaultValue); }
		::gpk::error_t				push_i8			(::gpk::vcsc_t label, s0_t    defaultValue = (s0_t   )::gpk::DEBUG_BUILD) { return push_any(label, defaultValue); }
		::gpk::error_t				push_i16		(::gpk::vcsc_t label, s1_t   defaultValue = (s1_t  )::gpk::DEBUG_BUILD) { return push_any(label, defaultValue); }
		::gpk::error_t				push_i32		(::gpk::vcsc_t label, s2_t   defaultValue = (s2_t  )::gpk::DEBUG_BUILD) { return push_any(label, defaultValue); }
		::gpk::error_t				push_i64		(::gpk::vcsc_t label, s3_t   defaultValue = (s3_t  )::gpk::DEBUG_BUILD) { return push_any(label, defaultValue); }
		::gpk::error_t				push_f32		(::gpk::vcsc_t label, f2_t   defaultValue = (f2_t  )::gpk::DEBUG_BUILD) { return push_any(label, defaultValue); }
		::gpk::error_t				push_f64		(::gpk::vcsc_t label, f3_t   defaultValue = (f3_t  )::gpk::DEBUG_BUILD) { return push_any(label, defaultValue); }
		::gpk::error_t				push_uchar		(::gpk::vcsc_t label, uc_t defaultValue = (uc_t)::gpk::DEBUG_BUILD) { return push_any(label, defaultValue); }
		::gpk::error_t				push_char		(::gpk::vcsc_t label, sc_t  defaultValue = (sc_t )::gpk::DEBUG_BUILD) { return push_any(label, defaultValue); }
	};
} // namespace 

#endif // GPK_POD_DEFINITION_H_23627