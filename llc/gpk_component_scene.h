#include "gpk_color.h"
#include "gpk_matrix.h"
#include "gpk_matrix2.h"
#include "gpk_matrix3.h"
#include "gpk_array_ptr.h"
#include "gpk_enum.h"
#include "gpk_stl.h"
#include "gpk_range.h"
#include "gpk_apod_color.h"
#include "gpk_grid_color.h"
#include "gpk_img_color.h"

#ifndef GPK_COMPONENT_SCENE_H_23627
#define GPK_COMPONENT_SCENE_H_23627

namespace gpk {
	GDEFINE_ENUM_TYPE(COMPONENT_FILE_TYPE, u0_t); // bit field
	GDEFINE_ENUM_VALUE(COMPONENT_FILE_TYPE, STL			, 1);
	GDEFINE_ENUM_VALUE(COMPONENT_FILE_TYPE, PNG			, 2);

	GDEFINE_ENUM_TYPE(BOUNDINGPRIMITIVE_TYPE, u0_t); //
	GDEFINE_ENUM_VALUE(BOUNDINGPRIMITIVE_TYPE, SPHERE	, 1);
	GDEFINE_ENUM_VALUE(BOUNDINGPRIMITIVE_TYPE, BOX		, 2);
	GDEFINE_ENUM_VALUE(BOUNDINGPRIMITIVE_TYPE, GRID		, 3);
	GDEFINE_ENUM_VALUE(BOUNDINGPRIMITIVE_TYPE, TORUS	, 4);
	GDEFINE_ENUM_VALUE(BOUNDINGPRIMITIVE_TYPE, CYLINDER	, 5);

	GDEFINE_FLAG_TYPE(GLIGHT_TYPE, u0_t); // bit field
	GDEFINE_FLAG_VALUE(GLIGHT_TYPE, NONE				, 0);
	GDEFINE_FLAG_VALUE(GLIGHT_TYPE, POINT				, 1);
	GDEFINE_FLAG_VALUE(GLIGHT_TYPE, SPOT				, 2);
	GDEFINE_FLAG_VALUE(GLIGHT_TYPE, DIRECTIONAL			, 4);
	GDEFINE_FLAG_VALUE(GLIGHT_TYPE, ALL					, 7);

	// Set the bit to 1 to enable processing of the light
	GDEFINE_FLAG_TYPE(GLIGHT_STATE, u0_t); // bit field (combines with GLIGHT_TYPE)
	GDEFINE_FLAG_VALUE(GLIGHT_STATE, ENABLED			, 0x10);

	GDEFINE_FLAG_TYPE(GCOLOR_CHANNEL, u0_t); // bit field
	GDEFINE_FLAG_VALUE(GCOLOR_CHANNEL, RED				, 0x01);
	GDEFINE_FLAG_VALUE(GCOLOR_CHANNEL, GREEN			, 0x02);
	GDEFINE_FLAG_VALUE(GCOLOR_CHANNEL, BLUE				, 0x04);
	GDEFINE_FLAG_VALUE(GCOLOR_CHANNEL, ALPHA			, 0x08);
	GDEFINE_FLAG_VALUE(GCOLOR_CHANNEL, DEPTH			, 0x10);
	GDEFINE_FLAG_VALUE(GCOLOR_CHANNEL, STENCIL			, 0x20);

	// These are different ways of using an image (texture) at render time (when drawing a particular object node).
	GDEFINE_FLAG_TYPE(GIMAGE_USAGE, u0_t); // bit field
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
	GDEFINE_FLAG_TYPE(GMIRROR_TYPE, u0_t); // bit field
	GDEFINE_FLAG_VALUE(GMIRROR_TYPE, NONE				, 0); // The surface doesn't reflect the surrounding environment in any way
	GDEFINE_FLAG_VALUE(GMIRROR_TYPE, PLANAR				, 1); // the reflection is in 1 direction only over the reflective surface (like in a mirror)
	GDEFINE_FLAG_VALUE(GMIRROR_TYPE, MAPPING			, 2); // the reflection direction changes depending on the geometry of the reflective surface (like in a metallic ball)

