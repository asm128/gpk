#include "gpk_color.h"
#include "gpk_matrix.h"
#include "gpk_ptr.h"
#include "gpk_enum.h"
#include "gpk_array.h"
#include "gpk_stl.h"
#include "gpk_png.h"

#ifndef GPK_COMPONENT_SCENE_H_92384723
#define GPK_COMPONENT_SCENE_H_92384723

namespace gpk {
	GDEFINE_ENUM_TYPE(COMPONENT_FILE_TYPE, uint8_t); // bit field
	GDEFINE_ENUM_VALUE(COMPONENT_FILE_TYPE, STL			, 1);
	GDEFINE_ENUM_VALUE(COMPONENT_FILE_TYPE, PNG			, 2);

	GDEFINE_ENUM_TYPE(BOUNDINGPRIMITIVE_TYPE, uint8_t); // bit field
	GDEFINE_ENUM_VALUE(BOUNDINGPRIMITIVE_TYPE, SPHERE	, 1);
	GDEFINE_ENUM_VALUE(BOUNDINGPRIMITIVE_TYPE, BOX		, 2);
	GDEFINE_ENUM_VALUE(BOUNDINGPRIMITIVE_TYPE, PLANE	, 3);
	GDEFINE_ENUM_VALUE(BOUNDINGPRIMITIVE_TYPE, TORUS	, 4);

	GDEFINE_FLAG_TYPE(GLIGHT_TYPE, uint8_t); // bit field
	GDEFINE_FLAG_VALUE(GLIGHT_TYPE, NONE				, 0);
	GDEFINE_FLAG_VALUE(GLIGHT_TYPE, POINT				, 1);
	GDEFINE_FLAG_VALUE(GLIGHT_TYPE, SPOT				, 2);
	GDEFINE_FLAG_VALUE(GLIGHT_TYPE, DIRECTIONAL			, 4);
	GDEFINE_FLAG_VALUE(GLIGHT_TYPE, ALL					, 7);

	// Set the bit to 1 to enable processing of the light
	GDEFINE_FLAG_TYPE(GLIGHT_STATE, uint8_t); // bit field (combines with GLIGHT_TYPE)
	GDEFINE_FLAG_VALUE(GLIGHT_STATE, ENABLED			, 0x10);

	GDEFINE_FLAG_TYPE(GCOLOR_CHANNEL, uint8_t); // bit field
	GDEFINE_FLAG_VALUE(GCOLOR_CHANNEL, RED				, 0x01);
	GDEFINE_FLAG_VALUE(GCOLOR_CHANNEL, GREEN			, 0x02);
	GDEFINE_FLAG_VALUE(GCOLOR_CHANNEL, BLUE				, 0x04);
	GDEFINE_FLAG_VALUE(GCOLOR_CHANNEL, ALPHA			, 0x08);
	GDEFINE_FLAG_VALUE(GCOLOR_CHANNEL, DEPTH			, 0x10);
	GDEFINE_FLAG_VALUE(GCOLOR_CHANNEL, STENCIL			, 0x20);

