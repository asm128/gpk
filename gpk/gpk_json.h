#include "gpk_array.h"
#include "gpk_coord.h"
#include "gpk_ptr.h"
#include "gpk_enum.h"

#ifndef GPK_JSON_H_92749028348923
#define GPK_JSON_H_92749028348923

namespace gpk
{
#pragma pack(push, 1)
#if defined(NULL)
#	undef NULL
#	define GPK_REDEFINE_NULL_ASAP
#endif
	GDEFINE_ENUM_TYPE(JSON_TYPE, int8_t);
	GDEFINE_ENUM_VALUE(JSON_TYPE, NULL		,  0);
	GDEFINE_ENUM_VALUE(JSON_TYPE, NUMBER	,  1);
	GDEFINE_ENUM_VALUE(JSON_TYPE, BOOL		,  2);
	GDEFINE_ENUM_VALUE(JSON_TYPE, MEMBER	,  3);
	GDEFINE_ENUM_VALUE(JSON_TYPE, STRING	,  4);
	GDEFINE_ENUM_VALUE(JSON_TYPE, KEY		,  5);
	GDEFINE_ENUM_VALUE(JSON_TYPE, VALUE		,  6);
	GDEFINE_ENUM_VALUE(JSON_TYPE, OBJECT	,  7);
	GDEFINE_ENUM_VALUE(JSON_TYPE, ARRAY		,  8);
	GDEFINE_ENUM_VALUE(JSON_TYPE, CODEPOINT	,  9);
	GDEFINE_ENUM_VALUE(JSON_TYPE, COUNT		, 10);
	GDEFINE_ENUM_VALUE(JSON_TYPE, UNKNOWN	, -1);
#if defined(GPK_REDEFINE_NULL_ASAP)
#	define NULL 0
#endif


	struct SJSONType {
						int32_t											ParentIndex;
						JSON_TYPE										Type;
						::gpk::SSlice<uint32_t>							Span;
	};

	struct SJSONNode {
						SJSONType										* Object				= 0;
						SJSONNode										* Parent				= 0;
						::gpk::array_obj<::gpk::ptr_obj<SJSONNode>>		Children				= 0;
						int32_t											ObjectIndex				= -1;
	};

	struct SJSONReaderState {
						uint32_t										IndexCurrentChar		= 0;
						int32_t											IndexCurrentElement		= -1;
						::gpk::SJSONType								* CurrentElement		= 0;
						int32_t											NestLevel				= 0;
						char											CharCurrent				= 0;
						bool											Escaping				: 1;
						bool											InsideString			: 1;
						bool											ExpectingSeparator		: 1;

																		SJSONReaderState		() 
		: Escaping			(false)
		, InsideString		(false)
		, ExpectingSeparator(false)
		{}
	};
#pragma pack(pop)

	struct SJSONReader {
						::gpk::array_obj<::gpk::SJSONType>				Object;
						::gpk::array_obj<::gpk::view_const_string>		View;
						::gpk::ptr_obj	<::gpk::SJSONNode>				Tree;
						::gpk::SJSONReaderState							StateRead;
	};

	// Reader functions: Populate a SJSONReader structure from an input JSON string.
					::gpk::error_t									jsonParse				(::gpk::SJSONReader& reader, const ::gpk::view_const_string& jsonAsString);
					::gpk::error_t									jsonParseStep			(::gpk::SJSONReader& reader, const ::gpk::view_const_string& jsonAsString);

	// Access functions: Iterate over children looking for keys or values.
					::gpk::error_t									jsonValueGet			(::gpk::SJSONReader& reader, ::gpk::ptr_obj<::gpk::SJSONNode> Tree);


} // namespace

#endif // GPK_JSON_H_92749028348923
