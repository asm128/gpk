#define GPK_DISABLE_DEBUG_BREAK_ON_ERROR_LOG

#include "gpk_xml_reader.h"
#include "gpk_file.h"
#include "gpk_parse.h"

GPK_USING_TYPEINT();

#define xml_info_printf(...) // info_printf

sttc bool xmlTextEquals(::gpk::vcst_t left, ::gpk::vcst_t right) {
	rtrn left.size() == right.size() && (0 == left.size() || 0 == memcmp(left.begin(), right.begin(), left.size()));
}

sttc bool xmlStartsWith(::gpk::vcst_t xmlDoc, ::gpk::u2_t offset, ::gpk::vcst_t text) {
	rtrn offset <= xmlDoc.size() && text.size() <= xmlDoc.size() - offset && 0 == memcmp(&xmlDoc[offset], text.begin(), text.size());
}

sttc ::gpk::err_t xmlTokenAdd(::gpk::SXMLReader & reader, ::gpk::XML_TOKEN type, ::gpk::u2_t offset, ::gpk::u2_t count, ::gpk::s2_t parent) {
	rtrn reader.Token.push_back({type, {offset, count}, parent});
}

sttc ::gpk::err_t xmlNameEnd(::gpk::vcst_t xmlDoc, ::gpk::u2_t offset, ::gpk::u2_t & end) {
	end						= offset;
	while(end < xmlDoc.size()) {
		cnst ::gpk::sc_t current	= xmlDoc[end];
		if(::gpk::isSpaceCharacter(current) || current == '/' || current == '>' || current == '?' || current == '=')
			break;
		++end;
	}
	if_true_fef(end == offset, "Missing XML name at offset:%u.", offset);
	rtrn 0;
}

sttc ::gpk::err_t xmlSkipSpaces(::gpk::vcst_t xmlDoc, ::gpk::u2_t & offset) {
	while(offset < xmlDoc.size() && ::gpk::isSpaceCharacter(xmlDoc[offset]))
		++offset;
	rtrn offset;
}

sttc ::gpk::err_t xmlAttributesParse
	(::gpk::SXMLReader & reader, ::gpk::vcst_t xmlDoc, ::gpk::u2_t iOwner, ::gpk::u2_t & offset, bool processingInstruction, bool & selfClosing) {
	selfClosing					= false;
	while(offset < xmlDoc.size()) {
		if_fail_fe(::xmlSkipSpaces(xmlDoc, offset));
		if(processingInstruction) {
			if(::xmlStartsWith(xmlDoc, offset, GPK_CXS("?>"))) {
				offset					+= 2;
				rtrn 0;
			}
		}
		else {
			if(::xmlStartsWith(xmlDoc, offset, GPK_CXS("/>"))) {
				selfClosing				= true;
				offset					+= 2;
				rtrn 0;
			}
			if(offset < xmlDoc.size() && xmlDoc[offset] == '>') {
				++offset;
				rtrn 0;
			}
		}

		cnst ::gpk::u2_t iAttribute	= offset;
		::gpk::u2_t iNameEnd			= {};
		if_fail_fe(::xmlNameEnd(xmlDoc, offset, iNameEnd));
		offset						= iNameEnd;
		if_fail_fe(::xmlSkipSpaces(xmlDoc, offset));
		if_true_fef(offset >= xmlDoc.size() || xmlDoc[offset] != '=', "Missing '=' after XML attribute at offset:%u.", iAttribute);
		if_fail_fe(::xmlSkipSpaces(xmlDoc, ++offset));
		if_true_fef(offset >= xmlDoc.size() || (xmlDoc[offset] != '\"' && xmlDoc[offset] != '\''), "Missing quote for XML attribute at offset:%u.", iAttribute);
		cnst ::gpk::sc_t quote			= xmlDoc[offset++];
		cnst ::gpk::u2_t iValue		= offset;
		while(offset < xmlDoc.size() && xmlDoc[offset] != quote)
			++offset;
		if_true_fef(offset >= xmlDoc.size(), "Unclosed XML attribute at offset:%u.", iAttribute);
		cnst ::gpk::u2_t iAttributeEnd	= ++offset;
		cnst ::gpk::err_t iToken		= ::xmlTokenAdd(reader, ::gpk::XML_TOKEN_ATTR, iAttribute, iAttributeEnd - iAttribute, (::gpk::s2_t)iOwner);
		if_fail_fe(iToken);
		if_fail_fe(::xmlTokenAdd(reader, ::gpk::XML_TOKEN_ATTR_NAME , iAttribute, iNameEnd - iAttribute, iToken));
		if_fail_fe(::xmlTokenAdd(reader, ::gpk::XML_TOKEN_ATTR_VALUE, iValue, iAttributeEnd - iValue - 1, iToken));
	}
	rtrn -1;
}