	// These are different ways of using an image (texture) at render time (when drawing a particular object node).
	GDEFINE_FLAG_TYPE(GIMAGE_USAGE, uint8_t); // bit field
	GDEFINE_FLAG_VALUE(GIMAGE_USAGE, COLOR				, 0x00); // The colors of the image are mapped as a common image
	GDEFINE_FLAG_VALUE(GIMAGE_USAGE, BLEND_COLOR		, 0x10); // A 2D color image to be placed depending on the map value of GIMAGE_USAGE_BLEND_MAP
	GDEFINE_FLAG_VALUE(GIMAGE_USAGE, BLEND_MAP			, 0x20); // Each color of this texture are used as a filter for each texture set as GIMAGE_USAGE_BLEND_COLOR
	GDEFINE_FLAG_VALUE(GIMAGE_USAGE, BUMP				, 0x30); // The image is used as a bump map to be used for light calculations
	GDEFINE_FLAG_VALUE(GIMAGE_USAGE, DISPLACEMENT		, 0x40); // The image is used as a displacement map
	GDEFINE_FLAG_VALUE(GIMAGE_USAGE, HEIGHT				, 0x50); // The image is used as a height map to be used for vertex calculations
	GDEFINE_FLAG_VALUE(GIMAGE_USAGE, ALPHA				, 0x60); // The image is used as a color map to be used for vertex calculations
	GDEFINE_FLAG_VALUE(GIMAGE_USAGE, NORMAL				, 0x70); // The image is used as a color map to be used for vertex calculations
	GDEFINE_FLAG_VALUE(GIMAGE_USAGE, LIGHT				, 0x80); // The image is used as a light map to be used for vertex calculations
	GDEFINE_FLAG_VALUE(GIMAGE_USAGE, METALNESS			, 0x90); // The image is used as a color map to be used for vertex calculations
	GDEFINE_FLAG_VALUE(GIMAGE_USAGE, AMBIENTOCCLUSION	, 0xA0); // The image is used as a occlusion map to be used for vertex calculations
	GDEFINE_FLAG_VALUE(GIMAGE_USAGE, GLOSS				, 0xB0); // The image is used as a gloss map to be used for vertex calculations
	//
	GDEFINE_FLAG_VALUE(GIMAGE_USAGE, COLOR_DIFFUSE		, 0x01); // The colors of the image are mapped as a common image
	GDEFINE_FLAG_VALUE(GIMAGE_USAGE, COLOR_SPECULAR		, 0x02); // The colors of the image are mapped as a common image
	GDEFINE_FLAG_VALUE(GIMAGE_USAGE, COLOR_AMBIENT		, 0x03); // The colors of the image are mapped as a common image
	GDEFINE_FLAG_VALUE(GIMAGE_USAGE, COLOR_EMISSIVE		, 0x04); // The colors of the image are mapped as a common image

	// These are different ways of rendering a reflection
	GDEFINE_FLAG_TYPE(GMIRROR_TYPE, uint8_t); // bit field
	GDEFINE_FLAG_VALUE(GMIRROR_TYPE, NONE				, 0); // The surface doesn't reflect the surrounding environment in any way
	GDEFINE_FLAG_VALUE(GMIRROR_TYPE, PLANAR				, 1); // the reflection is in 1 direction only over the reflective surface (like in a mirror)
	GDEFINE_FLAG_VALUE(GMIRROR_TYPE, MAPPING			, 2); // the reflection direction changes depending on the geometry of the reflective surface (like in a metallic ball)

	// These are different ways of rendering the geometry buffers
	GDEFINE_ENUM_TYPE(GPRIMITIVE_TYPE, uint8_t); //
	GDEFINE_ENUM_VALUE(GPRIMITIVE_TYPE, POINT			, 1);
	GDEFINE_ENUM_VALUE(GPRIMITIVE_TYPE, LINE			, 2);
	GDEFINE_ENUM_VALUE(GPRIMITIVE_TYPE, TRIANGLE		, 3);
	//GDEFINE_ENUM_VALUE(GPRIMITIVE_TYPE, LINESTRIP		, 4);
	//GDEFINE_ENUM_VALUE(GPRIMITIVE_TYPE, TRIANGLESTRIP	, 5);
	GDEFINE_ENUM_VALUE(GPRIMITIVE_TYPE, QUAD			, 6);

	// The way the texture wraps an object
	GDEFINE_FLAG_TYPE(GWRAP_MODE, uint8_t); // bit field
	GDEFINE_FLAG_VALUE(GWRAP_MODE, DEFAULT				, 0);
	GDEFINE_FLAG_VALUE(GWRAP_MODE, U					, 1);
	GDEFINE_FLAG_VALUE(GWRAP_MODE, V					, 2);
	GDEFINE_FLAG_VALUE(GWRAP_MODE, W					, 4);
	GDEFINE_FLAG_VALUE(GWRAP_MODE, 3					, 8);

