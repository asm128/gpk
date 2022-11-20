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
					::gpk::array_pod<int32_t>								TextureIndices;
					::gpk::array_pod<::gpk::SCoord3<float>>					Vertices;
					::gpk::array_pod<::gpk::SRSMTexCoord>					UVs;
					::gpk::array_pod<::gpk::SRSMFace>						Faces;
					char_t													Name		[40];
					char_t													ParentName	[40];
					::gpk::SRSMNodeTransform								Transform;
					::gpk::array_pod<SRSMFrameRotation>						RotationKeyframes;
					::gpk::array_pod<SRSMFramePosition>						PositionKeyframes;
	};

	struct SRSMFileContents {
					uint32_t												AnimLength;
					uint32_t												ShadeType;
					uint8_t													Alpha;
					char_t													Unknown[16];

					::gpk::array_obj<::gpk::array_static<char_t, 40>>		TextureNames;

					char_t													RootNodeName[40];
					::gpk::array_obj<::gpk::SRSMNode>						Nodes;
	};
				::gpk::error_t											rsmFileLoad								(::gpk::SRSMFileContents& loaded, const ::gpk::view_array<ubyte_t>	& input);
				::gpk::error_t											rsmFileLoad								(::gpk::SRSMFileContents& loaded, FILE								* input);
				::gpk::error_t											rsmFileLoad								(::gpk::SRSMFileContents& loaded, const ::gpk::view_const_string	& input);

	struct SModelNodeRSM {
					::gpk::array_pod<::gpk::SCoord3<float>>					Normals			;
					::gpk::array_pod<::gpk::SCoord3<float>>					Vertices		;
					::gpk::array_pod<::gpk::SCoord2<float>>					UVs				;
					::gpk::array_pod<::gpk::STriangle<uint32_t>>		VertexIndices	;
					int32_t													TextureIndex	;
					int32_t													RSMNodeIndex	;
					//::gpk::array_pod<int32_t>								SkinIndices		; // one per triangle (VertexIndices.size() / 3)
	};

	struct SModelHierarchyNodeRSM {
					int32_t													IdParent		;
					::gpk::array_pod<int32_t>								IdChildren		;
	};

				::gpk::error_t											rsmGeometryGenerate						(const ::gpk::SRSMFileContents& input, ::gpk::view_array<::gpk::SModelNodeRSM>& out_generated);
} // namespace

#endif // GPK_RSM_H_02938402983
