#include "gpk_enum.h"
#include "gpk_coord.h"
#include "gpk_ptr.h"

#ifndef GPK_JSON_H_92749028348923
#define GPK_JSON_H_92749028348923

namespace gpk
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE(JSON_TYPE, int32_t);
	GDEFINE_ENUM_VALUE(JSON_TYPE, NULL		,  0);
	GDEFINE_ENUM_VALUE(JSON_TYPE, INTEGER	,  1);
	GDEFINE_ENUM_VALUE(JSON_TYPE, DOUBLE	,  2);
	GDEFINE_ENUM_VALUE(JSON_TYPE, BOOL		,  3);
	GDEFINE_ENUM_VALUE(JSON_TYPE, STRING	,  4);
	GDEFINE_ENUM_VALUE(JSON_TYPE, KEY		,  5);
	GDEFINE_ENUM_VALUE(JSON_TYPE, VALUE		,  6);
	GDEFINE_ENUM_VALUE(JSON_TYPE, OBJECT	,  7);
	GDEFINE_ENUM_VALUE(JSON_TYPE, ARRAY		,  8);
	GDEFINE_ENUM_VALUE(JSON_TYPE, CODEPOINT	,  9);
	GDEFINE_ENUM_VALUE(JSON_TYPE, COUNT		, 10);
	GDEFINE_ENUM_VALUE(JSON_TYPE, UNKNOWN	, -1);

	struct SJSONToken {
								int32_t													ParentIndex;
								JSON_TYPE												Type;
								::gpk::SSlice<uint32_t>									Span;
								uint64_t												Value;
	};

	struct SJSONNode {
								SJSONToken												* Token					= 0;
								SJSONNode												* Parent				= 0;
								::gpk::aobj<::gpk::pobj<SJSONNode>>				Children				= 0;
								int32_t													ObjectIndex				= -1;
	};

	struct SJSONReaderState {
								uint32_t												IndexCurrentChar		= 0;
								int32_t													IndexCurrentElement		= -1;
								::gpk::SJSONToken										* CurrentElement		= 0;
								int32_t													NestLevel				= 0;
								char													CharCurrent				= 0;
								bool													Escaping				: 1;
								bool													InsideString			: 1;
								bool													ExpectingSeparator		: 1;
								bool													DoneReading				: 1;

																						SJSONReaderState		()
		: Escaping			(false)
		, InsideString		(false)
		, ExpectingSeparator(false)
		, DoneReading		(false)
		{}
	};
