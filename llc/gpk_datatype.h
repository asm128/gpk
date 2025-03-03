/// This file is part of the Game Object Data System
/// Copyright � 2011-2016: Pablo Ariel Zorrilla Cepeda
#include "gpk_eval.h"
#include "gpk_typeint.h"

// --------------------------------------------------------------- DATA_TYPE ---------------------------------------------------------------
#ifndef GPK_DATATYPE_H_23627
#define GPK_DATATYPE_H_23627

namespace gpk
{
#pragma pack(push, 1)
	enum DATA_TYPE : uint32_t
		{	DATA_TYPE_IS_SIGNED				= 0x80000000
		,	DATA_TYPE_IS_NORMALIZED			= 0x40000000
		,	DATA_TYPE_IS_FLOAT				= 0x20000000
		,	DATA_TYPE_IS_BIG_ENDIAN			= 0x10000000
		,	DATA_TYPE_ELEMENT_COUNT_MASK	= 0x0FFFFF00
		,	DATA_TYPE_HAS_INNER_PADDING		= 0x80
		,	DATA_TYPE_BIT_COUNT_MASK		= 0x7F
		};

// Data types (may or may not be -completely?- supported by the hardware)
//
// This was made to represent data types for a variety of hardware and OSs.
// This system would allow us to represent type sizes up to 1,048,576 elements of 128 bits (16mib total?).
// as signed/unsigned integer, signed/unsigned float, signed/unsigned normalized integer,
// signed/unsigned normalized float, all of them in both little or big endianness
//
// Bits are arranged such that: (right-to-left bit ordering)
// bits 1 to 7	: element size in bits-1
// 8th bit		: 0 = no inner padding for elements / 1 = inner padding for elements to next byte size
// bits 9 to 28	: element count-1
// 29th bit		: 0 = little endian / 1 = big endian
// 30th bit		: 0 = int / 1 = float
// 31th bit		: 0 = Unorm / 1 = Norm
// 32th bit		: 0 = Signed / 1 = Unsigned
//
#define GTYPEID_MAKE( IsSigned, IsNorm, IsFloat, IsBigEndian, ElementCount, ElementPad, SizeInBits )	\
	( ::gpk::DATA_TYPE				\
		( ((IsSigned)			? 0 : ::gpk::DATA_TYPE_IS_SIGNED)										\
		| ((IsNorm)				? ::gpk::DATA_TYPE_IS_NORMALIZED		: 0)							\
		| ((IsFloat)			? ::gpk::DATA_TYPE_IS_FLOAT				: 0)							\
		| ((IsBigEndian)		? ::gpk::DATA_TYPE_IS_BIG_ENDIAN		: 0)							\
		| ((((ElementCount)-1)	& 0xFFFFF) << 8)														\
		| ((ElementPad)			? ::gpk::DATA_TYPE_HAS_INNER_PADDING	: 0)							\
		| (((SizeInBits)-1)		& 0x0000007F	)														\
	))

#define	GTYPEID_ISSIGNED(		_dataTypeID )	(0U == ( ::gpk::DATA_TYPE_IS_SIGNED			& _dataTypeID ))		// returns true if signed
#define	GTYPEID_ISNORMALIZED(	_dataTypeID )	(0U != ( ::gpk::DATA_TYPE_IS_NORMALIZED		& _dataTypeID ))		// returns true if the value represents a range between 0 and 1
#define	GTYPEID_ISFLOAT(		_dataTypeID )	(0U != ( ::gpk::DATA_TYPE_IS_FLOAT			& _dataTypeID ))		// returns true if the element bits represent a floating point number
#define	GTYPEID_ISBIGENDIAN(	_dataTypeID )	(0U != ( ::gpk::DATA_TYPE_IS_BIG_ENDIAN		& _dataTypeID ))		// returns true if the element bits has the bits inverted
#define	GTYPEID_ELEMENTPAD(		_dataTypeID )	(0U != ( ::gpk::DATA_TYPE_HAS_INNER_PADDING	& _dataTypeID ))		// returns true if the elements have inner padding to next byte size
#define	GTYPEID_ELEMENTCOUNT(	_dataTypeID )	(uint32_t((0x0FFFFF00U	& _dataTypeID) >> 8) + 1U)							// returns the element count (up to 1024k elements)
#define	GTYPEID_ELEMENTSIZE(	_dataTypeID )	(uint32_t( 0x7FU		& _dataTypeID) + 1U)								// returns the element size in bits (up to 128 bits)

// Return the byte count of the element type (ElementSize/8). Note: this funcion pads leading bits to next byte size.
#define	GTYPEID_ELEMENTBYTES( _dataTypeID )		((GTYPEID_ELEMENTSIZE( _dataTypeID )/8)	+ one_if(GTYPEID_ELEMENTSIZE( _dataTypeID ) % 8))

// Return the byte count of the data type (ElementCount*(ElementSize/8))
#define	GTYPEID_TOTALBYTES( _dataTypeID )																		\
	( GTYPEID_ELEMENTPAD(_dataTypeID) ?																			\
		(GTYPEID_ELEMENTBYTES( _dataTypeID )*GTYPEID_ELEMENTCOUNT( _dataTypeID )) :								\
		(							\
			( (GTYPEID_ELEMENTSIZE( _dataTypeID )*(uint32_t)GTYPEID_ELEMENTCOUNT( _dataTypeID )) / 8 )			\
			+ one_if( (GTYPEID_ELEMENTSIZE( _dataTypeID )*(uint32_t)GTYPEID_ELEMENTCOUNT( _dataTypeID )) % 8 )	\
		)							\
	)

	struct SDataTypeID {
		uint32_t			SizeInBits		:  7	;
		uint32_t			ElementPad		:  1	;
		uint32_t			ElementCount	: 20	;
		uint32_t			IsBigEndian		:  1	;
		uint32_t			IsFloat			:  1	;
		uint32_t			IsNorm			:  1	;
		uint32_t			IsSigned		:  1	;

		inlcxpr	uint32_t	ElementBytes	()							const	noexcept	{ return ( SizeInBits / 8 + one_if( SizeInBits % 8 ) ); }
		inlcxpr	uint32_t	TotalBytes		()							const	noexcept	{ return ElementPad ? ElementBytes() * (ElementCount + 1) : SizeInBits * (ElementCount + 1) / 8 + one_if( SizeInBits * (ElementCount + 1) % 8 ); }

		inlcxpr	bool		IsSTL			()							const	noexcept	{ return (1 == SizeInBits) && IsSigned && IsFloat			&& IsNorm			&& (IsBigEndian == 0); }
		inlcxpr	bool		IsNonUniform	()							const	noexcept	{ return (1 == SizeInBits) && IsSigned && (IsFloat == 0)	&& (IsNorm == 0)	&& (IsBigEndian == 0); }
		inlcxpr	bool		IsUniform		()							const	noexcept	{ return !IsNonUniform(); }