sttc ::gpk::err_t xmlOpeningName(cnst ::gpk::SXMLReader & reader, ::gpk::vcst_t xmlDoc, ::gpk::u2_t iNode, ::gpk::vcst_t & output) {
	for(::gpk::u2_t iToken = iNode + 1; iToken < reader.Token.size(); ++iToken) {
		cnst ::gpk::SXMLToken & token	= reader.Token[iToken];
		if(token.Parent == (::gpk::s2_t)iNode && (token.Type == ::gpk::XML_TOKEN_TAG_OPEN || token.Type == ::gpk::XML_TOKEN_TAG_OPENCLOSE)) {
			for(::gpk::u2_t iName = iToken + 1; iName < reader.Token.size(); ++iName)
				if(reader.Token[iName].Parent == (::gpk::s2_t)iToken && reader.Token[iName].Type == ::gpk::XML_TOKEN_TAG_NAME)
					rtrn ::gpk::xmlTokenView(xmlDoc, reader.Token[iName], output) < 0 ? -1 : (::gpk::err_t)iName;
		}
	}
	rtrn -1;
}

sttc ::gpk::err_t xmlSpecialParse(::gpk::SXMLReader & reader, ::gpk::vcst_t xmlDoc, ::gpk::u2_t iParent, ::gpk::u2_t & offset) {
	cnst ::gpk::u2_t iBegin		= offset;
	::gpk::XML_TOKEN type			= ::gpk::XML_TOKEN_DOCTYPE;
	::gpk::vcst_t terminator		= GPK_CXS(">");
	if(::xmlStartsWith(xmlDoc, offset, GPK_CXS("<!--"))) {
		type						= ::gpk::XML_TOKEN_COMMENT;
		terminator					= GPK_CXS("-->");
		offset						+= 4;
	}
	else if(::xmlStartsWith(xmlDoc, offset, GPK_CXS("<![CDATA["))) {
		type						= ::gpk::XML_TOKEN_CDATA;
		terminator					= GPK_CXS("]]>");
		offset						+= 9;
	}
	else if(::xmlStartsWith(xmlDoc, offset, GPK_CXS("<!DOCTYPE")))
		offset						+= 9;
	else
		rtrn -1;
	cnst ::gpk::err_t iEnd			= xmlDoc.find(terminator, offset);
	if_fail_fef(iEnd, "Unclosed XML special token at offset:%u.", iBegin);
	offset						= (::gpk::u2_t)iEnd + terminator.size();
	rtrn ::xmlTokenAdd(reader, type, iBegin, offset - iBegin, (::gpk::s2_t)iParent) < 0 ? -1 : 0;
}

gpk::err_t gpk::xmlTokenView(::gpk::vcst_t xmlDoc, cnst ::gpk::SXMLToken & token, ::gpk::vcst_t & output) {
	if_true_fef(token.Range.Offset > xmlDoc.size() || token.Range.Count > xmlDoc.size() - token.Range.Offset, "XML token range outside document. offset:%u, count:%u, size:%u.", token.Range.Offset, token.Range.Count, xmlDoc.size());
	rtrn xmlDoc.slice(output, token.Range.Offset, token.Range.Count);
}

gpk::err_t gpk::xmlNodeName(cnst ::gpk::SXMLReader & reader, ::gpk::vcst_t xmlDoc, ::gpk::u2_t iNode, ::gpk::vcst_t & output) {
	if_true_fef(iNode >= reader.Token.size() || reader.Token[iNode].Type != ::gpk::XML_TOKEN_TAG_NODE, "Invalid XML node index:%u.", iNode);
	rtrn ::xmlOpeningName(reader, xmlDoc, iNode, output);
}

gpk::err_t gpk::xmlNodeAttribute(cnst ::gpk::SXMLReader & reader, ::gpk::vcst_t xmlDoc, ::gpk::u2_t iNode, ::gpk::vcst_t name, ::gpk::vcst_t & output) {
	if_true_fef(iNode >= reader.Token.size() || reader.Token[iNode].Type != ::gpk::XML_TOKEN_TAG_NODE, "Invalid XML node index:%u.", iNode);
	for(::gpk::u2_t iToken = iNode + 1; iToken < reader.Token.size(); ++iToken) {
		cnst ::gpk::SXMLToken & owner	= reader.Token[iToken];
		if(owner.Parent != (::gpk::s2_t)iNode || (owner.Type != ::gpk::XML_TOKEN_TAG_OPEN && owner.Type != ::gpk::XML_TOKEN_TAG_OPENCLOSE))
			continue;
		for(::gpk::u2_t iAttribute = iToken + 1; iAttribute < reader.Token.size(); ++iAttribute) {
			cnst ::gpk::SXMLToken & attribute = reader.Token[iAttribute];
			if(attribute.Parent != (::gpk::s2_t)iToken || attribute.Type != ::gpk::XML_TOKEN_ATTR)
				continue;
			::gpk::vcst_t attributeName;
			::gpk::vcst_t attributeValue;
			::gpk::err_t iValue			= -1;
			for(::gpk::u2_t iPart = iAttribute + 1; iPart < reader.Token.size(); ++iPart) {
				cnst ::gpk::SXMLToken & part = reader.Token[iPart];
				if(part.Parent != (::gpk::s2_t)iAttribute)
					continue;
				if(part.Type == ::gpk::XML_TOKEN_ATTR_NAME ) if_fail_fe(::gpk::xmlTokenView(xmlDoc, part, attributeName));
				if(part.Type == ::gpk::XML_TOKEN_ATTR_VALUE) { if_fail_fe(::gpk::xmlTokenView(xmlDoc, part, attributeValue)); iValue = iPart; }
			}
			if(::xmlTextEquals(attributeName, name)) {
				output					= attributeValue;
				rtrn iValue;
			}
		}
	}
	rtrn -1;
}

