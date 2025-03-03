#define GPK_USE_DEBUG_BREAK_ON_ERROR_LOG

#include "gpk_xml_reader.h"
#include "gpk_array_static.h"
#include "gpk_parse.h"
#include "gpk_file.h"

using gpk::sc_t, gpk::sc_c;

#define xml_info_printf(...) ::gpk::dummy(__VA_ARGS__)// info_printf

//stacxpr	::gpk::astatic<const sc_t, 5> RESERVED_XML_CHARACTERS = {'#', '&', '"', '<', '>'};

::gpk::error_t			gpk::xmlFileRead			(::gpk::SXMLFile & file, const ::gpk::vcc & filename) {
	xml_info_printf("Loading xml file: %s.", filename.begin());
	gpk_necall(gpk::fileToMemory({filename.begin(), filename.size()}, file.Bytes), "Failed to load file: '%s'", filename.begin());
	return ::gpk::xmlParse(file.Reader, file.Bytes);
}

static	::gpk::error_t	xmlOpenElement				(::gpk::SXMLReader & reader, ::gpk::XML_TOKEN token, int32_t iStartCharacter = -1) {
	const int32_t			parentIndex					= reader.StateRead.IndexCurrentElement;
	const ::gpk::XML_TOKEN	parentType					= (parentIndex >= 0) ? reader.Token[parentIndex].Type : (::gpk::XML_TOKEN)-1;
	(void)parentType;
	const uint32_t			offsetChar					= (iStartCharacter == -1) ? reader.StateRead.IndexCurrentChar : iStartCharacter;
	gpk_necs(reader.StateRead.IndexCurrentElement	= reader.Token.push_back({token, {offsetChar, 0}, parentIndex}));
	xml_info_printf("Opening element type: %i (%s) at index %i."
		"\nNest level: %i"
		"\nParent index: %i"
		"\nParent type: %i (%s)"
		"\nCharacter index: %i"
		, token, ::gpk::get_value_label(token).begin(), reader.StateRead.IndexCurrentElement
		, reader.StateRead.NestLevel
		, parentIndex
		, parentType, ::gpk::get_value_label(parentType).begin()
		, offsetChar
	);
	reader.StateRead.CurrentElement						= &reader.Token[reader.StateRead.IndexCurrentElement];
	return ++reader.StateRead.NestLevel;
}

static	::gpk::error_t	xmlCloseElement				(::gpk::SXMLReader & reader, int32_t iStopCharacter = -1) {
	const ::gpk::XML_TOKEN	token						= reader.StateRead.CurrentElement->Type;
	const int32_t			parentIndex					= reader.StateRead.CurrentElement->Parent;
	const ::gpk::XML_TOKEN	parentType					= (parentIndex >= 0) ? reader.Token[parentIndex].Type : (::gpk::XML_TOKEN)-1;
	(void)token;
	(void)parentType;
	xml_info_printf("Closing element type %i (%s) at index %i."
		"\nNest level: %i"
		"\nParent index: %i"
		"\nParent type: %i (%s)"
		, token, ::gpk::get_value_label(token).begin(), reader.StateRead.IndexCurrentElement
		, reader.StateRead.NestLevel
		, parentIndex
		, parentType, ::gpk::get_value_label(parentType).begin()
	);
	reader.StateRead.CurrentElement->Range.Count		= ((iStopCharacter == -1) ? reader.StateRead.IndexCurrentChar : iStopCharacter) - reader.StateRead.CurrentElement->Range.Offset;
	reader.StateRead.IndexCurrentElement				= parentIndex;
	reader.StateRead.CurrentElement						= (parentIndex >= 0) ? &reader.Token[reader.StateRead.IndexCurrentElement] : 0;
	return --reader.StateRead.NestLevel;
}