	// The way the triangles are discarded depending on their winding order (clockwise, counter-clockwise, no culling)
	GDEFINE_FLAG_TYPE(GCULL_MODE, uint8_t); // bit field
	GDEFINE_FLAG_VALUE(GCULL_MODE, CCW					, 0);
	GDEFINE_FLAG_VALUE(GCULL_MODE, CW					, 1);
	GDEFINE_FLAG_VALUE(GCULL_MODE, NONE					, 2);

	GDEFINE_FLAG_TYPE(GFILL_MODE, uint8_t); // bit field
	GDEFINE_FLAG_VALUE(GFILL_MODE, SOLID				, 0);
	GDEFINE_FLAG_VALUE(GFILL_MODE, WIREFRAME			, 1);
	GDEFINE_FLAG_VALUE(GFILL_MODE, POINT				, 2);

	// Some projection modes for constructing the projection matrices
	GDEFINE_FLAG_TYPE(GPROJECTION_TYPE, uint8_t); // bit field
	GDEFINE_FLAG_VALUE(GPROJECTION_TYPE, PERSPECTIVE	, 0);	// The perspective mode transforms vertices as in 3D space
	GDEFINE_FLAG_VALUE(GPROJECTION_TYPE, ORTHOGONAL		, 1);	// The orthogonal displays the image as if they were in a plane
	GDEFINE_FLAG_VALUE(GPROJECTION_TYPE, SCREENCOORD	, 2);	// This mode uses 2D top-to-down screen coordinates as frame of reference

#pragma pack(push, 1)
	struct SBlendIndices {
		uint8_t											A								= 0xFF
			,											B								= 0xFF
			,											C								= 0xFF
			,											D								= 0xFF
			;
		constexpr										SBlendIndices					()												noexcept 	= default;
		constexpr										SBlendIndices					(uint8_t a, uint8_t b, uint8_t c, uint8_t d)	noexcept	: A(a), B(b), C(c), D(d)	{}
		constexpr										SBlendIndices					(uint32_t packedValue)							noexcept
			: A((uint8_t)((packedValue & 0x000000FF) >>  0))
			, B((uint8_t)((packedValue & 0x0000FF00) >>  8))
			, C((uint8_t)((packedValue & 0x00FF0000) >> 16))
			, D((uint8_t)((packedValue & 0xFF000000) >> 24))
		{}
	};

	struct SBoundingVolume {
		double											Radius							= 0;
		::gpk::SMinMax<::gpk::SCoord3<float>>			Limits							= {{-1, -1, -1}, {1, 1, 1}};
		::gpk::SCoord3<float>							Center							= {0, 0, 0};
		::gpk::BOUNDINGPRIMITIVE_TYPE					Type							= BOUNDINGPRIMITIVE_TYPE_SPHERE;
	};

	struct SBufferSlice {
		::gpk::SRange<int32_t>							Indices							= {};
		::gpk::SRange<int32_t>							Vertices						= {};
	};

	struct SLight {
		::gpk::SColorFloat								Ambient							= {};
		::gpk::SColorFloat								Diffuse							= {};
		::gpk::SColorFloat								Specular						= {};
		::gpk::SCoord3<float>							Position						= {};
		::gpk::SCoord3<float>							Direction						= {};
		double											Angle							= {};
		float											RangeSquared					= 1;
		uint32_t										Type							: 3;
		uint32_t										Disabled						: 1;
	};

	struct SCamera {
		::gpk::SCoord3<float>							Position						= {};
		::gpk::SCoord3<float>							Target							= {};
		::gpk::SCoord3<float>							Up								= {};
		::gpk::SNearFar									ClipPlanes						= {};
		double											Angle							= {};
	};

	struct SMaterial {
		::gpk::SColorFloat								Ambient							= {};
		::gpk::SColorFloat								Diffuse							= {};
		::gpk::SColorFloat								Specular						= {};
		::gpk::SColorFloat								Emissive						= {};
		float											SpecularFactor					= {};
		float											Transparency					= {};

	};
	struct SRenderNodeTransform {
		::gpk::SMatrix4<float>							Matrix							= {};
		::gpk::SMatrix4<float>							MatrixInverse					= {};

