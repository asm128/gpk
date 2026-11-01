#include "gpk_array.h"
#include "gpk_quat.h"
#include "gpk_array_static.h"
#include "gpk_tri.h"

#ifndef GPK_RSM_H_23627
#define GPK_RSM_H_23627

namespace gpk
{
#pragma pack(push, 1)
	struct SRSMFace {
		s1_t									Vertices[3]			;	// Vertex indices?
		s1_t									UVs		[3]			;	// UVs indices?
		s1_t									IndexTextureIndex	;	// texture index index
		s1_t									todo1				;	//
		s2_t									TwoSided			;	//
		s2_t									SmoothGroup			;	//
	};

	struct SRSMNodeTransform {
		::gpk::n3f2_t							Row0;
		::gpk::n3f2_t							Row1;
		::gpk::n3f2_t							Row2;
		::gpk::n3f2_t							Offset;
		::gpk::n3f2_t							Translation;
		f2_t									Rotation;
		::gpk::n3f2_t							RotAxis;
		::gpk::n3f2_t							Scale;
	}; //

	struct SRSMFrameRotation {
		int										Time;
		::gpk::quatf2_t							Orientation;
	};

	struct SRSMFramePosition {
		int										Time;
		::gpk::n3f2_t							Position;
	};

	struct SRSMTexCoord {
		u2_t								Unknown;
		::gpk::n2f2_t							UV;
	};
#pragma pack(pop)
	struct SRSMNode {
		::gpk::as2_t								TextureIndices;
		::gpk::apod<::gpk::n3f2_t>				Vertices;
		::gpk::apod<::gpk::SRSMTexCoord>		UVs;
		::gpk::apod<::gpk::SRSMFace>			Faces;
		sc_t									Name		[40];
		sc_t									ParentName	[40];
		::gpk::SRSMNodeTransform				Transform;
		::gpk::apod<SRSMFrameRotation>			RotationKeyframes;
		::gpk::apod<SRSMFramePosition>			PositionKeyframes;
	};

	struct SRSMFileContents {
		u2_t								AnimLength;
		u2_t								ShadeType;
		u0_t									Alpha;
		sc_t									Unknown[16];
		::gpk::aobj<::gpk::astatic<sc_t, 40>>	TextureNames;
		sc_t									RootNodeName[40];
		::gpk::aobj<::gpk::SRSMNode>			Nodes;
	};
	gpk::err_t 						rsmFileLoad			(::gpk::SRSMFileContents & loaded, const ::gpk::vu0_t	& input);
	gpk::err_t 						rsmFileLoad			(::gpk::SRSMFileContents & loaded, FILE				* input);
	gpk::err_t 						rsmFileLoad			(::gpk::SRSMFileContents & loaded, const ::gpk::vcst_t	& input);

	struct SModelNodeRSM {
		::gpk::apod<::gpk::n3f2_t>				Normals			;
		::gpk::apod<::gpk::n3f2_t>				Vertices		;
		::gpk::apod<::gpk::n2f2_t>				UVs				;
		::gpk::apod<::gpk::triu2_t>				VertexIndices	;
		s2_t									TextureIndex	;
		s2_t									RSMNodeIndex	;
		//::gpk::as2_t					SkinIndices		; // one per triangle (VertexIndices.size() / 3)
	};

	struct SModelHierarchyNodeRSM {
		s2_t									IdParent		;
		::gpk::as2_t								IdChildren		;
	};

	gpk::err_t 						rsmGeometryGenerate	(const ::gpk::SRSMFileContents& input, ::gpk::view<::gpk::SModelNodeRSM>& out_generated);
} // namespace

#endif // GPK_RSM_H_23627