static	::gpk::error_t	xmlParseStringCharacter	(::gpk::SXMLReader& reader, ::gpk::vcc xmlDoc) {
	const sc_t				& currentChar				= xmlDoc[reader.StateRead.IndexCurrentChar];
	xml_info_printf("Parsing character %i '%c' at index %i for token '%s'.", currentChar, currentChar, reader.StateRead.IndexCurrentChar, ::gpk::get_value_label(reader.StateRead.CurrentElement->Type).begin());
	if(::gpk::isSpaceCharacter(currentChar)) {
		::gpk::skipToNextCharacter(reader.StateRead.IndexCurrentChar, xmlDoc);
		--reader.StateRead.IndexCurrentChar;
		return 0;
	}
	switch(currentChar) {
	default: break;
	case '"': // close attribute string
		gpk_necall(xmlCloseElement(reader), "Invalid nest level: %i", reader.StateRead.NestLevel);
		if(reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_ATTR)
			gpk_necall(xmlCloseElement(reader, reader.StateRead.IndexCurrentChar + 1), "Invalid nest level: %i", reader.StateRead.NestLevel);
		break;
	case '<': // error
	case '&': //
	case '?': //
	case '>': //
		error_printf("Character '%c' must be escaped.", currentChar);
		return -1;
	}
	return 0;
}

static	::gpk::error_t	xmlParseTagCharacter		(::gpk::SXMLReader& reader, ::gpk::vcc xmlDoc) {
	const sc_t				& currentChar				= xmlDoc[reader.StateRead.IndexCurrentChar];
	xml_info_printf("Parsing character %i '%c' at index %i for token '%s'.", currentChar, currentChar, reader.StateRead.IndexCurrentChar, ::gpk::get_value_label(reader.StateRead.CurrentElement->Type).begin());
	if(::gpk::isSpaceCharacter(currentChar)) {
		::gpk::skipToNextCharacter(reader.StateRead.IndexCurrentChar, xmlDoc);
		--reader.StateRead.IndexCurrentChar;
		return 0;
	}
	switch(currentChar) {
	default:
		if( ::gpk::XML_TOKEN_PI			== reader.StateRead.CurrentElement->Type
		 || ::gpk::XML_TOKEN_DOCTYPE	== reader.StateRead.CurrentElement->Type
		 || ::gpk::XML_TOKEN_TAG_OPEN	== reader.StateRead.CurrentElement->Type
		) {
			gpk_necs(xmlOpenElement(reader, ::gpk::XML_TOKEN_ATTR));
			gpk_necs(xmlOpenElement(reader, ::gpk::XML_TOKEN_ATTR_NAME));
		}
		break;
	case '/':
		// if it is, then we need to remove the node-open-name elements and replace them
		if(reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_TAG_OPEN) {
			const uint32_t nextIdx = reader.StateRead.IndexCurrentChar + 1;
			if(nextIdx < xmlDoc.size() && xmlDoc[nextIdx] == '>') // maybe it's an open tag that is also a close tag
				reader.StateRead.CurrentElement->Type	= ::gpk::XML_TOKEN_TAG_OPENCLOSE;
		}
		break;
	case '<': // error
		error_printf("Invalid character: %c", currentChar);
		break;
	case '"': // open attribute string
		ree_if(reader.StateRead.CurrentElement->Type != ::gpk::XML_TOKEN_ATTR, "Invalid character: %c", currentChar);
		gpk_necs(xmlOpenElement(reader, ::gpk::XML_TOKEN_ATTR_VALUE, reader.StateRead.IndexCurrentChar + 1));
		break;
	case '>': // close tag name and tag. Also close tag node if CurrentElement is a TAG_CLOSE
	{
		stacxpr	::gpk::astatic<const ::gpk::XML_TOKEN, 5> validScopes =
			{ ::gpk::XML_TOKEN_TAG_NODE
			, ::gpk::XML_TOKEN_TAG_OPEN
			, ::gpk::XML_TOKEN_TAG_CLOSE
			, ::gpk::XML_TOKEN_TAG_OPENCLOSE
			, ::gpk::XML_TOKEN_TAG_NAME
			};
		gpk_necall(gpk::find<const ::gpk::XML_TOKEN>(reader.StateRead.CurrentElement->Type, validScopes), "Invalid character: %c in node of type '%s'", currentChar, ::gpk::get_value_label(reader.StateRead.CurrentElement->Type).begin());
		{
			bool				closeNode				= reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_TAG_CLOSE || reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_TAG_OPENCLOSE;
			gpk_necall(xmlCloseElement(reader, reader.StateRead.IndexCurrentChar + 1), "Invalid nest level: %i", reader.StateRead.NestLevel); // close TAG_OPEN, TAG_CLOSE or TAG_OPENCLOSE elements
			if(closeNode) {
				gpk_necall(xmlCloseElement(reader, reader.StateRead.IndexCurrentChar + 1), "Invalid nest level: %i", reader.StateRead.NestLevel);
			}
		}
	}
		break;
	case '?': // close tag if TOKEN_PI
		if(reader.StateRead.CurrentElement->Type != ::gpk::XML_TOKEN_PI)
			error_printf("Invalid character: %i (%c)", currentChar, currentChar);
		else {
			if(reader.StateRead.IndexCurrentChar >= xmlDoc.size() - 1)
				error_printf("Invalid character at EOF: %i (%c)", currentChar, currentChar);
			else {
				if(xmlDoc[reader.StateRead.IndexCurrentChar + 1] != '>')
					error_printf("Invalid character: %i (%c)", currentChar, currentChar);
				else {
					gpk_necs(xmlCloseElement(reader, reader.StateRead.IndexCurrentChar + 2)); // close PI tag
					gpk_necs(xmlCloseElement(reader, reader.StateRead.IndexCurrentChar + 2)); // close TAG_NODE tag
					++reader.StateRead.IndexCurrentChar;
				}
			}
		}
		break;
	}
	return 0;
}