		inline	void									SetIdentity						()	noexcept {
			Matrix			.SetIdentity();
			MatrixInverse	.SetIdentity();
		}
	};

	struct SRenderNode {
		int32_t											BoundingVolume					= -1;
		int32_t											TexCoords						= -1;
		int32_t											BlendIndices					= -1;
		int32_t											Indices							= -1;
		int32_t											Vertices						= -1;
		int32_t											Normals							= -1;
		int32_t											VertexColor						= -1;
		int32_t											Material						= -1;
		int32_t											Transform						= -1;
		int32_t											Color							= -1;

		::gpk::array_pod<uint32_t>						Textures						= {};
		::gpk::array_pod<uint32_t>						Lights							= {};

		uint32_t										PerFaceNormal					: 1;
		uint32_t										PerFaceColor					: 1;
		uint32_t										Transparent						: 1;
		uint32_t										Hidden							: 1;
		uint32_t										CullMode						: 2;
		uint32_t										PrimitiveType					: 3;

	};
#pragma pack(pop)

	typedef	::gpk::array_pod<::gpk::SCoord3<uint16_t		>>	TIndexBuffer			;
	typedef	::gpk::array_pod<::gpk::SCoord3<float			>>	TVertexBuffer			, TNormalBuffer,	TTangentBuffer;
	typedef	::gpk::array_pod<::gpk::SColorBGRA				>	TVertexColorBuffer		;
	typedef	::gpk::array_pod<::gpk::SCoord2<float			>>	TTexCoordBuffer			;
	typedef	::gpk::array_pod<::gpk::SBlendIndices			>	TBlendIndicesBuffer		;
	typedef	::gpk::array_pod<::gpk::SCoord2<int16_t>		>	TPixelCoordBuffer		;
	typedef	::gpk::array_pod<::gpk::STriangleWeights<float>	>	TTriangleWeightBuffer	;

	struct SRendererCache {
		::gpk::array_pod<uint32_t>						NodesToRender					= {};
		::gpk::array_obj<::gpk::SCoord3<float>>			NodeLightPositions				= {};	// an element for each node to render
		::gpk::array_obj<::gpk::SCoord3<float>>			NodeLightDirections				= {};	// an element for each node to render
		::gpk::array_obj<::gpk::array_pod<uint32_t>>	NodeLights						= {};	// an element for each node to render
		::gpk::array_pod<::gpk::SMatrix4<float>>		NodesWVP						= {};	// an element for each node to render

		::gpk::array_pod<::gpk::SCoord3<float>>			LightWorldDirections			= {};	// an element for each light in renderer
		::gpk::array_pod<::gpk::SCoord3<float>>			LightWorldPositions 			= {};	// an element for each light in renderer

		::gpk::array_pod<::gpk::SCoord3<float>>			CameraWorldDirections			= {};
		::gpk::array_pod<::gpk::SCoord3<float>>			CameraWorldPositions 			= {};

		TIndexBuffer									IndexBuffer						= {};
		TVertexBuffer									VertexBuffer					= {};
		TVertexColorBuffer								VertexColorBuffer				= {};
		TTexCoordBuffer									TexCoordBuffer					= {};
		TBlendIndicesBuffer								BlendIndicesBuffer				= {};
		TPixelCoordBuffer								PixelCoordBuffer				= {};
		TTriangleWeightBuffer							TriangleWeightBuffer			= {};
	};

	template<typename _tContainer, typename _tValue>
	struct SKeyedArray {
		::gpk::array_obj<::gpk::view_const_char>			Names	;
		_tContainer											Values	;

		_tValue &											operator[]					(uint32_t index)				{ return Values[index]; }
		const _tValue &										operator[]					(uint32_t index)	const		{ return Values[index]; }

		const uint32_t &									size						()				const noexcept	{ return Names.size(); }
		::gpk::error_t										push_back					(::gpk::view_const_char name, const _tValue& value) {
			gpk_necall(Names.push_back(name), "%s", "");
			return Values.push_back(value);
		}
	};

