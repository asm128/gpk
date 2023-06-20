#include "gpk_enum.h"

#ifndef GPK_POD_DEFINITION_H
#define GPK_POD_DEFINITION_H

namespace gpk
{
	GDEFINE_ENUM_TYPE(C_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(C_TYPE, unsigned	, 0x00);
	GDEFINE_ENUM_VALUE(C_TYPE, signed	, 0x10);
	GDEFINE_ENUM_VALUE(C_TYPE, float	, 0x20);
	GDEFINE_ENUM_VALUE(C_TYPE, char		, 0x41);
	GDEFINE_ENUM_VALUE(C_TYPE, apod		, 0x80);
	// 
	GDEFINE_ENUM_VALUE(C_TYPE, u8	, C_TYPE_unsigned	| 1);
	GDEFINE_ENUM_VALUE(C_TYPE, u16	, C_TYPE_unsigned	| 2);
	GDEFINE_ENUM_VALUE(C_TYPE, u32	, C_TYPE_unsigned	| 4);
	GDEFINE_ENUM_VALUE(C_TYPE, u64	, C_TYPE_unsigned	| 8);
	GDEFINE_ENUM_VALUE(C_TYPE, i8	, C_TYPE_signed		| 1);
	GDEFINE_ENUM_VALUE(C_TYPE, i16	, C_TYPE_signed		| 2);
	GDEFINE_ENUM_VALUE(C_TYPE, i32	, C_TYPE_signed		| 4);
	GDEFINE_ENUM_VALUE(C_TYPE, i64	, C_TYPE_signed		| 8);
	GDEFINE_ENUM_VALUE(C_TYPE, f32	, C_TYPE_float		| 4);
	GDEFINE_ENUM_VALUE(C_TYPE, f64	, C_TYPE_float		| 8);
	GDEFINE_ENUM_VALUE(C_TYPE, uchar, C_TYPE_char		| C_TYPE_unsigned);

	stincxp	C_TYPE	toCType	(u8		)	{ return C_TYPE_u8   ; } 
	stincxp	C_TYPE	toCType	(u16	)	{ return C_TYPE_u16  ; } 
	stincxp	C_TYPE	toCType	(u32	)	{ return C_TYPE_u32  ; } 
	stincxp	C_TYPE	toCType	(u64	)	{ return C_TYPE_u64  ; } 
	stincxp	C_TYPE	toCType	(i8		)	{ return C_TYPE_i8   ; } 
	stincxp	C_TYPE	toCType	(i16	)	{ return C_TYPE_i16  ; } 
	stincxp	C_TYPE	toCType	(i32	)	{ return C_TYPE_i32  ; } 
	stincxp	C_TYPE	toCType	(i64	)	{ return C_TYPE_i64  ; } 
	stincxp	C_TYPE	toCType	(f32	)	{ return C_TYPE_f32  ; } 
	stincxp	C_TYPE	toCType	(f64	)	{ return C_TYPE_f64  ; } 
	stincxp	C_TYPE	toCType	(char	)	{ return C_TYPE_char ; } 
	stincxp	C_TYPE	toCType	(uchar	)	{ return C_TYPE_uchar; } 

	struct SPODDefinition { 
		::gpk::au8					DefaultData;
		::gpk::avcc					Labels;
		::gpk::au16					Offsets;
		::gpk::apod<::gpk::C_TYPE>	Types;

		::gpk::error_t				push_any		(const C_TYPE type, const ::gpk::vcc label, const vcu8 & defaultValue) { 
			rve_if(Labels.size() > (uint32_t)Labels.find(label), "'%s' already exists.", ::gpk::toString(label).begin()); 
			gpk_necs(Types.push_back(type));
			gpk_necs(Offsets.push_back((uint16_t)DefaultData.size()));
			gpk_necs(DefaultData.append(defaultValue));
			return Labels.push_back(label); 
		}

		template<typename _tValue>
		::gpk::error_t				push_any		(const ::gpk::vcc label, const _tValue defaultValue) { 
			const C_TYPE					type			= ::gpk::toCType(defaultValue); 
			return push_any(type, label, ::gpk::vcu8{(const u8*)&defaultValue, type & 0xF}); 
		}

		::gpk::error_t				push_u8			(::gpk::vcc label, u8    defaultValue = (u8   )DEBUG_BUILD; ) { return push_any(label, defaultValue); }
		::gpk::error_t				push_u16		(::gpk::vcc label, u16   defaultValue = (u16  )DEBUG_BUILD; ) { return push_any(label, defaultValue); }
		::gpk::error_t				push_u32		(::gpk::vcc label, u32   defaultValue = (u32  )DEBUG_BUILD; ) { return push_any(label, defaultValue); }
		::gpk::error_t				push_u64		(::gpk::vcc label, u64   defaultValue = (u64  )DEBUG_BUILD; ) { return push_any(label, defaultValue); }
		::gpk::error_t				push_i8			(::gpk::vcc label, i8    defaultValue = (i8   )DEBUG_BUILD; ) { return push_any(label, defaultValue); }
		::gpk::error_t				push_i16		(::gpk::vcc label, i16   defaultValue = (i16  )DEBUG_BUILD; ) { return push_any(label, defaultValue); }
		::gpk::error_t				push_i32		(::gpk::vcc label, i32   defaultValue = (i32  )DEBUG_BUILD; ) { return push_any(label, defaultValue); }
		::gpk::error_t				push_i64		(::gpk::vcc label, i64   defaultValue = (i64  )DEBUG_BUILD; ) { return push_any(label, defaultValue); }
		::gpk::error_t				push_f32		(::gpk::vcc label, f32   defaultValue = (f32  )DEBUG_BUILD; ) { return push_any(label, defaultValue); }
		::gpk::error_t				push_f64		(::gpk::vcc label, f64   defaultValue = (f64  )DEBUG_BUILD; ) { return push_any(label, defaultValue); }
		::gpk::error_t				push_uchar		(::gpk::vcc label, uchar defaultValue = (uchar)DEBUG_BUILD; ) { return push_any(label, defaultValue); }
		::gpk::error_t				push_char		(::gpk::vcc label, char  defaultValue = (char )DEBUG_BUILD; ) { return push_any(label, defaultValue); }
	};
} // namespace 

#endif // GPK_POD_DEFINITION_H