		inlcxpr				SDataTypeID		( ::gpk::DATA_TYPE typeId )			noexcept
			: SizeInBits	(GTYPEID_ELEMENTSIZE	(typeId) - 1)
			, ElementPad	(GTYPEID_ELEMENTPAD		(typeId))
			, ElementCount	(GTYPEID_ELEMENTCOUNT	(typeId) - 1)
			, IsBigEndian	(GTYPEID_ISBIGENDIAN	(typeId))
			, IsFloat		(GTYPEID_ISFLOAT		(typeId))
			, IsNorm		(GTYPEID_ISNORMALIZED	(typeId))
			, IsSigned		(GTYPEID_ISSIGNED		(typeId))
		{}
		//
		inlcxpr				SDataTypeID
			( uint8_t	sizeInBits		= 1
			, uint32_t	elementCount	= 1
			, bool		isFloat			= false
			, bool		isSigned		= true
			, bool		isNorm			= false
			, bool		elementPad		= false
			, bool		isBigEndian		= false
			)					noexcept
			: SizeInBits	(sizeInBits		- 1)
			, ElementPad	(elementPad		? 1 : 0)
			, ElementCount	(elementCount	- 1)
			, IsBigEndian	(isBigEndian	? 1 : 0)
			, IsFloat		(isFloat		? 1 : 0)
			, IsNorm		(isNorm			? 1 : 0)
			, IsSigned		(isSigned		? 0 : 1)
		{}
		inlcxpr	operator	DATA_TYPE		()							const				{
			return GTYPEID_MAKE
				( !IsSigned
				, IsNorm
				, IsFloat
				, IsBigEndian
				, ElementCount+1
				, ElementPad
				, SizeInBits+1
				);
		}
		inlcxpr	bool		operator==		(const DATA_TYPE& other)	const				{
			return	uint32_t(SizeInBits) + 1	==	uint32_t(GTYPEID_ELEMENTSIZE	(other))
				&&	ElementPad					==			(GTYPEID_ELEMENTPAD		(other) ? 1U : 0U)
				&&	uint32_t(ElementCount) + 1	==	uint32_t(GTYPEID_ELEMENTCOUNT	(other))
				&&	IsBigEndian					==			(GTYPEID_ISBIGENDIAN	(other) ? 1U : 0U)
				&&	IsFloat						==			(GTYPEID_ISFLOAT		(other) ? 1U : 0U)
				&&	IsNorm						==			(GTYPEID_ISNORMALIZED	(other) ? 1U : 0U)
				&&	IsSigned					==			(GTYPEID_ISSIGNED		(other) ? 0U : 1U)
				;
		}
		inlcxpr	bool		operator==		(const SDataTypeID& other)	const				{
			return	SizeInBits		==	other.SizeInBits
				&&	ElementPad		==	other.ElementPad
				&&	ElementCount	==	other.ElementCount
				&&	IsBigEndian		==	other.IsBigEndian
				&&	IsFloat			==	other.IsFloat
				&&	IsNorm			==	other.IsNorm
				&&	IsSigned		==	other.IsSigned
				;
		}
	};
	//----------------------------------------------------------------------------------------------------------------------------------

// ( IsSigned, IsNorm, IsFloat, IsBigEndian, ElementCount, ElementPad, SizeInBits )
#define GTYPEID_POINTER_MAKE(	TypeName )							::gpk::SDataTypeID((uint8_t)(sizeof(TypeName*)*8)	, 1	, false, false)	// defines an unsigned unorm little endian integer of sizeof(TypeName*) bytes.
#define GTYPEID_DATA_MAKE(		TypeName )							::gpk::SDataTypeID(8, sizeof(TypeName)					, false, false)	// defines an array of unsigned unorm little endian sizeof(TypeName) bytes.

#define GTYPEID_MAKE_NON_UNIFORM(	_TypeCode )						::gpk::SDataTypeID(1, _TypeCode, false, true, false)
#define GTYPEID_MAKE_STL(			_TypeCode )						::gpk::SDataTypeID(1, _TypeCode, true , true, true )

// ( IsSigned, IsNorm, IsFloat, IsBigEndian, ElementCount, ElementPad, SizeInBits )
#define GTYPEID_MAKE_INT(			ElementBits, ElementCount )		::gpk::SDataTypeID(ElementBits, ElementCount				)
#define GTYPEID_MAKE_UINT(			ElementBits, ElementCount )		::gpk::SDataTypeID(ElementBits, ElementCount, false, false	)
#define GTYPEID_MAKE_FLOAT(			ElementBits, ElementCount )		::gpk::SDataTypeID(ElementBits, ElementCount, true			)
#define GTYPEID_MAKE_UFLOAT(		ElementBits, ElementCount )		::gpk::SDataTypeID(ElementBits, ElementCount, true , false	)
//
#define GTYPEID_MAKE_INT_NORM(		ElementBits, ElementCount )		::gpk::SDataTypeID(ElementBits, ElementCount, false, true	, true)
#define GTYPEID_MAKE_UINT_NORM(		ElementBits, ElementCount )		::gpk::SDataTypeID(ElementBits, ElementCount, false, false	, true)
#define GTYPEID_MAKE_FLOAT_NORM(	ElementBits, ElementCount )		::gpk::SDataTypeID(ElementBits, ElementCount, true	, true	, true)
#define GTYPEID_MAKE_UFLOAT_NORM(	ElementBits, ElementCount )		::gpk::SDataTypeID(ElementBits, ElementCount, true , false	, true)

// ( IsSigned, IsNorm, IsFloat, IsBigEndian, ElementCount, ElementPad, SizeInBits )
#define GTYPEID_MAKE_UINT8(		ElementCount)						GTYPEID_MAKE_UINT	( 8  , ElementCount )
#define GTYPEID_MAKE_UINT16(	ElementCount)						GTYPEID_MAKE_UINT	( 16 , ElementCount )
#define GTYPEID_MAKE_UINT32(	ElementCount)						GTYPEID_MAKE_UINT	( 32 , ElementCount )
#define GTYPEID_MAKE_UINT64(	ElementCount)						GTYPEID_MAKE_UINT	( 64 , ElementCount )
#define GTYPEID_MAKE_INT8(		ElementCount)						GTYPEID_MAKE_INT	( 8  , ElementCount )
#define GTYPEID_MAKE_INT16(		ElementCount)						GTYPEID_MAKE_INT	( 16 , ElementCount )
#define GTYPEID_MAKE_INT32(		ElementCount)						GTYPEID_MAKE_INT	( 32 , ElementCount )
#define GTYPEID_MAKE_INT64(		ElementCount)						GTYPEID_MAKE_INT	( 64 , ElementCount )

#define GTYPEID_MAKE_UFLOAT16(ElementCount)							GTYPEID_MAKE_UFLOAT		( 16 , ElementCount )
#define GTYPEID_MAKE_UFLOAT32(ElementCount)							GTYPEID_MAKE_UFLOAT		( 32 , ElementCount )
#define GTYPEID_MAKE_UFLOAT64(ElementCount)							GTYPEID_MAKE_UFLOAT		( 64 , ElementCount )
#define GTYPEID_MAKE_FLOAT16( ElementCount)							GTYPEID_MAKE_FLOAT		( 16 , ElementCount )
#define GTYPEID_MAKE_FLOAT32( ElementCount)							GTYPEID_MAKE_FLOAT		( 32 , ElementCount )
#define GTYPEID_MAKE_FLOAT64( ElementCount)							GTYPEID_MAKE_FLOAT		( 64 , ElementCount )