	// These are different ways of rendering the geometry buffers
	GDEFINE_ENUM_TYPE(GPRIMITIVE_TYPE, u0_t); //
	GDEFINE_ENUM_VALUE(GPRIMITIVE_TYPE, POINT			, 1);
	GDEFINE_ENUM_VALUE(GPRIMITIVE_TYPE, LINE			, 2);
	GDEFINE_ENUM_VALUE(GPRIMITIVE_TYPE, TRIANGLE		, 3);
	//GDEFINE_ENUM_VALUE(GPRIMITIVE_TYPE, LINESTRIP		, 4);
	//GDEFINE_ENUM_VALUE(GPRIMITIVE_TYPE, TRIANGLESTRIP	, 5);
	GDEFINE_ENUM_VALUE(GPRIMITIVE_TYPE, QUAD			, 6);

	// The way the texture wraps an object
	GDEFINE_FLAG_TYPE(GWRAP_MODE, u0_t); // bit field
	GDEFINE_FLAG_VALUE(GWRAP_MODE, DEFAULT				, 0);
	GDEFINE_FLAG_VALUE(GWRAP_MODE, U					, 1);
	GDEFINE_FLAG_VALUE(GWRAP_MODE, V					, 2);
	GDEFINE_FLAG_VALUE(GWRAP_MODE, W					, 4);
	GDEFINE_FLAG_VALUE(GWRAP_MODE, 3					, 8);

	// The way the triangles are discarded depending on their winding order (clockwise, counter-clockwise, no culling)
	GDEFINE_FLAG_TYPE(GCULL_MODE, u0_t); // bit field
	GDEFINE_FLAG_VALUE(GCULL_MODE, CCW					, 0);
	GDEFINE_FLAG_VALUE(GCULL_MODE, CW					, 1);
	GDEFINE_FLAG_VALUE(GCULL_MODE, NONE					, 2);

	GDEFINE_FLAG_TYPE(GFILL_MODE, u0_t); // bit field
	GDEFINE_FLAG_VALUE(GFILL_MODE, SOLID				, 0);
	GDEFINE_FLAG_VALUE(GFILL_MODE, WIREFRAME			, 1);
	GDEFINE_FLAG_VALUE(GFILL_MODE, POINT				, 2);

	// Some projection modes for constructing the projection matrices
	GDEFINE_FLAG_TYPE(GPROJECTION_TYPE, u0_t); // bit field
	GDEFINE_FLAG_VALUE(GPROJECTION_TYPE, PERSPECTIVE	, 0);	// The perspective mode transforms vertices as in 3D space
	GDEFINE_FLAG_VALUE(GPROJECTION_TYPE, ORTHOGONAL		, 1);	// The orthogonal displays the image as if they were in a plane
	GDEFINE_FLAG_VALUE(GPROJECTION_TYPE, SCREENCOORD	, 2);	// This mode uses 2D top-to-down screen coordinates as frame of reference

#pragma pack(push, 1)
	struct SBlendIndices {
		u0_t											A								= 0xFF
			,											B								= 0xFF
			,											C								= 0xFF
			,											D								= 0xFF
			;
		constexpr										SBlendIndices					()												noexcept 	= default;
		constexpr										SBlendIndices					(u0_t a, u0_t b, u0_t c, u0_t d)	noexcept	: A(a), B(b), C(c), D(d)	{}
		constexpr										SBlendIndices					(u2_t packedValue)							noexcept
			: A((u0_t)((packedValue & 0x000000FF) >>  0))
			, B((u0_t)((packedValue & 0x0000FF00) >>  8))
			, C((u0_t)((packedValue & 0x00FF0000) >> 16))
			, D((u0_t)((packedValue & 0xFF000000) >> 24))
		{}
	};

