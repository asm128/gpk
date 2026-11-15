#ifndef GPK_ENUM_OPERATORS_H_23627
#define GPK_ENUM_OPERATORS_H_23627

#define GDEFINE_ENUM_OPERATORS(EnumName) \
	sinx	EnumName	oper &	(EnumName  a, EnumName b)	nxpt	{ return (EnumName)		((__underlying_type(EnumName))a & (__underlying_type(EnumName))b); } \
	sinx	EnumName	oper ~	(EnumName  a)				nxpt	{ return (EnumName)		(~(__underlying_type(EnumName))a); } \
	sinx	EnumName	oper ^	(EnumName  a, EnumName b)	nxpt	{ return (EnumName)		((__underlying_type(EnumName))a ^ (__underlying_type(EnumName))b); } \
	stin	EnumName &	oper |=	(EnumName & a, EnumName b)	nxpt	{ return (EnumName &)	(((__underlying_type(EnumName) &)a) |= (__underlying_type(EnumName))b); } \
	stin	EnumName &	oper &=	(EnumName & a, EnumName b)	nxpt	{ return (EnumName &)	(((__underlying_type(EnumName) &)a) &= (__underlying_type(EnumName))b); } \
	stin	EnumName &	oper ^=	(EnumName & a, EnumName b)	nxpt	{ return (EnumName &)	(((__underlying_type(EnumName) &)a) ^= (__underlying_type(EnumName))b); } \
	sinx	EnumName	oper |	(EnumName  a, EnumName b)	nxpt	{ return (EnumName)		((__underlying_type(EnumName))a | (__underlying_type(EnumName))b); }

#endif // GPK_ENUM_OPERATORS_H_23627
