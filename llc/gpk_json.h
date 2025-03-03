#include "gpk_enum.h"
#include "gpk_array_ptr.h"
#include "gpk_slice.h"

#ifndef GPK_JSON_H_23627
#define GPK_JSON_H_23627

namespace gpk
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE(JSON_TYPE, int32_t);
	GDEFINE_ENUM_VALUE(JSON_TYPE, NULL		,  0);
	GDEFINE_ENUM_VALUE(JSON_TYPE, INTEGER	,  1);
	GDEFINE_ENUM_VALUE(JSON_TYPE, DECIMAL	,  2);
	GDEFINE_ENUM_VALUE(JSON_TYPE, BOOLEAN	,  3);
	GDEFINE_ENUM_VALUE(JSON_TYPE, STRING	,  4);
	GDEFINE_ENUM_VALUE(JSON_TYPE, KEY		,  5);
	GDEFINE_ENUM_VALUE(JSON_TYPE, VALUE		,  6);
	GDEFINE_ENUM_VALUE(JSON_TYPE, OBJECT	,  7);
	GDEFINE_ENUM_VALUE(JSON_TYPE, ARRAY		,  8);
	GDEFINE_ENUM_VALUE(JSON_TYPE, CODEPOINT	,  9);
	GDEFINE_ENUM_VALUE(JSON_TYPE, COUNT		, 10);
	GDEFINE_ENUM_VALUE(JSON_TYPE, UNKNOWN	, -1);

	struct SJSONToken {
		int32_t					ParentIndex;
		JSON_TYPE				Type;
		::gpk::sliceu32			Span;
		uint64_t				Value;
	};

	struct SJSONNode {
		SJSONToken				* Token					= 0;
		SJSONNode				* Parent				= 0;
		::gpk::apobj<SJSONNode>	Children				= {};
		int32_t					ObjectIndex				= -1;
	};

	struct SJSONReaderState {
		uint32_t				IndexCurrentChar		= 0;
		int32_t					IndexCurrentElement		= -1;
		::gpk::SJSONToken		* CurrentElement		= 0;
		int32_t					NestLevel				= 0;
		sc_t					CharCurrent				= 0;
		bool					Escaping				: 1;
		bool					InsideString			: 1;
		bool					ExpectingSeparator		: 1;
		bool					DoneReading				: 1;

								SJSONReaderState		()
		: Escaping			(false)
		, InsideString		(false)
		, ExpectingSeparator(false)
		, DoneReading		(false)
		{}
	};
