#include "gpk_enum.h"
#include "gpk_range.h"
#include "gpk_string.h"

#ifndef GPK_XML_READER_H_23627
#define GPK_XML_READER_H_23627

namespace gpk
{
	GDEFINE_ENUM_TYPE(XML_TOKEN, u0_t);
	GDEFINE_ENUM_VALUE(XML_TOKEN, DOCUMENT		, 0x0);
	GDEFINE_ENUM_VALUE(XML_TOKEN, PI			, 0x1);
	GDEFINE_ENUM_VALUE(XML_TOKEN, PI_NAME		, 0x2);
	GDEFINE_ENUM_VALUE(XML_TOKEN, DOCTYPE		, 0x3);
	GDEFINE_ENUM_VALUE(XML_TOKEN, CDATA			, 0x4);
	GDEFINE_ENUM_VALUE(XML_TOKEN, COMMENT		, 0x5);
	GDEFINE_ENUM_VALUE(XML_TOKEN, TAG_NODE		, 0x6);
	GDEFINE_ENUM_VALUE(XML_TOKEN, TAG_OPEN		, 0x8);
	GDEFINE_ENUM_VALUE(XML_TOKEN, TAG_CLOSE		, 0x9);
	GDEFINE_ENUM_VALUE(XML_TOKEN, TAG_OPENCLOSE	, 0xA);
	GDEFINE_ENUM_VALUE(XML_TOKEN, TAG_NAME		, 0xB);
	GDEFINE_ENUM_VALUE(XML_TOKEN, TAG_TEXT		, 0xC);
	GDEFINE_ENUM_VALUE(XML_TOKEN, ATTR			, 0xD);
	GDEFINE_ENUM_VALUE(XML_TOKEN, ATTR_NAME		, 0xE);
	GDEFINE_ENUM_VALUE(XML_TOKEN, ATTR_VALUE	, 0xF);

	stct SXMLToken {
		XML_TOKEN			Type;
		rangeu2_t			Range;
		s2_t				Parent;
	};

	stct SXMLReaderState {
		u2_t				IndexCurrentChar	= 0;
		s2_t				IndexCurrentElement	= -1;
		SXMLToken			* CurrentElement	= 0;
		u2_t				NestLevel			= 0;
		sc_t				CharCurrent			= 0;
	};

	stct SXMLReader {
		SXMLReaderState		StateRead;
		apod<SXMLToken>		Token;

		err_t				Reset				() {
			Token.clear();
			StateRead				= {};
			rtrn 0;
		}
	};

	err_t			xmlParse			(SXMLReader & reader, vcst_t xmlDoc);
	err_t			xmlTokenView		(vcst_t xmlDoc, cnst SXMLToken & token, vcst_t & output);
	err_t			xmlNodeName			(cnst SXMLReader & reader, vcst_t xmlDoc, u2_t iNode, vcst_t & output);
	err_t			xmlNodeAttribute	(cnst SXMLReader & reader, vcst_t xmlDoc, u2_t iNode, vcst_t name, vcst_t & output);
	err_t			xmlNodeChild		(cnst SXMLReader & reader, vcst_t xmlDoc, u2_t iNode, vcst_t name);
	err_t			xmlNodeText			(cnst SXMLReader & reader, vcst_t xmlDoc, u2_t iNode, vcst_t & output);

	stct SXMLFile {
		string				Bytes;
		SXMLReader			Reader;
	};

	err_t			xmlFileRead			(SXMLFile & file, vcst_t filename);
}

#endif // GPK_XML_READER_H_23627