static	::gpk::error_t	xmlParseNameCharacter		(::gpk::SXMLReader& reader, ::gpk::vcc xmlDoc) {
	const sc_t				& currentChar				= xmlDoc[reader.StateRead.IndexCurrentChar];
	xml_info_printf("Parsing character %i '%c' at index %i for token '%s'.", currentChar, currentChar, reader.StateRead.IndexCurrentChar, ::gpk::get_value_label(reader.StateRead.CurrentElement->Type).begin());
	if(::gpk::isSpaceCharacter(currentChar)) {
		gpk_necall(xmlCloseElement(reader), "Invalid nest level: %i", reader.StateRead.NestLevel);
		return 0;
	}
	switch(currentChar) {
	default: break;
	case '/': // maybe it's a close tag
		// if it is, then we need to remove the node-open-name elements and replace them
		if(reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_TAG_NAME) {
			if(reader.StateRead.CurrentElement->Range.Offset == reader.StateRead.IndexCurrentChar) { // test if it's the first character after '<'
				reader.StateRead.CurrentElement				= &reader.Token[reader.StateRead.IndexCurrentElement = reader.Token.pop_back() - 1];
				reader.StateRead.CurrentElement				= &reader.Token[reader.StateRead.IndexCurrentElement = reader.Token.pop_back() - 1];
				reader.StateRead.NestLevel					-= 2;
				reader.StateRead.CurrentElement->Type		= ::gpk::XML_TOKEN_TAG_CLOSE;
				gpk_necs(xmlOpenElement(reader, ::gpk::XML_TOKEN_TAG_NAME, reader.StateRead.IndexCurrentChar + 1));
			}
			else {
				const uint32_t			nextIdx					= reader.StateRead.IndexCurrentChar + 1;
				if(nextIdx < xmlDoc.size() && xmlDoc[nextIdx] == '>') // maybe it's an open tag that is also a close tag
					reader.Token[reader.StateRead.CurrentElement->Parent].Type		= ::gpk::XML_TOKEN_TAG_OPENCLOSE;
			}
		}
		else if(reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_TAG_OPEN) {
			const uint32_t			nextIdx					= reader.StateRead.IndexCurrentChar + 1;
			if(nextIdx < xmlDoc.size() && xmlDoc[nextIdx] == '>') // maybe it's an open tag that is also a close tag
				reader.StateRead.CurrentElement->Type	= ::gpk::XML_TOKEN_TAG_OPENCLOSE;
		}
		break;
	case '=':
		gpk_necall(xmlCloseElement(reader), "Invalid nest level: %i", reader.StateRead.NestLevel);
		break;
	case '?':
		if(reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_TAG_NAME && reader.StateRead.CurrentElement->Range.Offset == reader.StateRead.IndexCurrentChar) {
			reader.Token[reader.StateRead.CurrentElement->Parent].Type		= ::gpk::XML_TOKEN_PI;
			reader.StateRead.CurrentElement->Type								= ::gpk::XML_TOKEN_PI_NAME;
			break;
		}
		return -1;
	case '>': // close tag name and tag. Also close tag node if CurrentElement is a TAG_CLOSE
		stacxpr	::gpk::array_static<const ::gpk::XML_TOKEN, 5> validScopes =
			{ ::gpk::XML_TOKEN_TAG_NODE
			, ::gpk::XML_TOKEN_TAG_OPEN
			, ::gpk::XML_TOKEN_TAG_CLOSE
			, ::gpk::XML_TOKEN_TAG_OPENCLOSE
			, ::gpk::XML_TOKEN_TAG_NAME
			};
		gpk_necall(gpk::find<const ::gpk::XML_TOKEN>(reader.StateRead.CurrentElement->Type, validScopes), "Invalid character: %c in node of type '%s'", currentChar, ::gpk::get_value_label(reader.StateRead.CurrentElement->Type).begin());
		{
			gpk_necall(xmlCloseElement(reader), "Invalid nest level: %i", reader.StateRead.NestLevel); // close TAG_NAME
			bool					isClose					= reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_TAG_CLOSE || reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_TAG_OPENCLOSE;
			if (isClose ||	reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_TAG_OPEN) {
				gpk_necall(xmlCloseElement(reader, reader.StateRead.IndexCurrentChar + 1), "Invalid nest level: %i", reader.StateRead.NestLevel); // close TAG_OPEN, TAG_CLOSE or TAG_OPENCLOSE
				if(isClose) {
					gpk_necall(xmlCloseElement(reader, reader.StateRead.IndexCurrentChar + 1), "Invalid nest level: %i", reader.StateRead.NestLevel); // close TAG_NODE
				}
			} 
			if(reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_TAG_NODE) {
				gpk_necs(xmlOpenElement(reader, ::gpk::XML_TOKEN_TAG_TEXT, reader.StateRead.IndexCurrentChar + 1));
			}
		}
		break;
	}
	return 0;
}