	// Type ID can now be built with the GTYPEID_MAKE macros, so these would be "helper" constants
	stacxpr DATA_TYPE	DATA_TYPE_UINT1				= GTYPEID_MAKE_UINT		 (1,0x001);
	stacxpr DATA_TYPE	DATA_TYPE_UINT5				= GTYPEID_MAKE_UINT		 (5,0x001);
	stacxpr DATA_TYPE	DATA_TYPE_UINT5_3			= GTYPEID_MAKE_UINT		 (5,0x003);
	stacxpr DATA_TYPE	DATA_TYPE_UINT8				= GTYPEID_MAKE_UINT8	 (0x001);
	stacxpr DATA_TYPE	DATA_TYPE_UINT8_2			= GTYPEID_MAKE_UINT8	 (0x002);
	stacxpr DATA_TYPE	DATA_TYPE_UINT8_3			= GTYPEID_MAKE_UINT8	 (0x003);
	stacxpr DATA_TYPE	DATA_TYPE_UINT8_4			= GTYPEID_MAKE_UINT8	 (0x004);
	stacxpr DATA_TYPE	DATA_TYPE_UINT8_5			= GTYPEID_MAKE_UINT8	 (0x005);
	stacxpr DATA_TYPE	DATA_TYPE_UINT8_6			= GTYPEID_MAKE_UINT8	 (0x006);
	stacxpr DATA_TYPE	DATA_TYPE_UINT8_7			= GTYPEID_MAKE_UINT8	 (0x007);
	stacxpr DATA_TYPE	DATA_TYPE_UINT8_8			= GTYPEID_MAKE_UINT8	 (0x008);
	stacxpr DATA_TYPE	DATA_TYPE_UINT8_9			= GTYPEID_MAKE_UINT8	 (0x009);
	stacxpr DATA_TYPE	DATA_TYPE_UINT8_16			= GTYPEID_MAKE_UINT8	 (0x010);
	stacxpr DATA_TYPE	DATA_TYPE_UINT8_32			= GTYPEID_MAKE_UINT8	 (0x020);
	stacxpr DATA_TYPE	DATA_TYPE_UINT8_64			= GTYPEID_MAKE_UINT8	 (0x040);
	stacxpr DATA_TYPE	DATA_TYPE_UINT8_128			= GTYPEID_MAKE_UINT8	 (0x080);
	stacxpr DATA_TYPE	DATA_TYPE_UINT8_256			= GTYPEID_MAKE_UINT8	 (0x100);
	stacxpr DATA_TYPE	DATA_TYPE_UINT16			= GTYPEID_MAKE_UINT16	 (0x001);
	stacxpr DATA_TYPE	DATA_TYPE_UINT16_2			= GTYPEID_MAKE_UINT16	 (0x002);
	stacxpr DATA_TYPE	DATA_TYPE_UINT16_3			= GTYPEID_MAKE_UINT16	 (0x003);
	stacxpr DATA_TYPE	DATA_TYPE_UINT16_4			= GTYPEID_MAKE_UINT16	 (0x004);
	stacxpr DATA_TYPE	DATA_TYPE_UINT16_5			= GTYPEID_MAKE_UINT16	 (0x005);
	stacxpr DATA_TYPE	DATA_TYPE_UINT16_6			= GTYPEID_MAKE_UINT16	 (0x006);
	stacxpr DATA_TYPE	DATA_TYPE_UINT16_7			= GTYPEID_MAKE_UINT16	 (0x007);
	stacxpr DATA_TYPE	DATA_TYPE_UINT16_8			= GTYPEID_MAKE_UINT16	 (0x008);
	stacxpr DATA_TYPE	DATA_TYPE_UINT16_9			= GTYPEID_MAKE_UINT16	 (0x009);
	stacxpr DATA_TYPE	DATA_TYPE_UINT16_16			= GTYPEID_MAKE_UINT16	 (0x010);
	stacxpr DATA_TYPE	DATA_TYPE_UINT16_32			= GTYPEID_MAKE_UINT16	 (0x020);
	stacxpr DATA_TYPE	DATA_TYPE_UINT16_64			= GTYPEID_MAKE_UINT16	 (0x040);
	stacxpr DATA_TYPE	DATA_TYPE_UINT16_128		= GTYPEID_MAKE_UINT16	 (0x080);
	stacxpr DATA_TYPE	DATA_TYPE_UINT16_256		= GTYPEID_MAKE_UINT16	 (0x100);
	stacxpr DATA_TYPE	DATA_TYPE_UINT32			= GTYPEID_MAKE_UINT32	 (0x001);
	stacxpr DATA_TYPE	DATA_TYPE_UINT32_2			= GTYPEID_MAKE_UINT32	 (0x002);
	stacxpr DATA_TYPE	DATA_TYPE_UINT32_3			= GTYPEID_MAKE_UINT32	 (0x003);
	stacxpr DATA_TYPE	DATA_TYPE_UINT32_4			= GTYPEID_MAKE_UINT32	 (0x004);
	stacxpr DATA_TYPE	DATA_TYPE_UINT32_5			= GTYPEID_MAKE_UINT32	 (0x005);
	stacxpr DATA_TYPE	DATA_TYPE_UINT32_6			= GTYPEID_MAKE_UINT32	 (0x006);
	stacxpr DATA_TYPE	DATA_TYPE_UINT32_7			= GTYPEID_MAKE_UINT32	 (0x007);
	stacxpr DATA_TYPE	DATA_TYPE_UINT32_8			= GTYPEID_MAKE_UINT32	 (0x008);
	stacxpr DATA_TYPE	DATA_TYPE_UINT32_9			= GTYPEID_MAKE_UINT32	 (0x009);
	stacxpr DATA_TYPE	DATA_TYPE_UINT32_16			= GTYPEID_MAKE_UINT32	 (0x010);
	stacxpr DATA_TYPE	DATA_TYPE_UINT32_32			= GTYPEID_MAKE_UINT32	 (0x020);
	stacxpr DATA_TYPE	DATA_TYPE_UINT32_64			= GTYPEID_MAKE_UINT32	 (0x040);
	stacxpr DATA_TYPE	DATA_TYPE_UINT32_128		= GTYPEID_MAKE_UINT32	 (0x080);
	stacxpr DATA_TYPE	DATA_TYPE_UINT32_256		= GTYPEID_MAKE_UINT32	 (0x100);
	stacxpr DATA_TYPE	DATA_TYPE_UINT64			= GTYPEID_MAKE_UINT64	 (0x001);
	stacxpr DATA_TYPE	DATA_TYPE_UINT64_2			= GTYPEID_MAKE_UINT64	 (0x002);
	stacxpr DATA_TYPE	DATA_TYPE_UINT64_3			= GTYPEID_MAKE_UINT64	 (0x003);
	stacxpr DATA_TYPE	DATA_TYPE_UINT64_4			= GTYPEID_MAKE_UINT64	 (0x004);
	stacxpr DATA_TYPE	DATA_TYPE_UINT64_5			= GTYPEID_MAKE_UINT64	 (0x005);
	stacxpr DATA_TYPE	DATA_TYPE_UINT64_6			= GTYPEID_MAKE_UINT64	 (0x006);
	stacxpr DATA_TYPE	DATA_TYPE_UINT64_7			= GTYPEID_MAKE_UINT64	 (0x007);
	stacxpr DATA_TYPE	DATA_TYPE_UINT64_8			= GTYPEID_MAKE_UINT64	 (0x008);
	stacxpr DATA_TYPE	DATA_TYPE_UINT64_9			= GTYPEID_MAKE_UINT64	 (0x009);
	stacxpr DATA_TYPE	DATA_TYPE_UINT64_16			= GTYPEID_MAKE_UINT64	 (0x010);
	stacxpr DATA_TYPE	DATA_TYPE_UINT64_32			= GTYPEID_MAKE_UINT64	 (0x020);
	stacxpr DATA_TYPE	DATA_TYPE_UINT64_64			= GTYPEID_MAKE_UINT64	 (0x040);
	stacxpr DATA_TYPE	DATA_TYPE_UINT64_128		= GTYPEID_MAKE_UINT64	 (0x080);
	stacxpr DATA_TYPE	DATA_TYPE_UINT64_256		= GTYPEID_MAKE_UINT64	 (0x100);
	stacxpr DATA_TYPE	DATA_TYPE_INT5				= GTYPEID_MAKE_INT		 (5,0x001);
	stacxpr DATA_TYPE	DATA_TYPE_INT5_3			= GTYPEID_MAKE_INT		 (5,0x003);
	stacxpr DATA_TYPE	DATA_TYPE_INT8				= GTYPEID_MAKE_INT8		 (0x001);
	stacxpr DATA_TYPE	DATA_TYPE_INT8_2			= GTYPEID_MAKE_INT8		 (0x002);
	stacxpr DATA_TYPE	DATA_TYPE_INT8_3			= GTYPEID_MAKE_INT8		 (0x003);
	stacxpr DATA_TYPE	DATA_TYPE_INT8_4			= GTYPEID_MAKE_INT8		 (0x004);
	stacxpr DATA_TYPE	DATA_TYPE_INT8_5			= GTYPEID_MAKE_INT8		 (0x005);
	stacxpr SDataTypeID	DATA_TYPE_INT8_6			= GTYPEID_MAKE_INT8		 (0x006);
	stacxpr SDataTypeID	DATA_TYPE_INT8_7			= GTYPEID_MAKE_INT8		 (0x007);
	stacxpr SDataTypeID	DATA_TYPE_INT8_8			= GTYPEID_MAKE_INT8		 (0x008);
	stacxpr SDataTypeID	DATA_TYPE_INT8_9			= GTYPEID_MAKE_INT8		 (0x009);
	stacxpr SDataTypeID	DATA_TYPE_INT8_16			= GTYPEID_MAKE_INT8		 (0x010);
	stacxpr SDataTypeID	DATA_TYPE_INT8_32			= GTYPEID_MAKE_INT8		 (0x020);
	stacxpr SDataTypeID	DATA_TYPE_INT8_64			= GTYPEID_MAKE_INT8		 (0x040);
	stacxpr SDataTypeID	DATA_TYPE_INT8_128			= GTYPEID_MAKE_INT8		 (0x080);
	stacxpr SDataTypeID	DATA_TYPE_INT8_256			= GTYPEID_MAKE_INT8		 (0x100);
	stacxpr SDataTypeID	DATA_TYPE_INT16				= GTYPEID_MAKE_INT16	 (0x001);
	stacxpr SDataTypeID	DATA_TYPE_INT16_2			= GTYPEID_MAKE_INT16	 (0x002);
	stacxpr SDataTypeID	DATA_TYPE_INT16_3			= GTYPEID_MAKE_INT16	 (0x003);
	stacxpr SDataTypeID	DATA_TYPE_INT16_4			= GTYPEID_MAKE_INT16	 (0x004);
	stacxpr SDataTypeID	DATA_TYPE_INT16_5			= GTYPEID_MAKE_INT16	 (0x005);
	stacxpr SDataTypeID	DATA_TYPE_INT16_6			= GTYPEID_MAKE_INT16	 (0x006);
	stacxpr SDataTypeID	DATA_TYPE_INT16_7			= GTYPEID_MAKE_INT16	 (0x007);
	stacxpr SDataTypeID	DATA_TYPE_INT16_8			= GTYPEID_MAKE_INT16	 (0x008);
	stacxpr SDataTypeID	DATA_TYPE_INT16_9			= GTYPEID_MAKE_INT16	 (0x009);
	stacxpr SDataTypeID	DATA_TYPE_INT16_16			= GTYPEID_MAKE_INT16	 (0x010);
	stacxpr SDataTypeID	DATA_TYPE_INT16_32			= GTYPEID_MAKE_INT16	 (0x020);
	stacxpr SDataTypeID	DATA_TYPE_INT16_64			= GTYPEID_MAKE_INT16	 (0x040);
	stacxpr SDataTypeID	DATA_TYPE_INT16_128			= GTYPEID_MAKE_INT16	 (0x080);
	stacxpr SDataTypeID	DATA_TYPE_INT16_256			= GTYPEID_MAKE_INT16	 (0x100);
	stacxpr SDataTypeID	DATA_TYPE_INT32				= GTYPEID_MAKE_INT32	 (0x001);
	stacxpr SDataTypeID	DATA_TYPE_INT32_2			= GTYPEID_MAKE_INT32	 (0x002);
	stacxpr SDataTypeID	DATA_TYPE_INT32_3			= GTYPEID_MAKE_INT32	 (0x003);
	stacxpr SDataTypeID	DATA_TYPE_INT32_4			= GTYPEID_MAKE_INT32	 (0x004);
	stacxpr SDataTypeID	DATA_TYPE_INT32_5			= GTYPEID_MAKE_INT32	 (0x005);
	stacxpr SDataTypeID	DATA_TYPE_INT32_6			= GTYPEID_MAKE_INT32	 (0x006);
	stacxpr SDataTypeID	DATA_TYPE_INT32_7			= GTYPEID_MAKE_INT32	 (0x007);
	stacxpr SDataTypeID	DATA_TYPE_INT32_8			= GTYPEID_MAKE_INT32	 (0x008);
	stacxpr SDataTypeID	DATA_TYPE_INT32_9			= GTYPEID_MAKE_INT32	 (0x009);
	stacxpr SDataTypeID	DATA_TYPE_INT32_16			= GTYPEID_MAKE_INT32	 (0x010);
	stacxpr SDataTypeID	DATA_TYPE_INT32_32			= GTYPEID_MAKE_INT32	 (0x020);
	stacxpr SDataTypeID	DATA_TYPE_INT32_64			= GTYPEID_MAKE_INT32	 (0x040);
	stacxpr SDataTypeID	DATA_TYPE_INT32_128			= GTYPEID_MAKE_INT32	 (0x080);
	stacxpr SDataTypeID	DATA_TYPE_INT32_256			= GTYPEID_MAKE_INT32	 (0x100);
	stacxpr SDataTypeID	DATA_TYPE_INT64				= GTYPEID_MAKE_INT64	 (0x001);
	stacxpr SDataTypeID	DATA_TYPE_INT64_2			= GTYPEID_MAKE_INT64	 (0x002);
	stacxpr SDataTypeID	DATA_TYPE_INT64_3			= GTYPEID_MAKE_INT64	 (0x003);
	stacxpr SDataTypeID	DATA_TYPE_INT64_4			= GTYPEID_MAKE_INT64	 (0x004);
	stacxpr SDataTypeID	DATA_TYPE_INT64_5			= GTYPEID_MAKE_INT64	 (0x005);
	stacxpr SDataTypeID	DATA_TYPE_INT64_6			= GTYPEID_MAKE_INT64	 (0x006);
	stacxpr SDataTypeID	DATA_TYPE_INT64_7			= GTYPEID_MAKE_INT64	 (0x007);
	stacxpr SDataTypeID	DATA_TYPE_INT64_8			= GTYPEID_MAKE_INT64	 (0x008);
	stacxpr SDataTypeID	DATA_TYPE_INT64_9			= GTYPEID_MAKE_INT64	 (0x009);
	stacxpr SDataTypeID	DATA_TYPE_INT64_16			= GTYPEID_MAKE_INT64	 (0x010);
	stacxpr SDataTypeID	DATA_TYPE_INT64_32			= GTYPEID_MAKE_INT64	 (0x020);
	stacxpr SDataTypeID	DATA_TYPE_INT64_64			= GTYPEID_MAKE_INT64	 (0x040);
	stacxpr SDataTypeID	DATA_TYPE_INT64_128			= GTYPEID_MAKE_INT64	 (0x080);
	stacxpr SDataTypeID	DATA_TYPE_INT64_256			= GTYPEID_MAKE_INT64	 (0x100);
	stacxpr SDataTypeID	DATA_TYPE_INT64_NORM_256	= GTYPEID_MAKE_FLOAT16	 (0x100);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16			= GTYPEID_MAKE_FLOAT16	 (0x001);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_2			= GTYPEID_MAKE_FLOAT16	 (0x002);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_3			= GTYPEID_MAKE_FLOAT16	 (0x003);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_4			= GTYPEID_MAKE_FLOAT16	 (0x004);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_5			= GTYPEID_MAKE_FLOAT16	 (0x005);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_6			= GTYPEID_MAKE_FLOAT16	 (0x006);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_7			= GTYPEID_MAKE_FLOAT16	 (0x007);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_8			= GTYPEID_MAKE_FLOAT16	 (0x008);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_9			= GTYPEID_MAKE_FLOAT16	 (0x009);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_16		= GTYPEID_MAKE_FLOAT16	 (0x010);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_32		= GTYPEID_MAKE_FLOAT16	 (0x020);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_64		= GTYPEID_MAKE_FLOAT16	 (0x040);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_128		= GTYPEID_MAKE_FLOAT16	 (0x080);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_256		= GTYPEID_MAKE_FLOAT16	 (0x100);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32			= GTYPEID_MAKE_FLOAT32	 (0x001);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_2			= GTYPEID_MAKE_FLOAT32	 (0x002);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_3			= GTYPEID_MAKE_FLOAT32	 (0x003);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_4			= GTYPEID_MAKE_FLOAT32	 (0x004);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_5			= GTYPEID_MAKE_FLOAT32	 (0x005);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_6			= GTYPEID_MAKE_FLOAT32	 (0x006);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_7			= GTYPEID_MAKE_FLOAT32	 (0x007);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_8			= GTYPEID_MAKE_FLOAT32	 (0x008);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_9			= GTYPEID_MAKE_FLOAT32	 (0x009);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_16		= GTYPEID_MAKE_FLOAT32	 (0x010);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_32		= GTYPEID_MAKE_FLOAT32	 (0x020);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_64		= GTYPEID_MAKE_FLOAT32	 (0x040);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_128		= GTYPEID_MAKE_FLOAT32	 (0x080);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_256		= GTYPEID_MAKE_FLOAT32	 (0x100);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64			= GTYPEID_MAKE_FLOAT64	 (0x001);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_2			= GTYPEID_MAKE_FLOAT64	 (0x002);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_3			= GTYPEID_MAKE_FLOAT64	 (0x003);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_4			= GTYPEID_MAKE_FLOAT64	 (0x004);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_5			= GTYPEID_MAKE_FLOAT64	 (0x005);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_6			= GTYPEID_MAKE_FLOAT64	 (0x006);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_7			= GTYPEID_MAKE_FLOAT64	 (0x007);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_8			= GTYPEID_MAKE_FLOAT64	 (0x008);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_9			= GTYPEID_MAKE_FLOAT64	 (0x009);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_16		= GTYPEID_MAKE_FLOAT64	 (0x010);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_32		= GTYPEID_MAKE_FLOAT64	 (0x020);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_64		= GTYPEID_MAKE_FLOAT64	 (0x040);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_128		= GTYPEID_MAKE_FLOAT64	 (0x080);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_256		= GTYPEID_MAKE_FLOAT64	 (0x100);
	stacxpr SDataTypeID	DATA_TYPE_UINT5_NORM		= GTYPEID_MAKE_UINT_NORM	(0x05,0x001);
	stacxpr SDataTypeID	DATA_TYPE_UINT5_NORM_3		= GTYPEID_MAKE_UINT_NORM	(0x05,0x003);
	stacxpr SDataTypeID	DATA_TYPE_UINT8_NORM		= GTYPEID_MAKE_UINT_NORM	(0x08,0x001);
	stacxpr SDataTypeID	DATA_TYPE_UINT8_NORM_2		= GTYPEID_MAKE_UINT_NORM	(0x08,0x002);
	stacxpr SDataTypeID	DATA_TYPE_UINT8_NORM_3		= GTYPEID_MAKE_UINT_NORM	(0x08,0x003);
	stacxpr SDataTypeID	DATA_TYPE_UINT8_NORM_4		= GTYPEID_MAKE_UINT_NORM	(0x08,0x004);
	stacxpr SDataTypeID	DATA_TYPE_UINT8_NORM_5		= GTYPEID_MAKE_UINT_NORM	(0x08,0x005);
	stacxpr SDataTypeID	DATA_TYPE_UINT8_NORM_6		= GTYPEID_MAKE_UINT_NORM	(0x08,0x006);
	stacxpr SDataTypeID	DATA_TYPE_UINT8_NORM_7		= GTYPEID_MAKE_UINT_NORM	(0x08,0x007);
	stacxpr SDataTypeID	DATA_TYPE_UINT8_NORM_8		= GTYPEID_MAKE_UINT_NORM	(0x08,0x008);
	stacxpr SDataTypeID	DATA_TYPE_UINT8_NORM_9		= GTYPEID_MAKE_UINT_NORM	(0x08,0x009);
	stacxpr SDataTypeID	DATA_TYPE_UINT8_NORM_16		= GTYPEID_MAKE_UINT_NORM	(0x08,0x010);
	stacxpr SDataTypeID	DATA_TYPE_UINT8_NORM_32		= GTYPEID_MAKE_UINT_NORM	(0x08,0x020);
	stacxpr SDataTypeID	DATA_TYPE_UINT8_NORM_64		= GTYPEID_MAKE_UINT_NORM	(0x08,0x040);
	stacxpr SDataTypeID	DATA_TYPE_UINT8_NORM_128	= GTYPEID_MAKE_UINT_NORM	(0x08,0x080);
	stacxpr SDataTypeID	DATA_TYPE_UINT8_NORM_256	= GTYPEID_MAKE_UINT_NORM	(0x08,0x100);
	stacxpr SDataTypeID	DATA_TYPE_UINT16_NORM		= GTYPEID_MAKE_UINT_NORM	(0x10,0x001);
	stacxpr SDataTypeID	DATA_TYPE_UINT16_NORM_2		= GTYPEID_MAKE_UINT_NORM	(0x10,0x002);
	stacxpr SDataTypeID	DATA_TYPE_UINT16_NORM_3		= GTYPEID_MAKE_UINT_NORM	(0x10,0x003);
	stacxpr SDataTypeID	DATA_TYPE_UINT16_NORM_4		= GTYPEID_MAKE_UINT_NORM	(0x10,0x004);
	stacxpr SDataTypeID	DATA_TYPE_UINT16_NORM_5		= GTYPEID_MAKE_UINT_NORM	(0x10,0x005);
	stacxpr SDataTypeID	DATA_TYPE_UINT16_NORM_6		= GTYPEID_MAKE_UINT_NORM	(0x10,0x006);
	stacxpr SDataTypeID	DATA_TYPE_UINT16_NORM_7		= GTYPEID_MAKE_UINT_NORM	(0x10,0x007);
	stacxpr SDataTypeID	DATA_TYPE_UINT16_NORM_8		= GTYPEID_MAKE_UINT_NORM	(0x10,0x008);
	stacxpr SDataTypeID	DATA_TYPE_UINT16_NORM_9		= GTYPEID_MAKE_UINT_NORM	(0x10,0x009);
	stacxpr SDataTypeID	DATA_TYPE_UINT16_NORM_16	= GTYPEID_MAKE_UINT_NORM	(0x10,0x010);
	stacxpr SDataTypeID	DATA_TYPE_UINT16_NORM_32	= GTYPEID_MAKE_UINT_NORM	(0x10,0x020);
	stacxpr SDataTypeID	DATA_TYPE_UINT16_NORM_64	= GTYPEID_MAKE_UINT_NORM	(0x10,0x040);
	stacxpr SDataTypeID	DATA_TYPE_UINT16_NORM_128	= GTYPEID_MAKE_UINT_NORM	(0x10,0x080);
	stacxpr SDataTypeID	DATA_TYPE_UINT16_NORM_256	= GTYPEID_MAKE_UINT_NORM	(0x10,0x100);
	stacxpr SDataTypeID	DATA_TYPE_UINT32_NORM		= GTYPEID_MAKE_UINT_NORM	(0x20,0x001);
	stacxpr SDataTypeID	DATA_TYPE_UINT32_NORM_2		= GTYPEID_MAKE_UINT_NORM	(0x20,0x002);
	stacxpr SDataTypeID	DATA_TYPE_UINT32_NORM_3		= GTYPEID_MAKE_UINT_NORM	(0x20,0x003);
	stacxpr SDataTypeID	DATA_TYPE_UINT32_NORM_4		= GTYPEID_MAKE_UINT_NORM	(0x20,0x004);
	stacxpr SDataTypeID	DATA_TYPE_UINT32_NORM_5		= GTYPEID_MAKE_UINT_NORM	(0x20,0x005);
	stacxpr SDataTypeID	DATA_TYPE_UINT32_NORM_6		= GTYPEID_MAKE_UINT_NORM	(0x20,0x006);
	stacxpr SDataTypeID	DATA_TYPE_UINT32_NORM_7		= GTYPEID_MAKE_UINT_NORM	(0x20,0x007);
	stacxpr SDataTypeID	DATA_TYPE_UINT32_NORM_8		= GTYPEID_MAKE_UINT_NORM	(0x20,0x008);
	stacxpr SDataTypeID	DATA_TYPE_UINT32_NORM_9		= GTYPEID_MAKE_UINT_NORM	(0x20,0x009);
	stacxpr SDataTypeID	DATA_TYPE_UINT32_NORM_16	= GTYPEID_MAKE_UINT_NORM	(0x20,0x010);
	stacxpr SDataTypeID	DATA_TYPE_UINT32_NORM_32	= GTYPEID_MAKE_UINT_NORM	(0x20,0x020);
	stacxpr SDataTypeID	DATA_TYPE_UINT32_NORM_64	= GTYPEID_MAKE_UINT_NORM	(0x20,0x040);
	stacxpr SDataTypeID	DATA_TYPE_UINT32_NORM_128	= GTYPEID_MAKE_UINT_NORM	(0x20,0x080);
	stacxpr SDataTypeID	DATA_TYPE_UINT32_NORM_256	= GTYPEID_MAKE_UINT_NORM	(0x20,0x100);
	stacxpr SDataTypeID	DATA_TYPE_UINT64_NORM		= GTYPEID_MAKE_UINT_NORM	(0x40,0x001);
	stacxpr SDataTypeID	DATA_TYPE_UINT64_NORM_2		= GTYPEID_MAKE_UINT_NORM	(0x40,0x002);
	stacxpr SDataTypeID	DATA_TYPE_UINT64_NORM_3		= GTYPEID_MAKE_UINT_NORM	(0x40,0x003);
	stacxpr SDataTypeID	DATA_TYPE_UINT64_NORM_4		= GTYPEID_MAKE_UINT_NORM	(0x40,0x004);
	stacxpr SDataTypeID	DATA_TYPE_UINT64_NORM_5		= GTYPEID_MAKE_UINT_NORM	(0x40,0x005);
	stacxpr SDataTypeID	DATA_TYPE_UINT64_NORM_6		= GTYPEID_MAKE_UINT_NORM	(0x40,0x006);
	stacxpr SDataTypeID	DATA_TYPE_UINT64_NORM_7		= GTYPEID_MAKE_UINT_NORM	(0x40,0x007);
	stacxpr SDataTypeID	DATA_TYPE_UINT64_NORM_8		= GTYPEID_MAKE_UINT_NORM	(0x40,0x008);
	stacxpr SDataTypeID	DATA_TYPE_UINT64_NORM_9		= GTYPEID_MAKE_UINT_NORM	(0x40,0x009);
	stacxpr SDataTypeID	DATA_TYPE_UINT64_NORM_16	= GTYPEID_MAKE_UINT_NORM	(0x40,0x010);
	stacxpr SDataTypeID	DATA_TYPE_UINT64_NORM_32	= GTYPEID_MAKE_UINT_NORM	(0x40,0x020);
	stacxpr SDataTypeID	DATA_TYPE_UINT64_NORM_64	= GTYPEID_MAKE_UINT_NORM	(0x40,0x040);
	stacxpr SDataTypeID	DATA_TYPE_UINT64_NORM_128	= GTYPEID_MAKE_UINT_NORM	(0x40,0x080);
	stacxpr SDataTypeID	DATA_TYPE_UINT64_NORM_256	= GTYPEID_MAKE_UINT_NORM	(0x40,0x100);
	stacxpr SDataTypeID	DATA_TYPE_INT5_NORM			= GTYPEID_MAKE_INT_NORM		(0x05,0x001);
	stacxpr SDataTypeID	DATA_TYPE_INT5_NORM_3		= GTYPEID_MAKE_INT_NORM		(0x05,0x003);
	stacxpr SDataTypeID	DATA_TYPE_INT8_NORM			= GTYPEID_MAKE_INT_NORM		(0x08,0x001);
	stacxpr SDataTypeID	DATA_TYPE_INT8_NORM_2		= GTYPEID_MAKE_INT_NORM		(0x08,0x002);
	stacxpr SDataTypeID	DATA_TYPE_INT8_NORM_3		= GTYPEID_MAKE_INT_NORM		(0x08,0x003);
	stacxpr SDataTypeID	DATA_TYPE_INT8_NORM_4		= GTYPEID_MAKE_INT_NORM		(0x08,0x004);
	stacxpr SDataTypeID	DATA_TYPE_INT8_NORM_5		= GTYPEID_MAKE_INT_NORM		(0x08,0x005);
	stacxpr SDataTypeID	DATA_TYPE_INT8_NORM_6		= GTYPEID_MAKE_INT_NORM		(0x08,0x006);
	stacxpr SDataTypeID	DATA_TYPE_INT8_NORM_7		= GTYPEID_MAKE_INT_NORM		(0x08,0x007);
	stacxpr SDataTypeID	DATA_TYPE_INT8_NORM_8		= GTYPEID_MAKE_INT_NORM		(0x08,0x008);
	stacxpr SDataTypeID	DATA_TYPE_INT8_NORM_9		= GTYPEID_MAKE_INT_NORM		(0x08,0x009);
	stacxpr SDataTypeID	DATA_TYPE_INT8_NORM_16		= GTYPEID_MAKE_INT_NORM		(0x08,0x010);
	stacxpr SDataTypeID	DATA_TYPE_INT8_NORM_32		= GTYPEID_MAKE_INT_NORM		(0x08,0x020);
	stacxpr SDataTypeID	DATA_TYPE_INT8_NORM_64		= GTYPEID_MAKE_INT_NORM		(0x08,0x040);
	stacxpr SDataTypeID	DATA_TYPE_INT8_NORM_128		= GTYPEID_MAKE_INT_NORM		(0x08,0x080);
	stacxpr SDataTypeID	DATA_TYPE_INT8_NORM_256		= GTYPEID_MAKE_INT_NORM		(0x08,0x100);
	stacxpr SDataTypeID	DATA_TYPE_INT16_NORM		= GTYPEID_MAKE_INT_NORM		(0x10,0x001);
	stacxpr SDataTypeID	DATA_TYPE_INT16_NORM_2		= GTYPEID_MAKE_INT_NORM		(0x10,0x002);
	stacxpr SDataTypeID	DATA_TYPE_INT16_NORM_3		= GTYPEID_MAKE_INT_NORM		(0x10,0x003);
	stacxpr SDataTypeID	DATA_TYPE_INT16_NORM_4		= GTYPEID_MAKE_INT_NORM		(0x10,0x004);
	stacxpr SDataTypeID	DATA_TYPE_INT16_NORM_5		= GTYPEID_MAKE_INT_NORM		(0x10,0x005);
	stacxpr SDataTypeID	DATA_TYPE_INT16_NORM_6		= GTYPEID_MAKE_INT_NORM		(0x10,0x006);
	stacxpr SDataTypeID	DATA_TYPE_INT16_NORM_7		= GTYPEID_MAKE_INT_NORM		(0x10,0x007);
	stacxpr SDataTypeID	DATA_TYPE_INT16_NORM_8		= GTYPEID_MAKE_INT_NORM		(0x10,0x008);
	stacxpr SDataTypeID	DATA_TYPE_INT16_NORM_9		= GTYPEID_MAKE_INT_NORM		(0x10,0x009);
	stacxpr SDataTypeID	DATA_TYPE_INT16_NORM_16		= GTYPEID_MAKE_INT_NORM		(0x10,0x010);
	stacxpr SDataTypeID	DATA_TYPE_INT16_NORM_32		= GTYPEID_MAKE_INT_NORM		(0x10,0x020);
	stacxpr SDataTypeID	DATA_TYPE_INT16_NORM_64		= GTYPEID_MAKE_INT_NORM		(0x10,0x040);
	stacxpr SDataTypeID	DATA_TYPE_INT16_NORM_128	= GTYPEID_MAKE_INT_NORM		(0x10,0x080);
	stacxpr SDataTypeID	DATA_TYPE_INT16_NORM_256	= GTYPEID_MAKE_INT_NORM		(0x10,0x100);
	stacxpr SDataTypeID	DATA_TYPE_INT32_NORM		= GTYPEID_MAKE_INT_NORM		(0x20,0x001);
	stacxpr SDataTypeID	DATA_TYPE_INT32_NORM_2		= GTYPEID_MAKE_INT_NORM		(0x20,0x002);
	stacxpr SDataTypeID	DATA_TYPE_INT32_NORM_3		= GTYPEID_MAKE_INT_NORM		(0x20,0x003);
	stacxpr SDataTypeID	DATA_TYPE_INT32_NORM_4		= GTYPEID_MAKE_INT_NORM		(0x20,0x004);
	stacxpr SDataTypeID	DATA_TYPE_INT32_NORM_5		= GTYPEID_MAKE_INT_NORM		(0x20,0x005);
	stacxpr SDataTypeID	DATA_TYPE_INT32_NORM_6		= GTYPEID_MAKE_INT_NORM		(0x20,0x006);
	stacxpr SDataTypeID	DATA_TYPE_INT32_NORM_7		= GTYPEID_MAKE_INT_NORM		(0x20,0x007);
	stacxpr SDataTypeID	DATA_TYPE_INT32_NORM_8		= GTYPEID_MAKE_INT_NORM		(0x20,0x008);
	stacxpr SDataTypeID	DATA_TYPE_INT32_NORM_9		= GTYPEID_MAKE_INT_NORM		(0x20,0x009);
	stacxpr SDataTypeID	DATA_TYPE_INT32_NORM_16		= GTYPEID_MAKE_INT_NORM		(0x20,0x010);
	stacxpr SDataTypeID	DATA_TYPE_INT32_NORM_32		= GTYPEID_MAKE_INT_NORM		(0x20,0x020);
	stacxpr SDataTypeID	DATA_TYPE_INT32_NORM_64		= GTYPEID_MAKE_INT_NORM		(0x20,0x040);
	stacxpr SDataTypeID	DATA_TYPE_INT32_NORM_128	= GTYPEID_MAKE_INT_NORM		(0x20,0x080);
	stacxpr SDataTypeID	DATA_TYPE_INT32_NORM_256	= GTYPEID_MAKE_INT_NORM		(0x20,0x100);
	stacxpr SDataTypeID	DATA_TYPE_INT64_NORM		= GTYPEID_MAKE_INT_NORM		(0x40,0x001);
	stacxpr SDataTypeID	DATA_TYPE_INT64_NORM_2		= GTYPEID_MAKE_INT_NORM		(0x40,0x002);
	stacxpr SDataTypeID	DATA_TYPE_INT64_NORM_3		= GTYPEID_MAKE_INT_NORM		(0x40,0x003);
	stacxpr SDataTypeID	DATA_TYPE_INT64_NORM_4		= GTYPEID_MAKE_INT_NORM		(0x40,0x004);
	stacxpr SDataTypeID	DATA_TYPE_INT64_NORM_5		= GTYPEID_MAKE_INT_NORM		(0x40,0x005);
	stacxpr SDataTypeID	DATA_TYPE_INT64_NORM_6		= GTYPEID_MAKE_INT_NORM		(0x40,0x006);
	stacxpr SDataTypeID	DATA_TYPE_INT64_NORM_7		= GTYPEID_MAKE_INT_NORM		(0x40,0x007);
	stacxpr SDataTypeID	DATA_TYPE_INT64_NORM_8		= GTYPEID_MAKE_INT_NORM		(0x40,0x008);
	stacxpr SDataTypeID	DATA_TYPE_INT64_NORM_9		= GTYPEID_MAKE_INT_NORM		(0x40,0x009);
	stacxpr SDataTypeID	DATA_TYPE_INT64_NORM_16		= GTYPEID_MAKE_INT_NORM		(0x40,0x010);
	stacxpr SDataTypeID	DATA_TYPE_INT64_NORM_32		= GTYPEID_MAKE_INT_NORM		(0x40,0x020);
	stacxpr SDataTypeID	DATA_TYPE_INT64_NORM_64		= GTYPEID_MAKE_INT_NORM		(0x40,0x040);
	stacxpr SDataTypeID	DATA_TYPE_INT64_NORM_128	= GTYPEID_MAKE_INT_NORM		(0x40,0x080);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_NORM		= GTYPEID_MAKE_FLOAT_NORM	(0x10,0x001);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_NORM_2	= GTYPEID_MAKE_FLOAT_NORM	(0x10,0x002);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_NORM_3	= GTYPEID_MAKE_FLOAT_NORM	(0x10,0x003);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_NORM_4	= GTYPEID_MAKE_FLOAT_NORM	(0x10,0x004);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_NORM_5	= GTYPEID_MAKE_FLOAT_NORM	(0x10,0x005);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_NORM_6	= GTYPEID_MAKE_FLOAT_NORM	(0x10,0x006);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_NORM_7	= GTYPEID_MAKE_FLOAT_NORM	(0x10,0x007);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_NORM_8	= GTYPEID_MAKE_FLOAT_NORM	(0x10,0x008);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_NORM_9	= GTYPEID_MAKE_FLOAT_NORM	(0x10,0x009);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_NORM_16	= GTYPEID_MAKE_FLOAT_NORM	(0x10,0x010);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_NORM_32	= GTYPEID_MAKE_FLOAT_NORM	(0x10,0x020);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_NORM_64	= GTYPEID_MAKE_FLOAT_NORM	(0x10,0x040);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_NORM_128	= GTYPEID_MAKE_FLOAT_NORM	(0x10,0x080);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT16_NORM_256	= GTYPEID_MAKE_FLOAT_NORM	(0x10,0x100);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_NORM		= GTYPEID_MAKE_FLOAT_NORM	(0x20,0x001);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_NORM_2	= GTYPEID_MAKE_FLOAT_NORM	(0x20,0x002);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_NORM_3	= GTYPEID_MAKE_FLOAT_NORM	(0x20,0x003);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_NORM_4	= GTYPEID_MAKE_FLOAT_NORM	(0x20,0x004);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_NORM_5	= GTYPEID_MAKE_FLOAT_NORM	(0x20,0x005);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_NORM_6	= GTYPEID_MAKE_FLOAT_NORM	(0x20,0x006);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_NORM_7	= GTYPEID_MAKE_FLOAT_NORM	(0x20,0x007);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_NORM_8	= GTYPEID_MAKE_FLOAT_NORM	(0x20,0x008);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_NORM_9	= GTYPEID_MAKE_FLOAT_NORM	(0x20,0x009);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_NORM_16	= GTYPEID_MAKE_FLOAT_NORM	(0x20,0x010);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_NORM_32	= GTYPEID_MAKE_FLOAT_NORM	(0x20,0x020);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_NORM_64	= GTYPEID_MAKE_FLOAT_NORM	(0x20,0x040);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_NORM_128	= GTYPEID_MAKE_FLOAT_NORM	(0x20,0x080);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT32_NORM_256	= GTYPEID_MAKE_FLOAT_NORM	(0x20,0x100);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_NORM		= GTYPEID_MAKE_FLOAT_NORM	(0x40,0x001);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_NORM_2	= GTYPEID_MAKE_FLOAT_NORM	(0x40,0x002);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_NORM_3	= GTYPEID_MAKE_FLOAT_NORM	(0x40,0x003);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_NORM_4	= GTYPEID_MAKE_FLOAT_NORM	(0x40,0x004);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_NORM_5	= GTYPEID_MAKE_FLOAT_NORM	(0x40,0x005);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_NORM_6	= GTYPEID_MAKE_FLOAT_NORM	(0x40,0x006);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_NORM_7	= GTYPEID_MAKE_FLOAT_NORM	(0x40,0x007);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_NORM_8	= GTYPEID_MAKE_FLOAT_NORM	(0x40,0x008);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_NORM_9	= GTYPEID_MAKE_FLOAT_NORM	(0x40,0x009);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_NORM_16	= GTYPEID_MAKE_FLOAT_NORM	(0x40,0x010);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_NORM_32	= GTYPEID_MAKE_FLOAT_NORM	(0x40,0x020);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_NORM_64	= GTYPEID_MAKE_FLOAT_NORM	(0x40,0x040);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_NORM_128	= GTYPEID_MAKE_FLOAT_NORM	(0x40,0x080);
	stacxpr SDataTypeID	DATA_TYPE_FLOAT64_NORM_256	= GTYPEID_MAKE_FLOAT_NORM	(0x40,0x100);

