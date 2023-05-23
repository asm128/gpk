#include "gpk_array.h"
#include "gpk_coord.h"
#include "gpk_quat.h"
#include "gpk_array_static.h"
#include "gpk_tri.h"

#ifndef GPK_RSM_H_02938402983
#define GPK_RSM_H_02938402983

namespace gpk
{
#pragma pack(push, 1)
	struct SRSMFace {
		int16_t									Vertices[3]			;	// Vertex indices?
		int16_t									UVs		[3]			;	// UVs indices?
		int16_t									IndexTextureIndex	;	// texture index index
		int16_t									todo1				;	//
		int32_t									TwoSided			;	//
		int32_t									SmoothGroup			;	//
	};

	struct SRSMNodeTransform {
		::gpk::n3f32							Row0;
		::gpk::n3f32							Row1;
		::gpk::n3f32							Row2;
		::gpk::n3f32							Offset;
		::gpk::n3f32							Translation;
		float									Rotation;
		::gpk::n3f32							RotAxis;
		::gpk::n3f32							Scale;
	}; //

	struct SRSMFrameRotation {
		int										Time;
		::gpk::quatf32							Orientation;
	};

	struct SRSMFramePosition {
		int										Time;
		::gpk::n3f32							Position;
	};

	struct SRSMTexCoord {
		uint32_t								Unknown;
		::gpk::n2f32							UV;
	};
#pragma pack(pop)
	struct SRSMNode {
		::gpk::ai32								TextureIndices;
		::gpk::apod<::gpk::n3f32>				Vertices;
		::gpk::apod<::gpk::SRSMTexCoord>		UVs;
		::gpk::apod<::gpk::SRSMFace>			Faces;
		char									Name		[40];
		char									ParentName	[40];
		::gpk::SRSMNodeTransform				Transform;
		::gpk::apod<SRSMFrameRotation>			RotationKeyframes;
		::gpk::apod<SRSMFramePosition>			PositionKeyframes;
	};

	struct SRSMFileContents {
		uint32_t								AnimLength;
		uint32_t								ShadeType;
		uint8_t									Alpha;
		char									Unknown[16];
		::gpk::aobj<::gpk::astatic<char, 40>>	TextureNames;
		char									RootNodeName[40];
		::gpk::aobj<::gpk::SRSMNode>			Nodes;
	};
	::gpk::error_t							rsmFileLoad			(::gpk::SRSMFileContents & loaded, const ::gpk::vu8	& input);
	::gpk::error_t							rsmFileLoad			(::gpk::SRSMFileContents & loaded, FILE				* input);
	::gpk::error_t							rsmFileLoad			(::gpk::SRSMFileContents & loaded, const ::gpk::vcs	& input);

	struct SModelNodeRSM {
		::gpk::apod<::gpk::n3f32>				Normals			;
		::gpk::apod<::gpk::n3f32>				Vertices		;
		::gpk::apod<::gpk::n2f32>				UVs				;
		::gpk::apod<::gpk::triu32>				VertexIndices	;
		int32_t									TextureIndex	;
		int32_t									RSMNodeIndex	;
		//::gpk::ai32					SkinIndices		; // one per triangle (VertexIndices.size() / 3)
	};

	struct SModelHierarchyNodeRSM {
		int32_t									IdParent		;
		::gpk::ai32								IdChildren		;
	};

	::gpk::error_t							rsmGeometryGenerate	(const ::gpk::SRSMFileContents& input, ::gpk::view_array<::gpk::SModelNodeRSM>& out_generated);
} // namespace

#endif // GPK_RSM_H_02938402983