static	::gpk::error_t	xmlParseDocumentCharacter	(::gpk::SXMLReader& reader, ::gpk::vcc xmlDoc) {
	const sc_t				& currentChar				= xmlDoc[reader.StateRead.IndexCurrentChar];
	xml_info_printf("Parsing character %i '%c' at index %i for token '%s'.", currentChar, currentChar, reader.StateRead.IndexCurrentChar, ::gpk::get_value_label(reader.StateRead.CurrentElement->Type).begin());
	switch(currentChar) {
	default: {
		::gpk::array_static<const ::gpk::XML_TOKEN, 9>	validScopes		=
			{ ::gpk::XML_TOKEN_DOCUMENT
			, ::gpk::XML_TOKEN_COMMENT
			, ::gpk::XML_TOKEN_PI
			, ::gpk::XML_TOKEN_CDATA
			, ::gpk::XML_TOKEN_TAG_NODE
			, ::gpk::XML_TOKEN_TAG_NAME
			, ::gpk::XML_TOKEN_ATTR
			, ::gpk::XML_TOKEN_ATTR_VALUE
			, ::gpk::XML_TOKEN_TAG_TEXT
			};
		gpk_necall(gpk::find<const ::gpk::XML_TOKEN>(reader.StateRead.CurrentElement->Type, validScopes), "Invalid character: %c in node of type '%s'", currentChar, ::gpk::get_value_label(reader.StateRead.CurrentElement->Type).begin());
		if( reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_DOCUMENT) {
			gpk_necall(xmlOpenElement(reader, ::gpk::XML_TOKEN_TAG_TEXT), "Invalid nest level: %i", reader.StateRead.NestLevel);
		}
		else if( ::gpk::XML_TOKEN_PI	== reader.StateRead.CurrentElement->Type
		 || ::gpk::XML_TOKEN_DOCTYPE	== reader.StateRead.CurrentElement->Type
		 || ::gpk::XML_TOKEN_TAG_OPEN	== reader.StateRead.CurrentElement->Type
		) {
			gpk_necs(xmlOpenElement(reader, ::gpk::XML_TOKEN_ATTR));
			gpk_necs(xmlOpenElement(reader, ::gpk::XML_TOKEN_ATTR_NAME));
		}
	}
		break;
	case '\r': case '\n': case '\t': case '\b': case ' ': {
		stacxpr	::gpk::array_static<const ::gpk::XML_TOKEN, 11> validScopes =
			{ ::gpk::XML_TOKEN_DOCUMENT
			, ::gpk::XML_TOKEN_COMMENT
			, ::gpk::XML_TOKEN_PI
			, ::gpk::XML_TOKEN_CDATA
			, ::gpk::XML_TOKEN_TAG_NODE
			, ::gpk::XML_TOKEN_TAG_OPEN
			, ::gpk::XML_TOKEN_TAG_CLOSE
			, ::gpk::XML_TOKEN_TAG_TEXT
			, ::gpk::XML_TOKEN_TAG_NAME
			, ::gpk::XML_TOKEN_ATTR
			, ::gpk::XML_TOKEN_ATTR_VALUE
			};
		gpk_necall(gpk::find<const ::gpk::XML_TOKEN>(reader.StateRead.CurrentElement->Type, validScopes), "Invalid character: %c in node of type '%s'", currentChar, ::gpk::get_value_label(reader.StateRead.CurrentElement->Type).begin());
		if( reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_DOCUMENT) {
			gpk_necall(xmlOpenElement(reader, ::gpk::XML_TOKEN_TAG_TEXT), "Invalid nest level: %i", reader.StateRead.NestLevel);
		}
		::gpk::skipToNextCharacter(reader.StateRead.IndexCurrentChar, xmlDoc);
		--reader.StateRead.IndexCurrentChar;
	}
		break;
	case '<': {
		stacxpr	::gpk::array_static<const ::gpk::XML_TOKEN, 4> validScopes =
			{ ::gpk::XML_TOKEN_COMMENT
			, ::gpk::XML_TOKEN_DOCUMENT
			, ::gpk::XML_TOKEN_TAG_TEXT
			, ::gpk::XML_TOKEN_TAG_NODE
			};
		gpk_necall(gpk::find<const ::gpk::XML_TOKEN>(reader.StateRead.CurrentElement->Type, validScopes), "Invalid character: %c in node of type '%s'", currentChar, ::gpk::get_value_label(reader.StateRead.CurrentElement->Type).begin());
		if(reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_TAG_TEXT)
			gpk_necs(xmlCloseElement(reader));
		gpk_necs(xmlOpenElement(reader, ::gpk::XML_TOKEN_TAG_NODE));
		gpk_necs(xmlOpenElement(reader, ::gpk::XML_TOKEN_TAG_OPEN));
		gpk_necs(xmlOpenElement(reader, ::gpk::XML_TOKEN_TAG_NAME, reader.StateRead.IndexCurrentChar + 1));
	}
		break;
	case '>': { // close (pi, open, close, doctype) tag
		constexpr ::gpk::array_static<const ::gpk::XML_TOKEN, 3> validScopes =
			{ ::gpk::XML_TOKEN_TAG_OPEN
			, ::gpk::XML_TOKEN_TAG_CLOSE
			, ::gpk::XML_TOKEN_TAG_OPENCLOSE
			};
		gpk_necall(gpk::find<const ::gpk::XML_TOKEN>(reader.StateRead.CurrentElement->Type, validScopes), "Invalid character: %c in node of type '%s'", currentChar, ::gpk::get_value_label(reader.StateRead.CurrentElement->Type).begin());

		{
			bool isClose =	reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_TAG_CLOSE || reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_TAG_OPENCLOSE;
			gpk_necall(xmlCloseElement(reader, reader.StateRead.IndexCurrentChar + 1), "Invalid nest level: %i", reader.StateRead.NestLevel); // close TAG_OPEN, TAG_CLOSE or TAG_OPENCLOSE
			if(isClose) {
				gpk_necall(xmlCloseElement(reader, reader.StateRead.IndexCurrentChar + 1), "Invalid nest level: %i", reader.StateRead.NestLevel); // close TAG_NODE
			}
			else if(reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_TAG_NODE) {
				gpk_necs(xmlOpenElement(reader, ::gpk::XML_TOKEN_TAG_TEXT, reader.StateRead.IndexCurrentChar + 1));
			}
		}
	}
		break;
	case '#': break;
	case '&': break;
	case '"': break;
	}
	return 0;
}