#pragma pack(pop)

	struct SJSONReader {
								::gpk::apod<::gpk::SJSONToken>						Token;
								::gpk::aobj<::gpk::vcc>				View;
								::gpk::aobj<::gpk::pobj<::gpk::SJSONNode>>		Tree;
								::gpk::SJSONReaderState									StateRead;

								::gpk::error_t											Reset					()							{
			Token	.clear();
			View	.clear();
			StateRead																		= {};
			return 0;
		}

								const ::gpk::pobj<::gpk::SJSONNode>&					operator[]				(uint32_t index)	const	{ return Tree[index]; }
	};

	// Reader functions: Populate a SJSONReader structure from an input JSON string.
							::gpk::error_t											jsonParse				(::gpk::SJSONReader & reader, const ::gpk::vcc & jsonAsString);
							::gpk::error_t											jsonParseStep			(::gpk::SJSONReader & reader, const ::gpk::vcc & jsonAsString);



	// Access functions: Iterate over children looking for keys or values.
							::gpk::error_t											jsonArrayValueGet		(const ::gpk::SJSONNode & node, uint32_t index);	// returns the index of the JSON element corresponding to the index provided as parameter.
	static inline			::gpk::error_t											jsonArrayValueGet		(const ::gpk::SJSONReader & reader, uint32_t iNode, uint32_t indexOfValueToGet)			noexcept	{ ree_if(iNode >= reader.Tree.size(), "Invalid node index: %i. Total nodes: %u", iNode, reader.Tree.size()); return ::gpk::jsonArrayValueGet(*reader.Tree[iNode], indexOfValueToGet); }

							::gpk::error_t											jsonObjectValueGet		(const ::gpk::SJSONNode & node, const ::gpk::view<::gpk::vcc> & views, const ::gpk::vcs & key);
	static inline			::gpk::error_t											jsonObjectValueGet		(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs& key)	noexcept	{ ree_if(iNode >= reader.Tree.size(), "Invalid node index: %i. Total nodes: %u", iNode, reader.Tree.size()); return ::gpk::jsonObjectValueGet(*reader.Tree[iNode], reader.View, {key.begin(), key.size()}); }
							::gpk::error_t											jsonObjectKeyList		(const ::gpk::SJSONNode & node, ::gpk::apod<int32_t> & indices);
							::gpk::error_t											jsonObjectKeyList		(const ::gpk::SJSONNode & node, const ::gpk::view<::gpk::vcc> & views, ::gpk::aobj<::gpk::vcc> & keys);
							::gpk::error_t											jsonObjectKeyList		(const ::gpk::SJSONNode & node, const ::gpk::view<::gpk::vcc> & views, ::gpk::apod<int32_t> & indices, ::gpk::aobj<::gpk::vcc> & keys);
	static inline			::gpk::error_t											jsonObjectKeyList		(const ::gpk::SJSONReader & reader, uint32_t iNode, ::gpk::apod<int32_t> & indices)																			{ ree_if(iNode >= reader.Tree.size(), "Invalid node index: %i. Total nodes: %u", iNode, reader.Tree.size()); return jsonObjectKeyList(*reader.Tree[iNode], indices);				}
	static inline			::gpk::error_t											jsonObjectKeyList		(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::view<::gpk::vcc> & views, ::gpk::aobj<::gpk::vcc> & keys)									{ ree_if(iNode >= reader.Tree.size(), "Invalid node index: %i. Total nodes: %u", iNode, reader.Tree.size()); return jsonObjectKeyList(*reader.Tree[iNode], views, keys);			}
	static inline			::gpk::error_t											jsonObjectKeyList		(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::view<::gpk::vcc> & views, ::gpk::apod<int32_t> & indices, ::gpk::aobj<::gpk::vcc> & keys)	{ ree_if(iNode >= reader.Tree.size(), "Invalid node index: %i. Total nodes: %u", iNode, reader.Tree.size()); return jsonObjectKeyList(*reader.Tree[iNode], views, indices, keys);	}

	static inline constexpr	::gpk::error_t											jsonObjectKeyCount		(const ::gpk::SJSONNode		& node)						noexcept	{ return node.Children.size() / 2; }
	static inline			::gpk::error_t											jsonObjectKeyCount		(const ::gpk::SJSONReader	& reader, uint32_t iNode)	noexcept	{ ree_if(iNode >= reader.Tree.size(), "Invalid node index: %i. Total nodes: %u", iNode, reader.Tree.size()); return ::gpk::jsonObjectKeyCount(*reader.Tree[iNode]); }
	static inline constexpr	::gpk::error_t											jsonArraySize			(const ::gpk::SJSONNode		& node)						noexcept	{ return node.Children.size(); }	// returns the index of the JSON element corresponding to the index provided as parameter.
	static inline			::gpk::error_t											jsonArraySize			(const ::gpk::SJSONReader	& reader, uint32_t iNode)	noexcept	{ ree_if(iNode >= reader.Tree.size(), "Invalid node index: %i. Total nodes: %u", iNode, reader.Tree.size()); return ::gpk::jsonArraySize(*reader.Tree[iNode]); }
							::gpk::error_t											jsonCompareNumber		(const ::gpk::SJSONNode & node, const ::gpk::view<::gpk::vcc> & views, const ::gpk::SJSONNode& other, const ::gpk::view<::gpk::vcc> & otherViews);
							::gpk::error_t											jsonCompareArray		(const ::gpk::SJSONNode & node, const ::gpk::view<::gpk::vcc> & views, const ::gpk::SJSONNode& other, const ::gpk::view<::gpk::vcc> & otherViews);
							::gpk::error_t											jsonCompareObject		(const ::gpk::SJSONNode & node, const ::gpk::view<::gpk::vcc> & views, const ::gpk::SJSONNode& other, const ::gpk::view<::gpk::vcc> & otherViews);
	static inline			::gpk::error_t											jsonCompareArray		(const ::gpk::SJSONNode & node, const ::gpk::SJSONNode& other, const ::gpk::view<::gpk::vcc> & views) { return ::gpk::jsonCompareArray	(node, views, other, views); }
	static inline			::gpk::error_t											jsonCompareObject		(const ::gpk::SJSONNode & node, const ::gpk::SJSONNode& other, const ::gpk::view<::gpk::vcc> & views) { return ::gpk::jsonCompareObject	(node, views, other, views); }
	static inline			::gpk::error_t											jsonCompareNumber		(const ::gpk::SJSONNode & node, const ::gpk::SJSONNode& other, const ::gpk::view<::gpk::vcc> & views) { return ::gpk::jsonCompareNumber	(node, views, other, views); }

							::gpk::error_t											jsonTreeRebuild			(::gpk::view<::gpk::SJSONToken>& in_object, ::gpk::aobj<::gpk::pobj<::gpk::SJSONNode>> & out_nodes);
	struct SJSONFile {
								::gpk::apod<char_t>								Bytes					= {};
								::gpk::SJSONReader										Reader					= {};
	};

	typedef ::gpk::SKeyVal<::gpk::vcc, ::gpk::SJSONFile>				TKeyValJSONFile;

							::gpk::error_t											jsonFileRead			(::gpk::SJSONFile & file, const ::gpk::vcc & filename);
							::gpk::error_t											jsonWrite				(const ::gpk::SJSONNode* node, const ::gpk::view<::gpk::vcc> & jsonViews, ::gpk::apod<char_t> & output);
							::gpk::error_t											jsonArraySplit			(const ::gpk::SJSONNode & jsonArrayToSplit, const ::gpk::view<::gpk::vcc> & jsonViews, const uint32_t blockSize, ::gpk::aobj<::gpk::apod<char_t>> & outputJsons);

	struct SJSONFieldBinding {
								::gpk::vcc									Field			;
								::gpk::JSON_TYPE										Type			;
	};

							::gpk::error_t											jsonMapToFields			(::gpk::apod<int32_t> & indicesOfFields, const ::gpk::view<const ::gpk::SJSONFieldBinding> fields, const ::gpk::view<const ::gpk::TKeyValConstChar> fieldMaps);
							::gpk::error_t											jsonFieldsToMap			(::gpk::apod<int32_t> & indicesOfFields, const ::gpk::view<const ::gpk::SJSONFieldBinding> fields, const ::gpk::view<const ::gpk::TKeyValConstChar> fieldMaps);
} // namespace

#endif // GPK_JSON_H_92749028348923