	// --------------	--------------------	------------------------- DATA_TYPE ---------------------------------------------------------------
	// -- Non-POD types:
	stacxpr SDataTypeID	DATA_TYPE_UNKNOWN			= GTYPEID_MAKE_NON_UNIFORM(0x01);				// aka DATA_TYPE_INT1
	stacxpr SDataTypeID	DATA_TYPE_GPKVECTOR			= GTYPEID_MAKE_NON_UNIFORM(0x02);				// aka DATA_TYPE_INT1_2
	stacxpr SDataTypeID	DATA_TYPE_GPKSTRING			= GTYPEID_MAKE_NON_UNIFORM(0x03);				// aka DATA_TYPE_INT1_3
	stacxpr SDataTypeID	DATA_TYPE_GLABEL			= GTYPEID_MAKE_NON_UNIFORM(0x04);				// aka DATA_TYPE_INT1_4
	stacxpr SDataTypeID	DATA_TYPE_GENUM				= GTYPEID_MAKE_NON_UNIFORM(0x05);				// aka DATA_TYPE_INT1_5
	stacxpr SDataTypeID	DATA_TYPE_GDESCRIPTOR		= GTYPEID_MAKE_NON_UNIFORM(0x06);				// aka DATA_TYPE_INT1_6
	stacxpr SDataTypeID	DATA_TYPE_GPOD				= GTYPEID_MAKE_NON_UNIFORM(0x07);				// aka DATA_TYPE_INT1_7
	stacxpr SDataTypeID	DATA_TYPE_GOBJ				= GTYPEID_MAKE_NON_UNIFORM(0x08);				//
	stacxpr SDataTypeID	DATA_TYPE_GNCO				= GTYPEID_MAKE_NON_UNIFORM(0x09);				//
	stacxpr SDataTypeID	DATA_TYPE_GREF				= GTYPEID_MAKE_NON_UNIFORM(0x0A);				//
	stacxpr SDataTypeID	DATA_TYPE_GLIST				= GTYPEID_MAKE_NON_UNIFORM(0x0B);				//
	stacxpr SDataTypeID	DATA_TYPE_GPURE				= GTYPEID_MAKE_NON_UNIFORM(0x0C);				//
	stacxpr SDataTypeID	DATA_TYPE_CUSTOM			= GTYPEID_MAKE_NON_UNIFORM(0x0D);				//
	stacxpr SDataTypeID	DATA_TYPE_POD				= GTYPEID_MAKE_NON_UNIFORM(0x0E);				//
	stacxpr SDataTypeID	DATA_TYPE_OBJ				= GTYPEID_MAKE_NON_UNIFORM(0x0F);				//
	stacxpr SDataTypeID	DATA_TYPE_NCO				= GTYPEID_MAKE_NON_UNIFORM(0x10);				//
	stacxpr SDataTypeID	DATA_TYPE_MOD				= GTYPEID_MAKE_NON_UNIFORM(0x11);				//
	stacxpr SDataTypeID	DATA_TYPE_FUN				= GTYPEID_MAKE_NON_UNIFORM(0x12);				//
	stacxpr SDataTypeID	DATA_TYPE_PRC				= GTYPEID_MAKE_NON_UNIFORM(0x13);				// aka DATA_TYPE_INT1_13
	stacxpr SDataTypeID	DATA_TYPE_POINTER			= GTYPEID_MAKE_NON_UNIFORM(sizeof(void*)*8);	// notice that this definition changes depending on the platform

