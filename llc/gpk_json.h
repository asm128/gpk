#include "gpk_array.h"
#include "gpk_coord.h"
#include "gpk_ptr.h"
#include "gpk_enum.h"

#ifndef GPK_JSON_H_92749028348923
#define GPK_JSON_H_92749028348923

namespace gpk
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE(JSON_TYPE, int8_t);
	GDEFINE_ENUM_VALUE(JSON_TYPE, NULL		,  0);
	GDEFINE_ENUM_VALUE(JSON_TYPE, NUMBER	,  1);
	GDEFINE_ENUM_VALUE(JSON_TYPE, BOOL		,  2);
	GDEFINE_ENUM_VALUE(JSON_TYPE, STRING	,  4);
	GDEFINE_ENUM_VALUE(JSON_TYPE, KEY		,  5);
	GDEFINE_ENUM_VALUE(JSON_TYPE, VALUE		,  6);
	GDEFINE_ENUM_VALUE(JSON_TYPE, OBJECT	,  7);
	GDEFINE_ENUM_VALUE(JSON_TYPE, ARRAY		,  8);
	GDEFINE_ENUM_VALUE(JSON_TYPE, CODEPOINT	,  9);
	GDEFINE_ENUM_VALUE(JSON_TYPE, COUNT		, 10);
	GDEFINE_ENUM_VALUE(JSON_TYPE, UNKNOWN	, -1);
	struct SJSONToken {
								int32_t														ParentIndex;
								JSON_TYPE													Type;
								::gpk::SSlice<uint32_t>										Span;
	};

	struct SJSONNode {
								SJSONToken													* Token					= 0;
								SJSONNode													* Parent				= 0;
								::gpk::array_obj<::gpk::ptr_obj<SJSONNode>>					Children				= 0;
								int32_t														ObjectIndex				= -1;
	};

	struct SJSONReaderState {
								uint32_t													IndexCurrentChar		= 0;
								int32_t														IndexCurrentElement		= -1;
								::gpk::SJSONToken											* CurrentElement		= 0;
								int32_t														NestLevel				= 0;
								char														CharCurrent				= 0;
								bool														Escaping				: 1;
								bool														InsideString			: 1;
								bool														ExpectingSeparator		: 1;
								bool														DoneReading				: 1;

																							SJSONReaderState		()
		: Escaping			(false)
		, InsideString		(false)
		, ExpectingSeparator(false)
		, DoneReading		(false)
		{}
	};
#pragma pack(pop)

	struct SJSONReader {
								::gpk::array_pod<::gpk::SJSONToken>							Object;
								::gpk::array_obj<::gpk::view_const_string>					View;
								::gpk::array_obj<::gpk::ptr_obj<::gpk::SJSONNode>>			Tree;
								::gpk::SJSONReaderState										StateRead;

								const ::gpk::ptr_obj<::gpk::SJSONNode>&						operator[]				(uint32_t index)	const { return Tree[index]; }
	};

	// Reader functions: Populate a SJSONReader structure from an input JSON string.
							::gpk::error_t												jsonParse				(::gpk::SJSONReader& reader, const ::gpk::view_const_string& jsonAsString);
							::gpk::error_t												jsonParseStep			(::gpk::SJSONReader& reader, const ::gpk::view_const_string& jsonAsString);

	// Access functions: Iterate over children looking for keys or values.
							::gpk::error_t												jsonArrayValueGet		(const ::gpk::SJSONNode& node, uint32_t index);	// returns the index of the JSON element corresponding to the index provided as parameter.
	static inline constexpr	::gpk::error_t												jsonArraySize			(const ::gpk::SJSONNode& node)		noexcept	{ return node.Children.size(); }	// returns the index of the JSON element corresponding to the index provided as parameter.

							::gpk::error_t												jsonObjectValueGet		(const ::gpk::SJSONNode& node, const ::gpk::view_array<::gpk::view_const_string>& views, const ::gpk::view_const_string & key);
							::gpk::error_t												jsonObjectKeyList		(const ::gpk::SJSONNode& node, const ::gpk::view_array<::gpk::view_const_string>& views, ::gpk::array_obj<int32_t> & indices, ::gpk::array_obj<::gpk::view_const_string> & keys);
	static inline constexpr	::gpk::error_t												jsonObjectKeyCount		(const ::gpk::SJSONNode& node)		noexcept	{ return node.Children.size() / 2; }

	struct SJSONFile {
								::gpk::array_pod<char_t>									Bytes					= {};
								::gpk::SJSONReader											Reader					= {};
	};

	typedef ::gpk::SKeyVal<::gpk::view_const_string, ::gpk::SJSONFile>					TKeyValJSONFile;

							::gpk::error_t												jsonFileRead			(::gpk::SJSONFile & file, const ::gpk::view_const_string & filename);
							::gpk::error_t												jsonWrite				(const ::gpk::SJSONNode* node, const ::gpk::view_array<::gpk::view_const_string> & jsonViews, ::gpk::array_pod<char_t> & output);
							::gpk::error_t												jsonArraySplit			(const ::gpk::SJSONNode & jsonArrayToSplit, const ::gpk::view_array<::gpk::view_const_string> & jsonViews, const uint32_t blockSize, ::gpk::array_obj<::gpk::array_pod<char_t>> & outputJsons);

} // namespace

#endif // GPK_JSON_H_92749028348923