	struct SBodyVolumeOld {
		f3_t							Radius							= 0;
		minmax<n3f2_t>	Limits							= {{-1, -1, -1}, {1, 1, 1}};
		n3f2_t					Center							= {0, 0, 0};
		BOUNDINGPRIMITIVE_TYPE	Type							= BOUNDINGPRIMITIVE_TYPE_SPHERE;
	};

	struct SBufferSlice {
		ranges2_t			Indices							= {};
		ranges2_t			Vertices						= {};
	};

	struct SLight {
		rgbaf			Ambient							= {};
		rgbaf			Diffuse							= {};
		rgbaf			Specular						= {};
		n3f2_t			Position						= {};
		n3f2_t			Direction						= {};
		f3_t					Angle							= {};
		f2_t					RangeSquared					= 1;
		u2_t				Type							: 3;
		u2_t				Disabled						: 1;
	};

	struct SCamera {
		n3f2_t			Position						= {};
		n3f2_t			Target							= {};
		n3f2_t			Up								= {};
		minmax<f2_t>	ClipPlanes						= {};
		f3_t					Angle							= {};
	};

	struct SMaterial {
		rgbaf			Ambient							= {};
		rgbaf			Diffuse							= {};
		rgbaf			Specular						= {};
		rgbaf			Emissive						= {};
		f2_t					SpecularFactor					= {};
		f2_t					Transparency					= {};

	};
	struct SRenderNodeTransform {
		m4f2_t			Matrix							= {};
		m4f2_t			MatrixInverse					= {};

		inline	void			SetIdentity						()	noexcept {
			Matrix			.Identity();
			MatrixInverse	.Identity();
		}
	};

	struct SRenderNode {
		s2_t					BoundingVolume					= -1;
		s2_t					TexCoords						= -1;
		s2_t					BlendIndices					= -1;
		s2_t					Indices							= -1;
		s2_t					Vertices						= -1;
		s2_t					Normals							= -1;
		s2_t					VertexColor						= -1;
		s2_t					Material						= -1;
		s2_t					Transform						= -1;
		s2_t					Color							= -1;

		au2_t			Textures						= {};
		au2_t			Lights							= {};

		u2_t				PerFaceNormal					: 1;
		u2_t				PerFaceColor					: 1;
		u2_t				Transparent						: 1;
		u2_t				Hidden							: 1;
		u2_t				CullMode						: 2;
		u2_t				PrimitiveType					: 3;

	};
#pragma pack(pop)

	typedef	apod<n3u1_t			>	TIndexBuffer			;
	typedef	apod<n3f2_t			>	TVertexBuffer			, TNormalBuffer,	TTangentBuffer;
	typedef	apod<bgra			>	TVertexColorBuffer		;
	typedef	apod<n2f2_t			>	TTexCoordBuffer			;
	typedef	apod<SBlendIndices	>	TBlendIndicesBuffer		;
	typedef	apod<n2s1_t			>	TPixelCoordBuffer		;
	typedef	apod<trif2_t		>	TTriangleWeightBuffer	;

	struct SRendererCache {
		au2_t					NodesToRender			= {};
		aobj<n3f2_t>			NodeLightPositions		= {};	// an element for each node to render
		aobj<n3f2_t>			NodeLightDirections		= {};	// an element for each node to render
		aobj<au2_t>				NodeLights				= {};	// an element for each node to render
		apod<m4f2_t>			NodesWVP				= {};	// an element for each node to render

		apod<n3f2_t>			LightWorldDirections	= {};	// an element for each light in renderer
		apod<n3f2_t>			LightWorldPositions 	= {};	// an element for each light in renderer

		apod<n3f2_t>			CameraWorldDirections	= {};
		apod<n3f2_t>			CameraWorldPositions 	= {};