::gpk::error_t			gpk::xmlParseCharacter	(::gpk::SXMLReader& reader, ::gpk::vcc xmlDoc)	{
	stacxpr	::gpk::array_static<const ::gpk::XML_TOKEN, 4> TOKENS_CHARACTER_TAG	= {::gpk::XML_TOKEN_TAG_OPEN, ::gpk::XML_TOKEN_TAG_CLOSE, ::gpk::XML_TOKEN_PI, ::gpk::XML_TOKEN_ATTR};
	stacxpr	::gpk::array_static<const ::gpk::XML_TOKEN, 3> TOKENS_CHARACTER_NAME	= {::gpk::XML_TOKEN_ATTR_NAME, ::gpk::XML_TOKEN_TAG_NAME, ::gpk::XML_TOKEN_PI_NAME};
	if(0 == reader.StateRead.CurrentElement) {
		gpk_necs(xmlOpenElement(reader, ::gpk::XML_TOKEN_DOCUMENT));
	}

	if(reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_ATTR_VALUE)
		gpk_necs(xmlParseStringCharacter(reader, xmlDoc));
	else if(-1 != ::gpk::find<const ::gpk::XML_TOKEN>(reader.StateRead.CurrentElement->Type, TOKENS_CHARACTER_TAG))
		gpk_necs(xmlParseTagCharacter(reader, xmlDoc));
	else if(-1 != ::gpk::find<const ::gpk::XML_TOKEN>(reader.StateRead.CurrentElement->Type, TOKENS_CHARACTER_NAME))
		gpk_necs(xmlParseNameCharacter(reader, xmlDoc));
	else {
		gpk_necs(xmlParseDocumentCharacter(reader, xmlDoc));
	}
	if(xmlDoc.size() == (reader.StateRead.IndexCurrentChar + 1) && reader.StateRead.CurrentElement) {
		if(reader.StateRead.CurrentElement && reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_TAG_TEXT)
			xmlCloseElement(reader);
		if(reader.StateRead.CurrentElement && reader.StateRead.CurrentElement->Type == ::gpk::XML_TOKEN_DOCUMENT)
			xmlCloseElement(reader, reader.StateRead.IndexCurrentChar + 1);
	}
	return 0;
}

