/// Copyright 2011-2017 - asm128
#include "gpk_datatype.h"

#include "gpk_io.h"
#include "gpk_string.h"
#include "gpk_size.h"

//
uint32_t		gpk::getStringFromDataType		( ::gpk::DATA_TYPE dataType, char* string ) {
	stacxpr const char	typeUINT	[]					= "UINT"
		,				typeINT		[]					= "INT"
		,				typeFLOAT	[]					= "FLOAT"
		,				typeUFLOAT	[]					= "UFLOAT"
		,				typeNORM	[]					= "_NORM"
		,				typeUNORM	[]					= ""
		,				typePAD		[]					= "_P"
		,				typeNOPAD	[]					= ""
		;
	const char			* typeString					= GTYPEID_ISFLOAT(dataType) ? (GTYPEID_ISSIGNED(dataType)?typeFLOAT:typeUFLOAT) : (GTYPEID_ISSIGNED(dataType)?typeINT:typeUINT);
	char				typeElements	[16]			= {0};
	uint32_t			elementCount					= GTYPEID_ELEMENTCOUNT(dataType);
	if( elementCount > 1 )
		snprintf(typeElements, ::gpk::size(typeElements) - 2, "_%u", elementCount);

	char				outString		[64]			= {0};
	snprintf(outString, ::gpk::size(outString) - 2, "GDATA_TYPE_%s%u%s%s%s", typeString, GTYPEID_ELEMENTSIZE(dataType), GTYPEID_ISNORMALIZED(dataType) ? typeNORM : typeUNORM, typeElements, GTYPEID_ELEMENTPAD(dataType) ? typePAD : typeNOPAD );
	uint32_t			resultLen						= (uint32_t)strlen(outString);
	if(string)
		strcpy_s(string, (uint64_t)resultLen+1, outString);
	return resultLen;
}