		TIndexBuffer			IndexBuffer				= {};
		TVertexBuffer			VertexBuffer			= {};
		TVertexColorBuffer		VertexColorBuffer		= {};
		TTexCoordBuffer			TexCoordBuffer			= {};
		TBlendIndicesBuffer		BlendIndicesBuffer		= {};
		TPixelCoordBuffer		PixelCoordBuffer		= {};
		TTriangleWeightBuffer	TriangleWeightBuffer	= {};
	};

	tplt<tpnm _tContainer, tpnm _tValue>
	struct SKeyedArray {
		aobj<vcst_t>		Names	;
		_tContainer			Values	;

		_tValue &			operator[]					(u2_t index)				{ return Values[index]; }
		const _tValue &		operator[]					(u2_t index)	const		{ return Values[index]; }

		const u2_t &		size						()				const noexcept	{ return Names.size(); }
		err_t				push_back					(vcst_t name, const _tValue & value) {
			if_fail_fe(Names.push_back(name));
			return Values.push_back(value);
		}
	};

	tplt<tpnm _tValue>	struct SKeyedArrayPOD : public SKeyedArray<apod<_tValue>, _tValue> {};
	tplt<tpnm _tValue>	struct SKeyedArrayOBJ : public SKeyedArray<aobj<_tValue>, _tValue> {};

	struct SNodeRenderer {
		typedef	apod<n3u1_t>		TIndexBuffer		;
		typedef	apod<n3f2_t>		TVertexBuffer		, TNormalBuffer,	TTangentBuffer;
		typedef	a8bgra				TVertexColorBuffer	;
		typedef	apod<n2f2_t>		TTexCoordBuffer		;
		typedef	apod<SBlendIndices>	TBlendIndicesBuffer	;

		SKeyedArrayOBJ<TIndexBuffer			>	Indices					= {};
		SKeyedArrayOBJ<TVertexBuffer		>	Vertices				= {};
		SKeyedArrayOBJ<TNormalBuffer		>	Normals					= {};
		SKeyedArrayOBJ<TTangentBuffer		>	Tangents				= {};
		SKeyedArrayOBJ<TVertexColorBuffer	>	VertexColors			= {};
		SKeyedArrayOBJ<TTexCoordBuffer		>	TexCoords				= {};
		SKeyedArrayOBJ<TBlendIndicesBuffer	>	BlendIndices			= {};
		SKeyedArrayOBJ<img<bgra>			>	Textures				= {};

		SKeyedArrayPOD<bgra					>	Colors					= {};
		SKeyedArrayPOD<SMaterial			>	Materials				= {};
		SKeyedArrayPOD<SLight				>	Lights					= {};
		SKeyedArrayPOD<SCamera				>	Cameras					= {};
		SKeyedArrayPOD<SRenderNodeTransform	>	Transforms				= {};
		SKeyedArrayPOD<SBodyVolumeOld		>	BoundingVolumes			= {};
		//
		SKeyedArrayOBJ<SRenderNode>				Nodes					= {};
		//
		SRendererCache							RenderCache				= {};
	};

	err_t					nodeRendererDrawNode
		( SNodeRenderer			& renderer
		, u2_t						iNode
		, const m4f2_t			& view
		, const m4f2_t			& projection
		, const m4f2_t			& viewProjection
		, g8bgra					target_image
		, grid<u2_t>			target_depth
		);

	err_t					nodeRendererDraw
		( SNodeRenderer			& renderer
		, s2_t						iCamera
		, g8bgra					target_image
		, grid<u2_t>			target_depth
		, bool							drawHidden			= false
		);

	struct SComponentData {
		s2_t											Parent					= -1;
		as2_t									RenderNodes				= {};
		apod<SRenderNodeTransform>		LocalTransforms			= {};
	};

	struct SComponentScene {
		SKeyedArrayOBJ<SComponentData>	Components				= {};
		apod<as2_t>						Children				= {};	// A children list for each component

		SNodeRenderer							Renderer				= {};

		err_t									Create					(vcst_t name);
		err_t									CreateFromFile			(vcst_t filename);
	};
} // namespace

#endif // GPK_COMPONENT_SCENE_H_23627