::gpk::error_t			gpk::xmlParse			(::gpk::SXMLReader& reader, ::gpk::vcc xmlDoc)	{
	for(reader.StateRead.IndexCurrentChar = 0; reader.StateRead.IndexCurrentChar < xmlDoc.size(); ++reader.StateRead.IndexCurrentChar) {
		if (errored(::gpk::xmlParseCharacter(reader, xmlDoc))) {
			error_printf("Failed to parse XML:"
				"\niCharacter		: %i ('%c')"
				"\niNestLevel		: %i"
				"\nToken type		: '%s'"
				"\nToken start		: %i"
				"\nToken length		: %i"
				"\nToken parent		: %i"
				, reader.StateRead.IndexCurrentChar, xmlDoc[reader.StateRead.IndexCurrentChar]
				, reader.StateRead.NestLevel
				, reader.StateRead.CurrentElement	? ::gpk::get_value_label(reader.StateRead.CurrentElement->Type).begin() : ""
				, reader.StateRead.CurrentElement	? reader.StateRead.CurrentElement->Range.Offset : 0
				, reader.StateRead.CurrentElement	? reader.StateRead.CurrentElement->Range.Count  : 0
				, reader.StateRead.CurrentElement	? reader.StateRead.CurrentElement->Parent  : 0
				);
			break;
		}
	}
	ree_if(reader.StateRead.NestLevel, "Invalid nest level: %i", reader.StateRead.NestLevel);
	return 0;
}