	template<typename _tValue>	struct SKeyedArrayPOD : public ::gpk::SKeyedArray<::gpk::array_pod<_tValue>, _tValue> {};
	template<typename _tValue>	struct SKeyedArrayOBJ : public ::gpk::SKeyedArray<::gpk::array_obj<_tValue>, _tValue> {};

	struct SNodeRenderer {
		typedef	::gpk::array_pod<::gpk::SCoord3<uint16_t	>>	TIndexBuffer			;
		typedef	::gpk::array_pod<::gpk::SCoord3<float		>>	TVertexBuffer			, TNormalBuffer,	TTangentBuffer;
		typedef	::gpk::array_pod<::gpk::SColorBGRA			>	TVertexColorBuffer		;
		typedef	::gpk::array_pod<::gpk::SCoord2<float		>>	TTexCoordBuffer			;
		typedef	::gpk::array_pod<::gpk::SBlendIndices		>	TBlendIndicesBuffer		;

		::gpk::SKeyedArrayOBJ<TIndexBuffer						>	Indices					= {};
		::gpk::SKeyedArrayOBJ<TVertexBuffer						>	Vertices				= {};
		::gpk::SKeyedArrayOBJ<TNormalBuffer						>	Normals					= {};
		::gpk::SKeyedArrayOBJ<TTangentBuffer					>	Tangents				= {};
		::gpk::SKeyedArrayOBJ<TVertexColorBuffer				>	VertexColors			= {};
		::gpk::SKeyedArrayOBJ<TTexCoordBuffer					>	TexCoords				= {};
		::gpk::SKeyedArrayOBJ<TBlendIndicesBuffer				>	BlendIndices			= {};
		::gpk::SKeyedArrayOBJ<::gpk::SImage<::gpk::SColorBGRA>	>	Textures				= {};

		::gpk::SKeyedArrayPOD<::gpk::SColorBGRA					>	Colors					= {};
		::gpk::SKeyedArrayPOD<::gpk::SMaterial					>	Materials				= {};
		::gpk::SKeyedArrayPOD<::gpk::SLight						>	Lights					= {};
		::gpk::SKeyedArrayPOD<::gpk::SCamera					>	Cameras					= {};
		::gpk::SKeyedArrayPOD<::gpk::SRenderNodeTransform		>	Transforms				= {};
		::gpk::SKeyedArrayPOD<::gpk::SBoundingVolume			>	BoundingVolumes			= {};
		//
		::gpk::SKeyedArrayOBJ<::gpk::SRenderNode>					Nodes					= {};
		//
		::gpk::SRendererCache										RenderCache				= {};
	};

	::gpk::error_t											nodeRendererDrawNode
		( ::gpk::SNodeRenderer					& renderer
		, uint32_t								iNode
		, const ::gpk::SMatrix4<float>			& view
		, const ::gpk::SMatrix4<float>			& projection
		, const ::gpk::SMatrix4<float>			& viewProjection
		, ::gpk::view_grid<::gpk::SColorBGRA>	target_image
		, ::gpk::view_grid<uint32_t>			target_depth
		);

	::gpk::error_t											nodeRendererDraw
		( ::gpk::SNodeRenderer					& renderer
		, int32_t								iCamera
		, ::gpk::view_grid<::gpk::SColorBGRA>	target_image
		, ::gpk::view_grid<uint32_t>			target_depth
		, bool									drawHidden			= false
		);

	struct SComponentData {
		int32_t													IndexParent				= -1;
		::gpk::array_pod<int32_t>								RenderNodes				= {};
		::gpk::array_pod<::gpk::SRenderNodeTransform>			LocalTransforms			= {};
	};

	struct SComponentScene {
		::gpk::SKeyedArrayOBJ<::gpk::SComponentData>			Components				= {};
		::gpk::array_pod<::gpk::array_pod<int32_t>>				Children				= {};	// A children list for each component

		::gpk::SNodeRenderer									Renderer				= {};

		::gpk::error_t											Create					(::gpk::view_const_string name);
		::gpk::error_t											CreateFromFile			(::gpk::view_const_string filename);
	};
} // namespace

#endif // GPK_COMPONENT_SCENE_H_92384723
