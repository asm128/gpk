#include "gpk_array.h"
#include "gpk_coord.h"
#include "gpk_array_static.h"

#ifndef GPK_RSM_H_02938402983
#define GPK_RSM_H_02938402983

namespace gpk
{
#pragma pack(push, 1)
	struct SRSMFace {
					int16_t													Vertices[3]			;	// Vertex indices?
					int16_t													UVs		[3]			;	// UVs indices?
					int16_t													IndexTextureIndex	;	// texture index index
					int16_t													todo1				;	//
					int32_t													TwoSided			;	//
					int32_t													SmoothGroup			;	//
	};

	struct SRSMNodeTransform {
					::gpk::SCoord3<float>									Row0;
					::gpk::SCoord3<float>									Row1;
					::gpk::SCoord3<float>									Row2;
					::gpk::SCoord3<float>									Offset;
					::gpk::SCoord3<float>									Translation;
					float													Rotation;
					::gpk::SCoord3<float>									RotAxis;
					::gpk::SCoord3<float>									Scale;
	}; //

	struct SRSMFrameRotation {
					int														Time;
					::gpk::SQuaternion<float>								Orientation;
	};

	struct SRSMFramePosition {
					int														Time;
					::gpk::SCoord3<float>									Position;
	};

	struct SRSMTexCoord {
					uint32_t												Unknown;
					::gpk::SCoord2<float>									UV;
	};
#pragma pack(pop)
	struct SRSMNode {
					::gpk::apod<int32_t>								TextureIndices;
					::gpk::apod<::gpk::SCoord3<float>>					Vertices;
					::gpk::apod<::gpk::SRSMTexCoord>					UVs;
					::gpk::apod<::gpk::SRSMFace>						Faces;
					char_t													Name		[40];
					char_t													ParentName	[40];
					::gpk::SRSMNodeTransform								Transform;
					::gpk::apod<SRSMFrameRotation>						RotationKeyframes;
					::gpk::apod<SRSMFramePosition>						PositionKeyframes;
	};

	struct SRSMFileContents {
					uint32_t												AnimLength;
					uint32_t												ShadeType;
					uint8_t													Alpha;
					char_t													Unknown[16];

					::gpk::aobj<::gpk::array_static<char_t, 40>>		TextureNames;

					char_t													RootNodeName[40];
					::gpk::aobj<::gpk::SRSMNode>						Nodes;
	};
				::gpk::error_t											rsmFileLoad								(::gpk::SRSMFileContents& loaded, const ::gpk::view_array<ubyte_t>	& input);
				::gpk::error_t											rsmFileLoad								(::gpk::SRSMFileContents& loaded, FILE								* input);
				::gpk::error_t											rsmFileLoad								(::gpk::SRSMFileContents& loaded, const ::gpk::vcs	& input);

	struct SModelNodeRSM {
					::gpk::apod<::gpk::SCoord3<float>>					Normals			;
					::gpk::apod<::gpk::SCoord3<float>>					Vertices		;
					::gpk::apod<::gpk::SCoord2<float>>					UVs				;
					::gpk::apod<::gpk::STriangle<uint32_t>>		VertexIndices	;
					int32_t													TextureIndex	;
					int32_t													RSMNodeIndex	;
					//::gpk::apod<int32_t>								SkinIndices		; // one per triangle (VertexIndices.size() / 3)
	};

	struct SModelHierarchyNodeRSM {
					int32_t													IdParent		;
					::gpk::apod<int32_t>								IdChildren		;
	};

				::gpk::error_t											rsmGeometryGenerate						(const ::gpk::SRSMFileContents& input, ::gpk::view_array<::gpk::SModelNodeRSM>& out_generated);
} // namespace

#endif // GPK_RSM_H_02938402983