#pragma pack(pop)

	struct SJSONReader {
		::gpk::apod<::gpk::SJSONToken>	Token;
		::gpk::avcc						View;
		::gpk::apobj<::gpk::SJSONNode>	Tree;
		::gpk::SJSONReaderState			StateRead;

		::gpk::error_t					Reset					()							{
			Token	.clear();
			View	.clear();
			StateRead						= {};
			return 0;
		}

		const ::gpk::pobj<::gpk::SJSONNode>&	operator[]				(uint32_t index)	const	{ return Tree[index]; }
	};

	// Reader functions: Populate a SJSONReader structure from an input JSON string.
	::gpk::error_t			jsonParse				(::gpk::SJSONReader & reader, const ::gpk::vcc & jsonAsString);
	::gpk::error_t			jsonParseStep			(::gpk::SJSONReader & reader, const ::gpk::vcc & jsonAsString);
	::gpk::error_t			jsonTreeRebuild			(::gpk::view<::gpk::SJSONToken> & in_object, ::gpk::apobj<::gpk::SJSONNode> & out_nodes);

	stincxp	::gpk::error_t	jsonObjectKeyCount		(const ::gpk::SJSONNode & node)		noexcept	{ return node.Children.size() / 2; }
	stincxp	::gpk::error_t	jsonArraySize			(const ::gpk::SJSONNode & node)		noexcept	{ return node.Children.size(); }	// returns the index of the JSON element corresponding to the index provided as parameter.
	::gpk::error_t			jsonArrayValueGet		(const ::gpk::SJSONNode & node, uint32_t index);	// returns the index of the JSON element corresponding to the index provided as parameter.
	::gpk::error_t			jsonObjectValueGet		(const ::gpk::SJSONNode & node, const ::gpk::view<::gpk::vcc> & views, const ::gpk::vcs & key);
	::gpk::error_t			jsonObjectKeyList		(const ::gpk::SJSONNode & node, ::gpk::ai32 & indices);
	::gpk::error_t			jsonObjectKeyList		(const ::gpk::SJSONNode & node, const ::gpk::view<::gpk::vcc> & views, ::gpk::avcc & keys);
	::gpk::error_t			jsonObjectKeyList		(const ::gpk::SJSONNode & node, const ::gpk::view<::gpk::vcc> & views, ::gpk::ai32 & indices, ::gpk::avcc & keys);
	::gpk::error_t			jsonCompareNumber		(const ::gpk::SJSONNode & node, const ::gpk::view<::gpk::vcc> & views, const ::gpk::SJSONNode & other, const ::gpk::view<::gpk::vcc> & otherViews);
	::gpk::error_t			jsonCompareArray		(const ::gpk::SJSONNode & node, const ::gpk::view<::gpk::vcc> & views, const ::gpk::SJSONNode & other, const ::gpk::view<::gpk::vcc> & otherViews);
	::gpk::error_t			jsonCompareObject		(const ::gpk::SJSONNode & node, const ::gpk::view<::gpk::vcc> & views, const ::gpk::SJSONNode & other, const ::gpk::view<::gpk::vcc> & otherViews);


	// Access functions: Iterate over children looking for keys or values.
	ndstain	::gpk::error_t	jsonObjectKeyCount	(const ::gpk::SJSONReader & reader, uint32_t iNode)												{ rew_if(iNode >= reader.Tree.size(), "Invalid node index: %i. Total nodes: %u", iNode, reader.Tree.size()); return jsonObjectKeyCount(*reader.Tree[iNode]); }
	ndstain	::gpk::error_t	jsonArraySize		(const ::gpk::SJSONReader & reader, uint32_t iNode)												{ rew_if(iNode >= reader.Tree.size(), "Invalid node index: %i. Total nodes: %u", iNode, reader.Tree.size()); return jsonArraySize(*reader.Tree[iNode]); }
	ndstain	::gpk::error_t	jsonArrayValueGet	(const ::gpk::SJSONReader & reader, uint32_t iNode, uint32_t indexOfValueToGet)					{ rew_if(iNode >= reader.Tree.size(), "Invalid node index: %i. Total nodes: %u", iNode, reader.Tree.size()); return jsonArrayValueGet(*reader.Tree[iNode], indexOfValueToGet); }
	ndstain	::gpk::error_t	jsonObjectValueGet	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key)						{ rew_if(iNode >= reader.Tree.size(), "Invalid node index: %i. Total nodes: %u", iNode, reader.Tree.size()); return jsonObjectValueGet(*reader.Tree[iNode], reader.View, {key.begin(), key.size()}); }
	stainli	::gpk::error_t	jsonObjectKeyList	(const ::gpk::SJSONReader & reader, uint32_t iNode, ::gpk::ai32 & indices)						{ rew_if(iNode >= reader.Tree.size(), "Invalid node index: %i. Total nodes: %u", iNode, reader.Tree.size()); return jsonObjectKeyList(*reader.Tree[iNode], indices); }
	stainli	::gpk::error_t	jsonObjectKeyList	(const ::gpk::SJSONReader & reader, uint32_t iNode, ::gpk::avcc & keys)							{ rew_if(iNode >= reader.Tree.size(), "Invalid node index: %i. Total nodes: %u", iNode, reader.Tree.size()); return jsonObjectKeyList(*reader.Tree[iNode], reader.View, keys); }
	stainli	::gpk::error_t	jsonObjectKeyList	(const ::gpk::SJSONReader & reader, uint32_t iNode, ::gpk::ai32 & indices, ::gpk::avcc & keys)	{ rew_if(iNode >= reader.Tree.size(), "Invalid node index: %i. Total nodes: %u", iNode, reader.Tree.size()); return jsonObjectKeyList(*reader.Tree[iNode], reader.View, indices, keys); }
	ndstain	::gpk::error_t	jsonCompareArray	(const ::gpk::SJSONNode & node, const ::gpk::SJSONNode & other, const ::gpk::view<::gpk::vcc> & views)	{ return ::gpk::jsonCompareArray	(node, views, other, views); }
	ndstain	::gpk::error_t	jsonCompareObject	(const ::gpk::SJSONNode & node, const ::gpk::SJSONNode & other, const ::gpk::view<::gpk::vcc> & views)	{ return ::gpk::jsonCompareObject	(node, views, other, views); }
	ndstain	::gpk::error_t	jsonCompareNumber	(const ::gpk::SJSONNode & node, const ::gpk::SJSONNode & other, const ::gpk::view<::gpk::vcc> & views)	{ return ::gpk::jsonCompareNumber	(node, views, other, views); }

	::gpk::error_t	jsonObjectGetString (const ::gpk::SJSONReader & reader, uint32_t iNode, vcc		& value);
	::gpk::error_t	jsonObjectGetBoolean(const ::gpk::SJSONReader & reader, uint32_t iNode, bool	& value);
	::gpk::error_t	jsonObjectGetInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, s3_t	& value);
	::gpk::error_t	jsonObjectGetInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, s2_t	& value);
	::gpk::error_t	jsonObjectGetInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, s1_t	& value);
	::gpk::error_t	jsonObjectGetInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, s0_t	& value);
	::gpk::error_t	jsonObjectGetInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, u3_t	& value);
	::gpk::error_t	jsonObjectGetInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, u2_t	& value);
	::gpk::error_t	jsonObjectGetInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, u1_t	& value);
	::gpk::error_t	jsonObjectGetInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, u0_t	& value);
	::gpk::error_t	jsonObjectGetDecimal(const ::gpk::SJSONReader & reader, uint32_t iNode, double	& value);
	::gpk::error_t	jsonObjectGetDecimal(const ::gpk::SJSONReader & reader, uint32_t iNode, float	& value);

	// TODO: Finish
	//::gpk::error_t	jsonObjectGetIntegerAsString (const ::gpk::SJSONReader & reader, uint32_t iNode, vcc	& value);
	::gpk::error_t	jsonObjectGetIntegerAsBoolean(const ::gpk::SJSONReader & reader, uint32_t iNode, bool	& value);
	::gpk::error_t	jsonObjectGetIntegerAsDecimal(const ::gpk::SJSONReader & reader, uint32_t iNode, double	& value);
	::gpk::error_t	jsonObjectGetDecimalAsInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, s3_t	& value);
	//::gpk::error_t	jsonObjectGetDecimalAsString (const ::gpk::SJSONReader & reader, uint32_t iNode, vcc	& value);
	::gpk::error_t	jsonObjectGetDecimalAsBoolean(const ::gpk::SJSONReader & reader, uint32_t iNode, bool	& value);
	//::gpk::error_t	jsonObjectGetStringAsInteger (const ::gpk::SJSONReader & reader, uint32_t iNode, s3_t	& value);
	//::gpk::error_t	jsonObjectGetStringAsBoolean (const ::gpk::SJSONReader & reader, uint32_t iNode, bool	& value);
	//::gpk::error_t	jsonObjectGetStringAsDecimal (const ::gpk::SJSONReader & reader, uint32_t iNode, double	& value);
	::gpk::error_t	jsonObjectGetBooleanAsInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, s3_t	& value);
	//::gpk::error_t	jsonObjectGetBooleanAsString (const ::gpk::SJSONReader & reader, uint32_t iNode, vcc	& value);
	::gpk::error_t	jsonObjectGetBooleanAsDecimal(const ::gpk::SJSONReader & reader, uint32_t iNode, double	& value);
	// 
	//::gpk::error_t	jsonObjectGetAsString	(const ::gpk::SJSONReader & reader, uint32_t iNode, vcc		& value);
	//::gpk::error_t	jsonObjectGetAsBoolean	(const ::gpk::SJSONReader & reader, uint32_t iNode, bool	& value);
	//::gpk::error_t	jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, s3_t	& value);
	//::gpk::error_t	jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, s2_t	& value);
	//::gpk::error_t	jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, s1_t	& value);
	//::gpk::error_t	jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, s0_t	& value);
	//::gpk::error_t	jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, u3_t	& value);
	//::gpk::error_t	jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, u2_t	& value);
	//::gpk::error_t	jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, u1_t	& value);
	//::gpk::error_t	jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, u0_t	& value);
	//::gpk::error_t	jsonObjectGetAsDecimal	(const ::gpk::SJSONReader & reader, uint32_t iNode, double	& value);
	//::gpk::error_t	jsonObjectGetAsDecimal	(const ::gpk::SJSONReader & reader, uint32_t iNode, float	& value);

	::gpk::error_t	jsonObjectGetAsString	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, vcc		& value);
	::gpk::error_t	jsonObjectGetAsBoolean	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, bool	& value);
	::gpk::error_t	jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, s3_t	& value);
	::gpk::error_t	jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, s2_t	& value);
	::gpk::error_t	jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, s1_t	& value);
	::gpk::error_t	jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, s0_t	& value);
	::gpk::error_t	jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, u3_t	& value);
	::gpk::error_t	jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, u2_t	& value);
	::gpk::error_t	jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, u1_t	& value);
	::gpk::error_t	jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, u0_t	& value);
	::gpk::error_t	jsonObjectGetAsDecimal	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, double	& value);
	::gpk::error_t	jsonObjectGetAsDecimal	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, float	& value);

	struct SJSONFile {
		::gpk::achar			Bytes					= {};
		::gpk::SJSONReader		Reader					= {};
	};

	typedef ::gpk::SKeyVal<::gpk::vcc, ::gpk::SJSONFile>	TKeyValJSONFile;

	::gpk::error_t			jsonFileRead			(::gpk::SJSONFile & file, const ::gpk::vcc & filename);
	::gpk::error_t			jsonWrite				(const ::gpk::SJSONNode * node, const ::gpk::view<::gpk::vcc> & jsonViews, ::gpk::apod<sc_t> & output);
	::gpk::error_t			jsonArraySplit			(const ::gpk::SJSONNode & jsonArrayToSplit, const ::gpk::view<::gpk::vcc> & jsonViews, const uint32_t blockSize, ::gpk::aobj<::gpk::apod<sc_t>> & outputJsons);

	struct SJSONFieldBinding {
		::gpk::vcc				Field					= {};
		::gpk::JSON_TYPE		Type					= {};
	};

	::gpk::error_t			jsonMapToFields			(::gpk::ai32 & indicesOfFields, const ::gpk::view<const ::gpk::SJSONFieldBinding> fields, const ::gpk::view<const ::gpk::TKeyValConstChar> fieldMaps);
	::gpk::error_t			jsonFieldsToMap			(::gpk::ai32 & indicesOfFields, const ::gpk::view<const ::gpk::SJSONFieldBinding> fields, const ::gpk::view<const ::gpk::TKeyValConstChar> fieldMaps);
} // namespace

#endif // GPK_JSON_H_23627
