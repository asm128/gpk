#include "gpk_enum.h"
#include "gpk_n2.h"
#include "gpk_range.h"

#ifndef GPK_XML_READER_H_23627
#define GPK_XML_READER_H_23627

namespace gpk 
{
	GDEFINE_ENUM_TYPE(XML_TOKEN, uint8_t);
	GDEFINE_ENUM_VALUE(XML_TOKEN, DOCUMENT		, 0x0);
	GDEFINE_ENUM_VALUE(XML_TOKEN, PI			, 0x1);
	GDEFINE_ENUM_VALUE(XML_TOKEN, PI_NAME		, 0x2);
	GDEFINE_ENUM_VALUE(XML_TOKEN, DOCTYPE		, 0x3);
	GDEFINE_ENUM_VALUE(XML_TOKEN, CDATA			, 0x4);
	GDEFINE_ENUM_VALUE(XML_TOKEN, COMMENT		, 0x5);
	GDEFINE_ENUM_VALUE(XML_TOKEN, TAG_NODE		, 0x6);
	//GDEFINE_ENUM_VALUE(XML_TOKEN, TAG			, 0x7);
	GDEFINE_ENUM_VALUE(XML_TOKEN, TAG_OPEN		, 0x8);
	GDEFINE_ENUM_VALUE(XML_TOKEN, TAG_CLOSE		, 0x9);
	GDEFINE_ENUM_VALUE(XML_TOKEN, TAG_OPENCLOSE	, 0xA);
	GDEFINE_ENUM_VALUE(XML_TOKEN, TAG_NAME		, 0xB);
	GDEFINE_ENUM_VALUE(XML_TOKEN, TAG_TEXT		, 0xC);
	GDEFINE_ENUM_VALUE(XML_TOKEN, ATTR			, 0xD);
	GDEFINE_ENUM_VALUE(XML_TOKEN, ATTR_NAME		, 0xE);
	GDEFINE_ENUM_VALUE(XML_TOKEN, ATTR_VALUE	, 0xF);

	struct SXMLToken {
		XML_TOKEN				Type;
		::gpk::rangeu2_t		Range;
		int32_t					Parent;	// -1 for root element
	};

	struct SXMLReaderState {
		uint32_t				IndexCurrentChar	= 0;
		int32_t					IndexCurrentElement	= -1;
		SXMLToken				* CurrentElement	= 0;
		uint32_t				NestLevel			= 0;
		sc_t					CharCurrent			= 0;
		::gpk::apod<SXMLToken>	Token				= {};
	};

	struct SXMLReader {
		SXMLReaderState			StateRead;
		::gpk::apod<SXMLToken>	Token				= {};
	};

	::gpk::error_t			xmlParse			(SXMLReader& reader, ::gpk::vcst_t xmlDoc);
	::gpk::error_t			xmlParseCharacter	(SXMLReader& reader, ::gpk::vcst_t xmlDoc);

	struct SXMLFile {
		::gpk::apod<sc_t>		Bytes				= {};
		::gpk::SXMLReader		Reader				= {};
	};

	::gpk::error_t			xmlFileRead			(::gpk::SXMLFile & file, ::gpk::vcst_c & filename);

}

#endif // GPK_XML_READER_H_20220702