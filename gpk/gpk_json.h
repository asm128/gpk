#include "gpk_array.h"
#include "gpk_coord.h"
#include "gpk_ptr.h"

#ifndef GPK_JSON_H_92749028348923
#define GPK_JSON_H_92749028348923

namespace gpk
{
#pragma pack(push, 1)
	enum JSON_TYPE : int32_t
		{	JSON_TYPE_UNKNOWN		= 0
		,	JSON_TYPE_INT
		,	JSON_TYPE_FLOAT
		,	JSON_TYPE_STRING
		,	JSON_TYPE_OBJECT
		,	JSON_TYPE_ARRAY
		,	JSON_TYPE_COUNT
		,	JSON_TYPE_INVALID		= -1
		};

	struct SJSONType {
						int32_t											ParentIndex;
						JSON_TYPE										Type;
						::gpk::SSlice<uint32_t>							Span;
	};

	struct SJSONNode {
						SJSONType										* Object;
						SJSONNode										* Parent;
						::gpk::array_obj<::gpk::ptr_obj<SJSONNode>>		Children;
						int32_t											ObjectIndex;
	};

	struct SJSONDocument {
						::gpk::array_obj<::gpk::SJSONType>				Object;
	};

					::gpk::error_t									jsonParse				(::gpk::SJSONDocument& document, ::gpk::SJSONNode& jsonTree, const char* jsonAsString, uint32_t jsonLength);
	static inline	::gpk::error_t									jsonParse				(::gpk::SJSONDocument& document, ::gpk::SJSONNode& jsonTree, const ::gpk::view_const_string& jsonAsString)	{ return ::gpk::jsonParse(document, jsonTree, jsonAsString.begin(), jsonAsString.size());			}
	static inline	::gpk::error_t									jsonParse				(::gpk::SJSONDocument& document, ::gpk::SJSONNode& jsonTree, const ::std::string& jsonAsString)				{ return ::gpk::jsonParse(document, jsonTree, jsonAsString.data (), (uint32_t)jsonAsString.size());	}
#pragma pack(pop)
}

#endif // GPK_JSON_H_92749028348923