gpk::err_t gpk::xmlNodeChild(cnst ::gpk::SXMLReader & reader, ::gpk::vcst_t xmlDoc, ::gpk::u2_t iNode, ::gpk::vcst_t name) {
	if_true_fef(iNode >= reader.Token.size(), "Invalid XML parent index:%u.", iNode);
	for(::gpk::u2_t iToken = iNode + 1; iToken < reader.Token.size(); ++iToken) {
		cnst ::gpk::SXMLToken & token	= reader.Token[iToken];
		if(token.Parent != (::gpk::s2_t)iNode || token.Type != ::gpk::XML_TOKEN_TAG_NODE)
			continue;
		::gpk::vcst_t nodeName;
		if(0 <= ::gpk::xmlNodeName(reader, xmlDoc, iToken, nodeName) && ::xmlTextEquals(nodeName, name))
			rtrn iToken;
	}
	rtrn -1;
}

gpk::err_t gpk::xmlNodeText(cnst ::gpk::SXMLReader & reader, ::gpk::vcst_t xmlDoc, ::gpk::u2_t iNode, ::gpk::vcst_t & output) {
	if_true_fef(iNode >= reader.Token.size(), "Invalid XML parent index:%u.", iNode);
	for(::gpk::u2_t iToken = iNode + 1; iToken < reader.Token.size(); ++iToken)
		if(reader.Token[iToken].Parent == (::gpk::s2_t)iNode && reader.Token[iToken].Type == ::gpk::XML_TOKEN_TAG_TEXT)
			rtrn ::gpk::xmlTokenView(xmlDoc, reader.Token[iToken], output) < 0 ? -1 : (::gpk::err_t)iToken;
	rtrn -1;
}

gpk::err_t gpk::xmlFileRead(::gpk::SXMLFile & file, ::gpk::vcst_t filename) {
	xml_info_printf("Loading xml file: %.*s.", (int)filename.size(), filename.begin());
	if_fail_fef(::gpk::fileToMemory(filename, file.Bytes), "Failed to load XML file:'%.*s'.", (int)filename.size(), filename.begin());
	rtrn ::gpk::xmlParse(file.Reader, file.Bytes);
}