	// -- STL types
	stacxpr SDataTypeID	DATA_TYPE_STLOTHER			= GTYPEID_MAKE_STL(0x01);	// aka DATA_TYPE_FLOAT1_NORM
	stacxpr SDataTypeID	DATA_TYPE_STLVECTOR			= GTYPEID_MAKE_STL(0x02);	// aka DATA_TYPE_FLOAT1_NORM_2
	stacxpr SDataTypeID	DATA_TYPE_STLSTRING			= GTYPEID_MAKE_STL(0x03);	// aka DATA_TYPE_FLOAT1_NORM_3

	stacxpr SDataTypeID	DATA_TYPE_SIZE_T			= ::gpk::SDataTypeID(sizeof(size_t)*8, 1, false, sizeof(size_t) == 4 ? false : true);

	//}; // DATA_TYPE
	// ------------------------------------------ Aliases
	stacxpr	DATA_TYPE	DATA_TYPE_DATATYPE			= DATA_TYPE_UINT32;
	stacxpr	DATA_TYPE	DATA_TYPE_BOOL				= DATA_TYPE_UINT8;

#pragma pack(pop)
	uint32_t			getStringFromDataType			(DATA_TYPE dataType, sc_t* string);

#if defined(__WINDOWS__)
#	pragma warning(disable : 4063)	// On Windows, using enum types like we do cause the compiler to throw a warning when the warning level is set to 4
#endif

} // namespace

#endif // GPK_DATATYPE_H_23627
