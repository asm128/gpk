#include "gpk_array.h"
#include "gpk_quat.h"
#include "gpk_array_static.h"
#include "gpk_tri.h"

#ifndef GPK_RSM_H_23627
#define GPK_RSM_H_23627

nmsp gpk
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
		n3f2_t							Row0;
		n3f2_t							Row1;
		n3f2_t							Row2;
		n3f2_t							Offset;
		n3f2_t							Translation;
		f2_t									Rotation;
		n3f2_t							RotAxis;
		n3f2_t							Scale;
	}; //

	struct SRSMFrameRotation {
		int										Time;
		quatf2_t							Orientation;
	};

	struct SRSMFramePosition {
		int										Time;
		n3f2_t							Position;
	};

	struct SRSMTexCoord {
		u2_t								Unknown;
		n2f2_t							UV;
	};
#pragma pack(pop)
	struct SRSMNode {
		as2_t								TextureIndices;
		apod<n3f2_t>				Vertices;
		apod<SRSMTexCoord>		UVs;
		apod<SRSMFace>			Faces;
		sc_t									Name		[40];
		sc_t									ParentName	[40];
		SRSMNodeTransform				Transform;
		apod<SRSMFrameRotation>			RotationKeyframes;
		apod<SRSMFramePosition>			PositionKeyframes;
	};

	struct SRSMFileContents {
		u2_t								AnimLength;
		u2_t								ShadeType;
		u0_t									Alpha;
		sc_t									Unknown[16];
		aobj<astatic<sc_t, 40>>	TextureNames;
		sc_t									RootNodeName[40];
		aobj<SRSMNode>			Nodes;
	};
	gpk::err_t 						rsmFileLoad			(SRSMFileContents & loaded, const vu0_t	& input);
	gpk::err_t 						rsmFileLoad			(SRSMFileContents & loaded, FILE				* input);
	gpk::err_t 						rsmFileLoad			(SRSMFileContents & loaded, const vcst_t	& input);

	struct SModelNodeRSM {
		apod<n3f2_t>				Normals			;
		apod<n3f2_t>				Vertices		;
		apod<n2f2_t>				UVs				;
		apod<triu2_t>				VertexIndices	;
		s2_t									TextureIndex	;
		s2_t									RSMNodeIndex	;
		//as2_t					SkinIndices		; // one per triangle (VertexIndices.size() / 3)
	};

	struct SModelHierarchyNodeRSM {
		s2_t									IdParent		;
		as2_t								IdChildren		;
	};

	gpk::err_t 						rsmGeometryGenerate	(const SRSMFileContents& input, view<SModelNodeRSM>& out_generated);
} // nmsp

#endif // GPK_RSM_H_23627
