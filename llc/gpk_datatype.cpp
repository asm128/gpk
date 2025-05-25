/// Copyright 2011-2017 - asm128
#include "gpk_datatype.h"

#include "gpk_io.h"
#include "gpk_cstring.h"
#include "gpk_size.h"

//
uint32_t		gpk::getStringFromDataType		( ::gpk::DATA_TYPE dataType, sc_t* string ) {
	stacxpr const sc_t	typeUINT	[]					= "UINT"
		,				typeINT		[]					= "INT"
		,				typeFLOAT	[]					= "FLOAT"
		,				typeUFLOAT	[]					= "UFLOAT"
		,				typeNORM	[]					= "_NORM"
		,				typeUNORM	[]					= ""
		,				typePAD		[]					= "_P"
		,				typeNOPAD	[]					= ""
		;
	const sc_t			* typeString					= GTYPEID_ISFLOAT(dataType) ? (GTYPEID_ISSIGNED(dataType)?typeFLOAT:typeUFLOAT) : (GTYPEID_ISSIGNED(dataType)?typeINT:typeUINT);
	sc_t				typeElements	[16]			= {0};
	uint32_t			elementCount					= GTYPEID_ELEMENTCOUNT(dataType);
	if( elementCount > 1 )
		snprintf(typeElements, ::gpk::size(typeElements) - 2, "_%" GPK_FMT_U32, elementCount);

	sc_t				outString		[64]			= {0};
	snprintf(outString, ::gpk::size(outString) - 2, "GDATA_TYPE_%s%" GPK_FMT_U32 "%s%s%s", typeString, GTYPEID_ELEMENTSIZE(dataType), GTYPEID_ISNORMALIZED(dataType) ? typeNORM : typeUNORM, typeElements, GTYPEID_ELEMENTPAD(dataType) ? typePAD : typeNOPAD );
	uint32_t			resultLen						= (uint32_t)strlen(outString);
	if(string)
		strcpy_s(string, (uint64_t)resultLen+1, outString);
	return resultLen;
}