gpk::err_t gpk::xmlParse(::gpk::SXMLReader & reader, ::gpk::vcst_t xmlDoc) {
	if_fail_fe(reader.Reset());
	cnst ::gpk::err_t iDocument	= ::xmlTokenAdd(reader, ::gpk::XML_TOKEN_DOCUMENT, 0, xmlDoc.size(), -1);
	if_fail_fe(iDocument);
	::gpk::u2_t iCurrentNode		= (::gpk::u2_t)iDocument;
	::gpk::u2_t offset				= 0;
	while(offset < xmlDoc.size()) {
		reader.StateRead.IndexCurrentChar = offset;
		reader.StateRead.CharCurrent	= xmlDoc[offset];
		if(xmlDoc[offset] != '<') {
			cnst ::gpk::u2_t iText		= offset;
			while(offset < xmlDoc.size() && xmlDoc[offset] != '<')
				++offset;
			if(offset > iText)
				if_fail_fe(::xmlTokenAdd(reader, ::gpk::XML_TOKEN_TAG_TEXT, iText, offset - iText, (::gpk::s2_t)iCurrentNode));
			continue;
		}
		if(::xmlStartsWith(xmlDoc, offset, GPK_CXS("<!"))) {
			if_fail_fef(::xmlSpecialParse(reader, xmlDoc, iCurrentNode, offset), "Invalid XML special token at offset:%u.", offset);
			continue;
		}
		if(::xmlStartsWith(xmlDoc, offset, GPK_CXS("<?"))) {
			cnst ::gpk::u2_t iBegin	= offset;
			offset					+= 2;
			::gpk::u2_t iNameEnd		= {};
			if_fail_fe(::xmlNameEnd(xmlDoc, offset, iNameEnd));
			cnst ::gpk::err_t iPI		= ::xmlTokenAdd(reader, ::gpk::XML_TOKEN_PI, iBegin, 0, (::gpk::s2_t)iCurrentNode);
			if_fail_fe(iPI);
			if_fail_fe(::xmlTokenAdd(reader, ::gpk::XML_TOKEN_PI_NAME, offset, iNameEnd - offset, iPI));
			offset					= iNameEnd;
			bool ignored				= false;
			if_fail_fe(::xmlAttributesParse(reader, xmlDoc, iPI, offset, true, ignored));
			reader.Token[iPI].Range.Count = offset - iBegin;
			continue;
		}
		if(::xmlStartsWith(xmlDoc, offset, GPK_CXS("</"))) {
			if_true_fef(iCurrentNode == (::gpk::u2_t)iDocument, "Closing XML tag without an open node at offset:%u.", offset);
			cnst ::gpk::u2_t iBegin	= offset;
			offset					+= 2;
			::gpk::u2_t iNameEnd		= {};
			if_fail_fe(::xmlNameEnd(xmlDoc, offset, iNameEnd));
			::gpk::vcst_t openingName;
			if_fail_fef(::xmlOpeningName(reader, xmlDoc, iCurrentNode, openingName), "Open XML node has no name at token:%u.", iCurrentNode);
			::gpk::vcst_t closingName	= {&xmlDoc[offset], iNameEnd - offset};
			if_true_fef(false == ::xmlTextEquals(openingName, closingName), "Mismatched XML closing tag:'%.*s'; expected:'%.*s'.", (int)closingName.size(), closingName.begin(), (int)openingName.size(), openingName.begin());
			offset					= iNameEnd;
			if_fail_fe(::xmlSkipSpaces(xmlDoc, offset));
			if_true_fef(offset >= xmlDoc.size() || xmlDoc[offset] != '>', "Missing '>' for closing XML tag at offset:%u.", iBegin);
			++offset;
			cnst ::gpk::err_t iClose	= ::xmlTokenAdd(reader, ::gpk::XML_TOKEN_TAG_CLOSE, iBegin, offset - iBegin, (::gpk::s2_t)iCurrentNode);
			if_fail_fe(iClose);
			if_fail_fe(::xmlTokenAdd(reader, ::gpk::XML_TOKEN_TAG_NAME, iBegin + 2, iNameEnd - iBegin - 2, iClose));
			reader.Token[iCurrentNode].Range.Count = offset - reader.Token[iCurrentNode].Range.Offset;
			iCurrentNode				= (::gpk::u2_t)reader.Token[iCurrentNode].Parent;
			--reader.StateRead.NestLevel;
			continue;
		}

		cnst ::gpk::u2_t iBegin		= offset++;
		::gpk::u2_t iNameEnd			= {};
		if_fail_fe(::xmlNameEnd(xmlDoc, offset, iNameEnd));
		cnst ::gpk::err_t iNode		= ::xmlTokenAdd(reader, ::gpk::XML_TOKEN_TAG_NODE, iBegin, 0, (::gpk::s2_t)iCurrentNode);
		if_fail_fe(iNode);
		cnst ::gpk::err_t iOpen		= ::xmlTokenAdd(reader, ::gpk::XML_TOKEN_TAG_OPEN, iBegin, 0, iNode);
		if_fail_fe(iOpen);
		if_fail_fe(::xmlTokenAdd(reader, ::gpk::XML_TOKEN_TAG_NAME, offset, iNameEnd - offset, iOpen));
		offset						= iNameEnd;
		bool selfClosing				= false;
		if_fail_fe(::xmlAttributesParse(reader, xmlDoc, iOpen, offset, false, selfClosing));
		reader.Token[iOpen].Range.Count = offset - iBegin;
		if(selfClosing) {
			reader.Token[iOpen].Type	= ::gpk::XML_TOKEN_TAG_OPENCLOSE;
			reader.Token[iNode].Range.Count = offset - iBegin;
		}
		else {
			iCurrentNode				= (::gpk::u2_t)iNode;
			++reader.StateRead.NestLevel;
		}
	}
	if_true_fef(iCurrentNode != (::gpk::u2_t)iDocument, "Unclosed XML node at token:%u.", iCurrentNode);
	reader.StateRead.IndexCurrentChar = xmlDoc.size();
	reader.StateRead.IndexCurrentElement = -1;
	reader.StateRead.CurrentElement = 0;
	reader.StateRead.NestLevel	= 0;
	reader.StateRead.CharCurrent	= 0;
	rtrn reader.Token.size();
}


